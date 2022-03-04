/**
 * @author: Dusk
 * @date: 2022-03-04 19:20:25
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:20:25
 * @copyright Copyright (c) 2022 Dusk.
 */
#ifndef NERAL_ENGINE_COMMON_CAMERA_H
#define NERAL_ENGINE_COMMON_CAMERA_H

#include <array>
#include "Vectors.hpp"
namespace Neral
{
    class Camera
    {
    public:
        Camera();
        ~Camera();

    private:
        Matrix4fAlignas16 camera_;
    };
}
#endif // NERAL_ENGINE_COMMON_CAMERA_H
