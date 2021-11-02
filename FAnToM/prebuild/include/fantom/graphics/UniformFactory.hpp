#pragma once

#include <fantom/math.hpp>

#include <memory>


namespace fantom
{
    namespace graphics
    {
        class Uniform;
    }
} // namespace fantom


namespace fantom
{
    namespace graphics
    {
        class UniformFactory
        {
        public:
            virtual ~UniformFactory();

            virtual std::unique_ptr< Uniform > create( int a ) const = 0;
            virtual std::unique_ptr< Uniform > create( int a, int b ) const = 0;
            virtual std::unique_ptr< Uniform > create( int a, int b, int c ) const = 0;
            virtual std::unique_ptr< Uniform > create( int a, int b, int c, int d ) const = 0;

            virtual std::unique_ptr< Uniform > create( float a ) const = 0;
            virtual std::unique_ptr< Uniform > create( float a, float b ) const = 0;
            virtual std::unique_ptr< Uniform > create( float a, float b, float c ) const = 0;
            virtual std::unique_ptr< Uniform > create( float a, float b, float c, float d ) const = 0;

            virtual std::unique_ptr< Uniform > create( bool a ) const = 0;
            virtual std::unique_ptr< Uniform > create( bool a, bool b ) const = 0;
            virtual std::unique_ptr< Uniform > create( bool a, bool b, bool c ) const = 0;
            virtual std::unique_ptr< Uniform > create( bool a, bool b, bool c, bool d ) const = 0;

            virtual std::unique_ptr< Uniform > create( const MatrixF< 2 >& matrix ) const = 0;
            virtual std::unique_ptr< Uniform > create( const MatrixF< 3 >& matrix ) const = 0;
            virtual std::unique_ptr< Uniform > create( const MatrixF< 4 >& matrix ) const = 0;

            virtual std::unique_ptr< Uniform > create( const VectorF< 2 >& vector ) const = 0;
            virtual std::unique_ptr< Uniform > create( const VectorF< 3 >& vector ) const = 0;
            virtual std::unique_ptr< Uniform > create( const VectorF< 4 >& vector ) const = 0;
            virtual std::unique_ptr< Uniform > create( const Color& color ) const = 0;
        };
    } // namespace graphics
} // namespace fantom
