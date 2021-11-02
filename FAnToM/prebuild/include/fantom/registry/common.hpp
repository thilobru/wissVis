#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../exceptions.hpp"

namespace fantom
{
    //------------------------------ Abstract RegistrationService ------------------------------

    /**
     * \tparam FactoryType the base class of factory objects. It is required to provide a \a getName() method returning
     * a std::string
     */
    template < typename FactoryType >
    class RegistrationService
    {
    private:
        std::unordered_map< std::string, std::shared_ptr< FactoryType > > mFactories;
        std::string mObjectType;

    public:
        /// Get Instance of the factory.
        /// It is only forward declared here. You MUST provide an implementation in the defining module!
        static RegistrationService< FactoryType >& getInstance();

        /**
         * \param objectType Name of the class of objects that are registered here, e.g., "Algorithm".
         */
        RegistrationService( const std::string& objectType )
            : mObjectType( objectType )
        {
        }

        virtual ~RegistrationService()
        {
        }

        void registerFactory( const std::shared_ptr< FactoryType >& reg )
        {
            auto i = mFactories.find( reg->getName() );
            if( i != mFactories.end() )
            {
                throw fantom::runtime_error( mObjectType + " \"" + std::string( reg->getName() )
                                             + "\" already registered." );
            }

            mFactories.insert( std::make_pair( reg->getName(), reg ) );
        }

        void unregisterFactory( const std::shared_ptr< FactoryType >& reg )
        {
            auto i = mFactories.find( reg->getName() );
            if( i != mFactories.end() )
                mFactories.erase( i );
        }

        /// returns the names of all registered types
        const std::vector< std::string > registered() const
        {
            std::vector< std::string > registered;
            for( auto i = mFactories.begin(); i != mFactories.end(); ++i )
                registered.push_back( i->first );

            return registered;
        }

        /// returns a shared_ptr to the Factory with the specified name
        std::shared_ptr< FactoryType >& registered( const std::string& name )
        {
            auto i = mFactories.find( name );
            if( i == mFactories.end() )
            {
                throw fantom::runtime_error( mObjectType + " \"" + name + "\" not found." );
            }

            return i->second;
        }
    };
}
