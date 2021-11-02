#pragma once

#include <fantom/graphics.hpp>
#include <fantom/math.hpp>

#include <fantom/registry/plugins.hpp>

#include "Font.hpp"
#include "Helpers.hpp"

#include <mutex>


namespace fantom
{
    namespace graphics
    {

        /// the ObjectRenderer is a factory for high level drawables \ingroup gfx
        /**
         */
        class ObjectRenderer
        {
        public:
            /// Enums for the object types used in the reserve( ... ) function.
            /**
             *
             */
            enum class ObjectType
            {
                SPHERE,
                CONE,
                CYLINDER,
                ELLIPSOID,
                ARROW,
                ARROW_VOLUME,
                TEXT_LABEL
            };

            /// Creates the ObjectRenderer
            /**
             * \param system the reference to the GraphicsSystem singleton ( In VisAlgorithm: auto const& system =
             graphics::GraphicsSystem::instance(); )
            */
            ObjectRenderer( graphics::GraphicsSystem const& system, ShadingType type = ShadingType::PHONG );

            ObjectRenderer& setShadingType( ShadingType type )
            {
                mShadingType = type;
                return *this;
            }

            /// Reserve memory for increased perforemence when adding many objects.
            void reserve( ObjectType type, size_t maxElements );

            /// Adds an arrow to this primitive.
            /**
             * \param center the position of the tail of the arrow.
             * \param direction the direction of the arrow.
             * \param thickness the thickness of the arrow.
             * \param color the color of the arrow.
             * \param drawWithVolume true if you want to draw a volumentric arrow.
             */
            template < typename S, typename T >
            ObjectRenderer& addArrow(
                Tensor< T, 3 > center, Tensor< T, 3 > direction, S thickness, Color color, bool drawWithVolume );

            /// Adds a cone to this primitive.
            /**
             * \param center the center of the cone.
             * \param direction the direction of the cone.
             * \param radiusBottom the radius of the cone's base face.
             * \param radiusTop the radius of the cone's top face.
             * \param color the color of the cone.
             */
            template < typename S, typename T >
            ObjectRenderer&
                addCone( Tensor< T, 3 > center, Tensor< T, 3 > direction, S radiusTop, S radiusBottom, Color color );

            /// Adds a cylinder to this primitive.
            /**
             * \param center the center of the cylinder.
             * \param direction the direction of the cylinder.
             * \param radius the radius of the cylinder's base face.
             * \param color the color of the cylinder.
             */
            template < typename S, typename T >
            ObjectRenderer& addCylinder( Tensor< T, 3 > center, Tensor< T, 3 > direction, S radius, Color color );

            /// Adds an ellipsoid to this primitive.
            /**
             * \param center the center of the ellipsoid.
             * \param rotation the quaternion respresenting the rotation.
             * \param size the extent of the radius of the different ellipsoids' axes.
             * \param color the color of the ellipsoid.
             */
            template < typename T >
            ObjectRenderer& addEllipsoid( Tensor< T, 3 > const& center,
                                          Quaternion< T > const& rotation,
                                          Tensor< T, 3 > const& size,
                                          Color const& color );

            /// Adds an ellipsoid to this primitive.
            /**
             * \param center the center of the ellipsoid.
             * \param first,second,third vectors defining direction and radius of the different ellipsoid's axes.
             * \param color the color of the ellipsoid.
             */
            template < typename T >
            ObjectRenderer& addEllipsoid( Tensor< T, 3 > const& center,
                                          Tensor< T, 3 > const& first,
                                          Tensor< T, 3 > const& second,
                                          Tensor< T, 3 > const& third,
                                          Color const& color );

            /// Adds a sphere to this primitive.
            /**
             * \param center the center of the sphere.
             * \param radius the radius of the sphere.
             * \param color the color of the sphere.
             */
            template < typename S, typename T >
            ObjectRenderer& addSphere( Tensor< T, 3 > center, S radius, Color color );

            /// Adds a text label as a billboard.
            /**
             * \param center the center of the billboard.
             * \param label the label's content.
             * \param size the label's size.
             * \param color the color in which the text will be rendered
             * \param fontfamily the font family
             * \param emphasis the font's emphasis
             * \param alignment the location of the anchor point of the text
             */
            template < typename S, typename T >
            ObjectRenderer& addTextLabel( const Tensor< T, 3 >& center,
                                          const std::string& label,
                                          S size,
                                          const Color& color,
                                          Font::FontAlignment alignment = Font::FontAlignment::Center,
                                          Font::FontFamily fontfamily = Font::FontFamily::SansSerif,
                                          Font::FontEmphasis emphasis = Font::FontEmphasis::Regular );

            /// Generates the Drawable from the added Objects. This finalizes the process.
            /**
             */
            std::shared_ptr< graphics::DrawableCompound > commit();


            void setLineWidth( float width );

        private:
            void clearBufferData();

            // Buffers for line arrows
            std::vector< PointF< 3 > > mArrowPositions;
            std::vector< unsigned int > mArrowIndices;
            std::vector< Color > mArrowColors;
            float mLineWidth;
            bool uniformArrowColor;

            // Buffers for spheres
            std::vector< PointF< 3 > > mSpherePositions;
            std::vector< float > mSphereRadii;
            std::vector< Color > mSphereColors;
            bool uniformSphereColor;

            // Buffers for ellipsoids
            std::vector< PointF< 3 > > mEllipsoidPositions;
            std::vector< Tensor< float, 3 > > mEllipsoidRadii;
            std::vector< Tensor< float, 4 > > mEllipsoidRotations;
            std::vector< Color > mEllipsoidColors;
            std::vector< float > mBoundingRadiiEllipsoids;
            bool uniformEllipsoidColor;

            // Buffers for cones
            std::vector< PointF< 3 > > mConePositions;
            std::vector< VectorF< 3 > > mConeNormals;
            std::vector< Tensor< float, 3 > > mConeRadiiAndLength;
            std::vector< Color > mConeColors;
            std::vector< float > mBoundingRadiiCones;
            bool uniformConeColor;

            // Buffers for cylinders
            std::vector< PointF< 3 > > mCylinderPositions;
            std::vector< VectorF< 3 > > mCylinderNormals;
            std::vector< Tensor< float, 2 > > mCylinderRadiusAndLength;
            std::vector< Color > mCylinderColors;
            std::vector< float > mBoundingRadiiCylinders;
            bool uniformCylinderColor;

            // Textlabels
            std::vector< std::shared_ptr< graphics::Drawable > > mTextLabels;

            std::string mResPath;
            fantom::graphics::GraphicsSystem const& mSystem;

            ShadingType mShadingType;

            std::mutex mMutex;
        };
    } // namespace graphics
} // namespace fantom
