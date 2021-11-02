#pragma once

#include "../Drawable.hpp"


namespace fantom
{
    namespace graphics
    {
        class Transform : public Drawable
        {
        public:
            Transform( std::shared_ptr< Drawable > child );

            Transform( const MatrixF< 4 >& matrix, std::shared_ptr< Drawable > child );

            void setMatrix( const MatrixF< 4 >& matrix );

            // Override: Drawable
            virtual const BoundingSphere& boundingSphere() const override;
            virtual bool update( const RenderInfo& info ) override;
            virtual void draw( RenderState& state ) const override;
            virtual void traverseChildren( const ChildCallback& callback ) const override;

        private:
            bool mChildChanged;
            BoundingSphere mBoundingSphere;

            MatrixF< 4 > mMatrix;
            std::shared_ptr< Drawable > mChild;
        };
    } // namespace graphics
} // namespace fantom
