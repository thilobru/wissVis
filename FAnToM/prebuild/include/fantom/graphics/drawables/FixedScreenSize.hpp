#pragma once

#include "../Drawable.hpp"


namespace fantom
{
    namespace graphics
    {
        class FixedScreenSize : public Drawable
        {
        public:
            FixedScreenSize( std::shared_ptr< Drawable > child );

            // Override: Drawable
            virtual const BoundingSphere& boundingSphere() const override;
            virtual bool update( const RenderInfo& info ) override;
            virtual void draw( RenderState& state ) const override;
            virtual void traverseChildren( const ChildCallback& callback ) const override;

        private:
            float mScaleFactor;
            BoundingSphere mBoundingSphere;
            std::shared_ptr< Drawable > mChild;
        };
    } // namespace graphics
} // namespace fantom
