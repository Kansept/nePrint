#ifndef DIALOGRECEPIENT_H
#define DIALOGRECEPIENT_H

#include <QDialog>
#include <QShowEvent>

#include "recepient.h"

namespace Ui {
class DialogRecepient;
}

class DialogRecepient : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRecepient(QWidget *parent = 0);
    ~DialogRecepient();

    void add(Recepient sendRecepient);

protected:
    void showEvent(QShowEvent *event);

private slots:
    void save();

private:
    Ui::DialogRecepient *ui;
    Recepient *recepient;

    void clearUI();

signals:
    void sendRecepient(Recepient recepient);
};

#endif // DIALOGRECEPIENT_H
