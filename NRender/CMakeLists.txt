function(aux_source_from_dir_list DIR_LIST RESULT)
    foreach(DIR ${DIR_LIST})
        aux_source_directory(${DIR} RESULT)
    endforeach(DIR ${DIR_LIST})

    set(${RESULT} ${${RESULT}} PARENT_SCOPE)
endfunction()

set(NRENDER_DIR "${PROJECT_SOURCE_DIR}/NRender")
include_directories(${NRENDER_DIR}/Common/include/)
include_directories(${NRENDER_DIR}/)

set(RELATIVE_LIBS "Gdiplus")
List(APPEND RELATIVE_LIBS "d3d12")
List(APPEND RELATIVE_LIBS "dxgi")
List(APPEND RELATIVE_LIBS "d3dcompiler")

set(NRENDER_SOURCES_DIRS)
List(APPEND NRENDER_SOURCES_DIRS ${NRENDER_DIR}/Common/source)
List(APPEND NRENDER_SOURCES_DIRS ${NRENDER_DIR}/D3D12Render)
List(APPEND NRENDER_SOURCES_DIRS ${NRENDER_DIR}/SoftRender)
List(APPEND NRENDER_SOURCES_DIRS ${NRENDER_DIR}/WindowFramework/source)
aux_source_from_dir_list("${NRENDER_SOURCES_DIRS}" SOURCE_FILES)

add_subdirectory(Test)

aux_source_directory(. SOURCE_FILES)
message("----Those files are now in project:\n----${SOURCE_FILES}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${RELATIVE_LIBS})