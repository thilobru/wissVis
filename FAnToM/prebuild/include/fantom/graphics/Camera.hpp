#pragma once

#include "BoundingSphere.hpp"
#include "common.hpp"

#include "../math.hpp"

#include <memory>

namespace fantom
{
    namespace graphics
    {
        class Camera
        {
        public:
            enum class Type
            {
                ORTHOGRAPHIC,
                PERSPECTIVE,
                CUSTOM
            };

            virtual ~Camera();

            virtual Type type() const = 0;

            virtual void position( Tensor< float, 3 > const& position ) = 0;
            virtual Tensor< float, 3 > position() const = 0;

            virtual void orientation( Quaternion< float > const& orientation ) = 0;
            virtual Quaternion< float > orientation() const = 0;

            virtual void focusDistance( float distance ) = 0;
            virtual float focusDistance() const = 0;

            virtual void fieldOfViewY( float fieldOfViewY ) = 0;
            virtual float fieldOfViewY() const = 0;

            virtual Tensor< float, 4, 4 > projectionMatrix( Size2D targetSize,
                                                            BoundingSphere const& boundingSphere ) const = 0;

            virtual Tensor< float, 4, 4 > viewMatrix() const = 0;

            virtual std::unique_ptr< Camera > clone() const = 0;

            //-------------------------------------------------------------------------------------

            /// Computes the focus point, which is "focusDistance" away in view direction from the
            /// camera position
            Tensor< float, 3 > focusPoint() const;

            /// Compute the direction the camera is looking in
            Tensor< float, 3 > viewDirection() const;

            /// Compute the direction pointing upwards
            Tensor< float, 3 > upDirection() const;

            void
            lookAt( Tensor< float, 3 > const& eye, Tensor< float, 3 > const& center, Tensor< float, 3 > const& up );
        };


        class OrthographicCamera : public Camera
        {
        public:
            OrthographicCamera();

            OrthographicCamera( Camera const& camera );

            virtual Type type() const override;

            virtual void position( Tensor< float, 3 > const& position ) override;
            virtual Tensor< float, 3 > position() const override;

            virtual void orientation( Quaternion< float > const& orientation ) override;
            virtual Quaternion< float > orientation() const override;

            virtual void focusDistance( float distance ) override;
            virtual float focusDistance() const override;

            virtual void fieldOfViewY( float fieldOfViewY ) override;
            virtual float fieldOfViewY() const override;

            virtual Tensor< float, 4, 4 > projectionMatrix( Size2D targetSize,
                                                            BoundingSphere const& boundingSphere ) const override;

            virtual Tensor< float, 4, 4 > viewMatrix() const override;

            virtual std::unique_ptr< Camera > clone() const override;

        private:
            Tensor< float, 3 > mPosition;
            Quaternion< float > mOrientation;
            float mFocusDistance; // Distance from position to focus point
            float mFieldOfViewY;
        };


        class PerspectiveCamera : public Camera
        {
        public:
            PerspectiveCamera();

            PerspectiveCamera( Camera const& camera );

            virtual Type type() const override;

            virtual void position( Tensor< float, 3 > const& position ) override;
            virtual Tensor< float, 3 > position() const override;

            virtual void orientation( Quaternion< float > const& orientation ) override;
            virtual Quaternion< float > orientation() const override;

            virtual void focusDistance( float distance ) override;
            virtual float focusDistance() const override;

            virtual void fieldOfViewY( float fieldOfViewY ) override;
            virtual float fieldOfViewY() const override;

            virtual Tensor< float, 4, 4 > projectionMatrix( Size2D targetSize,
                                                            BoundingSphere const& boundingSphere ) const override;

            virtual Tensor< float, 4, 4 > viewMatrix() const override;

            virtual std::unique_ptr< Camera > clone() const override;

        private:
            Tensor< float, 3 > mPosition;
            Quaternion< float > mOrientation;
            float mFocusDistance; // Distance from position to focus point
            float mFieldOfViewY;
        };
    }
}
