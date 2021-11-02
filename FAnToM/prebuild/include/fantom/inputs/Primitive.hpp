#pragma once

#include "Base.hpp"

namespace fantom
{
    /// base class for all c++ primitive types (integral and floating point)
    /// \internal
    class InputArithmetic : public InputBase
    {
        typedef RTTI_Info< InputArithmetic, InputBase > TypeInfo;
        TYPE_INFORMATION( "InputPrimitive" )

    public:
        /// constructs the input object
        /**
         * \param name Name of the input
         * \param info Info description
         * \param notifier Change Notifier
         */
        InputArithmetic( const std::string& name, const std::string& info, InputChangeNotifier& notifier );

        virtual void fromString( const std::string& str ) = 0;
        virtual std::string toString() const = 0;

        template < typename T >
        struct ArithmeticBaseType
        {
        private:
            // default types
            using boolType = bool;
            using floatType = double;
            using integralType = long;
            using unsignedIntegralType = unsigned long;

            // remove const and references etc.
            using TBase = typename std::decay< T >::type;

            // assert validity
            static_assert( std::is_arithmetic< TBase >::value, "Type must be an arithmetic type!" );

            // determine correct type
            static constexpr bool isFloat = std::is_floating_point< TBase >::value;
            static constexpr bool isBool = std::is_same< bool, TBase >::value;
            static constexpr bool isUnsigned = std::is_unsigned< TBase >::value;

        public:
            using type = typename std::
                conditional< isFloat,
                             floatType,
                             typename std::conditional< isBool,
                                                        boolType,
                                                        typename std::conditional< isUnsigned,
                                                                                   unsignedIntegralType,
                                                                                   integralType >::type >::type >::type;
        };
    };

    /// base class for primitive c++ type inputs
    /**
     * \internal
     * \tparam T type of the data represented by this input field
     * Subclasses must not redeclare FilterType or ValueType and have to follow the
     * constructor conventions documented at the constructor of this class.
     */
    template < typename T >
    class InputArithmeticImpl : public InputArithmetic
    {
        typedef RTTI_Info< InputArithmeticImpl< T >, InputArithmetic > TypeInfo;
        TYPE_INFORMATION( "InputPrimitiveImpl" )

    public:
        /// type of the acceptor function
        using FilterType = std::function< T( const T& ) >;
        using ValueType = T;
        using StorageType = typename InputArithmetic::ArithmeticBaseType< T >::type;

        /// constructs the input object
        /**
         * \param name Name of the input
         * \param info Info description
         * \param notifier Change Notifier
         * \param value reference to default value
         * \param filter an acceptor callback. On rejection the callback
         *        must either return a valid value or throw an exception (strong).
         */
        InputArithmeticImpl( const std::string& name,
                             const std::string& info,
                             InputChangeNotifier& notifier,
                             ValueType value,
                             const FilterType& filter = &InputBase::ignore< ValueType > )
            : InputArithmetic( name, info, notifier )
            , mValue( value )
            , mFilter( filter )
        {
        }

        /// sets new value
        ValueType set( const ValueType& value )
        {
            ValueType newValue = mFilter( value );
            if( newValue != mValue )
            {
                mValue = newValue;
                mNotifier.setContentChanged( *this );
            }
            return mValue;
        }

        /// gives the value of this input
        const ValueType get() const
        {
            return mValue;
        }

        /// checks whether a given value would be valid for this input
        ValueType check( const ValueType& value ) const
        {
            return mFilter( value );
        }

        virtual void fromString( const std::string& str ) override
        {
            set( lexical_cast< T >( str ) );
        }

        virtual std::string toString() const override
        {
            return lexical_cast< std::string >( get() );
        }

    private:
        void assignFrom( const InputBase& rhs ) override
        {
            auto& input = static_cast< const InputArithmeticImpl< T >& >( rhs );
            mValue = input.mValue;
        }

        StorageType mValue;
        FilterType mFilter;
    };

    //------------------------------------- InputObject ---------------------------------------

    /// base class for input fields of complex classes with value semantics
    /**
     * \ingroup options
     * \internal
     * Subclasses must declare FilterType and ValueType and have to follow the
     * constructor conventions documented at the constructor of this class.
     */
    class InputObject : public InputBase
    {
        typedef RTTI_Info< InputObject, InputBase > TypeInfo;
        TYPE_INFORMATION( "InputObject" )

    public:
        /// constructs the input object
        /**
         * \param name Name of the input
         * \param info Info description
         * \param notifier Change Notifier
         */
        InputObject( const std::string& name, const std::string& info, InputChangeNotifier& notifier );
    };

    /// implementation of input fields of class type
    /**
     * \ingroup options
     * \internal
     * \tparam DataType type of the data represented by this input field
     * Subclasses must not redeclare FilterType or ValueType and have to follow the
     * constructor conventions documented at the constructor of this class.
     */
    template < class DataType >
    class Input : public InputObject
    {
        typedef RTTI_Info< Input< DataType >, InputObject > TypeInfo;
        TYPE_INFORMATION( "Input" )

    public:
        /// type of the acceptor function
        typedef std::function< DataType( const DataType& ) > FilterType;
        typedef DataType ValueType;

        /// constructs the input object
        /**
         * \param name Name of the input
         * \param info Info description
         * \param notifier Change Notifier
         * \param value reference to default value
         * \param filter an acceptor callback. On rejection the callback
         *        must either return a valid value or throw an exception (strong).
         */
        Input( const std::string& name,
               const std::string& info,
               InputChangeNotifier& notifier,
               const ValueType& value,
               const FilterType& filter = &InputBase::ignore< ValueType > )
            : InputObject( name, info, notifier )
            , mValue( value )
            , mFilter( filter )
        {
        }

