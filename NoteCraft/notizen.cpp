#include "notizen.h"
#include "ui_notizen.h"
#include <QMessageBox>
#include <fstream>
#include <sstream>
#include <QDebug>
#include <QFileDialog>
#include <QImageReader>

Notizen::Notizen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Notizen)
{
    ui->setupUi(this);

    connect(ui->notizenSpeichernButton,&QPushButton::clicked,this, &Notizen::onNotizenSpeichernButtonClicked);
    connect(ui->refreshButton,&QPushButton::clicked,this, &Notizen::onRefreshButtonClicked);
    connect(ui->bildEinfuegenButton, &QPushButton::clicked, this, &Notizen::ToolsAddImage);

    //connect(ui->notizenTitelAnzeige, &QAbstractItemView::clicked, this, &Notizen::refreshTitelAnzeige);
    //ui->notizenTitelAnzeige->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->notizenTitelAnzeige->setModel(model);
    ui->lineEdit->setVisible(false);
    importAlleNotizen();
    ui->titelEdit->setPlaceholderText("Titel...");
    ui->notizenInhalt->setPlaceholderText("Notizen...");
}

Notizen::~Notizen()
{
    delete ui;
}

void Notizen::kursnotizenHinzufuegen(string name)
{
    Notizen_Backend *kursNotizen = new Notizen_Backend(name);
    notizenBackend.push_back(kursNotizen);
}

void Notizen::initialisiereNotizen(int index)
{
    clearEingabeTitelUndNotiz();
    if(index >= (int)notizenBackend.size()){
        QMessageBox::information(this, "Fehler!", "Falscher Index!");
    }
    else{
        aktuellerIndex = index;
        refreshTitelAnzeige(index);
        ui->notizenFensterTitel->setText(QString::fromStdString(notizenBackend[aktuellerIndex]->getName()));
    }
}

void Notizen::refreshTitelAnzeige(int index)
{
    for(int i = 0; i < notizenBackend[index]->anzahlNotizenImVector();i++){
        model->setItem(i,0, new QStandardItem(notizenBackend[index]->titelAusgeben(i)));
    }
    int result = -1;
    QItemSelectionModel *selectionModel = ui->notizenTitelAnzeige->selectionModel();
    QModelIndexList selectetIndexes = selectionModel->selectedIndexes();
    if(!selectetIndexes.isEmpty()){
        QModelIndex selectetIndex = selectetIndexes.first();
        result = selectetIndex.row();
        ui->notizenInhalt->setText(notizenBackend[index]->notizenAusgeben(result));
        ui->lineEdit->setText(QString::number(notizenBackend[index]->getIdVonNotiz(result)));
        ui->titelEdit->setText(notizenBackend[index]->titelAusgeben(result));
    }
}

void Notizen::clearEingabeTitelUndNotiz()
{
    model->clear();
    ui->titelEdit->clear();
    ui->notizenInhalt->clear();
    ui->lineEdit->clear();
}

void Notizen::exportAlleNotizen()
{
    ofstream exportDatei;
    exportDatei.open("alleNotizen.csv", ios::out);
    if(exportDatei.is_open()){
        for(unsigned int i = 0; i < notizenBackend.size();i++){
            exportDatei << "n;" << notizenBackend[i]->getName() << "\n";
            exportDatei << notizenBackend[i]->exportNotizenAusgabe();
            exportDatei << "###\n";
        }
    }
    else{
        //"Datei konnte nicht geoeffnet werden!"
    }
    exportDatei.close();
}

