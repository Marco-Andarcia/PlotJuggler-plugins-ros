

include_directories( ${ROS_DIRECTORY}/include ./ )


SET( ROS_PARSER_SRC
    	ros-type-parser.cpp
	ros-type-parser.h
	string.hpp
    )

add_library(RosTypeParser ${ROS_PARSER_SRC})

target_link_libraries(RosTypeParser)
