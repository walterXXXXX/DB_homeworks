cmake_minimum_required(VERSION 3.20)

project(Lesson05_Task1)

add_executable(Lesson05_Task1 main.cpp)

add_subdirectory(libpqxx-7.9.0 libpqxx-build)

target_compile_features(Lesson05_Task1 PRIVATE cxx_std_20)

target_link_libraries(Lesson05_Task1 pqxx)