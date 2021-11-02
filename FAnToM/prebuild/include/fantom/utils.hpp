#pragma once

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <vector>

#if defined( __GNUC__ ) && !defined( __clang__ )
#define INTENTIONAL_FALLTHROUGH [[gnu::fallthrough]]
#else
#define INTENTIONAL_FALLTHROUGH
#endif

#define F_GUI_DECLARE_PRIVATE( Class )                                                                                 \
    inline gui_private::Class##Private* p()                                                                            \
    {                                                                                                                  \
        return reinterpret_cast< gui_private::Class##Private* >( mPimpl );                                             \
    }                                                                                                                  \
    inline const gui_private::Class##Private* p() const                                                                \
    {                                                                                                                  \
        return reinterpret_cast< const gui_private::Class##Private* >( mPimpl );                                       \
    }                                                                                                                  \
    friend class gui_private::Class##Private;                                                                          \
    friend class gui_private::GuiFactory;

#define F_GRAPHICS2D_DECLARE_PRIVATE( Class )                                                                          \
    inline graphics2D_private::Class##Private* p()                                                                     \
    {                                                                                                                  \
        return reinterpret_cast< graphics2D_private::Class##Private* >( mPimpl );                                      \
    }                                                                                                                  \
    inline const graphics2D_private::Class##Private* p() const                                                         \
    {                                                                                                                  \
        return reinterpret_cast< const graphics2D_private::Class##Private* >( mPimpl );                                \
    }                                                                                                                  \
    friend class graphics2D_private::Class##Private;                                                                   \
    friend class graphics2D_private::Graphics2DFactory;

#define F_GUI_DECLARE_PUBLIC( Class )                                                                                  \
    inline Class* p()                                                                                                  \
    {                                                                                                                  \
        return static_cast< Class* >( mPumpl );                                                                        \
    }                                                                                                                  \
    inline const Class* p() const                                                                                      \
    {                                                                                                                  \
        return static_cast< const Class* >( mPumpl );                                                                  \
    }                                                                                                                  \
    friend class Class;

#define F_GRAPHICS2D_DECLARE_PUBLIC( Class )                                                                           \
    inline Class* p()                                                                                                  \
    {                                                                                                                  \
        return static_cast< Class* >( mPumpl );                                                                        \
    }                                                                                                                  \
    inline const Class* p() const                                                                                      \
    {                                                                                                                  \
        return static_cast< const Class* >( mPumpl );                                                                  \
    }                                                                                                                  \
    friend class Class;


/**
 * \def FANTOM_DEPRECATED
 * macros to declare functions as deprecated
 * usage:
 * FANTOM_DEPRECATED void oldFunc() const;
 * Other things to do when marking a function deprecated for later releases:
 * - Always provide a new function.
 * - Re-implement the old function using the new interface.
 * - mark the function \ deprecated in the doxygen documentation and refer to the new function.
 * - provide example code that uses the new function to help developers to port their code.
 * - run the tests on the new implementaion of oldFunc()
 */
#if defined( __GNUC__ )
// this should work for clang and gcc
#define FANTOM_DEPRECATED __attribute__( ( deprecated ) )
#elif defined( _MSC_VER )
// this should work for Microsoft Visual C++
#define FANTOM_DEPRECATED __declspec( deprecated )
#else
#pragma message( "WARNING: You need to implement the FANTOM_DEPRECATED macro for this compiler in fantom/utils.hpp" )
#define FANTOM_DEPRECATED
#endif

namespace fantom
{
    class RTTI;          // forward-declaration of RTTI-enabled types
    class DataObject;    // forward-declaration of data object types
    class DataInterface; // forward-declaration of data interface types

    /// Global always-false abort flag for use in toolboxes as a default argument for abortable functions.
    const bool& neverAbort();


    // ================================================================================================================


    namespace detail
    {
        /// Create a delegate template that simply absorbs the template type.
        template < typename T >
        struct sfinae_true : std::true_type
        {
        };


        /// Base implementation of tag based index types
        template < typename Tag, typename IndexT = size_t >
        struct Index
        {
            explicit Index( IndexT index )
                : mIndex( index )
            {
            }

