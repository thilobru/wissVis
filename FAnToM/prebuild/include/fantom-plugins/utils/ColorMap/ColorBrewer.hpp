#pragma once

#include <fantom/math.hpp>

#include <vector>


//=========================================================================================================================
// declarations
//=========================================================================================================================

namespace fantom
{
    /// Access to qualitative color brewer schemes
    /**
     * \ingroup PlottingLibrary
     */
    class ColorBrewerQualitative
    {
    public:
        static std::vector< std::string > const& getNames();

        static std::vector< fantom::Color > const& getBrewerColors( std::string const& name );

        /// Create a vector with the colors for the given number of categories for at most 12 categories.
        /// If less categories are possible, less will be returned.
        static std::vector< fantom::Color > getBrewerColors( std::string const& name, size_t count );
    };


    /// Construct diverging color brewer schemes (for 3 to 11 classes)
    /**
     * \ingroup PlottingLibrary
     */
    class ColorBrewerDiverging
    {
    public:
        static std::vector< std::string > const& getNames();

        static std::vector< fantom::Color > const& getBrewerColors( std::string const& name );

        /// Create a vector with the colors for the given number of categories (3 to 11).
        static std::vector< fantom::Color > getBrewerColors( std::string const& name, size_t count );
    };


    /// Construct sequential color brewer schemes (for 3 to 9 classes)
    /**
     * \ingroup PlottingLibrary
     */
    class ColorBrewerSequential
    {
    public:
        static std::vector< std::string > const& getNames();

        static std::vector< fantom::Color > const& getBrewerColors( std::string const& name );

        /// Create a vector with the colors for the given number of categories (3 to 9).
        static std::vector< fantom::Color > getBrewerColors( std::string const& name, size_t count );
    };


    /**
     * Wrapper for index based access to Color Brewer color maps.
     *
     * General usage:
     *   (i)   initialize with type and color map name.
     *   (ii)  set the number of requested colors (important!)
     *   (iii) access the colors via operator[]
     * Color Brewer color maps support up to 12 elements, but usually not that much.
     * If you request more colors, the last color of the map is used to fill up the map.
     *
     * This class is also usable in the Algorithm::Options. Just use add<BrewerMapper>.
     * A specialized input will be presented to the user for selecting an appropriate color map.
     */
    class BrewerMapper
    {
    public:
        enum class Type
        {
            Qualitative,
            Diverging,
            Sequential
        };

        BrewerMapper();

        BrewerMapper( Type type, std::string const& brewerName, size_t number );

        BrewerMapper( Type type, std::string const& brewerName );

        BrewerMapper( BrewerMapper const& ) = default;

        BrewerMapper& operator=( BrewerMapper const& ) = default;

        static BrewerMapper makeDefaultQualitative();

        static BrewerMapper makeDefaultDiverging();

        static BrewerMapper makeDefaultSequential();

        void setColorCount( size_t number );

        size_t getMaximalUsefulColorCount() const;

        Type getType() const;

        std::string const& getName() const;

        /// map the color
        fantom::Color operator[]( size_t index ) const;

        bool operator==( BrewerMapper const& rhs ) const;

        bool operator!=( BrewerMapper const& rhs ) const;

    private:
        fantom::Color mapQualitative( size_t number ) const;

        fantom::Color mapDiverging( size_t number ) const;

        fantom::Color mapSequential( size_t number ) const;

        Type mType;
        std::string mName;
        size_t mNumber;
        std::vector< fantom::Color > const* mBaseColors;
    };

    std::ostream& operator<<( std::ostream& stream, BrewerMapper const& b );

    std::istream& operator>>( std::istream& stream, BrewerMapper& b );
} // namespace fantom


//=========================================================================================================================
// inline definitions
//=========================================================================================================================

namespace fantom
{
    inline BrewerMapper::BrewerMapper()
        : mType( Type::Qualitative )
        , mNumber( 0 )
        , mBaseColors( nullptr )
    {
    }

    inline BrewerMapper BrewerMapper::makeDefaultQualitative()
    {
        return BrewerMapper( Type::Qualitative, "paired" );
    }

    inline BrewerMapper BrewerMapper::makeDefaultDiverging()
    {
        return BrewerMapper( Type::Diverging, "rdgy" );
    }

    inline BrewerMapper BrewerMapper::makeDefaultSequential()
    {
        return BrewerMapper( Type::Sequential, "oranges" );
    }

    inline BrewerMapper::Type BrewerMapper::getType() const
    {
        return mType;
    }

    inline std::string const& BrewerMapper::getName() const
    {
        return mName;
    }

    inline bool BrewerMapper::operator==( BrewerMapper const& rhs ) const
    {
        return mBaseColors == rhs.mBaseColors && mType == rhs.mType;
    }

    inline bool BrewerMapper::operator!=( BrewerMapper const& rhs ) const
    {
        return !( *this == rhs );
    }
} // namespace fantom
