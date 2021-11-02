#pragma once

#include <cmath>
#include <ostream>

// forward declarations
//=================================================================================================

namespace fantom
{
    template < typename TType, size_t... TDimensions >
    class Tensor;
}


// declarations
//=============================================================================

namespace fantom
{
    /// Converts degrees in radians
    template < typename TType >
    TType degToRad( TType deg );


    template < typename TType >
    class Quaternion
    {
    public:
        Quaternion();
        Quaternion( TType w, TType x, TType y, TType z );
        Quaternion( TType rot, Tensor< TType, 3 > const& axis );
        Quaternion( Tensor< TType, 3 > const& from, Tensor< TType, 3 > const& to );

        /// Creates a quaternion from a rotation matrix.
        Quaternion( Tensor< TType, 3, 3 > const& matrix );

        TType w() const;
        TType x() const;
        TType y() const;
        TType z() const;

        TType& w();
        TType& x();
        TType& y();
        TType& z();

        TType azimuth() const;
        TType polar() const;

    private:
        TType m_w;
        TType m_x;
        TType m_y;
        TType m_z;
    };

    template < typename TType >
    TType norm( Quaternion< TType > const& quat );

    template < typename TType >
    TType norm2( Quaternion< TType > const& quat );

    template < typename TType >
    Quaternion< TType > inverse( Quaternion< TType > const& quat );

    template < typename TType >
    Quaternion< TType > normalize( Quaternion< TType > const& quat );

    template < typename TType >
    Quaternion< TType > operator*( Quaternion< TType > const& lhs, Quaternion< TType > const& rhs );

    template < typename TType >
    Quaternion< TType > operator*( TType lhs, Quaternion< TType > const& rhs );

    template < typename TType >
    Quaternion< TType > operator*( Quaternion< TType > const& lhs, TType rhs );

    template < typename TType >
    Quaternion< TType > operator+( Quaternion< TType > const& lhs, Quaternion< TType > const& rhs );

    template < typename TType >
    Tensor< TType, 3 > operator*( Quaternion< TType > const& lhs, Tensor< TType, 3 > const& rhs );

    template < typename TType >
    std::ostream& operator<<( std::ostream& stream, Quaternion< TType > const& quat );
}


// definitions
//=============================================================================

namespace fantom
{
    template < typename TType >
    inline TType degToRad( TType deg )
    {
        return deg / static_cast< TType >( 180 ) * static_cast< TType >( M_PI );
    }


    template < typename TType >
    inline Quaternion< TType >::Quaternion()
        : m_w( 1 )
        , m_x( 0 )
        , m_y( 0 )
        , m_z( 0 )
    {
    }

    template < typename TType >
    inline Quaternion< TType >::Quaternion( TType w, TType x, TType y, TType z )
        : m_w( w )
        , m_x( x )
        , m_y( y )
        , m_z( z )
    {
    }

    template < typename TType >
    Quaternion< TType >::Quaternion( TType rot, Tensor< TType, 3 > const& axis )
    {
        using std::sin;

        auto factor = sin( rot / TType( 2 ) ) / norm( axis );

        m_w = std::cos( rot / TType( 2 ) );
        m_x = axis( 0 ) * factor;
        m_y = axis( 1 ) * factor;
        m_z = axis( 2 ) * factor;
    }

    template < typename TType >
    Quaternion< TType >::Quaternion( Tensor< TType, 3 > const& from, Tensor< TType, 3 > const& to )
    {
        if( from == -to )
        {
            // get an axis that is orthogonal to from / to
            Tensor< TType, 3 > tmp( 1, 0, 0 );
            auto axis = cross( tmp, from );
            if( norm( axis ) == 0 )
            {
                tmp( 1 ) = 1;
                axis = cross( tmp, from );
            }
            axis = normalized( axis );
            m_w = 0.0;
            m_x = axis( 0 );
            m_y = axis( 1 );
            m_z = axis( 2 );
        }
        else
        {
            Tensor< TType, 3 > halfVec = normalized( from + to );
            m_w = halfVec * to;
            halfVec = cross( halfVec, to );
            m_x = halfVec( 0 );
            m_y = halfVec( 1 );
            m_z = halfVec( 2 );
        }
    }

