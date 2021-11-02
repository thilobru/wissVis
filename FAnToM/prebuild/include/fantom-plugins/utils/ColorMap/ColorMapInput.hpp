#pragma once

#include "ColorMapFunctions.hpp"

#include <fantom/options.hpp>

namespace fantom
{
    /// specialized input for representing color maps in the options dialog
    /**
     * This is different from color maps as data objects, which are the preferred
     * way to get a color map input for an algorithm. You should avoid using this
     * option-based input if you do not have a good reason to do otherwise.
     *
     * \ingroup options
     */
    typedef Input< std::shared_ptr< ColorMap > > InputColorMap;
}
