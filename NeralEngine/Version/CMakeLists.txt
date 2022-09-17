#set relative project name
set(RELATIVE_PROJECT_NAME "Neral")
#config BuildVersion.txt
set(BUILD_VERSION_FILE_NAME "BuildVersion.txt")
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_VERSION_FILE_NAME}")
    message(STATUS "${BUILD_VERSION_FILE_NAME} not found. CMake will create it.")
    file(WRITE ./${BUILD_VERSION_FILE_NAME} "0")
    file(WRITE ./VersionHelper.cpp
"#include \"VersionHelper.h\"

namespace ${RELATIVE_PROJECT_NAME}
{
    const std::uint32_t build_version{0};
    const char* const build_time = __TIME__;
    const char* const build_date = __DATE__;
}
"
        )
endif()
#config IncrementBuildVersion.cmake
set(SCRIPT_NAME "IncreaseBuildVersion.cmake")
set(BUILD_VERSION_INCREASE_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/${SCRIPT_NAME}")
if(NOT EXISTS ${BUILD_VERSION_INCREASE_SCRIPT})
    message(STATUS "${SCRIPT_NAME} not found. CMake will create it.")
    file(WRITE ${BUILD_VERSION_INCREASE_SCRIPT}
"file(READ \$\{CMAKE_CURRENT_SOURCE_DIR\}/${BUILD_VERSION_FILE_NAME} BUILD_VERSION)
MATH(EXPR BUILD_VERSION \"\$\{BUILD_VERSION\}+1\")
file(WRITE \$\{CMAKE_CURRENT_SOURCE_DIR\}/${BUILD_VERSION_FILE_NAME} \"\$\{BUILD_VERSION\}\")
file(WRITE \$\{CMAKE_CURRENT_SOURCE_DIR\}/VersionHelper.cpp
\"#include \\\"VersionHelper.h\\\"

namespace \$\{RELATIVE_PROJECT_NAME\}
{
    const std::uint32_t build_version{\$\{BUILD_VERSION\}};
    const char* const build_time = __TIME__;
    const char* const build_date = __DATE__;
}
\"
    )
"
        )
endif()
#show current build version
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_VERSION_FILE_NAME} BUILD_VERSION)
message(STATUS "Current build version is ${BUILD_VERSION}")
#config build version auto incerse
add_custom_target("BuildVersionIncreaser"
    COMMAND cmake -P "${BUILD_VERSION_INCREASE_SCRIPT}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/")
add_dependencies(${MAIN_PROJECT_NAME} "BuildVersionIncreaser")
#config lib
aux_source_directory(. VERSION_SOURCE)
set(LIB_NAME "${RELATIVE_PROJECT_NAME}Version")
add_library(${LIB_NAME} STATIC EXCLUDE_FROM_ALL ${VERSION_SOURCE})
target_compile_definitions(${LIB_NAME} PRIVATE NERAL_BUILD_TYPE="${CMAKE_BUILD_TYPE}")
target_compile_definitions(${LIB_NAME} PRIVATE NERAL_MANUAL_VERSION="${MAIN_PROJECT_VERSION}")
append_lib_to_global_property(GLOBAL_DEPENDENT_LIBS "${LIB_NAME}")
add_dependencies(${MAIN_PROJECT_NAME} ${LIB_NAME})
#config test
config_test_source(TEST_SOURCE)
aux_source_directory(./Test TEST_SOURCE)
create_gtest_for_lib("VersionTest" "${TEST_SOURCE}" "${LIB_NAME}")
