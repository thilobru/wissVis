// TODO: Currently, the add and remove methods of the DrawableCompound directly insert/remove drawables from the child
// set. This is not consistent with the concept of Drawables only being changed in the update method. The
// insertion/removal process should thus be replaced by a synchronized change list that performs cached changes in the
// update method.

#pragma once

#include "../Drawable.hpp"

#include <memory>
#include <unordered_set>


namespace fantom
{
    namespace graphics
    {
        class DrawableCompound : public Drawable
        {
        public:
            DrawableCompound();

            template < typename TIterator >
            DrawableCompound( TIterator first, TIterator last );

            void add( std::shared_ptr< Drawable > drawable );

            void remove( const std::shared_ptr< Drawable >& drawable );

            // Override: Drawable
            virtual const BoundingSphere& boundingSphere() const override;
            virtual bool update( const RenderInfo& info ) override;
            virtual void draw( RenderState& state ) const override;
            virtual void traverseChildren( const ChildCallback& callback ) const override;

        private:
            void computeBoundingSphere();

            /// Flag indicating whether new children were added or old ones removed.
            /// It is used to signal bounding sphere computation in the update method.
            bool mChildrenChanged;

            BoundingSphere mBoundingSphere;
            std::unordered_set< std::shared_ptr< Drawable > > mDrawables;
        };

        std::shared_ptr< DrawableCompound >
        makeCompound( std::initializer_list< std::shared_ptr< Drawable > > drawables );

        std::shared_ptr< DrawableCompound > makeCompound( const std::vector< std::shared_ptr< Drawable > >& drawables );
    } // namespace graphics
} // namespace fantom


namespace fantom
{
    namespace graphics
    {
        template < typename TIterator >
        DrawableCompound::DrawableCompound( TIterator first, TIterator last )
            : mChildrenChanged( true )
            , mDrawables( first, last )
        {
        }
    } // namespace graphics
} // namespace fantom
