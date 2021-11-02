#pragma once

#include <fantom-plugins/utils/ColorMap/ColorMapFunctions.hpp>
#include <fantom/graphics.hpp>


namespace fantom
{
    std::shared_ptr< fantom::graphics::Texture1D > generateColorMapTexture(
        const fantom::graphics::GraphicsSystem& system, const ColorMapDataObject& colorMap, size_t size );
}