            bool operator==( const Index< Tag, IndexT >& rhs ) const
            {
                return mIndex == rhs.mIndex;
            }

            bool operator!=( const Index< Tag, IndexT >& rhs ) const
            {
                return mIndex != rhs.mIndex;
            }

            operator IndexT() const
            {
                return mIndex;
            }

        private:
            IndexT mIndex;
        };
    } // namespace detail


    // ================================================================================================================


    template < typename... Types >
    struct TypeList;

    namespace typelist_detail
    {
        template < typename... Lists >
        struct ConcatLists;
        template <>
        struct ConcatLists<> // empty list
        {
            using type = TypeList<>;
        };
        template < typename... T1 >
        struct ConcatLists< TypeList< T1... > > // single typelist
        {
            using type = TypeList< T1... >;
        };
        template < typename... T1, typename... T2, typename... Lists >
        struct ConcatLists< TypeList< T1... >, TypeList< T2... >, Lists... > // multiple typelists
        {
            using type = typename ConcatLists< TypeList< T1..., T2... >, Lists... >::type;
        };

        template < typename T1, typename T2 >
        using ConcatLists_t = typename ConcatLists< T1, T2 >::type;

        template < typename... L >
        struct Split;
        template <>
        struct Split<> // empty list
        {
            using First = void;
            using Rest = TypeList<>;
        };
        template < typename F, typename... L >
        struct Split< F, L... > // general case
        {
            using First = F;
            using Rest = TypeList< L... >;
        };

        template < typename... L >
        using GetFirst_t = typename Split< L... >::First;
        template < typename... L >
        using GetRest_t = typename Split< L... >::Rest;

        template < typename List >
        struct Flatten
        {
            using type = TypeList< List >; // fall-back for non-list arguments
        };
        template < typename... T >
        struct Flatten< TypeList< T... > > // single type list -> flatten recursively
        {
            using type = typename ConcatLists< typename Flatten< T >::type... >::type;
        };
        template <>
        struct Flatten< TypeList<> > // empty type list
        {
            using type = TypeList<>;
        };

        template < typename List >
        using GetFlat_t = typename Flatten< List >::type;

        template < bool... V >
        struct BoolList
        {
        };

        template < bool... V >
        using all_of_t = std::is_same< BoolList< V... >, BoolList< ( V | true )... > >;
    } // namespace typelist_detail

    /// Simple type list interface
    template < typename... Types >
    struct TypeList
    {
        /// Concatenate two type lists
        template < typename ListType >
        using _or = typelist_detail::ConcatLists_t< TypeList< Types... >, ListType >;

        using First = typelist_detail::GetFirst_t< Types... >;
        using Rest = typelist_detail::GetRest_t< Types... >;

        /// Flatten, i.e., if a TypeList is among Types then unpack it recursively.
        using Flat = typelist_detail::GetFlat_t< TypeList< Types... > >;

        static const bool isEmpty = sizeof...( Types ) == 0;

        /// Test with a predicate whether all types satisfy the predicate.
        /// The Predicate<Type> must be a derived from bool_constant.
        template < template < typename > class Predicate >
        using all_of = typelist_detail::all_of_t< Predicate< Types >{}... >;

        /// Apply a functor to every type in the list.
        /// The Functor<Type> must define a type member type that contains the transformed type.
        template < template < typename > class Functor >
        using map = TypeList< typename Functor< Types >::type... >;
    };

    /// Detect whether a type is a type list
    template < typename T >
    struct is_type_list : std::false_type
    {
    };

    template < typename... Types >
    struct is_type_list< TypeList< Types... > > : std::true_type
    {
    };


    // ================================================================================


    namespace detail
    {
        template < typename TType, typename Enable = void >
        struct CallFunctorForType
        {
        };

        template < typename TType >
        struct CallFunctorForType< TType, typename std::enable_if_t< std::is_base_of< RTTI, TType >::value > >
        {
            template < typename Functor >
            static bool apply( const RTTI& d, Functor& func )
            {
                auto casted = dynamic_cast< std::add_const_t< TType >* >( &d );
                if( casted )
                {
                    func( *casted );
                    return true;
                }
                else
                {
                    return false;
                }
            }

