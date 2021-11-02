#pragma once

#include <fantom/math.hpp> //< for Color
#include <fantom/outputs.hpp>
#include <fantom/rtti.hpp>

// #include <iostream>
#include <map> //< std::pair
#include <string>
#include <vector>

namespace fantom
{
    namespace Graphics2D
    {
        class Image;
    }

    // color map is defined below
    class ColorMap;

    /**
     * @brief A data object type that can be used in the flow graph that
     * stores a color map object.
     *
     * Use this object in your algorithm options as:
     * add<ColorMapDataObject>("Color Map", "Color transfer function" );
     * or in the filtered version
     * add<ColorMapDataObject>("Color Map", "Color transfer function", colorMapFilter );
     *
     * To retreive the object, do a
     *
     * auto colormapobj = parameters.get < ColorMapDataObject >( "Color Map" );
     * if( colormapobj )
     * {
     *      auto colormap = colormapobj->getColorMap();
     *      if( colormap ){...}
     * }
     */
    class ColorMapDataObject : public DataObject
    {
        typedef RTTI_Info< ColorMapDataObject, DataObject > TypeInfo;
        TYPE_INFORMATION( "ColorMapDataObject" )
    public:
        /// constructor: creates an empty object
        ColorMapDataObject();

        /// constructor: assigns a color map
        ColorMapDataObject( std::shared_ptr< ColorMap > cm );

        /// reimplemented from DataObject
        virtual std::vector< std::pair< std::string, std::string > > getInfoStrings() const override;

        /// set a new color map
        void setColorMap( std::shared_ptr< ColorMap > cm )
        {
            mColorMap = cm;
        }

        /// receiver must check if returned object is valid
        std::shared_ptr< ColorMap > getColorMap() const
        {
            return mColorMap;
        }

        /// sets a range in which the colormap is valid
        void setRange( double newMin, double newMax )
        {
            mMin = newMin;
            mMax = newMax;
        }

        /**
         * @brief returns the min and max value of the range in which the color map is valid
         * It is the user's responsibility to map the input values into the range when using
         * the color map object
         */
        std::pair< double, double > getRange() const
        {
            return std::make_pair( mMin, mMax );
        }

        void setOutOfLowerBoundsColor( const Color& color )
        {
            mOutOfLowerBoundsColor = color;
        }

        void setOutOfUpperBoundsColor( const Color& color )
        {
            mOutOfUpperBoundsColor = color;
        }

        Color getOutOfUpperBoundsColor() const
        {
            return mOutOfUpperBoundsColor;
        }

        Color getOutOfLowerBoundsColor() const
        {
            return mOutOfLowerBoundsColor;
        }

        void setInvalidValueColor( const Color& color )
        {
            mInvalidValueColor = color;
        }

        Color getInvalidValueColor() const
        {
            return mInvalidValueColor;
        }

        /**
         * @brief maps the value t to the parameter between mMin and mMax
         */
        double mapValue( double t ) const;

        /**
         * convenience function that maps the value to a color honoring the specified range
         * @pre the color map is valid. Otherwise, the behavior is undefined
         */
        Color mapValueToColorScaled( double t ) const;

    private:
        std::shared_ptr< ColorMap > mColorMap;
        double mMin;
        double mMax;

        Color mOutOfLowerBoundsColor; ///< the color used for values below the min value
        Color mOutOfUpperBoundsColor; ///< the color used for values above the max value
        Color mInvalidValueColor;     ///< the color used for nan and inf values
    };

    /**
     * Base class for all color maps
     */
    class ColorMap : public RTTI
    {
        typedef RTTI_Info< ColorMap > TypeInfo;
        TYPE_INFORMATION( "ColorMap" )
    public:
        /**
         * Default Constructor
         */
        ColorMap();

        /**
         * Default destructor
         */
        virtual ~ColorMap();

        /**
         * create a deep copy of the color map
         */
        virtual std::shared_ptr< ColorMap > clone() const = 0;

        /**
         * returns the name of the color map
         */
        const std::string& getName() const;

        /**
         * \brief maps the value in [ 0,1 ] ( closed interval ) to
         * a color
         *
         * The function must be implemented thread-safe as
         * multiple threads and even multiple parts of the
         * code will work on the same object.
         *
         * \returns the color for parameter t
         */
        virtual Color mapValueToColor( double t ) const = 0;

        // data object interface

        virtual std::vector< std::pair< std::string, std::string > > getInfoStrings() const;

