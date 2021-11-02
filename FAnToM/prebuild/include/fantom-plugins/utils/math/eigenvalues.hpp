#pragma once

#include <array>
#include <complex>
#include <vector>

#include <Eigen/Core>
#include <Eigen/Eigenvalues>

#include <fantom/math.hpp>

namespace fantom
{
    namespace math
    {
        namespace math_private
        {
            /**
             * Converts a FAnToM matrix to an Eigen3 matrix
             */
            template < size_t D >
            Eigen::Matrix< double, D, D > convertMatrix( const Tensor< double, D, D >& matrix )
            {
                Eigen::Matrix< double, D, D > out;
                for( size_t i = 0; i < D; i++ )
                    for( size_t j = 0; j < D; j++ )
                        out( i, j ) = matrix( i, j );

                return out;
            }
            /**
             * Converts an Eigen3 matrix to a FAnToM matrix
             */
            template < size_t D >
            Tensor< std::complex< double >, D, D >
            convertMatrix( const Eigen::Matrix< std::complex< double >, D, D >& matrix )
            {
                Tensor< std::complex< double >, D, D > out;
                for( size_t i = 0; i < D; i++ )
                    for( size_t j = 0; j < D; j++ )
                        out( i, j ) = matrix( i, j );

                return out;
            }
            /**
             * Converts an Eigen3 matrix to a FAnToM matrix
             */
            template < size_t D >
            Tensor< double, D, D > convertMatrix( const Eigen::Matrix< double, D, D >& matrix )
            {
                Tensor< double, D, D > out;
                for( size_t i = 0; i < D; i++ )
                    for( size_t j = 0; j < D; j++ )
                        out( i, j ) = matrix( i, j );

                return out;
            }
            /**
             * Convert a complex valued vector from Eigen3 format to fantom format
             */
            template < size_t D >
            Tensor< std::complex< double >, D >
            convertComplexVector( const Eigen::Matrix< std::complex< double >, D, 1 >& vector )
            {
                Tensor< std::complex< double >, D > vec;
                for( size_t i = 0; i < D; i++ )
                    vec( i ) = vector( i );
                return vec;
            }
        }

        template < size_t Dim >
        struct RealFirstDecreasingNorm
        {
            const Tensor< std::complex< double >, Dim >* eigenvalues;
            bool operator()( size_t v1, size_t v2 ) const
            {
                return ( *this )( ( *eigenvalues )( v1 ), ( *eigenvalues )( v2 ) );
            }

            bool operator()( const std::complex< double >& v1, const std::complex< double >& v2 ) const
            {
                if( v1.imag() != 0 && v2.imag() == 0 )
                    return false; // prefer second one
                else if( v1.imag() == 0 && v2.imag() != 0 )
                    return true; // prefer first one
                else
                    return std::abs( v1 ) > std::abs( v2 );
            }
        };


        /**
         * Calculation of the Eigenvalues of a given symmetric matrix.
         *
         * Eigen3 is used to calculate the eigenvalues of the symmetric matrix m.
         * The zeroThreshold is used to cut off very small values in order to improve numeric stability.
         * They are sorted in ascending order by their corresponding value.
         *
         * Note: to use this function you have to link your toolbox to the Eigen3 library!
         */
        template < size_t Dim >
        std::array< double, Dim > getEigenvaluesSymmetric( const Tensor< double, Dim, Dim >& m,
                                                           double zeroThreshold = 1e-9 )
        {
            Eigen::Matrix< double, Dim, Dim > matrix( math_private::convertMatrix( m ) );
            Eigen::SelfAdjointEigenSolver< Eigen::Matrix< double, Dim, Dim > > solver( matrix );
            Eigen::Matrix< double, Dim, 1 > eigenvalues = solver.eigenvalues();

            // MAKE VERY SMALL VALUES ZERO
            for( size_t i = 0; i < Dim; ++i )
            {
                if( std::abs( eigenvalues( i ) ) < zeroThreshold )
                    eigenvalues( i ) = 0.0;
            }
            // push values to out
            std::array< double, Dim > out;
            for( size_t k = 0; k < Dim; k++ )
            {
                out[k] = eigenvalues[k];
            }
            return out;
        }

