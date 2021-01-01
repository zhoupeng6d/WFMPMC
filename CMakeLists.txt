

cmake_minimum_required(VERSION 3.10)

project(wfmpmc)

enable_testing()

add_compile_options(-std=c++11)


find_package(GTest REQUIRED)

add_executable(test_queue test_queue.cc)

target_link_libraries(test_queue GTest::GTest GTest::Main)
add_test(NAME test_queue
         COMMAND test_queue)