    protected:
        /**
         * name is the global identifier that must be unique in the color map factory
         * It should be "Custom" for any other object that is not handled in the factory
         */
        std::string mName;
    };

    class TabularColorMap : public ColorMap
    {
        typedef RTTI_Info< TabularColorMap, TabularColorMap > TypeInfo;
        TYPE_INFORMATION( "TabularColorMap" )
    public:
        TabularColorMap();

        virtual ~TabularColorMap();

        /**
         * return the number of linearly interpolated
         * colors used for the map.
         * \return number of colors
         */
        size_t numColors() const;

        /**
         * get the i-th color used
         * \pre i must be in  { 0, ..., numColors() - 1 }
         * \param i the index of the color
         * \returns Color the i-th color
         */
        const Color& getColor( size_t i ) const;

        /// reimplemented from ColorMap
        virtual std::vector< std::pair< std::string, std::string > > getInfoStrings() const override;

    protected:
        //! the number of colors in the map
        size_t mNumColors;
        //! a pointer to the list of colors
        Color const* mColor;
    };

    /**
     * \brief Default object for linearly interpolated color maps
     *
     * The color map interpolates color values linearly in RGBA
     * space.
     */
    class LinearColorMapRGB : public TabularColorMap
    {
        typedef RTTI_Info< LinearColorMapRGB, TabularColorMap > TypeInfo;
        TYPE_INFORMATION( "LinearColorMapRGB" )
    public:
        /**
         * Default Constructor
         */
        LinearColorMapRGB();

        virtual ~LinearColorMapRGB();

        /**
         * @brief returns a complete copy of the color map
         * @returns the copy
         */
        // virtual std::shared_ptr< ColorMap> clone() const override

        /**
         * @brief Reimplementation of same function from ColorMap
         *
         * Performs a linear interpolation of the colors stored in the
         * TabularColorMap based on RGB values
         *
         * \param t a parameter between [ 0, 1 ] to a color
         * \returns the color for parameter t
         */
        virtual Color mapValueToColor( double t ) const override;
    };

    namespace
    {
        /**
         * @brief less comparison of first component of a pair
         */
        template < class T >
        struct less_first
        {
            bool operator()( const std::pair< float, T >& a, const std::pair< float, T >& b )
            {
                return a.first < b.first;
            }
        };
    }

    class CustomLinearColorMapRGB : public LinearColorMapRGB
    {
        typedef RTTI_Info< CustomLinearColorMapRGB, LinearColorMapRGB > TypeInfo;
        TYPE_INFORMATION( "CustomLinearColorMapRGB" )

    public:
        CustomLinearColorMapRGB()
        {
            mName = "Custom";
        }

        CustomLinearColorMapRGB( const CustomLinearColorMapRGB& rhs )
        {
            mName = "Custom";
            mColors = rhs.mColors;
            mAlphas = rhs.mAlphas;
        }

        explicit CustomLinearColorMapRGB( std::string string )
        {
            mName = "Custom";
            setFromString( string );
        }

        virtual ~CustomLinearColorMapRGB()
        {
        }

        /**
         * clone the color map object
         */
        virtual std::shared_ptr< ColorMap > clone() const override
        {
            return std::shared_ptr< ColorMap >( new CustomLinearColorMapRGB( *this ) );
        }

        /**
         * Reverse the color map, i.e., map the interval from back to front instead of front to back.
         */
        void reverse()
        {
            std::reverse( mColors.begin(), mColors.end() );
            for( auto& p : mColors )
            {
                p.first = 1.0 - p.first;
            }
            std::reverse( mAlphas.begin(), mAlphas.end() );
            for( auto& p : mAlphas )
            {
                p.first = 1.0 - p.first;
            }
        }

        /**
         * @brief create a copy of the given object
         *
         * try to get a good copy of the given color map object by either copying the data
         * directly, by creating a copy of the table, or by resampling the data is we have
         * no better option.
         */
        void makeCopyOf( const ColorMap* colorMap )
        {
            if( auto rhs = dynamic_cast< const CustomLinearColorMapRGB* >( colorMap ) )
            {
                // std::cout <<"-- copy from CustomLinearColorMapRGB" << std::endl;
                ( *this ) = *rhs;
            }
            else if( auto rhs = dynamic_cast< const TabularColorMap* >( colorMap ) )
            {
                // std::cout <<"-- copy from TabularColorMap" << std::endl;
                makeCopyOf( *rhs );
            }
            else
            {
                // std::cout <<"-- copy from (make sampled copy)" << std::endl;
                /// TODO: find a mechanism to determine the best number of samples used here
                makeDiscreteCopy( colorMap, 32 );
            }
        }