            template < typename Functor >
            static bool apply( const std::shared_ptr< const RTTI >& d, Functor&& func )
            {
                auto casted = std::dynamic_pointer_cast< std::add_const_t< TType > >( d );
                if( casted )
                {
                    func( casted );
                    return true;
                }
                else
                {
                    return false;
                }
            }
        };

        template < typename TType >
        struct CallFunctorForType< TType, typename std::enable_if_t< std::is_base_of< DataInterface, TType >::value > >
        {
            template < typename Functor >
            static bool apply( const DataObject& d, Functor& func )
            {
                auto casted = TType::applyTo( d );
                if( casted )
                {
                    func( move( casted ) );
                    return true;
                }
                else
                {
                    return false;
                }
            }

            template < typename Functor >
            static bool apply( const std::shared_ptr< const DataObject >& d, Functor& func )
            {
                return apply( *d, func );
            }
        };

        // General case: handling the first element of the type list and delegating to the rest list
        template < typename Data, typename Functor, typename TypeList >
        struct ApplyForListImpl
        {
            static bool apply( const Data& d, Functor&& func )
            {
                return CallFunctorForType< typename TypeList::First >::apply( d, func )
                       || ApplyForListImpl< Data, Functor, typename TypeList::Rest >::apply(
                              d, std::forward< Functor >( func ) );
            }

            static bool apply( const std::shared_ptr< const Data >& d, Functor&& func )
            {
                return CallFunctorForType< typename TypeList::First >::apply( d, func )
                       || ApplyForListImpl< Data, Functor, typename TypeList::Rest >::apply(
                              d, std::forward< Functor >( func ) );
            }
        };

        // End of recursion: empty type list
        template < typename Data, typename Functor >
        struct ApplyForListImpl< Data, Functor, TypeList<> >
        {
            static bool apply( const Data&, Functor&& )
            {
                return false;
            }

            static bool apply( const std::shared_ptr< const Data >&, Functor&& )
            {
                return false;
            }
        };
    } // namespace detail

    /// Call a functor with the casted data when d can be cast to any of the specified types. \ingroup fields
    /**
     * The function can be used for easy type multiplexing, e.g., when handling multiple possible input types.
     * In the easiest case, this is combined with C++14 generic lambdas for handling the visiting, though any struct
     * with overloaded operator() for all types in the type list would do, also.
     *
     * Usage:
     *     applyFor< Function< Scalar >, Function< Vector2 >, Function< Vector3 > >( dataObject, []( auto& field ) {
     *         // do something
     *         } );
     *
     * \return true if any cast was successful
     * \param d the data that should be cast
     * \param func the functor that will be called
     * \tparam Types A list of types that the cast will be tried for. Also TypeList's can be used.
     * \tparam Functor the functor type (auto-deduced!)
     * \tparam Data the type of the data (auto-deduced!)
     */
    template < typename... Types, typename Functor, typename Data >
    bool applyFor( const Data& d, Functor&& func )
    {
        return detail::ApplyForListImpl< Data, Functor, typename TypeList< Types... >::Flat >::apply(
            d, std::forward< Functor >( func ) );
    }

    /// Call a functor with the casted data when d can be cast to any of the specified types. \ingroup fields
    /**
     * Overload for shared_ptrs.
     *
     * \copydoc applyFor
     */
    template < typename... Types, typename Functor, typename Data >
    bool applyFor( const std::shared_ptr< const Data >& d, Functor&& func )
    {
        if( !d )
        {
            return false;
        }
        return detail::ApplyForListImpl< Data, Functor, typename TypeList< Types... >::Flat >::apply(
            d, std::forward< Functor >( func ) );
    }


    // ================================================================================================================


    class bad_lexical_cast : public std::exception
    {
        virtual const char* what() const throw();
    };

    namespace detail
    {
        // most generic fallback
        // use only for non-string output to differentiate from the string-output type
        template < class Out,
                   class In,
                   typename = typename std::enable_if< !std::is_base_of< std::string, Out >::value >::type,
                   typename = void >
        Out lexical_cast_impl( const In& in )
        {
            Out out;
            std::stringstream s;
            auto prec = std::max( std::numeric_limits< Out >::digits10, std::numeric_limits< In >::digits10 );
            if( prec )
                s.precision( prec - 2 );

            s << in;
            s >> out;

            if( s.fail() || !s.eof() )
                throw bad_lexical_cast();

            return out;
        }

