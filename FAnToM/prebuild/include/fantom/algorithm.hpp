#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "impl/runtime.hpp"
#include "options.hpp"
#include "outputs.hpp"

namespace fantom
{
    class AlgorithmImpl;
    class DataObject;

    namespace graphics
    {
        class GraphicsSystem;
        class Drawable;
    } // namespace graphics


    //------------------------------ Abstract Algorithm ------------------------------

    /// abstract base class of algorithms \ingroup status
    /**
     * It represents the persistent part of a computation, i.e., data members can be used
     * to store intermediate results for accelerating computations.
     * This base class is not intended for direct use! There are specialized algorithm
     * types data processors (DataAlgorithm) and visualizations (VisAlgorithm).
     * The execute-function must be strongly exception safe!
     *
     * \throw nothing is thrown by the destructor, debugLog(), infoLog(),
     *        warningLog().
     * \throw std::bad_alloc is thrown on insufficient memory (strong).
     * \note The members debugLog(), infoLog(), warningLog() return different output streams.
     *       The output streams of different algorithms are also different. Content of a
     *       stream is only shown on std::flush and std::endl. The content may be formated
     *       and written to console, file, and/or screen.
     * \note All members and access to inputs as well as outputs is thread-safe.
     *       Usually, the algorithm can presume that is is executed in the only thread
     *       of the system.
     *       In case of visual outputs, however, the returned objects may live within a
     *       different thread. Particularly for 2D scenes and GUI windows, an own
     *       synchronization is mandatory!
     *
     * \sa
     * This class follows the private implementation design pattern.
     * The implementation is kept invisible from the algorithm side.
     * The abstract interface to the implementation can be found in #fantom::AlgorithmImpl,
     * its implementation of the encompassed functionality is found in the Control classes of
     * Options, DataOutputs and VisOutputs, as well as in the fantom::kernel namespace.
     *
     */
    class Algorithm
    {
    public:
        class InitData;
        class Progress;

        /// Base class for algorithm specific options
        /**
         * You can rely on the Algorithm::Options class being instantiated exactly once per
         * algorithm instance (instantiation of AlgorithmImpl). Its life time corresponds to the
         * life time of the algorithm instance. Thus, it is explicitly allowed to define member functions
         * (in the Options object) and use them as option callbacks via std::bind.
         */
        using Options = fantom::Options;

        /// Determines whether an instance of this algorithm should automatically be re-executed when an option value
        /// changes.
        static const bool isAutoRun = false;

        /// Creates new algorithm instance
        /**
         * May be used to initialize private data members
         * and should not perform expensive computations.
         */
        Algorithm( InitData& data );

        /// Destroys algorithm.
        virtual ~Algorithm() {}

        /// Performs the computation of this algorithm.
        /**
         * The implementation has to assure to release all existing outputs, it does not need.
         * During execution, \a abortFlag might become true. In that case, the execution shall
         * be canceled without an exception, i.e., the function should simply return.
         */
        virtual void execute( const Options& parameters, const volatile bool& abortFlag ) = 0;

    protected:
        // this is the interface to the kernel

        /// Asks the user a question.
        /**
         * \param question the question to show the user.
         * \param positiveAnswer the positive answer to the question.
         * \param negativeAnswer the negative answer to the question.
         * \return true, if and only if the user chose positiveAnswer.
         */
        bool ask( const std::string& question,
                  const std::string& positiveAnswer = "Yes",
                  const std::string& negativeAnswer = "No" ) const;

        /// Gives the algorithm's log for debug messages.
        std::ostream& debugLog() const;

        /// Gives the algorithm's log for info messages.
        std::ostream& infoLog() const;

        /// Gives the algorithm's log for warning messages.
        std::ostream& warningLog() const;

        /// Gives the path to FAnToM's resource directory.
        const std::string& resourcePath() const;

    private:
        AlgorithmImpl& mImpl;

        friend class DataAlgorithm;
        friend class VisAlgorithm;
        friend class InteractionAlgorithm;
    };

    //------------------------------ DataAlgorithm -----------------------------------

