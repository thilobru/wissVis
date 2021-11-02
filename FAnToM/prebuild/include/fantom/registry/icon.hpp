#pragma once

#include <functional>
#include <iostream>
#include <memory>

#include "../graphics2D.hpp"
#include "../rtti.hpp"
#include "common.hpp"

namespace fantom
{
    //----------------------------------- Icon Factory -------------------------------------

    class IconFactory
    {
    public:
        using Icon = Graphics2D::Image;
        typedef std::function< bool( const RTTI& ) > ApplicableFunction;

    private:
        RTTI::TypeId mObjectType;

        Graphics2D::Image mIcon;
        ApplicableFunction mIsApplicable;

        size_t mPriority;
        const std::string mName;

        static size_t generateId();

    public:
        IconFactory( const RTTI::TypeId& objectType, const Icon& icon )
            : mObjectType( objectType )
            , mIcon( icon )
            , mPriority( generateId() )
            , mName( fantom::lexical_cast< std::string >( mPriority ) )
        {
            fantom_release_assert( mIcon.isValid(), "Invalid empty icon." );
        }

        IconFactory( const RTTI::TypeId& objectType, const Icon& icon, ApplicableFunction isApplicable )
            : mObjectType( objectType )
            , mIcon( icon )
            , mIsApplicable( isApplicable )
            , mPriority( generateId() )
            , mName( fantom::lexical_cast< std::string >( mPriority ) )
        {
            fantom_release_assert( mIcon.isValid(), "Invalid empty icon." );
        }

        const RTTI::TypeId& objectType() const;

        const Icon& getIcon() const
        {
            return mIcon;
        }

        bool isApplicable( const RTTI& object ) const
        {
            if( !mIsApplicable )
            {
                return true;
            }
            return mIsApplicable( object );
        }

        bool isSpecialization() const
        {
            // we are a specialization if there is an isApplicable function
            return (bool)mIsApplicable;
        }

        const std::string& getName() const
        {
            return mName;
        }

        size_t getPriority() const;
    };

    class RTTIIconRegistrationService : public RegistrationService< IconFactory >
    {
        static const IconFactory::Icon& emptyIcon();

    public:
        static RTTIIconRegistrationService& getInstance();

        RTTIIconRegistrationService();

        /// Creates a InputUI widget for the input type.
        /**
         * \throw fantom::logic_error is thrown when no matching InputUI type is registered
         */
        const IconFactory::Icon& createMostSpecializedFor( const RTTI& object );
    };

    template < class RTTIType >
    class IconRegister
    {
    private:
        std::shared_ptr< IconFactory > ptr;

        using Icon = Graphics2D::Image;
        typedef std::function< bool( const RTTI& ) > ApplicableFunction;

    public:
        /// Register the icon in the system.
        /**
         * If this type is already registered a warning is printed to std::cerr.
         */
        IconRegister( const Icon& icon )
        {
            ptr.reset( new IconFactory( RTTI::typeId< RTTIType >(), icon ) );
            try
            {
                RTTIIconRegistrationService::getInstance().registerFactory( ptr );
            }
            catch( std::exception& e )
            {
                std::cerr << "error: registration of Icon failed." << std::endl;
                std::cerr << e.what() << std::endl;
            }
            catch( ... )
            {
                std::cerr << "error: registration of Icon failed." << std::endl;
            }
        }

        IconRegister( const Icon& icon, ApplicableFunction applicable )
        {
            ptr.reset( new IconFactory( RTTI::typeId< RTTIType >(), icon, applicable ) );
            try
            {
                RTTIIconRegistrationService::getInstance().registerFactory( ptr );
            }
            catch( std::exception& e )
            {
                std::cerr << "error: registration of Icon failed." << std::endl;
                std::cerr << e.what() << std::endl;
            }
            catch( ... )
            {
                std::cerr << "error: registration of Icon failed." << std::endl;
            }
        }

        /// Un-register the icon from the system.
        ~IconRegister()
        {
            RTTIIconRegistrationService::getInstance().unregisterFactory( ptr );
        }
    };

    /**
     * parse the header of the xpm array of strings and return a
     * vector of strings, which is FAnToM's internal representation
     * for icons.
     *
     * \param xpm the xpm image variable as stored in an xpm file
     * \returns the same image stored in a vector of strings
     */
    IconFactory::Icon parseXPM( XPM xpm );
}
