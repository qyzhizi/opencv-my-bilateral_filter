# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.10.2)

project(mybilateralfilter)


find_package( OpenCV REQUIRED )

message("WARNING" "OpenCV_INCLUDE_DIRS:" ${OpenCV_INCLUDE_DIRS})
message("WARNING" "OpenCV_LIBS:" ${OpenCV_LIBS})

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(mybilateralfilter main.cpp my_bilateral_filter.cpp )
target_link_libraries(mybilateralfilter ${OpenCV_LIBS})

