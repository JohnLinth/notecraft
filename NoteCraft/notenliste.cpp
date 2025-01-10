#include "notenliste.h"
#include "ui_notenliste.h"
#include <QString>
#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QLayoutItem>
#include <QMessageBox>
#include <QDialog>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QMap>
#include <QStringList>
#include <QColor>
#include <QTimer>


Notenliste::Notenliste(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Notenliste),  user(new User())
{
    qDebug() << "hier wird das Fenster gezeichnet";

    user->initModule();

    ui->setupUi(this);
    this->setFixedSize(778, 620);


    this->resize(800, 600);

    statistic = this->statistikBerechnen(user->myModule);


    this->showList();

    auto *vertical_statistik = new QVBoxLayout;
    statistik_rahmen = new QFrame();
    statistik_rahmen->setFrameStyle(QFrame::Box | QFrame::Raised);
    statistik_rahmen->setLayout(vertical_statistik);

    ui->statistik_layout->addWidget(statistik_rahmen, 0, 0);

    qDebug() << ui->statistik_layout->sizeHint();


    durchschnittL = new QLabel("Durchschnitt: " + QString::number(statistic["durchschnitt"]));
    durchschnittL->setFrameStyle(QFrame::Box | QFrame::Raised);

    ectsL = new QLabel("Gesamt ECTS: " + QString::number(statistic["ects"]));
    ectsL->setFrameStyle(QFrame::Box | QFrame::Raised);

    besteNoteL = new QLabel("Beste Note: " + QString::number(statistic["bBewertung"]));
    besteNoteL->setFrameStyle(QFrame::Box | QFrame::Raised);

    schlechtesteNoteL = new QLabel("Schlechteste Note: " + QString::number(statistic["sBewertung"]));
    schlechtesteNoteL->setFrameStyle(QFrame::Box | QFrame::Raised);


    vertical_statistik->addWidget(durchschnittL);
    vertical_statistik->addWidget(ectsL);
    vertical_statistik->addWidget(besteNoteL);
    vertical_statistik->addWidget(schlechtesteNoteL);

    this->kursErstellen();

    ui->vergleichButton->setText("Notenstatistik");
    connect(ui->vergleichButton, &QPushButton::clicked, this, &Notenliste::onCompareButtonClicked);
}

Notenliste::~Notenliste()
{
    delete ui;
    delete user;

    delete durchschnittL;
    delete ectsL;
    delete besteNoteL;
    delete schlechtesteNoteL;

    delete statistik_rahmen;

    delete kursInfoLayoutV;
    delete kursLayoutH;
    delete kurseListeLayoutV;

    delete rahmen;
    //emit aboutToBeDestroyed();
}

void Notenliste::showList()
{
    ui->scrollArea->setWidgetResizable(true);

    QWidget *contentWidget = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout();
    contentWidget->setLayout(scrollLayout);

    int row = 0;



    QDynamicButton::resetID();

    for(const auto& m : user->myModule){
        rahmen = new QFrame();
        rahmen->setFrameStyle(QFrame::Box | QFrame::Raised);

        kurseListeLayoutV = new QVBoxLayout();
        rahmen->setLayout(kurseListeLayoutV);
        kursLayoutH = new QHBoxLayout();


        QFont font;
        font.setPointSize(12);
        font.setBold(true);

        auto *kursNameLabel = new QLabel();
        kursNameLabel->setFont(font);
        kursNameLabel->setText(QString::fromStdString(m.name));
        kursInfoLayoutV = new QVBoxLayout();
        auto *ects_label = new QLabel(QString::number(m.ects) + " ECTS");
        ects_label->setAlignment(Qt::AlignRight);

        QLabel *note_label;
        if(m.grade <= 4.0 && m.grade > 0)
            note_label = new QLabel("Note: " + QString::number(m.grade) + " - bestanden");
        else if(m.grade > 4.0)
            note_label = new QLabel("Note: " + QString::number(m.grade) + " - nicht bestanden");
        else
            note_label = new QLabel("Note: - ");

        QDynamicButton *deleteButton = new QDynamicButton();
        deleteButton->setText("Kurs löschen");
        deleteButton->setMaximumWidth(120);
        qDebug() << "Button ID: " << deleteButton->getID();
        connect(deleteButton, &QDynamicButton::clicked, this, &Notenliste::onDeleteButtonClicked);


        note_label->setAlignment(Qt::AlignRight);
        ui->kurse_layout->addWidget(rahmen, row, 0);
        scrollLayout->addWidget(rahmen);
        kurseListeLayoutV->addLayout(kursLayoutH);
        kursLayoutH->addWidget(kursNameLabel);
        kursLayoutH->addLayout(kursInfoLayoutV);
        kursInfoLayoutV->addWidget(ects_label);
        kursInfoLayoutV->addWidget(note_label);
        kursInfoLayoutV->addWidget(deleteButton);
        row++;

    }

    ui->scrollArea->setWidget(contentWidget);
}


