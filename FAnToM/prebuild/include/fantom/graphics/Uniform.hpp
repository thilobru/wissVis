#pragma once


//=========================================================================================================================
// forward declarations
//=========================================================================================================================

namespace fantom
{
    namespace graphics
    {
        namespace detail
        {
            class UniformHandle;
        }

        class ShaderProgram;
    } // namespace graphics
} // namespace fantom


//=========================================================================================================================
// declarations
//=========================================================================================================================

namespace fantom
{
    namespace graphics
    {
        class Uniform
        {
        public:
            virtual ~Uniform();

            template < typename TType >
            TType const& value() const;

            // Method for printing the uniforms content to the given stream
            virtual std::ostream& write( std::ostream& stream ) const = 0;

            // TODO: Find a way to get rid of this public method
            virtual void use( const ShaderProgram& shaderProgram, const std::string& name ) const = 0;
        };


        std::ostream& operator<<( std::ostream& stream, const Uniform& uniform );


        template < typename TType >
        class TypedUniform : public Uniform
        {
        public:
            virtual ~TypedUniform();

            virtual TType const& value() const = 0;
            virtual std::ostream& write( std::ostream& stream ) const override;
        };
    } // namespace graphics
} // namespace fantom


//=========================================================================================================================
// definitions
//=========================================================================================================================

namespace fantom
{
    namespace graphics
    {
        inline Uniform::~Uniform() = default;

        template < typename TType >
        TType const& Uniform::value() const
        {
#ifndef NDEBUG
            if( auto typedUniform = dynamic_cast< const TypedUniform< TType >* >( this ) )
#else
            if( auto typedUniform = static_cast< const TypedUniform< TType >* >( this ) )
#endif
            {
                return typedUniform->value();
            }
            else
            {
                throw fantom::runtime_error( "Invalid uniform type" );
            }
        }


        inline std::ostream& operator<<( std::ostream& stream, const Uniform& uniform )
        {
            return uniform.write( stream );
        }


        template < typename TType >
        TypedUniform< TType >::~TypedUniform() = default;

        template < typename TType >
        std::ostream& TypedUniform< TType >::write( std::ostream& stream ) const
        {
            return stream << value();
        }
    } // namespace graphics
} // namespace fantom
