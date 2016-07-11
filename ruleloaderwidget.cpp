#include <QFile>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include "ruleloaderwidget.h"
#include "ui_ruleloaderwidget.h"

RuleLoaderWidget::RuleLoaderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RuleLoaderWidget)
{
    ui->setupUi(this);

    QSettings settings( "IcarusTechnology", "PlotJuggler");

    if( settings.contains( "RuleLoaderWidget.previouslyLoadedRules" ) )
    {
        QString fileName = settings.value( "RuleLoaderWidget.previouslyLoadedRules" ).toString();
        // check if it exist
        QFile file(fileName);

        if( file.exists() )
        {
            readRuleFile( file );
        }
        else{
            ui->labelLoadedRules->setText("none");
        }
    }

}

RuleLoaderWidget::~RuleLoaderWidget()
{
    QSettings settings( "IcarusTechnology", "PlotJuggler");
    settings.setValue(  "RuleLoaderWidget.enableRules" , ui->checkBoxEnableSubstitution->isChecked() );

    delete ui;
}

QString RuleLoaderWidget::getLoadedRules() const
{
    if( ui->checkBoxEnableSubstitution->isChecked() )
        return _loaded_rules;
    else
        return QString();
}

void RuleLoaderWidget::on_pushButtonLoadOther_pressed()
{
    QSettings settings( "IcarusTechnology", "PlotJuggler");

    QString directory_path = QDir::currentPath();
    QString fileName;

    if( settings.contains( "RuleLoaderWidget.previouslyLoadedRules" ) )
    {
        fileName = settings.value( "RuleLoaderWidget.previouslyLoadedRules" ).toString();
        QFileInfo info( fileName );
        directory_path = info.absoluteFilePath();
    }

    fileName = QFileDialog::getOpenFileName(this, "Open Layout", directory_path, "*.txt");

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    readRuleFile( file );
}

void RuleLoaderWidget::on_checkBoxEnableSubstitution_toggled(bool checked)
{
    ui->pushButtonLoadOther->setEnabled( checked );
}

void RuleLoaderWidget::readRuleFile(QFile &file)
{
    if ( file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&file);
        this->_loaded_rules = in.readAll();

        QSettings settings( "IcarusTechnology", "PlotJuggler");
        settings.setValue( "RuleLoaderWidget.previouslyLoadedRules", file.fileName() );

        ui->labelLoadedRules->setText( file.fileName() );
        file.close();
    }
    else{
        QMessageBox::warning(0, tr("Warning"), tr("Can't open file\n") );
        ui->labelLoadedRules->setText("none");
    }
}
