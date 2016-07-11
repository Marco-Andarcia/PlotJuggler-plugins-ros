#include <QMessageBox>
#include <QModelIndexList>
#include <QListWidget>
#include <QSettings>
#include <ros/ros.h>
#include <ros/master.h>
#include "rostopicselector.h"
#include "ui_rostopicselector.h"

RosTopicSelector::RosTopicSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RosTopicSelector)
{
    ui->setupUi(this);

    QSettings settings( "IcarusTechnology", "PlotJuggler");

    auto master_ui = settings.value("DataStreamRos.master_uri", tr("http://localhost:11311")).toString();
    auto host_ip   = settings.value("DataStreamRos.host_ip", tr("localhost")).toString();

    ui->lineEditMasterURI->setText( master_ui );
    ui->lineEditHostIP->setText( host_ip );

    _rule_widget = new RuleLoaderWidget(this);
    ui->frame_2->layout()->addWidget( _rule_widget );

}

RosTopicSelector::~RosTopicSelector()
{
    QSettings settings( "IcarusTechnology", "PlotJuggler");

    settings.setValue ("DataStreamRos.master_uri",  ui->lineEditMasterURI->text() );
    settings.setValue("DataStreamRos.host_ip",      ui->lineEditHostIP->text() );

    delete ui;
}

QStringList RosTopicSelector::getSelectedTopicsList()
{
    return _selected_topics;
}

void RosTopicSelector::showNoMasterMessage() {
    QMessageBox msgBox;
    msgBox.setText("Couldn't find the ros master.");
    msgBox.exec();
}

void RosTopicSelector::on_buttonConnect_pressed()
{
    int init_argc = 0;
    char** init_argv = NULL;

    if(ui->checkBoxEnvironmentSettings->isChecked() &&  qgetenv("ROS_MASTER_URI").isEmpty() )
    {
        QMessageBox msgBox;
        msgBox.setText("ROS_MASTER_URI is not defined in the environment.\n"
                       "Either type the following or (preferrably) add this to your ~/.bashrc \n"
                       "file in order set up your local machine as a ROS master:\n\n"
                       "    export ROS_MASTER_URI=http://localhost:11311\n\n"
                       "Then, type 'roscore' in another shell to actually launch the master program.");
        msgBox.exec();
        return;
    }

    if( ui->checkBoxEnvironmentSettings->isChecked() )
    {
        ros::init(init_argc,init_argv,"PlotJugglerStreamingListener");
        if ( ! ros::master::check() ) {
            showNoMasterMessage();
            return;
        }
    }
    else{
        std::map<std::string,std::string> remappings;
        remappings["__master"]   = ui->lineEditMasterURI->text().toStdString();
        remappings["__hostname"] = ui->lineEditHostIP->text().toStdString();

        ros::init(remappings, "PlotJugglerStreamingListener");
        if ( ! ros::master::check() ) {
            showNoMasterMessage();
            return;
        }
    }
    ui->buttonConnect->setEnabled(false);
    ui->buttonDisconnect->setEnabled(true);

    QStringList topic_advertised;

    ros::master::V_TopicInfo topic_infos;
    ros::master::getTopics(topic_infos);
    for (ros::master::TopicInfo topic_info: topic_infos)
    {
        topic_advertised.append( QString( topic_info.name.c_str() )  );
    }
    ui->listTopics->addItems( topic_advertised );
}

void RosTopicSelector::on_buttonDisconnect_pressed()
{
    ui->listTopics->clear();
    ui->buttonConnect->setEnabled(true);
    ui->buttonDisconnect->setEnabled(false);
}

void RosTopicSelector::on_checkBoxEnvironmentSettings_toggled(bool checked)
{
    ui->lineEditMasterURI->setEnabled( !checked );
    ui->lineEditHostIP->setEnabled( !checked );
}

void RosTopicSelector::on_listTopics_itemSelectionChanged()
{
    QModelIndexList indexes = ui->listTopics->selectionModel()->selectedIndexes();
    ui->buttonBox->setEnabled( indexes.count() > 0);
}

void RosTopicSelector::on_buttonBox_accepted()
{
    QListWidget* list = ui->listTopics;
    QModelIndexList indexes = list->selectionModel()->selectedIndexes();

    _selected_topics.clear();
    foreach(QModelIndex index, indexes)
    {
        _selected_topics .append(  index.data(Qt::DisplayRole ).toString() );
    }
    this->accept();
}



QString RosTopicSelector::getLoadedRules() const
{
    return _rule_widget->getLoadedRules();
}
