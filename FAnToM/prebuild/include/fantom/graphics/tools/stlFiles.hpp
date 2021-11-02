#pragma once

#include <fantom/math.hpp>

#include <vector>


namespace fantom
{
    namespace graphics
    {
        struct STLData
        {
            std::vector< PointF< 3 > > vertices;
            std::vector< uint32_t > indices;
            std::vector< VectorF< 3 > > triangleNormals;
        };

        STLData loadSTLFile( const std::string& filename );
    } // namespace graphics
} // namespace fantom
