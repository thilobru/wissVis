#pragma once

#include "../gui.hpp"
#include "../utils.hpp"
#include <functional>
#include <memory>
#include <string>

namespace fantom
{

    namespace gui_private
    {

        class GuiFactory
        {
        public:
            virtual ~GuiFactory();

            virtual SplashscreenWidgetPrivate* createSplashscreenWidget( SplashscreenWidget& splashscreen ) = 0;
            virtual BoxLayoutPrivate* createBoxLayout( BoxLayout& boxLayout, Widget& parent ) = 0;
            virtual GridLayoutPrivate* createGridLayout( GridLayout& gridLayout, Widget& parent ) = 0;
            virtual StatusBoxLayoutPrivate* createStatusBoxLayout( StatusBoxLayout& boxLayout, MainWindow& parent ) = 0;
            virtual FormWidgetPrivate* createFormWidget( FormWidget& formWidget, Widget& parent ) = 0;
            virtual TabWidgetPrivate* createTabWidget( TabWidget& widget, Widget& parent ) = 0;
            virtual GroupBoxPrivate* createGroupBox( GroupBox& widget, Widget& parent ) = 0;
            virtual ScrollAreaPrivate* createScrollArea( ScrollArea& scrollArea, Widget& parent ) = 0;
            virtual LabelPrivate* createLabel( Label& label, Widget& parent, bool expandable ) = 0;
            virtual ProgressBarPrivate* createProgressBar( ProgressBar& progressBar, Widget& parent ) = 0;
            virtual SeparatorPrivate* createSeparator( Separator& separator, Widget& parent, bool horizontal ) = 0;
            virtual PushButtonPrivate* createPushButton( PushButton& pushButton, Widget& parent ) = 0;
            virtual CheckBoxPrivate* createCheckBox( CheckBox& checkBox, Widget& parent ) = 0;
            virtual SliderPrivate* createSlider( Slider& slider, Widget& parent, bool horizontal ) = 0;
            virtual ComboBoxPrivate* createComboBox( ComboBox& comboBox, Widget& parent ) = 0;
            virtual LineEditPrivate* createLineEdit( LineEdit& textBox, Widget& parent ) = 0;
            virtual HighlightedTextEditPrivate* createHighlightedTextEdit( HighlightedTextEdit& textBox,
                                                                           Widget& parent )
                = 0;
            virtual ShellPrivate* createShell( Shell& shell, Widget& parent ) = 0;
            virtual TreeViewPrivate* createTreeView( TreeView& treeView, Widget& parent ) = 0;
            virtual TreeItemPrivate* createTreeItem( TreeItem& treeItem, TreeView& parent ) = 0;
            virtual TreeItemPrivate* createTreeItem( TreeItem& treeItem, TreeItem& parent ) = 0;
            virtual OffscreenGlContextPrivate* createOffscreenGlContext( OffscreenGlContext& context, GlContext* share )
                = 0;
            virtual GlAreaPrivate*
            createGlArea( GlArea& glArea, Widget& parent, std::unique_ptr< GlPainter > painter, GlContext* share )
                = 0;
            virtual DockWindowPrivate* createDockWindow( DockWindow& window, MainWindow& parent ) = 0;
            virtual MainWindowPrivate* createMainWindow( MainWindow& mainWindow ) = 0;
            virtual MenuPrivate* createMenu( Menu& menu, MainWindow& parent ) = 0;
            virtual MenuPrivate* createMenu( Menu& menu, Widget& parent ) = 0;
            virtual MenuItemPrivate* createMenuItem( MenuItem& menuItem, MenuItem& parent ) = 0;
            virtual MenuItemPrivate* createMenuItem( MenuItem& menuItem, Menu& parent ) = 0;
            virtual MenuItemPrivate* createMenuSeparator( MenuItem& menuItem, MenuItem& parent ) = 0;
            virtual MenuItemPrivate* createMenuSeparator( MenuItem& menuItem, Menu& parent ) = 0;
            virtual ToolBarPrivate* createToolBar( ToolBar& toolBar, MainWindow& parent ) = 0;
            virtual ToolBarItemPrivate* createToolBarItem( ToolBarItem& toolbarItem, ToolBar& parent ) = 0;
            virtual ToolBarItemPrivate* createToolBarSeparator( ToolBarItem& toolbarItem, ToolBar& parent ) = 0;
            virtual TimerPrivate* createTimer( Timer& timer ) = 0;
            virtual SceneView2DPrivate* createSceneView2D( SceneView2D& sceneView,
                                                           Widget& parent,
                                                           std::unique_ptr< Graphics2D::ScenePainter > painter,
                                                           bool scrollable,
                                                           bool useOpenGL )
                = 0;

