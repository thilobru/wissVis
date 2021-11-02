//#pragma once
//
//#include <cassert>
//#include <memory>
//#include <vector>
//
//#include <fantom/graphics.hpp>
//
//using namespace fantom;
//
//namespace Interactor
//{
//    enum DIRECTIONS
//    {
//        XYZ,
//        XY,
//        XZ,
//        YZ,
//        X,
//        Y,
//        Z
//    };
//
//    //--------------------------------------------------------------------------------
//
//    /** \brief Public interface for creating a torus
//     * Convenience class for GraphicsGeometry to provide easy torus handling.
//     */
//    class Torus
//    {
//    public:
//        Torus( Primitive& primitive );
//        ~Torus();
//
//        /** Sets the torus radii
//         * \param torusRadius the radius of the torus from center point to center of the torus tube
//         * \param tubeRadius the radius of the torus tube
//         */
//        void setRadius( double torusRadius, double tubeRadius );
//
//        /// sets the center of the torus
//        void setCenter( Point3 center );
//
//        /// sets the normal of the plain containing the median plane of the torus
//        void setNormal( Vector3 normal );
//
//        /// set the torus color
//        void setColor( Color const& col );
//
//        void commit();
//
//    private:
//        void createVertices( std::vector< unsigned int >& indices, int iteration, int maxIteration );
//
//        void createMesh( std::vector< unsigned int > const& indices1, std::vector< unsigned int > const& indices2 );
//
//        Primitive& primitive;
//        Color color;
//        Point3 center;
//        Vector3 normal;
//        double torusRadius;
//        double tubeRadius;
//        std::vector< unsigned int > torusIndices;
//        std::vector< Vector3 > torusVertices, torusNormals;
//    };
//
//    //--------------------------------------------------------------------------------
//    class ViewDelegate : public View
//    {
//    public:
//        ViewDelegate();
//
//        void addView( View& view );
//
//        virtual Matrix4 matrix() const;
//
//        virtual void reset();
//
//        virtual void rotate( const Vector3& d, double angle );
//
//        virtual void scale( double x, double y, double z );
//
//        virtual void translate( const Vector3& d );
//
//        std::vector< View* > views;
//    };
//
//    //--------------------------------------------------------------------------------
//
//    class ComplexManipulator
//    {
//    public:
//        virtual ~ComplexManipulator();
//
//        virtual View& view() = 0;
//        virtual const View& view() const = 0;
//
//        Point3 getOrigin() const;
//    };
//
//    //--------------------------------------------------------------------------------
//
//    class Translator : public ComplexManipulator
//    {
//    public:
//        typedef std::function< void( Vector3 translation ) > TranslationCallback;
//        Translator( Graphics& graphics, Vector3 direction, Color color = Color( 1, 0, 0 ) );
//        ~Translator();
//
//        View& view();
//        const View& view() const;
//
//        void setTranslationCallback( const TranslationCallback& callback );
//
//    private:
//        void drag( Point3 s0, Vector3 s, Point3 t0, Vector3 t );
//
//        TranslationCallback callback;
//        Vector3 direction;
//        std::unique_ptr< Manipulator > manipulator;
//        View& graphicsView;
//    };
//
//    //--------------------------------------------------------------------------------
//
//    class Rotator : public ComplexManipulator
//    {
//    public:
//        typedef std::function< void( Point3 origin, Vector3 axis, double angle ) > RotationCallback;
//
//        Rotator( Graphics& graphics, Vector3 direction, Color color = Color( 1, 0, 0 ) );
//        ~Rotator();
//
//        View& view();
//        const View& view() const;
//
//        void setRotationCallback( const RotationCallback& callback );
//
//    private:
//        void drag( Point3 s0, Vector3 s, Point3 t0, Vector3 t );
//
//        RotationCallback callback;
//        Vector3 direction;
//        std::unique_ptr< Manipulator > manipulator;
//        View& graphicsView;
//    };
//
//    //--------------------------------------------------------------------------------
//
//    class MultiTranslator : public ComplexManipulator
//    {
//    public:
//        MultiTranslator( Graphics& graphics,
//                         DIRECTIONS directions = XYZ,
//                         Color xCol = Color( 1, 0, 0 ),
//                         Color yCol = Color( 0, 1, 0 ),
//                         Color zCol = Color( 0, 0, 1 ) );
//
//        Point3 getOrigin() const;
//
//        View& view();
//        const View& view() const;
//
//        void setTranslationCallback( const Translator::TranslationCallback& callback );
//
//    private:
//        std::unique_ptr< Translator > mX, mY, mZ;
//        ViewDelegate delegate;
//    };
//
//    //--------------------------------------------------------------------------------
//
//    class MultiRotator
//    {
//    public:
//        MultiRotator( Graphics& graphics,
//                      DIRECTIONS directions = XYZ,
//                      Color xCol = Color( 1, 0, 0 ),
//                      Color yCol = Color( 0, 1, 0 ),
//                      Color zCol = Color( 0, 0, 1 ) );
//
//        void setRotationCallback( const Rotator::RotationCallback& callback );
//
//        View& view();
//
//    private:
//        std::unique_ptr< Rotator > mX, mY, mZ;
//        ViewDelegate delegate;
//    };
//
//    //--------------------------------------------------------------------------------
//
//    class MultiManipulator
//    {
//    public:
//        MultiManipulator( Graphics& graphics,
//                          DIRECTIONS translateDirections = XYZ,
//                          DIRECTIONS rotateDirections = XYZ,
//                          Color xCol = Color( 1, 0, 0 ),
//                          Color yCol = Color( 0, 1, 0 ),
//                          Color zCol = Color( 0, 0, 1 ) );
//
//        void setRotationCallback( const Rotator::RotationCallback& callback );
//
//        void setTranslationCallback( const Translator::TranslationCallback& callback );
//
//        View& view();
//
//    private:
//        MultiRotator rotate;
//        MultiTranslator translate;
//        ViewDelegate delegate;
//    };
//}

