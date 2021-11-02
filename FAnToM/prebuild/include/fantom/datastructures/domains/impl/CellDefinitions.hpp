#pragma once

#include "../../../cells.hpp"
#include "../../../math.hpp"
#include "../../ValueArray.hpp"

/**
 * This file defines the basic topological information of the cell types used in FAnToM.
 *
 * The basic structure is:
 * \code{.cpp}
 * struct NewCellType
 * {
 * static const Cell::Stats stats; //< the stats object
 * static const Cell::Type type = Cell::NEWCELLTYPE; //< the unique name of the cell
 * static const std::size_t dimension = 0; //< the topological dimension of the cell
 * static const std::size_t numPoints = 1; //< the number of vertices in the cell
 * static const std::size_t numValues = numPoints; //< the number of values in a cell
 * static const std::size_t numFaces = 0; //< the number of faces of a cell
 * };
 * \endcode
 *
 * Every cell can contain an interpolate function that computes the interpolation weights for the cell vertices form the
 * internal coordinates:
 * \code{.cpp} void interpolate( double coefficients[], const double coordinates[] ); \endcode
 * In addition, a cell may implement contain-functions for any spatial dimension where it will be used:
 * \code{.cpp} bool contains( double coordinates[], const PointD& position, const ValueArray<VectorD>& points, const
 *std::size_t indices[] ); \endcode
 * It returns true iff the cell contains the position and in that case returns the internal coordinates in coordinates.
 */
// TODO(SV): make room for acceleration structure

namespace fantom
{
    namespace cells
    {
        // define how many points we must allocate if we want to allocate information about all values we store in the
        // cell, such as for the interpolation weights
        constexpr std::size_t maxNumValuesPerCell
            = 20;                                // currently the QuadraticHexahedron, but this may change in the future
        constexpr std::size_t maxDimensions = 3; // currently only cells with up to three dimesions are supported

        struct PointCell
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::POINT;
            static const std::size_t dimension = 0;
            static const std::size_t numPoints = 1;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 0;
        };