    template < typename TType >
    Quaternion< TType >::Quaternion( Tensor< TType, 3, 3 > const& matrix )
    {
        using std::sqrt;
        auto tr = trace( matrix );

        if( tr > 0 )
        {
            auto s = sqrt( tr + 1 ) * 2;
            m_w = TType( 0.25 ) * s;
            m_x = ( matrix( 2, 1 ) - matrix( 1, 2 ) ) / s;
            m_y = ( matrix( 0, 2 ) - matrix( 2, 0 ) ) / s;
            m_z = ( matrix( 1, 0 ) - matrix( 0, 1 ) ) / s;
        }
        else if( ( matrix( 0, 0 ) > matrix( 1, 1 ) ) & ( matrix( 0, 0 ) > matrix( 2, 2 ) ) )
        {
            auto s = sqrt( 1 + matrix( 0, 0 ) - matrix( 1, 1 ) - matrix( 2, 2 ) ) * 2;
            m_w = ( matrix( 2, 1 ) - matrix( 1, 2 ) ) / s;
            m_x = TType( 0.25 ) * s;
            m_y = ( matrix( 0, 1 ) + matrix( 1, 0 ) ) / s;
            m_z = ( matrix( 0, 2 ) + matrix( 2, 0 ) ) / s;
        }
        else if( matrix( 1, 1 ) > matrix( 2, 2 ) )
        {
            auto s = sqrt( 1.0 + matrix( 1, 1 ) - matrix( 0, 0 ) - matrix( 2, 2 ) ) * 2;
            m_w = ( matrix( 0, 2 ) - matrix( 2, 0 ) ) / s;
            m_x = ( matrix( 0, 1 ) + matrix( 1, 0 ) ) / s;
            m_y = TType( 0.25 ) * s;
            m_z = ( matrix( 1, 2 ) + matrix( 2, 1 ) ) / s;
        }
        else
        {
            auto s = sqrt( 1.0 + matrix( 2, 2 ) - matrix( 0, 0 ) - matrix( 1, 1 ) ) * 2;
            m_w = ( matrix( 1, 0 ) - matrix( 0, 1 ) ) / s;
            m_x = ( matrix( 0, 2 ) + matrix( 2, 0 ) ) / s;
            m_y = ( matrix( 1, 2 ) + matrix( 2, 1 ) ) / s;
            m_z = TType( 0.25 ) * s;
        }
    }

    template < typename TType >
    inline TType Quaternion< TType >::w() const
    {
        return m_w;
    }

    template < typename TType >
    inline TType Quaternion< TType >::x() const
    {
        return m_x;
    }

    template < typename TType >
    inline TType Quaternion< TType >::y() const
    {
        return m_y;
    }

    template < typename TType >
    inline TType Quaternion< TType >::z() const
    {
        return m_z;
    }

    template < typename TType >
    inline TType& Quaternion< TType >::w()
    {
        return m_w;
    }

    template < typename TType >
    inline TType& Quaternion< TType >::x()
    {
        return m_x;
    }

    template < typename TType >
    inline TType& Quaternion< TType >::y()
    {
        return m_y;
    }

    template < typename TType >
    inline TType& Quaternion< TType >::z()
    {
        return m_z;
    }

    template < typename TType >
    inline TType Quaternion< TType >::azimuth() const
    {
        using std::atan2;
        return atan2( m_y, m_x ) * TType( 114.5915590262 );
    }

    template < typename TType >
    inline TType Quaternion< TType >::polar() const
    {
        using std::atan2;
        return atan2( m_y, m_z ) * TType( 114.5915590262 );
    }

    template < typename TType >
    inline TType norm( Quaternion< TType > const& quat )
    {
        using std::sqrt;
        return sqrt( norm2( quat ) );
    }

    template < typename TType >
    inline TType norm2( Quaternion< TType > const& quat )
    {
        return quat.w() * quat.w() + quat.x() * quat.x() + quat.y() * quat.y() + quat.z() * quat.z();
    }

    template < typename TType >
    inline Quaternion< TType > inverse( Quaternion< TType > const& quat )
    {
        return { quat.w(), -quat.x(), -quat.y(), -quat.z() };
    }

    template < typename TType >
    inline Quaternion< TType > normalize( Quaternion< TType > const& quat )
    {
        TType len = norm( quat );
        return { quat.w() / len, quat.x() / len, quat.y() / len, quat.z() / len };
    }

    template < typename TType >
    Quaternion< TType > operator*( Quaternion< TType > const& lhs, Quaternion< TType > const& rhs )
    {
        return { lhs.w() * rhs.w() - lhs.x() * rhs.x() - lhs.y() * rhs.y() - lhs.z() * rhs.z(),
                 lhs.w() * rhs.x() + lhs.x() * rhs.w() + lhs.y() * rhs.z() - lhs.z() * rhs.y(),
                 lhs.w() * rhs.y() + lhs.y() * rhs.w() + lhs.z() * rhs.x() - lhs.x() * rhs.z(),
                 lhs.w() * rhs.z() + lhs.z() * rhs.w() + lhs.x() * rhs.y() - lhs.y() * rhs.x() };
    }

    template < typename TType >
    Quaternion< TType > operator*( TType scalar, Quaternion< TType > const& quat )
    {
        return { quat.w() * scalar, quat.x() * scalar, quat.y() * scalar, quat.z() * scalar };
    }

    template < typename TType >
    Quaternion< TType > operator*( Quaternion< TType > const& quat, TType scalar )
    {
        return { quat.w() * scalar, quat.x() * scalar, quat.y() * scalar, quat.z() * scalar };
    }

    template < typename TType >
    Quaternion< TType > operator+( Quaternion< TType > const& lhs, Quaternion< TType > const& rhs )
    {
        return { lhs.w() + rhs.w(), lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z() };
    }

    template < typename TType >
    Tensor< TType, 3 > operator*( Quaternion< TType > const& lhs, Tensor< TType, 3 > const& rhs )
    {
        Quaternion< TType > inv = inverse( lhs );
        Quaternion< TType > x( 0, rhs( 0 ), rhs( 1 ), rhs( 2 ) );
        inv = lhs * x * inv;
        return { inv.x(), inv.y(), inv.z() };
    }

    template < typename TType >
    std::ostream& operator<<( std::ostream& stream, Quaternion< TType > const& quat )
    {
        stream << "[" << quat.w() << ", " << quat.x() << ", " << quat.y() << ", " << quat.z() << "]";
        return stream;
    }
}
