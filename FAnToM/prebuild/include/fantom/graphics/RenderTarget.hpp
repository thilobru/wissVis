#pragma once

#include "../math.hpp"
#include "RenderState.hpp"

#include <cstdlib>


namespace fantom
{
    namespace graphics
    {
        class RenderTarget
        {
        public:
            RenderTarget( Size2D size );

            virtual ~RenderTarget();

            size_t width() const;

            size_t height() const;

            Size2D size() const;

        private:
            Size2D mSize;

            virtual void use( RenderState const& state ) const = 0;
            virtual void reuse( RenderState const& state ) const = 0;
            virtual void unuse( RenderState const& state ) const = 0;

            friend class detail::TargetModification;
        };
    } // namespace graphics
} // namespace fantom


namespace fantom
{
    namespace graphics
    {
        inline size_t RenderTarget::width() const
        {
            return mSize[0];
        }

        inline size_t RenderTarget::height() const
        {
            return mSize[1];
        }

        inline Size2D RenderTarget::size() const
        {
            return mSize;
        }
    } // namespace graphics
} // namespace fantom
