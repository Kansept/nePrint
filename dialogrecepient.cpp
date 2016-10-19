#include <QDebug>

#include "dialogrecepient.h"
#include "ui_dialogrecepient.h"


DialogRecepient::DialogRecepient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRecepient)
{
    ui->setupUi(this);

    recepient = new Recepient();

    connect ( ui->save, SIGNAL(clicked()), SLOT(save()) );
    connect ( ui->cancel, SIGNAL(clicked()), SLOT(close()) );
}


DialogRecepient::~DialogRecepient()
{
    delete ui;
}


void DialogRecepient::showEvent(QShowEvent *event)
{
    if(event->isAccepted())
        clearUI();
}


void DialogRecepient::add(Recepient sendRecepient)
{
    recepient->setId( sendRecepient.getId() );
    recepient->setName( sendRecepient.getName() );
    recepient->setAddress( sendRecepient.getAddress() );
    recepient->setNumber( sendRecepient.getNumber() );

    ui->name->setText(recepient->getName());
    ui->address->setText(recepient->getAddress());
    ui->number->setText(recepient->getNumber());
}


void DialogRecepient::save()
{
    recepient->setName( ui->name->text() );
    recepient->setAddress( ui->address->toPlainText() );
    recepient->setNumber( ui->number->text() );

    sendRecepient(*recepient);
    close();

}


void DialogRecepient::clearUI()
{
    ui->name->clear();
    ui->address->clear();
    ui->number->clear();
}