        /**
         * Calculation of the eigensystem of a given symmetric matrix.
         *
         * Eigen3 is used to calculate the eigenvalues of the symmetric matrix m.
         * The zeroThreshold is used to cut off very small values in order to improve numeric stability.
         * They are sorted in ascending order by their corresponding value.
         * Eigenvectors are normalized.
         * This function returns a pair of lists, one containing the eigenvalues and one containing the eigenvectors.
         * Corresponding eigenvalues and eigenvectors have the same index in both lists.
         *
         * Note: to use this function you have to link your toolbox to the Eigen3 library!
         */
        template < size_t Dim >
        std::pair< std::array< double, Dim >, std::array< Tensor< double, Dim >, Dim > >
        getEigensystemSymmetric( const Tensor< double, Dim, Dim >& m, double zeroThreshold = 1e-9 )
        {
            Eigen::Matrix< double, Dim, Dim > matrix( math_private::convertMatrix( m ) );
            Eigen::SelfAdjointEigenSolver< Eigen::Matrix< double, Dim, Dim > > solver( matrix );
            Eigen::Matrix< double, Dim, Dim > eigenvectors = solver.eigenvectors();
            Eigen::Matrix< double, Dim, 1 > eigenvalues = solver.eigenvalues();

            // MAKE VERY SMALL VALUES ZERO
            for( size_t i = 0; i < Dim; ++i )
            {
                if( std::abs( eigenvalues( i ) ) < zeroThreshold )
                    eigenvalues( i ) = 0.0;
            }
            // push values to out
            std::pair< std::array< double, Dim >, std::array< Tensor< double, Dim >, Dim > > out
                = make_pair( std::array< double, Dim >(), std::array< Tensor< double, Dim >, Dim >() );
            for( size_t k = 0; k < Dim; k++ )
            {
                out.first[k] = eigenvalues[k];
                for( size_t i = 0; i < Dim; i++ )
                    out.second[k]( i ) = eigenvectors( i, k ); // k-th column is index-th eigenvector
            }
            return out;
        }

        /**
         * Calculation of the eigenvalues of a given matrix.
         *
         * Eigen3 is used to calculate the (possibly complex) eigenvalues of the matrix m.
         * The zeroThreshold is used to cut off very small values in order to improve numeric stability.
         * They are sorted in ascending order by their corresponding norm, whereby real eigenvalues are always sorted in
         * front of the complex ones.
         *
         * Note: to use this function you have to link your toolbox to the Eigen3 library!
         */
        template < size_t Dim >
        std::array< std::complex< double >, Dim > getEigenvalues( const Tensor< double, Dim, Dim >& m,
                                                                  double zeroThreshold = 1e-9 )
        {
            Eigen::Matrix< double, Dim, Dim > matrix( math_private::convertMatrix( m ) );
            Eigen::Matrix< std::complex< double >, Dim, 1 > eigenEigenvalues = matrix.eigenvalues();
            Tensor< std::complex< double >, Dim > eigenvalues
                = math_private::convertComplexVector< Dim >( eigenEigenvalues );
            std::array< std::complex< double >, Dim > out;
            std::cout << "Using matrix " << m << std::endl;
            std::cout << "which became " << matrix << std::endl;
            std::cout << "Got eigenvalues from eigen: " << eigenEigenvalues << std::endl;

            // MAKE VERY SMALL VALUES ZERO
            for( size_t i = 0; i < Dim; ++i )
            {
                if( std::abs( eigenvalues( i ).imag() ) < zeroThreshold )
                    eigenvalues( i ) = std::complex< double >( eigenvalues( i ).real(), 0.0 );
                if( std::abs( eigenvalues[i].real() ) < zeroThreshold )
                    eigenvalues( i ) = std::complex< double >( 0.0, eigenvalues( i ).imag() );
                out[i] = eigenvalues( i );
            }

            // sort values: real first and ordered by norm (ascending)
            sort( out.begin(), out.end(), RealFirstDecreasingNorm< Dim >{ &eigenvalues } );
            return out;
        }