        void makeDiscreteCopy( const ColorMap* colorMap, size_t samples )
        {
            mColors.clear();
            mAlphas.clear();

            for( size_t i = 0; i < samples; ++i )
            {
                float param = (float)i / (float)( samples - 1 );
                addColor( colorMap->mapValueToColor( param ), param );
            }
            addAlpha( 1.0, 0.0 );
            addAlpha( 1.0, 1.0 );

            mNumColors = mColors.size();
        }

        // helper to create a copy of a default color map
        void makeCopyOf( const TabularColorMap& rhs )
        {
            mColors.clear();
            mAlphas.clear();

            // std::cout << "copying numColors:" << rhs.numColors() << std::endl;
            double lastAlpha = 1.0;
            for( size_t i = 0; i < rhs.numColors(); ++i )
            {
                float param = (float)i / (float)( rhs.numColors() - 1 );
                addColor( rhs.getColor( i ), param );
                if( i == 0 || i == rhs.numColors() - 1 )
                {
                    // add first and last alpha value
                    addAlpha( rhs.getColor( i ).a(), param );
                    lastAlpha = rhs.getColor( i ).a();
                }
                else if( lastAlpha != rhs.getColor( i ).a() )
                {
                    // add other values only if they differ (typically they are all 1.0, so we do not want sampling
                    // points there)
                    addAlpha( rhs.getColor( i ).a(), param );
                    lastAlpha = rhs.getColor( i ).a();
                }
            }

            mNumColors = mColors.size();
        }

        static double clamp( double d )
        {
            return d < 0. ? 0. : ( d > 1. ? 1. : d );
        }

        static float clamp( float d )
        {
            return d < 0.f ? 0.f : ( d > 1.f ? 1.f : d );
        }

        // user interface
        virtual void addColor( const Color& rgba, float pos )
        {
            mColors.push_back( std::make_pair( clamp( pos ), rgba ) );
            std::sort( mColors.begin(), mColors.end(), less_first< Color >() );
            mNumColors = mColors.size();
        }

        virtual void removeColor( size_t id )
        {
            auto iter = mColors.begin();
            iter = iter + id;
            mColors.erase( iter );
            mNumColors = mColors.size();
        }

        virtual void moveColor( size_t idx, const Color& color, float pos )
        {
            if( idx == 0 )
            {
                pos = 0.0;
            }
            if( idx > 0 )
            {
                pos = std::max( pos, mColors[idx - 1].first );
            }
            if( idx < mColors.size() - 1 )
            {
                pos = std::min( pos, mColors[idx + 1].first );
            }
            else
            {
                pos = 1.0;
            }
            mColors[idx] = std::make_pair( pos, color );
        }

        virtual void addAlpha( float alpha, float pos )
        {
            mAlphas.push_back( std::make_pair( clamp( pos ), clamp( alpha ) ) );
            std::sort( mAlphas.begin(), mAlphas.end(), less_first< float >() );
        }

        virtual void moveAlpha( size_t idx, float alpha, float pos )
        {
            if( idx == 0 )
            {
                pos = 0.0;
            }
            else if( idx > 0 )
            {
                pos = std::max( pos, mAlphas[idx - 1].first );
            }
            if( idx < mAlphas.size() - 1 )
            {
                pos = std::min( pos, mAlphas[idx + 1].first );
            }
            else
            {
                pos = 1.0;
            }
            mAlphas[idx] = std::make_pair( clamp( pos ), clamp( alpha ) );
        }

        virtual void removeAlpha( size_t id )
        {
            auto iter = mAlphas.begin();
            iter = iter + id;
            mAlphas.erase( iter );
        }

        // private
        float computeParameter( float from, float to, float position ) const
        {
            return ( position - from ) / ( to - from );
        }


        // private
        float interpolate( float a, float b, float param ) const
        {
            float t2 = param;
            float t1 = 1.f - param;
            return a * t1 + b * t2;
        }

        // private
        Color interpolate( Color a, Color b, float param ) const
        {
            float t2 = param;
            float t1 = 1.f - param;
            return Color(
                a.r() * t1 + b.r() * t2, a.g() * t1 + b.g() * t2, a.b() * t1 + b.b() * t2, a.a() * t1 + b.a() * t2 );
        }

