#pragma once

#include <memory>
#include <string>
#include <vector>

#include "math.hpp"

namespace fantom
{
    namespace Graphics2D
    {
        class Image;
    }

    /// Codes for modifier keys
    enum class ModifierKey : int
    {
        Shift = 1,   ///< a Shift key is pressed
        Control = 2, ///< a Ctrl key is pressed (corresponds to the Command key on Mac OS X)
        Alt = 4,     ///< an Alt key is pressed
        Meta = 8, ///< a Meta key is pressed (corresponds to the Windows key on Windows and the Control key on Mac OS X)
    };

    struct ModifierKeys
    {
        ModifierKeys()
            : mModifiers( ModifierKey( 0 ) )
        {
        }

        void add( ModifierKey key )
        {
            mModifiers = ModifierKey( int( mModifiers ) | int( key ) );
        }

        bool contains( ModifierKey key ) const
        {
            return ( int( mModifiers ) & int( key ) ) != 0;
        }

        bool isEmpty() const
        {
            return int( mModifiers ) == 0;
        }

    private:
        ModifierKey mModifiers;
    };

    /// Key codes for common default keys
    enum class Key : int
    {
        Escape = 0x01000000,
        Tab = 0x01000001,
        Backspace = 0x01000003,
        Return = 0x01000004,
        Enter = 0x01000005, ///< located on the keypad
        Insert = 0x01000006,
        Delete = 0x01000007,
        Pause = 0x01000008, ///< Pause/Break key
        Print = 0x01000009,
        Home = 0x01000010,
        End = 0x01000011,
        Left = 0x01000012,
        Up = 0x01000013,
        Right = 0x01000014,
        Down = 0x01000015,
        PageUp = 0x01000016,
        PageDown = 0x01000017,
        Shift = 0x01000020,   ///< see \c ModifierKey
        Control = 0x01000021, ///< see \c ModifierKey
        Meta = 0x01000022,    ///< see \c ModifierKey
        Alt = 0x01000023,     ///< see \c ModifierKey
        F1 = 0x01000030,
        F2 = 0x01000031,
        F3 = 0x01000032,
        F4 = 0x01000033,
        F5 = 0x01000034,
        F6 = 0x01000035,
        F7 = 0x01000036,
        F8 = 0x01000037,
        F9 = 0x01000038,
        F10 = 0x01000039,
        F11 = 0x0100003a,
        F12 = 0x0100003b,
        Space = 0x00000020,
    };

    /// Codes for mouse buttons
    enum class MouseButton : int
    {
        None = 0,
        Left = 1,
        Right = 2,
        Middle = 4
    };

    /// Mouse wheel orientation.
    enum class WheelType : int
    {
        Horizontal, //!< horizontal mouse wheel
        Vertical    //!< vertical mouse wheel
    };

    /// Event handler for drop events.
    /**
     * The behavior follows that of QT's drag and drop:
     * Whenever a drag enters the widget, dragEnterEvent is called
     * and the event carries information about the dragged items.
     * To receive further events, the event has to be accepted.
     *
     * If the event has been accepted in the dragEnterEvent, on
     * every move, a dragMoveEvent is raised. The event has to be
     * accepted to receive drop events.
     *
     * If the last dragMoveEvent has been accepted, a drop event is
     * raised upon the release of the mouse and the application
     * should perform the appropriate actions. In this case, a better
     * checking of the event should take place and the event should
     * only be accepted when the drop event has been completed
     * successfully.
     */
    class DropHandler
    {
    public:
        class DropEvent;

        virtual ~DropHandler() = default;

        // drag and drop implementation

        //! reimplement the function to accept drop events
        /**
         * This function should accept the event
         * if the window accepts drops. Otherwise, no
         * other drag action for the same item may be
         * triggered.
         * \return true if the event has been accepted
         */
        virtual bool dragEnterEvent( const DropEvent& event ) = 0;

        //! reimplement the function to accept drop events
        /**
         * Reimplement and accept the event whenever
         * the item has been moved over an item that accepts
         * the event. If the event is not accepted,
         * dropEvent will never happen.
         * \return true if the event has been accepted
         */
        virtual bool dragMoveEvent( const DropEvent& event ) = 0;

        //! reimplement this function to handle drop events
        /**
         * called when the dragged item is released.
         * Accept the event whenever the drop has succeeded.
         * Otherwise, ignore the event and return false.
         * The internal gui implementation shall indicate
         * the failure appropriately
         * \return true if the event has been accepted
         */
        virtual bool dropEvent( const DropEvent& event ) = 0;
    };

    //=========================================================================================================================

    /// Data structure to communicate drag and drop events
    /**
     * The event class is given by the gui to communicate events
     */
    class DropHandler::DropEvent
    {
    public:
        DropEvent();
        ~DropEvent();

        /**
         * \returns the location of the event in window coordinates,
         * typically this is the gui's pixel coordinates
         *
         * QT4's coordinate system of communicating events is the
         * default for now.
         */
        Pos2D pos() const;

        /**
         * \returns true if the event contains a color
         */
        bool hasColor() const;

        /**
         * if the event contains a color, the color is returned
         */
        const Color& color() const;

        /**
         * if the event contains a test, the test is returned
         */
        const std::string& getText() const;

