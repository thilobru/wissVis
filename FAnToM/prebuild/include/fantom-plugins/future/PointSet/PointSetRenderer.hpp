#pragma once

#include <fantom-plugins/utils/Graphics/ObjectRenderer.hpp>
#include <fantom/dataset.hpp>
#include <fantom/graphics.hpp>

namespace fantom
{
    namespace graphics
    {
        class PointSetRenderer
        {
        public:
            PointSetRenderer( const graphics::GraphicsSystem& system )
                : mSphereRadius( 1 )
                , mSystem( system )
            {
            }

            PointSetRenderer& setShadingType( ShadingType type )
            {
                mShadingType = type;
                return *this;
            }

            PointSetRenderer& setPointPositions( std::shared_ptr< const PointSetBase > domain )
            {
                applyFor< All_PointSet >( *domain, [this]( auto& d ) { this->setPointPositions( d ); } );
                return *this;
            }

            template < size_t D >
            PointSetRenderer& setPointPositions( const PointSet< D >& domain )
            {
                mPointPositions.resize( domain.points().size() );
                for( size_t ptIdx = 0; ptIdx < domain.points().size(); ++ptIdx )
                {
                    mPointPositions[ptIdx] = toPoint3( domain.points()[ptIdx] );
                }
                return *this;
            }

            PointSetRenderer& setSphereRadii( double radius )
            {
                mSphereRadius = radius;
                return *this;
            }

            std::shared_ptr< graphics::DrawableCompound > renderWithColors( std::vector< Color > colors ) const
            {
                auto renderer = std::make_shared< graphics::ObjectRenderer >( mSystem );
                renderer->setShadingType( mShadingType );
                renderer->reserve( graphics::ObjectRenderer::ObjectType::SPHERE, mPointPositions.size() );

                for( size_t ptIdx = 0; ptIdx < mPointPositions.size(); ++ptIdx )
                {
                    renderer->addSphere( mPointPositions[ptIdx], mSphereRadius, colors[ptIdx] );
                }
                return renderer->commit();
            }

            std::vector< Point< 3 > >& getPointPositions()
            {
                return mPointPositions;
            }

        private:
            ShadingType mShadingType;
            std::vector< Point< 3 > > mPointPositions;
            double mSphereRadius;
            const fantom::graphics::GraphicsSystem& mSystem;
        };
    } // namespace graphics
} // namespace fantom
