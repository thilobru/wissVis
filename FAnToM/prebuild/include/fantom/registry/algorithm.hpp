#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include "../algorithm.hpp"
#include "common.hpp"

namespace fantom
{
    //------------------------------ Algorithm Factory -------------------------------------

    class AlgorithmFactory
    {
    public:
        enum class AlgorithmType
        {
            DataAlgorithm,
            VisAlgorithm,
            InteractionAlgorithm
        };

        AlgorithmFactory( const std::string& name,
                          const std::string& description,
                          AlgorithmType type,
                          bool isAutoRun,
                          const std::function< std::unique_ptr< Algorithm::Options >( Options::Control& ) > makeOptions,
                          const std::function< void( DataOutputs::Control& ) > makeOutputs,
                          const std::function< void( VisOutputs::Control& ) > makeGraphicsOutputs,
                          const std::function< std::unique_ptr< Algorithm >( Algorithm::InitData& ) > makeAlgorithm,
                          const std::function< std::unique_ptr< InteractionAlgorithm::Interactions >(
                              Algorithm::InitData& ) > makeInteractions );

        const std::string& getName() const;
        const std::string& getDescription() const;
        bool isAutoRun() const;
        AlgorithmType getType() const;
        std::unique_ptr< Algorithm::Options > makeOptions( Options::Control& control ) const;
        void makeDataOutputs( DataOutputs::Control& control ) const;
        void makeVisOutputs( VisOutputs::Control& control ) const;
        std::unique_ptr< Algorithm > makeAlgorithm( Algorithm::InitData& init ) const;
        std::unique_ptr< InteractionAlgorithm::Interactions > makeInteractions( Algorithm::InitData& ) const;

    private:
        const std::string mName;
        const std::string mDescription;
        const AlgorithmType mType;
        const bool mAutoRun;
        const std::function< std::unique_ptr< Algorithm::Options >( Options::Control& ) > mMakeOptions;
        const std::function< void( DataOutputs::Control& ) > mMakeOutputs;
        const std::function< void( VisOutputs::Control& ) > mMakeVisOutputs;
        const std::function< std::unique_ptr< Algorithm >( Algorithm::InitData& ) > mMakeAlgorithm;
        const std::function< std::unique_ptr< InteractionAlgorithm::Interactions >( Algorithm::InitData& ) >
            mMakeInteractions;
    };

    using AlgorithmRegistrationService = RegistrationService< AlgorithmFactory >;


    // ================== Tag dispatching ===============================================


    namespace detail
    {
        namespace tag
        {
            struct DataAlgorithm
            {
            };
            struct VisAlgorithm
            {
            };
            struct InteractionAlgorithm
            {
            };
        }

        template < typename AlgorithmType >
        struct AlgorithmTypeTag
        {
            static const bool isDataAlgorithm = std::is_base_of< DataAlgorithm, AlgorithmType >::value;
            static const bool isVisAlgorithm = std::is_base_of< VisAlgorithm, AlgorithmType >::value;
            static const bool isInteractionAlgorithm = std::is_base_of< InteractionAlgorithm, AlgorithmType >::value;

            static const size_t typeCount = isDataAlgorithm + isVisAlgorithm + isInteractionAlgorithm;

            static_assert( typeCount == 1,
                           "The algorithm must inherit from exactly one of DataAlgorithm, VisAlgorithm, or "
                           "InteractionAlgorithm!" );

            using type = typename std::
                conditional< isDataAlgorithm,
                             tag::DataAlgorithm,
                             typename std::conditional< isVisAlgorithm,
                                                        tag::VisAlgorithm,
                                                        typename std::conditional< isInteractionAlgorithm,
                                                                                   tag::InteractionAlgorithm,
                                                                                   void >::type >::type >::type;

            static const AlgorithmFactory::AlgorithmType enumType
                = isDataAlgorithm ? AlgorithmFactory::AlgorithmType::DataAlgorithm :
                                    ( isVisAlgorithm ? AlgorithmFactory::AlgorithmType::VisAlgorithm :
                                                       AlgorithmFactory::AlgorithmType::InteractionAlgorithm );
        };
    }

    // ================================ Type checks and implementation =======================

    namespace detail
    {
        template < typename AlgoTag, typename AlgorithmType >
        struct AlgorithmTypeCheck;

        template < typename AlgorithmType, typename AlgoTag >
        std::unique_ptr< Algorithm::Options > makeOptions( AlgoTag, Options::Control& control )
        {
            return std::unique_ptr< Algorithm::Options >( new typename AlgorithmType::Options( control ) );
        }

        template < typename AlgorithmType, typename AlgoTag >
        void makeDataOutputs( AlgoTag, DataOutputs::Control& )
        {
            // no-op
        }

