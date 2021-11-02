#pragma once

#include "inputs/Base.hpp"
#include "inputs/DataObject.hpp"
#include "inputs/Events.hpp"
#include "inputs/Primitive.hpp"
#include "inputs/ui.hpp"

#include "datastructures/DataInterface.hpp"
#include "datastructures/DataObject.hpp"

#include "utils.hpp"

namespace fantom
{
    // forward-declarations for InputDataObject
    class DataObjectBundle;

    namespace input_detail
    {
        struct RegularInput_t
        {
        };
        struct DataObjectInput_t
        {
        };
        struct DataInterfaceInput_t
        {
        };
        struct TypeListInput_t
        {
        };

        template < typename T, bool isInput >
        struct ValueTypeExtractor;
        template < typename T >
        struct ValueTypeExtractor< T, true >
        {
            using type = typename T::ValueType;
        };
        template < typename T >
        struct ValueTypeExtractor< T, false >
        {
            using type = T;
        };

        /// struct maps the type of an input value to FAnToM input value types
        /**
         * This struct realizes the following mapping: (prior mentioned matching mapping prioritized)
         * Input<T> -> Input<T>
         * DataObject -> InputDataObject
         * DataInterface -> InputDataObject
         * primitive c++ type T -> InputArithmeticImpl<T>
         * arbitrary class -> Input<T>
         */
        template < class T >
        struct InputTypeTraits
        {
        private:
            using RawType = typename std::decay< T >::type;
            static const bool isInput = std::is_base_of< InputBase, RawType >();
            static const bool isDataObjectInput = std::is_base_of< InputDataObject, RawType >();
            static const bool isTypeList = is_type_list< RawType >();

        public:
            using ContentType = std::conditional_t< isDataObjectInput || isTypeList,
                                                    DataObject,
                                                    typename ValueTypeExtractor< RawType, isInput >::type >;

        public:
            static const bool isArithmetic = std::is_arithmetic< ContentType >();
            static const bool isClass = std::is_class< ContentType >();
            static const bool isDataObject = std::is_base_of< DataObject, ContentType >();
            static const bool isDataInterface = std::is_base_of< DataInterface, ContentType >();

        private:
            static_assert( isArithmetic || isClass,
                           "Invalid input type: only arithmetic c++ datatypes or class types are allowed." );
            using GenerateNonDOInput
                = std::conditional_t< isArithmetic, InputArithmeticImpl< ContentType >, Input< ContentType > >;
            using GenerateInput = std::
                conditional_t< isDataObject || isDataInterface || isTypeList, InputDataObject, GenerateNonDOInput >;

        public:
            using InputType = std::conditional_t< isInput, RawType, GenerateInput >;
            using ValueType = std::conditional_t<
                isDataInterface,
                std::unique_ptr< ContentType >,
                std::conditional_t< isDataObject, std::shared_ptr< const ContentType >, ContentType > >;
            using FilterType = typename InputType::FilterType;

            using tag = std::conditional_t<
                isDataInterface,
                DataInterfaceInput_t,
                std::conditional_t< isTypeList,
                                    TypeListInput_t,
                                    std::conditional_t< isDataObject, DataObjectInput_t, RegularInput_t > > >;

            static const bool isRelatedToDataObject = isDataObject || isDataInterface;
        };
    } // namespace input_detail


    //------------------------------ Options ----------------------------------------

    /// base class of options \ingroup options
    /**
     * Subclasses can add inputs with the various add(...) methods during constructor.
     * The input data can then be accessed with the set- and get-methods.
     *
     * \throw nothing is thrown by the destructor.
     * \throw std::bad_alloc is thrown on insufficient memory (strong).
     * \note All reading members are re-entrant.
     *
     * This class follows the private implementation design pattern.
     * The implementation is kept invisible from the user of the options (i.e., derived classes).
     * The abstract interface to the implementation can be found in #fantom::Options::Control.
     */
    class Options
    {

