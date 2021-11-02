#pragma once

#include "../Drawable.hpp"


namespace fantom
{
    namespace graphics
    {
        class HUDAnchor : public Drawable
        {
        public:
            enum class HAlignment
            {
                LEFT,
                CENTER,
                RIGHT,
            };

            enum class VAlignment
            {
                TOP,
                CENTER,
                BOTTOM,
            };

            HUDAnchor( HAlignment hAlignment,
                       VAlignment vAlignment,
                       const Vector2F& offset,
                       std::shared_ptr< Drawable > child,
                       bool relative = false );

            // Override: Drawable
            virtual const BoundingSphere& boundingSphere() const override;
            virtual bool update( const RenderInfo& info ) override;
            virtual void draw( RenderState& state ) const override;
            virtual void traverseChildren( const ChildCallback& callback ) const override;

        private:
            BoundingSphere mBoundingSphere;
            Vector3F mTranslation;

            HAlignment mHAlignment;
            VAlignment mVAlignment;
            Vector2F mOffset;
            bool mRelative;
            std::shared_ptr< Drawable > mChild;
        };
    } // namespace graphics
} // namespace fantom
