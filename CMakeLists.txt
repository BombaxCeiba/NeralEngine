cmake_minimum_required(VERSION 3.15)

set(MAIN_PROJECT_NAME "NRender")

project(${MAIN_PROJECT_NAME})
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
    message("----Enable ccache!")
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
endif(CCACHE_FOUND)

find_program(LLD_FOUND lld)
if(LLD_FOUND)
    if(NOT DISABLE_LLD)
            if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
                message("----This project will use lld as linker.")
                message("----Disable this behavior by setting CMake variable DISABLE_LLD=ON.")
                set(CMAKE_CXX_FLAGS "-fuse-ld=lld ${CMAKE_CXX_FLAGS}")
        endif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    endif(NOT DISABLE_LLD)
endif(LLD_FOUND)

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
    set(CMAKE_CXX_FLAGS "/MP /source-charset:utf-8 /Zc:__cplusplus ${CMAKE_CXX_FLAGS}")
endif()

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

#add_subdirectory(3rdpart)
add_subdirectory("${MAIN_PROJECT_NAME}")