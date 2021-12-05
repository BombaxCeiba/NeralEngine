/***
 * @Author: Dusk
 * @Date: 2021-11-30 19:18:34
 * @FilePath: \NRender\NRender\Common\include\RenderBase.h
 * @Copyright (c) 2021 Dusk. All rights reserved.
 */
#pragma once
#include<memory>

class RenderBase
{
    friend std::shared_ptr<RenderBase>;
public:
    RenderBase() = default;
protected:
    ~RenderBase() = default;
};