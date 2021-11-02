#pragma once

#include "Font.hpp"

#include <fantom/exceptions.hpp>
#include <fantom/graphics.hpp>

#include <stdio.h>
#include <string>

/**
 * \file
 * a set of macros useful to add text labels and all required options.
 */

// some helpers for dealing with the font selection
namespace
{
    inline fantom::Font::FontEmphasis convertEmphasis( const std::string& name )
    {
        if( name == "Regular" )
        {
            return fantom::Font::FontEmphasis::Regular;
        }
        if( name == "Bold" )
        {
            return fantom::Font::FontEmphasis::Bold;
        }
        if( name == "Italic" )
        {
            return fantom::Font::FontEmphasis::Italic;
        }

        throw fantom::logic_error( "Invalid string in convertEmphasis(...)" );
    }

    inline fantom::Font::FontFamily convertFamily( const std::string& name )
    {
        if( name == "Sans Serif" )
        {
            return fantom::Font::FontFamily::SansSerif;
        }
        if( name == "Serif" )
        {
            return fantom::Font::FontFamily::Serif;
        }
        if( name == "Typewriter" )
        {
            return fantom::Font::FontFamily::Typewriter;
        }

        throw fantom::logic_error( "Invalid string in convertFamily(...)" );
    }
}


/**
 * \def ADD_FONT_OPTIONS
 * macro to add all options to the option object
 * \param name is a prefix for all options.
 */
#define ADD_FONT_OPTIONS( name )                                                                                       \
    add< double >( std::string( name ) + " font size", "", 32.0 );                                                     \
    add< InputChoices >( std::string( name ) + " font family",                                                         \
                         "",                                                                                           \
                         std::vector< std::string >( { "Sans Serif", "Serif", "Typewriter" } ),                        \
                         "Sans Serif" );                                                                               \
    add< InputChoices >( std::string( name ) + " font emphasis",                                                       \
                         "",                                                                                           \
                         std::vector< std::string >( { "Regular", "Bold", "Italic" } ),                                \
                         "Regular" );                                                                                  \
    add< fantom::Color >(                                                                                              \
        std::string( name ) + " font color", "The color of the text labels.", fantom::Color( 0.2, 0.2, 0.5 ) );

// helpers to query the font parameters. The options object/variable must be named "parameters" and defined in the
// context of the call
#define FANTOM_OPTION_FONT_SIZE( name ) ( parameters.get< double >( std::string( name ) + " font size" ) )

#define FANTOM_OPTION_FONT_EMPHASIS( name )                                                                            \
    convertEmphasis( parameters.get< std::string >( std::string( name ) + " font emphasis" ) )

#define FANTOM_OPTION_FONT_FAMILY( name )                                                                              \
    convertFamily( parameters.get< std::string >( std::string( name ) + " font family" ) )

#define FANTOM_OPTION_FONT_COLOR( name ) ( parameters.get< fantom::Color >( std::string( name ) + " font color" ) )

/**
 * Function to add a text label to a 3D primitive using the font parameters defined by ADD_FONT_OPTIONS
 */
inline std::shared_ptr< fantom::graphics::Drawable > ADD_TEXT_LABEL( const fantom::Algorithm::Options& parameters,
                                                                     const std::string& name,
                                                                     fantom::Point3 point,
                                                                     fantom::Font::FontAlignment alignment,
                                                                     const std::string& text,
                                                                     float angle = 0.0 )
{
    return fantom::fontHelper().makeTextLabel( fantom::VectorF< 3 >( point ),
                                               text,
                                               FANTOM_OPTION_FONT_SIZE( name ),
                                               FANTOM_OPTION_FONT_COLOR( name ),
                                               alignment,
                                               FANTOM_OPTION_FONT_FAMILY( name ),
                                               FANTOM_OPTION_FONT_EMPHASIS( name ),
                                               angle );
}

namespace fantom
{
    /**
     * convert a value given the specified format string
     * \param value a value to represent as a string
     * \param format a format string as defined by snprintf on your platform
     *
     * this function is basically a wrapper around the C functionality with
     * additional error checking
     */
    inline std::string convertFormat( double value, std::string format )
    {
        constexpr size_t size = 100;
        char mystring[size];
        size_t rsize = snprintf( mystring, size, format.c_str(), value );
        if( rsize >= size )
        {
            throw fantom::runtime_error( "Format string results in too big string." );
        }

        return std::string( mystring );
    }
} // namespace fantom
