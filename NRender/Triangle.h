#pragma once
//***************************************************************************************
//Triangle.h by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
#include<array>
#include"Vectors.hpp"

template<size_t align = alignof(std::max_align_t)>
struct alignas(align) Triangle
{
    std::array<Vector4f<align>, 3> position;
    std::array<Vector2f<align>, 3> texture_uv;
    std::array<Vector3f<align>, 3> normal;
    std::array<Vector3f<align>, 3> color;
};