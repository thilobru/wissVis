#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "events.hpp"
#include "math.hpp"
#include "utils.hpp"

namespace fantom
{
    using XPM = const char* const*;

    class SceneView2D;

    namespace graphics2D_private
    {
        class Graphics2DFactory;
        class ClipInfo;
        class ImagePrivate;
        class ItemPrivate;
        class GroupPrivate;
        class ScenePainterPrivate;
        class PolygonPrivate;
        class SmoothPolygonPrivate;
        class PolylinePrivate;
        class SplinePrivate;
        class TextPrivate;
        class EllipsePrivate;
        class PixmapPrivate;
        class ClippingGroupPrivate;
        class CustomItemPrivate;

        template < class Item >
        class ItemHolder
        {
        public:
            void addChild( Item* child )
            {
                mChilds.push_back( child );
            }
            void removeChild( Item* child )
            {
                auto it = std::find( mChilds.begin(), mChilds.end(), child );
                if( it != mChilds.end() )
                {
                    mChilds.erase( it );
                }
            }
            std::list< Item* > mChilds;
        };
    }

    namespace Graphics2D
    {
        /// bounding box in terms of left upper position (std::pair::first) and size (std::pair::second) \ingroup GUI
        /**
         * In terms of the GUI, the origin is on
         * the upper right corner. Therefore, for
         * a valid bounding box, lower() > upper()
         * and right() > left().
         */
        class BoundingBox : public std::pair< Point2F, Vector2F >
        {
        public:
            /// constructs an empty bounding box
            BoundingBox();

            /// construct a bounding box where box.first is the origin and box.second the size
            BoundingBox( const std::pair< Point2F, Vector2F >& box );

            /// check whether the bouding box is empty
            bool isEmpty() const;

            /// returns the upper left corner
            Point2F upperLeft() const;

            /// returns the lower right corner
            Point2F lowerRight() const;

            /// returns the upper right corner
            Point2F upperRight() const;

            /// returns the lower left corner
            Point2F lowerLeft() const;

            /// returns the size in as width and height
            Vector2F size() const;

            /// returns the left boundary
            float left() const;

            /// returns the top boundary
            float top() const;

            /// returns the width of the box
            float width() const;

            /// returns the height of the box
            float height() const;

            /// returns the bottom boundary
            float bottom() const;

            /// returns the right boundary
            float right() const;

            /// returns the true if the box contains the given point
            bool contains( const Point2F& pos ) const;

            /// Check whether bounding boxes intersect
            bool intersects( const BoundingBox& bb ) const;

            BoundingBox& operator=( const std::pair< Point2F, Point2F >& box );
        };

        /// Style hint how to fill shapes
        enum class FillStyle
        {
            Solid,         /// uniform color fill
            Empty,         /// no fill
            Horizontal,    /// horizontal line fill
            Vertical,      /// vertical line fill
            Cross,         /// both vertical and horizontal lines
            DiagonalUp,    /// diagonal line fill from bottom left to top right
            DiagonalDown,  /// diagonal line fill from top left to bottom right
            DiagonalCross, /// both up- and downwards diagonal line fill
            Dotted,        /// sparse dotted fill
            Shaded         /// half dense fill
        };

        /// Style hint how to draw lines
        enum class LineStyle
        {
            Solid,  /// plain line
            Empty,  /// no line
            Dash,   /// dashed line
            Dot,    /// dotted line
            DashDot /// alternate dashes and dots
        };

        /// Style hint how to draw the end of a line
        enum class LineEndStyle
        {
            Flat, /// flat, unrounded end of the line
            Round /// rounded line end
        };

        /// Style hint how to draw the area where two conescutive line elements join
        enum class LineJoinStyle
        {
            Sharp, /// sharp corner
            Flat,  /// beveled corner
            Round  /// rounded corner (i.e., a circular arc between the lines)
        };

