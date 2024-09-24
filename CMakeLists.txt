cmake_minimum_required(VERSION 3.0)
project(Q4)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(Q4 Q4.cpp)
target_link_libraries(Q4 ${OpenCV_LIBS})
