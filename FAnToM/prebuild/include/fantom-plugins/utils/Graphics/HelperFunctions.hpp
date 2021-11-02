#pragma once

#include <fantom/graphics.hpp>
#include <fantom/math.hpp>

#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace fantom
{

    namespace graphics
    {
        // 3D Ritter Algorithm for the computation of the bounding sphere of a given point set

        /// This function computes a suitable bounding sphere for an array of points \ingroup gfx
        /**
         * \param points the points inside the bounding sphere
         */
        BoundingSphere computeBoundingSphere( const std::vector< Tensor< float, 3 > >& points );

        /// This function computes a suitable bounding sphere for an array of spheres\ingroup gfx
        /**
         * \param points the points inside the bounding sphere
         * \param pointRadii the radii of the points
         */
        BoundingSphere computeBoundingSphere( const std::vector< Tensor< float, 3 > >& points,
                                              const std::vector< float >& pointRadii );

        /// This function computes the vertex normals of a triangular mesh \ingroup gfx
        /**
         * \param points the vertex positions of the mesh
         * \param indices the indices of the triangular mesh
         */
        std::vector< Tensor< float, 3 > > computeNormals( const std::vector< PointF< 3 > >& points,
                                                          const std::vector< unsigned int >& indices );
    } // namespace graphics
} // namespace fantom