        /// Style hint how to render text
        enum class FontFamily
        {
            Serif,      /// serif fonts like times new roman
            SansSerif,  /// sans serif fonts like Helvetica
            TypeWriter, /// monospace fonts like Courier New
            Default     /// unspecified. The system picks its default font.
        };

        /// Alignment hint for text drawing
        enum class Alignment
        {
            Left = 0,
            Middle = 1,
            Right = 2,
            Bottom = 0,
            Top = 2
        };

        Point2F measureTextSize( const std::string& text,
                                 size_t fontPixelSize = 20,
                                 FontFamily family = FontFamily::Default,
                                 bool bold = false,
                                 bool italic = false,
                                 bool underline = false );

        //=========================================================================================================================

        class Image;
        class ScenePainter;

        /// Interface for painters on images
        /**
         * Coordinate system: (0, 0) represents the upper left corner. X and Y extend right and down.
         *
         * All operations are using floating point precision.
         * Antialiasing is performed to get a smooth drawing.
         */
        class Painter
        {
        public:
            virtual ~Painter()
            {
            }

            /// RAII class that represents that the Painter is clipped.
            /**
             * The clip is nullified when the Clip instance is destroyed.
             * This implies that the Clip instance must be destroyed before the Painter (otherwise segfaults).
             * Also, it is unspecified behavior (read: it WILL lead to problems! For sure!)
             * to destroy multiple Clip instaces in anything other than their reverse instantiation
             * order. This should be hard to accomplish by the way, because these instances
             * are created on the stack by the Painter::clipToRegion(...) function.
             */
            class Clip
            {
            public:
                ~Clip();
                Clip() = delete;
                Clip( const Clip& ) = delete;
                Clip( Clip&& );

                Clip& operator=( const Clip& ) = delete;
                Clip& operator=( Clip&& ) = delete;

            private:
                Clip( Painter& painter, std::unique_ptr< graphics2D_private::ClipInfo > originalClip );

                Painter& mPainter;
                std::unique_ptr< graphics2D_private::ClipInfo > mOriginalClip;
                friend class Painter;
            };

            /**
             * define whether anti-aliasing should be enabled or not
             * The default is that it is enabled
             */
            virtual void setAntialiasing( bool /* on */ )
            {
            } //< default to ignore this feature

            /**
             * define whether draw operations always clear the drawing behind them
             * (effectively re-setting the drawing to transparent and then performing a
             *  draw operation) or not (default).
             */
            virtual void setDiscardBackground( bool /* on */ )
            {
            } //< default to ignore this feature

            /// sets the background color
            virtual void setFillColor( Color color ) = 0;

            /// sets the foreground color (used for lines and text)
            virtual void setLineColor( Color color ) = 0;

            /// sets the line style. Using \a width == 0 creates a line, that is exactly one pixel wide.
            virtual void setLineStyle( float width = 0,
                                       LineStyle style = LineStyle::Solid,
                                       LineEndStyle endStyle = LineEndStyle::Round,
                                       LineJoinStyle joinStyle = LineJoinStyle::Round )
                = 0;

            /// sets the style of fillings
            virtual void setFillStyle( FillStyle style = FillStyle::Solid ) = 0;

            /// sets the font size
            virtual void setFontSize( size_t pixelSize = 20 ) = 0;

            /// sets the font style
            virtual void setFontStyle( FontFamily family = FontFamily::Default,
                                       bool bold = false,
                                       bool italic = false,
                                       bool underline = false )
                = 0;

            /// draws an ellipse
            /// \a angle is in radians (counter-clockwise)
            virtual void ellipse( Point2F center, float width, float height, float angle = 0.0f ) = 0;

            /// draws an image into the painting
            virtual void image( Point2F upperLeft, const Image& image ) = 0;

            /// draws a straight line
            virtual void line( Point2F start, Point2F end ) = 0;

            /// draws a straight line-strip
            virtual void lines( const std::vector< Point2F >& positions ) = 0;

