#include "dialogsetting.h"
#include "ui_dialogsetting.h"

#include <QDebug>
#include <QFileDialog>
#include <QFontDialog>


DialogSetting::DialogSetting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetting)
{
    ui->setupUi(this);

    setting = new Setting();

    connect ( ui->pushButtonOk, SIGNAL(clicked()), SLOT(saveSetting()) );
    connect ( ui->pushButtonCancel, SIGNAL(clicked()), SLOT(close()) );
    connect ( ui->listWidget, SIGNAL(clicked(QModelIndex)), SLOT(setIndex(QModelIndex)) );
    // Notice
    connect ( ui->toolButtonTemplateNoticeFront, SIGNAL(clicked()), SLOT(setTemplateNoticeFront()) );
    connect ( ui->toolButtonTemplateNoticeBack, SIGNAL(clicked()), SLOT(setTemplateNoticeBack())  );
    connect ( ui->fontNotice, SIGNAL(clicked()), SLOT(setFontNotice())          );
    connect ( ui->marginAuto, SIGNAL(clicked(bool)), SLOT(setAutoMargin()) );
    // Envelope
    connect ( ui->toolButtonTemplateEnvelope, SIGNAL(clicked()), SLOT(setTemplateEnvelope()) );
    connect ( ui->fontEnvelope, SIGNAL(clicked()), SLOT(setFontEnvelope()) );

    ui->listWidget->setCurrentRow(0);
}

DialogSetting::~DialogSetting()
{
    delete ui;
}


void DialogSetting::showEvent(QShowEvent *event)
{
    if(event->isAccepted())
        loadSetting();
}


void DialogSetting::setIndex(QModelIndex index)
{
    ui->stackedWidget->setCurrentIndex( index.row() );
}


void DialogSetting::setTemplateNoticeFront()
{
    QString fOpen = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "", tr("All support files (*.svg);;") );

    if ( fOpen.isEmpty() )
        return;

    ui->pathTemplateNoticeFront->setText( fOpen );
}


void DialogSetting::setTemplateNoticeBack()
{
    QString fOpen = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "", tr("All support files (*.svg);;") );

    if ( fOpen.isEmpty() )
        return;

    ui->pathTemplateNoticeBack->setText( fOpen );
}


void DialogSetting::setAutoMargin()
{
    if ( ui->marginAuto->isChecked() ) {
        ui->marginFrontTop->setDisabled(true);
        ui->marginFrontTop->setDisabled(true);
        ui->marginBackTop->setDisabled(true);
        ui->marginBackTop->setDisabled(true);
    } else {
        ui->marginFrontTop->setDisabled(false);
        ui->marginFrontTop->setDisabled(false);
        ui->marginBackTop->setDisabled(false);
        ui->marginBackTop->setDisabled(false);
    }
}


void DialogSetting::setTemplateEnvelope()
{
    QString fOpen = QFileDialog::getOpenFileName(this, tr("Открыть файл"), "", tr("All support files (*.svg);;") );

    if ( fOpen.isEmpty() )
        return;

    ui->pathTemplateEnvelope->setText( fOpen );
}


void DialogSetting::saveSetting()
{
    setting->showBoundary = ui->showBoundary->isChecked();
    setting->duplexPrintNotice       = ui->duplexPrintNotice->isChecked();
    setting->pathTemplateEnvelope    = ui->pathTemplateEnvelope->text();
    setting->pathTemplateNoticeFront = ui->pathTemplateNoticeFront->text();
    setting->pathTemplateNoticeBack  = ui->pathTemplateNoticeBack->text();

    setting->autoMargin = ui->marginAuto->isChecked();
    setting->marginFrontTop  = ui->marginFrontTop->value();
    setting->marginFrontLeft = ui->marginFrontLeft->value();
    setting->marginBackTop   = ui->marginBackTop->value();
    setting->marginBackLeft  = ui->marginBackLeft->value();

    setting->saveSetting();
    emit settingSaved();
    close();
}


void DialogSetting::showSettingNotice()
{
    show();
    ui->listWidget->setCurrentRow(0);
    ui->stackedWidget->setCurrentIndex(0);
}


void DialogSetting::showSettingEnvelope()
{
    show();
    ui->listWidget->setCurrentRow(1);
    ui->stackedWidget->setCurrentIndex(1);
}


void DialogSetting::loadSetting()
{
    setting->loadSetting();

    ui->pathTemplateEnvelope->setText(setting->pathTemplateEnvelope);
    ui->pathTemplateNoticeFront->setText(setting->pathTemplateNoticeFront);
    ui->pathTemplateNoticeBack->setText(setting->pathTemplateNoticeBack);
    ui->duplexPrintNotice->setChecked(setting->duplexPrintNotice);
    ui->showBoundary->setChecked(setting->showBoundary);

    ui->marginAuto->setChecked( setting->autoMargin );
    ui->marginFrontTop->setValue( setting->marginFrontTop );
    ui->marginFrontLeft->setValue( setting->marginFrontLeft );
    ui->marginBackTop->setValue( setting->marginBackTop );
    ui->marginBackLeft->setValue( setting->marginBackLeft );
}


void DialogSetting::setFontEnvelope()
{
    bool bOk;
    QFont fontEnvelope;
    fontEnvelope = QFontDialog::getFont(&bOk, setting->fontEnvelope, 0, "Шрифт для конверта");

    if(bOk)
        setting->fontEnvelope = fontEnvelope;
}


void DialogSetting::setFontNotice()
{
    bool bOk;
    QFont fontNotice;
    fontNotice = QFontDialog::getFont(&bOk, setting->fontNotice, 0, "Шрифт для конверта");

    if(bOk)
        setting->fontNotice = fontNotice;
}

