#pragma once

#include <stdexcept>
#include <string>

namespace fantom
{
/**
 * \def fantom_release_assert
 * a macro throwing a verbose exception if the assertion is not met.
 * The macro is compiled in debug and release mode, therefore possibly
 * slowing down the execution.
 * In cases where the assertion is checked frequently, use the
 * fantom_debug_assert macro to only throw exceptions in debug mode.
 * The advantage above the assert function in <cassert> is that
 * fantom_release_assert does not crash the running program but throws a
 * back-traceable exception instead.
 *
 * fantom_release_assert should be used to ensure certain preconditions
 * in functions that are not called in time-critical parts of the code.
 * E.g., creation of new TensorFields from pre-computed data qualifies
 * to be guarded with fantom_release_asserts.
 *
 * Coding guidelines:
 * - You must never rely on the assertion begin thrown.
 * - You must not perform any operations in "expression" that alter any
 *   state of the system.
 *
 * Hint: if you have to assert an expression that contains commas, wrap it in
 * parentheses to support the compiler in matching the macro arguments.
 * description has to be a C-string.
 */
#define fantom_release_assert( expression, description )                                                               \
    if( expression )                                                                                                   \
    { /* do nothing */                                                                                                 \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        throw fantom::assertion_failed( description ": " #expression, __FILE__, __BASE_FILE__, __LINE__ );             \
    }

/** \def fantom_debug_assert
 * a macro throwing a verbose exception if the assertion is not met
 * The macro is only compiled in debug mode to not slow down the system
 * in release mode.
 * The advantage above the assert function in <cassert> is that
 * fantom_debug_assert does not crash the running program but throws a
 * back-traceable exception instead.
 *
 * Hint: if you have to assert an expression that contains commas, wrap it in
 * parentheses to support the compiler in matching the macro arguments.
 * description has to be a C-string.
 *
 * \sa fantom_release assert
 */
#ifdef NDEBUG
#define fantom_debug_assert( expression, description ) ( (void)0 )
#else
#define fantom_debug_assert( expression, description )                                                                 \
    if( expression )                                                                                                   \
    { /* do nothing */                                                                                                 \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        throw fantom::assertion_failed( description ": " #expression, __FILE__, __BASE_FILE__, __LINE__ );             \
    }
#endif

    /// A base class for all fantom-created exceptions
    /**
     * Note that none of these exceptions are meant to be return
     * values, they all indicate programming errors that should
     * never occur in the released system.
     */
    class fantom_exception
    {
    public:
        virtual ~fantom_exception() noexcept;

        /// returns a human-readable backtrace of the stack
        /// leading to the exception
        virtual const std::string& getBacktrace() const noexcept = 0;
    };

    namespace detail
    {
        std::string obtainBacktrace() noexcept;

        /**
         * A strategy pattern for all fantom-created exceptions
         */
        template < class ExceptionBase >
        class fantom_exception_type : public ExceptionBase, public fantom_exception
        {
        protected:
            fantom_exception_type( const std::string& what_arg )
                : ExceptionBase( what_arg )
                , mBacktrace( obtainBacktrace() )
            {
            }
            fantom_exception_type( const char* what_arg )
                : ExceptionBase( what_arg )
                , mBacktrace( obtainBacktrace() )
            {
            }

        public:
            const std::string& getBacktrace() const noexcept override
            {
                return mBacktrace;
            }

        private:
            std::string mBacktrace;
        };
    }

#define DEFINE_DEFAULT_FANTOM_EXCEPTION( name, base )                                                                  \
    class name : public detail::fantom_exception_type< base >                                                          \
    {                                                                                                                  \
    public:                                                                                                            \
        typedef detail::fantom_exception_type< base > BaseType;                                                        \
                                                                                                                       \
        name( const std::string& what_arg )                                                                            \
            : BaseType( what_arg )                                                                                     \
        {                                                                                                              \
        }                                                                                                              \
                                                                                                                       \
        name( const char* what_arg )                                                                                   \
            : BaseType( what_arg )                                                                                     \
        {                                                                                                              \
        }                                                                                                              \
    };

    /// A fantom-created equivalent to std::logic_error
    /**
     * Logic errors should be thrown any time a severe issue with
     * the program code has been identified that cannot be resolved
     * immediately.
     *
     * \sa fantom_runtime_error
     */
    DEFINE_DEFAULT_FANTOM_EXCEPTION( logic_error, std::logic_error )

    /// A fantom exception for use in function stubs.
    DEFINE_DEFAULT_FANTOM_EXCEPTION( not_implemented_error, std::logic_error )

    /// A fantom-created equivalent to std::runtime_error
    /**
     * Runtime errors should be thrown any time a severe issue has
     * been identified that is not part of the basic code logic,
     * but rather an issue of invalid external input that cannot
     * be predicted easily in advance.
     *
     * A typical behavior would be throwing runtime errors when an
     * invalid input has been read from a file.
     */
    DEFINE_DEFAULT_FANTOM_EXCEPTION( runtime_error, std::runtime_error )

    /// A fantom-created equivalent to std::out_of_range
    /**
     * \sa logic_error
     */
    DEFINE_DEFAULT_FANTOM_EXCEPTION( out_of_range, std::out_of_range )

    /// A fantom-created equivalent to std::invalid_argument
    /**
     * \sa logic_error
     */
    DEFINE_DEFAULT_FANTOM_EXCEPTION( invalid_argument, std::invalid_argument )

    /// Exception thrown by fantom_assert (and it should be used only there)
    /**
     * \sa logic_error
     */
    class assertion_failed : public detail::fantom_exception_type< std::logic_error >
    {
    public:
        assertion_failed( const std::string& expression,
                          const std::string& file,
                          const std::string& baseFile,
                          int line );
    };

} // namespace fantom

namespace fantom
{
    namespace detail
    {
        extern template class fantom_exception_type< std::logic_error >;
        extern template class fantom_exception_type< std::runtime_error >;
        extern template class fantom_exception_type< std::out_of_range >;
        extern template class fantom_exception_type< std::invalid_argument >;
    }
}
