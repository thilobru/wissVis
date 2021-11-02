#pragma once

#include <iosfwd>

#include "Base.hpp"

namespace fantom
{
    /// Base class for events that are passed between algorithms.
    class Event
    {
    public:
        enum class Action
        {
            Noop,
            Rerun
        };

        virtual ~Event();
    };

    namespace detail_inputevent
    {
        struct any_t
        {
            template < typename T >
            any_t( const T& );
            template < typename T >
            any_t( T& );
        };

        std::false_type operator<<( std::ostream&, any_t );
        std::false_type operator>>( std::istream&, any_t );

        template < typename State >
        struct CheckInputState
        {
        private:
            template < typename IState >
            static auto testProcess( int ) -> fantom::detail::sfinae_true< decltype(
                std::declval< IState >().processEvent( std::declval< Event >() ) ) >;

            template < typename IState >
            static auto testProcess( long ) -> std::false_type;

            using TheState = typename std::decay< State >::type;

        public:
            static const bool isSerializable
                = !std::is_same< decltype( std::declval< std::ostream& >() << std::declval< const TheState& >() ),
                                 std::false_type >::value;
            static const bool isDeserializable
                = !std::is_same< decltype( std::declval< std::istream& >() >> std::declval< TheState& >() ),
                                 std::false_type >::value;
            static const bool hasProcessEvent
                = std::is_base_of< std::true_type, decltype( testProcess< TheState >( 0 ) ) >::value;
            static const bool typeIsOk = isSerializable && isDeserializable && hasProcessEvent
                                         && std::is_copy_constructible< TheState >::value;

            static_assert( isSerializable,
                           "The State type must be serializable into a std::ostream. Please overload operator<<!" );
            static_assert( isDeserializable,
                           "The State type must be deserializable from a std::istream. Please overload operator>>!" );
            static_assert( std::is_copy_constructible< TheState >::value,
                           "The State type must be copy constructible!" );
            static_assert( hasProcessEvent,
                           "The State type must contain a member function processEvent(const Event&)!" );
        };
    }

    /// Event based input for interactions.
    /**
     * Events are connected here and processed by the processEvent method.
     * Results of the event processing are stored in a state object.
     * Thereby, the state type must satisfy the following requirements:
     *   - it must be serializable, so that the state that is created by interactions can be stored to the session file.
     *   - The type must be able to be assigned to itself, so that the state can be copied into the executing thread of
     *     the algorithm.
     *   - The type must contain a member function processEvent(const Event&) to act on incoming events.
     */
    class InputEvent : public InputBase
    {
        typedef RTTI_Info< InputEvent, InputBase > TypeInfo;
        TYPE_INFORMATION( "InputEvent" )

    public:
        template < typename State >
        InputEvent( const std::string& name, const std::string& info, InputChangeNotifier& notifier, State&& state )
            : InputBase( name, info, notifier )
        {
            static_assert( detail_inputevent::CheckInputState< typename std::decay< State >::type >::typeIsOk,
                           "State type does not satisfy the requirements!" );

            mDeserialize = []( std::istream& str, void* data ) {
                str >> *static_cast< typename std::decay< State >::type* >( data );
            };
            mSerialize = []( std::ostream& str, const void* data ) {
                str << *static_cast< const typename std::decay< State >::type* >( data );
            };
            mCopy = []( const void* data ) {
                return std::make_shared< typename std::decay< State >::type >( *static_cast< const State* >( data ) );
            };
            mProcess = []( const Event& e, void* data ) {
                return static_cast< typename std::decay< State >::type* >( data )->processEvent( e );
            };

            mData = std::make_shared< typename std::decay< State >::type >( std::forward< State >( state ) );
        }

        Event::Action processEvent( const Event& event )
        {
            return mProcess( event, mData.get() );
        }

        template < typename State >
        State& access()
        {
            return *static_cast< State* >( mData.get() );
        }

        template < typename State >
        const State& access() const
        {
            return *static_cast< const State* >( mData.get() );
        }

        void serialize( std::ostream& );
        void deserialize( std::istream& );

    private:
        void assignFrom( const InputBase& rhs ) override;

        std::function< void( std::istream&, void* ) > mDeserialize;
        std::function< void( std::ostream&, const void* ) > mSerialize;
        std::function< std::shared_ptr< void >( const void* ) > mCopy;
        std::function< Event::Action( const Event&, void* ) > mProcess;
        std::shared_ptr< void > mData;
    };
}
