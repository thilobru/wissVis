#pragma once

#include "Camera.hpp"
#include "RenderTarget.hpp"


namespace fantom
{
    namespace graphics
    {
        /// A datastrucure that gets passed to Drawables during update cycle.
        /// It contains all information about the upcoming rendering process.
        struct RenderInfo
        {
            Camera const& camera;
            uint32_t painterId;
            RenderTarget const& target;
            Size2D fullSize;
            Size2D tileOffset;
        };
    } // namespace graphics
} // namespace fantom
