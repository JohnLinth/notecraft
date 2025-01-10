#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QMainWindow>
#include <QProgressBar>

#include "Quiz/quiz.h"
#include "kalender.h"
#include "notenliste.h"
#include "notizen.h"
#include "notizen_backend.h"
#include "qdynamicbutton.h"

#include "customtoolbutton.h"
#include <QMenu>
#include <QToolBar>

QT_BEGIN_NAMESPACE
namespace Ui { class HomePage; }
QT_END_NAMESPACE

class HomePage : public QMainWindow
{
    Q_OBJECT

public:
    HomePage(QWidget *parent = nullptr);
    ~HomePage();

    // action menu functions
    void openCalendar();
    void openNotizen();
    void openQuiz();
    void openNotenliste();
    void changeBgImg();
    void setupRecentNotizen();

    // test functions
    void fillNoteList();

    // calendar functions
    void updateCalendarTable();
    void loadCalendarData();

    //dropdown notes menu function
    void initNotizMenu();
    int findNoteByText(QString text);
    void exportQueue();
    void importQueue();
    void initRecentNotes();

private slots:
    void on_quiz_button_clicked();

    //notizen
    void onKursHinzufuegenClicked();
    void slotOeffneNotiz();
    void initialisiereButtons();

    //notizen dropdown
    void slotOpenNotesDD();
    void slotButtonUpdateRecent();
    void slotActionUpdateRecent();

    //notenliste
    void slotUpdateNote();

private:
    Ui::HomePage *ui;
    Notizen *notizenFenster = new Notizen();
    Kalender *k = new Kalender(this);
    Quiz *quiz = new Quiz(this);
    Notenliste *notenliste = new Notenliste(this);

    CustomToolButton *dropdownNotes = new CustomToolButton();
    QMenu *notesMenu = new QMenu;
    QToolBar *editToolbar = new QToolBar(this);
    vector<int> notesQueue = {0,1,2,3,4};
};
#endif // HOMEPAGE_H
