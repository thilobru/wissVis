#pragma once

#include <string>
#include <vector>


//=========================================================================================================================
// forward declarations
//=========================================================================================================================

namespace fantom
{
    namespace graphics
    {
        namespace detail
        {
            class ShaderProgramHandle;
            class ShaderProgramModification;
        } // namespace detail

        class RenderState;
    } // namespace graphics
} // namespace fantom


//=========================================================================================================================
// declarations
//=========================================================================================================================

namespace fantom
{
    namespace graphics
    {
        class ShaderProgram
        {
        public:
            virtual ~ShaderProgram();

            virtual std::vector< std::string > const& attributeNames() const = 0;
            virtual std::vector< std::string > const& uniformNames() const = 0;

        private:
            virtual void use( RenderState const& state ) const = 0;
            virtual void reuse( RenderState const& state ) const = 0;
            virtual void unuse( RenderState const& state ) const = 0;

            friend class detail::ShaderProgramModification;
        };
    } // namespace graphics
} // namespace fantom
