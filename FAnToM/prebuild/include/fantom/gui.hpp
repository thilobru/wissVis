#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "events.hpp"
#include "graphics2D.hpp"
#include "math.hpp"
#include "utils.hpp"

namespace fantom
{
    namespace gui_private
    {
        class GuiFactory;
        class WidgetPrivate;
        class SplashscreenWidgetPrivate;
        class BoxLayoutPrivate;
        class GridLayoutPrivate;
        class StatusBoxLayoutPrivate;
        class FormWidgetPrivate;
        class ScrollAreaPrivate;
        class TreeItemPrivate;
        class TreeViewPrivate;
        class LabelPrivate;
        class PushButtonPrivate;
        class CheckBoxPrivate;
        class SliderPrivate;
        class SeparatorPrivate;
        class ComboBoxPrivate;
        class LineEditPrivate;
        class HighlightedTextEditPrivate;
        class ShellPrivate;
        class OffscreenGlContextPrivate;
        class GlAreaPrivate;
        class ProgressBarPrivate;
        class SceneView2DPrivate;
        class DockWindowPrivate;
        class MainWindowPrivate;
        class MenuPrivate;
        class MenuItemPrivate;
        class ToolBarPrivate;
        class ToolBarItemPrivate;
        class TimerPrivate;
        class TabWidgetPrivate;
        class GroupBoxPrivate;

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
            /// Must be called during destructor of derived classes
            void eraseChildren()
            {
                std::list< Item* > childs( std::move( mChilds ) );
                for( Item* child : childs )
                {
                    delete child;
                }
            }
            std::list< Item* > mChilds;
        };
    } // namespace gui_private

    using Icon = Graphics2D::Image;

    class Menu;

    //=========================================================================================================================

    /// base class for all widgets \ingroup GUI
    /**
     * The all child widgets and the menu (if existant) are deleted during destruction of this widget.
     * However, it is possible (and also recommended) to delete child widgets manually before deleting their parent
     *widget.
     *
     * This class is not intended to be used directly. There are specialized subclasses for all possible use-cases.
     */
    class Widget : private gui_private::ItemHolder< Widget >
    {

    public:
        class DropArea;

        /// Destroys widget.
        virtual ~Widget();

        /// Enables or disables the widget.
        void setEnabled( bool enabled );

        /// Shows or hides the widget.
        void setVisible( bool visible );

        bool isVisible() const;

        /// Sets the tool-tip of the widget.
        void setToolTip( const std::string& text );

        bool isChild( Widget& child );

    protected:
        Widget( Widget* parent, gui_private::WidgetPrivate* pimpl );

        Widget( Widget& parent ) = delete;
        Widget& operator=( const Widget& ) = delete;
        Widget( const Widget& ) = delete;

    private:
        /// throws an exception, if Menu is already set. Call \c setMenu(nullptr) to discard the menu.
        void setMenu( Menu* menu );

        Widget* mParent;
        Menu* mMenu;

        friend class DropArea;
        // responsibility of drop area remains with its creator
        void registerDropArea( DropArea* area );
        void unregisterDropArea( DropArea* area );
        std::list< DropArea* > mDropAreas;

        F_GUI_DECLARE_PRIVATE( Widget )

        friend class Menu;
        friend class TabWidget;
        friend class FormWidget;

    protected:
        gui_private::WidgetPrivate* mPimpl;
    };

    /// a splashscreen widget \ingroup GUI
    /**
     * This class is only intended to be used during startup and closing
     */
    class SplashscreenWidget : public Widget
    {
        F_GUI_DECLARE_PRIVATE( SplashscreenWidget )

    public:
        SplashscreenWidget();

        /// Destroys widget.
        virtual ~SplashscreenWidget();

        /// Enables or disables the widget.
        void setEnabled( bool enabled );

        /// Shows or hides the widget.
        void setVisible( bool visible );

        bool isVisible() const;

        /// Sets the tool-tip of the widget.
        void setToolTip( const std::string& text );

        /// Sets the a message in the splashscreen
        void setMessage( const std::string& text );

        /// Sets the picture of the splash screen
        void setPixmap( const Icon& icon );

    protected:
        SplashscreenWidget( SplashscreenWidget& parent ) = delete;
        SplashscreenWidget& operator=( const SplashscreenWidget& ) = delete;
        SplashscreenWidget( const SplashscreenWidget& ) = delete;
    };

    //=========================================================================================================================

    /// Area on top of a widget that handles drag and drop events
    /**
     * Drop areas are used on top of most widgets to implement
     * drag and drop functionality ( i.e., the drop part of it )
     * and are intended for drops coming from outside fantom.
     *
     * Such events are common when text fragments, color information
     * or whole files are dragged from other applications onto
     * widgets in fantom.
     *
     * This class registers with the widget to receive the widget's
     * events and can promote these events by overloading the
     * dragEnterEvent, dragMoveEvent and dropEvent functions.
     * It automatically unregisters upon deletion of the drop are,
     * which must be done before the widget is deleted.
     *
     * \copydoc DropHandler
     *
     */
    class Widget::DropArea : public DropHandler
    {
    public:
        /**
         * Constructor
         * Creates a drop area and registers with the widget
         * \param parent the widget from which to receive drop events
         * \pre parent is a valid widget
         */
        DropArea( Widget& parent )
            : mWidget( parent )
        {
            mWidget.registerDropArea( this );
        }

        /**
         * Destructor
         * Unregisters from the widget
         * \pre the widget of this are must still be valid
         */
        virtual ~DropArea()
        {
            mWidget.unregisterDropArea( this );
        }

    private:
        /// pointer to the widget on which the area is working
        Widget& mWidget;
    };

    //=========================================================================================================================

    /// simple layout which arranges its widgets horizontally or vertically \ingroup GUI
    class BoxLayout : public Widget
    {
        F_GUI_DECLARE_PRIVATE( BoxLayout )

    public:
        /// Representation of a stretchable area in the BoxLayout
        class Stretch;

        /// Creates a new layout.
        /**
         * \param parent the parent widget of this layout
         * \param horizontal whether the widgets should be arranged horizontally or vertically.
         */
        BoxLayout( Widget& parent, bool horizontal );

        /// Adds a stretch element to the layout.
        /// The returned object is just a dummy for easy placement of stretches between items during intialization of a
        /// layout.
        /// Removing or retaining it does not change anything as removal of stretches is currently not supported.
        Stretch addStretch( int stretchFactor = 0 );

        /// Sets margin in pixels.
        void setMargin( std::size_t margin );

        /// Sets spacing between widgets in pixels.
        void setSpacing( std::size_t spacing );

        /// Sets the stretching of a widget to factor
        void setStretchFactor( int factor, Widget& widget );

    protected:
        BoxLayout( Widget& parent, gui_private::BoxLayoutPrivate* pimpl );
    };

    class BoxLayout::Stretch
    {
    public:
        Stretch();
    };


    //=========================================================================================================================


    /// layout which arranges its widgets in a grid \ingroup GUI
    class GridLayout : public Widget
    {
        F_GUI_DECLARE_PRIVATE( GridLayout )

    public:
        /// Creates a new layout.
        /**
         * \param parent the parent widget of this layout
         */
        GridLayout( Widget& parent );

        /// Sets margin in pixels.
        void
        setMargin( std::size_t leftMargin, std::size_t topMargin, std::size_t rightMargin, std::size_t bottomMargin );

        /// Sets spacing between widgets in pixels.
        void setSpacing( std::size_t horizontalSpacing, std::size_t verticalSpacing );

        /// Sets the position and size of a widget in the layout.
        void setWidgetPosition(
            Widget& child, std::size_t xPos, std::size_t yPos, std::size_t xSize = 1, std::size_t ySize = 1 );

        /// Sets the strech factor of the specific column
        /// By default columns have a stretch factor of 0. If a larger factor is assigned to a column, columns will get
        /// their space redistributed according to the relative proportions of their factors.
        void setColumnStretchFactor( std::size_t column, std::size_t factor );

    protected:
        GridLayout( Widget& parent, gui_private::GridLayoutPrivate* pimpl );
    };

    //=========================================================================================================================

    class FormWidget : public Widget
    {
        F_GUI_DECLARE_PRIVATE( FormWidget )

    public:
        FormWidget( Widget& parent );

        /// Sets the caption for the given widget.
        /// If \a text is empty, the caption label will be omitted and the widget will be laid out over the full width.
        void setCaption( Widget& widget, const std::string& text, bool bold = false );

        void alignToBottom();

    protected:
        FormWidget( Widget& parent, gui_private::FormWidgetPrivate* pimpl );
    };

    //=========================================================================================================================

    /// layout which arranges widgets in tabs \ingroup GUI
    /**
     * Just put child widgets into it.
     */
    class TabWidget : public Widget
    {
        F_GUI_DECLARE_PRIVATE( TabWidget )

    public:
        explicit TabWidget( Widget& parent );

        /// Sets the caption of the tab of child widgets.
        /**
         * \pre \a child is a subwidget of this TabWidget
         */
        void setText( Widget& child, const std::string& text );

    protected:
        TabWidget( Widget& parent, gui_private::TabWidgetPrivate* pimpl );
    };

    //=========================================================================================================================

    /// widget group decorator \ingroup GUI
    /**
     * Children are placed in a vertical column.
     */
    class GroupBox : public Widget
    {
        F_GUI_DECLARE_PRIVATE( GroupBox )

    public:
        /// Creates new group decorator.
        explicit GroupBox( Widget& parent, const std::string& caption );

        void setCaption( const std::string& caption );

    protected:
        GroupBox( Widget& parent, gui_private::GroupBoxPrivate* pimpl );
    };

    //=========================================================================================================================

    /// scroll bar decorator \ingroup GUI
    /**
     * After creation, exactly one child widget can be added to it, that is shown in the scroll area.
     */
    class ScrollArea : public Widget
    {
        F_GUI_DECLARE_PRIVATE( ScrollArea )

    public:
        /// Creates new scroll bar decorator.
        explicit ScrollArea( Widget& parent );

    protected:
        ScrollArea( Widget& parent, gui_private::ScrollAreaPrivate* pimpl );
    };

    //=========================================================================================================================

    /// label widget \ingroup GUI
    class Label : public Widget
    {
        F_GUI_DECLARE_PRIVATE( Label )

    public:
        /// Creates new label widget.
        Label( Widget& parent, const std::string& text, bool expandable = true );

        /// Sets the text of the label.
        void setText( const std::string& text );

    protected:
        Label( Widget& parent, gui_private::LabelPrivate* pimpl );
    };

    //=========================================================================================================================


    /// progress bar \ingroup GUI
    class ProgressBar : public Widget
    {
        F_GUI_DECLARE_PRIVATE( ProgressBar )

    public:
        /// Creates a new progress bar widget.
        /**
         * \param parent the parent widget
         */
        ProgressBar( Widget& parent );

        /// set the current value (as percentage in 0...100) of the progress
        void setValue( int value );

        /// sets a text describing the progress' status
        void setText( const std::string& text );

    protected:
        ProgressBar( Widget& parent, gui_private::ProgressBarPrivate* pimpl );
    };

    //=========================================================================================================================

    /// horizontal or vertical separation line (not for menus or toolboxes, but for usage in widget layouts)
    class Separator : public Widget
    {
        F_GUI_DECLARE_PRIVATE( Separator )

    public:
        Separator( Widget& parent, bool horizontal );

    protected:
        Separator( Widget& parent, gui_private::SeparatorPrivate* pimpl );
    };

    //=========================================================================================================================

    ///  widget for pushing \ingroup GUI
    class PushButton : public Widget
    {
        F_GUI_DECLARE_PRIVATE( PushButton )

    public:
        /// Creates a new push button widget.
        /**
         * \param parent the parent widget
         * \param text the initial text of the button's label.
         * \param action the callback that is called when the button is pressed.
         */
        PushButton( Widget& parent, const std::string& text, const std::function< void() >& action = nullptr );

        /// Sets the text of the button's label.
        void setText( const std::string& text );

        /// Sets the icon of the button.
        void setIcon( const Icon& icon );

        /// Sets the callback that is called when the button is pressed.
        void setAction( const std::function< void() >& action );

    protected:
        PushButton( Widget& parent, gui_private::PushButtonPrivate* pimpl );
    };

    //=========================================================================================================================

    /// check box widget \ingroup GUI
    class CheckBox : public Widget
    {
        F_GUI_DECLARE_PRIVATE( CheckBox )

    public:
        /// Creates a new check box widget.
        /**
         * \param parent the parent widget
         * \param action the callback that is called when the check box is checked or unchecked.
         */
        CheckBox( Widget& parent, const std::function< void( bool ) >& action = nullptr );

        /// Sets the callback that is called when the button is pressed.
        void setAction( const std::function< void( bool ) >& action );

        /// Checks or unchecks the check box.
        void set( bool value );

        /// Determines whether the check box is checked.
        bool get() const;

        /// Sets the text label of the check box.
        /**
         * \param textLabel the new label
         */
        void setLabel( const std::string& textLabel );

    protected:
        CheckBox( Widget& parent, gui_private::CheckBoxPrivate* pimpl );
    };

    //=========================================================================================================================

    /// combo box widget \ingroup GUI
    class ComboBox : public Widget
    {
        F_GUI_DECLARE_PRIVATE( ComboBox )

    public:
        /// Creates a new combo box widget.
        /**
         * \param parent the parent widget
         * \param update the callback that is called when the combo box's selection changes.
         */
        ComboBox( Widget& parent, const std::function< void( size_t ) >& update = nullptr );

        /// Sets the callback that is called when the button is pressed.
        void setUpdate( const std::function< void( size_t ) >& update );

        /// Sets the list of options.
        void setData( const std::vector< std::string >& data,
                      const std::vector< Icon >& icons = std::vector< Icon >() );

        /// Sets the currently selected option.
        void set( std::size_t value );

        /// Determines the currently selected option.
        std::size_t get() const;

        /// can be overridden to generate the combobox content dynamically before showing the popup
        virtual void beforeShow();

        /// clear internal state after hiding. Useful when \a beforeShow has been overridden
        virtual void afterHide();

    protected:
        ComboBox( Widget& parent, gui_private::ComboBoxPrivate* pimpl );
    };

    //=========================================================================================================================

    /// slider widget \ingroup GUI
    class Slider : public Widget
    {
        F_GUI_DECLARE_PRIVATE( Slider )

    public:
        /// Creates a new slider widget.
        /**
         * Slider values are in the range [0,max] inclusive.
         * \param parent the parent widget
         * \param max maximum value.
         * \param horizontal whether the slider is horizontal or vertical.
         * \param update the callback that is called when the slider changes.
         */
        Slider( Widget& parent,
                std::size_t max,
                bool horizontal = true,
                const std::function< void( size_t ) >& update = nullptr );

        /// Sets the callback that is called when the button is pressed.
        void setUpdate( const std::function< void( size_t ) >& update );

        /// Sets the currently selected value.
        void set( std::size_t value );

        /// Sets the maximal value.
        void setMax( size_t max );

        /// Determines the currently selected value.
        std::size_t get() const;

    protected:
        Slider( Widget& parent, gui_private::SliderPrivate* pimpl );
    };

    //=========================================================================================================================

    /// line edit widget \ingroup GUI
    class LineEdit : public Widget
    {
        F_GUI_DECLARE_PRIVATE( LineEdit )

    public:
        /// Creates a new line edit widget.
        /**
         * \param parent the parent widget
         * \param update the callback that is called when the line edit's content changes.
         */
        LineEdit( Widget& parent, const std::function< void( const std::string& ) >& update = nullptr );

        /// Sets the callback that is called when the button is pressed.
        void setUpdate( const std::function< void( const std::string& ) >& update );

        /// Sets the content of the line edit.
        void set( const std::string& value );

        /// Determines the content of the line edit.
        std::string get() const;

        /// Set background color
        void setBackgroundColor( const Color& color );

        /// Set foreground (i.e. text) color
        void setForegroundColor( const Color& color );

    protected:
        LineEdit( Widget& parent, gui_private::LineEditPrivate* pimpl );
    };

    //=========================================================================================================================

    /// text edit widget \ingroup GUI
    class HighlightedTextEdit : public Widget
    {
        F_GUI_DECLARE_PRIVATE( HighlightedTextEdit )

    public:
        /// Creates a new line edit widget.
        /**
         * \param parent the parent widget
         * \param update the callback that is called when the line edit's content changes.
         */
        HighlightedTextEdit( Widget& parent, const std::function< void( const std::string& ) >& update = nullptr );

        /// Sets the callback that is called when the text is changed.
        void setUpdate( const std::function< void( const std::string& ) >& update );

        /// Sets the content of the line edit.
        void set( const std::string& value );

        /// Determines the content of the line edit.
        std::string get() const;

        /// Set background color
        void setBackgroundColor( const Color& color );

        /// Set foreground (i.e. text) color
        void setForegroundColor( const Color& color );

        /// Set the programming language for highlighting
        /**
         * lanuage can be one of
         * Python
         * GLSL
         * text
         */
        void setLanguage( const std::string& language );

    protected:
        HighlightedTextEdit( Widget& parent, gui_private::HighlightedTextEditPrivate* pimpl );
    };

    //=========================================================================================================================


    /// Basic class for implementing Shells.
    /**
     * The shell contains a history area that can be used to display previous commands and output of executing them,
     * as well as a command prompt area, where a new command can be entered interactively.
     *
     * The Shell takes care of all user interaction needed to manipulate the content of the command prompt.
     * All keyboard events are also relayed to child classes via the keyPress/keyRelease functions. Therefore, these
     * functions can be used to implement everything related to the execution of commands.
     *
     * \ingroup GUI
     */
    class Shell : public Widget
    {
        F_GUI_DECLARE_PRIVATE( Shell )

    public:
        explicit Shell( Widget& parent );

        void setBackgroundColor( Color color );
        /// sets the current command prompt
        void setCommand( const std::string& command );
        /// adds colored text to the history/log area of the shell
        void addText( const std::string& text, Color color, bool withPrompt = false );
        /// gets the content of the command prompt
        std::string getCommand();

        /// Process key press event.
        /**
         * \param event the key event
         * \pre nothing.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual void keyPress( const EventHandler::KeyEvent& event );

        /// Process key release event.
        /**
         * \param event the key event
         * \pre nothing.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual void keyRelease( const EventHandler::KeyEvent& event );

    protected:
        Shell( Widget& parent, gui_private::ShellPrivate* pimpl );
    };

    //=========================================================================================================================

    class TreeItem;
    /// widget for a file explorer-like tree view \ingroup GUI
    class TreeView : public Widget, private gui_private::ItemHolder< TreeItem >
    {
        F_GUI_DECLARE_PRIVATE( TreeView )

    public:
        /// Creates new tree view.
        explicit TreeView( Widget& parent );
        virtual ~TreeView();

        /// Sets heading of the tree view.
        void setHeading( const std::vector< std::string >& heading );

        /// Select a specific tree item.
        void selectItem( TreeItem& item );

    protected:
        TreeView( Widget& parent, gui_private::TreeViewPrivate* pimpl );

    private:
        gui_private::ItemHolder< TreeItem >* itemHolder();
        friend class TreeItem;
    };

    //=========================================================================================================================

    /// tree item in a tree view \ingroup GUI
    class TreeItem : private gui_private::ItemHolder< TreeItem >
    {
        F_GUI_DECLARE_PRIVATE( TreeItem )

    public:
        /// Creates new tree item.
        /**
         * \param h the something that can hold a widget.
         * \param text the initial label of the item.
         * \param action the callback that is called when the item is selected
         * \param checkUpdate the callback that is called when the check state of the tree item changes
         */
        TreeItem( TreeView& h,
                  const std::string& text,
                  const std::function< void() >& action = nullptr,
                  const std::function< void( bool ) >& checkUpdate = nullptr );
        /// Creates new tree item.
        /**
         * \param h the something that can hold a widget.
         * \param text the initial label of the item.
         * \param action the callback that is called when the item is selected
         * \param checkUpdate the callback that is called when the check state of the tree item changes
         */
        TreeItem( TreeItem& h,
                  const std::string& text,
                  const std::function< void() >& action = nullptr,
                  const std::function< void( bool ) >& checkUpdate = nullptr );

        /// Destroys tree item.
        virtual ~TreeItem();

        /// Determines the text of the tree item.
        const std::string getText() const;

        /// Sets the text in a certain column.
        void setText( std::size_t i, const std::string& text );

        /// Determines whether the tree item is checked.
        bool getChecked() const;

        /// Checks or unchecks the tree item.
        void setChecked( bool checked );

        /// Sets the text color to black
        void setTextColorBlack();

        /// Sets the text color to grey
        void setTextColorGrey();

        /// Sets the callback when the check state of the tree item changes.
        void setCheckUpdate( const std::function< void( bool ) >& checkUpdate );

        /// Sets the callback when the item is selected.
        void setAction( const std::function< void() >& action );

        /// Sets the callback when the item is double clicked
        void setDoubleClickAction( const std::function< void() >& action );

        /// Sets the tool tip of the item
        void setToolTip( const std::string& toolTipString );

        /// Expands or collapses the tree item.
        void setExpanded( bool expanded );

        /// Sets the icon of the entry in the chosen column
        void setIcon( const Icon& icon, int column );

    private:
        gui_private::ItemHolder< TreeItem >* itemHolder();

        gui_private::ItemHolder< TreeItem >* mParent;

        gui_private::TreeItemPrivate* mPimpl;

        friend class TreeView;
    };

    //=========================================================================================================================

    /// interface to an OpenGL painter reimplemented by algorithm \ingroup GUI
    class GlPainter : public EventHandler
    {

    public:
        /// inititalize OpenGL painter
        /**
         * This member is used to initialize the OpenGL context, e.g. textures and
         * display lists. It will be executed exactly once and also informs about
         * the callback that can be used to request paint().
         * \param update this function may be called by the painter (or some other
         *        party) to request a call to paint(). The request is scheduled and executed later.
         * \pre OpenGL content is current, init(), resize() and paint() have not been called before.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual void init( const std::function< void() >& update ) = 0;

        /// Repaint drawing area using OpenGL.
        /**
         * This member is called if the window system repaints windows
         * or if it was requested via update() (see init()).
         * \pre OpenGL content is current, init() has been called once before,
         * paint() and resize() are not executed currently in other threads.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual void paint() = 0;

    protected:
        GlPainter() {}
    };

    //=========================================================================================================================

    /// OpenGL context for OpenGl operations \ingroup GUI
    class GlContext
    {
    public:
        virtual ~GlContext();

    protected:
        /// Creates new OpenGL context.
        GlContext();
    };

    /// OpenGL context for offscreen operations \ingroup GUI
    class OffscreenGlContext : public GlContext
    {
        F_GUI_DECLARE_PRIVATE( OffscreenGlContext )

    public:
        /// Creates new OpenGL context.
        /**
         * \param share the context to share (for display lists, textures, shaders, etc).
         */
        OffscreenGlContext( GlContext* share = nullptr );

        virtual ~OffscreenGlContext();

        /**
         * @brief Execute OpenGL commands in the context
         * \param commands Function object that will execute the requested commands.
         * \param sync If true, return after the commands have been run. Otherwise, return immediately and only schedule
         *             the commands.
         */
        void executeGL( const std::function< void() >& commands, bool sync = true );

    private:
        gui_private::OffscreenGlContextPrivate* mPimpl;
    };

    /// OpenGL drawing area \ingroup GUI
    class GlArea : public Widget, public GlContext
    {
        F_GUI_DECLARE_PRIVATE( GlArea )

    public:
        /// Creates new OpenGL drawing area.
        /**
         * \pre painter.get() != 0
         * \param parent the parent widget
         * \param painter the object that paints the content of the area.
         * \param share the context to share (for display lists, textures, shaders, etc).
         * \note The widgets will take ownership of the painter. Use \c getPainter()
         * to access the painter.
         */
        GlArea( Widget& parent, std::unique_ptr< GlPainter > painter, GlContext* share = nullptr );

        /// Determines the painter of the drawing area.
        GlPainter& getPainter();
    };

    //=========================================================================================================================

    /// Two-dimensional canvas widget \ingroup GUI
    class SceneView2D : public Widget
    {
        F_GUI_DECLARE_PRIVATE( SceneView2D )

    public:
        /// Creates new 2D canvas as a GUI widget
        /**
         * \param parent the parent widget.
         * \param painter the object that paints the content of the area.
         * \param scrollable determines whether the view is scrollable.
         * \param useOpenGL determines whether hardware acceleration should be used.
         * In case of a scrollable view (default), scrollbars will be shown when needed, and the translation of the
         * scene
         * is managed by the scrollbars. Thus, changing the translation by transformation of the painter does not have
         * any effect.
         * Scrollable views are aligned to the top left by default. In general, any change of the scene defined by the
         * ScenePainter
         * (transformation, translation, change of content, etc.) does NOT trigger a change of the SceneView2D viewport.
         * Only the
         * scrollbars are adjusted to reflect the new bounding rectangle of the scene. If the complete scene is shown
         * in the viewport, scrolling is not possible. Also, if not the complete scene is shown, scrolling the viewport
         * out of the scene
         * is not allowed. If the viewport is already out of the scene (because of a change in the scene's bounding
         * box), scrolling further
         * out of the scene is not allowed.
         */
        SceneView2D( Widget& parent,
                     std::unique_ptr< Graphics2D::ScenePainter > painter,
                     bool scrollable = true,
                     bool useOpenGL = false );

        /// Determines the painter of the canvas.
        Graphics2D::ScenePainter& getPainter();

        /// Determines the painter of the canvas.
        const Graphics2D::ScenePainter& getPainter() const;
    };


    //=========================================================================================================================

    class ToolBar;
    class DockWindow;
    /// top-level window \ingroup GUI
    /**
     * After creation, exactly one child widget can be added to the main window, that represents the main area of the
     * window.
     * Being a top-level window, this widget is never deleted implicitely. You are required to hold the MainWindow
     * object directly
     * or a pointer to it and destroy it manually.
     */
    class MainWindow : public Widget, private gui_private::ItemHolder< ToolBar >
    {
        F_GUI_DECLARE_PRIVATE( MainWindow )
        friend class ToolBar;

    public:
        /// Creates new top-level window.
        /**
         * \param closeAction the callback for when the window is closing.
         * If closeAction is valid function, it will be called when the window
         * is about to be closed. The window will only be closed if closeAction returns true.
         */
        MainWindow( const std::function< bool() >& closeAction = nullptr );

        /** Sets the closeAction callback.
         * If closeAction is valid function, it will be called when the window
         * is about to be closed. The window will only be closed if closeAction returns true.
         */
        void setCloseAction( const std::function< bool() >& closeAction );

        /// moves second window as tab onto first window
        void tabifyDockWidget( DockWindow& firstWindow, DockWindow& secondWindow );

        /// Closes the window.
        void close();

        /// Reads saved window states
        void loadState( const std::string& organization, const std::string& application, const std::string& name );

        /// Stores window states
        void
        storeState( const std::string& organization, const std::string& application, const std::string& name ) const;

        /// maximizes the window
        void showMainWindowMaximized();

        /// Sets the icon of the window.
        void setIcon( const Icon& icon );

        /// Sets the size of the window.
        void setSize( std::size_t width, std::size_t height );

        /// Sets the title of the window.
        void setTitle( const std::string& title );

        /**
         * Register callback for when the window is shown/hidden.
         * This is used to show the correct visibility status of algorithm visual outputs.
         * Thus it should not be used from within algorithms!
         * \internal
         */
        void setShowHideCallback( const std::function< void( bool ) >& callback = nullptr );
    };

    //=========================================================================================================================

    /// layout that can be added to a main window as a status bar which arranges its widgets horizontally or vertically
    /// \ingroup GUI */
    class StatusBoxLayout : public Widget
    {
        F_GUI_DECLARE_PRIVATE( StatusBoxLayout )

    public:
        /// Creates a new layout.
        /**
         * \param parent the parent window that will get a status bar
         * \param horizontal whether the widgets should be arranged horizontally or vertically.
         */
        StatusBoxLayout( MainWindow& parent, bool horizontal );

        /// Adds a stretch element at the end of the box layout
        void addStretch( int stretch = 0 );

        /// Sets margin in pixels.
        void setMargin( std::size_t margin );

        /// Sets spacing between widgets in pixels.
        void setSpacing( std::size_t spacing );

        /// Sets the stretching of a widget to factor
        void setStretchFactor( int factor, Widget* widget );

    protected:
        StatusBoxLayout( MainWindow& parent, gui_private::StatusBoxLayoutPrivate* pimpl );
    };

    //=========================================================================================================================

    /// sub-level window \ingroup GUI
    /**
     * After creation, exactly one child widget can be added to the dock window, that represents the content of the
     * window.
     * Dock-Windows will be deleted by their parent MainWindow, if not deleted explicitely before that.
     */
    class DockWindow : public Widget
    {
    public:
        F_GUI_DECLARE_PRIVATE( DockWindow )

        /// location of the sub-level window inside its parent
        enum Location
        {
            FFREE,  ///< free floating
            FLEFT,  ///< left docking area
            FRIGHT, ///< right docking area
            FTOP,   ///< top docking area
            FBOTTOM ///< bottom docking area
        };

        /// Creates new window inside a top-level window.
        /**
         * \param w the top-level window.
         * \param location the location inside the top-level window.
         * \param title the initial window title.
         * \param closeAction the callback for when the window is closing.
         * If closeAction is valid function, it will be called when the window
         * is about to be closed. The window will only be closed if closeAction returns true.
         */
        DockWindow( MainWindow& w,
                    Location location,
                    const std::string& title,
                    const std::function< bool() >& closeAction = nullptr );

        /** Sets the closeAction callback.
         * If closeAction is valid function, it will be called when the window
         * is about to be closed. The window will only be closed if closeAction returns true.
         */
        void setCloseAction( const std::function< bool() >& closeAction );

        /// Sets the location of the window inside the top-level window.
        void setLocation( Location location );

        /// Sets the size of the widget
        void setSize( size_t width, size_t height );

        /// Sets the window title.
        void setTitle( const std::string& title );

        /**
         * Register callback for when the window is shown/hidden.
         * This is used to show the correct visibility status of algorithm visual outputs.
         * Thus it should not be used from within algorithms!
         * \internal
         */
        void setShowHideCallback( const std::function< void( bool ) >& callback = nullptr );
    };

    //=========================================================================================================================

    class MenuItem;
    /// Menu bar or context menu \ingroup GUI
    /**
     * Subclass for dynamically generated menus.
     */
    class Menu : private gui_private::ItemHolder< MenuItem >
    {
        gui_private::MenuPrivate* mPimpl;
        F_GUI_DECLARE_PRIVATE( Menu )

    public:
        /// Creates a new context menu.
        Menu( Widget& parent );
        /// Creates a new menu bar
        Menu( MainWindow& parent );

        /// Destroys menu.
        virtual ~Menu();

        /// can be overridden to generate the menu dynamically before showing. Only supported on context menus.
        virtual void beforeShow() {}

        /// useful in the context of auto-generated menus
        virtual void afterHide() {}

    private:
        gui_private::ItemHolder< MenuItem >* itemHolder();
        Widget* mParent;

        friend class MenuItem;
    };

    //=========================================================================================================================

    /// menu item \ingroup GUI
    /**
     * Create new items with \a this as parent to create submenus
     */
    class MenuItem : private gui_private::ItemHolder< MenuItem >
    {
        gui_private::MenuItemPrivate* mPimpl;
        F_GUI_DECLARE_PRIVATE( MenuItem )

    public:
        /// Creates new menu item.
        /**
         * \param m the parent menu.
         * \param text the menu item's text.
         * \param checkable determines whether the menu item should have a checkbox
         * \param action the callback for when the menu item has been selected (parameter gives checked state).
         */
        MenuItem( Menu& m,
                  const std::string& text,
                  bool checkable,
                  const std::function< void( bool ) >& action = nullptr );

        /// Creates new menu item.
        /**
         * \param m the parent menu.
         * \param text the menu item's text.
         * \param checkable determines whether the menu item should have a checkbox
         * \param action the callback for when the menu item has been selected (parameter gives checked state).
         */
        MenuItem( MenuItem& m,
                  const std::string& text,
                  bool checkable,
                  const std::function< void( bool ) >& action = nullptr );

        /// Creates new uncheckable menu item.
        /**
         * \param m the parent menu.
         * \param text the menu item's text.
         * \param action the callback for when the menu item has been selected (parameter gives checked state).
         */
        MenuItem( Menu& m, const std::string& text, const std::function< void( bool ) >& action = nullptr );

        /// Creates new uncheckable menu item.
        /**
         * \param m the parent menu.
         * \param text the menu item's text.
         * \param action the callback for when the menu item has been selected (parameter gives checked state).
         */
        MenuItem( MenuItem& m, const std::string& text, const std::function< void( bool ) >& action = nullptr );

        /// Destroys menu item.
        virtual ~MenuItem();

        /// Sets the callback for when the menu item has been selected (parameter gives checked state).
        void setAction( const std::function< void( bool ) >& action );

        /**
         * Resets the text shown in the menu entry
         * \param text the new text of the entry
         */
        void setText( const std::string& text );

        /**
         * Sets a keyboard shortcut for the menu entry
         * \param shortcut the keyboard shortcut ( currently according to QT's string guidelines )
         */
        void setShortcutSequence( const std::string& shortcut );

        /**
         * Returns status of action check
         **/
        bool isChecked() const;

        /**
         * Sets the check status
         * \param status
         */
        void setChecked( bool status );

        /**
         * Sets ability to check the action
         * \param status of ability
         */
        void setCheckable( bool status );

        /**
         * Activates or deactivates the menu entry on platforms that support it.
         * Otherwise, the menu entry will remain active.
         */
        void setActive( bool isActive );

    protected:
        MenuItem( MenuItem& m, gui_private::MenuItemPrivate* pimpl );
        MenuItem( Menu& m, gui_private::MenuItemPrivate* pimpl );

    private:
        gui_private::ItemHolder< MenuItem >* itemHolder();
        gui_private::ItemHolder< MenuItem >* mParent;
    };

    //=========================================================================================================================

    /// menu separator \ingroup GUI
    /**
     * Not meant to be subclassed.
     */
    class MenuSeparator final : public MenuItem
    {
    public:
        /// Creates new menu separator.
        MenuSeparator( Menu& m );
        MenuSeparator( MenuItem& m );
    };

    //=========================================================================================================================

    class ToolBarItem;
    /// tool bar \ingroup GUI
    class ToolBar : private gui_private::ItemHolder< ToolBarItem >
    {
        gui_private::ToolBarPrivate* mPimpl;
        MainWindow* mParent;
        F_GUI_DECLARE_PRIVATE( ToolBar )

    public:
        /// Creates a new tool bar.
        ToolBar( MainWindow& w, const std::string& title );

        /// Destroys tool bar.
        virtual ~ToolBar();

    private:
        friend class ToolBarItem;
    };

    //=========================================================================================================================

    /// tool bar item \ingroup GUI
    class ToolBarItem
    {
        gui_private::ToolBarItemPrivate* mPimpl;
        F_GUI_DECLARE_PRIVATE( ToolBarItem )

        ToolBar* mParent;

    public:
        /// Creates a new item in a tool bar.
        ToolBarItem( ToolBar& t, const std::string& text, const std::function< void() >& action = nullptr );

        /// Creates a new item in a tool bar.
        ToolBarItem( ToolBar& t,
                     const std::string& text,
                     const Icon& icon,
                     const std::function< void() >& action = nullptr );

        virtual ~ToolBarItem();

        /// Sets the callback for when the toolbar item is clicked.
        void setAction( const std::function< void() >& action );

    protected:
        ToolBarItem( ToolBar& t, gui_private::ToolBarItemPrivate* pimpl );
    };

    //=========================================================================================================================

    /// tool bar separator \ingroup GUI
    /**
     * Not meant to be subclassed.
     */
    class ToolBarSeparator final : public ToolBarItem
    {

    public:
        /// Creates a new separator in a tool bar.
        ToolBarSeparator( ToolBar& t );
    };

    //=========================================================================================================================

    /// timer \ingroup GUI
    /**
     * Not meant to be subclassed.
     */
    class Timer final
    {
        gui_private::TimerPrivate* mPimpl;
        F_GUI_DECLARE_PRIVATE( Timer )

    public:
        /// Create timer with specific frequency.
        Timer( double frequency );

        /// Destroys timer.
        virtual ~Timer();

        /// change frequency
        void init( double frequency );

        /// Sets callback for when timer ticks.
        /**
         * \param action the callback for when timer ticks.
         * \note Also starts or stops the timer based on whether the function is valid.
         */
        void setAction( const std::function< void() >& action = nullptr );
    };

} // namespace fantom