            /// draws a dot
            virtual void point( Point2F point ) = 0;

            /// draws a polygon.
            /// \a vertices are the vertices of the polygon, without specifying the first vertex twice.
            virtual void polygon( const std::vector< Point2F >& vertices ) = 0;

            /// draws an axis-parallel rectangle.
            virtual void rectangle( const Point2F& upperLeft, const Point2F& size ) = 0;

            /// draws a spline between two points
            virtual void spline( Point2F start, Vector2F startDir, Vector2F endDir, Point2F end ) = 0;

            /// draws a text.
            /// \a angle is in radians (counter-clockwise)
            /// Alignments are specified relative to the text, i.e., horizontal alignment translates between beginning
            /// and end of text, regardless of the rotation.
            virtual void text( Point2F position,
                               const std::string& text,
                               float angle,
                               Alignment horizontalAlignment = Alignment::Left,
                               Alignment verticalAlignment = Alignment::Bottom )
                = 0;

            /// draws a text (non-rotated).
            virtual void text( Point2F position,
                               const std::string& text,
                               Alignment horizontalAlignment = Alignment::Left,
                               Alignment verticalAlignment = Alignment::Bottom )
                = 0;

            /// Clips the painter to the specified region. Only integral rectangle coordinates are supported.
            /**
             * If multiple clip regions are created simultaneously, or the painter was initially clipped,
             * the clip regions intersect each other.
             * Note: the clipping might not be implemented by specific Painter implementations.
             */
            Clip clipToRegion( Pos2D upperLeft, Size2D size );

        private:
            /// intersects the current clip region with the specified rectangle. Returns the original clip region for
            /// restoring later on.
            virtual std::unique_ptr< graphics2D_private::ClipInfo > intersectClipRegion( Pos2D upperLeft, Size2D size );
            /// restores a clip region.
            virtual void resetClipRegion( std::unique_ptr< graphics2D_private::ClipInfo > clip );
        };

        //=========================================================================================================================

        /// Pixel images
        /**
         * @brief An image consists of \a width * \a height pixels.
         * Thereby, (0, 0) represents the upper left corner and (width-1, height-1) the lower right corner.
         * Every pixel is assigned a color that can be changed by the \c setPixel() function.
         */
        class Image
        {
            graphics2D_private::ImagePrivate* mPimpl;
            F_GRAPHICS2D_DECLARE_PRIVATE( Image )

        public:
            /// Construct a new, all transparent image with given dimensions
            Image( size_t width, size_t height ); // default constructor

            /// Copy-constructs a new image from a given image
            Image( const Image& image ); // copy constructor

            /// Move constructor to grab contents of a given image. Afterwards, \c image is invalid, but destructible.
            Image( Image&& image ); // move constructor

            /// Assignment operator
            Image& operator=( Image image ); // assignment operator (copy-and-swap-idiom)

            ~Image();

            /// Returns a painter for performing high-level drawing operations on the image
            std::unique_ptr< Painter > painter();

            /// Returns the color value for the given pixel
            /**
             * @return transparent black, if pixel is out of range
             */
            Color getPixel( size_t x, size_t y ) const;

            /// Sets the color value for the given pixel
            /**
             * The operation is ignored for pixels out of range
             */
            void setPixel( size_t x, size_t y, Color color );

            /// Fill the image uniformely with the given color
            void fill( Color color );

            /// Invert the image pixel-wise. I.e., every color component p is re-set to (1-p). Thereby, the alpha
            /// channel is not changed.
            void invert();

            /// Multiply every pixel's color with \a color. Useful for colorizing grayscale images.
            void multiply( Color color );

            /// Resizes the image to new dimensions
            /**
             * This operation performs smooth scaling.
             * If the image was empty beforehand, an all-transparent image with the given dimensions is created
             */
            void resize( size_t newWidth, size_t newHeight );

