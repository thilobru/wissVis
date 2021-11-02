#pragma once

#include "../graphics.hpp"
#include "OptionInterface.hpp"
#include "UniformFactory.hpp"


namespace fantom
{
    namespace graphics
    {
        /**
         * Base class for FAnToM's graphics system.
         * This class follows the singleton design pattern. Only one instance can be existent.
         * This instance can be obtained with \c GraphicsSystem::instance() .
         */
        class GraphicsSystem
        {
            static auto getBufferType( float ) -> std::integral_constant< BufferType, BufferType::FLOAT >;
            static auto getBufferType( int32_t ) -> std::integral_constant< BufferType, BufferType::INT >;
            static auto getBufferType( uint32_t ) -> std::integral_constant< BufferType, BufferType::UNSIGNED_INT >;
            static auto getBufferType( int16_t ) -> std::integral_constant< BufferType, BufferType::SHORT >;
            static auto getBufferType( uint16_t ) -> std::integral_constant< BufferType, BufferType::UNSIGNED_SHORT >;
            static auto getBufferType( int8_t ) -> std::integral_constant< BufferType, BufferType::BYTE >;
            static auto getBufferType( uint8_t ) -> std::integral_constant< BufferType, BufferType::UNSIGNED_BYTE >;
            static auto getBufferType( ... ) -> void;
            template < typename T >
            using bufferType_t = decltype( getBufferType( std::declval< T >() ) );

        public:
            static GraphicsSystem const& instance()
            {
                return *mInstance;
            }

            virtual ~GraphicsSystem();

            template < typename T >
            std::shared_ptr< VertexBuffer > makeBuffer( std::vector< T > const& scalars ) const;

            template < typename T, size_t comps >
            std::shared_ptr< VertexBuffer > makeBuffer( std::vector< Tensor< T, comps > > const& vectors ) const;

            template < typename T, size_t comps >
            std::shared_ptr< VertexBuffer >
            makeBuffer( std::vector< Tensor< T, comps, comps > > const& matrices ) const;

            std::shared_ptr< VertexBuffer > makeBuffer( std::vector< Color > const& colors,
                                                        ColorChannel channel = ColorChannel::RGBA ) const;

            virtual std::shared_ptr< IndexBuffer > makeIndexBuffer( std::vector< uint32_t > const& indices ) const = 0;

            virtual std::shared_ptr< graphics::FrameBuffer > makeFrameBuffer( Size2D size ) const = 0;

            // Programs
            virtual std::shared_ptr< graphics::ShaderProgram >
            makeProgramFromFiles( std::string const& vertexPath, std::string const& fragmentPath ) const = 0;

            virtual std::shared_ptr< graphics::ShaderProgram >
            makeProgramFromFiles( std::string const& vertexPath,
                                  std::string const& fragmentPath,
                                  std::string const& geometryPath ) const = 0;

            virtual std::shared_ptr< graphics::ShaderProgram >
            makeProgramFromSource( std::string const& vertex, std::string const& fragment ) const = 0;

            virtual std::shared_ptr< graphics::ShaderProgram > makeProgramFromSource(
                std::string const& vertex, std::string const& fragment, std::string const& geometry ) const = 0;


            // Primitives
            /**
             * \brief Draw primitive geometry.
             *
             * Being the most fundamental drawing facility in the graphics engine,
             * the primitive draws a set of similar objects using a Shader program.
             *
             * The shader program can optionally make use of some pre-defined
             * uniforms:
             *  - mat4 model               Model matrix
             *  - mat4 model_inv           Inverse model matrix
             *  - mat4 view                View matrix
             *  - mat4 view_inv            Inverse view matrix
             *  - mat4 proj                Projection matrix
             *  - mat4 proj_inv            Inverse projection matrix
             *  - bool perspective_proj    True if the camera uses perspective projection
             *  - int screenWidth
             *  - int screenHeight
             *
             * While fragment shaders can have multiple color outputs, the default color output must be called
             * "out_color".
             *
             * It is your responsibility to respect the clipping planes.
             * For this, declare a uniform
             *     uniform vec4 clippingPlane[6]
             * and an out parameter
             *     out float gl_ClipDistance[6]
             * in your vertex shader or geometry shader, if present.
             * For every vertex (i.e., simply in the vertex shader, or for every out vertex in the geometry shader) set
             the corresponding gl_ClipDistanceS using the following code snippet:
             * \code{.glsl}
                 for( int i=0; i<6; ++i )
                     gl_ClipDistance[i] = dot( positionInWorldCoords, clippingPlane[6] );
             * \endcode
             */
            virtual std::shared_ptr< graphics::Drawable >
            makePrimitive( PrimitiveConfig config,
                           std::shared_ptr< graphics::ShaderProgram > const& program ) const = 0;