    /// base class of data algorithms \ingroup plugins
    /**
     * It represents an algorithm that only can output data objects but
     * cannot create any visual output.
     *
     * \note If you want to output graphics: Implement a VisDataAlgorithm.
     * \note For visual debugging of the computation progress, please use
     *       export a DataObject that describes your debugging data and use
     *       default visualization algorithms for analysis of that data.
     *       We have, e.g., LineSet for Point and Line-Data, Grid for meshes
     *       or TensorField for scalar or vector field data.
     */
    class DataAlgorithm : public Algorithm
    {
    public:
        using DataOutputs = fantom::DataOutputs;

        /// \copydoc Algorithm::Algorithm
        DataAlgorithm( InitData& data );

    protected:
        /**
         * @brief Sets the content for the output port identified by \a name.
         * Usually, the algorithm should NOT hold any references to that data afterwards,
         * so that data handling and possibly unloading can be controlled by the longer-living AlgorithmImpl.
         *
         * @throw fantom::logic_error if content has wrong type.
         * @param name name of the result port, i.e., the local unique name of the data object
         * @param dataObject the data object itself
         *
         * @sa clearResult
         */
        void setResult( const std::string& name, std::shared_ptr< const DataObject > dataObject );

        /**
         * @brief Fetches an existing result from the list of returned data objects
         * An empty shared_ptr is returned, if the result does not exist (yet resp. anymore).
         * An exception is thrown, if the output port does not exist.
         * @param name the name of the result port, i.e., the local unique name of the data object
         * @return shared pointer to the data object
         */
        std::shared_ptr< const DataObject > getResult( const std::string& name ) const;

        /**
         * @brief Clears the output port.
         *
         * @param name name of the result port, i.e., the local unique name of the data object
         * @sa setResult
         */
        void clearResult( const std::string& name );

        /**
         * @brief Clears all output ports.
         *
         * @sa setResult, clearResult
         */
        void clearResults();
    };

    //------------------------------ VisAlgorithm ------------------------------------

    /// base class of visualization algorithms \ingroup plugins
    /**
     * It represents an algorithm that cannot have any data output but only
     * created visual output. Private data members can be used for caching
     * to accelerate the interactive visualization.
     *
     * \note By default, VisAlgorithms use interactive options, i.e., they
     *       automatically re-execute when an option changes. To override
     *       this behavior, re-declare the static member isAutoRun with a
     *       false value.
     * \note If you want to output data: Implement a DataAlgorithm.
     */
    class VisAlgorithm : public Algorithm
    {
    public:
        using VisOutputs = fantom::VisOutputs;
        static const bool isAutoRun = true;

        /// \copydoc Algorithm::Algorithm
        VisAlgorithm( InitData& data );

    protected:
        /**
         * @brief Clears the content for the graphics output registered by the name.
         * An exception is thrown if the graphics output does not exist.
         * @param name the name of the graphics output (locally unique)
         */
        void clearGraphics( const std::string& name );

        /**
         * @brief Sets the content for the graphics output registered by the name.
         * An exception is thrown if the graphics output does not exist.
         * @param name the name of the graphics output (locally unique)
         * @param drawable the graphical content that will be rendered for the output
         */
        void setGraphics( const std::string& name, const std::shared_ptr< graphics::Drawable >& drawable );

        /**
         * @brief Returns the 2D scene output registered by the name
         * An exception is thrown if the scene output does not exist.
         * @param name the name of the scene output (locally unique)
         * @return reference to the scene
         *
         * \note The returned scene usually lives in a thread that is
         *       different from the thread the Algorithm is executed in.
         *       Access to the scene needs to be synchronized, either
         *       within the scene itself or by using the run mechanism
         *       in Runtime::instance().runSync(...) or
         *       Runtime::instance().runAsync(...).
         */
        template < typename SceneType >
        SceneType& get2DScene( const std::string& name ) const
        {
            return dynamic_cast< SceneType& >( get2DSceneImpl( name ) );
        }

