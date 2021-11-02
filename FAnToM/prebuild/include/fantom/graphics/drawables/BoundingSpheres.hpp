#pragma once

#include "../Drawable.hpp"


namespace fantom
{
    namespace graphics
    {
        class BoundingSpheres : public Drawable
        {
        public:
            BoundingSpheres( std::shared_ptr< Drawable > geometry,
                             const std::shared_ptr< const Drawable >& child,
                             bool recursive = false );

            // Override: Drawable
            virtual BoundingSphere const& boundingSphere() const override;
            virtual void draw( RenderState& state ) const override;

        private:
            std::shared_ptr< Drawable > mGeometry;
            std::weak_ptr< const Drawable > mChild;
            // bool mChildValid; // unused
            bool mRecursive;
        };
    } // namespace graphics
} // namespace fantom
