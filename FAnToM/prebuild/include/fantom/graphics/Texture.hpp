#pragma once

#include "common.hpp"

#include <vector>


namespace fantom
{
    namespace graphics
    {
        class ShaderProgram;

        namespace detail
        {
            class TextureModification;
        }
    } // namespace graphics
} // namespace fantom


namespace fantom
{
    namespace graphics
    {
        class Texture
        {
        public:
            Texture( ColorChannel channel );
            virtual ~Texture();

            std::vector< Color > download() const;
            virtual std::vector< float > downloadData() const = 0;

            ColorChannel colorChannel() const;

            static std::vector< Color > dataToColor( std::vector< float > const& data, ColorChannel channel );
            static std::vector< float > colorToData( std::vector< Color > const& colors, ColorChannel channel );

            virtual void minFilterFunction( MinFilter filter ) = 0;
            virtual void magFilterFunction( MagFilter filter ) = 0;

        private:
            virtual void use( uint8_t unit ) const = 0;
            virtual void release() const = 0;

            ColorChannel mChannel;

            friend class detail::TextureModification;
        };


        class Texture1D : public Texture
        {
        public:
            Texture1D( ColorChannel channel, size_t w );

            std::vector< Color > range( size_t xstart, size_t w ) const;
            virtual std::vector< float > rangeData( size_t xstart, size_t w ) const = 0;

            void range( size_t xstart, size_t w, std::vector< Color > const& pixels );
            virtual void rangeData( size_t xstart, size_t w, std::vector< float > const& pixels ) = 0;
            virtual void rangeData( size_t xstart, size_t w, std::vector< uint8_t > const& pixels ) = 0;

            virtual void wrapMode( WrapMode modeX ) = 0;

            size_t width() const;

            size_t indexFromCoords( size_t x ) const;

        private:
            size_t mW;
        };


        class Texture2D : public Texture
        {
        public:
            Texture2D( ColorChannel channel, Size2D size );

            std::vector< Color > range( Pos2D pos, Size2D size ) const;
            virtual std::vector< float > rangeData( Pos2D pos, Size2D size ) const = 0;

            void range( Pos2D pos, Size2D size, std::vector< Color > const& pixels );
            virtual void rangeData( Pos2D pos, Size2D size, std::vector< float > const& pixels ) = 0;
            virtual void rangeData( Pos2D pos, Size2D size, std::vector< uint8_t > const& pixels ) = 0;

            virtual void wrapMode( WrapMode modeX, WrapMode modeY ) = 0;

            size_t width() const;
            size_t height() const;

            Size2D size() const;

            size_t indexFromCoords( Pos2D pos ) const;

        private:
            Size2D mSize;
        };


        class Texture3D : public Texture
        {
        public:
            Texture3D( ColorChannel channel, Size3D size );

            std::vector< Color > range( Pos3D pos, Size3D size ) const;
            virtual std::vector< float > rangeData( Pos3D pos, Size3D size ) const = 0;

            void range( Pos3D pos, Size3D size, std::vector< Color > const& pixels );
            virtual void rangeData( Pos3D pos, Size3D size, std::vector< float > const& pixels ) = 0;

            virtual void wrapMode( WrapMode modeX, WrapMode modeY, WrapMode modeZ ) = 0;

            size_t width() const;
            size_t height() const;
            size_t depth() const;

            Size3D size() const;

            size_t indexFromCoords( Pos3D pos ) const;

        private:
            Size3D mSize;
        };
    } // namespace graphics
} // namespace fantom
