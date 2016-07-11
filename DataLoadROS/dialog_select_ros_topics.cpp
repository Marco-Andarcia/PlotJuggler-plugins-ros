#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QFileDialog>
#include <QListView>
#include <QListWidgetItem>
#include "dialog_select_ros_topics.h"
#include "ui_dialog_select_ros_topics.h"



DialogSelectRosTopics::DialogSelectRosTopics(QStringList topic_list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dialogSelectRosTopics)
{
    auto flags = this->windowFlags();
    this->setWindowFlags( flags | Qt::WindowStaysOnTopHint);

    ui->setupUi(this);

    for (int i=0; i< topic_list.size(); i++) {
        ui->listRosTopics->addItem( new QListWidgetItem( topic_list[i] ) );
    }

    _rule_widget = new RuleLoaderWidget(this);

    ui->frame->layout()->addWidget( _rule_widget );

}

DialogSelectRosTopics::~DialogSelectRosTopics()
{
    delete ui;
}

QStringList DialogSelectRosTopics::getSelectedItems()
{
    return _topic_list;
}

QString DialogSelectRosTopics::getLoadedRules() const
{
    return _rule_widget->getLoadedRules();
}


void DialogSelectRosTopics::on_buttonBox_accepted()
{
    QModelIndexList indexes = ui->listRosTopics->selectionModel()->selectedIndexes();

    foreach(QModelIndex index, indexes)
    {
        _topic_list.push_back( index.data(Qt::DisplayRole).toString() );
    }
}

void DialogSelectRosTopics::on_listRosTopics_itemSelectionChanged()
{
    QModelIndexList indexes = ui->listRosTopics->selectionModel()->selectedIndexes();

    ui->buttonBox->setEnabled( indexes.size() > 0) ;
}

