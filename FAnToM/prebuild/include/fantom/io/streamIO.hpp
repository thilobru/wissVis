#pragma once

#include "endianness.hpp"
#include <iostream>


namespace fantom
{
    namespace io
    {
        template < typename TType >
        TType read( std::istream& stream, Endianness endianness );
    }
} // namespace fantom


namespace fantom
{
    namespace io
    {
        template < typename TType >
        TType read( std::istream& stream, Endianness endianness )
        {
            // This line unfortunately assumes the type to be default-constructible, which is a requirement we usually
            // don't want to make. However, it enables return value optimization, which is worth more in this case.
            TType result;

            stream.read( reinterpret_cast< char* >( &result ), sizeof( TType ) );

            if( endianness != systemEndianness() )
            {
                auto ptr = reinterpret_cast< uint8_t* >( &result );
                std::reverse( ptr, ptr + sizeof( TType ) );
            }

            return result;
        }
    } // namespace io
} // namespace fantom
