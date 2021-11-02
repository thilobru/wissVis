#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <fantom/graphics.hpp>
#include <fantom/math.hpp>

namespace fantom
{
    class FontFile
    {
    public:
        struct Char
        {
            fantom::Point2F texLowerLeft;  // in texture coordinates
            fantom::Point2F texUpperRight; // in texture coordinates
            fantom::Point2F posLowerLeft;  // in pt
            fantom::Point2F posUpperRight; // in pt
            double width;                  // in pt
        };

        FontFile( const std::string& fontTexturePath,
                  const std::string& fontFormatPath,
                  const graphics::GraphicsSystem& gs );

        size_t getLineHeight() const; // in pt
        size_t getWidthOfText( const std::string& text ) const;

        Char getCharacterInfo( unsigned char character ) const;

        const std::shared_ptr< graphics::Texture2D >& getTexture() const;

    private:
        bool loadFontFile( const std::string& path );
        bool loadTexture( const std::string& path, const graphics::GraphicsSystem& gs );

        struct CharTextureInfo
        {
            int x, y, width, height;
            int xOffset;
            int yOffset;
            int xAdvance;
        };

        std::unordered_map< size_t, CharTextureInfo > mCharSet;
        std::shared_ptr< graphics::Texture2D > mTexture;

        size_t mLineHeight;
        size_t mHeight;
        size_t mWidth;
    };
}
