#pragma once

#include "FontFile.hpp"

#include <fantom/graphics.hpp>
#include <fantom/math.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace fantom
{
    class Font
    {
    public:
        /// available font emphasis
        enum class FontEmphasis
        {
            Regular,
            Bold,
            Italic
        };

        /// available fonts
        enum class FontFamily
        {
            Serif,
            SansSerif,
            Typewriter,
            Default
        };

        /// available text alignments
        enum class FontAlignment
        {
            Left = 0b0001,
            TopLeft = 0b0101,
            Top = 0b0100,
            TopRight = 0b0110,
            Right = 0b0010,
            BottomRight = 0b1010,
            Bottom = 0b1000,
            BottomLeft = 0b1001,
            Center = 0b0000
        };

        /// Creates a text label as a billboard.
        /**
         * \param center the center of the billboard.
         * \param label the label's content.
         * \param size the label's size.
         * \param color the color in which the text will be rendered
         * \param fontfamily the font family
         * \param emphasis the font's emphasis
         * \param alignment the location of the anchor point of the text
         */
        std::shared_ptr< graphics::Drawable > makeTextLabel( const VectorF< 3 >& center,
                                                             const std::string& label,
                                                             double size,
                                                             const Color& color = Color( 0.2f, 0.2f, 0.4f, 1.0f ),
                                                             FontAlignment alignment = FontAlignment::Center,
                                                             FontFamily fontfamily = FontFamily::SansSerif,
                                                             FontEmphasis emphasis = FontEmphasis::Regular,
                                                             float angle = 0.0 ) const;

    private:
        Font( const std::string& resourcePath, const graphics::GraphicsSystem& gs );

        const FontFile& getFont( FontFamily fontFamily ) const;

        FontFile mSans;
        FontFile mSerif;
        FontFile mTypeWriter;
        std::shared_ptr< graphics::ShaderProgram > mShader;
        const graphics::GraphicsSystem& mGS;

        friend const Font& fontHelper();
    };

    inline constexpr Font::FontAlignment operator|( Font::FontAlignment x, Font::FontAlignment y )
    {
        return static_cast< Font::FontAlignment >( static_cast< int >( x ) | static_cast< int >( y ) );
    }
    inline Font::FontAlignment& operator|=( Font::FontAlignment& x, Font::FontAlignment y )
    {
        x = x | y;
        return x;
    }

    /// Singleton creation function for Font instance
    const Font& fontHelper();
} // namespace fantom
