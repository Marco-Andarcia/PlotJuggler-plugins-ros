project(PlotJuggler-ros-plugins)
cmake_minimum_required(VERSION 2.8)


find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Xml)


set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")

set( ROS_DIRECTORY "/opt/ros/kinetic" )

include_directories(
    /usr/local/include
    ${Qt5Widgets_INCLUDE_DIRS}
    ${ROS_DIRECTORY}/include
    ros-type-parser
    )

add_subdirectory( ros-type-parser )
add_subdirectory( DataLoadROS )
add_subdirectory( DataStreamROS )


SET(CMAKE_INSTALL_PREFIX  "/usr/local" )


