file(READ ${CMAKE_CURRENT_SOURCE_DIR}/BuildVersion.txt BUILD_VERSION)
MATH(EXPR BUILD_VERSION "${BUILD_VERSION}+1")
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/BuildVersion.txt "${BUILD_VERSION}")
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/VersionHelper.cpp
"#include \"VersionHelper.h\"

namespace Neral
{
    const std::uint32_t build_version{${BUILD_VERSION}};
    const char* const build_time = __TIME__;
    const char* const build_date = __DATE__;
}
"
    )