        /// public interpolated color map interface
        virtual Color mapValueToColor( double t ) const override;

        /**
         * return the number of linearly interpolated
         * colors used for the map.
         * \return number of colors
         */
        size_t numColors() const
        {
            return mColors.size();
        }

        /**
         * return the number of linearly interpolated
         * colors used for the map.
         * \return number of colors
         */
        size_t numAlphas() const
        {
            return mAlphas.size();
        }


        /**
         * get the i-th color used
         * \pre i must be in  { 0, ..., numColors() - 1 }
         * \param i the index of the color
         * \returns Color the i-th color
         * the returned alpha value is useless
         */
        const Color& getColor( size_t i ) const
        {
            if( i >= mColors.size() )
            {
                throw std::logic_error( "Invalid index in color map LinearColorMapRGB::getColor(...)" );
            }
            return mColors[i].second;
        }

        /**
         * \pre i must be in { 0, ..., numColors() - 1 }
         * \param i the index of the color value
         * \returns the position of the i-th color value
         */
        float getColorPosition( size_t i ) const
        {
            return mColors[i].first;
        }

        /**
         * get the i-th color used
         * \pre i must be in  { 0, ..., numAlphas() - 1 }
         * \param i the index of the alpha value
         * \returns the i-th alpha value
         */
        float getAlpha( size_t i ) const
        {
            if( i >= mAlphas.size() )
            {
                throw std::logic_error( "Invalid index in color map LinearColorMapRGB::getColor(...)" );
            }
            return mAlphas[i].second;
        }

        /**
         * \pre i must be in { 0, ..., numAlphas() - 1 }
         * \param i the index of the alpha value
         * \returns the position of the i-th alpha value
         */
        float getAlphaPosition( size_t i ) const
        {
            return mAlphas[i].first;
        }

        /**
         * Creates a color map from an image where the x-axis is seen as the parameter of the map
         *
         * @param image a 2D image from which the center line in y direction is used
         * @param errorThreshold the maximal allowed error in value for each pixel
         * @returns a color map that resembles the center line in y direction of the image
         *
         * @pre image has at least one pixel in x and y direction
         */
        static std::shared_ptr< CustomLinearColorMapRGB > createColorMapFromImage( const Graphics2D::Image& image,
                                                                                   double errorThreshold = 5.0 / 255. );

        /**
         * @see createColorMapFromImage
         */
        static std::shared_ptr< CustomLinearColorMapRGB > createColorMapFromImage( const std::vector< Color >& colors,
                                                                                   double errorThreshold = 5.0 / 255. );

        /**
         * creates a string representation that can be parsed by setFromString
         */
        std::string toString() const;

        /**
         * parses the string representation and initializes the color map accordingly
         */
        void setFromString( std::string cm );


    private:
        std::vector< std::pair< float, Color > > mColors;
        std::vector< std::pair< float, float > > mAlphas;
    };


    /**
     * the factory keeping track of different color map types
     * TODO( mario ): maybe anyone changes this to registry class that is now in the kernel?
     */
    class ColorMapFactory
    {
    public:
        /**
         * Destructor
         */
        virtual ~ColorMapFactory();

        /**
         * the number of available color maps
         */
        size_t numColorMaps() const;

        /**
         * returns a default color map
         */
        ColorMap* getDefaultColorMap() const;

        /**
         * get a list of names of color maps
         */
        std::vector< std::string > getNames() const;

        /**
         * get a pointer to a color map object defined by its unique name
         *
         * \returns pointer to the requested color map. The pointer is a
         * global pointer and construction/deletion is handled by the kernel
         */
        const ColorMap* getColorMapByName( const std::string& name ) const;

        /**
         * add a new color map to the factory.
         * you keep responsibility for the object being available as long as it
         * is registered and delete it when the software closes.
         * This pointer is handed over to different parts of the code.
         */
        static void registerColorMap( std::string& name, ColorMap* colorMap );

        /**
         * returns a pointer to the factory singleton
         */
        static ColorMapFactory* theFactory();

    private:
        /**
         * Constructor
         */
        ColorMapFactory();

        /**
         * private implementation of registerColorMap( ... )
         */
        void registerColorMapPrivate( std::string& name, ColorMap* colorMap );

        //! internal data structure to hold the colors
        // TODO: make this a PIMPL?
        std::map< std::string, ColorMap* > mColorMaps;
    };

} // namespace fantom
