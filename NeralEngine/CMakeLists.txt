################ Function declaration start ################
function(aux_source_from_dir_list DIR_LIST RESULT)
    foreach(DIR ${DIR_LIST})
        aux_source_directory(${DIR} RESULT)
    endforeach(DIR ${DIR_LIST})

    set(${RESULT} ${${RESULT}} PARENT_SCOPE)
endfunction()

function(get_current_directory_name RESULT)
    string(REGEX REPLACE ".*/\(.*\)" "\\1" OUTPUT ${CMAKE_CURRENT_SOURCE_DIR})
    set(${RESULT} ${OUTPUT} PARENT_SCOPE)
endfunction()

function(aux_source_from_current_directory_to_project_and_output PROJECT_NAME SOURCE_IN_CURRENT_DIRECTORY)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE)
    target_sources(${PROJECT_NAME} ${SOURCE_IN_CURRENT_DIRECTORY})
    set(${SOURCE_IN_CURRENT_DIRECTORY} ${SOURCE} PARENT_SCOPE)
endfunction()

function(aux_source_from_current_directory_to_project PROJECT_NAME)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE)
    if(NOT "${SOURCE}" STREQUAL "")
        target_sources(${PROJECT_NAME} PRIVATE ${SOURCE})
    endif()
endfunction()

function(aux_source_from_directory_to_project PROJECT_NAME DIRECTORY)
    aux_source_directory(${DIRECTORY} SOURCE)
    if(NOT "${SOURCE}" STREQUAL "")
        target_sources(${PROJECT_NAME} PRIVATE ${SOURCE})
    endif()
endfunction()

function(append_lib_to_global_property PROPERTY_NAME CONTENT)
    get_property(THE_PROPERTY GLOBAL PROPERTY "${PROPERTY_NAME}")
    List(APPEND THE_PROPERTY "${CONTENT}")
    set_property(GLOBAL PROPERTY "${PROPERTY_NAME}" "${THE_PROPERTY}")
endfunction()

function(config_test_source RESULT)
    get_property(TEST_SOURCE GLOBAL PROPERTY GLOBAL_TEST_MAIN_SOURCE_FILE)
    set(${RESULT} ${TEST_SOURCE} PARENT_SCOPE)
endfunction()

function(create_gtest TEST_NAME TEST_FILES DEPENDENT_SOURCE_FILES DEPENDENT_LIBS)
    if("${TEST_FILES}" STREQUAL "")
        return()
    endif()
    List(LENGTH TEST_FILES TEST_FILE_COUNT)
    set(SHOULD_CREATE_TEST OFF)
    if(TEST_FILE_COUNT EQUAL 2)
        List(GET TEST_FILES 1 TEST_2ND_FILE)
        if(NOT "${TEST_2ND_FILE}" STREQUAL "")
            set(SHOULD_CREATE_TEST ON)
        endif()
    elseif(TEST_FILE_COUNT GREATER 1)
        set(SHOULD_CREATE_TEST ON)
    endif()

    if(SHOULD_CREATE_TEST)
        message(STATUS "Create gtest:${TEST_NAME}.")
        #create test executable
        add_executable(${TEST_NAME} ${TEST_FILES})
        #set test dependent lib name
        set(TEST_DEPENDENCIES_LIB "${TEST_NAME}Dependency")
        #create test dependent lib
        if(NOT "${DEPENDENT_SOURCE_FILES}" STREQUAL "")
            add_library(${TEST_DEPENDENCIES_LIB} STATIC EXCLUDE_FROM_ALL ${DEPENDENT_SOURCE_FILES})
            #force cmake output lib to ${CMAKE_BINARY_DIR}/Test/lib
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/Test/lib)
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Test/lib)
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Test/lib)
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Test/lib)
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Test/lib)
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Test)
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Test)
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Test)
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Test)
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Test)
            #force the lib full name is lib${TEST_DEPENDENCIES_LIB}
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES PREFIX "lib")
            #force debug the lib postfix is "_d"
            set_target_properties(${TEST_DEPENDENCIES_LIB} PROPERTIES DEBUG_POSTFIX "_d")
            #force build the lib before test executable
            add_dependencies(${TEST_NAME} ${TEST_DEPENDENCIES_LIB})
            #dependent the lib link test executable dependent libs
            target_link_libraries(${TEST_DEPENDENCIES_LIB} ${DEPENDENT_LIBS})
            #add the lib to test executable dependent libs
            List(APPEND DEPENDENT_LIBS "${TEST_DEPENDENCIES_LIB}")
        endif()
        #use pthread or mt
        if(MSVC)
            set_property(TARGET ${TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
            target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest ${DEPENDENT_LIBS})
        else()
            target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest pthread ${DEPENDENT_LIBS})
        endif()
        #force cmake output test executable to ${CMAKE_BINARY_DIR}/Test
        set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Test)
        set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Test)
        set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Test)
        set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Test)
        set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Test)
        gtest_discover_tests(${TEST_NAME}
                             WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Test
                             TEST_LIST ${GTEST_LIST})
        #add test to target BuildAndTestAll
        add_dependencies(BuildAndTestAll ${TEST_NAME})
        #finish config
        message(STATUS "Succeed!")
    endif()