    public:
        class Visitor;
        class Control;

        enum Required
        {
            REQUIRED = true,
            OPTIONAL = false
        };

        DEFINE_DEFAULT_FANTOM_EXCEPTION( missing_option_error, std::runtime_error )

        /// Constructs options.
        Options( Control& control );

        // Options are non-copyable per definition.
        explicit Options( const Options& ) = delete;

        /// Destroys options.
        virtual ~Options();

        /// \internal Visits options in the order of declaration.
        /**
         * \internal
         * \param visitor the visitor object.
         */
        void traverse( const Visitor& visitor ) const;

        /// \internal Lists all valid option names.
        std::vector< std::string > getOptionNames() const;

        /// Get the input object of an option.
        /**
         * \param name input identifier
         */
        template < class InputType >
        const InputType& getInput( const std::string& name ) const;

        /// Get the input object of an option.
        /**
         * \param name input identifier
         */
        template < class InputType >
        InputType& getInput( const std::string& name );

        /// Gives the value of an option.
        /**
         * \tparam T option type
         * \param name input identifier.
         * \throw fantom::logic_error is thrown when no input with identifier
         *        \a name exists or it has type other than \a T ( strong ).
         * \throw fantom::Options::missing_option_error is thrown if the option is mandatory but has not been set
         *
         * Note: T can be the type of the related Input (e.g., InputSeed) or the data type (e.g., double
         * or Field< 3, Vector3 >).
         */
        template < typename T >
        typename input_detail::InputTypeTraits< T >::ValueType get( const std::string& name ) const;

        /// Sets the value of an option.
        /**
         * \tparam T the type of the option
         * \param name input identifier
         * \param value the new value of the option
         * \post \c get( name ) == \c callback_of_name( value ).
         * \throw fantom::logic_error is thrown when no option with identifier
         *        \a name exists or it has type other than \a T ( strong ).
         * \throw anything may be thrown by the callback of \a name ( strong ).
         */
        template < class T >
        void set( const std::string& name, const typename input_detail::InputTypeTraits< T >::ValueType& value );

        /// Gets the state object of an EventInput.
        /**
         * \tparam State the type of the state input.
         * \param name input identifier
         * \throw fantom::logic_error is thrown when no option with identifier
         *        \a name exists or it has type other than \a T ( strong ).
         * \throw anything may be thrown by the callback of \a name ( strong ).
         */
        template < typename State >
        const typename std::decay< State >::type& getState( const std::string& name ) const;

        /// \copydoc getState(const std::string&) const
        template < typename State >
        typename std::decay< State >::type& getState( const std::string& name );

        /// Check whether option has changed in the mean time (e.g., since last algorithm execution)
        bool hasChanged( const std::string& name ) const;

        /// Subclasses can provide this method to react on input changes.
        /**
         * This can be used to enable/disable or hide/show a specific option dynamically.
         * Default implementation is no-op.
         * \param name input identifier of the changed input
         * \throw must now throw anything.
         */
        virtual void optionChanged( const std::string& name );

        /// Determines whether an option is currently enabled
        /**
         * \param name the name of the option
         * \returns \c true if the option is enabled, otherwise \c false
         */
        bool isEnabled( const std::string& name ) const;

        /// Determines whether an option is currently hidden
        /**
         * \param name the name of the option
         * \returns \c true if the option is hidden, otherwise \c false
         */
        FANTOM_DEPRECATED bool isHidden( const std::string& name ) const;

        /// Determines whether an option is currently hidden
        /**
         * \param name the name of the option
         * \returns \c true if the option is visible, otherwise \c false
         */
        bool isVisible( const std::string& name ) const;

        bool isRequired( const std::string& name ) const;

    protected:
        /// Hides / shows the input with identifier \a name
        /**
         * Set whether the option with the name \c name is hidden.
         * Hidden options are not shown in the gui.
         * By default, options are not hidden.
         * @param name the name of the option
         * @param hidden true if the option should be hidden, false to show the option
         */
        void setHidden( const std::string& name, bool hidden = true );

