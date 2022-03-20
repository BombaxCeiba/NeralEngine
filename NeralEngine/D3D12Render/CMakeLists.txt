aux_source_from_current_directory_to_project("${MAIN_PROJECT_NAME}")
#config test
config_test_source(TEST_SOURCE)
aux_source_directory(./Test TEST_SOURCE)
set(DEPENDENT_LIBS "d3d12;dxgi")
create_gtest_from_files_and_use_current_directory_as_dependency("${TEST_SOURCE}" "${DEPENDENT_LIBS}")
#end config test
append_lib_to_global_property(GLOBAL_DEPENDENT_LIBS "${DEPENDENT_LIBS}")