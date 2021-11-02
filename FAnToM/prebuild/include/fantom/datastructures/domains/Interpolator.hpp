#pragma once

#include "EmbeddedCellComplex.hpp"
#include "impl/celltree.hpp"

#include <memory>

namespace fantom
{
    /// Base class for combining the output of an Interpolator into the final interpolation result.
    /**
     * Therefore, the values at the indices returned by the Interpolator have to be linearly combined using the
     * corresponding weights.
     *
     * \internal
     */
    struct Combiner
    {
        virtual ~Combiner()
        {
        }

        /**
         * Method called by the Interpolator to declare the indices and weights to be used for interpolation.
         * \param size  the number of values in index and weight
         * \param index index of the supporting point
         * \param weight weight of the supporting point
         */
        virtual void operator()( size_t size, const size_t index[], const double weight[] ) = 0;
    };

    /**
     * \brief Base class for interpolations.
     * Every interpolation basically determines a linear combination of values at
     * structure elements (points, cells or gauss points). This interpolator
     * returns the indices of the structure elements as well as the corresponding weights.
     *
     * \internal
     */
    template < size_t D >
    class Interpolator
    {
    public:
        /// Destroys evaluator.
        virtual ~Interpolator()
        {
        }

        /// Determines whether the current location is inside the domain.
        virtual explicit operator bool() const = 0;

        /// Sets the position of the interpolation.
        /**
         * \param p the requested point.
         */
        void reset( const Point< D >& p )
        {
            implReset( p );
        }

        /// Determines the value of the tensor field at the current location.
        /**
         * \param op Combiner that takes the index-weight-pairs for determining the result of the interpolation.
         * \pre *this (the current location is inside the domain).
         */
        void value( Combiner& op ) const
        {
#ifndef NDEBUG
            if( !*this )
            {
                throw fantom::out_of_range( "Evaluation at invalid position." );
            }
#endif
            implValue( op );
        }

    private:
        virtual void implReset( const Point< D >& ) = 0;
        virtual void implValue( Combiner& ) const = 0;
    };


    /**
     * \brief Base class for domains that allow for interpolation in the Euclidean space that they are embedded in.
     */
    template < size_t D >
    class InterpolatableDomain
    {
    public:
        virtual ~InterpolatableDomain() = default;

        /// Create an Interpolator for this domain.
        /**
         * \param domainType The part of the domain for which an interpolator should be constructed.
         * This matches the DomainPart that the Function that will be interpolated, is defined on.
         *
         * Throws an exception if no interpolator is available for the specified DomainPart.
         */
        virtual std::unique_ptr< Interpolator< D > > makeInterpolator( const DomainPart& domainType ) const = 0;
    };
}
