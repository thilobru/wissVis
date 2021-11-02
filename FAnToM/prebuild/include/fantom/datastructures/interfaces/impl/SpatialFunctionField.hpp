#pragma once

#include "../../Function.hpp"
#include "../Field.hpp"

namespace fantom
{
    namespace interface_detail
    {
        template < size_t D, typename T >
        class SpatialFunctionField : public Field< D, T >
        {
            struct Evaluator : public FieldEvaluator< D, T >
            {
                Evaluator( const SpatialFunction< D, T >& func )
                    : FieldEvaluator< D, T >( std::make_pair( std::numeric_limits< double >::lowest(),
                                                              std::numeric_limits< double >::max() ) )
                    , mFunc( func )
                {
                }

                void implReset( const Point< D >& p, double t ) override
                {
                    mPos = p;
                    mT = t;
                }

                T implValue() const override
                {
                    return mFunc( mPos, mT );
                }

                explicit operator bool() const override
                {
                    return true;
                }

                const SpatialFunction< D, T >& mFunc;
                Point< D > mPos;
                double mT;
            };

        public:
            SpatialFunctionField( const SpatialFunction< D, T >& func )
                : mFunc( func )
            {
            }

            std::unique_ptr< FieldEvaluator< D, T > > makeEvaluator() const override
            {
                return std::make_unique< Evaluator >( mFunc );
            }

            TimeBehavior getTimeBehavior() const override
            {
                return TimeBehavior::unsteady;
            }

        private:
            const SpatialFunction< D, T >& mFunc;
        };
    }
}