            // Textures
            virtual std::shared_ptr< graphics::Texture1D >
            makeTexture( size_t w, ColorChannel channel = ColorChannel::RGBA ) const = 0;
            virtual std::shared_ptr< graphics::Texture2D >
            makeTexture( Size2D size, ColorChannel channel = ColorChannel::RGBA ) const = 0;
            virtual std::shared_ptr< graphics::Texture3D >
            makeTexture( Size3D size, ColorChannel channel = ColorChannel::RGBA ) const = 0;

            virtual std::shared_ptr< graphics::Texture2D >
            makeTextureFromFile( const std::string& path, ColorChannel channel = ColorChannel::RGBA ) const;

            // Access to the factory that generates implementation-specific uniforms
            virtual const UniformFactory& uniform() const = 0;

            // Access to the option interface to set render options
            virtual const OptionInterface& options() const = 0;

            /// Indicates if vertex-, geometry- and fragment shader will be manipulated to calculate transparency
            /// using a Depth-Peeling algorithm.
            // Todo: Make mUseDepthPeeling private
            static bool mUseDepthPeeling;

        protected:
            GraphicsSystem();

        private:
            virtual std::shared_ptr< graphics::VertexBuffer >
            makeVertexBuffer( BufferType type, size_t numComponents, void const* data, size_t vertexCount ) const = 0;

            static GraphicsSystem* mInstance;
        };


        // --------------------------------------------------------------------------------


        template < typename T >
        std::shared_ptr< VertexBuffer > GraphicsSystem::makeBuffer( std::vector< T > const& scalars ) const
        {
            static_assert( !std::is_same< void, bufferType_t< T > >{},
                           "Data type not usable for buffer definitions. Use one of float, int32_t, int16_t, int8_t, "
                           "uint32_t, uint16_t, or uint8_t." );
            return makeVertexBuffer( bufferType_t< T >{}, 1, scalars.data(), scalars.size() );
        }

        template < typename T, size_t comps >
        std::shared_ptr< VertexBuffer >
        GraphicsSystem::makeBuffer( std::vector< Tensor< T, comps > > const& vectors ) const
        {
            static_assert( comps >= 1 && comps <= 4, "Only vectors of size 1 to 4 are allowed." );
            static_assert( !std::is_same< void, bufferType_t< T > >{},
                           "Data type not usable for buffer definitions. Use one of float, int32_t, int16_t, int8_t, "
                           "uint32_t, uint16_t, or uint8_t." );
            return makeVertexBuffer( bufferType_t< T >{}, comps, vectors.data(), vectors.size() );
        }

        template < typename T, size_t comps >
        std::shared_ptr< VertexBuffer >
        GraphicsSystem::makeBuffer( std::vector< Tensor< T, comps, comps > > const& matrices ) const
        {
            static_assert( comps >= 2 && comps <= 4, "Only matrices of size 2 to 4 are allowed." );
            static_assert( !std::is_same< void, bufferType_t< T > >{},
                           "Data type not usable for buffer definitions. Use one of float, int32_t, int16_t, int8_t, "
                           "uint32_t, uint16_t, or uint8_t." );
            return makeVertexBuffer( bufferType_t< T >{}, comps * comps, matrices.data(), matrices.size() );
        }
    } // namespace graphics
} // namespace fantom