            template < typename T >
            static auto getP( T& parent ) -> decltype( std::declval< T >().p() )
            {
                return parent.p();
            }
        };

        class WidgetPrivate
        {
        protected:
            Widget* mPumpl;
            F_GUI_DECLARE_PUBLIC( Widget )
        public:
            WidgetPrivate( Widget& pumpl )
                : mPumpl( &pumpl )
            {
            }

            virtual ~WidgetPrivate();

            virtual bool isVisible() const = 0;
            virtual void setEnabled( bool enabled ) = 0;
            virtual void setVisible( bool visible ) = 0;
            virtual void setToolTip( const std::string& text ) = 0;


            // drag and drop implementation

            //! reimplement the function to accept drop events
            /**
             * has to be called by the actual implementation to
             * distribute drag enter events to listening drop areas
             */
            bool dragEnterEvent( const DropHandler::DropEvent& event );

            //!
            /**
             * has to be called by the actual implementation to
             * distribute drag move events to listening drag-move areas
             */
            bool dragMoveEvent( const DropHandler::DropEvent& event );

            /**
             * has to be called by the actual implementation to
             * distribute drop events to listening drop areas
             */
            bool dropEvent( const DropHandler::DropEvent& event );
        };

        class SplashscreenWidgetPrivate : public WidgetPrivate
        {
        protected:
            SplashscreenWidget* mPumpl;
            F_GUI_DECLARE_PUBLIC( SplashscreenWidget )
        public:
            SplashscreenWidgetPrivate( SplashscreenWidget& pumpl )
                : WidgetPrivate( pumpl )
                , mPumpl( &pumpl )
            {
            }

            virtual ~SplashscreenWidgetPrivate();

            virtual bool isVisible() const = 0;
            virtual void setEnabled( bool enabled ) = 0;
            virtual void setVisible( bool visible ) = 0;
            virtual void setToolTip( const std::string& text ) = 0;
            virtual void setMessage( const std::string& text ) = 0;

            virtual void setPixmap( const Icon& icon ) = 0;
        };


        class BoxLayoutPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( BoxLayout )
        public:
            BoxLayoutPrivate( BoxLayout& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void addStretch( int factor ) = 0;
            virtual void setDirection( bool horizontal ) = 0;
            virtual void setMargin( std::size_t margin ) = 0;
            virtual void setSpacing( std::size_t spacing ) = 0;
            virtual void setStretchFactor( int factor, Widget& widget ) = 0;
        };

        class GridLayoutPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( GridLayout )
        public:
            GridLayoutPrivate( GridLayout& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setMargin( std::size_t leftMargin,
                                    std::size_t topMargin,
                                    std::size_t rightMargin,
                                    std::size_t bottomMargin )
                = 0;
            virtual void setSpacing( std::size_t horizontalSpacing, std::size_t verticalSpacing ) = 0;
            virtual void setWidgetPosition(
                Widget& child, std::size_t xPos, std::size_t yPos, std::size_t xSize = 1, std::size_t ySize = 1 )
                = 0;
            virtual void setColumnStretchFactor( std::size_t column, std::size_t factor ) = 0;
        };

        class StatusBoxLayoutPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( StatusBoxLayout )
        public:
            StatusBoxLayoutPrivate( StatusBoxLayout& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void addStretch( int stretch = 0 ) = 0;
            virtual void setDirection( bool horizontal ) = 0;
            virtual void setMargin( std::size_t margin ) = 0;
            virtual void setSpacing( std::size_t spacing ) = 0;
            virtual void setStretchFactor( int factor, Widget* widget ) = 0;
        };

        class FormWidgetPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( FormWidget )
        public:
            FormWidgetPrivate( FormWidget& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setCaption( WidgetPrivate& widget, const std::string& text, bool bold = false ) = 0;

            virtual void alignToBottom() = 0;
        };

        class TabWidgetPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( TabWidget )
        public:
            TabWidgetPrivate( TabWidget& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setText( WidgetPrivate& child, const std::string& text ) = 0;
        };

        class GroupBoxPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( GroupBox )
        public:
            GroupBoxPrivate( GroupBox& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setCaption( const std::string& caption ) = 0;
        };

        class ScrollAreaPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( ScrollArea )
        public:
            ScrollAreaPrivate( ScrollArea& pumpl )
                : WidgetPrivate( pumpl )
            {
            }
        };