void Notenliste::onKursErstellenClicked()
{
    string name;
    double bewertung;
    double ects;
    bool isValidInput;

    name = ui->nameLineEdit->text().toStdString();
    if(!name.empty()){
        isValidInput = true;
    }

    bool ectsInputOk;
    QString ectsText = ui->ectsLineEdit->text();
    ects = ectsText.toDouble(&ectsInputOk);
    if(!ectsInputOk || ectsText.contains(',')){
        isValidInput = false;
    }

    QMessageBox msg;
    bool bewertungInputOk;
    QString bewertungsText = ui->bewertungLineEdit->text();
    bewertung = bewertungsText.toDouble(&bewertungInputOk);
    if(!bewertungInputOk || bewertungsText.contains(',') ||
        (bewertung < 1.0 || bewertung > 5.0)){
        isValidInput = false;
    }


    if(isValidInput){
        user->myModule.push_back({name, bewertung, ects, {bewertung}});



        Module& m = user->myModule[user->myModule.size() - 1];

        double randomNoten[] = {1.0, 1.3, 1.7, 2.0, 2.3, 2.7, 3.0, 3.3, 3.7, 4.0, 5.0};

        int lower_bound = 0;
        int upper_bound = 10;

        std::uniform_int_distribution<int> unif(lower_bound, upper_bound);
        std::default_random_engine re;

        for(int i = 0; i < 50; i++){
            int randomGradeIndex = unif(re);
            double grade = randomNoten[randomGradeIndex];
            m.noten.push_back(grade);
        }
        std::sort(m.noten.begin(), m.noten.end());
        for(auto mn : m.noten){
            qDebug () << m.name << ": " << mn;
        }

        msg.setText("Kurs erfolgreich hinzugefügt");
        msg.show();
        msg.exec();
        this->draw();
    } else {
        msg.setText("Ungültige oder fehldende Eingaben");
        msg.show();
        msg.exec();
    }
}

void Notenliste::onDeleteButtonClicked()
{
    QDynamicButton *clickedButton = qobject_cast<QDynamicButton *>(sender());
    if (clickedButton) {
        int buttonID = clickedButton->getID();
        qDebug() << "Clicked Button ID: " << buttonID;

        user->removeModule(buttonID - 1);

        QMessageBox msg;
        msg.setText("Kurs erfolgreich gelöscht");
        msg.show();
        msg.exec();

        this->draw();
    }
}

