#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "../options.hpp"
#include "../rtti.hpp"
#include "common.hpp"

namespace fantom
{

    //------------------------------ InputUI Factory -------------------------------------

    class InputUIFactory
    {
    private:
        const std::string mName;
        size_t mPriority;
        const std::function< const RTTI::TypeId&() > mInputType;
        const std::function< std::unique_ptr< InputUI >(
            Widget&, InputBase&, std::ostream& errorLog, MainWindow& mainWindow ) >
            mMakeInputUI;

        static size_t generateId();

    public:
        InputUIFactory( const std::string& name,
                        const std::function< const RTTI::TypeId&() > inputType,
                        const std::function< std::unique_ptr< InputUI >(
                            Widget&, InputBase&, std::ostream& errorLog, MainWindow& mainWindow ) >& makeInputUI );

        const std::string& getName() const;
        const RTTI::TypeId& inputType() const;
        std::unique_ptr< InputUI >
        makeInputUI( Widget& w, InputBase& input, std::ostream& errorLog, MainWindow& mainWindow ) const;

        size_t getPriority() const;
    };

    class InputUIRegistrationService : public RegistrationService< InputUIFactory >
    {
    public:
        static InputUIRegistrationService& getInstance();

        InputUIRegistrationService();

        /// Creates a InputUI widget for the input type.
        /**
         * \throw fantom::logic_error is thrown when no matching InputUI type is registered
         */
        std::unique_ptr< InputUI >
        createMostSpecializedFor( Widget& widget, InputBase& input, std::ostream& errorLog, MainWindow& mainWindow );
    };

    //------------------------------ InputUIRegister ------------------------------

    /// registers an InputUI type at the kernel \ingroup plugins
    /**
     * Static variables of this class are used to automatically
     * register InputUIs. Example:
     * \code
     * InputUIRegister<MyInputUI> dummy();
     * \endcode
     * After registration, the InputUI type can be used by the GUI to handle algorithm inputs.
     *
     * \tparam InputUIType the type of InputUI to register. It is required to
     *       be a valid subtype of InputUI as documented at the InputUI class.
     */
    template < class InputUIType >
    class InputUIRegister
    {
    private:
        std::shared_ptr< InputUIFactory > ptr;

        static const RTTI::TypeId& inputType()
        {
            return InputUIType::getInputType();
        }
        static std::unique_ptr< InputUI >
        makeInputUI( Widget& w, InputBase& i, std::ostream& errorLog, MainWindow& mainWindow )
        {
            return std::unique_ptr< InputUI >( new InputUIType( w, i, errorLog, mainWindow ) );
        }

    public:
        /// Register InputUI in the system.
        /**
         * If this InputUI type is already registered a warning is printed to std::cerr.
         */
        InputUIRegister()
        {
            ptr.reset( new InputUIFactory( typeid( InputUIType ).name(),
                                           &InputUIRegister< InputUIType >::inputType,
                                           &InputUIRegister< InputUIType >::makeInputUI ) );
            try
            {
                InputUIRegistrationService::getInstance().registerFactory( ptr );
            }
            catch( std::exception& e )
            {
                std::cerr << "error: registration of InputUI " << ptr->getName() << " failed." << std::endl;
                std::cerr << e.what() << std::endl;
            }
            catch( ... )
            {
                std::cerr << "error: registration of InputUI " << ptr->getName() << " failed." << std::endl;
            }
        }

        /// Un-register InputUI from the system.
        ~InputUIRegister()
        {
            InputUIRegistrationService::getInstance().unregisterFactory( ptr );
        }
    };
}