        /// Hides / shows the input with identifier \a name
        /**
         * Set whether the option with the name \c name is visible.
         * Hidden options are not shown in the gui.
         * By default, options are visible.
         * @param name the name of the option
         * @param visible true if the option should be visible, false to hide the option
         */
        void setVisible( const std::string& name, bool visible = true );

        /// Enables / disables the input with identifier \a name
        /**
         * Set whether the option with the name \c name is disabled.
         * Only enabled options can be manipulated in the GUI.
         * By default, options are enabled.
         * @param name the name of the option
         * @param disabled \c false if the option should be enabled, \c true to disable the option
         */
        void setDisabled( const std::string& name, bool disabled = true );

        /// Enables / disables the input with identifier \a name
        /**
         * Set whether the option with the name \c name is enabled.
         * Only enabled options can be manipulated in the GUI.
         * By default, options are enabled.
         * @param name the name of the option
         * @param enabled \c true if the option should be enabled, \c false to disable the option
         */
        void setEnabled( const std::string& name, bool enabled = true );

        /// Declares a new option
        /**
         * \tparam T the type of input to add. Can be a subclass of InputBase.
         *           Otherwise an appropriate such subclass is chosen automatically.
         * \param name option identifier.
         * \param info option description, suitable for tool-tips.
         * \param args additional arguments for the Input type, including the default value
         * \throw fantom::logic_error is thrown when an option with identifier
         *        \a name already exists or name was the empty string ( strong ).
         * \throw anything may be thrown by the callback of \a name ( strong ).
         */
        template < class T, class... Args >
        std::enable_if_t< !input_detail::InputTypeTraits< T >::isRelatedToDataObject >
        add( const std::string& name, const std::string& info, Args&&... args );

        /// Declares a new DataObject option
        /**
         * \tparam DataObjectType the type (or list of types) of a data object or data interface to add
         * \param name option identifier.
         * \param info option description, suitable for tool-tips.
         * \param required whether the option is mandatory or optional for the execution of the algorithm
         * \throw fantom::logic_error is thrown when an option with identifier
         *        \a name already exists or name was the empty string ( strong ).
         * \throw anything may be thrown by the callback of \a name ( strong ).
         *
         * \note This function is an overloaded version of fantom::Options::add(const std::string&, const std::string&,
         * const InputDataObject::FilterType&, Required) with \a filter = InputDataObject::ignore
         */
        template < class DataObjectType >
        std::enable_if_t< input_detail::InputTypeTraits< DataObjectType >::isRelatedToDataObject >
        add( const std::string& name, const std::string& info, Required required );

        /// Declares a new DataObject option
        /**
         * \tparam DataObjectType the type (or list of types) of a data object or data interface to add
         * \param name option identifier.
         * \param info option description, suitable for tool-tips.
         * \param filter additional filter for the data object input
         * \param required whether the option is mandatory or optional for the execution of the algorithm
         * \throw fantom::logic_error is thrown when an option with identifier
         *        \a name already exists or name was the empty string ( strong ).
         * \throw anything may be thrown by the callback of \a name ( strong ).
         *
         * \note the filter function will only be called with DataObjects that are of type DataObjectType according to
         * RTTI. Therefore, it is safe to cast the filter input to the expected type without checking for success (even
         * statically).
         */
        template < class DataObjectType >
        std::enable_if_t< input_detail::InputTypeTraits< DataObjectType >::isRelatedToDataObject >
        add( const std::string& name,
             const std::string& info,
             const InputDataObject::FilterType& filter = InputDataObject::ignore,
             Required required = OPTIONAL );

