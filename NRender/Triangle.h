#pragma once
//***************************************************************************************
//Triangle.h by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
#include"Vectors.hpp"

template<size_t align = alignof(std::max_align_t)>
struct alignas(align) Triangle
{
    Vector4f<align> position[3];
    Vector2f<align> texture_uv[3];
    Vector3f<align> normal[3];
    Vector3f<align> color[3];
};