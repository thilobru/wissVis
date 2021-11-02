#pragma once


namespace fantom
{
    template < typename TType, size_t TDimension >
    class Range
    {
    public:
        Range();

        Range( const Tensor< TType, TDimension >& min, const Tensor< TType, TDimension >& max );

        const Tensor< TType, TDimension >& min() const;

        const Tensor< TType, TDimension >& max() const;

        TType min( size_t dimension ) const;

        TType max( size_t dimension ) const;

        TType extent( size_t dimension ) const;

    private:
        Tensor< TType, TDimension > mMin;
        Tensor< TType, TDimension > mMax;
    };


    template < typename TType, size_t TDimension >
    std::ostream& operator<<( std::ostream& stream, const Range< TType, TDimension >& range );
} // namespace fantom


namespace fantom
{
    template < typename TType, size_t TDimension >
    Range< TType, TDimension >::Range()
        : mMin()
        , mMax()
    {
    }

    template < typename TType, size_t TDimension >
    inline Range< TType, TDimension >::Range( const Tensor< TType, TDimension >& min,
                                              const Tensor< TType, TDimension >& max )
        : mMin( min )
        , mMax( max )
    {
    }

    template < typename TType, size_t TDimension >
    inline const Tensor< TType, TDimension >& Range< TType, TDimension >::min() const
    {
        return mMin;
    }

    template < typename TType, size_t TDimension >
    inline const Tensor< TType, TDimension >& Range< TType, TDimension >::max() const
    {
        return mMax;
    }

    template < typename TType, size_t TDimension >
    inline TType Range< TType, TDimension >::min( size_t dimension ) const
    {
        return mMin( dimension );
    }

    template < typename TType, size_t TDimension >
    inline TType Range< TType, TDimension >::max( size_t dimension ) const
    {
        return mMax( dimension );
    }

    template < typename TType, size_t TDimension >
    inline TType Range< TType, TDimension >::extent( size_t dimension ) const
    {
        return max( dimension ) - min( dimension );
    }


    template < typename TType, size_t TDimension >
    inline std::ostream& operator<<( std::ostream& stream, const Range< TType, TDimension >& range )
    {
        stream << "[ [ ";

        for( size_t d = 0; d < TDimension; ++d )
        {
            if( d )
            {
                stream << ", ";
            }

            stream << range.min( d );
        }

        stream << " ], [ ";

        for( size_t d = 0; d < TDimension; ++d )
        {
            if( d )
            {
                stream << ", ";
            }

            stream << range.max( d );
        }

        stream << " ]";

        return stream;
    }
} // namespace fantom
