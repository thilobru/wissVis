#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../datastructures/DataObject.hpp"
#include "../rtti.hpp"

namespace fantom
{
    /**
     * \brief Data outputs of algorithms.
     *
     * Subclasses can add data outputs with the add- and addBundle methods during constructor.
     *
     * \throw nothing is thrown by the destructor.
     * \throw std::bad_alloc is thrown on insufficient memory (strong).
     *
     * This class follows the private implementation design pattern.
     * The implementation is kept invisible from the user of the outputs (i.e., derived classes).
     * The abstract interface to the implementation can be found in #fantom::DataOutputs::Control.
     */
    class DataOutputs
    {
    public:
        class Control;
        class OutputDefinition;

        DataOutputs( Control& control );
        virtual ~DataOutputs() = default;

        template < class T >
        void add( const std::string& name );

        template < class T >
        void addBundle( const std::string& name );

    protected:
        DataOutputs::Control& mControl;
    };

    class DataOutputs::OutputDefinition
    {
    public:
        OutputDefinition( RTTICollection types, bool holdsBundle );

        const RTTICollection& getTypes() const
        {
            return mTypes;
        }
        bool holdsBundle() const
        {
            return mHoldsBundle;
        }

        bool isCompatible( const std::shared_ptr< const DataObject >& result ) const;

    private:
        const RTTICollection mTypes;
        bool mHoldsBundle;
    };


    /// Abstract interface to the private implementation of Outputs.
    class DataOutputs::Control
    {
    public:
        virtual ~Control() {}

        bool hasResult( const std::string& name ) const;

        std::vector< std::string > getResultNames() const;

        std::shared_ptr< const DataObject > getResult( const std::string& name ) const;

        const OutputDefinition& getResultDefinition( const std::string& name ) const;

    protected:
        /// Create the output port. Throw exception if it already exists.
        void createOutputPort( const std::string& name, RTTICollection types, bool holdsBundle );

        virtual void setResult( const std::string& name, const std::shared_ptr< const DataObject >& result );

        void clearResult( const std::string& name );

        void clearResults();

    private:
        std::unordered_map< std::string, OutputDefinition > mDefinitions;
        std::unordered_map< std::string, std::shared_ptr< const DataObject > > mResults;

        friend class DataOutputs;
        friend class AlgorithmImpl;
        friend class DataAlgorithm;
        friend class InteractionAlgorithm;
    };


    //================================================ Inline definitions =====

    inline DataOutputs::DataOutputs( Control& control )
        : mControl( control )
    {
    }

    //============================================== Template definitions =====

    template < class T >
    void DataOutputs::add( const std::string& name )
    {
        using List = typename TypeList< T >::Flat;
        static_assert( typename List::template all_of< is_dataobject >(),
                       "Only DataObjects are supported for outputs" );
        mControl.createOutputPort( name, RTTICollection::constructFor< List >(), false );
    }

    template < class T >
    void DataOutputs::addBundle( const std::string& name )
    {
        using List = typename TypeList< T >::Flat;
        static_assert( typename List::template all_of< is_dataobject >(),
                       "Only DataObjects are supported for outputs" );
        mControl.createOutputPort( name, RTTICollection::constructFor< List >(), true );
    }
} // namespace fantom