        // use only for string output
        template < class Out,
                   class In,
                   typename = typename std::enable_if< std::is_base_of< std::string, Out >::value >::type >
        Out lexical_cast_impl( const In& in )
        {
            std::ostringstream s;
            auto prec = std::numeric_limits< In >::digits10;
            if( prec )
                s.precision( prec - 2 );

            s << in;

            return s.str();
        }

        // generic fallback for string input
        // use only for non-string output to differentiate from string output version
        template < class Out,
                   typename = typename std::enable_if< !std::is_base_of< std::string, Out >::value >::type,
                   typename = void >
        Out lexical_cast_impl( const std::string& in )
        {
            Out out;
            std::istringstream s( in );
            auto prec = std::numeric_limits< Out >::digits10;
            if( prec )
                s.precision( prec - 2 );

            s >> out;

            if( s.fail() || !s.eof() )
                throw bad_lexical_cast();

            return out;
        }

        // special case for reusing strings
        // use only for string output
        template < class Out, typename = typename std::enable_if< std::is_base_of< std::string, Out >::value >::type >
        Out lexical_cast_impl( const std::string& in )
        {
            return in;
        }
    } // namespace detail

    template < class Out, class In >
    Out lexical_cast( const In& in )
    {
        return detail::lexical_cast_impl< Out >( in );
    }


    // ================================================================================================================


    /**
     * Default clock of fantom. Please use this instead of a fully qualified std::chrono::... type
     * so all clock calculations are consistent internally and robust against possible future changes.
     */
    typedef std::chrono::steady_clock clock;

    template < typename Ref, typename Period >
    std::string formatDuration( const std::chrono::duration< Ref, Period >& duration )
    {
        std::stringstream s;

        size_t minutes = std::chrono::duration_cast< std::chrono::minutes >( duration ).count();
        size_t seconds = std::chrono::duration_cast< std::chrono::seconds >( duration ).count() - 60 * minutes;

        s << minutes << "m " << seconds << "s";

        return s.str();
    }

    template < typename Ref, typename Period >
    std::string formatDurationFloatingPoint( const std::chrono::duration< Ref, Period >& duration )
    {
        std::stringstream s;

        s << std::chrono::duration_cast< std::chrono::duration< float > >( duration ).count() << "s";

        return s.str();
    }


    // ================================================================================================================


    /**
     * @brief Simple class for time measurements.
     */
    class Timing
    {
    public:
        /// Initializes the timing. If startPaused is false, the timing starts immediately.
        Timing( bool startPaused = true );

        /// Start the timing. Has no effect if not currently paused.
        void start();

        /// Pause the timing and add the elapsed time to the duration. Has no effect if already paused.
        void pause();

        /// Reset the current elapsed time to zero. Also restarts the timing if not currently paused.
        void reset();

        /// Get the elapsed time in seconds.
        double getSeconds() const;

        /// Get the elapsed time in milli seconds.
        double getMilliSeconds() const;

        /// Get the elapsed time in nano seconds.
        std::chrono::nanoseconds getElapsed() const;

        /// Reported paused status.
        bool isPaused() const;

        /**
         * @brief Class for creating time measurements and printing them in a "resource acquisition is
         * initialization" fashion. The timing lasts as long as the object lives. At destruction, the message and
         * the timing is printed to an outstream.
         */
        class ScopedTiming;

    private:
        using clock = std::chrono::steady_clock;
        clock::duration mDuration;
        clock::time_point mStart;
        bool mPaused;
    };


    // ----------------------------------------------------------------------------------------------------------------


    // TODO: maybe, we should not output anything here, no matter how tempting that seems.
    // TODO: just use the ScopedTiming to time exactly during that scope
    class Timing::ScopedTiming
    {
    public:
        ScopedTiming( std::ostream& logStream );
        ScopedTiming( const std::string& message, std::ostream& logStream );
        ~ScopedTiming();

    private:
        std::string mMessage;
        std::ostream& mLogStream;

        Timing mTiming;
    };
} // namespace fantom