            /// Resizes the image to new width.
            /**
             * This operation performs smooth scaling.
             * If the image was empty beforehand, an all-transparent image with the size \a newWidth * 1 is created
             */
            void resizeToWidth( size_t newWidth );

            /// Resizes the image to new height.
            /**
             * This operation performs smooth scaling.
             * If the image was empty beforehand, an all-transparent image with the size 1 * \a newHeight is created
             */
            void resizeToHeight( size_t newHeight );

            /// Crops image at point (x,y) to dim (width, height)
            void crop( int x, int y, int w, int h );

            /// Convenience function for resizing the image to a given width without changing its height.
            void stretchToWidth( size_t newWidth );

            /// Convenience function for resizing the image to a given height without changing its width.
            void stretchToHeight( size_t newHeight );

            /// Rotates the image counter-clockwise.
            /// The angle is given in radians.
            void rotate( float angle );

            /// Determines whether the image is valid or empty (size 0*0)
            bool isEmpty() const;

            /// Determines whether the image is valid  for the specific gui implementation. Defaults to not empty in
            /// most cases.
            bool isValid() const;

            /// Returns the current size of the image
            std::pair< size_t, size_t > getSize() const;

            /// Renders a graphics scene into this image.
            /// Note: using a ScenePainter here that has been put into a SceneView2D, is undefined behavior!
            void renderScene( ScenePainter& scene );

            /// Load an image from the given file
            static Image loadFromFile( const std::string& filename );

            /// Create an image from the given pixmap
            static Image fromXPM( XPM pixmap );

            /// Save an image to the given file
            void saveToFile( const std::string& filename );

        private:
            friend class Pixmap;
        };

        //=========================================================================================================================

        class Group;

        /// base class for 2D graphics items
        /**
         * \par Coordinate System
         * Every item has its own coordinate system. This coordinate system is positioned inside the parent coordinate
         *system
         * according to the position, rotation and scale settings as well as the general transformation matrix of this
         *item.
         * The local coordinate system is a 2D system that corresponds to screen coordinates in the untransformed state
         *(i.e.,
         * only default transformations in this item as well as all parents). Thereby, the x-axis is horizontal
         *(positive
         * direction to the right) and y-axis is vertical (positive direction downwards).
         *
         * \par Stacking Order
         * All items are drawn in a stable order. Thus, it is well defined, what are the top-most items (optically).
         * In general, later added items overdraw previously added items.
         * Also, child items overdraw parent items. Since the only possible parent item is a group, which does not draw
         *anything itself, this is
         * not very important for the resulting drawing. However, it is important logically, because the ScenePainter
         *returns items
         * at a given position in the stacking order from top to bottom. So, the item group is always listed \b after
         *its child items there.
         *
         * \ingroup GUI
         */
        class Item
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( Item )

        public:
            virtual ~Item();

            Item( const Item& ) = delete;
            Item& operator=( const Item& ) = delete;

            /// return rectangular bounding box in local coordinates
            BoundingBox getBoundingBox() const;

            /// Returns the bounding box of \a item in the coordinate system of this item
            BoundingBox getBoundingBox( const Item& item ) const;

            /// transforms local coordinates into the coordinate system of \a target
            Point2F mapTo( const Item& target, Point2F coords ) const;

            /// transforms coordinates from the coordinates system of \a target into the local coordinate system
            Point2F mapFrom( const Item& target, Point2F coords ) const;

            /// transforms bounding box in local coordinates into bounding box in the coordinate system of \a target.
            /// Note: this generally enlarges the bounding box!
            BoundingBox mapTo( const Item& target, const BoundingBox& box ) const;

            /// transforms bounding box coordinate system of \a target into bounding box in local coordinates.
            /// Note: this generally enlarges the bounding box!
            BoundingBox mapFrom( const Item& target, const BoundingBox& box ) const;

            /// Returns the position of this item's origin within the parent coordinate system.
            Point2F getPosition() const;

            /// Returns the counter-clockwise rotation (in radians) of this item with respect to the parent coordinate
            /// system.
            float getRotation() const;

