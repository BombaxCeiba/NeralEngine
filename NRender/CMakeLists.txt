include_directories("./Common/include")

aux_source_directory(. SOURCE_FILES)
aux_source_directory(Common/source SOURCE_FILES)
aux_source_directory(D3D12Render SOURCE_FILES)
aux_source_directory(SoftRender SOURCE_FILES)
aux_source_directory(WindowFramework/source SOURCE_FILES)
message("Those files are now in project:\n ${SOURCE_FILES}")

set(RELATIVE_LIBS "Gdiplus")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${RELATIVE_LIBS})