        /**
         * Calculation of the eigensystem of a given matrix.
         *
         * Eigen3 is used to calculate the (possibly complex) eigenvalues of the matrix m.
         * The zeroThreshold is used to cut off very small values in order to improve numeric stability.
         * They are sorted in ascending order by their corresponding norm, whereby real eigenvalues are always sorted in
         * front of the complex ones.
         * Eigenvectors are normalized.
         * This function returns a pair of lists, one containing the eigenvalues and one containing the eigenvectors.
         * Corresponding eigenvalues and eigenvectors have the same index in both lists.
         * Note however that the returned eigenvectors are in general complex!
         *
         * Note: to use this function you have to link your toolbox to the Eigen3 library!
         */
        template < size_t Dim >
        std::pair< std::vector< std::complex< double > >, std::vector< Tensor< std::complex< double >, Dim > > >
        getEigensystem( const Tensor< double, Dim, Dim >& m, double zeroThreshold = 1e-9 )
        {
            Eigen::Matrix< double, Dim, Dim > matrix( math_private::convertMatrix( m ) );
            Eigen::EigenSolver< Eigen::Matrix< double, Dim, Dim > > solver( matrix );
            Eigen::Matrix< std::complex< double >, Dim, Dim > eigenEigenvectors = solver.eigenvectors();
            Eigen::Matrix< std::complex< double >, Dim, 1 > eigenEigenvalues = solver.eigenvalues();
            Tensor< std::complex< double >, Dim > eigenvalues
                = math_private::convertComplexVector< Dim >( eigenEigenvalues );
            Tensor< std::complex< double >, Dim, Dim > eigenvectors
                = math_private::convertMatrix< Dim >( eigenEigenvectors );

            // MAKE VERY SMALL VALUES ZERO
            for( size_t i = 0; i < Dim; ++i )
            {
                if( std::abs( eigenvalues( i ).imag() ) < zeroThreshold )
                    eigenvalues( i ) = std::complex< double >( eigenvalues( i ).real(), 0.0 );
                if( std::abs( eigenvalues[i].real() ) < zeroThreshold )
                    eigenvalues( i ) = std::complex< double >( 0.0, eigenvalues( i ).imag() );
            }
            // sort values: real first and ordered by norm (ascending)
            std::vector< size_t > indices;
            for( size_t i = 0; i < Dim; ++i )
                indices.push_back( i );
            sort( indices.begin(), indices.end(), RealFirstDecreasingNorm< Dim >{ &eigenvalues } );
            // push values to out
            std::pair< std::vector< std::complex< double > >, std::vector< Tensor< std::complex< double >, Dim > > > out
                = make_pair( std::vector< std::complex< double > >(),
                             std::vector< Tensor< std::complex< double >, Dim > >() );
            for( size_t k = 0; k < Dim; k++ )
            {
                size_t index = indices[k];
                out.first.push_back( eigenvalues( index ) );
                Tensor< std::complex< double >, Dim > vec;
                for( size_t i = 0; i < Dim; i++ )
                    vec( i ) = eigenvectors( i, index ); // index-th column is index-th eigenvector
                out.second.push_back( vec );
            }
            return out;
        }
        /**
         * Calculation of the eigensystem of a given matrix.
         *
         * See fantom::math::getEigensystem for further information.
         * This function differs in so far, that it uses pseudo eigenvectors instead of ordinary eigenvectors and thus
         * ensuring to get real valued eigenvectors.
         *
         * Note: to use this function you have to link your toolbox to the Eigen3 library!
         */
        template < size_t Dim >
        std::pair< std::vector< std::complex< double > >, std::vector< Tensor< double, Dim > > >
        getPseudoEigensystem( const Tensor< double, Dim, Dim >& m, double zeroThreshold = 1e-9 )
        {
            Eigen::Matrix< double, Dim, Dim > matrix( math_private::convertMatrix( m ) );
            Eigen::EigenSolver< Eigen::Matrix< double, Dim, Dim > > solver( matrix );
            Eigen::Matrix< double, Dim, Dim > eigenEigenvectors = solver.pseudoEigenvectors();
            Eigen::Matrix< std::complex< double >, Dim, 1 > eigenEigenvalues = solver.eigenvalues();
            Tensor< std::complex< double >, Dim > eigenvalues
                = math_private::convertComplexVector< Dim >( eigenEigenvalues );
            Tensor< double, Dim, Dim > eigenvectors = math_private::convertMatrix< Dim >( eigenEigenvectors );

            // MAKE VERY SMALL VALUES ZERO
            for( size_t i = 0; i < Dim; ++i )
            {
                if( std::abs( eigenvalues( i ).imag() ) < zeroThreshold )
                    eigenvalues( i ) = std::complex< double >( eigenvalues( i ).real(), 0.0 );
                if( std::abs( eigenvalues[i].real() ) < zeroThreshold )
                    eigenvalues( i ) = std::complex< double >( 0.0, eigenvalues( i ).imag() );
            }
            // sort values: real first and ordered by norm (ascending)
            std::vector< size_t > indices;
            for( size_t i = 0; i < Dim; ++i )
                indices.push_back( i );
            sort( indices.begin(), indices.end(), RealFirstDecreasingNorm< Dim >{ &eigenvalues } );
            // push values to out
            std::pair< std::vector< std::complex< double > >, std::vector< Tensor< double, Dim > > > out
                = make_pair( std::vector< std::complex< double > >(), std::vector< Tensor< double, Dim > >() );
            for( size_t k = 0; k < Dim; k++ )
            {
                size_t index = indices[k];
                out.first.push_back( eigenvalues( index ) );
                Tensor< double, Dim > vec;
                for( size_t i = 0; i < Dim; i++ )
                    vec( i ) = eigenvectors( i, index ); // index-th column is index-th eigenvector
                out.second.push_back( vec );
            }
            return out;
        }

        /**
         * returns the spectral norm aka. matrix operator norm
         *
         * It is the square root of the largest Eigenvalue of the product
         * of the self adjoint of A and A ( = A^* A)
         */
        template< class T, std::size_t M, std::size_t N >
        decltype( norm2( T() ) ) matrix2Norm( const Tensor< T, M, N >& A )
        {
            auto mat = convertMatrix( A );

            return mat.operatorNorm();
        }

    }
}