        /// Declares a new DataObjectBundle option
        /**
         * \tparam DataObjectType the type (or a list of types) of a data object to add. DataInterface is not allowed at
         * the moment.
         * \param name option identifier.
         * \param info option description, suitable for tool-tips.
         * \param required whether the option is mandatory or optional for the execution of the algorithm
         * \throw fantom::logic_error is thrown when an option with identifier
         *        \a name already exists or name was the empty string ( strong ).
         * \throw anything may be thrown by the callback of \a name ( strong ).
         *
         * \note This function is an overloaded version of fantom::Options::addBundle(const std::string&, const
         * std::string&, const fantom::InputDataObjectBundle::FilterType&, Required) with \a filter =
         * InputDataObject::ignore
         */
        template < class DataObjectType >
        void addBundle( const std::string& name, const std::string& info, Required required );

        /// Declares a new DataObjectBundle option
        /**
         * \tparam DataObjectType the type (or a list of types) of a data object to add. DataInterface is not allowed at
         * the moment.
         * \param name option identifier.
         * \param info option description, suitable for tool-tips.
         * \param filter additional filter for the data object input
         * \param required whether the option is mandatory or optional for the execution of the algorithm
         * \throw fantom::logic_error is thrown when an option with identifier
         *        \a name already exists or name was the empty string ( strong ).
         * \throw anything may be thrown by the callback of \a name ( strong ).
         *
         * \note the filter function will only be called with DataObjects that are of DataObjectBundles and contain only
         * DataObjects of type DataObjectType according to RTTI.
         * This also excludes empty bundles. Therefore, it is safe to cast the filter input to the expected
         * type without checking for success (even statically).
         */
        template < class DataObjectType >
        void addBundle( const std::string& name,
                        const std::string& info,
                        const InputDataObjectBundle::FilterType& filter = InputDataObjectBundle::ignore,
                        Required required = OPTIONAL );

        /// Declares a new event input using the specified State type.
        /**
         * \tparam State the type of the state object. For requirements see \c InputEvent.
         * \param name option identifier.
         * \param info option description, suitable for tool-tips.
         */
        template < typename State >
        void addState( const std::string& name, const std::string& info );

        /// Adds a small separator.
        void addSeparator();

        /// Adds a push-button that triggers the optionChanged() method.
        void addButton( const std::string& name );

        /// Adds a new option tab.
        /**
         * \param name the name of the tab.
         * \note Empty tabs may be omitted in the UI.
         */
        void addTab( const std::string& name );

        /// Adds a new group. Subsequent added options are displayed within the group until \a closeGroup() is called.
        /**
         * \param name the caption and name of the group
         * \note Empty groups may be omitted in the UI.
         */
        void addGroup( const std::string& name );

        /// Closes the lastly opened group. Subsequent calls to \a add(...) add options below, but not inside the group.
        void closeGroup();

    private:
        template < typename T >
        T getImpl( input_detail::RegularInput_t,
                   typename input_detail::InputTypeTraits< T >::InputType& input,
                   bool /*required*/ ) const;
        template < typename T >
        std::unique_ptr< T > getImpl( input_detail::DataInterfaceInput_t, InputDataObject& input, bool required ) const;
        template < typename T >
        std::shared_ptr< const T >
        getImpl( input_detail::DataObjectInput_t, InputDataObject& input, bool required ) const;
        template < typename T >
        std::shared_ptr< const DataObject >
        getImpl( input_detail::TypeListInput_t, InputDataObject& input, bool required ) const;
        template < typename T >
        void addImpl( input_detail::DataObjectInput_t,
                      const std::string& name,
                      const std::string& info,
                      const InputDataObject::FilterType& filter,
                      bool required );
        template < typename T >
        void addImpl( input_detail::DataInterfaceInput_t,
                      const std::string& name,
                      const std::string& info,
                      const InputDataObject::FilterType& filter,
                      bool required );
        template < typename T >
        void addImpl( input_detail::TypeListInput_t,
                      const std::string& name,
                      const std::string& info,
                      const InputDataObject::FilterType& filter,
                      bool required );

        Control& mControl;

