/**
 * @author: Dusk
 * @date: 2022-03-20 22:50:34
 * @last modified by:   Dusk
 * @last modified time: 2022-03-20 22:50:34
 * @copyright Copyright (c) 2022 Dusk.
*/
#include "Version.h"

namespace
{
    char manual_version[] = NERAL_MANUAL_VERSION;
    char build_type[] = NERAL_BUILD_TYPE;
} // namespace

namespace Neral
{
    const char* const ManualVersion::Get() noexcept
    {
        return manual_version;
    }

    std::uint32_t BuildVersion::Get() noexcept
    {
        return ::Neral::build_version;
    }

    const char* const BuildType::Get() noexcept
    {
        return build_type;
    }

    const char* const BuildDate::Get() noexcept
    {
        return ::Neral::build_date;
    }

    const char* const BuildTime::Get() noexcept
    {
        return ::Neral::build_time;
    }
} // namespace Neral
