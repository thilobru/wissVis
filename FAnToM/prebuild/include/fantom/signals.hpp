#pragma once

#include <functional>
#include <list>
#include <set>
#include <type_traits>

#include "impl/runtime.hpp"

namespace fantom
{
    class Observer;
    class Observable;
    template < typename... Args >
    class SignalBase;

    namespace fantom_private
    {

        class ConnectionBase
        {
        public:
            ConnectionBase( Observable& observable, Observer& observer );
            virtual ~ConnectionBase();

            void removeFromObserver();
            void removeFromObservable();
            Observer& getObserver();

        private:
            Observable& mObservable;
            Observer& mObserver;
        };

        template < typename... Args >
        class Connection : public ConnectionBase
        {
        public:
            typedef std::function< void( Args... ) > Caller;

            Connection( Observable& observable, Observer& observer, size_t signalNumber, Caller callFunc )
                : ConnectionBase( observable, observer )
                , mSignalNumber( signalNumber )
                , mCallFunc( callFunc )
            {
            }

            void call( Args... args ) const
            {
                mCallFunc( args... );
            }
            size_t signalNumber() const
            {
                return mSignalNumber;
            }

        private:
            size_t mSignalNumber;
            Caller mCallFunc;
        };

        template < typename... Args >
        struct List;
        template < typename Arg1, typename... Args >
        struct List< Arg1, Args... >
        {
            typedef Arg1 First;
            typedef List< Args... > Rest;
            template < template < typename > class Functor >
            using map = List< typename Functor< Arg1 >::type, typename Functor< Args >::type... >;
        };
        template <>
        struct List<>
        {
            typedef List<> Rest;
            typedef void First;
            template < template < typename > class Functor >
            using map = List<>;
        };

        template < typename T >
        struct MapToRef
        {
            using type = const T&;
        };
        template < typename T >
        struct MapToRef< T& >
        {
            using type = T&;
        };

        template < typename Func >
        struct MemberFunctionPointer;
        template < class Obj, typename... Args >
        struct MemberFunctionPointer< void ( Obj::* )( Args... ) >
        {
            typedef Obj ObjectType;
            typedef void ( Obj::*Function )( Args... );
        };

        //------------------ perfect forwarding of arguments for signal implementation ------------------------

        template < typename T >
        T&& forward( typename std::remove_reference< T >::type& t,
                     typename std::enable_if< !std::is_reference< T >::value, void* >::type = 0 )
        {
            return std::forward< T >( t );
        }

        template < typename T >
        T&& forward( typename std::remove_reference< T >::type&& t,
                     typename std::enable_if< !std::is_reference< T >::value, void* >::type = 0 )
        {
            return std::forward< T >( t );
        }

        template < typename T >
        std::reference_wrapper< typename std::remove_reference< T >::type >
        forward( T t,
                 typename std::enable_if< std::is_reference< T >::value
                                              && !std::is_const< typename std::remove_reference< T >::type >::value,
                                          void* >::type
                 = 0 )
        {
            return std::ref( t );
        }

        template < typename T >
        std::reference_wrapper< typename std::remove_reference< T >::type >
        forward( T t,
                 typename std::enable_if< std::is_reference< T >::value
                                              && std::is_const< typename std::remove_reference< T >::type >::value,
                                          void* >::type
                 = 0 )
        {
            return std::cref( t );
        }
    }

    /**
     * Observable class that can send signals.
     * On destruction, connections are automatically removed.
     * Construction and destruction has to be performed in GUI/Kernel-thread to prevent race conditions.
     *
     * NOTE: the naming convention for signals is "signal[signal name]".
     */
    class Observable
    {
    public:
        virtual ~Observable();

        /**
         * Method to connect am observable to an observer.
         * The member functions identified by \a signal and \a slot
         * need to have matching signatures.
         * Connections have to be established from the GUI/Kernel-thread to prevent race conditions.
         */
        template < typename SignalType, typename Func >
        inline void connect( const SignalType& signal,
                             typename fantom_private::MemberFunctionPointer< Func >::ObjectType& observer,
                             Func slot ) const
        {
            static_assert(
                std::is_base_of< Observer, typename fantom_private::MemberFunctionPointer< Func >::ObjectType >::value,
                "observer must be derived from fantom::Observer" );

            new typename SignalType::ConnectionType( *const_cast< Observable* >( this ),
                                                     observer,
                                                     signal.signalNumber(),
                                                     SignalType::makeCaller( observer, slot ) );
        }

    private:
        void addConnection( fantom_private::ConnectionBase* con );
        void removeConnection( fantom_private::ConnectionBase* con );
        void destruct();
        size_t nextSignalNumber();