        template < typename AlgorithmType, typename AlgoTag >
        void makeVisOutputs( AlgoTag, VisOutputs::Control& /*control*/ )
        {
            // no-op
        }

        template < typename AlgorithmType, typename AlgoTag >
        std::unique_ptr< InteractionAlgorithm::Interactions > makeInteractions( AlgoTag, Algorithm::InitData& /*init*/ )
        {
            return nullptr; // no-op
        }

        template < typename AlgorithmType, typename AlgoTag >
        std::unique_ptr< Algorithm > makeAlgorithm( AlgoTag, Algorithm::InitData& init )
        {
            return std::unique_ptr< Algorithm >( new AlgorithmType( init ) );
        }

        //------------------------------ DataAlgorithmRegister ------------------------------

        template < typename AlgorithmType >
        struct AlgorithmTypeCheck< tag::DataAlgorithm, AlgorithmType >
        {
            static_assert( std::is_base_of< Algorithm::Options, typename AlgorithmType::Options >::value,
                           "Wrong options type! Please inherit from Algorithm::Options." );
            static_assert( std::is_base_of< fantom::DataOutputs, typename AlgorithmType::DataOutputs >::value,
                           "Wrong outputs type! Please inherit from fantom::DataOutputs." );
        };

        template < typename AlgorithmType >
        void makeDataOutputs( tag::DataAlgorithm, DataOutputs::Control& control )
        {
            typename AlgorithmType::DataOutputs outputs( control );
            // create and immediately unload. Just has the side effect of creating all needed algorithm outputs.
        }

        //------------------------------ VisAlgorithmRegister ------------------------------

        template < typename AlgorithmType >
        struct AlgorithmTypeCheck< tag::VisAlgorithm, AlgorithmType >
        {
            static_assert( std::is_base_of< Algorithm::Options, typename AlgorithmType::Options >::value,
                           "Wrong options type! Please inherit from Algorithm::Options." );
            static_assert( std::is_base_of< fantom::VisOutputs, typename AlgorithmType::VisOutputs >::value,
                           "Wrong outputs type! Please inherit from fantom::VisOutputs." );
        };

        template < typename AlgorithmType >
        void makeVisOutputs( tag::VisAlgorithm, VisOutputs::Control& control )
        {
            typename AlgorithmType::VisOutputs outputs( control );
            // create and immediately unload. Just has the side effect of creating all needed algorithm outputs.
        }

        //------------------------------ InteractionAlgorithmRegister ------------------------------

        template < typename AlgorithmType >
        struct AlgorithmTypeCheck< tag::InteractionAlgorithm, AlgorithmType >
        {
            static_assert( std::is_base_of< Algorithm::Options, typename AlgorithmType::Options >::value,
                           "Wrong options type! Please inherit from Algorithm::Options." );
            static_assert( std::is_base_of< fantom::DataOutputs, typename AlgorithmType::DataOutputs >::value,
                           "Wrong data outputs type! Please inherit from fantom::DataOutputs." );
            static_assert( std::is_base_of< fantom::VisOutputs, typename AlgorithmType::VisOutputs >::value,
                           "Wrong vis outputs type! Please inherit from fantom::VisOutputs." );
            static_assert(
                std::is_base_of< InteractionAlgorithm::Interactions, typename AlgorithmType::Interactions >::value,
                "Wrong interactions type! Please inherit from InteractionAlgorithm::Interactions." );
            static_assert(
                !std::is_same< InteractionAlgorithm::Interactions, typename AlgorithmType::Interactions >::value,
                "No interactions type! Interaction algorithms are required to provide their own interactions object." );
        };

        /// Try to call the contains method on a dummy object. If that succeeds, this function is existent.
        template < typename Algo >
        static auto testConstructor( int )
            -> sfinae_true< decltype( Algo( std::declval< Algorithm::InitData& >(),
                                            std::declval< typename Algo::Interactions& >() ) ) >;

        /// If the call does not succeed, the return type is std::false_type. If the above function is existent,
        /// this function is not selected because of the parameter type.
        template < typename Algo >
        static auto testConstructor( long ) -> std::false_type;

        template < typename AlgoType,
                   bool hasInteractions
                   = std::is_base_of< std::true_type, decltype( testConstructor< AlgoType >( 0 ) ) >::value >
        struct ConstructorChooser;

        template < typename AlgoType >
        struct ConstructorChooser< AlgoType, true >
        {
            static std::unique_ptr< Algorithm > makeAlgorithm( Algorithm::InitData& init )
            {
                return std::unique_ptr< Algorithm >(
                    new AlgoType( init, static_cast< typename AlgoType::Interactions& >( init.getInteractions() ) ) );
            }
        };

        template < typename AlgoType >
        struct ConstructorChooser< AlgoType, false >
        {
            static std::unique_ptr< Algorithm > makeAlgorithm( Algorithm::InitData& init )
            {
                return std::unique_ptr< Algorithm >( new AlgoType( init ) );
            }
        };

