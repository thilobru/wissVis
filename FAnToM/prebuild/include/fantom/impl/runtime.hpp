#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace fantom
{

    namespace gui_private
    {
        class GuiFactory;
    }
    namespace graphics2D_private
    {
        class Graphics2DFactory;
    }

    class Color;

    /// Color Chooser widget \ingroup GUI
    /**
     * An abstract interface class whose main purpose is to hold the platform-dependent color dialog
     * The unique pointer to this class has to be kept alive as long as the widget exists
     * \ingroup GUI
     */
    class ColorChooser
    {
    protected:
        ColorChooser()
        {
        }

    public:
        /**
         * @brief updates the dialog's current color
         * @param color the new color
         */
        virtual void setColor( const Color& color ) = 0;

        virtual ~ColorChooser()
        {
        }
    };

    /**
     * \brief Base class for GUI platforms. \ingroup GUI
     * Provides the GUI main loop and application wide interactive dialogs.
     * This class follows the singleton design pattern. Only one instance can be existent.
     * This instance can be obtained with \c Runtime::instance() .
     */
    class Runtime
    {
    public:
        typedef std::function< void() > Job;

        static Runtime& instance()
        {
            return *mInstance;
        }

        virtual ~Runtime()
        {
        }

        /// Return the factory for GUI elements
        virtual gui_private::GuiFactory& guiFactory() = 0;
        virtual graphics2D_private::Graphics2DFactory& graphics2DFactory() = 0;

        /// Main loop of the application. Meant to be called only once by the application loader.
        virtual void run() = 0;
        /// Quits the application
        virtual void quit() = 0;

        /// Runs a job in the main thread and returns when the job has finished.
        virtual void runSync( const Job& job ) = 0;
        /// Runs a job in the main thread, but returns immediately.
        virtual void runAsync( const Job& job ) = 0;

        /// Checks whether there is a setting with key \p settingName stored in the persistent application settings
        virtual bool
        hasSetting( const std::string& organization, const std::string& application, const std::string& settingName )
            = 0;

        /// Access the persistent application settings store. Returns an empty string on failure.
        virtual std::string
        readSetting( const std::string& organization, const std::string& application, const std::string& settingName )
            = 0;

        /// Write to the persistent application settings store.
        virtual void storeSetting( const std::string& organization,
                                   const std::string& application,
                                   const std::string& settingName,
                                   const std::string& content )
            = 0;

        /// Restores all settings to their default value
        virtual void resetSettings( const std::string& organization, const std::string& application ) = 0;


        // -------------------------------------- application wide interactions ----------------------
        /// severity of message dialogs
        enum MessageType
        {
            FERROR,
            FWARNING,
            FINFO,
            FABOUT
        };

        /// Shows a small message dialog.
        /// Can be run from any thread.
        virtual void message( const std::string& title, MessageType type, const std::string& message ) = 0;

        /// Shows a small question dialog.
        /// Can be run from any thread.
        virtual bool question( const std::string& title,
                               const std::string& question,
                               const std::string& positive_answer,
                               const std::string& negative_answer )
            = 0;

        /// Show a small dialog querying for a user text input.
        /// Can be run from any thread.
        virtual std::string
        askForText( const std::string& title, const std::string& question, const std::string& defaultValue = "" )
            = 0;

        /// Shows the system's color chooser. This creates an application modal dialog.
        /// Can be run from any thread.
        virtual void chooseColor( const std::string& title, float color[3] ) = 0;

        /// Shows the color chooser. This creates a color chooser that runs in its own window and provides life updates
        /// using the callback function.
        /// Note: it can only be called from the GUI thread!
        virtual std::unique_ptr< ColorChooser >
        chooseColor( const std::string& title,
                     const Color& initialColor,
                     const std::function< void( const Color& ) >& callbackFunction )
            = 0;

        /// Shows the system's dialog for choosing a directory.
        /// Can be run from any thread.
        virtual std::string openDir( const std::string& title, const std::string& path ) = 0;

        /// Shows the system's dialog for choosing a file to open.
        /// Can be run from any thread.
        virtual std::string openFile( const std::string& title,
                                      const std::string& path,
                                      const std::vector< std::pair< std::string, std::string > >& extensions )
            = 0;

        /// Returns the path to the PythonTestSession file.
        virtual std::string getPythonTestSession( )
            = 0;


        /// Shows the system's dialog for choosing a list of files to open.
        /// Can be run from any thread.
        virtual std::vector< std::string >
        openFiles( const std::string& title,
                   const std::string& path,
                   const std::vector< std::pair< std::string, std::string > >& extensions )
            = 0;

        /// Shows the system's dialog for choosing a file to save.
        /// Can be run from any thread.
        virtual std::string saveFile( const std::string& title,
                                      const std::string& path,
                                      const std::vector< std::pair< std::string, std::string > >& extensions )
            = 0;

    protected:
        Runtime();

    private:
        static Runtime* mInstance;
    };

} // namespace fantom
