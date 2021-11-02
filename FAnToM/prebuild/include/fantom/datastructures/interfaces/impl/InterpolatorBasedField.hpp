#pragma once

#include "../../Function.hpp"
#include "../../domains/Interpolator.hpp"
#include "../Field.hpp"

namespace fantom
{
    namespace interface_detail
    {
        template < size_t D, typename T >
        class InterpolatorBasedField : public Field< D, T >
        {
            struct Combiner : fantom::Combiner
            {
                T& mValue;
                FunctionEvaluator< T >& mDiscreteEvaluator;

                Combiner( T& value, FunctionEvaluator< T >& evaluator )
                    : mValue( value )
                    , mDiscreteEvaluator( evaluator )
                {
                }

                void operator()( size_t size, const size_t index[], const double weight[] ) override
                {
                    LinearCombination< T > comb;
                    comb( mValue, size, mDiscreteEvaluator, index, weight );
                }
            };

            class Evaluator : public FieldEvaluator< D, T >
            {

            public:
                Evaluator( std::unique_ptr< Interpolator< D > > interpolator,
                           std::unique_ptr< FunctionEvaluator< T > > discreteEvaluator )
                    : FieldEvaluator< D, T >( discreteEvaluator->getTimeRange() )
                    , mInterpolator( move( interpolator ) )
                    , mDiscreteEvaluator( move( discreteEvaluator ) )
                {
                }

                explicit operator bool() const override
                {
                    return static_cast< bool >( *mInterpolator ) && static_cast< bool  >( *mDiscreteEvaluator );
                }

            private:
                void implReset( const Point< D >& p, double t ) override
                {
                    mDiscreteEvaluator->reset( t );
                    mInterpolator->reset( p );
                }

                T implValue() const override
                {
                    T value;
                    Combiner op( value, *mDiscreteEvaluator );
                    mInterpolator->value( op );
                    return value;
                }

                std::unique_ptr< Interpolator< D > > mInterpolator;
                std::unique_ptr< FunctionEvaluator< T > > mDiscreteEvaluator;
            };

        public:
            InterpolatorBasedField( const Function< T >& func )
                : mFunc( func )
                , mDomain( dynamic_cast< const InterpolatableDomain< D >& >( *func.domain() ) )
            {
            }

            std::unique_ptr< FieldEvaluator< D, T > > makeEvaluator() const override
            {
                return std::make_unique< Evaluator >( mDomain.makeInterpolator( mFunc.domainPart() ),
                                                      mFunc.makeEvaluator() );
            }

            TimeBehavior getTimeBehavior() const override
            {
                return mFunc.timeBehavior();
            }

        private:
            const Function< T >& mFunc;
            const InterpolatableDomain< D >& mDomain;
        };
    }
}
