#ifndef DATASTREAM_ROS_TOPIC_H
#define DATASTREAM_ROS_TOPIC_H

#include <QtPlugin>
#include <thread>
#include <topic_tools/shape_shifter.h>
#include <PlotJuggler/datastreamer_base.h>
#include "ruleloaderwidget.h"
#include <ros_type_introspection/ros_type_introspection.hpp>

class  DataStreamROS: public QObject, DataStreamer
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.icarustechnology.Superplotter.DataStreamer" "../datastreamer.json")
    Q_INTERFACES(DataStreamer)

public:

    DataStreamROS();

    virtual PlotDataMap &getDataMap();

    virtual bool launch();

    virtual void enableStreaming(bool enable);

    virtual bool isStreamingEnabled() const;

    virtual ~DataStreamROS();

    virtual const char* name();

private:

    void topicCallback(const topic_tools::ShapeShifter::ConstPtr& msg, const std::string &topic_name);

    void update();

    PlotDataMap _plot_data;

    bool _enabled;

    bool _running;

    std::thread _thread;

    RosTypeParser::RosTypeMap _ros_type_map;

    void extractInitialSamples();

    ros::Time _initial_time;

    std::vector<ros::Subscriber> _subscribers;

    std::vector<RosTypeParser::SubstitutionRule> _rules;
};

#endif // DATALOAD_CSV_H
