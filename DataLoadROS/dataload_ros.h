#ifndef DATALOAD_ROS_H
#define DATALOAD_ROS_H

#include <ros/ros.h>
#include <rosbag/bag.h>

#include <QObject>
#include <QtPlugin>
#include <PlotJuggler/dataloader_base.h>
#include "ros-type-parser.h"

class  DataLoadROS: public QObject, DataLoader
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.icarustechnology.Superplotter.DataLoader" "../dataloader.json")
    Q_INTERFACES(DataLoader)

public:
    DataLoadROS();
    virtual const std::vector<const char*>& compatibleFileExtensions() const ;

    virtual PlotDataMap readDataFromFile(const std::string& file_name,
                                          std::string &time_index_name  );

    virtual ~DataLoadROS();

protected:
    void loadSubstitutionRule(QStringList all_topic_names);

private:
    std::vector<RosTypeParser::SubstitutionRule>  _rules;

    std::vector<const char*> _extensions;


};

#endif // DATALOAD_CSV_H
