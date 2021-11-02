#pragma once

#include "../math.hpp"
#include "DataObject.hpp"

namespace fantom
{
    /**
    * @brief
    * an ROI (Region of Interest) is a bounding box
    * defined by two points in space.
    *
    * \ingroup picking
    *
    * The internal storage is of a 3D object, where
    * the queries of lower-dimensional objects are
    * seen with the higher-dimensional components
    * being set to 0.0 . Thus, a 2D bounding box
    * is one that only includes points in the plane
    * z = 0.0 .
    *
    * For the bounding box to fill any space, it must
    * hold mLowerBound[ i ] <= mUpperBound[ i ] where
    * the special case with
    * mLowerBound[ i ] == mUpperBound[ i ] forall i
    * contains exactly one point.
    *
    * The bounding box defines the closed set of points
    * x for which
    * mLowerBound[ i ] <= x <= mUpperBound[ i ]
    */
    class ROI : public DataObject
    {
        typedef RTTI_Info< ROI, DataObject > TypeInfo;
        TYPE_INFORMATION( "Region of Interest" )

        Point3 mLowerBound, mUpperBound;

    public:
        // the ROI is defined including both boundary points
        ROI( const Point1& lowerBound, const Point1& upperBound );
        ROI( const Point2& lowerBound, const Point2& upperBound );
        ROI( const Point3& lowerBound, const Point3& upperBound );

        Point3 getLowerBound() const;
        Point3 getUpperBound() const;
        bool setLowerBound( const Point3& lowerBound );
        bool setUpperBound( const Point3& upperBound );
        bool contains( const Point1& position ) const;
        bool contains( const Point2& position ) const;
        bool contains( const Point3& position ) const;

        virtual std::vector< std::pair< std::string, std::string > > getInfoStrings() const override;
    };
}
