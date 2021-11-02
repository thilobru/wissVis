#pragma once

#include "../graphics2D.hpp"
#include "../utils.hpp"
#include <functional>
#include <memory>
#include <string>


namespace fantom
{
    namespace graphics2D_private
    {

        using namespace Graphics2D;

        class Graphics2DFactory
        {
        public:
            virtual ~Graphics2DFactory()
            {
            }

            virtual ImagePrivate* createImage( Image& image, size_t width, size_t height ) = 0;

            virtual GroupPrivate* createGroup( Group& itemGroup ) = 0;
            virtual PolygonPrivate* createPolygon( Polygon& polygon ) = 0;
            virtual SmoothPolygonPrivate* createSmoothPolygon( SmoothPolygon& polygon ) = 0;
            virtual PolylinePrivate* createPolyline( Polyline& polyline ) = 0;
            virtual SplinePrivate* createSpline( Spline& spline ) = 0;
            virtual TextPrivate* createText( Text& text ) = 0;
            virtual EllipsePrivate* createEllipse( Ellipse& ellipse ) = 0;
            virtual PixmapPrivate* createPixmap( Pixmap& pixmap ) = 0;
            virtual ClippingGroupPrivate* createClippingGroup( ClippingGroup& group ) = 0;
            virtual CustomItemPrivate* createCustomItem( CustomItem& item ) = 0;

            virtual ScenePainterPrivate* createScenePainter( ScenePainter& painter ) = 0;

            virtual Point2F measureTextSize( const std::string& text,
                                             size_t fontPixelSize,
                                             FontFamily family,
                                             bool bold,
                                             bool italic,
                                             bool underline )
                = 0;

            template < typename T >
            static auto getP( T& parent ) -> decltype( std::declval< T >().p() )
            {
                return parent.p();
            }
        };

        class ClipInfo
        {
        public:
            virtual ~ClipInfo()
            {
            }
        };

        class ImagePrivate
        {
        public:
            ImagePrivate( Image& pumpl )
                : mPumpl( &pumpl )
            {
            }

            virtual ~ImagePrivate()
            {
            }

            virtual void copy( const ImagePrivate* rhs ) = 0;

            virtual std::unique_ptr< Painter > painter() = 0;

            virtual Color getPixel( size_t x, size_t y ) const = 0;

            virtual void setPixel( size_t x, size_t y, Color color ) = 0;

            virtual void fill( Color color ) = 0;

            virtual void invert() = 0;

            virtual void resize( size_t newWidth, size_t newHeight ) = 0;

            virtual void resizeToWidth( size_t newWidth ) = 0;

            virtual void resizeToHeight( size_t newHeight ) = 0;

            virtual void crop( int x, int y, int w, int h ) = 0;

            virtual void rotate( float angle ) = 0;

            virtual std::pair< size_t, size_t > getSize() const = 0;

            virtual bool isValid() const = 0;

            virtual void loadFromFile( const std::string& filename ) = 0;

            virtual void saveToFile( const std::string& filename ) = 0;

            virtual void loadFromXPM( XPM pixmap ) = 0;

            virtual void renderScene( ScenePainter& scene ) = 0;

        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( Image )
            Image* mPumpl;
        };

        class ItemPrivate
        {
        public:
            ItemPrivate( Item& pumpl )
                : mPumpl( &pumpl )
            {
            }

            virtual ~ItemPrivate()
            {
            }

            virtual BoundingBox getBoundingBox() const = 0;

            virtual Point2F mapTo( const ItemPrivate* target, Point2F coords ) const = 0;

            virtual BoundingBox mapTo( const ItemPrivate* target, const BoundingBox& box ) const = 0;

            virtual Point2F getPosition() const = 0;

            virtual float getRotation() const = 0;

            virtual float getScale() const = 0;

            virtual Matrix3F getTransform() const = 0;

            virtual void setPosition( Point2F origin ) = 0;

            virtual void setRotation( float angle ) = 0;

            virtual void setScale( float scale ) = 0;

            virtual void setTransform( const Matrix3F& transform ) = 0;

            virtual void setVisible( bool visible ) = 0;

            virtual void setToolTip( const std::string& tooltip ) = 0;

            // public, so that conversion from private to public interface can be done in backend implementation
            F_GRAPHICS2D_DECLARE_PUBLIC( Item )

        protected:
            Item* mPumpl;
        };

        class GroupPrivate : public ItemPrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( Group )
        public:
            GroupPrivate( Group& pumpl )
                : ItemPrivate( pumpl )
            {
            }

