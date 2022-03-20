/**
 * @author: Dusk
 * @date: 2022-03-20 15:23:53
 * @last modified by:   Dusk
 * @last modified time: 2022-03-20 15:23:53
 * @copyright Copyright (c) 2022 Dusk.
 */
#ifndef NERAL_ENGINE_VERSION_VERSION_H
#define NERAL_ENGINE_VERSION_VERSION_H
#include <cstdint>
#include "VersionHelper.h"

namespace Neral
{
    class ManualVersion
    {
    public:
        static const char* const Get() noexcept;
    };
    class BuildVersion
    {
    public:
        static std::uint32_t Get() noexcept;
    };
    class BuildType
    {
    public:
        static const char* const Get() noexcept;
    };
    class BuildDate
    {
    public:
        static const char* const Get() noexcept;
    };
    class BuildTime
    {
    public:
        static const char* const Get() noexcept;
    };
} // namespace Neral

#endif // NERAL_ENGINE_VERSION_VERSION_H
