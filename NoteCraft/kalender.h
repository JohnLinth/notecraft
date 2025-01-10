#ifndef Kalender_H
#define Kalender_H

#include "qdatetime.h"
#include "qtablewidget.h"
#include <QDialog>

struct calendarItem {
    int id;
    QString location;
    QDateTime dateStart;
    QDateTime dateEnd;
    QString name;
    QString blockColorHex;
};

extern std::vector<calendarItem> calendarItems;

namespace Ui {
class Kalender;
}

class Kalender : public QDialog
{
    Q_OBJECT

private:
    Ui::Kalender *ui;
    std::vector<calendarItem> calendarItems;

public:
    explicit Kalender(QWidget *parent = nullptr);
    ~Kalender();
    void addBlock();
    void deleteBlock();

    int calendarID = 0;
    int weekNumber = QDate::currentDate().weekNumber();

    void addBlockToTable(QString blockName, QString blockColorHex, QDateTime timeStart, QDateTime timeEnd);
    void showErrorDialog(const QString &errorMessage);
    void handleItemClicked(QTableWidgetItem *item);
    void saveCalendar();
    void loadCalendar();
    void updateCalendarTable();
    void editCalendarItem(const calendarItem &item);
    void deleteBlock(const calendarItem &selectedItem);

    void nextWeek();
    void lastWeek();
    void updateWeekTable();
    int getCalendarWeek(QDate date);
    void obsImport();
    QDateTime parseDateTime(const QString &datePart);
    int calculateCalendarID();
    QString generateRandomColor();
};

#endif // Kalender_H
