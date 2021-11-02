#pragma once

#include "../Drawable.hpp"

#include <functional>
#include <memory>


namespace fantom
{
    namespace graphics
    {
        /**
         * This Drawable takes a callback that is called in every update cycle and allows the on-the-fly modification of
         * a wrapped child. This allows for easy implementation of dynamic drawables without the need of custom classes.
         */
        class CallbackWrapper : public Drawable
        {
        public:
            using Callback = std::function< void( std::shared_ptr< Drawable >& ) >;

            CallbackWrapper( Callback callback );

            CallbackWrapper( std::shared_ptr< Drawable > child, Callback callback );

            // Override: Drawable
            virtual const BoundingSphere& boundingSphere() const override;
            virtual bool update( const RenderInfo& info ) override;
            virtual void draw( RenderState& state ) const override;
            virtual void traverseChildren( const ChildCallback& callback ) const override;

        private:
            std::shared_ptr< Drawable > mChild;
            Callback mCallback;
        };
    } // namespace graphics
} // namespace fantom