        template < typename AlgorithmType >
        void makeDataOutputs( tag::InteractionAlgorithm, DataOutputs::Control& control )
        {
            typename AlgorithmType::DataOutputs outputs( control );
            // create and immediately unload. Just has the side effect of creating all needed algorithm outputs.
        }

        template < typename AlgorithmType >
        void makeVisOutputs( tag::InteractionAlgorithm, VisOutputs::Control& control )
        {
            typename AlgorithmType::VisOutputs outputs( control );
            // create and immediately unload. Just has the side effect of creating all needed algorithm outputs.
        }

        template < typename AlgorithmType >
        std::unique_ptr< InteractionAlgorithm::Interactions > makeInteractions( tag::InteractionAlgorithm,
                                                                                Algorithm::InitData& init )
        {
            return std::unique_ptr< InteractionAlgorithm::Interactions >(
                new typename AlgorithmType::Interactions( init ) );
        }

        template < typename AlgorithmType >
        std::unique_ptr< Algorithm > makeAlgorithm( tag::InteractionAlgorithm, Algorithm::InitData& init )
        {
            return ConstructorChooser< AlgorithmType >::makeAlgorithm( init );
        }
    }


    //------------------------------ AlgorithmRegister ------------------------------


    /// registers an algorithm at the kernel \ingroup plugins
    /**
     * Static variables of this class are used to automatically
     * register algorithms. Example:
     * \code
       AlgorithmRegister<MyAlgorithm> dummy("My Algorithm", "What my algorithm does...");
       \endcode
     * After registration, this class is able to construct an instance of the algorithm.
     *
     * \tparam AlgorithmType the type of algorithm to register. It is required to
     *       have a public constructor taking an Algorithm::InitData object, be
     *       derived from one of {Loader-,Data-,Vis-,Interaction-}Algorithm.
     *       It is allowed to have an inner class AlgorithmType::Object that is derived from Algorithm::Options,
     *       and an inner class AlgorithmType::DataOutputs that is derived from fantom::DataOutputs.
     *       If the algorithm has any graphical output, it should also sport an inner class AlgorithmType::VisOutputs
     *       that is derived from fantom::VisOutputs.
     *       Note, that these inner classes are not appropriate in all cases. You will get a compiler error
     *       when using wrong or inappropriate types.
     *       All constructors and the AlgorithmType::execute-function be strongly exception-safe and all
     *       destructors non-throwing.
     */
    template < class AlgorithmType >
    class AlgorithmRegister
    {
        using tagType = detail::AlgorithmTypeTag< AlgorithmType >;
        using typeChecks = detail::AlgorithmTypeCheck< typename tagType::type, AlgorithmType >;

        std::shared_ptr< AlgorithmFactory > ptr;

    public:
        /// Register algorithm in the system.
        /**
         * If an algorithm with the same name is already registered a warning is printed to std::cerr.
         * \param name the name of the algorithm suitable for menus, "/" can be used as a submenu separator
         * \param description a little description of the algorithm suitable for tool-tips
         */
        AlgorithmRegister( const std::string& name, const std::string& description )
        {
            ptr.reset( new AlgorithmFactory(
                name,
                description,
                tagType::enumType,
                AlgorithmType::isAutoRun,
                []( Options::Control& control ) -> std::unique_ptr< fantom::Options > {
                    return detail::makeOptions< AlgorithmType >( typename tagType::type{}, control );
                },
                []( DataOutputs::Control& control ) {
                    detail::makeDataOutputs< AlgorithmType >( typename tagType::type{}, control );
                },
                []( VisOutputs::Control& control ) {
                    detail::makeVisOutputs< AlgorithmType >( typename tagType::type{}, control );
                },
                []( Algorithm::InitData& init ) -> std::unique_ptr< Algorithm > {
                    return detail::makeAlgorithm< AlgorithmType >( typename tagType::type{}, init );
                },
                []( Algorithm::InitData& init ) -> std::unique_ptr< InteractionAlgorithm::Interactions > {
                    return detail::makeInteractions< AlgorithmType >( typename tagType::type{}, init );
                } ) );
            try
            {
                AlgorithmRegistrationService::getInstance().registerFactory( ptr );
            }
            catch( std::exception& e )
            {
                std::cerr << "error: registration of algorithm " << ptr->getName() << " failed." << std::endl;
                std::cerr << e.what() << std::endl;
            }
            catch( ... )
            {
                std::cerr << "error: registration of algorithm " << ptr->getName() << " failed." << std::endl;
            }
        }

        /// Un-register algorithm from the system.
        ~AlgorithmRegister()
        {
            if( ptr )
                AlgorithmRegistrationService::getInstance().unregisterFactory( ptr );
        }
    };
}
