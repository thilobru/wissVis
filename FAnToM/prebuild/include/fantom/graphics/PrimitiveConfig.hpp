#pragma once

#include "../math.hpp"
#include "BoundingSphere.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "Uniform.hpp"
#include "common.hpp"

#include <array>
#include <map>
#include <memory>
#include <string>

namespace fantom
{
    namespace graphics
    {
        struct PrimitiveConfigData
        {
            RenderPrimitives mType;
            std::map< RenderOption, bool > mOptions;
            CullFace mCullFace;
            std::pair< Blend, Blend > mBlendFunction;
            Depth mDepthFunction;
            RenderBin mRenderBin;
            BoundingSphere mBoundingSphere;
            std::map< std::string, std::shared_ptr< VertexBuffer > > mAttributes;
            std::shared_ptr< IndexBuffer > mIndexBuffer;
            std::map< std::string, std::shared_ptr< Texture > > mTextures;
            std::map< std::string, std::shared_ptr< Uniform > > mUniforms;
        };

        /**
         * Configuration of how a primitive will be drawn by a ShaderProgram.
         *
         * Note: this class is not designed to be derived from. We use this for wrapping and copying render state.
         */
        class PrimitiveConfig final : private PrimitiveConfigData
        {
        public:
            PrimitiveConfig( RenderPrimitives type );

            PrimitiveConfig& renderOption( RenderOption option, bool value );
            PrimitiveConfig& cullFace( CullFace face );
            PrimitiveConfig& depthFunction( Depth depth );
            PrimitiveConfig& blendFunction( Blend sourceBlend, Blend destinationBlend );
            PrimitiveConfig& renderBin( RenderBin bin );
            PrimitiveConfig& modelMatrix( MatrixF< 4 > const& matrix );

            PrimitiveConfig& boundingSphere( BoundingSphere boundingSphere );
            PrimitiveConfig& vertexBuffer( std::string const& attributeName,
                                           std::shared_ptr< VertexBuffer > const& vertexBuffer );

            PrimitiveConfig& indexBuffer( std::shared_ptr< IndexBuffer > const& indexBuffer );

            PrimitiveConfig& texture( std::string const& name, std::shared_ptr< Texture > const& texture );

            PrimitiveConfig& uniform( std::string const& name, int a );
            PrimitiveConfig& uniform( std::string const& name, int a, int b );
            PrimitiveConfig& uniform( std::string const& name, int a, int b, int c );
            PrimitiveConfig& uniform( std::string const& name, int a, int b, int c, int d );

            PrimitiveConfig& uniform( std::string const& name, float a );
            PrimitiveConfig& uniform( std::string const& name, float a, float b );
            PrimitiveConfig& uniform( std::string const& name, float a, float b, float c );
            PrimitiveConfig& uniform( std::string const& name, float a, float b, float c, float d );

            PrimitiveConfig& uniform( std::string const& name, bool a );
            PrimitiveConfig& uniform( std::string const& name, bool a, bool b );
            PrimitiveConfig& uniform( std::string const& name, bool a, bool b, bool c );
            PrimitiveConfig& uniform( std::string const& name, bool a, bool b, bool c, bool d );

            PrimitiveConfig& uniform( std::string const& name, MatrixF< 2 > const& matrix );
            PrimitiveConfig& uniform( std::string const& name, MatrixF< 3 > const& matrix );
            PrimitiveConfig& uniform( std::string const& name, MatrixF< 4 > const& matrix );

            PrimitiveConfig& uniform( std::string const& name, VectorF< 2 > const& vector );
            PrimitiveConfig& uniform( std::string const& name, VectorF< 3 > const& vector );
            PrimitiveConfig& uniform( std::string const& name, VectorF< 4 > const& vector );
            PrimitiveConfig& uniform( std::string const& name, Color const& color );

            const PrimitiveConfigData& data() const
            {
                return *this;
            }
        };
    } // namespace graphics
} // namespace fantom
