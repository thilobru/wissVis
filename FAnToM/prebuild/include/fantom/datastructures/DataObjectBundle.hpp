#pragma once

#include <memory>

#include "../rtti.hpp"
#include "DataObject.hpp"

namespace fantom
{
    /**
     * Vector of arbitrary data objects that can be added as a DataOutput.
     *
     * This class is a \c DataObject itself, but also stores a bundle of \c DataObject s
     * that are of the same kind in most cases, but need not be.
     */
    class DataObjectBundle : public DataObject
    {
        typedef RTTI_Info< DataObjectBundle, DataObject > TypeInfo;
        TYPE_INFORMATION( "Data Object Bundle" )

    public:
        // ------------------ construction ------------------------------------------------------------

        /**
         * Default constructor to create an empty bundle.
         */
        DataObjectBundle() = default;

        /**
         * Constructor to reserve a certain capacity to speed up insertion.
         */
        DataObjectBundle( size_t capacity );

        /**
         * Construct a DataObjectBundle and fill it with the data of the given vector.
         */
        DataObjectBundle( std::vector< std::shared_ptr< const DataObject > > dataObjects );

        /**
         * Construct a DataObjectBundle and fill it with the given data.
         */
        template < class InputIterator >
        DataObjectBundle( InputIterator first, InputIterator last );


        // ------------------ manage content ------------------------------------------------------------

        /// Add another \c DataObject to the bundle.
        /**
         * When creating specialized DataObjectBundles, this method can be overridden
         * and be used for additional typechecks etc.
         */
        virtual void addContent( std::shared_ptr< const DataObject > dataObject, const std::string& name = "" );

        /// Sets the name of the DataObject at index \c position.
        void setName( size_t position, const std::string& name );

        /// Erase all \c DataObjects from the bundle.
        void clearContent();

        /**
         * Reserves a certain amount of capacity to speed up insertion.
         * \param capacity Capacity of new container
         */
        void reserve( size_t capacity );


        // ------------------ content metadata ------------------------------------------------------------

        static const RTTI::TypeId&
        getCommonType( const std::vector< std::shared_ptr< const DataObject > >& dataObjects );

        /// \return the type of most specialized common type of the \c DataObjects in the \c DataObjectBundle
        const RTTI::TypeId& getContentTypeId() const;

        /**
         * @copydoc DataObject::getInfoStrings()
         */
        virtual std::vector< std::pair< std::string, std::string > > getInfoStrings() const override;


        // ------------------ access content ------------------------------------------------------------

        /// \return the number of \c DataObjects in the bundle.
        size_t getSize() const;

        /// \return whether the bundle is empty
        bool isEmpty() const;

        /// \return the \c DataObject at index \c position in the bundle.
        const std::shared_ptr< const DataObject >& getContent( size_t position ) const;

        /// \return the DataObject with the name \c name or nullptr if no such DataObject exists.
        std::shared_ptr< const DataObject > getContent( const std::string& name ) const;

        /// \return the name of the DataObject at index \c position in the bundle
        const std::string& getName( size_t position ) const;

        /// \return the index of the DataObject with the name \c name. Returns size_t(-1) if no such DataObject exists.
        size_t getIndexOf( const std::string& name ) const;


        // ------------------ iteration ------------------------------------------------------------

        /// iterator to begin of contained data objects
        std::vector< std::shared_ptr< const DataObject > >::const_iterator begin() const;
        /// iterator to end of contained data objects
        std::vector< std::shared_ptr< const DataObject > >::const_iterator end() const;


        //==================================================== Deprecated =====

        /**
         * Creates a DataObjectBundle holding a griben type.
         *
         * \deprecated { Since typechecking has been moved to
         * Output system, this is not needed anymore.
         * Use std::make_shared< DataObjectBundle >() instead }
         */
        template < class T >
        FANTOM_DEPRECATED static std::shared_ptr< DataObjectBundle > create()
        {
            return std::shared_ptr< DataObjectBundle >( new DataObjectBundle() );
        }

        /**
         * Creates a DataObjectBundle holding a griben type.
         *
         * \deprecated { Since typechecking has been moved to
         * Output system, this is not needed anymore.
         * Use std::make_shared< DataObjectBundle >() instead }
         */
        FANTOM_DEPRECATED static std::shared_ptr< DataObjectBundle > create( const RTTI::TypeId& /*type*/ )
        {
            return std::shared_ptr< DataObjectBundle >( new DataObjectBundle() );
        }

    private:
        std::vector< std::shared_ptr< const DataObject > > mDataObjects;
        std::vector< std::string > mNames;
    };


    //================================================ Inline definitions =====

    inline void DataObjectBundle::clearContent()
    {
        mDataObjects.clear();
        mNames.clear();
    }

    inline void DataObjectBundle::reserve( size_t capacity )
    {
        mDataObjects.reserve( capacity );
        mNames.reserve( capacity );
    }

    inline size_t DataObjectBundle::getSize() const
    {
        return mDataObjects.size();
    }

    inline bool DataObjectBundle::isEmpty() const
    {
        return mDataObjects.empty();
    }

    inline std::vector< std::shared_ptr< const DataObject > >::const_iterator DataObjectBundle::begin() const
    {
        return mDataObjects.begin();
    }

    inline std::vector< std::shared_ptr< const DataObject > >::const_iterator DataObjectBundle::end() const
    {
        return mDataObjects.end();
    }


    //============================================== Template definitions =====

    template < class InputIterator >
    DataObjectBundle::DataObjectBundle( InputIterator first, InputIterator last )
        : mDataObjects( first, last )
    {
        mNames.resize( mDataObjects.size() );
    }
}
