#pragma once

#include "DataObject.hpp"
#include "Domain.hpp"
#include "ValueArray.hpp"
#include "domains/List.hpp"
#include "types.hpp"

#include "../math.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace fantom
{
    /**
     * Representation of a T-valued function on R^D. \ingroup fields
     *
     * This class can be used when a continuous function on Euclidean space is defined by a formula.
     * If the function is evaluated by interpolation between sample points in the space,
     * the discrete counterpart \c Function< T > should be used instead.
     */
    template < size_t D, typename T >
    class SpatialFunction : public DataObject
    {
        typedef RTTI_Info< SpatialFunction< D, T >, DataObject > TypeInfo;
        TYPE_INFORMATION( types::TensorName< T >() + "-valued Function in " + types::DimensionName< D >() + "-space" )

    public:
        virtual T operator()( const Point< D >& position, double time = 0.0 ) const = 0;
    };


    // ================================================================================


    /// discrete evaluator
    /**
     * The purpose of an evaluator is to divide a value array into time-slices
     * and allowing an access to the values by value and time index.
     * \throw fantom::out_of_range when accessing invalid indices (only in debug build)
     * \note Use one evaluator per thread.
     */
    template < typename T >
    class DiscreteFunctionEvaluator
    {
    public:
        /// Destroys evaluator.
        virtual ~DiscreteFunctionEvaluator() {}

        /// Sets current time step.
        /**
         * \param timeStep the requested time step index.
         * \pre timeStep must be in the valid time range.
         */
        void reset( size_t timeStep )
        {
#ifndef NDEBUG
            if( timeStep >= numTimeSteps() )
                throw fantom::out_of_range( "Time step out of bounds." );
#endif

            mT = timeStep;
        }

        /// Determines the i-th value at the current time step (specified with \c reset()).
        /**
         * \param i value index.
         * \pre 0 <= \a i < \c numValues().
         */
        T value( size_t i ) const
        {
#ifndef NDEBUG
            if( i >= numValues() )
                throw fantom::out_of_range( "Value index out of bounds." );
#endif
            return implValue( i, mT );
        }

        /// \copydoc value(size_t) const
        T operator[]( size_t i ) const
        {
            return value( i );
        }

        /// Determines the number of values.
        virtual size_t numValues() const = 0;

        /// Determines the number of time steps. Has to be at least 1.
        virtual size_t numTimeSteps() const = 0;

        /// \copydoc numValues()
        size_t size() const
        {
            return numValues();
        }

    private:
        size_t mT = 0;

        virtual T implValue( size_t i, size_t t ) const = 0;
    };


    // --------------------------------------------------------------------------------


    /// discrete evaluator with time interpolation
    /**
     * The purpose of an evaluator is to divide a value array into time-slices
     * and allowing an access to the values which can perform interpolation
     * across time slices.
     * \throw fantom::out_of_range when accessing invalid indices (only in debug build)
     * \note Use one evaluator per thread.
     */
    template < typename T >
    class FunctionEvaluator
    {
    public:
        /// Destroys evaluator.
        virtual ~FunctionEvaluator() {}

        /// Determines whether a given time is inside the valid time range.
        /**
         * \param time the requested time.
         */
        bool contains( double time ) const
        {
            return mTimeRange.first <= time && time <= mTimeRange.second;
        }

        /// Sets current time.
        /**
         * \param time the requested time.
         * \pre time must be in the valid time range.
         */
        void reset( double time )
        {
            mContains = contains( time );
            if( mContains )
            {
                implReset( time );
            }
        }

        /// Determines the i-th value.
        /**
         * \param i value index.
         * \pre 0 <= \a i < \c numValues().
         */
        T value( size_t i ) const
        {
#ifndef NDEBUG
            if( i >= numValues() )
                throw fantom::out_of_range( "Value index out of bounds." );
#endif
#ifndef NDEBUG
            if( !mContains )
                throw fantom::out_of_range( "Time" );
#endif
            return implValue( i );
        }

        /// \copydoc value(size_t) const
        T operator[]( size_t i ) const
        {
            return value( i );
        }

        /// Determines whether the current time is inside the time domain.
        explicit operator bool() const
        {
            return mContains;
        }

        /// Determines the supported time range.
        const std::pair< double, double >& getTimeRange() const
        {
            return mTimeRange;
        }

        /// Determines the number of values.
        virtual size_t numValues() const = 0;

        /// \copydoc numValues()
        size_t size() const
        {
            return numValues();
        }

    protected:
        FunctionEvaluator( std::pair< double, double > timeRange )
            : mTimeRange( timeRange )
        {
        }

    private:
        const std::pair< double, double > mTimeRange;
        bool mContains;

        virtual T implValue( size_t i ) const = 0;
        virtual void implReset( double t ) = 0;
    };


    // --------------------------------------------------------------------------------


    /// Base class for discrete (sample-based) functions on Domains.
    /**
     * This class contains most of the accessor functions to the meta data of the function,
     * e.g., the temporal behavior and timesteps.
     * The actual implementation as well as value-type-specific accessors is in the templated
     * class \c Function< T >.
     * \internal
     */
    class FunctionBase : public DataObject
    {
        typedef RTTI_Info< FunctionBase, DataObject > TypeInfo;
        TYPE_INFORMATION( "Function" )

    public:
        /// Returns whether the other FunctionBase is defined on the same Domain and DomainPart
        /**
         * In that case, the two Functions are compatible and their values can be used together in computations.
         * Note, that the Functions can still differ in their number of timesteps.
         */
        bool isCompatibleTo( const FunctionBase& rhs ) const;

        /// Returns the domain this function is defined on.
        const std::shared_ptr< const Domain >& domain() const;

        /// Returns the domain part this function is defined on.
        const DomainPart& domainPart() const;

        /// Returns the time behavior of this function.
        /**
         * See TimeBehavior for further information.
         */
        TimeBehavior timeBehavior() const;

        /// Returns the number of time steps.
        size_t numTimeSteps() const;

        /// Returns the time of the i-th time step.
        double timeStep( size_t timeStepId ) const;

        /// Returns a vector of all time steps.
        const std::vector< double >& timeSteps() const;

        /// Returns a pair containing the lowest and highest time in the data.
        /**
         * In the steady case the lower and upper bound are returned.
         *
         * In the unsteady case, the first and the last time step are returned.
         */
        std::pair< double, double > timeRange() const;

        /// Returns the number of values in each time step.
        size_t numValues() const;

        /// Returns an info string containing the size and the precision of the function.
        std::vector< std::pair< std::string, std::string > > getInfoStrings() const override;

        /// Transfer the data to a new domain.
        /**
         * \param targetDomain the domain that the data should be transfered to
         * \param targetPart the part that the data should be transfered to. Use Domain::any to let the function
         * autoselect.
         * \param timesteps the time steps for the target domain. Leave empty to let the function autoselect.
         *
         * See \a transferData for details.
         *
         * \pre targetDomain != nullptr
         *
         * \throw fantom::logic_error on invalid inputs
         */
        virtual std::shared_ptr< const FunctionBase >
        transferToDomain( const std::shared_ptr< const Domain >& targetDomain,
                          const DomainPart& targetPart = Domain::any,
                          std::vector< double > timesteps = std::vector< double >{} ) const = 0;

    protected:
        FunctionBase( const std::shared_ptr< const Domain >& domain,
                      const DomainPart& part,
                      const ValueArrayBase& values,
                      std::vector< double > timesteps );

        const ValueArrayBase& values() const;

    private:
        std::shared_ptr< const Domain > mDomain;
        std::shared_ptr< const ValueArrayBase > mValues;
        const DomainPart& mDomainPart;
        TimeBehavior mTimeBehavior;
        std::vector< double > mTimesteps;
        std::pair< double, double > mTimeRange;
    };


    // --------------------------------------------------------------------------------


    /// Representation of data values on a discrete domain. \ingroup fields commit
    /**
     * The \a Domain can be any discrete structure, like \a Grid, \a Graph, \a PointSet, \a Table, etc.
     * Function< T > associates some values (represented by a \c ValueArray) with such a discrete structure.
     * All meta data that characterizes this association (time behavior, timesteps, part of the domain that the data
     * belongs to) is stored in this object.
     * Specific accessor objects for easy evaluation of the function at specific points in time are provided through the
     * \c makeEvaluator() and \c makeDiscreteEvaluator() functions.
     *
     * The goal of this design is to have an easy interface to the data that is independent from the specific domain the
     * data is registered on (regarding the C++ type). This makes it easy to write common and general algorithms that
     * only process data without needing to know the domain type.
     * All domain-related data is encoded in the meta-data and is easily copyable.
     * This allows for truly domain-agnostic algorithms without the disadvantage that the domain-related data would be
     * pruned.
     */
    template < typename T >
    class Function : public FunctionBase
    {
        typedef RTTI_Info< Function< T >, FunctionBase > TypeInfo;
        TYPE_INFORMATION( types::TensorName< T >() + "-valued Function" )

    public:
        /// The underlying data type. Use to query default types, like ScalarFunction::DataType.
        using DataType = T;

        /// Construct a new function on the given domain and domainpart.
        /**
         * \pre The domain must not be nullptr.
         * \pre domain.domainPartSize( part ) * timesteps.size() == values.size()
         */
        Function( const std::shared_ptr< const Domain >& domain,
                  const DomainPart& part,
                  const ValueArray< T >& values,
                  std::vector< double > timesteps = std::vector< double >{} )
            : FunctionBase( domain, part, values, move( timesteps ) )
        {
        }

        /// Return the underlying ValueArray.
        const ValueArray< T >& values() const
        {
            return static_cast< const ValueArray< T >& >( FunctionBase::values() );
        }

        /// Create a time-interpolated evaluator for access to the function at given points in time.
        std::unique_ptr< FunctionEvaluator< T > > makeEvaluator() const;
        /// Create a discrete evaluator for access to time-slices.
        std::unique_ptr< DiscreteFunctionEvaluator< T > > makeDiscreteEvaluator() const;

        std::shared_ptr< const FunctionBase > transferToDomain( const std::shared_ptr< const Domain >& targetDomain,
                                                                const DomainPart& targetPart = Domain::any,
                                                                std::vector< double > timesteps
                                                                = std::vector< double >{} ) const override;
    };

    /// Detect whether a type is a function
    template < typename T >
    using is_function = std::is_base_of< FunctionBase, T >;


    namespace function_detail
    {
        template < typename List >
        struct TestIsA
        {
            static bool test( const Domain& obj )
            {
                return obj.isA< typename List::First >() || TestIsA< typename List::Rest >::test( obj );
            }
        };

        template <>
        struct TestIsA< TypeList<> >
        {
            static bool test( const Domain& )
            {
                return false;
            }
        };
    } // namespace function_detail


    /// Acceptor function that checks whether a \a Function is defined on a specific \a Domain type. \ingroup fields
    /**
     * The domain type is specified as a template parameter.
     * The domain part is specified at runtime with the function parameter. Leave it empty to allow for functions on any
     * domain part.
     *
     * \tparam DomainT A type or TypeList of Domains.
     * \param part The DomainPart on which the data should be defined. Use Domain::any to let it remain unspecified.
     *
     * Usage example:
     *     add< Function< Scalar > >( "Values", "A scalar array on a grid", definedOn< Grid< 3 > >( Grid< 3 >::Points )
     * );
     */
    template < typename DomainT >
    static std::function< bool( const DataObject& ) > definedOn( const DomainPart& part = Domain::any )
    {
        using List = typename TypeList< DomainT >::Flat;
        static_assert( typename List::template all_of< is_domain >(),
                       "DomainT or the types in it must be derived from Domain." );

        return [&part]( const DataObject& obj ) -> bool {
            if( obj.isA< FunctionBase >() )
            {
                auto& func = static_cast< const FunctionBase& >( obj );
                if( part != Domain::any && part != func.domainPart() )
                {
                    return false;
                }
                auto& domain = func.domain();
                return function_detail::TestIsA< List >::test( *domain );
            }
            return false;
        };
    }


    // --------------------------------------------------------------------------------


    /// Register data on a domain. \ingroup commit
    /**
     * \param domain The domain the data is to be registered on.
     * \param part The part of this domain that the data is to be registered on.
     * \param values The actual data values.
     * \param timesteps The discrete timesteps that the data is available for.
     * \pre domain is not nullptr
     * \pre values.size() == timesteps.size() * domainPartSize( part )
     *
     * Note, that the values are grouped by timesteps, i.e., for every element in the domainPart there is one value and
     * all values for one timestep are consecutive in values.
     */
    template < typename T >
    std::shared_ptr< const Function< T > > addData( const std::shared_ptr< const Domain >& domain,
                                                    const DomainPart& part,
                                                    const ValueArray< T >& values,
                                                    std::vector< double > timesteps = std::vector< double >() )
    {
        return std::make_shared< Function< T > >( domain, part, values, move( timesteps ) );
    }

    /// Register data on a domain. \ingroup commit
    /**
     * \param domain The domain the data is to be registered on.
     * \param part The part of this domain that the data is to be registered on.
     * \param values The actual data values.
     * \param precision The data format the values are given in
     * \param timesteps The discrete timesteps that the data is available for.
     * \pre domain is not nullptr
     * \pre values.size() == timesteps.size() * domainPartSize( part )
     *
     * Note, that the values are grouped by timesteps, i.e., for every element in the domainPart there is one value and
     * all values for one timestep are consecutive in values.
     */
    template < typename T >
    std::shared_ptr< const Function< T > > addData( const std::shared_ptr< const Domain >& domain,
                                                    const DomainPart& part,
                                                    std::vector< T > values,
                                                    Precision precision = Precision::FLOAT64,
                                                    std::vector< double > timesteps = std::vector< double >() )
    {
        auto v = makeValueArray( move( values ), precision );
        return addData( domain, part, *v, move( timesteps ) );
    }

    /// Create a new List domain and register the data on it. \ingroup commit
    /**
     * This allows to easily define "raw" data lists, i.e., data that does not exist on a particular domain.
     *
     * \param values The actual data values.
     * \param timesteps The discrete timesteps that the data is available for.
     *
     * Note, that the values are grouped by timesteps, i.e., for every element in the domainPart there is one value and
     * all values for one timestep are consecutive in values.
     */
    template < typename T >
    std::shared_ptr< const Function< T > > makeData( const ValueArray< T >& values,
                                                     std::vector< double > timesteps = std::vector< double >() )
    {
        auto list = std::make_shared< List >( values.size() / std::max< size_t >( 1, timesteps.size() ) );
        return addData( list, List::Entries, values, move( timesteps ) );
    }

    /// Create a new List domain and register the data on it. \ingroup commit
    /**
     * This allows to easily define "raw" data lists, i.e., data that does not exist on a particular domain.
     *
     * \param values The actual data values.
     * \param precision The data format the values are given in
     * \param timesteps The discrete timesteps that the data is available for.
     *
     * Note, that the values are grouped by timesteps, i.e., for every element in the domainPart there is one value and
     * all values for one timestep are consecutive in values.
     */
    template < typename T >
    std::shared_ptr< const Function< T > > makeData( std::vector< T > values,
                                                     Precision precision = Precision::FLOAT64,
                                                     std::vector< double > timesteps = std::vector< double >() )
    {
        auto v = makeValueArray( move( values ), precision );
        return makeData( *v, move( timesteps ) );
    }

    /// Transfers a given Function to a new Domain. \ingroup commit
    /**
     * The result is a Function defined on \a targetDomain.
     * If the original data fit the targetDomain (in terms of the number of values), the transfer is done.
     * Otherwise, if targetDomain is a subdomain of the domain that the original data is registered on, the data is
     * transfered down to the subdomain by means of a lookup.
     * If the transfer does not complete, an exception is thrown.
     *
     * \tparam T the value type.
     * \param data the original Function
     * \param targetDomain the associated domain to create the tensor field for
     * \param targetPart the domain part that the data should be transfered to. Use Domain::any to let the function
     * autoselect.
     * \param timesteps the target timesteps for the data. Leave empty to let the function autoselect.
     *
     * \pre targetDomain != nullptr
     *
     * \throw fantom::logic_error on invalid inputs
     */
    template < class T >
    static std::shared_ptr< const Function< T > > transferData( const Function< T >& data,
                                                                const std::shared_ptr< const Domain >& targetDomain,
                                                                const DomainPart& targetPart = Domain::any,
                                                                std::vector< double > timesteps
                                                                = std::vector< double >{} )
    {
        if( !targetDomain )
        {
            throw fantom::logic_error( "Data can't be registered to non-existent domains." );
        }

        auto* part = &targetPart;
        if( targetPart == Domain::any )
        {
            part = &data.domainPart();
        }

        if( timesteps.empty() )
        {
            timesteps = data.timeSteps();
        }

        if( targetDomain->domainPartSize( *part ) * timesteps.size() == data.values().size() )
        {
            // transfer directly
            return addData( targetDomain, *part, data.values(), move( timesteps ) );
        }
        else if( auto subDomain = dynamic_cast< const Subdomain* >( targetDomain.get() ) )
        {
            if( subDomain->parent() == data.domain() && subDomain->subclassedPart() == data.domainPart() )
            {
                // transfer as SubValueArray
                auto values = std::make_shared< SubValueArray< T > >( data.values(), subDomain->getLookup() );
                return addData( targetDomain, data.domainPart(), *values, data.timeSteps() );
            }
        }
        throw fantom::logic_error( "The data does not fit onto the specified domain" );
    }


    // --------------------------------------------------------------------------------


    /// Scalar functions \ingroup fields
    using ScalarFunction = Function< Scalar >;
    /// Vector functions \ingroup fields
    template < size_t D >
    using VectorFunction = Function< Tensor< double, D > >;
    /// Matrix functions \ingroup fields
    template < size_t D >
    using MatrixFunction = Function< Tensor< double, D, D > >;
    /// Color functions \ingroup fields
    using ColorFunction = Function< Color >;
    /// Integer functions \ingroup fields
    using IntegerFunction = Function< Integer >;

    /// List of functions \ingroup fields
    template < typename... Types >
    using FunctionsOf = TypeList< Function< Types >... >;

    /// List of functions \ingroup fields
    template < size_t Dim, typename... Types >
    using SpatialFunctionsOf = TypeList< SpatialFunction< Dim, Types >... >;

    // ================================================================================


    namespace detail
    {
        template < class T >
        struct DiscreteFunctionEvaluatorDefault : DiscreteFunctionEvaluator< T >
        {
            DiscreteFunctionEvaluatorDefault( const ValueArray< T >& values, size_t numTimeSteps )
                : mValues( values )
                , mNumTimeSteps( numTimeSteps )
                , mStride( mValues.size() / numTimeSteps )
            {
            }

            size_t numValues() const override
            {
                return mStride;
            }

            size_t numTimeSteps() const override
            {
                return mNumTimeSteps;
            }

            T implValue( size_t i, size_t t ) const override
            {
                return mValues[mStride * t + i];
            }

            const ValueArray< T >& mValues;
            size_t mNumTimeSteps;
            size_t mStride;
        };

        template < class T >
        struct FunctionEvaluatorSteady : FunctionEvaluator< T >
        {
            FunctionEvaluatorSteady( const ValueArray< T >& values )
                : FunctionEvaluator< T >(
                      std::make_pair( std::numeric_limits< double >::lowest(), std::numeric_limits< double >::max() ) )
                , mValues( values )
            {
            }

            virtual void implReset( double ) override {}

            virtual size_t numValues() const override
            {
                return mValues.size();
            }

            virtual T implValue( size_t i ) const override
            {
                return mValues[i];
            }

            const ValueArray< T >& mValues;
        };

        template < class T >
        struct FunctionEvaluatorUnsteady : FunctionEvaluator< T >
        {
            const ValueArray< T >& mValues;
            const size_t mStride; // number of values for one time step
            const double* mFirst;
            const double* mLast;
            size_t mN; // index of first value for the first time step of the current time interval
            double mA; // interpolation weight within the current time interval

            FunctionEvaluatorUnsteady( const ValueArray< T >& values, size_t numTimeSteps, const double* timeSteps )
                : FunctionEvaluator< T >( std::make_pair( timeSteps[0], timeSteps[numTimeSteps - 1] ) )
                , mValues( values )
                , mStride( mValues.size() / numTimeSteps )
                , mFirst( &timeSteps[1] ) // not &timeSteps[0] as we always consider the upper bound of the timesteps
                , mLast( &timeSteps[numTimeSteps - 1] )
                , mN( 0 )
                , mA( 0 )
            {
            }

            void implReset( double time ) override
            {
                // find time-interval containing time (in this case the upper bound of it)
                const double* p = std::lower_bound( mFirst, mLast, time );
                // -> time step is between *(p-1) and *p
                // number of time step is (p - mFirst) as mFirst is (&timeSteps[0] + 1)
                mN = mStride * ( p - mFirst );
                mA = ( time - *( p - 1 ) ) / ( *p - *( p - 1 ) );
            }

            size_t numValues() const override
            {
                return mStride;
            }

            T implValue( size_t i ) const override
            {
                LinearCombination< T > comb;
                T result{};
                // equivalent to ( 1.0 - mA ) * mValues[mN + i] + mA * mValues[mN + mStride + i];
                size_t indices[2] = { mN + i, mN + mStride + i };
                double weights[2] = { 1.0 - mA, mA };
                comb( result, 2, mValues, indices, weights );
                return result;
            }
        };
    } // namespace detail

    template < typename T >
    std::unique_ptr< FunctionEvaluator< T > > Function< T >::makeEvaluator() const
    {
        if( timeBehavior() == TimeBehavior::steady )
        {
            return std::make_unique< detail::FunctionEvaluatorSteady< T > >( values() );
        }
        else
        {
            return std::make_unique< detail::FunctionEvaluatorUnsteady< T > >(
                values(), numTimeSteps(), timeSteps().data() );
        }
    }

    template < typename T >
    std::unique_ptr< DiscreteFunctionEvaluator< T > > Function< T >::makeDiscreteEvaluator() const
    {
        return std::make_unique< detail::DiscreteFunctionEvaluatorDefault< T > >( values(), numTimeSteps() );
    }

    template < typename T >
    std::shared_ptr< const FunctionBase >
    Function< T >::transferToDomain( const std::shared_ptr< const Domain >& targetDomain,
                                     const DomainPart& targetPart,
                                     std::vector< double > timesteps ) const
    {
        return transferData( *this, targetDomain, targetPart, move( timesteps ) );
    }


    // --------------------------------------------------------------------------------


    namespace function_detail
    {
        template < size_t D, typename T, typename Functor >
        class SpatialFunctionImpl : public SpatialFunction< D, T >
        {
            Functor mFunc;

            // FIXME: we have to check that Functor supports calling with Point<D> and double and returns a T.
            // BUT: C++14 does not allow that (easily).

        public:
            SpatialFunctionImpl( Functor&& func )
                : mFunc( std::move( func ) )
            {
            }

            SpatialFunctionImpl( const Functor& func )
                : mFunc( func )
            {
            }

            T operator()( const Point< D >& position, double time ) const override
            {
                return mFunc( position, time );
            }
        };
    } // namespace function_detail

    /// Factory function for spatial functions. \ingroup commit
    /**
     * \param function A callable object that evaluates a formula for this spatial function.
     * \tparam D the dimension of the spatial domain that this function is defined in
     * \tparam T the data type of the function
     * \tparam Functor the type of the function that evaluates the formula.
     *
     * The Functor type must be callable with the signature function( const Point<D>&, double ) and return an object of
     * type T.
     */
    template < size_t D, typename T, typename Functor >
    std::shared_ptr< const SpatialFunction< D, T > > makeSpatialFunction( Functor&& function )
    {
        return std::make_shared< function_detail::SpatialFunctionImpl< D, T, std::decay_t< Functor > > >(
            std::forward< Functor >( function ) );
    }


    // --------------------------------------------------------------------------------


    extern template class Function< Scalar >;
    extern template class Function< Vector1 >;
    extern template class Function< Vector2 >;
    extern template class Function< Vector3 >;
    extern template class Function< Vector4 >;
    extern template class Function< Matrix1 >;
    extern template class Function< Matrix2 >;
    extern template class Function< Matrix3 >;
    extern template class Function< Matrix4 >;
    extern template class Function< Color >;
    extern template class Function< std::string >;
    extern template class Function< std::complex< double > >;
} // namespace fantom