        class TreeItemPrivate
        {
            TreeItem* mPumpl;

        protected:
            F_GUI_DECLARE_PUBLIC( TreeItem )
        public:
            TreeItemPrivate( TreeItem& pumpl )
                : mPumpl( &pumpl )
            {
            }
            virtual ~TreeItemPrivate();
            virtual const std::string getText() const = 0;
            virtual void setText( std::size_t i, const std::string& text ) = 0;
            virtual void setAction( const std::function< void() >& action ) = 0;
            virtual void setDoubleClickAction( const std::function< void() >& action ) = 0;
            virtual void setToolTip( const std::string& toolTipString ) = 0;
            virtual void setIcon( const Icon& icon, int column ) = 0;
            virtual bool getChecked() const = 0;
            virtual void setChecked( bool checked ) = 0;
            virtual void setTextColorBlack() = 0;
            virtual void setTextColorGrey() = 0;
            virtual void setCheckUpdate( const std::function< void( bool ) >& update ) = 0;
            virtual void setExpanded( bool expanded ) = 0;
        };


        class TreeViewPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( TreeView )
        public:
            TreeViewPrivate( TreeView& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setHeading( const std::vector< std::string >& heading ) = 0;
            virtual void selectItem( TreeItemPrivate& item ) = 0;
        };


        class LabelPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( Label )
        public:
            LabelPrivate( Label& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setText( const std::string& text ) = 0;
        };

        class ProgressBarPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( ProgressBar )
        public:
            ProgressBarPrivate( ProgressBar& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setValue( int value ) = 0;
            virtual void setText( const std::string& text ) = 0;
        };

