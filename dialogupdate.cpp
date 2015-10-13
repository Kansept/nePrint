#include <QProcess>
#include <QDebug>
#include <QShowEvent>

#include "dialogupdate.h"
#include "ui_dialogupdate.h"

DialogUpdate::DialogUpdate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogUpdate)
{
    ui->setupUi(this);

    connect( ui->close, SIGNAL(clicked()), SLOT(close()) );
    connect( ui->update, SIGNAL(clicked()), SLOT(updateApp()) );
}


DialogUpdate::~DialogUpdate()
{
    delete ui;
}


void DialogUpdate::updateApp()
{
    QProcess *myProcess = new QProcess();
    QByteArray output;
    output.append("");
    myProcess->start(QCoreApplication::applicationDirPath() + "//UpdateQPrint.exe",
                       QStringList() << "-update" << "C://QPrint//");
    output.append( myProcess->readAllStandardOutput() );
    myProcess->waitForFinished();
    ui->textEdit->setText(output);

    if ( QProcess::startDetached(QString("\"") + QApplication::applicationFilePath() + "\"") )
      QApplication::quit();
}


bool DialogUpdate::checkNewVersion()
{
    bool newVersion;
    newVersion = false;

    QProcess *myProcess = new QProcess();
    QByteArray output;
    output.append("");
    myProcess->start(QCoreApplication::applicationDirPath() + "//updater.exe",
                       QStringList() << "-check" << "C://QPrint//");
    myProcess->waitForFinished();
    output.append( myProcess->readAllStandardOutput() );

    if ( output == "1" ) {
        newVersion = true;
    } else {
        newVersion = false;
    }
    return newVersion;
}


void DialogUpdate::showEvent(QShowEvent *event)
{
    if ( event->isAccepted() )
    {
        if ( checkNewVersion() == true ) {
            ui->title->setText("Доступна новая версия!");
        } else {
            ui->title->setText("Обновлений нет!");
        }
    }
}
