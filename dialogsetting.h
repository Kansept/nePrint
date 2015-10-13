#ifndef DIALOGSETTING_H
#define DIALOGSETTING_H

#include <QDialog>
#include <QModelIndex>
#include <QShowEvent>

#include "setting.h"

namespace Ui {
class DialogSetting;
}

class DialogSetting : public QDialog
{
    Q_OBJECT

    Setting *setting;

public:
    explicit DialogSetting(QWidget *parent = 0);
    ~DialogSetting();

public slots:
    void setIndex(QModelIndex index);
    void saveSetting();


    void setTemplateNoticeFront();
    void setTemplateNoticeBack();
    void setTemplateEnvelope();

    void loadSetting();
    void setFontEnvelope();
    void setFontNotice();
    void showSettingEnvelope();
    void showSettingNotice();
private:
    Ui::DialogSetting *ui;

signals:
    void settingSaved();
protected:
    void showEvent(QShowEvent *event);
};


#endif // DIALOGSETTING_H
