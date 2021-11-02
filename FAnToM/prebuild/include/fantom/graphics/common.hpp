#pragma once

#include "../math.hpp"

#include <cstdint>
#include <cstdlib>

namespace fantom
{
    namespace graphics
    {
        enum class BufferType
        {
            FLOAT,
            INT,
            UNSIGNED_INT,
            SHORT,
            UNSIGNED_SHORT,
            BYTE,
            UNSIGNED_BYTE
        };

        enum class ColorChannel : uint8_t
        {
            // [ 4-bit-size | 4-bit-type ]
            R = 0x10,
            RG = 0x21,
            RGB = 0x32,
            BGR = 0x33,
            RGBA = 0x44,
            BGRA = 0x45,
            Depth = 0x16,

        };

        inline size_t numComponents( ColorChannel channel )
        {
            return uint8_t( channel ) >> 4;
        }

        enum class VertexColor : size_t
        {
            R = 1,
            RG = 2,
            RGB = 3,
            RGBA = 4
        };

        enum class RenderOption
        {
            Blend,
            CullFace,
            DepthTest,
            LineSmooth,
            PolygonSmooth,
            StencilTest,
        };

        enum class CullFace
        {
            FRONT,
            BACK,
            FRONT_AND_BACK
        };

        enum class WrapMode
        {
            REPEAT,
            MIRRORED_REPEAT,
            CLAMP_TO_EDGE,
            CLAMP_TO_BORDER
        };

        enum class MinFilter
        {
            LINEAR,
            NEAREST,
            NEAREST_MIPMAP_NEAREST,
            LINEAR_MIPMAP_NEAREST,
            NEAREST_MIPMAP_LINEAR,
            LINEAR_MIPMAP_LINEAR
        };

        enum class MagFilter
        {
            LINEAR,
            NEAREST
        };

        enum class Depth
        {
            NEVER,
            LESS,
            EQUAL,
            LEQUAL,
            GREATER,
            NOTEQUAL,
            GEQUAL,
            ALWAYS
        };

        enum class Blend
        {
            ZERO,
            ONE,
            SRC_COLOR,
            ONE_MINUS_SRC_COLOR,
            DST_COLOR,
            ONE_MINUS_DST_COLOR,
            SRC_ALPHA,
            ONE_MINUS_SRC_ALPHA,
            DST_ALPHA,
            ONE_MINUS_DST_ALPHA,
            CONSTANT_COLOR,
            ONE_MINUS_CONSTANT_COLOR,
            CONSTANT_ALPHA,
            ONE_MINUS_CONSTANT_ALPHA
        };


        enum class RenderPrimitives
        {
            POINTS,
            LINE_STRIP,
            LINE_LOOP,
            LINES,
            TRIANGLES,
            TRIANGLE_STRIP,
            TRIANGLE_FAN,
            LINE_STRIP_ADJ,
            LINES_ADJ,
            TRIANGLES_ADJ,
            TRIANGLE_STRIP_ADJ
        };

        enum class RenderBin
        {
            Opaque,      ///< RenderBin for non-transparent geometry
            Transparent, ///< RenderBin for transparent geometry
            Volume       ///< RenderBin for direct volume rendering
        };

        struct enum_hash
        {
            template < typename T >
            std::size_t operator()( T t ) const
            {
                return static_cast< std::size_t >( t );
            }
        };
    } // namespace graphics
} // namespace fantom