        template < class OptionsBase >
        friend class OptionsDataObjectTrait;
    };

    //------------------------------ Option::Control ---------------------------------
    /// Private implementation of Options.
    /**
     * The public part allows hiding/enabling of inputs.
     * Default implementation is no-op.
     */
    class Options::Control : protected InputChangeNotifier
    {
    public:
        virtual ~Control() {}

        /**
         * @see Options::setVisible
         */
        virtual void setVisible( const std::string& name, bool visible );

        /**
         * @see Options::setEnabled
         */
        virtual void setEnabled( const std::string& name, bool enabled );

        /**
         * \returns true if the option is visible
         * @see Options::isVisible
         */
        virtual bool isVisible( const std::string& name ) const;

        /**
         * \returns true if the option is enabled
         * @see Options::isEnabled
         */
        virtual bool isEnabled( const std::string& name ) const;

        Control& operator=( const Control& rhs );

        /**
         * Clears the change flag for all options.
         */
        void clearChanges();

        /// Returns whether there is an input with changed flag.
        bool hasChanged() const;

        /// Returns a vector of all DataObjects in the options, so that the
        /// weak_ptrS in the InputDataObjectS are not invalidated while the vector
        /// is kept.
        std::vector< std::shared_ptr< const DataObject > > freezeDataObjects() const;

    protected:
        void setContentChanged( const InputBase& input ) override;

        void setDefinitionChanged( const InputBase& input ) override;

        bool hasContentChanged( const InputBase& input ) const override;

        /// \internal Add a new input
        /**
         * \throw fantom::logic_error is thrown when an option with identifier
         *        \a name already exists or name was the empty string ( strong ).
         * @see Options::addInput
         */
        void addInput( std::shared_ptr< InputBase > input, bool required = false );

        /// \internal Visits options in the order of declaration.
        /**
         * \internal
         * \param visitor the visitor object.
         * @see Options::traverse
         */
        void traverse( const Visitor& visitor ) const;

        /// \internal Lists all valid option names.
        std::vector< std::string > getOptionNames() const;

        /// \internal Returns a reference to the input identified by \a name.
        /**
         * \internal
         * @see Options::addSeparator
         * @see Options::addButton
         * @see Options::addTab
         * @see Options::addGroup
         * @see Options::closeGroup
         */
        InputBase& getInput( const std::string& name ) const;

        bool isRequired( const std::string& name ) const;

        struct InputState
        {
            bool changed;
            bool visible;
            bool enabled;
        };
        std::vector< std::shared_ptr< InputBase > > mInputs;
        std::vector< bool > mRequired;
        std::vector< InputState > mState;
        std::unordered_map< std::string, size_t > mLookup;

        friend class Options;
    };

    //------------------------------ Options::Visitor ------------------------------

    /// \internal visitor for options
    class Options::Visitor
    {

    protected:
        /// Destroys visitor.
        virtual ~Visitor();

    public:
        /// Processes a primitive input option.
        virtual void processInput( const std::string& name, const std::string& info, InputBase& input ) const;

        /// Processes a data object input option.
        virtual void processDataObject( const std::string& name,
                                        const std::string& info,
                                        InputDataObject& input,
                                        bool required ) const;

        /// Processes a separator.
        virtual void processSeparator() const;

        /// Processes an option buttotn.
        virtual void processButton( const std::string& name, const std::function< void() >& trigger ) const;

        /// Processes an option tab.
        virtual void processTab( const std::string& name ) const;

        /// Processes an option group.
        virtual void processGroup( const std::string& name ) const;

        /// Marks the end of an option group.
        virtual void processEndGroup() const;
    };

    //--------------------------- Template Implementations ----------------------------------