        /**
         * @brief Returns the window output registered by the name
         * An exception is thrown if the window output does not exist.
         * @param name the name of the window output (locally unique)
         * @return reference to the window
         *
         * \note The returned window usually lives in a thread that is
         *       different from the thread the Algorithm is executed in.
         *       Access to the window needs to be synchronized, either
         *       within the window itself or by using the run mechanism
         *       in Runtime::instance().runSync(...) or
         *       Runtime::instance().runAsync(...).
         */
        template < typename WindowType >
        WindowType& getWindow( const std::string& name ) const
        {
            return dynamic_cast< WindowType& >( getWindowImpl( name ) );
        }

        /// Send an event over the specified output port.
        /**
         * @param name The name of the output port.
         * @param event The event that will be send.
         */
        void sendEvent( const std::string& name, const Event& event ) const;

    private:
        Graphics2D::ScenePainter& get2DSceneImpl( const std::string& name ) const;

        Widget& getWindowImpl( const std::string& name ) const;
    };

    //------------------------------ InteractionAlgorithm ------------------------------------

    /// base class of algorithms for user interaction \ingroup plugins
    /**
     * It represents an algorithm that can have any data output as well as visual outputs.
     * Furthermore, it has to define an Interactions class that implements the user interaction.
     * All communication between different parts of the plugin, e.g., between an algorithm window
     * and the execute() method, is coordinated by this Interactions class.
     *
     * Note: There are two possible constructor signatures for subclasses.
     * These are documented at InteractionAlgorithm::InteractionAlgorithm.
     *
     * PLEASE DO NOT USE THIS TYPE OF ALGORITHM ANYMORE. IT WILL GO AWAY IN THE NEAR FUTURE.
     */
    class InteractionAlgorithm : public Algorithm
    {
    public:
        using DataOutputs = fantom::DataOutputs;
        using VisOutputs = fantom::VisOutputs;
        class Interactions;
        static const bool isAutoRun = true;

        /// Creates new algorithm instance
        /**
         * May be used to initialize private data members
         * and should not perform expensive computations.
         *
         * Subclasses must either use this signature for their own constructor,
         * or the following signature:
         *     InteractionSubClass::InteractionSubClass( InitData& data, Interactions& interactions );
         * Thereby, a reference to InteractionSubClass::Interactions is passed.
         * This reference can be stored to communicate from the execute() method to the Interactions object
         * (caution: synchronization needed!).
         * If the second signature is available, it will be used to construct the algorithm instance.
         */
        InteractionAlgorithm( InitData& data );

        virtual void execute( const Algorithm::Options& parameters, const volatile bool& abortFlag ) override;

    protected:
        /**
         * @brief Clears the content for the graphics output registered by the name.
         * An exception is thrown if the graphics output does not exist.
         * @param name the name of the graphics output (locally unique)
         */
        void clearGraphics( const std::string& name );

        /**
         * @brief Sets the content for the graphics output registered by the name.
         * An exception is thrown if the graphics output does not exist.
         * @param name the name of the graphics output (locally unique)
         * @param drawable the graphical content that will be rendered for the output
         */
        void setGraphics( const std::string& name, const std::shared_ptr< graphics::Drawable >& drawable );

        /**
         * @brief Sets the content for the output port identified by \a name.
         *
         * Use \sa clearResult to clear the output port.
         * Usually, the algorithm should NOT hold any references to that data afterwards,
         * so that data handling and possibly unloading can be controlled by the longer-living AlgorithmImpl.
         *
         * @throw fantom::logic_error if content has wrong type.
         * @param name name of the result port, i.e., the local unique name of the data object
         * @param dataObject the data object itself
         */
        void setResult( const std::string& name, std::shared_ptr< const DataObject > dataObject );

        /**
         * @brief Fetches an existing result from the list of returned data objects
         * An empty shared_ptr is returned, if the result does not exist (yet resp. anymore).
         * An exception is thrown, if the output port does not exist.
         * @param name the name of the result port, i.e., the local unique name of the data object
         * @return shared pointer to the data object
         */
        std::shared_ptr< const DataObject > getResult( const std::string& name ) const;

        /**
         * @brief Returns the 2D scene output registered by the name
         * An exception is thrown if the scene output does not exist.
         * @param name the name of the scene output (locally unique)
         * @return reference to the scene
         */
        template < typename SceneType >
        SceneType& get2DScene( const std::string& name ) const
        {
            return dynamic_cast< SceneType& >( get2DSceneImpl( name ) );
        }