            /// Returns the scale of this item with respect to the parent coordinate system.
            float getScale() const;

            /// Returns the general transformation matrix of this item.
            Matrix3F getTransform() const;

            /// Sets the position of the local coordinate system origin within the parent coordinate system.
            /**
             * This transformation matrix is combined with scale, rotation and transform to position
             * the local coordinate system in the parent coordinate system.
             * The default is (0.0, 0.0).
             */
            void setPosition( Point2F origin );

            /// Sets the counter-clockwise rotation (in radians) of the local coordinate system within the parent
            /// coordinate system.
            /**
             * This transformation matrix is combined with scale, position and transform to position
             * the local coordinate system in the parent coordinate system.
             * The default is 0.0.
             */
            void setRotation( float angle );

            /// Sets the scale of the local coordinate system within the parent coordinate system.
            /**
             * This transformation matrix is combined with rotation, position and transform to position
             * the local coordinate system in the parent coordinate system.
             * The default is 1.0.
             */
            void setScale( float scale );

            /// Sets the general transformation matrix of this item.
            /**
             * This transformation matrix (for pre-multiplication) is combined with scale, rotation and position to
             * position
             * the local coordinate system in the parent coordinate system.
             * The default is the identity matrix.
             * The transformation is pre-multiplied, i.e., [transformation matrix] * [column vector] = [transformed
             * vector].
             */
            void setTransform( const Matrix3F& transform );

            /// Resets all transformation parameters to default values.
            void resetTransformation();

            void setVisible( bool visible );

            void setToolTip( const std::string& tooltip );

        protected:
            Item( graphics2D_private::ItemPrivate* pimpl, Group* parent );

            graphics2D_private::ItemPrivate* mPimpl;
            Group* mParent;

            friend class Group;
        };

        //=========================================================================================================================

        /// Group of Items
        /**
         * Any items can be added to a group by simply passing the group as a reference to the item constructor.
         * You are responsible for keeping the children alive yourself, i.e., by holding a std::unique_ptr on it
         * or embedding it into a holder class.
         * Also, the children must be deleted before the group in order to guarantee sensible application behavior.
         * If there are still children around when the Group gets destroyed, a warning is printed to std::cerr.
         * Implementations are encouraged to guarantee no crashed when using a stale children, but in general it
         * is undefined behavior to use children items after their parent group was deleted.
         */
        class Group : public Item, private graphics2D_private::ItemHolder< Item >
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( Group )

        public:
            explicit Group( Group& parent );

            virtual ~Group();

            const std::list< Item* >& getChildren();

        protected:
            Group( graphics2D_private::GroupPrivate* pimpl, Group* parent );

        private:
            void addToGroup( Item& item );

            void removeFromGroup( Item& item );

