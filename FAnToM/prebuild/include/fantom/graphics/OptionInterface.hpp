#pragma once

#include "common.hpp"


namespace fantom
{
    namespace graphics
    {
        class OptionInterface
        {
        public:
            virtual ~OptionInterface();

            virtual void set( RenderOption option, bool value ) const = 0;
            virtual void setBlending( Blend sourceFactor, Blend targetFactor ) const = 0;
        };
    } // namespace graphics
} // namespace fantom