        /**
         * @brief Returns the window output registered by the name
         * An exception is thrown if the window output does not exist.
         * @param name the name of the window output (locally unique)
         * @return reference to the window
         */
        template < typename WindowType >
        WindowType& getWindow( const std::string& name ) const
        {
            return dynamic_cast< WindowType& >( getWindowImpl( name ) );
        }

        /**
         * @brief Clears the output port.
         *
         * @param name name of the result port, i.e., the local unique name of the data object
         *
         * \sa setResult
         */
        void clearResult( const std::string& name );

        /**
         * @brief Clears all output ports.
         *
         * @sa setResult, clearResult
         */
        void clearResults();

    private:
        Graphics2D::ScenePainter& get2DSceneImpl( const std::string& name ) const;

        Widget& getWindowImpl( const std::string& name ) const;
    };

    //------------------------------ Algorithm InitData ------------------------------

    namespace detail
    {
        template < typename AlgoType, bool >
        struct ConstructorChooser;
    }

    /// simple class for silently passing the private implementation...
    class Algorithm::InitData
    {
    public:
        InitData( AlgorithmImpl& impl )
            : mImpl( impl )
        {
        }

        void setOptions( Algorithm::Options* options )
        {
            mOptions = options;
        }

    private:
        InteractionAlgorithm::Interactions& getInteractions();
        Algorithm::Options& getOptions()
        {
            return *mOptions;
        }

        AlgorithmImpl& mImpl;
        Algorithm::Options* mOptions;

        friend class Algorithm;
        friend class InteractionAlgorithm::Interactions;
        template < typename AlgoType, bool >
        friend struct detail::ConstructorChooser;
    };

    //------------------------------ Algorithm Interactions ------------------------------

    /**
     * Base class for algorithm interactions.
     * This class represents the "event loop" or the central control unit of the algorithm.
     * It lives in the GUI thread, so no expensive computations should happen here.
     * To coordinate different parts of the algorithm, e.g., the execute() method and (multiple) visual outputs,
     * the Interactions class has direct access to all parts of the algorithm: options, visual outputs, data outputs,
     * and it can request the execute() method to be run.
     * Passing data from the Interactions to a visual output can be done directly and unsynchronized.
     * Passing data to the execute() method can be done via (possibly hidden) options.
     * The execute() method can communicate back if the algorithm stores a reference to the Interactions object
     * that it obtained in the constructor (see InteractionAlgorithm::InteractionAlgorithm).
     * Best practice for visual outputs is to communicate back via callback functions that are set during
     * construction of the Interactions object.
     *
     * Initialization order is as follows:
     * 1. options and outputs (data and visual)
     * 2. Interactions object
     * 3. (on demand) Algorithm object
     */
    class InteractionAlgorithm::Interactions
    {
    public:
        Interactions( Algorithm::InitData& initData );

        virtual ~Interactions() = default;

        /// \copydoc fantom::Options::optionChanged
        virtual void optionChanged( const std::string& name );

    protected:
        // TODO: send and receive events for inter-algorithm communication

        Algorithm::Options& getOptions() const;

        /// \copydoc InteractionAlgorithm::setResult
        void setResult( const std::string& name, std::shared_ptr< const DataObject > dataObject );

        /// \copydoc InteractionAlgorithm::getResult
        std::shared_ptr< const DataObject > getResult( const std::string& name ) const;

        /// \copydoc InteractionAlgorithm::clearResult
        void clearResult( const std::string& name );

        /// \copydoc InteractionAlgorithm::clearResults
        void clearResults();

        /// \copydoc InteractionAlgorithm::clearGraphics
        void clearGraphics( const std::string& name );

        /// \copydoc InteractionAlgorithm::setGraphics
        void setGraphics( const std::string& name, const std::shared_ptr< graphics::Drawable >& drawable );

        /// \copydoc InteractionAlgorithm::get2DScene
        template < typename SceneType >
        SceneType& get2DScene( const std::string& name ) const
        {
            return dynamic_cast< SceneType& >( get2DSceneImpl( name ) );
        }

