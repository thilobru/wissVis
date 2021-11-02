#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "../algorithm.hpp"
#include "common.hpp"

namespace fantom
{
    //----------------------------- Data Loader Factory -----------------------------------

    class DataLoaderFactory
    {
    private:
        const std::string mName;
        const std::function< bool( std::vector< std::string >& ) > mCanHandle;
        const std::function< bool( Algorithm::Options& options, std::vector< std::string >& ) > mSetLoaderOptions;

    public:
        DataLoaderFactory(
            const std::string& name,
            const std::function< bool( std::vector< std::string >& ) >& canHandle,
            const std::function< bool( Algorithm::Options&, std::vector< std::string >& ) >& setLoaderOptions );

        const std::string& getName() const;
        bool canHandle( std::vector< std::string >& filenames ) const;
        bool setLoaderOptions( Algorithm::Options& options, std::vector< std::string >& filenames ) const;
    };

    class DataLoaderRegistrationService : public RegistrationService< DataLoaderFactory >
    {
    public:
        static DataLoaderRegistrationService& getInstance();

        DataLoaderRegistrationService();

        /**
         * Returns true if the specified loader can handle some of the chosen files.
         *
         * The function is called to find out whether a loader can handle a certain set of files.
         * The loader shall return true if it can handle the files, but it must return false in the
         * case that it cannot handle any of the given files.
         *
         * The function will be called an arbitrary number of times before getLoaders is called with
         * explicitly no guarantee that the function is called at all.
         *
         * \param filenames a list of file names
         * \returns true if the algorithm algo can handle at least some of the files in filenames
         * \post filenames is unchanged
         */
        bool canHandle( const std::string& algo, const std::vector< std::string >& filenames ) const;

        /**
         * Finds the required loaders for the given files.
         *
         * \param[inout] filenames list of filenames
         * \returns a vector of loader names that can be used to load the given files
         * \post filenames now contains all files that can be loaded by the returned reader
         */
        std::vector< std::string > getLoaders( std::vector< std::string >& filenames ) const;

        /**
         * sets the options used to load files from filenames
         *
         * \param loader the algorithm that is used, which must match the options
         * \param options the options object where the options are set by the loader
         * \param filenames input: the files that should be loaded
         *                  output: the files with the files removed that will be loaded
         *                         with these settings
         * \returns true if the algorithm can be executed immediately, false otherwise
         *
         * \post the code will leave the list of filenames is a state, which is most-suitable
         * for calling the code again.
         * For DLR files with a grid and a value set, this means that it removes one value set
         * while keeping the grid in the list of strings. If no suitable value set is found, the
         * grid will be removed as well.
         */
        bool setLoaderOptions( const std::string& loader,
                               Algorithm::Options& options,
                               std::vector< std::string >& filenames ) const;
    };

    //------------------------------ DataLoaderRegister ---------------------------

    /// registers a DataLoader type at the kernel \ingroup plugins
    /**
     * Static variables of this class are used to automatically
     * register data loaders. Example:
     * \code
     * AlgorithmRegister<MyDataLoader> dummy("Load/My Data Loader", "What my algorithm does...");
     * DataLoaderRegister<MyDataLoader> dummy( "Load/My Data Loader" );
     * \endcode
     * After registration, the DataLoader type can be used by the GUI to handle loader algorithms.
     *
     * \tparam DataLoaderType The algorithm type to register. It is required to have the functions
     * loaderCanHandle and loaderSetOptions implemented as static functions.
     * It must be able to set options for algorithms created with the name specified
     * in the constructor
     * \see AlgorithmRegister
     */
    template < class DataLoaderType >
    class DataLoaderRegister
    {
    private:
        std::shared_ptr< DataLoaderFactory > ptr;

        /**
         * @brief check whether the loader can handle some of the given files
         * @param[ in ] filenames the list of files that should be loaded
         * @param[ out ] filenames the list of files that can not be handled by this reader
         * @returns true if the loader can handle at least one of the files specified
         */
        static bool loaderCanHandle( std::vector< std::string >& filenames )
        {
            // redirection to the static function in the algorithm class of type DataLoaderType
            return DataLoaderType::loaderCanHandle( filenames );
        }

        /**
         * @brief setup parameters and remove the files that are handled so far
         *
         * Example implementation: For vtk readers, this will pop one file
         * at a time and set the parameters accordingly
         *
         * More-complex example:
         * For DLR/netcdf files with independent grid/value files, a loader will be
         * initialized. This function will set the parameters to read one of the value
         * files in combination with a grid file and will remove the value file from
         * the filenames. If loaderCanHandle will still return true,
         * it will be called again with the next algorithm ( as canHandle is still true )
         * and remove the next value file...
         * At the time the last value file is removed, the grid will be removed
         * from the list as well.
         *
         * @param parameters The parameters object of the algorithm
         * @param[ in ]  filenames the file names that should be loaded
         * @param[ out ] filenames names of files with the files removed that are set in the options for this run
         */
        static bool loaderSetOptions( Algorithm::Options& parameters, std::vector< std::string >& filenames )
        {
            // redirection to the static function in the algorithm class of type DataLoaderType
            return DataLoaderType::loaderSetOptions( parameters, filenames );
        }

