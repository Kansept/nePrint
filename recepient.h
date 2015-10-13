#ifndef RECEPIENT_H
#define RECEPIENT_H

#include <QString>

class Recepient
{

protected:
    int id;
    QString name;
    QString address;
    QString number;

public:
    Recepient();

    void setId(const int newId);
    void setName(const QString newName);
    void setAddress(const QString newAddress);
    void setNumber(const QString newNumber);

    int getId();
    QString getAddress();
    QString getName();
    QString getNumber();

};

#endif // RECEPIENT_H