void Notenliste::onCompareButtonClicked()
{
    QDialog *dialog = new QDialog();

    this->hide();

    dialog->setFixedSize(700, 600);

    QVBoxLayout * mainLayout = new QVBoxLayout(dialog);

    QScrollArea * scrollArea = new QScrollArea(dialog);
    QWidget * scrollContent = new QWidget();
    QVBoxLayout * layoutV = new QVBoxLayout(scrollContent);


    for(auto m : user->myModule){
        QFont font;
        font.setBold(true);
        font.setPointSize(20);

        QLabel * kurs = new QLabel(QString::fromStdString(m.name));
        kurs->setFont(font);

        layoutV->addWidget(kurs);

        QPieSeries *series = new QPieSeries();
        QMap<double, int> notenMap;

        for(double value: m.noten){
            notenMap[value]++;
        }

        for(auto it = notenMap.begin(); it != notenMap.end(); ++it){
            QPieSlice * slice = series->append(QString::number(it.key()), it.value());

            if(m.grade == it.key()){
                slice->setLabel(QString("%1 : %2").arg(it.key()).arg(it.value()));

                QTimer *timer = new QTimer(slice);
                timer->setInterval(500);
                bool labelVisible = true;

                connect(timer, &QTimer::timeout, [=]() mutable{
                    if(labelVisible){
                        slice->setLabelVisible(false);
                        labelVisible = false;
                    } else {
                        slice->setLabelVisible(true);
                        labelVisible = true;
                    }
                });

                timer->start();

            } else {
                slice->setLabel(QString("%1 : %2").arg(it.key()).arg(it.value()));
                slice->setLabelVisible(true);
            }
        }

        QStringList colorNames = QColor::colorNames();
        int colorIndex = 0;

        for(auto *pieSlice : series->slices()){
            pieSlice->setColor(QColor(colorNames[colorIndex % colorNames.size()]));
            colorIndex++;
        }

        QChartView * chartview = new QChartView();
        chartview->setRenderHint(QPainter::Antialiasing);

        QChart* chart = new QChart();
        chart->addSeries(series);
        chart->setTitle("Kuchendiagram " + QString::fromStdString(m.name));

        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignRight);

        chartview->setChart(chart);

        layoutV->addWidget(chartview);
    }


    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    dialog->show();
    dialog->exec();
    this->show();
}



User *Notenliste::getUser() const
{
    return user;
}

void Notenliste::kursErstellen()
{
    ui->nameLineEdit->setPlaceholderText("Name des Kurses:");

    ui->bewertungLineEdit->setPlaceholderText("Bewertung des Kurses:");

    ui->ectsLineEdit->setPlaceholderText("ECTS des Kurses:");

    connect(ui->hinzufuegenButton, &QPushButton::clicked, this, &Notenliste::onKursErstellenClicked);
}






void Notenliste::draw()
{
    qDebug() << "hier wird das Fenster gezeichnet";

    statistic = this->statistikBerechnen(user->myModule);

    this->showList();

    durchschnittL->setText("Durchschnitt: " + QString::number(statistic["durchschnitt"]));
    durchschnittL->setScaledContents(true);

    ectsL->setText("Gesamt ECTS: " + QString::number(statistic["ects"]));

    besteNoteL->setText("Beste Note: " + QString::number(statistic["bBewertung"]));

    schlechtesteNoteL->setText("Schlechteste Note: " + QString::number(statistic["sBewertung"]));

}

map<string, double> Notenliste::statistikBerechnen(vector<Module> module)
{
    map<string, double> result;

    double summe = 0;
    for(const auto& k : module){
        summe += k.grade;
    }

    double durchschnitt = summe / user->myModule.size();

    result.insert(pair<string, double>("durchschnitt", durchschnitt));

    double ges_ects = 0.0;
    for(const auto& k : user->myModule){
        ges_ects += k.ects;
    }

    result.insert(pair<string, double>("ects", ges_ects));

    double schlechtesteBewertung = 1.0;
    double besteBewertung = 5.0;



    for(const auto& k : user->myModule){
        if(k.grade > schlechtesteBewertung){
            schlechtesteBewertung = k.grade;
        }
        if(k.grade < besteBewertung){
            besteBewertung = k.grade;
        }
    }

    result.insert(pair<string, double>("sBewertung", schlechtesteBewertung));
    result.insert(pair<string, double>("bBewertung", besteBewertung));

    emit aboutToBeClosed();
    return result;
}

QString Notenliste::getDurchschnitt(){
    return QString::number(statistic["durchschnitt"]);
}

void Notenliste::closeEvent(QCloseEvent *event){
    emit aboutToBeClosed();
    event->accept();
}

