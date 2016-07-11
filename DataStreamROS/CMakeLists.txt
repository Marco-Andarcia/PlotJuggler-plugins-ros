

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)

QT5_WRAP_UI ( UI_SRC
    rostopicselector.ui
    ../ruleloaderwidget.ui )


SET( SRC
    datastream_ROS.cpp
    datastream_ROS.h
    rostopicselector.h
    rostopicselector.cpp
    ../ruleloaderwidget.h
    ../ruleloaderwidget.cpp
    )


add_library(DataStreamROS SHARED ${SRC} ${UI_SRC}  )


target_link_libraries(DataStreamROS  RosTypeParser
    ${Qt5Widgets_LIBRARIES}
    ${ROS_DIRECTORY}/lib/libroscpp.so
    ${ROS_DIRECTORY}/lib/librosbag.so
    ${ROS_DIRECTORY}/lib/librosbag_storage.so
    ${ROS_DIRECTORY}/lib/librostime.so
    ${ROS_DIRECTORY}/lib/libroscpp_serialization.so
    ${ROS_DIRECTORY}/lib/libtopic_tools.so
    )

INSTALL(TARGETS DataStreamROS DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/PlotJuggler )