        /// sets new value
        ValueType set( const ValueType& value )
        {
            ValueType newValue = mFilter( value );
            if( newValue != mValue )
            {
                mValue = newValue;
                mNotifier.setContentChanged( *this );
            }
            return mValue;
        }

        /// gives the value of this input
        const ValueType& get() const
        {
            return mValue;
        }

        /// checks whether a given value would be valid for this input
        ValueType check( const ValueType& value ) const
        {
            return mFilter( value );
        }

    protected:
        void assignFrom( const InputBase& rhs ) override
        {
            auto& input = static_cast< const Input< DataType >& >( rhs );
            mValue = input.mValue;
        }

    private:
        ValueType mValue;
        FilterType mFilter;
    };

    /// specialized input for representing labels
    /**
     * \ingroup options
     */
    class InputLabel : public Input< std::string >
    {
        typedef RTTI_Info< InputLabel, Input< std::string > > TypeInfo;
        TYPE_INFORMATION( "Label" )

    public:
        InputLabel( const std::string& name,
                    const std::string& info,
                    InputChangeNotifier& notifier,
                    const std::string& content );
    };

    /// specialized input for representing choices
    /**
     * \ingroup options
     */
    class InputChoices : public Input< std::string >
    {
        typedef RTTI_Info< InputChoices, Input< std::string > > TypeInfo;
        TYPE_INFORMATION( "Choices" )

    public:
        InputChoices( const std::string& name,
                      const std::string& info,
                      InputChangeNotifier& notifier,
                      const std::vector< std::string >& choices,
                      const ValueType& value,
                      const FilterType& filter = &InputBase::ignore< std::string > );

        virtual const std::vector< std::string >& getChoices() const;
        virtual void setChoices( std::vector< std::string > choices );

    private:
        std::vector< std::string > mChoices;
    };

    /// specialized input for representing multiple choices
    /// The data value is a set of the selected indices.
    /**
     * \ingroup options
     */
    class InputMultiChoice : public Input< std::set< size_t > >
    {
        typedef RTTI_Info< InputMultiChoice, Input< std::set< size_t > > > TypeInfo;
        TYPE_INFORMATION( "Multi Choices" )

    public:
        InputMultiChoice( const std::string& name,
                          const std::string& info,
                          InputChangeNotifier& notifier,
                          const std::vector< std::string >& choices );

        virtual const std::vector< std::string >& getChoices() const;
        virtual void setChoices( std::vector< std::string > choices );

    private:
        std::vector< std::string > mChoices;
    };


    /// specialized input for representing seed values for random variables
    /**
     * \ingroup options
     */
    class InputSeed : public InputArithmeticImpl< unsigned int >
    {
        typedef RTTI_Info< InputSeed, InputArithmeticImpl< unsigned int > > TypeInfo;
        TYPE_INFORMATION( "Seed" )

    public:
        InputSeed( const std::string& name,
                   const std::string& info,
                   InputChangeNotifier& notifier,
                   const ValueType& value = 0 );
    };

    /// Specialized input for representing Sliders in the options dialog.
    /**
     * Thereby, an interval [0, n] is presented to the user and a slider is displayed to
     * choose a value therein.
     * \ingroup options
     */
    class InputSlider : public InputArithmeticImpl< size_t >
    {
        typedef RTTI_Info< InputSlider, InputArithmeticImpl< size_t > > TypeInfo;
        TYPE_INFORMATION( "Slider" )

    public:
        /// constructs a new Slider Input Option
        /**
         * @param name option name to show in the GUI
         * @param info info string
         * \param notifier Change Notifier
         * @param max the maximum value the slider will have
         * @param start initial value for the slider
         */
        InputSlider( const std::string& name,
                     const std::string& info,
                     InputChangeNotifier& notifier,
                     size_t max,
                     size_t start = 0 );


        /// returns the number of ticks
        /**
         * @return number of ticks
         */
        size_t getMax() const
        {
            return mMax;
        }

        void setMax( size_t max );

    private:
        size_t mMax;
    };

    enum class LoadSaveType
    {
        File,
        Dir
    };

    /// specialized input for representing load paths
    /**
     * The default is to select a file for loading, not a directory.
     * \ingroup options
     */
    class InputLoadPath : public Input< std::string >
    {
        typedef RTTI_Info< InputLoadPath, Input< std::string > > TypeInfo;
        TYPE_INFORMATION( "Load Path" )

    public:
        InputLoadPath( const std::string& name,
                       const std::string& info,
                       InputChangeNotifier& notifier,
                       const ValueType& value,
                       const FilterType& filter = &InputBase::ignore< std::string > );

        InputLoadPath( const std::string& name,
                       const std::string& info,
                       InputChangeNotifier& notifier,
                       const ValueType& value,
                       LoadSaveType loadType,
                       const FilterType& filter = &InputBase::ignore< std::string > );

        LoadSaveType getType() const;

    private:
        LoadSaveType mType;
    };

    /// specialized input for representing save paths
    /**
     * The default is to select a file for saving, not a directory.
     * \ingroup options
     */
    class InputSavePath : public Input< std::string >
    {
        typedef RTTI_Info< InputSavePath, Input< std::string > > TypeInfo;
        TYPE_INFORMATION( "Save Path" )

    public:
        InputSavePath( const std::string& name,
                       const std::string& info,
                       InputChangeNotifier& notifier,
                       const ValueType& value,
                       const FilterType& filter = &InputBase::ignore< std::string > );

        InputSavePath( const std::string& name,
                       const std::string& info,
                       InputChangeNotifier& notifier,
                       const ValueType& value,
                       LoadSaveType saveType,
                       const FilterType& filter = &InputBase::ignore< std::string > );

        LoadSaveType getType() const;

    private:
        LoadSaveType mType;
    };
}
