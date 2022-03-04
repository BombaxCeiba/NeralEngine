/**
 * @author: Dusk
 * @date: 2022-03-04 19:14:15
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:14:15
 * @copyright Copyright (c) 2022 Dusk.
*/
#ifndef NERAL_ENGINE_COMMON_TRIANGLE_H
#define NERAL_ENGINE_COMMON_TRIANGLE_H

#include <array>
#include "Vectors.hpp"

template <size_t align = alignof(std::max_align_t)>
struct alignas(align) Triangle
{
    std::array<Vector4f<align>, 3> position;
    std::array<Vector2f<align>, 3> texture_uv;
    std::array<Vector3f<align>, 3> normal;
    std::array<Vector3f<align>, 3> color;
};
#endif // NERAL_ENGINE_COMMON_TRIANGLE_H
