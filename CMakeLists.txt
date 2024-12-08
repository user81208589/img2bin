cmake_minimum_required(VERSION 3.10)
project(ImageCanvasApp)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt5Widgets REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(include)

add_executable(app main.cpp src/ImageCanvas.cpp)
target_link_libraries(app Qt5::Widgets ${OpenCV_LIBRARIES})
