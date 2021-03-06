#ifndef SETTING_H
#define SETTING_H

#include <QObject>
#include <QSettings>
#include <QCoreApplication>
#include <QPointF>
#include <QFont>
#include <QSizeF>


class Setting : public QObject
{
    Q_OBJECT
public:
    explicit Setting(QObject *parent = 0);

    QSettings *settings;

    QString pathTemplateEnvelope;
    QPointF positionNameEnvelope;
    QPointF positionAddressEnvelope;
    QFont fontEnvelope;
    QSizeF sizeEnvelope;

    QString pathTemplateNoticeFront;
    QString pathTemplateNoticeBack;
    QPointF positionNameNoticeBack;
    QPointF positionAddressNoticeBack;
    QPointF positionNumberNoticeBack;
    bool duplexPrintNotice;
    bool showBoundary;
    bool autoMargin;
    int marginFrontTop;
    int marginFrontLeft;
    int marginBackTop;
    int marginBackLeft;
    QFont fontNotice;

public slots:

    void saveSetting();
    void loadSetting();
    void setPositionEnvelope(QPointF name, QPointF address);
    void setPositionNotice(QPointF name, QPointF address, QPointF number);
};

#endif // SETTING_H
