#pragma once

#include "../DataInterface.hpp"
#include "../Function.hpp"
#include "../domains/Grid.hpp"
#include "../types.hpp"

#include "../../math.hpp"

#include <memory>
#include <string>

namespace fantom
{
    /// Evaluator. \ingroup fields
    /**
     * Objects of this class evaluate values of tensor fields at specific
     * locations in the domain. As evaluation of tensor fields on unstructured
     * grids can be slow, the evaluator is allowed to cache certain things
     * making it not thread-safe, but re-entrant.
     *
     * \tparam D dimension of the domain.
     * \tparam T tensor type of the co-domain (must be default constructable).
     * \throw fantom::out_of_range when accessing invalid indices or positions (only in debug build)
     * \note All members are re-entrant.
     */
    template < size_t D, typename T >
    class FieldEvaluator
    {
    public:
        /// Destroys evaluator.
        virtual ~FieldEvaluator()
        {
        }

        /// Determines whether a given time is inside the valid time range.
        /**
         * \param time the requested time.
         */
        bool contains( double time ) const
        {
            return mTimeRange.first <= time && time <= mTimeRange.second;
        }

        /// Determines whether the current location is inside the domain.
        virtual explicit operator bool() const = 0;

        /// Sets the position and time for evaluations.
        /**
         * \param p the requested point.
         * \param time the requested time.
         * \pre \c contains(time).
         * \return *this.
         */
        FieldEvaluator& reset( const Point< D >& p, double time = 0.0 )
        {
#ifndef NDEBUG
            if( !contains( time ) )
            {
                throw fantom::out_of_range( "Time " + lexical_cast< std::string >( time )
                                            + " out of valid time range." );
            }
#endif
            implReset( p, time );
            return *this;
        }

        /// Determines the value of the tensor field at the current location.
        /**
         * \pre *this (the current location is inside the domain).
         */
        T value() const
        {
#ifndef NDEBUG
            if( !*this )
            {
                throw fantom::out_of_range( "Evaluation at invalid position." );
            }
#endif
            return implValue();
        }

    protected:
        /// Creates evaluator with given time range.
        FieldEvaluator( std::pair< double, double > timeRange )
            : mTimeRange( timeRange )
        {
        }

    private:
        virtual void implReset( const Point< D >&, double ) = 0;
        virtual T implValue() const = 0;

        const std::pair< double, double > mTimeRange;
    };

    /// An analytical or interpolated tensor field. \ingroup fields
    /**
     * A tensor field is a function G -> V from a domain (kernel) G to a co-domain
     * (range, value) V, where V is usually a set of tensors of same order and dimension.
     * Note: T must support addition (T+T) and scalar multiplication (double*T) for the evaluator to work.
     *
     * \tparam D dimension of the domain.
     * \tparam T tensor type of the co-domain (must be default constructable).
     * \throw Nothing is thrown by any member.
     * \note All members are thread-safe.
     */
    template < size_t D, class T >
    class Field : public DataInterface
    {
    public:
        using AppliesTo = TypeList< Function< T >, SpatialFunction< D, T > >;

        /// Create an evaluator for access to this field.
        virtual std::unique_ptr< FieldEvaluator< D, T > > makeEvaluator() const = 0;

        /// Get the time behavior of the field.
        virtual TimeBehavior getTimeBehavior() const = 0;

        static bool appliesTo( const DataObject& obj );

        static std::unique_ptr< Field< D, T > > applyTo( const DataObject& obj ) noexcept;
    };

    /// List of Fields \ingroup fields
    template < size_t Dim, typename... Types >
    using FieldsOf = TypeList< Field< Dim, Types >... >;
}

#include "impl/InterpolatorBasedField.hpp"
#include "impl/SpatialFunctionField.hpp"

namespace fantom
{
    template < size_t D, class T >
    bool Field< D, T >::appliesTo( const DataObject& obj )
    {
        if( obj.isA< SpatialFunction< D, T > >() )
        {
            return true;
        }
        else if( obj.isA< Function< T > >() )
        {
            auto& func = static_cast< const Function< T >& >( obj );
            return dynamic_cast< const InterpolatableDomain< D >* >( func.domain().get() ) != nullptr;
        }
        else
        {
            return false;
        }
    }

    template < size_t D, class T >
    std::unique_ptr< Field< D, T > > Field< D, T >::applyTo( const DataObject& obj ) noexcept
    {
        if( obj.isA< SpatialFunction< D, T > >() )
        {
            return std::make_unique< interface_detail::SpatialFunctionField< D, T > >(
                static_cast< const SpatialFunction< D, T >& >( obj ) );
        }
        else if( obj.isA< Function< T > >() )
        {
            auto& func = static_cast< const Function< T >& >( obj );
            if( !dynamic_cast< const InterpolatableDomain< D >* >( func.domain().get() ) )
            {
                return nullptr;
            }
            else
            {
                return std::make_unique< interface_detail::InterpolatorBasedField< D, T > >( func );
            }
        }
        else
        {
            return nullptr;
        }
    }


    // --------------------------------------------------------------------------------


    extern template class Field< 2, Scalar >;
    extern template class Field< 3, Scalar >;
    extern template class Field< 2, Vector1 >;
    extern template class Field< 3, Vector1 >;
    extern template class Field< 2, Vector2 >;
    extern template class Field< 3, Vector2 >;
    extern template class Field< 2, Vector3 >;
    extern template class Field< 3, Vector3 >;
    extern template class Field< 2, Vector4 >;
    extern template class Field< 3, Vector4 >;
    extern template class Field< 2, Matrix1 >;
    extern template class Field< 3, Matrix1 >;
    extern template class Field< 2, Matrix2 >;
    extern template class Field< 3, Matrix2 >;
    extern template class Field< 2, Matrix3 >;
    extern template class Field< 3, Matrix3 >;
    extern template class Field< 2, Matrix4 >;
    extern template class Field< 3, Matrix4 >;
    extern template class Field< 2, Color >;
    extern template class Field< 3, Color >;
    extern template class Field< 2, std::string >;
    extern template class Field< 3, std::string >;
}
