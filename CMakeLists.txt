cmake_minimum_required(VERSION 2.8.3)
project(plotjuggler_ros_plugins)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  rosbag
  rosbag_storage
  roscpp
  roscpp_serialization
  ros_type_introspection
  rostime
  topic_tools
)



## System dependencies are found with CMake's conventions

find_package(Qt5Core)
find_package(Qt5Widgets)

# Qt related stuff
set(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIC")

include_directories(
    /usr/local/include
    ${Qt5Widgets_INCLUDE_DIRS}
    ${catkin_INCLUDE_DIRS}
    )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if you package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need

catkin_package(
  # INCLUDE_DIRS 
  # LIBRARIES   
   CATKIN_DEPENDS ros_type_introspection
   DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
# include_directories(include)
include_directories(
  ${catkin_INCLUDE_DIRS}
  ${Qt5Widgets_INCLUDE_DIRS}
  /usr/local/include
)

#############
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)

SET( DATALOAD_SRC
	 DataLoadROS/dataload_ros.cpp
     DataLoadROS/dialog_select_ros_topics.h
     DataLoadROS/dialog_select_ros_topics.cpp
     ruleloaderwidget.h
     ruleloaderwidget.cpp
    )

QT5_WRAP_UI ( DATALOAD_UI_SRC
    DataLoadROS/dialog_select_ros_topics.ui
    ruleloaderwidget.ui
    )

SET( DATASTREAM_SRC
    DataStreamROS/datastream_ROS.cpp
    DataStreamROS/datastream_ROS.h
    DataStreamROS/rostopicselector.h
    DataStreamROS/rostopicselector.cpp
    ruleloaderwidget.h
    ruleloaderwidget.cpp
    )
    
QT5_WRAP_UI ( DATASTREAM_UI_SRC
    DataStreamROS/rostopicselector.ui
    ruleloaderwidget.ui )   
#############  

## Declare a C++ library
add_library( DataLoadROS SHARED ${DATALOAD_SRC} ${DATALOAD_UI_SRC} )
target_link_libraries( DataLoadROS  ${catkin_LIBRARIES} )

add_dependencies(DataLoadROS 
	${${PROJECT_NAME}_EXPORTED_TARGETS} 
	${catkin_EXPORTED_TARGETS}
	)

add_library( DataStreamROS SHARED ${DATASTREAM_SRC} ${DATASTREAM_UI_SRC} )
target_link_libraries( DataStreamROS  ${catkin_LIBRARIES} )

add_dependencies(DataStreamROS 
	${${PROJECT_NAME}_EXPORTED_TARGETS} 
	${catkin_EXPORTED_TARGETS}
	)


#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executables and/or libraries for installation
install(TARGETS DataLoadROS DataStreamROS 
        LIBRARY DESTINATION /usr/local/lib/PlotJuggler )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )


