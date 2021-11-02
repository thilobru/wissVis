#pragma once

#include "common.hpp"

#include <memory>
#include <vector>


namespace fantom
{
    namespace graphics
    {
        class ShaderProgram;
        class Uniform;
        class RenderState;
        class Texture;
        class RenderTarget;
    } // namespace graphics
} // namespace fantom


namespace fantom
{
    namespace graphics
    {
        class RenderStateModifier
        {
        public:
            class Modification;

            RenderStateModifier( const RenderStateModifier& copy ) = delete;

            RenderStateModifier( RenderStateModifier&& copy );

            ~RenderStateModifier();

            RenderStateModifier& target( std::shared_ptr< const RenderTarget > target );

            RenderStateModifier& shaderProgram( std::shared_ptr< const ShaderProgram > shaderProgram );

            RenderStateModifier& uniform( std::string name, std::shared_ptr< const Uniform > uniform );

            RenderStateModifier& texture( std::string name, std::shared_ptr< const Texture > texture );

            RenderStateModifier& option( RenderOption option, bool value );

            RenderStateModifier& blending( Blend sourceFactor, Blend targetFactor );

        private:
            RenderStateModifier( RenderState& state );

            RenderState* mState;
            std::vector< std::unique_ptr< Modification > > mModifications;

            friend class RenderState;
        };
    } // namespace graphics
} // namespace fantom