            friend class Item;
        };

        //=========================================================================================================================

        /**
         * ScenePainter represents an interactive scene containing several 2D primitives.
         * Being an event handler, it receives mouse and keyboard events and can react on them.
         * Being a Graphics2D::Group it can contain child items. The global coordinate system of the scene
         * (i.e., the scene coordinates) is the local coordinate system of this group. This local coordinate system
         * can be positioned within the surrounding SceneView2D by calling \a setPosition(), \a setRotation(), \a
         * setScale(),
         * and \a setTransform(). Note, however, that translation will reflect the scrolling in scrollable SceneView2Ds.
         * getPosition() always returns the pixel-offset (in downward rightward direction) of the scene coordinate
         * system origin
         * from the top left corner of the SceneView2D.
         */
        class ScenePainter : public EventHandler, public Group
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( ScenePainter )
        public:
            ScenePainter();

            /// Transforms mouse coordinates (window relative) into scene coordinates
            Point2F getSceneCoordinates( int x, int y ) const;

            /// Transforms mouse coordinates (window relative) into scene coordinates
            Point2F getSceneCoordinates( Pos2D pos ) const;

            /// Returns all items at the given scene coordinates in the stacking order from top to bottom (see
            /// Graphics2D::Group for details).
            std::vector< Item* > getItemsAtCoord( Point2F coord ) const;

            /// React on scrolling of the scene.
            /**
             * This function is called when the scene has been scrolled.
             * It may be used to allow item dragging while scrolling, etc.
             * \pre nothing.
             * \post nothing.
             * \note Thread-safe.
             * \throws nothing.
             */
            virtual void hasScrolled();

            void storeToSVG( const std::string& filename, Size2D size );
        };

        //=========================================================================================================================

        /// arbitrary polygon
        class Polygon : public Item
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( Polygon )

        public:
            Polygon( Group& parent );

            /// vertices are listed in mathematically positive order without repeating the starting point
            void setVertices( const std::vector< Point2F >& vertices );

            /// vertices are listed in mathematically positive order without repeating the starting point
            void setVertices( std::initializer_list< Point2F > vertices );

            /// convenience function for creating rectangles
            void setRect( Point2F upperLeft, Point2F size );

            void setFillColor( Color color );

            void setLineColor( Color color );

            void setFillStyle( FillStyle style = FillStyle::Solid );

            /// sets the line style. Using \a width == 0 creates a line, that is exactly one pixel wide.
            void setLineStyle( float width = 0,
                               LineStyle style = LineStyle::Solid,
                               LineEndStyle endStyle = LineEndStyle::Round,
                               LineJoinStyle joinStyle = LineJoinStyle::Round );

        protected:
            Polygon( graphics2D_private::PolygonPrivate* pimpl, Group* parent );
        };

        //=========================================================================================================================

        /// polygon its edges are bezier splines
        class SmoothPolygon : public Polygon
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( SmoothPolygon )

        public:
            SmoothPolygon( Group& parent );

            /// Sets the control vectors for the bezier splines. For \c n positions
            /// you need to set \c 2n control vectors. Missing ones are replaced by zero vectors
            /// and additional ones are ignored.
            void setControlVectors( const std::vector< Point2F >& controlVectors );

            /// Sets the control vectors for the bezier splines. For \c n positions
            /// you need to set \c 2n control vectors. Missing ones are replaced by zero vectors
            /// and additional ones are ignored.
            void setControlVectors( std::initializer_list< Point2F > controlVectors );
        };

        //=========================================================================================================================

        /// arbitrary polyline
        class Polyline : public Item
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( Polyline )

        public:
            Polyline( Group& parent );

            void setVertices( const std::vector< Point2F >& vertices );

            void setVertices( std::initializer_list< Point2F > vertices );

            void setLineColor( Color color );

            /// sets the line style. Using \a width == 0 creates a line, that is exactly one pixel wide.
            void setLineStyle( float width = 0,
                               LineStyle style = LineStyle::Solid,
                               LineEndStyle endStyle = LineEndStyle::Round,
                               LineJoinStyle joinStyle = LineJoinStyle::Round );

        protected:
            Polyline( graphics2D_private::PolylinePrivate* pimpl, Group* parent );
        };

        //=========================================================================================================================

        /// draws a bezier spline.
        /// You can set the control vectors using \a setControlVectors. When new vertices are set, the control vectors
        /// are kept, when possible (i.e., matching number).
        class Spline : public Polyline
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( Spline )

        public:
            Spline( Group& parent );

            /// Sets the control vectors to zero, creating a straight polyline
            void setControlVectorsStraight();

            /// Sets the control vectors for this bezier spline. For \c n positions
            /// you need to set \c 2n-2 control vectors. Missing ones are replaced by zero vectors
            /// and additional ones are ignored.
            void setControlVectors( const std::vector< Point2F >& controlVectors );

            /// Sets the control vectors for this bezier spline. For \c n positions
            /// you need to set \c 2n-2 control vectors. Missing ones are replaced by zero vectors
            /// and additional ones are ignored.
            void setControlVectors( std::initializer_list< Point2F > controlVectors );
        };

        //=========================================================================================================================

        /// text within a 2D scene
        class Text : public Item
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( Text )

        public:
            Text( Group& parent );

            void setColor( Color color );

            void setFontSize( size_t pixelSize = 20 );

            void setFont( FontFamily family = FontFamily::Default,
                          bool bold = false,
                          bool italic = false,
                          bool underline = false );

            void setText( const std::string& text );
        };

        //=========================================================================================================================

        /// ellipse within a 2D scene
        class Ellipse : public Item
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( Ellipse )

        public:
            Ellipse( Group& parent );

            void setCenter( Point2F center );

            /// size[0] and size[1] are the lengths of the horizontal axis and the vertical axis respectively
            void setSize( Point2F size );

            /// convenience function for creating circles
            void setCircle( float radius );

            void setFillColor( Color color );

            void setLineColor( Color color );

            void setFillStyle( FillStyle style = FillStyle::Solid );

            /// sets the line style. Using \a width == 0 creates a line, that is exactly one pixel wide.
            void setLineStyle( float width = 0,
                               LineStyle style = LineStyle::Solid,
                               LineEndStyle endStyle = LineEndStyle::Round,
                               LineJoinStyle joinStyle = LineJoinStyle::Round );
        };

        //=========================================================================================================================

        /// images within a 2D scene
        class Pixmap : public Item
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( Pixmap )

        public:
            Pixmap( Group& parent );

            void setImage( const Image& image );

            void setBackgroundColor( const Color& bgColor );

            void loadFile( const std::string& filename );

            void setXPM( XPM pixmap );

            void resize( Point2F size );

            void resizeToWidth( int width );

            void resizeToHeight( int height );
        };

        //=========================================================================================================================

        /// Group that clips all its children to a given polygonal region.
        /// On creation, the children are clipped to an empty region, effectively disabling them.
        class ClippingGroup : public Group
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( ClippingGroup )

        public:
            explicit ClippingGroup( Group& parent );

            /// positions are listed in mathematically positive order without repeating the starting point
            void setClipPolygon( const std::vector< Point2F >& positions );

            /// positions are listed in mathematically positive order without repeating the starting point
            void setClipPolygon( std::initializer_list< Point2F > positions );

            /// convenience function for clipping to a rectangle
            void setClipRectangle( Point2F upperLeft, Point2F size );
        };

        //=========================================================================================================================

        /// Item that allows custom drawing.
        class CustomItem : public Item
        {
            F_GRAPHICS2D_DECLARE_PRIVATE( CustomItem )

        public:
            /// On construction, a zero bounding box is assumed.
            /// In case the initial bounding box is different (e.g., because of the
            /// constructor parameters or a non-zero default), it is required to
            /// call \c geometryChanged() in the subclass constructor!
            CustomItem( Group& parent );

            /// Defines the bounding box of this item in local coordinates. No drawing must be performed outside the
            /// bounding box.
            virtual BoundingBox defineBoundingBox() const = 0;

            /// Determines whether the given position is part of this object's content.
            /// \a position is in local coordinates.
            /// A pre-clipping is done, i.e., this method is quaranteed to be only called for positions inside the
            /// defined bounding box.
            /// Therefore, it is safe to implement an all-yes-default.
            virtual bool containsPosition( Point2F position ) const = 0;

            /// Paint the content of this item on the given painter (painting is done in local coordinates).
            /// All drawing outside the items defined bounding box will be ignored!
            /// Do not clear the background unless this is the specified
            /// behavior of this item, so that transparent / non-background areas allow
            /// to see through to items behind.
            virtual void paint( Painter& painter ) const = 0;

        protected:
            /// Method to call when the bounding box has changed. This implies a call to \a update().
            void geometryChanged();

            /// Requests a redraw of this item, e.g., when the content or appearance of this item changed.
            void requestUpdate();
        };
    }
}
