cmake_minimum_required(VERSION 3.0)
project(google-map-traffic-analysis)
set(sources traffic-analysis.cpp)
add_executable(traffic-analysis ${sources})
target_compile_options(traffic-analysis PUBLIC -O3 -std=c++1y -Wall -Wfloat-conversion)
target_include_directories(traffic-analysis PUBLIC .)
target_link_libraries(traffic-analysis PUBLIC
    opencv_imgproc
    opencv_core
    opencv_imgcodecs
    opencv_highgui
)
