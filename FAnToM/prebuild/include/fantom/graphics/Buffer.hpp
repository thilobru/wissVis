#pragma once

#include "common.hpp"

#include <cstdlib>

namespace fantom
{
    namespace graphics
    {
        class Buffer
        {
        public:
            Buffer( BufferType type, size_t numComponents );
            virtual ~Buffer();

            BufferType type() const;
            size_t numComponents() const;

            virtual size_t numElements() const = 0;

        private:
            BufferType mType;
            size_t mNumComponents;
        };



        class VertexBuffer : public Buffer
        {
        public:
            VertexBuffer( BufferType type, size_t numComponents );

            virtual ~VertexBuffer();
        };



        class IndexBuffer : public Buffer
        {
        public:
            IndexBuffer();

            virtual ~IndexBuffer();
        };
    }
}