        /**
         * \returns true, if the event has file names
         */
        bool hasFileNames() const;

        /**
         * \returns the file names associated with the event
         */
        std::vector< std::string > fileNames() const;

        /**
         * \returns true, if the event has an image
         */
        bool hasImage() const;

        /**
         * \returns true, if the event has a text set
         */
        bool hasText() const;

        /**
         * \pre hasImage must be checked and return true
         * \returns the attached pixmap
         */
        const Graphics2D::Image& getImage() const;

        //! internal interface
        void setColor( const Color& color );

        //! internal interface
        void addUrl( const std::string& file );

        //! internal interface
        void setImage( std::unique_ptr< Graphics2D::Image > image );

        //! internal interface
        void setText( const std::string& test );

        void setPos( const Pos2D& pos );

    private:
        // the event's data about the drag and drop content
        bool mHasColor;
        Color mColor;
        std::vector< std::string > mFiles;
        Pos2D mPos;
        std::unique_ptr< Graphics2D::Image > mImage;
        std::string mText;
    };

    //=========================================================================================================================

    /// interface for a generic event handler for use in graphic views \ingroup GUI
    /**
     * Usually, all methods are called from the GUI thread.
     * The default implementation ignores all events and passes them on.
     */
    class EventHandler
    {

    public:
        class KeyEvent;
        class MouseEvent;
        class WheelEvent;

        virtual ~EventHandler()
        {
        }

        /// Change drawing area's size.
        /**
         * \param size the new size of the drawing area.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual void resize( Size2D size );

        /// Process key press event.
        /**
         * \param event the key event
         * \return true if the event was accepted, false if the event should be passed on
         * \pre nothing.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual bool keyPress( const KeyEvent& event );

        /// Process key release event.
        /**
         * \param event the key event
         * \return true if the event was accepted, false if the event should be passed on
         * \pre nothing.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual bool keyRelease( const KeyEvent& event );

        /// Process mouse double click event.
        /**
         * \param event the mouse event
         * \return true if the event was accepted, false if the event should be passed on
         * \pre nothing.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual bool mouseDoubleClick( const MouseEvent& event );

        /// Process mouse move event.
        /**
         * \param event the mouse event
         * \return true if the event was accepted, false if the event should be passed on
         * \pre nothing.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual bool mouseMove( const MouseEvent& event );

        /// Process mouse button press event.
        /**
         * \param event the mouse event
         * \return true if the event was accepted, false if the event should be passed on
         * \pre nothing.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual bool mousePress( const MouseEvent& event );

        /// Process mouse button release event.
        /**
         * \param event the mouse event
         * \return true if the event was accepted, false if the event should be passed on
         * \pre nothing.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual bool mouseRelease( const MouseEvent& event );

        /// Process mouse wheel event.
        /**
         * \param event the wheel event
         * \return true if the event was accepted, false if the event should be passed on
         * \pre nothing.
         * \post nothing.
         * \note Thread-safe.
         * \throws nothing.
         */
        virtual bool mouseWheel( const WheelEvent& event );

    protected:
        EventHandler()
        {
        }

    private:
        EventHandler( const EventHandler& ) = delete;
        EventHandler& operator=( const EventHandler& ) = delete;
    };

    class EventHandler::KeyEvent
    {
    public:
        KeyEvent( Key key, ModifierKeys modifiers, std::string text )
            : mModifiers( modifiers )
            , mKey( key )
            , mText( text )
        {
        }

        /// Returns a string containing the text, that has been generated by the key. May be empty.
        const std::string& getText() const
        {
            return mText;
        }

        /// Returns the key code for this event.
        Key getKey() const
        {
            return mKey;
        }

        /// Returns the modifier codes for this event.
        const ModifierKeys& getModifiers() const
        {
            return mModifiers;
        }

    private:
        ModifierKeys mModifiers;
        Key mKey;
        std::string mText;
    };

    class EventHandler::MouseEvent
    {
    public:
        MouseEvent( Pos2D pos, MouseButton button, ModifierKeys modifiers )
            : mModifiers( modifiers )
            , mButton( button )
            , mPos( pos )
        {
        }

        /// Returns the coordinates of the mouse events relative to
        /// the drawing area. They may be outside the drawing area.
        const Pos2D& getPosition() const
        {
            return mPos;
        }

        /// Returns the mouse button for the event.
        MouseButton getButton() const
        {
            return mButton;
        }

        /// Returns the modifier keys for this event.
        const ModifierKeys& getModifiers() const
        {
            return mModifiers;
        }

    private:
        ModifierKeys mModifiers;
        MouseButton mButton;
        Pos2D mPos;
    };

    class EventHandler::WheelEvent : public MouseEvent
    {
    public:
        WheelEvent( Pos2D pos, WheelType type, float amount, MouseButton button, ModifierKeys modifiers )
            : MouseEvent( pos, button, modifiers )
            , mAmount( amount )
            , mType( type )
        {
        }

        /// The amount to scroll in degrees (positive-away, negative-closer). Usually, one step of the scroll wheel is
        /// 15 degrees.
        float getAmount() const
        {
            return mAmount;
        }

        /// The type of the wheel.
        WheelType getWheel() const
        {
            return mType;
        }

    private:
        float mAmount;
        WheelType mType;
    };

} // namespace fantom
