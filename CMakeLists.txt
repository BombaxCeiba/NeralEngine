cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME "NRender")

project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 20)

include(CheckCXXSourceRuns)
check_cxx_source_runs("#include<immintrin.h>
  int main()
  {
    static const __m128 test_var = _mm_set_ps(-0.375, 0.125, 0.125, 0.375);

    return 0;
  }" HAVE_SSE_SUPPORT)
if(HAVE_SSE_SUPPORT MATCHES 0)
    message(FATAL_ERROR "SSE UNSUPPORT")
endif()

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    message("enable ccache")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif(CCACHE_FOUND)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
# using Clang
    set(CMAKE_CXX_FLAGS "-march=native ${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
# using GCC
    set(CMAKE_CXX_FLAGS "-msse4 ${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
# using Visual Studio C++
    set(CMAKE_CXX_FLAGS "/MP /source-charset:utf-8 ${CMAKE_CXX_FLAGS}")
endif()

add_definitions(-D_UNICODE)
add_definitions(-DUNICODE)
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
    #if(MSVC)
        #set(CMAKE_CXX_FLAGS "/d1 reportSingleClassLayoutVector2f ${CMAKE_CXX_FLAGS}")
    #endif()
elseif(CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo" OR CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
    add_definitions(-DNDEBUG)
endif()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_C_VISIBILITY_PRESET hidden)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/bin/RelWithDebInfo)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/lib/RelWithDebInfo)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/bin/RelWithDebInfo)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR}/bin/MinSizeRel)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR}/lib/MinSizeRel)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR}/bin/MinSizeRel)

include_directories(${PROJECT_SOURCE_DIR}/NRender/Common/include/)
include_directories(${PROJECT_SOURCE_DIR}/NRender/)

aux_source_directory(NRender/Common/source SOURCE_FILES)
aux_source_directory(NRender/D3D12Render SOURCE_FILES)
aux_source_directory(NRender/SoftRender SOURCE_FILES)
aux_source_directory(NRender/WindowFramework/source SOURCE_FILES)
message("Those files are now in project:\n${SOURCE_FILES}")

set(RELATIVE_LIBS "Gdiplus")
List(APPEND RELATIVE_LIBS "d3d12")
List(APPEND RELATIVE_LIBS "dxgi")
List(APPEND RELATIVE_LIBS "d3dcompiler")

macro(create_test TEST_NAME)
    add_executable(${TEST_NAME} "Test/${TEST_NAME}.cpp" ${SOURCE_FILES})
    target_link_libraries(${TEST_NAME} PRIVATE ${RELATIVE_LIBS})
    add_test(NAME "${TEST_NAME}_CTest" COMMAND ${TEST_NAME})
    message("create test:${TEST_NAME}")
endmacro()

add_subdirectory(Test)

aux_source_directory(NRender SOURCE_FILES)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${RELATIVE_LIBS})