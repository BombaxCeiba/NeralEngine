#pragma once
//***************************************************************************************
//Camera.h by Dusk_NM02 (c) 2021 All Rights Reserved.
//***************************************************************************************
#include <array>
#include "Vectors.hpp"
class Camera
{
public:
	Camera();
	~Camera();

private:
    Matrix4fAlignas16 camera_;
};