        /// \copydoc InteractionAlgorithm::getWindow
        template < typename WindowType >
        WindowType& getWindow( const std::string& name ) const
        {
            return dynamic_cast< WindowType& >( getWindowImpl( name ) );
        }

        /// \copydoc Algorithm::ask
        bool ask( const std::string& question,
                  const std::string& positiveAnswer = "Yes",
                  const std::string& negativeAnswer = "No" ) const;

        /// \copydoc Algorithm::debugLog
        std::ostream& debugLog() const;

        /// \copydoc Algorithm::infoLog
        std::ostream& infoLog() const;

        /// \copydoc Algorithm::warningLog
        std::ostream& warningLog() const;

        /// \copydoc Algorithm::resourcePath
        const std::string& resourcePath() const;

        /// Requests the kernel to (re)-run the algorithm
        void requestAlgoRun();

        /// Requests the kernel to abort the algorithm if it is currently running
        void requestAlgoAbort();

    private:
        Graphics2D::ScenePainter& get2DSceneImpl( const std::string& name ) const;
        Widget& getWindowImpl( const std::string& name ) const;

        AlgorithmImpl& mImpl;
        Algorithm::Options& mOptions;
    };

    //------------------------------ Algorithm Progress ------------------------------

    /// algorithm progress \ingroup status
    /**
     * Implemented as a simple unsigned integral that registeres itself at the GUI.
     * \throw std::bad_alloc is thrown on insufficient memory (strong) by
     *        the constructor and reset().
     * \note reset() is re-entrant. Note that only the constructor, destructor,
     *       and reset() block the current thread. Changing the progress has no
     *       speed-penalty and is re-entrant.
     */
    class Algorithm::Progress
    {

    public:
        /// \internal implementation
        struct Impl
        {
        public:
            virtual ~Impl() {}
            virtual void reset() {}
        };

        /// Creates new progress.
        /**
         * \param algo the algorithm currently executing.
         * \param timeReporting determines whether the elapsed time should be logged to Algorithm::debugLog() on
         *        destruction.
         * \post *this == 0. (i.e., the progress is reset to zero)
         */
        Progress( Algorithm& algo, bool timeReporting = true );


        /// Creates new progress and immidiately starts time measuring.
        /**
         * \param algo the algorithm currently executing.
         * \param name the name of the current phase to be shown in UI.
         * \param steps the number of steps in the current phase.
         * \param timeReporting determines whether the elapsed time should be logged to Algorithm::debugLog() on
         *        destruction.
         * if steps is zero, an indefinite progress is created
         * \post *this == 0. (i.e., the progress is reset to zero)
         */
        Progress( Algorithm& algo, const std::string& name, std::size_t steps = 0, bool timeReporting = true );

        /// \copydoc Progress::Progress(Algorithm&, const std::string&, std::size_t, bool)
        Progress( Algorithm& algo, const char* name, std::size_t steps = 0, bool timeReporting = true );

        ~Progress();

        /// Reset progress.
        /**
         * \param name the name of the current phase to be shown in UI.
         * \param steps the number of steps in the current phase.
         * if steps is zero, an indefinite progress is created
         * \post *this == 0. (i.e., the progress is reset to zero)
         */
        void reset( const std::string& name, std::size_t steps = 0 );

        /// Finalizes the progress and writes measured time into the info log.
        void finalize();

        /// Assigns a given value to the progress.
        Progress& operator=( size_t progress );

        /// Casts to size_t.
        operator volatile size_t&();

        /// Casts to size_t.
        operator size_t() const;

        /// Returns the total number of steps.
        size_t steps() const;

        /// Returns the name of this process.
        const std::string& getName() const;

        /// Get the time elapsed since the instantiation of this process.
        std::chrono::nanoseconds getElapsed() const;

        /// Get the estimated time until the process is finished. Returns an empty duration if no estimate is possible.
        std::chrono::nanoseconds getEstimated() const;

    private:
        volatile std::size_t mProgress;

        Algorithm& mAlgo;
        bool mReportTime;

        std::size_t mSteps;
        std::string mName;

        const std::unique_ptr< Impl > mImpl;

        Timing mTiming;
    };
} // namespace fantom