    template < class T >
    typename input_detail::InputTypeTraits< T >::ValueType Options::get( const std::string& name ) const
    {
        InputBase& input = mControl.getInput( name );
        bool required = mControl.isRequired( name );

        using InputType = typename input_detail::InputTypeTraits< T >::InputType;
        using ContentType = typename input_detail::InputTypeTraits< T >::ContentType;

        if( auto castedInput = dynamic_cast< InputType* >( &input ) )
        {
            return getImpl< ContentType >( typename input_detail::InputTypeTraits< T >::tag{}, *castedInput, required );
        }
        else
        {
            throw fantom::logic_error( "implementation error: option type mismatch" );
        }
    }

    template < typename T >
    T Options::getImpl( input_detail::RegularInput_t,
                        typename input_detail::InputTypeTraits< T >::InputType& input,
                        bool /*required*/ ) const
    {
        return input.get();
    }

    template < typename T >
    std::unique_ptr< T >
    Options::getImpl( input_detail::DataInterfaceInput_t, InputDataObject& input, bool required ) const
    {
        static_assert( std::is_base_of< DataInterface, T >(), "Type must be derived from DataInterface" );

        auto content = input.get();
        if( content && T::appliesTo( *content ) )
        {
            return T::applyTo( *content );
        }
        else
        {
            if( required )
                throw missing_option_error( "Missing input for mandatory option \"" + input.getName() + "\"" );
            return nullptr;
        }
    }

    template < typename T >
    std::shared_ptr< const T >
    Options::getImpl( input_detail::DataObjectInput_t, InputDataObject& input, bool required ) const
    {
        static_assert( std::is_base_of< DataObject, T >(), "Type must be derived from DataObject" );

        auto content = std::dynamic_pointer_cast< const T >( input.get() );
        if( required && !content )
            throw missing_option_error( "Missing input for mandatory option \"" + input.getName() + "\"" );
        return content;
    }

    template < typename /*T*/ >
    std::shared_ptr< const DataObject >
    Options::getImpl( input_detail::TypeListInput_t, InputDataObject& input, bool required ) const
    {
        auto content = input.get();
        if( required && !content )
            throw missing_option_error( "Missing input for mandatory option \"" + input.getName() + "\"" );
        return content;
    }

    template < class T >
    void Options::set( const std::string& name, const typename input_detail::InputTypeTraits< T >::ValueType& value )
    {
        static_assert( !input_detail::InputTypeTraits< T >::isRelatedToDataObject,
                       "Can only set non-data object inputs directly. Use connect method instead." );

        InputBase& input = mControl.getInput( name );

        if( auto castedInput = dynamic_cast< typename input_detail::InputTypeTraits< T >::InputType* >( &input ) )
        {
            castedInput->set( value );
        }
        else
        {
            throw fantom::logic_error( "implementation error: option type mismatch" );
        }
    }

    template < typename State >
    const typename std::decay< State >::type& Options::getState( const std::string& name ) const
    {
        return this->getInput< InputEvent >( name ).access< State >();
    }

    template < typename State >
    typename std::decay< State >::type& Options::getState( const std::string& name )
    {
        return this->getInput< InputEvent >( name ).access< State >();
    }

    template < class T, class... Args >
    std::enable_if_t< !input_detail::InputTypeTraits< T >::isRelatedToDataObject >
    Options::add( const std::string& name, const std::string& info, Args&&... args )
    {
        InputChangeNotifier& notifier( mControl );

        mControl.addInput( std::make_shared< typename input_detail::InputTypeTraits< T >::InputType >(
            name, info, notifier, std::forward< Args >( args )... ) );
    }

    template < class DataObjectType >
    std::enable_if_t< input_detail::InputTypeTraits< DataObjectType >::isRelatedToDataObject >
    Options::add( const std::string& name, const std::string& info, Required required )
    {
        add< DataObjectType >( name, info, InputDataObject::ignore, required );
    }

    template < class DataObjectType >
    std::enable_if_t< input_detail::InputTypeTraits< DataObjectType >::isRelatedToDataObject > Options::add(
        const std::string& name, const std::string& info, const InputDataObject::FilterType& filter, Required required )
    {
        addImpl< DataObjectType >(
            typename input_detail::InputTypeTraits< DataObjectType >::tag(), name, info, filter, required );
    }

