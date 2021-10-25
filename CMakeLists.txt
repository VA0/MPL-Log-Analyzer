cmake_minimum_required(VERSION 3.10)
project(Analyzer)


add_executable(${PROJECT_NAME} main.cpp Line.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_link_libraries(${PROJECT_NAME} tbb)
