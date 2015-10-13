#ifndef DBRECEPIENT_H
#define DBRECEPIENT_H

#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QString>

#include "recepient.h"

class DbRecepient
{
public:
    DbRecepient();
    static bool createDb();
    static bool insert(const Recepient recepient);
    static bool update(const Recepient recepient);
    static bool saveToFile(QSqlDatabase memdb, QString filename, bool save);
    static int countEnableRecepient();
    static QVector<Recepient> getEnabledRecepient();
};

#endif // DBRECEPIENT_H