            virtual void addToGroup( ItemPrivate* item ) = 0;

            virtual void removeFromGroup( ItemPrivate* item ) = 0;
        };

        class PolygonPrivate : public ItemPrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( Polygon )
        public:
            PolygonPrivate( Polygon& pumpl )
                : ItemPrivate( pumpl )
            {
            }

            virtual void setVertices( const std::vector< Point2F >& vertices ) = 0;

            virtual void setFillColor( Color color ) = 0;

            virtual void setLineColor( Color color ) = 0;

            virtual void setFillStyle( FillStyle style ) = 0;

            virtual void setLineStyle( float width, LineStyle style, LineEndStyle endStyle, LineJoinStyle joinStyle )
                = 0;
        };

        class SmoothPolygonPrivate : public PolygonPrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( SmoothPolygon )
        public:
            SmoothPolygonPrivate( SmoothPolygon& pumpl )
                : PolygonPrivate( pumpl )
            {
            }

            virtual void setControlVectors( const std::vector< Point2F >& vectors ) = 0;
        };

        class PolylinePrivate : public ItemPrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( Polyline )
        public:
            PolylinePrivate( Polyline& pumpl )
                : ItemPrivate( pumpl )
            {
            }

            virtual void setVertices( const std::vector< Point2F >& positions ) = 0;

            virtual void setLineColor( Color color ) = 0;

            virtual void setLineStyle( float width, LineStyle style, LineEndStyle endStyle, LineJoinStyle joinStyle )
                = 0;
        };

        class SplinePrivate : public PolylinePrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( Spline )
        public:
            SplinePrivate( Spline& pumpl )
                : PolylinePrivate( pumpl )
            {
            }

            virtual void setControlVectors( const std::vector< Point2F >& vectors ) = 0;
        };

        class TextPrivate : public ItemPrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( Text )
        public:
            TextPrivate( Text& pumpl )
                : ItemPrivate( pumpl )
            {
            }

            virtual void setColor( Color color ) = 0;

            virtual void setFontSize( size_t pixelSize ) = 0;

            virtual void setFont( FontFamily family, bool bold, bool italic, bool underline ) = 0;

            virtual void setText( const std::string& text ) = 0;
        };

        class EllipsePrivate : public ItemPrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( Ellipse )
        public:
            EllipsePrivate( Ellipse& pumpl )
                : ItemPrivate( pumpl )
            {
            }

            virtual void setCenter( Point2F center ) = 0;

            virtual void setSize( Point2F size ) = 0;

            virtual void setFillColor( Color color ) = 0;

            virtual void setLineColor( Color color ) = 0;

            virtual void setFillStyle( FillStyle style ) = 0;

            virtual void setLineStyle( float width, LineStyle style, LineEndStyle endStyle, LineJoinStyle joinStyle )
                = 0;
        };

        class PixmapPrivate : public ItemPrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( Pixmap )
        public:
            PixmapPrivate( Pixmap& pumpl )
                : ItemPrivate( pumpl )
            {
            }

            virtual void setImage( const ImagePrivate* image ) = 0;

            virtual void setBackgroundColor( const Color& color ) = 0;

            virtual void loadFile( const std::string& filename ) = 0;

            virtual void setXPM( XPM pixmap ) = 0;

            virtual void resize( Point2F size ) = 0;

            virtual void resizeToHeight( int height ) = 0;

            virtual void resizeToWidth( int width ) = 0;
        };

        class ClippingGroupPrivate : public GroupPrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( ClippingGroup )
        public:
            ClippingGroupPrivate( ClippingGroup& pumpl )
                : GroupPrivate( pumpl )
            {
            }

            virtual void setClipPolygon( const std::vector< Point2F >& positions ) = 0;
        };

        class CustomItemPrivate : public ItemPrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( CustomItem )
        public:
            CustomItemPrivate( CustomItem& pumpl )
                : ItemPrivate( pumpl )
            {
            }

            virtual void geometryChanged() = 0;

            virtual void requestUpdate() = 0;
        };

        class ScenePainterPrivate : public GroupPrivate
        {
        protected:
            F_GRAPHICS2D_DECLARE_PUBLIC( ScenePainter )

        public:
            ScenePainterPrivate( ScenePainter& pumpl )
                : GroupPrivate( pumpl )
            {
            }

            virtual Point2F getSceneCoordinates( int x, int y ) const = 0;

            virtual std::vector< Graphics2D::Item* > getItemsAtCoord( Point2F coord ) const = 0;

            virtual void storeToSVG( const std::string& filename, Size2D size ) = 0;
        };
    }
}
