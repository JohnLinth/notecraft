#ifndef NOTIZEN_BACKEND_H
#define NOTIZEN_BACKEND_H
#include <vector>
#include <iostream>
#include <QString>
#include "notizenelement.h"

using namespace std;

class Notizen_Backend
{
public:
    Notizen_Backend(string name);
    void notizenAnlegen(QString titel, QString inhalt);
    QString notizenAusgeben(int index);
    QString titelAusgeben(int index);
    int anzahlNotizenImVector();
    int getIdVonNotiz(int index);
    void vorhandeneNotizBearbeiten(int id, QString titel, QString notiz);
    void importNotizen();
    string exportNotizenAusgabe();
    string getName() const;
    void setName(const string &newName);

private:
    vector<NotizenElement> alleNotizen;
    string name;
};

#endif // NOTIZEN_BACKEND_H
