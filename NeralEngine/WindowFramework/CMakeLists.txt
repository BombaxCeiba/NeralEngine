aux_source_from_directory_to_project("${MAIN_PROJECT_NAME}" ./source)
#config test
config_test_source(TEST_SOURCE)
aux_source_directory(./Test TEST_SOURCE)
create_gtest_from_files_and_use_specific_directory_as_dependency("${TEST_SOURCE}" ./source "gdiplus")
#end config test
append_lib_to_global_property(GLOBAL_DEPENDENT_LIBS "gdiplus")