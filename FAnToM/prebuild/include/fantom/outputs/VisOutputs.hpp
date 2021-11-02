#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../inputs/Events.hpp"

namespace fantom
{
    namespace Graphics2D
    {
        class ScenePainter;
    }

    class DockWindow;
    class MainWindow;
    class Widget;

    namespace graphics
    {
        class GraphicsSystem;
        class Drawable;
    }

    /**
     * \brief Visual outputs of algorithms.
     *
     * Subclasses can add visual outputs with the various add(...) methods during constructor.
     *
     * \throw nothing is thrown by the destructor.
     * \throw std::bad_alloc is thrown on insufficient memory (strong).
     *
     * This class follows the private implementation design pattern.
     * The implementation is kept invisible from the user of the outputs (i.e., derived classes).
     * The abstract interface to the implementation can be found in #fantom::VisOutputs::Control.
     */
    class VisOutputs
    {
    public:
        class Control;
        class GraphicsSlot;
        enum class VisualOutputType
        {
            Graphics, // 3D graphics
            Scene2D,  // 2D graphics Scene
            Window    // algorithm window
        };


        VisOutputs( Control& control );

        virtual ~VisOutputs() = default;

        /**
         * Adds a 3D graphics object to the visual otuputs of an algorithm.
         *
         * \param name Name of the output.
         *
         * An exception is thrown if a visual output of this name already exists.
         */
        void addGraphics( const std::string& name );

        /**
         * Adds a 3D graphics object in the heads up display area to the visual outputs of an algorithm.
         *
         * \param name Name of the output.
         *
         * An exception is thrown if a visual output of this name already exists.
         */
        void addGraphicsHUD( const std::string& name );

        /**
         * Adds a 2D scene of type SceneType to the visual outputs of an algorithm.
         * SceneType must be derived from Graphics2D::ScenePainter and default-constructable.
         *
         * \tparam SceneType type of the ScenePainter
         * \param name Name of the output.
         * \param scrollable specifies whether a scrollable scene view should be created
         * \param useOpenGL specifies whether hardware acceleration should be used for the scene
         *
         * An exception is thrown if a visual output of this name already exists.
         */
        template < class SceneType >
        void add2DScene( const std::string& name, bool scrollable = false, bool useOpenGL = false );

        /**
         * Adds a window type WindowType to the visual outputs of an algorithm.
         * WindowType needs to be derived from DockWindow or MainWindow.
         * In any case, WindowType must be constructible with the signature
         * WindowType( fantom::MainWindow& mainWindow, const std::string& name ).
         * The first parameter is the main window of the FAnToM Gui and the second parameter
         * the name of the output, which should be used as the title of the custom window.
         *
         * \tparam WindowType type of the algorithm window
         * \param name Name of the output.
         *
         * An exception is thrown if a visual output of this name already exists.
         */
        template < class WindowType >
        void addWindow( const std::string& name );

        /**
         * Adds an event output to the algorithm.
         * Events are meant as a way to signal other events of changes to the visualization.
         * For example, a selection can be propagated to other algorithms using an event.
         * This function declares a new event output that can be connected to other algorithms InputEvent inputs.
         * @param name Name of the output.
         */
        void addEvent( const std::string& name );

    protected:
        VisOutputs::Control& mGraphicsControl;
    };

    class VisOutputs::GraphicsSlot
    {
    public:
        GraphicsSlot( bool isHud );

        const std::shared_ptr< graphics::Drawable >& getContent() const;
        bool isHud() const;

    private:
        bool mIsHud;
        std::shared_ptr< graphics::Drawable > mContent;

        friend class VisOutputs::Control;
    };

    class VisOutputs::Control
    {
    public:
        virtual ~Control();

        /**
         * @brief Returns the 2D scene output registered by the name
         * An exception is thrown if the scene output does not exist.
         * @param name the name of the scene output (locally unique)
         * @return reference to the scene instance
         */
        Graphics2D::ScenePainter& get2DSceneOutput( const std::string& name ) const;
        Widget& get2DSceneWidget( const std::string& name ) const;