        class SeparatorPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( Separator )
        public:
            SeparatorPrivate( Separator& pumpl )
                : WidgetPrivate( pumpl )
            {
            }
        };

        class PushButtonPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( PushButton )
        public:
            PushButtonPrivate( PushButton& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setText( const std::string& text ) = 0;
            virtual void setIcon( const Icon& icon ) = 0;
            virtual void setAction( const std::function< void() >& action ) = 0;
        };


        class CheckBoxPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( CheckBox )
        public:
            CheckBoxPrivate( CheckBox& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setAction( const std::function< void( bool ) >& action ) = 0;
            virtual void set( bool value ) = 0;
            virtual bool get() const = 0;
            virtual void setLabel( const std::string& textLabel ) = 0;
        };


        class SliderPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( Slider )
        public:
            SliderPrivate( Slider& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setMax( size_t max ) = 0;
            virtual void setAction( const std::function< void( size_t ) >& action ) = 0;
            virtual void set( std::size_t value ) = 0;
            virtual std::size_t get() const = 0;
        };


        class ComboBoxPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( ComboBox )
        public:
            ComboBoxPrivate( ComboBox& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setAction( const std::function< void( size_t ) >& update ) = 0;
            virtual void setData( const std::vector< std::string >& data, const std::vector< Icon >& icons ) = 0;
            virtual void set( std::size_t value ) = 0;
            virtual std::size_t get() const = 0;
        };


        class LineEditPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( LineEdit )
        public:
            LineEditPrivate( LineEdit& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setAction( const std::function< void( const std::string& ) >& update ) = 0;
            virtual void set( const std::string& value ) = 0;
            virtual std::string get() const = 0;

            virtual void setBackgroundColor( const Color& color ) = 0;
            virtual void setForegroundColor( const Color& color ) = 0;

            // virtual void setLanguage( const std::string& language) = 0;
        };

        class HighlightedTextEditPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( HighlightedTextEdit )
        public:
            HighlightedTextEditPrivate( HighlightedTextEdit& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setAction( const std::function< void( const std::string& ) >& update ) = 0;
            virtual void set( const std::string& value ) = 0;
            virtual std::string get() const = 0;

            virtual void setBackgroundColor( const Color& color ) = 0;
            virtual void setForegroundColor( const Color& color ) = 0;

            virtual void setLanguage( const std::string& language ) = 0;
        };


        class ShellPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( Shell )
        public:
            ShellPrivate( Shell& pumpl )
                : WidgetPrivate( pumpl )
            {
            }
            virtual void setBackgroundColor( Color color ) = 0;
            virtual void addText( const std::string& text, Color color, bool withPrompt ) = 0;
            virtual std::string getCommand() = 0;
            virtual void setCommand( const std::string& newCommand ) = 0;
        };

        class OffscreenGlContextPrivate
        {
        public:
            virtual ~OffscreenGlContextPrivate();

            virtual void executeGL( const std::function< void() >& commands, bool sync ) = 0;
        };

        class GlAreaPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( GlArea )
        public:
            GlAreaPrivate( GlArea& pumpl )
                : WidgetPrivate( pumpl )
            {
            }
            virtual GlPainter& get() = 0;
        };

        class SceneView2DPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( SceneView2D )
        public:
            SceneView2DPrivate( SceneView2D& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual Graphics2D::ScenePainter& get() = 0;

            virtual const Graphics2D::ScenePainter& get() const = 0;
        };

        class DockWindowPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( DockWindow )
        public:
            DockWindowPrivate( DockWindow& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void setLocation( DockWindow::Location location ) = 0;
            virtual void setSize( size_t width, size_t height ) = 0;
            virtual void setTitle( const std::string& title ) = 0;
            virtual void setCloseAction( const std::function< bool() >& closeAction ) = 0;
            virtual void setShowHideCallback( const std::function< void( bool ) >& callback ) = 0;
        };

        class MainWindowPrivate : public WidgetPrivate
        {
        protected:
            F_GUI_DECLARE_PUBLIC( MainWindow )
        public:
            MainWindowPrivate( Widget& pumpl )
                : WidgetPrivate( pumpl )
            {
            }

            virtual void tabifyDockWidget( DockWindow& firstWindow, DockWindow& secondWindow ) = 0;
            virtual void setCloseAction( const std::function< bool() >& closeAction ) = 0;
            virtual void close() = 0;
            virtual void showMainWindowMaximized() = 0;
            virtual void setIcon( const Icon& icon ) = 0;
            virtual void setSize( std::size_t width, std::size_t height ) = 0;
            virtual void setTitle( const std::string& title ) = 0;

            virtual void
            loadState( const std::string& organization, const std::string& application, const std::string& name )
                = 0;

            virtual void storeState( const std::string& organization,
                                     const std::string& application,
                                     const std::string& name ) const = 0;
            virtual void setShowHideCallback( const std::function< void( bool ) >& callback ) = 0;
        };

        class MenuPrivate
        {
            Menu* mPumpl;

        protected:
            F_GUI_DECLARE_PUBLIC( Menu )
        public:
            MenuPrivate( Menu& pumpl )
                : mPumpl( &pumpl )
            {
            }
            virtual ~MenuPrivate();
        };


        class MenuItemPrivate
        {
            MenuItem* mPumpl;

        protected:
            F_GUI_DECLARE_PUBLIC( MenuItem )
        public:
            MenuItemPrivate( MenuItem& pumpl )
                : mPumpl( &pumpl )
            {
            }
            virtual ~MenuItemPrivate();

            virtual void setAction( const std::function< void( bool ) >& action ) = 0;
            virtual void setText( const std::string& text ) = 0;
            virtual void setShortcutSequence( const std::string& shortcut ) = 0;
            virtual bool isChecked() const = 0;
            virtual void setChecked( bool status ) = 0;
            virtual void setCheckable( bool status ) = 0;
            virtual void setActive( const bool active ) = 0;
        };

        class ToolBarPrivate
        {
            ToolBar* mPumpl;

        protected:
            F_GUI_DECLARE_PUBLIC( ToolBar )
        public:
            ToolBarPrivate( ToolBar& pumpl )
                : mPumpl( &pumpl )
            {
            }
            virtual ~ToolBarPrivate();
            virtual void setTitle( const std::string& title ) = 0;
        };


        class ToolBarItemPrivate
        {
            ToolBarItem* mPumpl;

        protected:
            F_GUI_DECLARE_PUBLIC( ToolBarItem )
        public:
            ToolBarItemPrivate( ToolBarItem& pumpl )
                : mPumpl( &pumpl )
            {
            }
            virtual ~ToolBarItemPrivate();
            virtual void setText( const std::string& text ) = 0;
            virtual void setAction( const std::function< void() >& action ) = 0;
            virtual void setIcon( const Icon& icon ) = 0;
        };

        class TimerPrivate
        {
            Timer* mPumpl;

        protected:
            F_GUI_DECLARE_PUBLIC( Timer )
        public:
            TimerPrivate( Timer& pumpl )
                : mPumpl( &pumpl )
            {
            }
            virtual ~TimerPrivate();

            virtual void init( double frequency ) = 0;
            virtual void setAction( const std::function< void() >& action ) = 0;
        };
    } // namespace gui_private
} // namespace fantom
