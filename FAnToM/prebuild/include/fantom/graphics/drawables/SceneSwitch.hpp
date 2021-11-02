#pragma once

#include "../Drawable.hpp"

#include <unordered_map>


namespace fantom
{
    namespace graphics
    {
        /**
         * The SceneSwitch Drawable displays different sub-Drawables for every window it is rendered in.
         * It remembers the window of the last traversal and reports a geometry change in case it is drawn to a
         * different window. An usage example is different HUD information, such as camera type, shown for the different
         * windows.
         */
        class SceneSwitch : public Drawable
        {
        public:
            void setChild( uint32_t windowId, std::shared_ptr< Drawable > drawable );

            // Override: Drawable
            virtual const BoundingSphere& boundingSphere() const override;
            virtual bool update( const RenderInfo& info ) override;
            virtual void draw( RenderState& state ) const override;
            virtual void traverseChildren( const ChildCallback& callback ) const override;

        private:
            std::shared_ptr< Drawable > mCurrentChild;
            std::unordered_map< uint32_t, std::shared_ptr< Drawable > > mChildren;
        };
    } // namespace graphics
} // namespace fantom
