#pragma once

#include "RenderTarget.hpp"
#include "Texture.hpp"

#include <memory>
#include <string>

namespace fantom
{
    namespace graphics
    {
        class FrameBuffer : public RenderTarget
        {
        public:
            FrameBuffer( Size2D size );

            virtual ~FrameBuffer();

            virtual void colorAttachment( const std::string& name, std::shared_ptr< Texture2D > const& texture ) = 0;

            virtual void depthAttachment( std::shared_ptr< Texture2D > const& texture ) = 0;

            Color const& backgroundColor() const;
            double backgroundDepth() const;

        private:
            Color mBackgroundColor;
            double mDepth;
        };
    } // namespace graphics
} // namespace fantom