void Notizen::importAlleNotizen()
{
    string muell, ausgelesenerTitel, ausgelesnerInhaltNotiz, nameNotiz;
    int indexKurs = 0, muellInt;
    ifstream importDatei;
    string ausgelesenerString;
    importDatei.open("alleNotizen.csv");
    if(importDatei.is_open()){
        while(getline(importDatei, ausgelesenerString)){
            istringstream puffer(ausgelesenerString);
            if(ausgelesenerString[0] == 'n'){
                if(getline(puffer, muell, ';')&&
                    getline(puffer, nameNotiz,';')){
                }
                kursnotizenHinzufuegen(nameNotiz);
            }
            else if(ausgelesenerString[0]=='e'){

                if(getline(puffer, muell, ';') &&puffer >> muellInt&& getline(puffer, muell, ';') && puffer&&getline(puffer,ausgelesenerTitel, ';')&&getline(puffer, ausgelesnerInhaltNotiz)){
                }
                notizenBackend[indexKurs]->notizenAnlegen(QString::fromStdString(ausgelesenerTitel), QString::fromStdString(ausgelesnerInhaltNotiz));
            }
            else if(ausgelesenerString == "###"){
                indexKurs++;
            }
        }
    }
    else{
        //"Fehler beim Import!"
    }
}

void Notizen::refresh(int index)
{
    ui->notizenInhalt->setText(notizenBackend[aktuellerIndex]->notizenAusgeben(index));
    ui->lineEdit->setText(QString::number(notizenBackend[aktuellerIndex]->getIdVonNotiz(index)));
    ui->titelEdit->setText(notizenBackend[aktuellerIndex]->titelAusgeben(index));
}




void Notizen::onNotizenSpeichernButtonClicked()
{
    if(!ui->lineEdit->text().isEmpty()){
        int notizId = ui->lineEdit->text().toInt();
        notizenBackend[aktuellerIndex]->vorhandeneNotizBearbeiten(notizId, ui->titelEdit->text(), ui->notizenInhalt->toPlainText());
        QString nachrichtFuerMessagebox = ui->titelEdit->text()+ "\nBearbeitung wurde gespeichert";
        QMessageBox::information(this, "Wurde Bearbeitet", nachrichtFuerMessagebox);
    }
    else{
        notizenBackend[aktuellerIndex]->notizenAnlegen(ui->titelEdit->text(), ui->notizenInhalt->toPlainText());
        QString nachrichtFuerMessagebox = ui->titelEdit->text()+ "\nWurde hinzugefügt";
        QMessageBox::information(this, "Gespeichert", nachrichtFuerMessagebox);
    }
    exportAlleNotizen();
    clearEingabeTitelUndNotiz();
    onRefreshButtonClicked();
}


void Notizen::onRefreshButtonClicked()
{
    clearEingabeTitelUndNotiz();
    refreshTitelAnzeige(aktuellerIndex);
}

int Notizen::getAnzahlNotizen()
{
    return notizenBackend.size();
}

string Notizen::getNotizenName(int index)
{
    return notizenBackend[index]->getName();
}

void Notizen::on_notizenTitelAnzeige_clicked(const QModelIndex &index)
{
    int i = index.row();
    refresh(i);
}

 void Notizen::ToolsAddImage()
 {
    QString file = QFileDialog::getOpenFileName(this, tr("Select an image"), ".",
                                                tr("PNG (*.png)\n"
                                                   "Bitmap Files (*.bmp)\n"
                                                   "JPEG (*.jpg *jpeg)\n"
                                                   "GIF (*.gif)\n"));

    QUrl Uri(QString("%1").arg(file));
    QImage image = QImageReader(file).read();

    QTextDocument *textDocument = ui->notizenInhalt->document(); //ui->textEdit->document();
    textDocument->addResource(QTextDocument::ImageResource, Uri, QVariant(image));
    QTextCursor cursor = ui->notizenInhalt->textCursor(); //ui->textEdit->textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setWidth(256);
    imageFormat.setHeight(256);
    //imageFormat.setQuality(10);
    imageFormat.setName(Uri.toString());
    cursor.insertImage(imageFormat);

    string test = ui->notizenInhalt->toHtml().toStdString();
    cout << "TEST";
    cout << test;
 }