        std::list< fantom_private::ConnectionBase* > mConnections;

        friend class fantom_private::ConnectionBase;
        template < typename... Args >
        friend class SignalBase;
    };

    template < typename... Args >
    class SignalBase
    {
        friend class Observable;
        using ConnectionType = fantom_private::Connection< typename fantom_private::MapToRef< Args >::type... >;
        template < class Obj, typename... FuncArgs >
        static std::function< void( typename fantom_private::MapToRef< Args >::type... ) >
        makeCaller( Obj& obj, void ( Obj::*func )( FuncArgs... ) )
        {
            Obj* objPtr = &obj;
            return [objPtr, func]( typename fantom_private::MapToRef< Args >::type... args ) {
                ( objPtr->*func )( args... );
            };
        }

    public:
        SignalBase( Observable& parent, bool synchronous )
            : mParent( parent )
            , mSignalNumber( mParent.nextSignalNumber() )
            , mSynchronous( synchronous )
        {
        }

        virtual ~SignalBase()
        {
        }

        void operator()( Args... args ) const
        {
            if( mSynchronous )
                Runtime::instance().runSync(
                    std::bind( &SignalBase::callConnectedSlots, this, fantom_private::forward< Args >( args )... ) );
            else
                Runtime::instance().runAsync(
                    std::bind( &SignalBase::callConnectedSlots, this, fantom_private::forward< Args >( args )... ) );
        }

        // convenience function for connecting
        template < typename Func >
        inline void connect( typename fantom_private::MemberFunctionPointer< Func >::ObjectType& observer,
                             Func slot ) const
        {
            mParent.connect( *this, observer, slot );
        }

        size_t signalNumber() const
        {
            return mSignalNumber;
        }

    private:
        /// called in the kernel thread
        void callConnectedSlots( typename fantom_private::MapToRef< Args >::type... args ) const
        {
            for( auto con : mParent.mConnections )
            {
                auto conP = dynamic_cast< ConnectionType* >( con );
                if( conP && conP->signalNumber() == mSignalNumber )
                {
                    conP->call( args... );
                }
            }
        }

        Observable& mParent;
        size_t mSignalNumber;
        bool mSynchronous;
    };

    /// Synchronised signal. A call to this signal only returns after all connected slots have been processed.
    template < typename... Args >
    struct SyncSignal : public SignalBase< Args... >
    {
        SyncSignal( Observable& parent )
            : SignalBase< Args... >( parent, true )
        {
        }
    };

    /// Asynchronised signal. A call to this signal schedules all connected slots for execution and immediately returns.
    template < typename... Args >
    struct AsyncSignal : public SignalBase< Args... >
    {
        AsyncSignal( Observable& parent )
            : SignalBase< Args... >( parent, false )
        {
        }
    };

    /**
     * Observer class that can watch Observables.
     * Regardless of the thread, the observable runs in, the observer is
     * always notified in the GUI/Kernel-thread.
     * On destruction, connections are automatically removed.
     * Construction and destruction has to be performed in GUI/Kernel-thread to prevent race conditions.
     *
     * NOTE: the naming convention for all slots is "slot[slot name]".
     */
    class Observer
    {
    public:
        virtual ~Observer();

    private:
        void addConnection( fantom_private::ConnectionBase* con );
        void removeConnection( fantom_private::ConnectionBase* con );
        void destruct();

        std::set< fantom_private::ConnectionBase* > mConnections;

        friend class fantom_private::ConnectionBase;
    };
}

#if 0

//========================= Usage example ========================================
// Note: by convention, signals should always be named "signal[signal name]"
// and slots should always be named "slot[slot name]". That helps to recognize
// event driven portions in the code.

#include <fantom/gui.hpp>
#include <fantom/impl/GuiImpl.hpp>
#include <memory>

extern std::unique_ptr<fantom::Runtime> makeRuntimeQt(int&, char**);

int main(int argc, char** argv) {

    class Test : public fantom::Observable
    {
    public:
        Test()
            : test(*this)
        {}
        AsyncSignal<std::string /*h*/, int /*i*/> signalTest;

    };
    class Test2 : public fantom::Observer
    {
    public:
        void slotTest(std::string h, int i)
        {
            ;
        }
    };

    std::unique_ptr<fantom::Runtime> appl(makeRuntimeQt(argc, argv));
    Test t;
    Test2 t2;

    t.connect(t.signalTest, t2, &Test2::slotTest);
    // or even
    t.signalTest.connect(t2, &Test2::slotTest);
}

#endif
