aux_source_from_current_directory_to_project("${MAIN_PROJECT_NAME}")
#config test
config_test_source(TEST_SOURCE)
aux_source_directory(./Test TEST_SOURCE)
create_gtest_from_files_and_use_current_directory_as_dependency("${TEST_SOURCE}" "")