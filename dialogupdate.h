#ifndef DIALOGUPDATE_H
#define DIALOGUPDATE_H

#include <QDialog>

namespace Ui {
class DialogUpdate;
}

class DialogUpdate : public QDialog
{
    Q_OBJECT

public:
    explicit DialogUpdate(QWidget *parent = 0);
    ~DialogUpdate();

public slots:
    static bool checkNewVersion();
    void updateApp();

private:
    Ui::DialogUpdate *ui;

protected:
    void showEvent(QShowEvent *event);
};

#endif // DIALOGUPDATE_H
