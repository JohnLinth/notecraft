#ifndef NOTIZENELEMENT_H
#define NOTIZENELEMENT_H

#include <qstring.h>
#include <iostream>

using namespace std;


class NotizenElement
{
public:
    NotizenElement(QString titel, QString notiz);

    QString getTitel() const;
    void setTitel(const QString &newTitel);

    QString getNotizenImElement() const;
    void setNotizenImElement(const QString &newNotizenImElement);

    int getId() const;
    void setId(int newId);

private:
    QString titel;
    QString notizenImElement;
    int id;
    static int nextId;
};

#endif // NOTIZENELEMENT_H
