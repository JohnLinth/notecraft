#include "notizen_backend.h"
#include <iostream>
#include <QString>
#include "notizenelement.h"
#include <fstream>



using namespace std;




Notizen_Backend::Notizen_Backend(string name):name(name)
{

}

void Notizen_Backend::notizenAnlegen(QString titel, QString inhalt)
{
    NotizenElement neueNotiz(titel,inhalt);
    alleNotizen.push_back(neueNotiz);
}

QString Notizen_Backend::notizenAusgeben(int index)
{
    return alleNotizen[index].getNotizenImElement();
}

QString Notizen_Backend::titelAusgeben(int index)
{
    return alleNotizen[index].getTitel();
}

int Notizen_Backend::anzahlNotizenImVector()
{
    return alleNotizen.size();
}

int Notizen_Backend::getIdVonNotiz(int index)
{
    return alleNotizen[index].getId();
}

void Notizen_Backend::vorhandeneNotizBearbeiten(int id, QString titel, QString notiz)
{
    for( unsigned int i = 0; i < alleNotizen.size();i++){
        if(alleNotizen[i].getId()== id){
            alleNotizen[i].setTitel(titel);
            alleNotizen[i].setNotizenImElement(notiz);
        }
    }
}

string Notizen_Backend::exportNotizenAusgabe()
{
    string exportDateiString;

    for(unsigned int i = 0; i < alleNotizen.size(); i++){
        exportDateiString += "e;";
        exportDateiString += to_string(alleNotizen[i].getId()) + ";";
        exportDateiString += alleNotizen[i].getTitel().toStdString() + ";";
        exportDateiString += alleNotizen[i].getNotizenImElement().toStdString() + "\n";
    }
    return exportDateiString;

}

string Notizen_Backend::getName() const
{
    return name;
}

void Notizen_Backend::setName(const string &newName)
{
    name = newName;
}