        /**
         * @brief Returns the window output registered by the name
         * An exception is thrown if the window output does not exist.
         * @param name the name of the window output (locally unique)
         * @return reference to the window instance
         */
        Widget& getWindowOutput( const std::string& name ) const;

        /// Lists the names of all visual outputs (2D graphics, 3D graphics, windows)
        std::vector< std::string > getOutputNames() const;

        /// Returns the type of visual output of the given name. An exception is thrown, if the output does not exist.
        VisualOutputType getOutputType( const std::string& name ) const;

        GraphicsSlot const& getGraphics( const std::string& name ) const;

        /// Send an event over the specified output port.
        void sendEvent( const std::string& name, const Event& event );

    protected:
        /// Tests, whether the name is unique. Throws an exception, if it is not.
        void testUniqueness( const std::string& name );
        void addGraphicsOutput( const std::string& name, bool hud );
        void add2DScene( const std::string& name,
                         std::unique_ptr< Graphics2D::ScenePainter > scene,
                         bool scrollable,
                         bool useOpenGL );
        void addWindow( const std::string& name,
                        std::function< std::unique_ptr< Widget >( MainWindow&, const std::string& ) > makerFunction );
        void addEvent( const std::string& name );

        virtual const GraphicsSlot& setGraphics( const std::string& name,
                                                 const std::shared_ptr< graphics::Drawable >& drawable );

        /// Create a corresponding output port.
        virtual std::pair< std::unique_ptr< Widget >, Graphics2D::ScenePainter* >
        create2DScene( const std::string& name,
                       std::unique_ptr< Graphics2D::ScenePainter > scene,
                       bool scrollable,
                       bool useOpenGL )
            = 0;

        /// Create a corresponding output port.
        virtual std::unique_ptr< Widget >
        createWindow( const std::string& name,
                      std::function< std::unique_ptr< Widget >( MainWindow&, const std::string& ) > makerFunction )
            = 0;

        /// Create an output port for events. The returned function is used to send an event.
        virtual std::function< void( const Event& ) > createEventOutput( const std::string& name ) = 0;

    private:
        std::unordered_map< std::string, std::unique_ptr< GraphicsSlot > > mGraphics;
        std::unordered_map< std::string, std::pair< std::unique_ptr< Widget >, Graphics2D::ScenePainter* > >
            m2DGraphics;
        std::unordered_map< std::string, std::unique_ptr< Widget > > mWindowItems;
        std::unordered_map< std::string, std::function< void( const Event& ) > > mEventItems;

        friend class AlgorithmImpl;
        friend class VisOutputs;
        friend class InteractionAlgorithm;
        friend class VisAlgorithm;
    };


    //============================================== Template definitions =====

    template < class SceneType >
    void VisOutputs::add2DScene( const std::string& name, bool scrollable, bool useOpenGL )
    {
        static_assert( std::is_base_of< Graphics2D::ScenePainter, SceneType >::value,
                       "SceneType must be derived from Graphics2D::ScenePainter!" );

        mGraphicsControl.add2DScene(
            name, std::unique_ptr< Graphics2D::ScenePainter >( new SceneType() ), scrollable, useOpenGL );
    }

    template < class WindowType >
    void VisOutputs::addWindow( const std::string& name )
    {
        static_assert( std::is_base_of< MainWindow, WindowType >::value
                           || std::is_base_of< DockWindow, WindowType >::value,
                       "Window must be derived from MainWindow or DockWindow!" );

        std::function< std::unique_ptr< Widget >( MainWindow&, const std::string& ) > makerFunction
            = []( MainWindow& mainWindow, const std::string& name ) {
                  return std::unique_ptr< Widget >( new WindowType( mainWindow, name ) );
              };

        mGraphicsControl.addWindow( name, makerFunction );
    }
}
