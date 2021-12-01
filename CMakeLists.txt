cmake_minimum_required(VERSION 3.20)

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
elseif(CMAKE_BUILD_TYPE MATCHES "Release" OR CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    add_definitions(-DNDEBUG)
endif()

add_subdirectory(NRender)

enable_testing()
add_subdirectory(Test)