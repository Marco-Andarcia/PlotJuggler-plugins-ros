#include "datastream_ROS.h"
#include <QTextStream>
#include <QFile>
#include <QMessageBox>
#include <QDebug>
#include <thread>
#include <mutex>
#include <chrono>
#include <thread>
#include <QProgressDialog>
#include <QtGlobal>
#include "rostopicselector.h"
#include <ros/callback_queue.h>
#include <QApplication>


DataStreamROS::DataStreamROS()
{
    _enabled = false;
    _running = false;
}

PlotDataMap& DataStreamROS::getDataMap()
{
    return _plot_data;
}

void DataStreamROS::topicCallback(const topic_tools::ShapeShifter::ConstPtr& msg, const std::string &topic_name)
{
    if( !_running ||  !_enabled)
    {
        return;
    }

    using namespace RosTypeParser;

    static std::set<std::string> registered_type;

    auto& data_type = msg->getDataType();

    if( registered_type.find( data_type ) == registered_type.end() )
    {
        registered_type.insert( data_type );
        buildRosTypeMapFromDefinition( data_type,
                                 msg->getMessageDefinition(),
                                 &_ros_type_map);
    }

    //------------------------------------
    uint8_t buffer[1024*64]; // "64 KB ought to be enough for anybody"
    double msg_time = (ros::Time::now() - _initial_time).toSec();

    RosTypeFlat flat_container;

    ros::serialization::OStream stream(buffer, sizeof(buffer));
    msg->write(stream);

    uint8_t* buffer_ptr = buffer;

    String datatype( data_type.data(), data_type.size() );
    String topicname( topic_name.data(), topic_name.length() );


    flat_container = buildRosFlatType( _ros_type_map, datatype, topicname, &buffer_ptr);
    applyNameTransform( _rules, &flat_container );

    // qDebug() << " pushing " << msg_time;

    for(auto& it: flat_container.value_renamed )
    {
        std::string field_name ( it.first.data(), it.first.size());
        auto value = it.second;

        auto plot = _plot_data.numeric.find( field_name );
        if( plot == _plot_data.numeric.end() )
        {
            PlotDataPtr temp(new PlotData());
            temp->setCapacity( 1000 );
            temp->setMaximumRangeX( 4.0 );
            auto res = _plot_data.numeric.insert( std::make_pair(field_name, temp ) );
            plot = res.first;
        }

        plot->second->pushBack( PlotData::Point(msg_time, value));
    }
}

void DataStreamROS::extractInitialSamples()
{
    _initial_time = ros::Time::now();

    int wait_time = 4;

    QProgressDialog progress_dialog;
    progress_dialog.setLabelText( "Collecting ROS topic samples to understand data layout. ");
    progress_dialog.setRange(0, wait_time*1000);
    progress_dialog.setAutoClose(true);
    progress_dialog.setAutoReset(true);

    progress_dialog.show();

    using namespace std::chrono;
    auto start_time = system_clock::now();

    _enabled = true;
    while ( system_clock::now() - start_time < seconds(wait_time) )
    {
        ros::getGlobalCallbackQueue()->callAvailable(ros::WallDuration(0.1));
        int i = duration_cast<milliseconds>(system_clock::now() - start_time).count() ;
        progress_dialog.setValue( i );
        QApplication::processEvents();
        if( progress_dialog.wasCanceled() )
        {
            break;
        }
    }
    _enabled = false;

    if( progress_dialog.wasCanceled() == false )
    {
        progress_dialog.cancel();
    }
}


bool DataStreamROS::launch()
{
    if( _running )
    {
        _plot_data.numeric.clear();
        _running = false;
        if(ros::isStarted())
        {
            ros::shutdown(); // explicitly needed since we use ros::start();
            ros::waitForShutdown();
        }
        _thread.join();
    }

    RosTopicSelector dialog( 0 );
    int res = dialog.exec();

    QStringList topic_selected = dialog.getSelectedTopicsList();

    if( res != QDialog::Accepted || topic_selected.empty() )
    {
        return false;
    }

    // load the rules
    QStringList rules_by_line = dialog.getLoadedRules().split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    _rules.clear();
    for (auto line: rules_by_line)
    {
        QStringList tags = line.split(QRegExp("[\r\n \t]"),QString::SkipEmptyParts);
        if( tags.size() == 3)
        {
            _rules.push_back( RosTypeParser::SubstitutionRule(
                              tags.at(0).toStdString() ,
                              tags.at(1).toStdString() ,
                              tags.at(2).toStdString()
                              ) );
        }
        else{
            qDebug() << "ERROR parsing this rule: " << line;
        }
    }
    //-------------------------

    ros::start(); // needed because node will go out of scope
    ros::NodeHandle node;

    _subscribers.clear();
    for (int i=0; i<topic_selected.size(); i++ )
    {
        auto topic_name = topic_selected.at(i).toStdString();
        boost::function<void(const topic_tools::ShapeShifter::ConstPtr&) > callback;
        callback = boost::bind( &DataStreamROS::topicCallback, this, _1, topic_name ) ;
        _subscribers.push_back( node.subscribe( topic_name, 1000,  callback)  );
    }

    _running = true;

    extractInitialSamples();

    _thread = std::thread([this](){ this->update();} );

    return true;
}

void DataStreamROS::enableStreaming(bool enable) { _enabled = enable; }

bool DataStreamROS::isStreamingEnabled() const { return _enabled; }

DataStreamROS::~DataStreamROS()
{
    _subscribers.clear();

    if(ros::isStarted() && _running)
    {
        _running = false;
        ros::shutdown(); // explicitly needed since we use ros::start();;
        _thread.join();
    }
}

const char*  DataStreamROS::name()
{
    return "ROS Topic Streamer";
}

void DataStreamROS::update()
{
    while (ros::ok() && _running)
    {
        ros::getGlobalCallbackQueue()->callAvailable(ros::WallDuration(0.1));
    }
    ros::shutdown();
}
