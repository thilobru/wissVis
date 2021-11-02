#pragma once

#include "../math.hpp"

namespace fantom
{
    namespace graphics
    {
        class BoundingSphere
        {
        public:
            /// Constructs an empty bounding sphere at the origin
            BoundingSphere();

            /// Constructs a bounding sphere enclosing all bounding spheres in range [first,last)
            template < typename TIter >
            BoundingSphere( TIter first, TIter last );

            BoundingSphere( VectorF< 3 > const& center, float radius );

            VectorF< 3 > const& center() const;
            float radius() const;

            /// Expands the bounding sphere such that it encloses the child
            void expand( BoundingSphere const& child );

            bool valid() const;

        private:
            VectorF< 3 > mCenter;
            float mRadius;
        };


        std::ostream& operator<<( std::ostream& stream, BoundingSphere const& boundingSphere );
    } // namespace graphics
} // namespace fantom


namespace fantom
{
    namespace graphics
    {
        inline BoundingSphere::BoundingSphere()
            : mCenter{ 0.0f, 0.0f, 0.0f }
            , mRadius( -1.0f )
        {
        }

        template < typename TIter >
        BoundingSphere::BoundingSphere( TIter first, TIter last )
        {
            if( first == last )
            {
                mCenter = { 0.0f, 0.0f, 0.0f };
                mRadius = -1.0f;
            }
            else
            {
                mCenter = first->center();
                mRadius = first->radius();
                while( std::advance( first ) != last )
                    expand( *first );
            }
        }

        inline BoundingSphere::BoundingSphere( VectorF< 3 > const& center, float radius )
            : mCenter( center )
            , mRadius( radius )
        {
        }

        inline VectorF< 3 > const& BoundingSphere::center() const
        {
            return mCenter;
        }

        inline float BoundingSphere::radius() const
        {
            return mRadius;
        }
    } // namespace graphics
} // namespace fantom
