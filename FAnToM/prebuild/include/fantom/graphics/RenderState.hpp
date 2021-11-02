#pragma once

#include "BoundingSphere.hpp"
#include "Camera.hpp"
#include "RenderStateModifier.hpp"
#include "common.hpp"

#include <array>
#include <cassert>
#include <functional>
#include <map>
#include <stack>
#include <vector>


//=========================================================================================================================
// forward declarations
//=========================================================================================================================

namespace fantom
{
    namespace graphics
    {
        class RenderState;
        class RenderTarget;
        class ShaderProgram;
        class Texture;
        class Uniform;

        namespace detail
        {
            class TargetModification;
            class TextureModification;
            class ShaderProgramModification;
            class UniformModification;
            class OptionModification;
            class BlendingModification;
        } // namespace detail

    } // namespace graphics
} // namespace fantom


//=========================================================================================================================
// declarations
//=========================================================================================================================

namespace fantom
{
    namespace graphics
    {
        /**
         * Class for handling all information about the current render state.
         */
        class RenderState
        {
        public:
            RenderState( RenderBin bin,
                         const Camera& camera,
                         std::array< Vector4, 6 > clippingPlanes,
                         const BoundingSphere& boundingSphere );

            virtual ~RenderState();

            // Member access
            RenderBin bin() const;

            const Camera& camera() const;

            const Vector4& clippingPlane( size_t nr ) const;

            const BoundingSphere& boundingSphere() const;

            std::shared_ptr< const RenderTarget > renderTarget() const;

            std::shared_ptr< const ShaderProgram > shaderProgram() const;

            using UniformCallback
                = std::function< void( const std::string&, const std::shared_ptr< const Uniform >& ) >;

            void iterateUniforms( const UniformCallback& callback ) const;

            std::shared_ptr< const Uniform > uniform( const std::string& name ) const;

            using TextureCallback
                = std::function< void( const std::string&, uint8_t, const std::shared_ptr< const Texture >& ) >;

            void iterateTextures( const TextureCallback& callback ) const;

            // State manipulation
            void bin( RenderBin bin );

            void boundingSphere( BoundingSphere const& boundingSphere );

            RenderStateModifier modify();

            /**
             * Resets the currently bound render target to the given color and depth
             *
             * \note
             * The RenderState is the most logical choice to do the clearing, as the RenderTarget hat to be bound to
             * perform a Clear. Currently, only one background color is supported. It could be handy to support multiple
             * background colors for multi-target FrameBuffers. It would make sense to then move the color definition to
             * the RenderTarget instead.
             *
             * \param color Background color
             * \param depth Background depth
             */
            virtual void clear( const Color& color, float depth = 1.0f ) const = 0;

            /**
             * Resets the depth of currently bound render target to the given value
             *
             * \param depth Background depth
             */
            virtual void clearDepth( float depth ) const = 0;

        private:
            std::shared_ptr< const RenderTarget > mTarget;
            std::shared_ptr< const ShaderProgram > mShaderProgram;
            std::map< std::string, std::shared_ptr< const Uniform > > mUniforms;
            std::map< RenderOption, bool > mOptions;
            Blend mBlendSourceFactor;
            Blend mBlendTargetFactor;

            struct TextureInfo
            {
                std::shared_ptr< const Texture > texture;
                uint8_t unit;
            };

            std::map< std::string, TextureInfo > mTextures;

            // Manage texture units
            std::vector< uint8_t > mFreeTextureUnits;
            size_t mNextTextureUnit;

            RenderBin mBin;
            const Camera& mCamera;
            std::array< Vector4, 6 > mClippingPlanes;
            BoundingSphere mBoundingSphere;

            friend class detail::TargetModification;
            friend class detail::ShaderProgramModification;
            friend class detail::UniformModification;
            friend class detail::TextureModification;
            friend class detail::OptionModification;
            friend class detail::BlendingModification;
        };
    } // namespace graphics
} // namespace fantom


//=========================================================================================================================
// inline definitions
//=========================================================================================================================

namespace fantom
{
    namespace graphics
    {
        inline RenderBin RenderState::bin() const
        {
            return mBin;
        }

        inline void RenderState::boundingSphere( BoundingSphere const& boundingSphere )
        {
            mBoundingSphere = boundingSphere;
        }

        inline Camera const& RenderState::camera() const
        {
            return mCamera;
        }

        inline Vector4 const& RenderState::clippingPlane( size_t nr ) const
        {
            assert( nr < 6 );
            return mClippingPlanes[nr];
        }

        inline BoundingSphere const& RenderState::boundingSphere() const
        {
            return mBoundingSphere;
        }
    } // namespace graphics
} // namespace fantom
