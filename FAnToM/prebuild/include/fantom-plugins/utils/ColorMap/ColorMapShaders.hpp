#pragma once

#include "ColorMapFunctions.hpp"

#include <fantom/math.hpp>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <vector>

namespace fantom
{
    namespace
    {
        inline std::string toVec4( Color c )
        {
            std::ostringstream oss;
            oss << "vec4( " << c.r() << ", " << c.g() << ", " << c.b() << ", " << c.a() << " )";
            return oss.str();
        }

        inline std::string interp( double v1, double v2, Color c1, Color c2 )
        {
            std::ostringstream oss;
            oss << std::setprecision( 7 )
                << std::fixed; // set higher precision to have tailing zeros and dots to have double values in shader
            oss << "      float t = ( value - " << v1 << " )/" << ( v2 - v1 ) << ";\n"
                                                                                 "      return "
                << toVec4( c1 ) << " * ( 1.-t ) + " << toVec4( c2 ) << " * t;\n";
            return oss.str();
        }
    } // namespace

    inline void addShaderWithoutAlphaToStream( std::ostream& os,
                                               CustomLinearColorMapRGB const* const cm,
                                               const Color& minColor,
                                               const Color& maxColor )
    {
        if( cm->numColors() < 1 )
        {
            os << "    if( value < 0.0 )\n"
                  "    {\n"
                  "         return "
               << toVec4( minColor ) << ";\n"
                                        "    }\n"
                                        "    else\n"
                                        "    {\n"
                                        "        return "
               << toVec4( maxColor ) << ";\n"
                                        "    }\n";
        }
        else
        {
            size_t i = 0;
            os << "    if( value < 0.0 )\n"
                  "    {\n"
                  "        return "
               << toVec4( minColor ) << ";\n"
                                        "    }\n"
                                        "    else if( value <= "
               << cm->getColorPosition( i ) << " )\n"
                                               "    {\n"
                                               "         return "
               << toVec4( cm->getColor( i ) ) << ";\n"
                                                 "    }\n";
            while( i < cm->numColors() - 1 )
            {
                os << "    else if( value < " << cm->getColorPosition( i + 1 ) << " )\n"
                                                                                  "    {\n"
                   << interp( cm->getColorPosition( i ),
                              cm->getColorPosition( i + 1 ),
                              cm->getColor( i ),
                              cm->getColor( i + 1 ) )
                   <<
                    //"         return " << toVec4( cm->getColor( i ) ) << ";\n"
                    "    }\n";
                ++i;
            }
            if( i < cm->numColors() )
            {
                os << "    else if( value <= 1.0 )\n"
                      "    {\n"
                      "         return "
                   << toVec4( cm->getColor( i ) ) << ";\n"
                                                     "    }\n"
                                                     "    else\n"
                                                     "    {\n"
                                                     "        return "
                   << toVec4( maxColor ) << ";\n"
                                            "    }\n";
            }
        }
    }

    inline void addShaderWithAlphaToStream( std::ostream& os,
                                            CustomLinearColorMapRGB const* const cm,
                                            const Color& minColor,
                                            const Color& maxColor )
    {
        std::vector< double > parameters;
        for( size_t i = 0; i < cm->numColors(); ++i )
        {
            parameters.push_back( cm->getColorPosition( i ) );
        }

        for( size_t i = 0; i < cm->numAlphas(); ++i )
        {
            parameters.push_back( cm->getAlphaPosition( i ) );
        }

        std::sort( parameters.begin(), parameters.end() );
        auto end = std::unique( parameters.begin(), parameters.end() );
        parameters.resize( end - parameters.begin() );

        if( parameters.size() < 1 )
        {
            os << "    if( value < 0.0 )\n"
                  "    {\n"
                  "         return "
               << toVec4( minColor ) << ";\n"
                                        "    }\n"
                                        "    else\n"
                                        "    {\n"
                                        "        return "
               << toVec4( maxColor ) << ";\n"
                                        "    }";
        }
        else
        {
            size_t i = 0;
            os << "    if( value < 0.0 )\n"
                  "    {\n"
                  "        return "
               << toVec4( minColor )
               << ";\n"
                  "    }\n"
                  // this is a bit tricky as I want to ensure that 0.0 is mapped to the first value
                  // in the color map even if the semantics otherwise more go for the < sign
                  "    else if( value <= "
               << parameters[i] << " )\n"
                                   "    {\n"
                                   "         return "
               << toVec4( cm->mapValueToColor( parameters[i] ) ) << ";\n"
                                                                    "    }\n";
            while( i < parameters.size() - 1 )
            {
                os << "    else if( value < " << parameters[i + 1] << " )\n"
                                                                      "    {\n"
                   << interp( parameters[i],
                              parameters[i + 1],
                              cm->mapValueToColor( parameters[i] ),
                              cm->mapValueToColor( parameters[i + 1] ) )
                   <<
                    //"         return " << toVec4( cm->getColor( i ) ) << ";\n"
                    "    }\n";
                ++i;
            }
            if( i < parameters.size() )
            {
                // 1.0 should be the value of the highest point, everything above is mapped to maxColor
                // for consistency, we really have to ensure that 1.0 is mapped to the last value in the color map
                os << "    else if( value <= 1.0 )\n"
                      "    {\n"
                      "         return "
                   << toVec4( cm->mapValueToColor( parameters[i] ) ) << ";\n"
                                                                        "    }\n"
                                                                        "    else\n"
                                                                        "    {\n"
                                                                        "        return "
                   << toVec4( maxColor ) << ";\n"
                                            "    }\n";
            }
        }
    }

    inline std::string createShaderCodeFromColorMap( const std::string& shader,
                                                     CustomLinearColorMapRGB const* const cm,
                                                     const Color& minColor,
                                                     const Color& maxColor,
                                                     bool withAlphaChannel = true )
    {
        std::ifstream is( shader );
        if( !is )
        {
            std::cerr << "createShaderCodeFromColorMap: cannot find shader file: " << shader << std::endl;
            return "";
        }
        std::string line;
        std::ostringstream os; // ( "/tmp/Simple-fragment-cm.glsl" );
        os << std::setprecision( 7 )
           << std::fixed; // set higher precision to have tailing zeros and dots to have double values in shader

        while( is )
        {
            std::getline( is, line );
            if( !is )
            {
                break;
            }
            if( !line.empty() && line[0] == '_' )
            {
                if( withAlphaChannel )
                {
                    addShaderWithAlphaToStream( os, cm, minColor, maxColor );
                }
                else
                {
                    addShaderWithoutAlphaToStream( os, cm, minColor, maxColor );
                }
            }
            else
            {
                os << line << '\n';
            }
        }
        os.flush();
        return os.str();
    }
} // namespace fantom
