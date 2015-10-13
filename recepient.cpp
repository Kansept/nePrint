#include "recepient.h"

Recepient::Recepient()
{
    id = -1;
    name.clear();
    address.clear();
    number.clear();
}


void Recepient::setName(const QString newName)
{
    name = newName;
}
QString Recepient::getName()
{
    return name;
}


void Recepient::setAddress(const QString newAddress)
{
    address = newAddress;
}
QString Recepient::getAddress()
{
    return address;
}


void Recepient::setNumber(const QString newNumber)
{
    number = newNumber;
}
QString Recepient::getNumber()
{
    return number;
}


void Recepient::setId(const int newId)
{
    id = newId;
}
int Recepient::getId()
{
    return id;
}
