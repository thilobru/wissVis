#pragma once

#include <iosfwd>

namespace fantom
{
    class InputBase;
    class MainWindow;
    class Widget;

    /// base class for UI components connected to Inputs
    /**
     * \ingroup options
     * Subclasses need to be registered.
     * A static method const RTTI::TypeId& getInputType() needs to be provided.
     * It should return the TypeId of the most general Input this InputUI can handle and
     * is used to find the correct matching InputUI-type for a given Input-type.
     * New InputUI-Types should be registered with an \a InputUIRegister.
     */
    class InputUI
    {
    public:
        /// constructs a new InputUI object
        /**
         * All subclasses are required to match this exact constructor signature.
         * @param parent Widget to construct the needed GUI components in.
         * @param input Reference to input object. It is guaranteed, that \a input is of a type that this InputUI can
         * handle
         * @param errorLog Reference to an output stream for error reporting. The content of \a errorLog is only shown
         * on std::endl or std::flush.
         * @param mainWindow Reference to the application main window for use in extended editor features (e.g.,
         * displaying system dialogs)
         */
        InputUI( Widget& parent, InputBase& input, std::ostream& errorLog, MainWindow& mainWindow );

        /// The underlying input has been changed externally, so the gui representation has to be updated
        virtual void refresh() = 0;

        /// Rewrite the current value from the gui representation into the input
        virtual void updateInput() = 0;

        virtual ~InputUI();
    };
}
