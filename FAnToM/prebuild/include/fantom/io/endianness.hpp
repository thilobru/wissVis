#pragma once


namespace fantom
{
    namespace io
    {
        enum class Endianness
        {
            BIG,
            LITTLE,
        };

        Endianness systemEndianness();
    } // namespace io
} // namespace fantom
