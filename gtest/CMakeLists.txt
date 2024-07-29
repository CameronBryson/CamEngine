find_package(GTest REQUIRED)
add_executable(google_testing main.cpp sampleTest.cpp)

target_link_libraries(google_testing GTest::gtest_main)

include(GoogleTest)
add_test (NAME google_testing COMMAND google_testing)
