#pragma once

#include <iostream>

#include "options.hpp"
#include "register.hpp"
#include "rtti.hpp"

#include <pybind11/pybind11.h>

namespace py = pybind11;


namespace fantom
{
    //======================================== PYTHON CONVERTERS ===========================================

    class __attribute__( ( visibility( "default" ) ) ) PythonConverter
    {
    public:
        PythonConverter( const RTTI::TypeId& inputType,
                         std::function< void( InputBase&, const py::object& ) > fromPython,
                         std::function< std::string( const InputBase& ) > toPython );

        virtual ~PythonConverter() {}

        void setInputFromPython( InputBase& input, const py::object& obj );

        std::string getPythonFromInput( const InputBase& input );

        const RTTI::TypeId& getInputType();

    private:
        const RTTI::TypeId& mType;
        std::function< void( InputBase&, const py::object& ) > mFromPython;
        std::function< std::string( const InputBase& ) > mToPython;
    };

    class __attribute__( ( visibility( "default" ) ) ) PythonConverterRegistrationService
    {
    private:
        std::unordered_map< const RTTI::TypeId*, std::shared_ptr< PythonConverter > > mFactories;
        std::shared_ptr< PythonConverter > getConverterFor( const InputBase& input );

    public:
        static PythonConverterRegistrationService& getInstance();

        void registerConverter( const std::shared_ptr< PythonConverter >& reg );
        void unregisterConverter( const std::shared_ptr< PythonConverter >& reg );

        void setInputFromPython( InputBase& input, const py::object& obj );
        std::string getPythonFromInput( const InputBase& input );
    };

    /// \cond
    // ------- template magic to check whether a type has operator>>
    template < class... >
    struct voider
    {
        using type = void;
    };
    template < typename... T >
    using void_t = typename voider< T... >::type;
    template < class InputType, typename T = void >
    struct hasRightShiftOperatorTest : std::false_type
    {
    };
    template < class InputType >
    struct hasRightShiftOperatorTest<
        InputType,
        void_t< decltype( std::declval< std::istream& >() >> std::declval< typename InputType::ValueType& >() ) > >
        : std::true_type
    {
    };
    template < class InputType >
    struct hasRightShiftOperator
    {
        static const bool value = std::is_base_of< std::true_type, hasRightShiftOperatorTest< InputType > >::value;
    };
    // -------- default conversion from string for types that support it
    template < class InputType, bool stringConversion = hasRightShiftOperator< InputType >::value >
    struct TryStringConversion
    {
        static void convertViaString( InputBase& input, const py::object& obj )
        {
            try
            {
                std::string conv = obj.cast< std::string >();
                typename InputType::ValueType value;
                std::istringstream str( conv );
                str >> value;
                static_cast< InputType& >( input ).set( value );
            }
            catch( py::cast_error& cast )
            {
                throw std::runtime_error( "Python object has wrong type for input \"" + input.getName() + "\"!\n"
                                          + cast.what() );
            }
        }
    };
    // -------- default conversion from string for types that don't support it
    template < class InputType >
    struct TryStringConversion< InputType, false >
    {
        static void convertViaString( InputBase& input, const py::object& )
        {
            throw std::runtime_error( "Python object has wrong type for input \"" + input.getName() + "\"!" );
        }
    };
    /// \endcond

    /// default conversion from python to input
    template < class InputType >
    void setInputFromPython( InputBase& input, const py::object& obj )
    {
        try
        {
            typename InputType::ValueType conv = obj.cast< typename InputType::ValueType >();
            static_cast< InputType& >( input ).set( conv );
        }
        catch( py::cast_error& cast )
        {
            TryStringConversion< InputType >::convertViaString( input, obj );
        }
    }

    /// takes a non-escaped string that is not wrapped in quotation marks and escapes and quotes it
    std::string escapeAndQuoteForPython( const std::string& str );

    /// default conversion from input into string
    template < class InputType >
    std::string defaultConversion( const InputType& input )
    {
        if( std::is_same< bool, typename InputType::ValueType >::value )
        {
            return input.get() != typename InputType::ValueType() ? "True" : "False";
        }
        else if( std::is_arithmetic< typename InputType::ValueType >::value )
        {
            return lexical_cast< std::string >( input.get() );
        }
        else
        {
            return escapeAndQuoteForPython( lexical_cast< std::string >( input.get() ) );
        }
    }

    /// registers a python converter for an Input \ingroup plugins
    /**
     * Static variables of this class are used to automatically
     * register python converters. Example:
     * \code
     * PythonConverterRegister<MyInputType> dummy();
     * \endcode
     * After registration, the converter type will be used to interpret python scripts.
     *
     * \tparam InputType the type of Input to register. It is required to
     *       be a valid subtype of InputBase.
     */
    template < class InputType >
    class PythonConverterRegister
    {
        static_assert( std::is_base_of< InputBase, InputType >::value, "InputType must be derived from InputBase!" );

    private:
        std::shared_ptr< PythonConverter > ptr;

        static std::string getPythonFromInput( const InputBase& input,
                                               std::function< std::string( const InputType& ) > toPythonFunc )
        {
            return toPythonFunc( static_cast< const InputType& >( input ) );
        }

    public:
        /// Register InputUI in the system.
        /**
         * If this converter type is already registered a warning is printed to std::err.
         */
        PythonConverterRegister( std::function< std::string( const InputType& ) > toPython
                                 = defaultConversion< InputType >,
                                 std::function< void( InputBase&, const py::object& ) > fromPython
                                 = setInputFromPython< InputType > )
        {
            ptr.reset(
                new PythonConverter( RTTI::typeId< InputType >(), fromPython, [toPython]( const InputBase& input ) {
                    return PythonConverterRegister< InputType >::getPythonFromInput( input, toPython );
                } ) );

            try
            {
                PythonConverterRegistrationService::getInstance().registerConverter( ptr );
            }
            catch( std::runtime_error& e )
            {
                std::cerr << "error: registration of python converter for " << RTTI::typeName< InputType >()
                          << " failed." << std::endl;
                std::cerr << e.what() << std::endl;
            }
            catch( ... )
            {
                std::cerr << "error: registration of python converter for " << RTTI::typeName< InputType >()
                          << " failed." << std::endl;
            }
        }

        ~PythonConverterRegister()
        {
            PythonConverterRegistrationService::getInstance().unregisterConverter( ptr );
        }
    };

    //======================================== PYTHON MODULES =============================================

    //#pragma GCC visibility push(default)
    class __attribute__( ( visibility( "default" ) ) ) PythonModuleFactory
    {
    public:
        typedef void ( *InitFunc )( py::module* );
        PythonModuleFactory( const std::string& name, InitFunc initfunc );

        const std::string& getName() const;
        InitFunc getInitFunc() const;

    private:
        const std::string mName;
        const InitFunc mInitFunc;
    };

    using PythonModuleRegistrationService = RegistrationService< PythonModuleFactory >;

    class __attribute__( ( visibility( "default" ) ) ) PythonModuleRegister
    {
    private:
        std::shared_ptr< PythonModuleFactory > ptr;

    public:
        typedef void ( *InitFunc )( py::module* );

        /// Register python module in the system.
        /**
         * If a module with the same name is already registered a warning is printed to std::err.
         * \param name the full qualified name of the module (e.g., fantom.vis)
         * \param initfunc a pointer to the module init function
         */
        PythonModuleRegister( const std::string& name, InitFunc initfunc );

        /// Un-register python module from the system.
        ~PythonModuleRegister();
    };
    //#pragma GCC visibility pop
} // namespace fantom