endfunction()
#example: create_gtest(AllTest "${TEST_FILES}" "${SOURCE_FILES}" "${LIBS})

function(create_gtest_for_lib TEST_NAME TEST_FILES LIB_NAME)
    message(STATUS "Create gtest:${TEST_NAME}.")
    #create test executable
    add_executable(${TEST_NAME} ${TEST_FILES})
    add_dependencies(${TEST_NAME} ${LIB_NAME})
    #use pthread or mt
    if(MSVC)
        set_property(TARGET ${TEST_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest ${LIB_NAME})
    else()
        target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest pthread ${LIB_NAME})
    endif()
        #force cmake output test executable to ${CMAKE_BINARY_DIR}/Test
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_BINARY_DIR}/Test)
    set_target_properties(${TEST_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_BINARY_DIR}/Test)
    gtest_discover_tests(${TEST_NAME}
                         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/Test
                         TEST_LIST ${GTEST_LIST})
    #add test to target BuildAndTestAll
    add_dependencies(BuildAndTestAll ${TEST_NAME})
    #finish config
    message(STATUS "Succeed!")
endfunction()

function(create_gtest_from_files_and_use_current_directory_as_dependency TEST_SOURCE DEPENDENT_LIBS)
    get_current_directory_name(TEST_NAME)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} TEST_DEPENDENCIES_SOURCE)
    create_gtest("${TEST_NAME}Test" "${TEST_SOURCE}" "${TEST_DEPENDENCIES_SOURCE}" "${DEPENDENT_LIBS}")
endfunction()

function(create_gtest_from_files_and_use_specific_directory_as_dependency TEST_SOURCE SPECIFIC_DIRECTORY DEPENDENT_LIBS)
    get_current_directory_name(TEST_NAME)
    aux_source_directory(${SPECIFIC_DIRECTORY} TEST_DEPENDENCIES_SOURCE)
    create_gtest("${TEST_NAME}Test" "${TEST_SOURCE}" "${TEST_DEPENDENCIES_SOURCE}" "${DEPENDENT_LIBS}")
endfunction()

# ############### Function declaration end ################

# config global test project
include(FetchContent)
set(GTEST_PROJECT_NAME "googletest_1_12_1")
set(GTEST_SOURCE_PATH "${THRID_PARTY_DIR}/${GTEST_PROJECT_NAME}")
FetchContent_Declare(${GTEST_PROJECT_NAME}
    GIT_REPOSITORY git@github.com:google/googletest.git
    GIT_TAG release-1.12.1
)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
set(gtest_build_tests OFF CACHE BOOL "" FORCE)
set(gtest_build_samples OFF CACHE BOOL "" FORCE)
message(STATUS "Configuring gtest...")
FetchContent_MakeAvailable(${GTEST_PROJECT_NAME})
message(STATUS "Finish!")

enable_testing()
include(GoogleTest)

# create all tests target
add_custom_target(BuildAndTestAll ${CMAKE_CTEST_COMMAND} -V --rerun-failed --output-on-failure)

# !!maybe_unused!! force cmake output executable to ${PROJECT_SOURCE_DIR}/Test
set_target_properties(BuildAndTestAll PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/Test)
set_target_properties(BuildAndTestAll PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/Test)
set_target_properties(BuildAndTestAll PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/Test)
set_target_properties(BuildAndTestAll PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/Test)
set_target_properties(BuildAndTestAll PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR}/Test)

# declare GLOBAL_TEST_MAIN_SOURCE_FILE through it's not necessary
set_property(GLOBAL PROPERTY GLOBAL_TEST_MAIN_SOURCE_FILE "")

# config public test components
add_subdirectory(PublicTestComponent)
include_directories(./PublicTestComponent)

#project build command start
set(NERAL_ENGINE_DIR "${PROJECT_SOURCE_DIR}/${MAIN_PROJECT_NAME}")
include_directories(${NERAL_ENGINE_DIR})
set_property(GLOBAL PROPERTY GLOBAL_DEPENDENT_LIBS "")
#create main project
aux_source_directory(. SOURCE_FILES)
add_executable(${MAIN_PROJECT_NAME} ${SOURCE_FILES})
set_target_properties(${MAIN_PROJECT_NAME} PROPERTIES VERSION ${MAIN_PROJECT_VERSION})
#add main project components
add_subdirectory(Version)
add_subdirectory(Common)
add_subdirectory(WindowFramework)
add_subdirectory(D3D12Render)
add_subdirectory(RayTrackingRender)
add_subdirectory(SoftRender)
#config dependencies
add_dependencies(${MAIN_PROJECT_NAME} NeralVersion)
#add GLOBAL_DEPENDENT_LIBS to main project config
get_property(RELATIVE_LIBS GLOBAL PROPERTY GLOBAL_DEPENDENT_LIBS)
List(REMOVE_DUPLICATES RELATIVE_LIBS)
target_link_libraries(${MAIN_PROJECT_NAME} PRIVATE ${RELATIVE_LIBS})