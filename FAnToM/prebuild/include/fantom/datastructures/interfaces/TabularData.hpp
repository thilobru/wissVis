#pragma once

#include "../DataInterface.hpp"
#include "../Function.hpp"
#include "../domains/Table.hpp"

namespace fantom
{

    template < class T >
    class TabularData : public DataInterface
    {
    public:
        using AppliesTo = TypeList< Function< T > >;

        TabularData( const Function< T >& func )
            : mData( func )
            , mTable( static_cast< const Table& >( *func.domain() ) )
        {
        }

        T operator()( size_t row, size_t col )
        {
            return mData.values()[mTable.getIndex( row, col )];
        }

        std::pair< size_t /*rows*/, size_t /*cols*/ > getSize() const
        {
            return mTable.getSize();
        }

        static bool appliesTo( const DataObject& obj );

        static std::unique_ptr< TabularData< T > > applyTo( const DataObject& obj ) noexcept;

    private:
        const Function< T >& mData;
        const Table& mTable;
    };


    template < class T >
    bool TabularData< T >::appliesTo( const DataObject& obj )
    {
        if( obj.isA< Function< T > >() )
        {
            auto& func = static_cast< const Function< T >& >( obj );
            return func.domain()->isA( Table::classTypeId() );
        }
        else
        {
            return false;
        }
    }


    template < class T >
    std::unique_ptr< TabularData< T > > TabularData< T >::applyTo( const DataObject& obj ) noexcept
    {
        if( obj.isA< Function< T > >() )
        {
            auto& func = static_cast< const Function< T >& >( obj );
            if( !func.domain()->isA( Table::classTypeId() ) )
            {
                return nullptr;
            }
            else
            {
                return std::make_unique< TabularData< T > >( func );
            }
        }
        else
        {
            return nullptr;
        }
    }
}