    template < typename T >
    void Options::addImpl( input_detail::DataObjectInput_t,
                           const std::string& name,
                           const std::string& info,
                           const InputDataObject::FilterType& filter,
                           bool required )
    {
        InputChangeNotifier& notifier( mControl );
        mControl.addInput(
            std::make_shared< InputDataObject >( name, info, notifier, RTTICollection::constructFor< T >(), filter ),
            required );
    }

    template < typename T >
    void Options::addImpl( input_detail::DataInterfaceInput_t,
                           const std::string& name,
                           const std::string& info,
                           const InputDataObject::FilterType& filter,
                           bool required )
    {
        InputChangeNotifier& notifier( mControl );
        mControl.addInput( std::make_shared< InputDataObject >(
                               name,
                               info,
                               notifier,
                               RTTICollection::constructFor< T >(),
                               [filter]( const DataObject& obj ) { return T::appliesTo( obj ) && filter( obj ); } ),
                           required );
    }

    template < typename T >
    void Options::addImpl( input_detail::TypeListInput_t,
                           const std::string& name,
                           const std::string& info,
                           const InputDataObject::FilterType& filter,
                           bool required )
    {
        InputChangeNotifier& notifier( mControl );
        mControl.addInput(
            std::make_shared< InputDataObject >( name, info, notifier, RTTICollection::constructFor< T >(), filter ),
            required );

        // Note: the RTTICollection already assures that the data has a supported type.
        // In case of a DataInterface, this is not completely accurate, i.e., some further constraints might still be
        // violated.
        // However, we do not test this at the moment for reasons of simplicity.
        // FIXME: to implement the correct behavior, we would have to create a filter that tests whether the type of the
        // data corresponds to one of the present DataInterfaces and then require the appliesTo method to succeed.
        // But, this test should only be done for active DataInterfaces in the sense that the required type is not
        // otherwise present in the typelist.
    }

    template < class DataObjectType >
    void Options::addBundle( const std::string& name, const std::string& info, Required required )
    {
        addBundle< DataObjectType >( name, info, InputDataObjectBundle::ignore, required );
    }

    template < class DataObjectType >
    void Options::addBundle( const std::string& name,
                             const std::string& info,
                             const InputDataObjectBundle::FilterType& filter,
                             Required required )
    {
        using List = typename TypeList< DataObjectType >::Flat;
        static_assert( typename List::template all_of< is_dataobject >(),
                       "Only DataObject types are supported for bundles at the moment." );

        InputChangeNotifier& notifier( mControl );

        mControl.addInput( std::make_shared< InputDataObjectBundle >(
                               name, info, notifier, RTTICollection::constructFor< List >(), filter ),
                           required );
    }

    template < typename State >
    void Options::addState( const std::string& name, const std::string& info )
    {
        static_assert( detail_inputevent::CheckInputState< typename std::decay< State >::type >::typeIsOk,
                       "State type does not satisfy requirements!" );

        InputChangeNotifier& notifier( mControl );

        mControl.addInput(
            std::make_shared< InputEvent >( name, info, notifier, typename std::decay< State >::type{} ) );
    }

    template < class InputType >
    const InputType& Options::getInput( const std::string& name ) const
    {
        const InputBase& input = mControl.getInput( name );

        if( auto castedInput = dynamic_cast< const InputType* >( &input ) )
        {
            return *castedInput;
        }

        throw fantom::logic_error( "implementation error: option type mismatch" );
    }

    template < class InputType >
    InputType& Options::getInput( const std::string& name )
    {
        InputBase& input = mControl.getInput( name );

        if( auto castedInput = dynamic_cast< InputType* >( &input ) )
        {
            return *castedInput;
        }

        throw fantom::logic_error( "implementation error: option type mismatch" );
    }

} // namespace fantom
