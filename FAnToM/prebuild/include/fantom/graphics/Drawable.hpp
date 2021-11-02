#pragma once

#include "BoundingSphere.hpp"
#include "RenderInfo.hpp"
#include "RenderState.hpp"
#include "common.hpp"

#include <functional>


namespace fantom
{
    namespace graphics
    {
        /**
         * Basic concept of a drawable object in our graphics engine.
         * It has to advertise the bounding sphere of its visible geometry,
         * so that camera clipping can be estimated correctly.
         * The draw call has to consider the render state, and use output FrameBufferS
         * correctly, if available.
         */
        class Drawable
        {
        public:
            virtual ~Drawable();

            /**
             * Getter for the bounding sphere enclosing the Drawable.
             *
             * This method is const so the caller can expect it to return quickly. In general, bounding sphere
             * computation should be performed in the update method.
             *
             * \return The current bounding sphere
             */
            virtual const BoundingSphere& boundingSphere() const = 0;

            /**
             * Traverse the Drawable tree and perform potential geometry updates.
             *
             * This method also reports back whether the geometry has been changed, potentially triggering bounding
             * sphere computation.
             *
             * Note: If in the future, more detailed information about the change is required, the bool could be
             * replaced with a struct containing additional information.
             *
             * \param info Information about the current drawing context, e.g., size, window id, etc.
             * \return Indicator whether the geometry bounds have changed
             */
            virtual bool update( const RenderInfo& info );

            /**
             * Rendering traversal of the Drawable tree.
             *
             * \param state A structure containing information about the rendering context, such as the render target,
             * uniforms, textures, etc.
             */
            virtual void draw( RenderState& state ) const = 0;

            /// The callback function used for traversing child drawables. The return value is used to stop the
            /// traversal process early, e.g., when the caller has found the drawable he was looking for.
            using ChildCallback = std::function< bool( const Drawable& ) >;

            /**
             * Traverse all children of the current drawable.
             *
             * The default implementation assumes that there are not child drawables to simplify the creation of custom
             * leaf drawables.
             *
             * \param callback A callback function that gets called for every child drawable.
             */
            virtual void traverseChildren( const ChildCallback& callback ) const;
        };
    } // namespace graphics
} // namespace fantom
