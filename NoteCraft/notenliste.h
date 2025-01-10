#ifndef NOTENLISTE_H
#define NOTENLISTE_H

#include <QDialog>
#include <QResizeEvent>
#include "kurs.h"
#include <vector>
using std::vector;

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include<string>
using std::string;
#include <map>
using std::map;
using std::pair;
#include <QResizeEvent>
#include <QMargins>
#include <QFrame>
#include <random>
#include "qdynamicbutton.h"
#include <algorithm>

struct Module{
    string name;
    double grade;
    double ects;
    vector<double> noten;
};


struct User{
    vector<Module> myModule;
    vector<Module> gradesForOtherModule;

    void initModule(){
        myModule.push_back(Module{"Betriebssysteme", 2.7, 5.0, {2.7}});
        myModule.push_back(Module{"Programmieren 2", 2.3, 7.5, {2.3}});
        myModule.push_back(Module{"It Sicherheit", 2.3, 5.0, {2.3}});
        myModule.push_back(Module{"Informatik und Gesellschaft", 3.0, 2.5, {3.0}});
        myModule.push_back(Module{"Rechnernetze", 2.0, 5.0, {2.0}});
        myModule.push_back(Module{"Mathematik für Informatiker 2", 2.7, 5.0, {2.7}});
        myModule.push_back(Module{"Rechnerarchitektur", 3.7, 5.0, {3.7}});

        double randomNoten[] = {1.0, 1.3, 1.7, 2.0, 2.3, 2.7, 3.0, 3.3, 3.7, 4.0, 5.0};

        int lower_bound = 0;
        int upper_bound = 10;

        std::uniform_int_distribution<int> unif(lower_bound, upper_bound);
        std::default_random_engine re;

        for(auto &m : myModule){
            for(int i = 0; i < 50; i++){
                int randomGradeIndex = unif(re);
                double grade = randomNoten[randomGradeIndex];
                m.noten.push_back(grade);
            }
            std::sort(m.noten.begin(), m.noten.end());
            for(auto mn : m.noten){
                qDebug () << m.name << ": " << mn;
            }
        }
    }

    void removeModule(unsigned int index){
        if(index < myModule.size()){
            myModule.erase(myModule.begin() + index);
        }
    }
};



namespace Ui {
class Notenliste;
}

class Notenliste : public QDialog
{
    Q_OBJECT

public:
    explicit Notenliste(QWidget *parent = nullptr);
    ~Notenliste();
    void showList();

    void draw();
    map<string, double> statistikBerechnen(vector<Module> module);
    User *getUser() const;
    void kursErstellen();
    QString getDurchschnitt();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onKursErstellenClicked();
    void onDeleteButtonClicked();
    void onCompareButtonClicked();
signals:
    void aboutToBeClosed();

private:
    Ui::Notenliste *ui;
    User *user;


    QVBoxLayout *kurseListeLayoutV;
    QHBoxLayout *kursLayoutH;
    QVBoxLayout *kursInfoLayoutV;

    QLabel *durchschnittL;
    QLabel *ectsL;
    QLabel *besteNoteL;
    QLabel *schlechtesteNoteL;

    QFrame *statistik_rahmen;
    QFrame *rahmen;

    map<string, double> statistic;
};

#endif // NOTENLISTE_H