    public:
        /**
         * @brief Register DataLoader in the system.
         *
         * If this DataLoader type is already registered a warning is printed to std::cerr.
         * @param name the name of the algorithm as registered in \class AlgorithmRegister
         */
        DataLoaderRegister( const std::string& name )
        {
            ptr.reset( new DataLoaderFactory( name,
                                              &DataLoaderRegister< DataLoaderType >::loaderCanHandle,
                                              &DataLoaderRegister< DataLoaderType >::loaderSetOptions ) );
            try
            {
                DataLoaderRegistrationService::getInstance().registerFactory( ptr );
            }
            catch( std::exception& e )
            {
                std::cerr << "error: registration of data loader " << ptr->getName() << " failed." << std::endl;
                std::cerr << e.what() << std::endl;
            }
            catch( ... )
            {
                std::cerr << "error: registration of data loader " << ptr->getName() << " failed." << std::endl;
            }
        }

        /// Un-register DataLoader from the system.
        ~DataLoaderRegister()
        {
            DataLoaderRegistrationService::getInstance().unregisterFactory( ptr );
        }

        static bool alwaysTrue( std::string )
        {
            return true;
        }

        // helper functions to be used in the algorithm classes
        static inline bool defaultLoaderCanHandle( std::vector< std::string >& filenames,
                                                   std::string suffix,
                                                   std::function< bool( std::string ) > check = alwaysTrue )
        {
            return defaultLoaderCanHandle( filenames, std::vector< std::string >( { suffix } ), check );
        }

        static inline bool defaultLoaderCanHandle( std::vector< std::string >& filenames,
                                                   std::vector< std::string > suffixes,
                                                   std::function< bool( std::string ) > check = alwaysTrue )
        {
            bool canLoad = false;
            for( auto it = filenames.begin(); it != filenames.end(); )
            {
                bool canHandle = false;
                for( auto suffix : suffixes )
                {
                    if( ( it->size() > 4 && it->substr( it->size() - suffix.length(), suffix.length() ) == suffix )
                        && ( check( *it ) ) )
                    {
                        it = filenames.erase( it );
                        canLoad = true;
                        canHandle = true;
                        break;
                    }
                }
                if( !canHandle )
                {
                    ++it;
                }
            }
            return canLoad;
        }

        static inline bool defaultLoaderSetOptions( Algorithm::Options& options,
                                                    std::vector< std::string >& filenames,
                                                    std::string suffix,
                                                    std::string optionName,
                                                    std::function< bool( std::string ) > check = alwaysTrue )
        {
            return defaultLoaderSetOptions(
                options, filenames, std::vector< std::string >( { suffix } ), optionName, check );
        }

        static inline bool defaultLoaderSetOptions( Algorithm::Options& options,
                                                    std::vector< std::string >& filenames,
                                                    std::vector< std::string > suffixes,
                                                    std::string optionName,
                                                    std::function< bool( std::string ) > check = alwaysTrue )
        {
            // find the name we will handle
            for( auto it = filenames.begin(); it != filenames.end(); ++it )
            {
                for( auto suffix : suffixes )
                {
                    if( ( it->substr( it->size() - suffix.length(), suffix.length() ) == suffix ) && ( check( *it ) ) )
                    {
                        options.set< std::string >( optionName, *it );
                        it = filenames.erase( it );
                        return true;
                    }
                }
            }
            return false;
        }
    };

// we need some basic helpers to clean up the code of the loaders and not to use the same code all over again
#define IMPLEMENT_CAN_HANDLE( fileSuffix )                                                                             \
    static bool loaderCanHandle( std::vector< std::string >& filenames )                                               \
    {                                                                                                                  \
        return DataLoaderRegister< void >::defaultLoaderCanHandle( filenames, fileSuffix );                            \
    }

#define IMPLEMENT_SET_OPTIONS( fileSuffix, optionName )                                                                \
    static bool loaderSetOptions( Algorithm::Options& options, std::vector< std::string >& filenames )                 \
    {                                                                                                                  \
        return DataLoaderRegister< void >::defaultLoaderSetOptions( options, filenames, fileSuffix, optionName );      \
    }

#define IMPLEMENT_CAN_HANDLE_CHECK( fileSuffix, checkFunction )                                                        \
    static bool loaderCanHandle( std::vector< std::string >& filenames )                                               \
    {                                                                                                                  \
        return DataLoaderRegister< void >::defaultLoaderCanHandle( filenames, fileSuffix, checkFunction );             \
    }

#define IMPLEMENT_SET_OPTIONS_CHECK( fileSuffix, optionName, checkFunction )                                           \
    static bool loaderSetOptions( Algorithm::Options& options, std::vector< std::string >& filenames )                 \
    {                                                                                                                  \
        return DataLoaderRegister< void >::defaultLoaderSetOptions(                                                    \
            options, filenames, fileSuffix, optionName, checkFunction );                                               \
    }
}
