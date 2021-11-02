#pragma once

#include <string>

#include "common.hpp"

namespace fantom
{
    class PluginInfo
    {
    private:
        const std::string mName;
        const std::string mResourcePath;

    public:
        PluginInfo( const std::string& pluginname, const std::string& resourcePath );

        const std::string& getName() const;
        const std::string& getResourcePath() const;
    };

    class PluginRegistrationService : public RegistrationService< PluginInfo >
    {
    public:
        static PluginRegistrationService& getInstance();

        PluginRegistrationService();

        /**
         * Returns the resource path for the given plugin.
         * A std::runtime_error is thrown, if the plugin is not known.
         */
        const std::string& getResourcePath( const std::string& pluginname ) const;
    };
}
