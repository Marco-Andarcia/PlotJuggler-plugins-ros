#include "dataload_ros.h"
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include "dialog_select_ros_topics.h"
#include <QDebug>
#include <QApplication>
#include <QProgressDialog>
#include "rosbag/view.h"
#include "ros-type-parser.h"

DataLoadROS::DataLoadROS()
{
    _extensions.push_back( "bag");
}

const std::vector<const char*> &DataLoadROS::compatibleFileExtensions() const
{
    return _extensions;
}


PlotDataMap DataLoadROS::readDataFromFile(const std::string& file_name,
                                          std::string &time_index_name  )
{

    using namespace RosTypeParser;

    QStringList all_topic_names;
    PlotDataMap plot_data;

    rosbag::Bag bag;
    bag.open( file_name, rosbag::bagmode::Read );

    rosbag::View bag_view ( bag, ros::TIME_MIN, ros::TIME_MAX, true );

    std::vector<const rosbag::ConnectionInfo *> connections = bag_view.getConnections();

    // create a list and a type map for each topic
    RosTypeMap type_map;

    for(int i=0;  i<  connections.size(); i++)
    {
        all_topic_names.push_back( QString( connections[i]->topic.c_str() ) );
        parseRosTypeDescription( connections[i]->datatype,
                                 connections[i]->msg_def,
                                 &type_map);
    }

    printRosTypeMap  (type_map );

    int count = 0;

    std::vector<uint8_t> buffer ( 65*1024 );

    DialogSelectRosTopics* dialog = new DialogSelectRosTopics( all_topic_names );

    std::set<std::string> topic_selected;

    if( dialog->exec() == QDialog::Accepted)
    {
        auto selected_items = dialog->getSelectedItems();
        for(auto item: selected_items)
        {
            topic_selected.insert( item.toStdString() );
        }
        // load the rules
        QStringList rules_by_line = dialog->getLoadedRules().split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
        _rules.clear();
        for (auto line: rules_by_line)
        {
            QStringList tags = line.split(QRegExp("[\r\n \t]"),QString::SkipEmptyParts);
            if( tags.size() == 3)
            {
                _rules.push_back( SubstitutionRule(
                                  tags.at(0).toStdString() ,
                                  tags.at(1).toStdString() ,
                                  tags.at(2).toStdString()
                                  ) );
            }
            else{
                qDebug() << "ERROR parsing this rule: " << line;
            }
        }
    }

    std::vector<SubstitutionRule> rules;
    rules.push_back( SubstitutionRule("/data.vectors[#].value",      "/data.vectors[#].name",   "#"));
    rules.push_back( SubstitutionRule("/data.doubles[#].value",      "/data.doubles[#].name",   "#"));
    rules.push_back( SubstitutionRule("/data.vectors3d[#].value[0]", "/data.vectors3d[#].name", "#.x"));
    rules.push_back( SubstitutionRule("/data.vectors3d[#].value[1]", "/data.vectors3d[#].name", "#.y"));
    rules.push_back( SubstitutionRule("/data.vectors3d[#].value[2]", "/data.vectors3d[#].name", "#.z"));

    QProgressDialog progress_dialog;
    progress_dialog.setLabelText("Loading... please wait");
    progress_dialog.setWindowModality( Qt::ApplicationModal );
    progress_dialog.setRange(0, bag_view.size() -1);
    progress_dialog.show();

    for(rosbag::MessageInstance msg: bag_view )
    {
        if( topic_selected.find( msg.getTopic()) == topic_selected.end() )
        {
            continue;
        }

        double msg_time = msg.getTime().toSec();

        if( count++ %100 == 0)
        {
            progress_dialog.setValue( count );
            QApplication::processEvents();

            if( progress_dialog.wasCanceled() ) {
                return PlotDataMap();
            }
        }

        RosTypeFlat flat_container;

        ros::serialization::OStream stream(buffer.data(), buffer.size());
        msg.write(stream);

        uint8_t* buffer_ptr = buffer.data();

        String datatype( msg.getDataType().data(), msg.getDataType().size() );
        String topic_name( msg.getTopic().data(), msg.getTopic().size() );

        buildRosFlatType(type_map, datatype, topic_name, &buffer_ptr,  &flat_container);
        applyNameTransform( _rules, &flat_container );

        for(auto& it: flat_container.value_renamed )
        {
            std::string field_name ( it.first.data(), it.first.size());
            auto value = it.second;

            auto plot = plot_data.numeric.find( field_name );
            if( plot == plot_data.numeric.end() )
            {
                PlotDataPtr temp(new PlotData());
                temp->setCapacity( bag_view.size() );
                auto res = plot_data.numeric.insert( std::make_pair(field_name, temp ) );
                plot = res.first;
            }

            plot->second->pushBack( PlotData::Point(msg_time, value));
        }

        //   qDebug() << msg.getTopic().c_str();
    }

    return plot_data;
}



DataLoadROS::~DataLoadROS()
{

}


