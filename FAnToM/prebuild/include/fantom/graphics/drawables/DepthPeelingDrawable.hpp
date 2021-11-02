#pragma once

#include "../Drawable.hpp"
#include "../FrameBuffer.hpp"
#include "../System.hpp"
#include "../Texture.hpp"


namespace fantom
{
    namespace graphics
    {
        /**
         * DepthPeelingDrawable implements the Front to Back Depth Peeling algorithm described in:
         * Cass Everitt, “Interactive order-independent transparency”, Technical report, NVIDIA Corporation, 2001.
         *
         * The Drawable does a defined number of render passes on a child Drawable. Each pass extracts a
         * "depth layer" from the child, where each layer consists of the fragments that where hidden by the previous
         * layer.
         *
         * @note Multiple DepthPeelingDrawables in a scene don't work together. This means if a scene contains multiple
         * DepthPeelingDrawables they do not share depth information of their individual child drawables.
         * Also should DepthPeelingDrawables not be children of each other, since then the ChildDepthPeelingDrawable
         * would do n times m render calls (where n and m are the number of depth layers used for the parent and child
         * DepthPeelingDrawables).
         * In conclusion it is advised that a scene only contains a single DepthPeelingDrawable which contains all
         * other Drawables from the scene.
         */
        class DepthPeelingDrawable : public Drawable
        {
        public:
            /**
             * Initializes a new DepthPeelingDrawable optionally defining a
             * @param child A Drawable that shall be rendered using the Depth Peeling algorithm.
             * @param depth A number of depth layers that indicates the number of depth layers to extract.
             */
            explicit DepthPeelingDrawable(std::shared_ptr< Drawable > child, size_t depth = 8 );

            /**
             * Returns the bounding sphere enclosing the geometry of child of the DepthPeelingDrawable.
             */
            BoundingSphere const& boundingSphere() const override;

            /**
             * Updates the framebuffer, color- and depth textures to the correct target size.
             * @param info A fantom::graphics RenderInfo.
             */
            bool update( const RenderInfo& info ) override;

            /**
             * Renders the depth peeling drawable.
             * @param state A fantom::graphics RenderState.
             */
            void draw( RenderState& state ) const override;

            /**
             * Runs a user defined function on each child in the child Drawable tree.
             * @param callback A ChildCallback.
             */
            void traverseChildren( const ChildCallback& callback ) const override;

            /**
             * Sets the number of depth layers that are extracted during a draw call.
             * @note The DepthPeelingDrawable must be redrawn in order for the new depth layers to be visible.
             * @param depth An unsigned long greater zero indicating the number of depth layers to extract.
             */
            void setDepth( size_t depth );

            /**
             * Get the number of depth layers the DepthPeelingDrawable extracts.
             */
            size_t getDepth() const;

            static std::shared_ptr<DepthPeelingDrawable> GlobalDepthPeelingDrawable;

        private:
            /**
             * The child Drawable that is to be rendered using the depth peeling algorithm.
             */
            std::shared_ptr< Drawable > mChild;

            /**
             * An unsigned long greater zero indicating the number of depth layers to extract.
             */
            size_t mDepth;

            /**
             * The framebuffer which is used as render target to render the child Drawable multiple times.
             */
            std::shared_ptr< FrameBuffer > mFrameBuffer;

            /**
             * A vector to store the extracted depth layers. The size of the vector is defined by the number of depth
             * layers.
             * The vector is redefined when the number of depth layers changes.
             */
            std::vector< std::shared_ptr< Texture2D > > mColorTextures;

            /**
             * An array of two textures. Both textures hold the depth information of the previous rendered
             * layer. It is used to discard fragments that do not belong to the current depth layer.
             */
            std::array< std::shared_ptr< Texture2D >, 2 > mDepthTextures;
        };
    } // namespace graphics
} // namespace fantom
