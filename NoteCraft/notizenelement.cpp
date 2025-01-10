#include "notizenelement.h"


int NotizenElement::nextId=0;


NotizenElement::NotizenElement(QString titel, QString notiz): titel(titel), notizenImElement(notiz)
{
    id = nextId++;

}

QString NotizenElement::getTitel() const
{
    return titel;
}

void NotizenElement::setTitel(const QString &newTitel)
{
    titel = newTitel;
}

QString NotizenElement::getNotizenImElement() const
{
    return notizenImElement;
}

void NotizenElement::setNotizenImElement(const QString &newNotizenImElement)
{
    notizenImElement = newNotizenImElement;
}

int NotizenElement::getId() const
{
    return id;
}

void NotizenElement::setId(int newId)
{
    id = newId;
}
