#include "setting.h"
#include <QDebug>


Setting::Setting(QObject *parent) : QObject(parent)
{

    pathTemplateNoticeFront.clear();
    pathTemplateNoticeBack.clear();
    pathTemplateEnvelope.clear();
    duplexPrintNotice = true;
    settings = new QSettings( (QCoreApplication::applicationDirPath()) + "//setting.ini",QSettings::IniFormat );
}


/**
 * Сохранение настроек
 *
 * @brief Setting::saveSetting
 */
void Setting::saveSetting()
{
    settings->beginGroup("setting");
        settings->setValue( "pathTemplateNoticeFront", pathTemplateNoticeFront );
        settings->setValue( "pathTemplateNoticeBack",  pathTemplateNoticeBack  );
        settings->setValue( "duplexPrintNotice",       duplexPrintNotice       );
        settings->setValue( "fontNotice",              fontNotice              );

        settings->setValue( "pathTemplateEnvelope", pathTemplateEnvelope );
        settings->setValue( "fontEnvelope",         fontEnvelope         );
    settings->endGroup();
    settings->sync();
}


/**
 * Сохранение настроек
 *
 * @brief Setting::saveSetting
 */
void Setting::loadSetting()
{
    settings->beginGroup("setting");    
        pathTemplateNoticeFront   = settings->value( "pathTemplateNoticeFront", "" ).toString();
        pathTemplateNoticeBack    = settings->value( "pathTemplateNoticeBack",  "" ).toString();
        fontNotice                = settings->value( "fontNotice",   QFont("Arial")).value<QFont>();
        duplexPrintNotice         = settings->value( "duplexPrintNotice", true ).toBool();
        positionNameNoticeBack    = settings->value( "positionNameNoticeBack",    QPointF(20,50)).toPointF();
        positionAddressNoticeBack = settings->value( "positionAddressNoticeBack", QPointF(20,50)).toPointF();
        positionNumberNoticeBack  = settings->value( "positionNumberNoticeBack",  QPointF(20,80)).toPointF();

        pathTemplateEnvelope    = settings->value( "pathTemplateEnvelope",    "" ).toString();
        fontEnvelope            = settings->value( "fontEnvelope",   QFont("Arial")).value<QFont>();
        positionNameEnvelope    = settings->value( "positionNameEnvelope",    QPointF(20,20)).toPointF();
        positionAddressEnvelope = settings->value( "positionAddressEnvelope", QPointF(20,50)).toPointF();
    settings->endGroup();
}


/**
 * Сохранить позиции элементов в конверте
 *
 * @brief Setting::setPositionEnvelope
 * @param name
 * @param address
 */
void Setting::setPositionEnvelope( QPointF name, QPointF address )
{
    positionNameEnvelope = name;
    positionAddressEnvelope = address;

    settings->beginGroup("setting");
        settings->setValue( "positionNameEnvelope",    positionNameEnvelope    );
        settings->setValue( "positionAddressEnvelope", positionAddressEnvelope );
    settings->endGroup();
    settings->sync();
}


/**
 * Сохранить позиции элементов в извещении
 *
 * @brief Setting::setPositionNotice
 * @param name
 * @param address
 * @param number
 */
void Setting::setPositionNotice( QPointF name, QPointF address, QPointF number )
{
    positionNameNoticeBack    = name;
    positionAddressNoticeBack = address;
    positionNumberNoticeBack  = number;

    settings->beginGroup("setting");
        settings->setValue( "positionNameNoticeBack",    positionNameNoticeBack    );
        settings->setValue( "positionAddressNoticeBack", positionAddressNoticeBack );
        settings->setValue( "positionNumberNoticeBack",  positionNumberNoticeBack  );
    settings->endGroup();
    settings->sync();
}