        struct LinearLine
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::LINE;
            static const std::size_t dimension = 1;
            static const std::size_t numPoints = 2;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 2;
            static bool contains( double coordinates[],
                                  const Point1& pos,
                                  const ValueArray< Vector1 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct LinearTriangle
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::TRIANGLE;
            static const std::size_t dimension = 2;
            static const std::size_t numPoints = 3;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 3;
            static bool contains( double coordinates[],
                                  const Point2& pos,
                                  const ValueArray< Vector2 >& points,
                                  const std::size_t indices[] );
            static bool contains( double coordinates[],
                                  const Point3& pos,
                                  const ValueArray< Vector3 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct LinearQuad
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::QUAD;
            static const std::size_t dimension = 2;
            static const std::size_t numPoints = 4;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 4;
            static bool contains( double coordinates[],
                                  const Point2& pos,
                                  const ValueArray< Vector2 >& points,
                                  const std::size_t indices[] );
            static bool contains( double coordinates[],
                                  const Point3& pos,
                                  const ValueArray< Vector3 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct LinearTetrahedron
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::TETRAHEDRON;
            static const std::size_t dimension = 3;
            static const std::size_t numPoints = 4;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 4;
            static const std::size_t faces[][4];
            static bool contains( double coordinates[],
                                  const Point3& pos,
                                  const ValueArray< Vector3 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct LinearPyramid
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::PYRAMID;
            static const std::size_t dimension = 3;
            static const std::size_t numPoints = 5;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 5;
            static const std::size_t faces[][4]; // first face is a quad, others are triangles
            static bool contains( double coordinates[],
                                  const Point3& pos,
                                  const ValueArray< Vector3 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct LinearPrism
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::PRISM;
            static const std::size_t dimension = 3;
            static const std::size_t numPoints = 6;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 5;
            static const std::size_t faces[][4]; // first two faces are triangles, others are quads
            static bool contains( double coordinates[],
                                  const Point3& pos,
                                  const ValueArray< Vector3 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct LinearHexahedron
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::HEXAHEDRON;
            static const std::size_t dimension = 3;
            static const std::size_t numPoints = 8;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 6;
            static const std::size_t faces[][4];
            static bool contains( double coordinates[],
                                  const Point3& pos,
                                  const ValueArray< Vector3 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct SubparametricQuadraticLine
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::SUBPARAMETRIC_QUADRATIC_LINE;
            static const std::size_t dimension = 1;
            static const std::size_t numPoints = 2;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 2;
            static bool contains( double coordinates[],
                                  const Point1& pos,
                                  const ValueArray< Vector1 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct QuadraticLine
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::QUADRATIC_LINE;
            static const std::size_t dimension = 1;
            static const std::size_t numPoints = 3;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 2;
            static bool contains( double coordinates[],
                                  const Point1& pos,
                                  const ValueArray< Vector1 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };


        struct SubparametricQuadraticTriangle
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::SUBPARAMETRIC_QUADRATIC_TRIANGLE;
            static const std::size_t dimension = 2;
            static const std::size_t numPoints = 3;
            static const std::size_t numValues = numPoints + 3;
            static const std::size_t numFaces = 3;
            static bool contains( double coordinates[],
                                  const Point2& pos,
                                  const ValueArray< Vector2 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct QuadraticTriangle
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::QUADRATIC_TRIANGLE;
            static const std::size_t dimension = 2;
            static const std::size_t numPoints = 6;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 3;
            static bool contains( double coordinates[],
                                  const Point2& pos,
                                  const ValueArray< Vector2 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct SubparametricQuadraticQuad
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::SUBPARAMETRIC_QUADRATIC_QUAD;
            static const std::size_t dimension = 2;
            static const std::size_t numPoints = 4;
            static const std::size_t numValues = numPoints + 4;
            static const std::size_t numFaces = 4;
            static bool contains( double coordinates[],
                                  const Point2& pos,
                                  const ValueArray< Vector2 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct QuadraticQuad
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::QUADRATIC_QUAD;
            static const std::size_t dimension = 2;
            static const std::size_t numPoints = 8;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 4;
            static bool contains( double coordinates[],
                                  const Point2& pos,
                                  const ValueArray< Vector2 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct SubparametricQuadraticTetrahedron
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::SUBPARAMETRIC_QUADRATIC_TETRAHEDRON;
            static const std::size_t dimension = 3;
            static const std::size_t numPoints = 4;
            static const std::size_t numValues = numPoints + 6;
            static const std::size_t numFaces = 4;
            // static const std::size_t faces[][7];
            static bool contains( double coordinates[],
                                  const Point3& pos,
                                  const ValueArray< Vector3 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct QuadraticTetrahedron
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::QUADRATIC_TETRAHEDRON;
            static const std::size_t dimension = 3;
            static const std::size_t numPoints = 10;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 4;
            static const std::size_t faces[][7];
            static bool contains( double coordinates[],
                                  const Point3& pos,
                                  const ValueArray< Vector3 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct SubparametricQuadraticHexahedron
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::QUADRATIC_HEXAHEDRON;
            static const std::size_t dimension = 3;
            static const std::size_t numPoints = 8;
            static const std::size_t numValues = numPoints + 12;
            static const std::size_t numFaces = 6;
            static bool contains( double coordinates[],
                                  const Point3& pos,
                                  const ValueArray< Vector3 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };

        struct QuadraticHexahedron
        {
            static const Cell::Stats stats;
            static const Cell::Type type = Cell::QUADRATIC_HEXAHEDRON;
            static const std::size_t dimension = 3;
            static const std::size_t numPoints = 20;
            static const std::size_t numValues = numPoints;
            static const std::size_t numFaces = 6;
            static bool contains( double coordinates[],
                                  const Point3& pos,
                                  const ValueArray< Vector3 >& points,
                                  const std::size_t indices[] );
            static void interpolate( double coefficients[], const double coordinates[] );
        };
    }
}
