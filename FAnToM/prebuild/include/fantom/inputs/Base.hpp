#pragma once

#include <functional>
#include <memory>

#include "../exceptions.hpp"
#include "../rtti.hpp"

namespace fantom
{
    class InputBase;

    /// Interface for notifications about changed inputs
    struct InputChangeNotifier
    {
        virtual ~InputChangeNotifier()
        {
        }

        /// Called, when an input changed its value
        virtual void setContentChanged( const InputBase& input ) = 0;

        /// Called, when an input changed its definition
        virtual void setDefinitionChanged( const InputBase& input ) = 0;

        /// Return, whether the input has changed (either directly or because the data object changed)
        virtual bool hasContentChanged( const InputBase& input ) const = 0;
    };

    /// base class of all input objects.
    /**
     * \ingroup options
     * holds only the info string.
     */
    class InputBase : public RTTI
    {
        typedef RTTI_Info< InputBase > TypeInfo;
        TYPE_INFORMATION( "InputBase" )

    public:
        InputBase( const std::string& name, const std::string& info, InputChangeNotifier& notifier );
        virtual ~InputBase()
        {
        }

        /// Gives the name of the input
        const std::string& getName() const;

        /// Gives a reference to the info description
        const std::string& getInfo() const;

        /// Assigns the value of the rhs to this input.
        void assign( const InputBase& rhs );

        /// Helper function that accepts any value.
        template < class T >
        static T ignore( const T& value )
        {
            return value;
        }

        /// Helper function that accepts values greater than or equal to min.
        template < class T >
        static T checkMin( const T& value, const T& min, bool exception = false )
        {
            if( value < min )
            {
                if( exception )
                {
                    throw fantom::runtime_error( "Value must be at least " + lexical_cast< std::string >( min ) + "." );
                }
                return min;
            }
            else
            {
                return value;
            }
        }

        /// Helper function that accepts values less than or equal to max.
        template < class T >
        static T checkMax( const T& value, const T& max, bool exception = false )
        {
            if( value > max )
            {
                if( exception )
                {
                    throw fantom::runtime_error( "Value must be at most " + lexical_cast< std::string >( max ) + "." );
                }
                return max;
            }
            else
            {
                return value;
            }
        }

        /// Helper function that accepts values between min and max (including min and max)
        template < class T >
        static T checkBetween( const T& value, const T& min, const T& max, bool exception = false )
        {
            if( value < min )
            {
                if( exception )
                {
                    throw fantom::runtime_error( "Value must be at least " + lexical_cast< std::string >( min ) + "." );
                }
                return min;
            }
            else if( value > max )
            {
                if( exception )
                {
                    throw fantom::runtime_error( "Value must be at most " + lexical_cast< std::string >( max ) + "." );
                }
                return max;
            }
            else
            {
                return value;
            }
        }

        template < class T >
        static std::function< T( const T& value ) > makeCheckMin( const T& min, bool exception = false )
        {
            return
                [min, exception]( const T& value ) { return checkMin< decltype( value ) >( value, min, exception ); };
        }

        template < class T >
        static std::function< T( const T& value ) > makeCheckMax( const T& max, bool exception = false )
        {
            return
                [max, exception]( const T& value ) { return checkMax< decltype( value ) >( value, max, exception ); };
        }

        template < class T >
        static std::function< T( const T& value ) >
        makeCheckBetween( const T& min, const T& max, bool exception = false )
        {
            return [min, max, exception]( const T& value ) {
                return checkBetween< decltype( value ) >( value, min, max, exception );
            };
        }

    protected:
        /**
         * \brief Assign the value of \c rhs to this input.
         *
         * The type of \c rhs has already been confirmed.
         * Therefore, static_cast can be used without concern.
         */
        virtual void assignFrom( const InputBase& rhs ) = 0;

        InputChangeNotifier& mNotifier;

    private:
        std::string mName;
        std::string mInfo;
    };


    //===========================================================================================================


    /// Represents a special options definition event like a tab, a separator, a group or the end of a group.
    /// The name is always empty (except for Group, where it equals the info string) and the info string may contain the
    /// caption.
    /// \internal
    struct InputSpecial : public InputBase
    {
        enum Type
        {
            Separator,
            Tab,
            Group,
            EndGroup,
            Button
        };
        InputSpecial( Type type, const std::string& info, InputChangeNotifier& notifier );

        Type getType() const;

    private:
        void assignFrom( const InputBase& ) override
        {
        }

        Type mType;
    };
}
