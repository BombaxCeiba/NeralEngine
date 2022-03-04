/**
 * @author: Dusk
 * @date: 2021-11-30 19:18:34
 * @last modified by:   Dusk
 * @last modified time: 2022-03-04 19:15:37
 * @copyright Copyright (c) 2022 Dusk.
 */
#ifndef NERAL_ENGINE_COMMON_RENDERBASE_H
#define NERAL_ENGINE_COMMON_RENDERBASE_H

#include <memory>

class RenderBase
{
    friend std::shared_ptr<RenderBase>;

public:
    RenderBase() = default;

protected:
    ~RenderBase() = default;
};
#endif // NERAL_ENGINE_COMMON_RENDERBASE_H
