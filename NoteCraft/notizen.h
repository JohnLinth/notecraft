#ifndef NOTIZEN_H
#define NOTIZEN_H
#include <QStandardItemModel>
#include <QDialog>
#include "notizen_backend.h"
#include <QItemSelection>

namespace Ui {
class Notizen;
}

class Notizen : public QDialog
{
    Q_OBJECT

public:
    explicit Notizen(QWidget *parent = nullptr);
    ~Notizen();
    void kursnotizenHinzufuegen(string name);
    void initialisiereNotizen(int index);
    int getAnzahlNotizen();
    string getNotizenName(int index);
private:
    void refreshTitelAnzeige(int index);
    void clearEingabeTitelUndNotiz();
    void exportAlleNotizen();
    void importAlleNotizen();
    void refresh(int index);
    void ToolsAddImage();


private slots:
    void onNotizenSpeichernButtonClicked();

    void onRefreshButtonClicked();

    void on_notizenTitelAnzeige_clicked(const QModelIndex &index);

private:
    int aktuellerIndex;
    Ui::Notizen *ui;
    //Notizen_Backend *notizenBackend = new Notizen_Backend();
    vector<Notizen_Backend*> notizenBackend;
    QStandardItemModel *model = new QStandardItemModel;

};

#endif // NOTIZEN_H
