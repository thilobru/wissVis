#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../algorithm.hpp"
#include "../options.hpp"
#include "../outputs.hpp"
#include "../register.hpp"
#include "../utils.hpp"
#include "LogImpl.hpp"

namespace fantom
{

    class DataObject;
    class Graphics;
    class Texture;

    /**
     * The interface from the algorithms to the core of FAnToM.
     * It is the base class for the kernel's algorithm's as seen from
     * the algorithm side.
     *
     * This interface is implemented in the core/Kernel of FAnToM
     * to provide access to the used GUI and other visualization
     * facilities as well as the basic software infrastructure such
     * as debug, information and warning output messages and the
     * data structures.
     *
     * This class follows the private data pattern/private
     * implementation ( PIMPL ) pattern to separate the internal
     * data structure from what the algorithm writer sees.
     * Whereas class #fantom::Algorithm is used as an opaque pointer to this
     * class, this class is used directly in the core of FAnToM.
     */
    class AlgorithmImpl
    {
    public:
        AlgorithmImpl( const std::string& typeName,
                       Options::Control& options,
                       DataOutputs::Control& dataOutputs,
                       VisOutputs::Control& visOutputs,
                       AlgorithmLogger& logger,
                       const std::string& resourcePath );

        AlgorithmImpl( const AlgorithmImpl& ) = delete;
        AlgorithmImpl& operator=( const AlgorithmImpl& ) = delete;

        AlgorithmFactory::AlgorithmType getType() const
        {
            return AlgorithmRegistrationService::getInstance().registered( mTypeName )->getType();
        }

        const std::string& typeName() const;
        Algorithm::Options& options() const;
        InteractionAlgorithm::Interactions& getInteractions() const;

        void instantiateInteractions( Algorithm::Options& options );

        /// Initiates algorithm.
        void instantiateAlgorithm();

        /// Makes the algorithm active and runs it. Returns true if everything went smooth and false if the algorithm
        /// crashed.
        bool run();

        /// Abort the current execution of the algorithm and free all resources.
        void unrun();

        /// Abort the current execution of the algorithm.
        void abort();

        /// Returns whether the algorithm is currently active, i.e., is instantiated.
        bool isActive() const;

        /// Returns whether the algorithm is currently running, but aborted.
        bool isAborted() const;

        const std::string& resourcePath() const
        {
            return mResourcePath;
        }

        Options::Control& getOptionsControl()
        {
            return mOptionsControl;
        }
        DataOutputs::Control& getDataOutputsControl()
        {
            return mDataOutputsControl;
        }
        VisOutputs::Control& getVisOutputsControl()
        {
            return mVisOutputsControl;
        }
        AlgorithmLogger& getLogger()
        {
            return mLogger;
        }

    private:
        bool volatile mAbortFlag;

        Options::Control& mOptionsControl;
        DataOutputs::Control& mDataOutputsControl;
        VisOutputs::Control& mVisOutputsControl;
        AlgorithmLogger& mLogger;

        std::string mResourcePath;
        std::string mTypeName;

        std::unique_ptr< Algorithm::Options > mOptions;
        std::unique_ptr< InteractionAlgorithm::Interactions > mInteractions;
        std::unique_ptr< Algorithm > mAlgorithm;
    };
}
