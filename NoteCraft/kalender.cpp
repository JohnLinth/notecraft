#include "Kalender.h"
#include "qmenubar.h"
#include "ui_Kalender.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QTimeEdit>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QColorDialog>
#include <fstream>
#include <vector>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cstdlib>

std::vector<calendarItem> calendarItems;

Kalender::Kalender(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Kalender)
{
    ui->setupUi(this);
    ui->calendarTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->calendarTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->calendarWeekLabel->setText("KW " + QString::number(weekNumber));

    // Loads data from data save file and updates table
    loadCalendar();
    updateWeekTable();

    // Sets calendarID to the highest ID from the import
    calendarID = calculateCalendarID();

    connect(ui->updateButton, &QPushButton::clicked, this, &Kalender::updateWeekTable);
    connect(ui->loadButton, &QPushButton::clicked, this, &Kalender::loadCalendar);
    connect(ui->saveButton, &QPushButton::clicked, this, &Kalender::saveCalendar);
    connect(ui->addBlockButton, &QPushButton::clicked, this, &Kalender::addBlock);
    connect(ui->calendarTable, &QTableWidget::itemClicked, this, &Kalender::handleItemClicked);
    connect(ui->rightButton, &QPushButton::clicked, this, &Kalender::nextWeek);
    connect(ui->leftButton, &QPushButton::clicked, this, &Kalender::lastWeek);
    connect(ui->obsImportButton, &QPushButton::clicked, this, &Kalender::obsImport);
}

Kalender::~Kalender()
{
    // Saves automatically on program exit
    saveCalendar();
    delete ui;
}

int Kalender::calculateCalendarID(){
    int baseID = 0;
    for(auto item: calendarItems){
        if(item.id > baseID)
            baseID = item.id;
    }
}

using namespace std;

// Function to parse a date string and return a QDateTime object
QDateTime Kalender::parseDateTime(const QString& datePart) {
    int year = datePart.mid(0, 4).toInt();
    int month = datePart.mid(4, 2).toInt();
    int day = datePart.mid(6, 2).toInt();
    int hour = datePart.mid(9, 2).toInt();
    int minute = datePart.mid(11, 2).toInt();

    return QDateTime(QDate(year, month, day), QTime(hour, minute));
}

QString Kalender::generateRandomColor() {
    // Generate random RGB values
    int red = rand() % 256;
    int green = rand() % 256;
    int blue = rand() % 256;

    // Format the values into a hex color string
    QString hexColor = QString("#%1%2%3")
                           .arg(red, 2, 16, QLatin1Char('0'))
                           .arg(green, 2, 16, QLatin1Char('0'))
                           .arg(blue, 2, 16, QLatin1Char('0'));

    qDebug() << hexColor;

    return hexColor;
}

void Kalender::obsImport() {
    string filename = "../Terminplan_770982.ics";

    ifstream infile(filename);
    unordered_map<QString, QString> colorMap;  // Map to store color for each calendarItem name
    calendarItem currentcalendarItem;

    string line;
    while (getline(infile, line)) {
        if (line.empty()) continue;

        if (line[1] == '0' || line[1] == '1' || line[1] == '3' || line[1] == '4') {
            currentcalendarItem.id = calendarID;
            calendarID++;  // Increment the calendarItem ID

            // Process and store calendarItem information
            calendarItems.push_back(currentcalendarItem);
            currentcalendarItem = calendarItem();

            while (getline(infile, line) && line[1] != '0' && line[1] != '1' && line[1] != '3' && line[1] != '4' && !line.empty()) {
                if (line.substr(0, 8) == "LOCATION" && line.length() > 24) {
                    currentcalendarItem.location = QString::fromStdString(line.substr(24));
                } else if (line.substr(0, 7) == "DTSTART" && line.length() > 8) {
                    currentcalendarItem.dateStart = parseDateTime(QString::fromStdString(line.substr(8)));
                } else if (line.substr(0, 5) == "DTEND" && line.length() > 8) {
                    currentcalendarItem.dateEnd = parseDateTime(QString::fromStdString(line.substr(6)));
                } else if (line.substr(0, 7) == "SUMMARY" && line.length() > 8) {
                    currentcalendarItem.name = QString::fromStdString(line.substr(23));

                    // Assign blockColorHex based on the calendarItem name
                    if (colorMap.find(currentcalendarItem.name) == colorMap.end()) {
                        // If the calendarItem name is not in the map, generate a new color
                        QString color = generateRandomColor();  // Implement your own function to generate a color
                        colorMap[currentcalendarItem.name] = color;
                        currentcalendarItem.blockColorHex = color;
                    } else {
                        // Use the existing color for the calendarItem name
                        currentcalendarItem.blockColorHex = colorMap[currentcalendarItem.name];
                    }
                }
            }
        }
    }

    infile.close();

    // Process the collected calendarItems or store them in a data structure.
    for (const calendarItem& calendarItem : calendarItems) {
        // Do something with each calendarItem (print, store, etc.)
        cout << "Item ID: " << calendarItem.id << endl;
        cout << "Name: " << calendarItem.name.toStdString() << endl;
        cout << "Location: " << calendarItem.location.toStdString() << endl;
        cout << "Date Start: " << calendarItem.dateStart.toString("yyyy.MM.dd hh:mm").toStdString() << endl;
        cout << "Date End: " << calendarItem.dateEnd.toString("yyyy.MM.dd hh:mm").toStdString() << endl;
        cout << "Block Color Hex: " << calendarItem.blockColorHex.toStdString() << endl;
        cout << "-----------------------------------" << endl;
    }
}

void Kalender::nextWeek(){
    if(weekNumber == 52){
        weekNumber = 0;
    }else{
        weekNumber++;
    }

    ui->calendarWeekLabel->setText("KW " + QString::number(weekNumber));
    updateWeekTable();
}

void Kalender::lastWeek(){
    if(weekNumber == 1){
        weekNumber = 52;
    }else{
        weekNumber--;
    }

    ui->calendarWeekLabel->setText("KW " + QString::number(weekNumber));
    updateWeekTable();
}

void Kalender::updateWeekTable(){
    // Clear all cell contents
    for (int i = 0; i < ui->calendarTable->rowCount(); ++i) {
        for (int j = 0; j < ui->calendarTable->columnCount(); ++j) {
            QTableWidgetItem* item = new QTableWidgetItem();
            // Clear content
            ui->calendarTable->setItem(i, j, item);
            ui->calendarTable->item(i, j)->setBackground(Qt::white);
        }
    }

    // Loop thru calendar items and add them to the QTable
    for (const auto& item : calendarItems) {
        int hourStart = item.dateStart.time().hour();
        int dayStart = item.dateStart.date().dayOfWeek();
        int hourEnd = item.dateEnd.time().hour();


        if (item.dateStart.date().weekNumber() == weekNumber)
        for (int hour = hourStart; hour <= hourEnd; ++hour) {
            int indexY = hour - 6; // Table starts at hour 6
            int indexX = dayStart - 1; // Days are represented as integers

            QTableWidgetItem *tableItem = new QTableWidgetItem("");
            if (hour == hourStart) {
                tableItem->setText(item.name);
            }

            tableItem->setData(Qt::BackgroundRole, QColor(item.blockColorHex));
            tableItem->setData(Qt::UserRole, QVariant(item.id));
            tableItem->setTextAlignment(Qt::AlignCenter);
            ui->calendarTable->setItem(indexY, indexX, tableItem);
        }

    }
}


// Opens a dialog for user input to create a new calendar event, adding it to the vector
void Kalender::addBlock()
{
    QDialog *dialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout(dialog);

    QLineEdit *lineEditBlockName = new QLineEdit(dialog);
    QDateTimeEdit *timeEditStart = new QDateTimeEdit(QDateTime::currentDateTime(), dialog);
    QDateTimeEdit *timeEditEnd = new QDateTimeEdit(QDateTime::currentDateTime(), dialog);

    layout->addWidget(new QLabel("Block Name:", dialog));
    layout->addWidget(lineEditBlockName);

    QPushButton *colorButton = new QPushButton("Pick Color", dialog);
    layout->addWidget(colorButton);

    layout->addWidget(new QLabel("Time Start:", dialog));
    layout->addWidget(timeEditStart);

    layout->addWidget(new QLabel("Time End:", dialog));
    layout->addWidget(timeEditEnd);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, dialog);
    layout->addWidget(buttonBox);

    QColor selectedColor = Qt::white;

    connect(colorButton, &QPushButton::clicked, [=, &selectedColor]() {
        QColorDialog colorDialog(selectedColor, dialog);
        colorDialog.setOption(QColorDialog::ShowAlphaChannel);
        if (colorDialog.exec()) {
            selectedColor = colorDialog.currentColor();
        }
    });

    connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

    if (dialog->exec() == QDialog::Accepted) {
        QString blockName = lineEditBlockName->text();
        QDateTime timeStart = timeEditStart->dateTime();
        QDateTime timeEnd = timeEditEnd->dateTime();

        calendarItem newItem;
        newItem.id = calendarID;
        newItem.location = "Default Location";
        newItem.dateStart = timeStart;
        newItem.dateEnd = timeEnd;
        newItem.name = blockName;
        newItem.blockColorHex = selectedColor.name();

        calendarItems.push_back(newItem);

        updateCalendarTable();

        calendarID++;
    }

    delete dialog;
}

// Displays a simple error dialog with a provided error message
void Kalender::showErrorDialog(const QString& errorMessage) {
    QMessageBox errorBox;
    errorBox.setIcon(QMessageBox::Critical);
    errorBox.setWindowTitle("Error");
    errorBox.setText(errorMessage);
    errorBox.exec();
}

// Determines the table cells for a given time range and adds corresponding items to the vector and qTable
void Kalender::addBlockToTable(QString blockName, QString blockColorHex, QDateTime timeStart, QDateTime timeEnd)
{
    int hourStart = timeStart.time().hour();
    int dayStart = timeStart.date().dayOfWeek();

    int hourEnd = timeEnd.time().hour();
    int dayEnd = timeEnd.date().dayOfWeek();
    qDebug() << dayEnd;

    for (int day = dayStart; day <= dayEnd; ++day) {
        int startHour = (day == dayStart) ? hourStart : 7;
        int endHour = (day == dayEnd) ? hourEnd : 23;

        for (int hour = startHour; hour <= endHour; ++hour) {
            int indexY = hour - 6; // Table starts at hour 7
            int indexX = day - 1; // Days are represented as integers

            QTableWidgetItem *item = new QTableWidgetItem("");
            if (hour == startHour) {
                item->setText(blockName);
            }

            item->setData(Qt::BackgroundRole, QColor(blockColorHex));
            item->setData(Qt::UserRole, QVariant(calendarID));
            item->setTextAlignment(Qt::AlignCenter);
            ui->calendarTable->setItem(indexY, indexX, item);
        }

        calendarID++;
    }
}

// Handles the click event on an item in the qTable, providing options to edit or delete the item
void Kalender::handleItemClicked(QTableWidgetItem *item) {
    if (item != nullptr) {
        QVariant idVariant = item->data(Qt::UserRole);
        if (idVariant.isValid()) {
            int tableItemId = idVariant.toInt();

            // Search for the calendar item with the given ID
            auto it = std::find_if(calendarItems.begin(), calendarItems.end(), [tableItemId](const calendarItem &item) {
                return item.id == tableItemId;
            });

            if (it != calendarItems.end()) {
                const calendarItem &selectedItem = *it;

                QDialog *itemDialog = new QDialog(this);
                QVBoxLayout *layout = new QVBoxLayout(itemDialog);

                QLabel *itemNameLabel = new QLabel("Item Name: " + selectedItem.name);
                QLabel *itemIDLabel = new QLabel("Item ID: " + QString::number(selectedItem.id));
                QLabel *locationLabel = new QLabel("Location: " + selectedItem.location);
                QLabel *dateStartLabel = new QLabel("Date Start: " + selectedItem.dateStart.toString(Qt::ISODate));
                QLabel *dateEndLabel = new QLabel("Date End: " + selectedItem.dateEnd.toString(Qt::ISODate));
                QLabel *blockColorLabel = new QLabel("Block Color Hex: " + selectedItem.blockColorHex);

                layout->addWidget(itemNameLabel);
                layout->addWidget(itemIDLabel);
                layout->addWidget(locationLabel);
                layout->addWidget(dateStartLabel);
                layout->addWidget(dateEndLabel);
                layout->addWidget(blockColorLabel);

                QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Discard, Qt::Horizontal, itemDialog);
                QPushButton *editButton = buttonBox->button(QDialogButtonBox::Ok);
                editButton->setText("Edit");
                QPushButton *deleteButton = buttonBox->button(QDialogButtonBox::Discard);
                deleteButton->setText("Delete");

                layout->addWidget(buttonBox);

                connect(buttonBox, &QDialogButtonBox::accepted, itemDialog, &QDialog::accept);
                connect(buttonBox, &QDialogButtonBox::rejected, itemDialog, &QDialog::reject);

                connect(buttonBox, &QDialogButtonBox::clicked, [=](QAbstractButton *button) {
                    if (buttonBox->standardButton(button) == QDialogButtonBox::Ok) {
                        editCalendarItem(selectedItem);
                    } else if (buttonBox->standardButton(button) == QDialogButtonBox::Cancel) {
                        // Handle 'Cancel' action
                        itemDialog->reject();
                    } else if (buttonBox->standardButton(button) == QDialogButtonBox::Discard) {
                        qDebug() << "called";
                        deleteBlock(selectedItem);
                    }
                });

                itemDialog->exec();
                delete itemDialog;
            }
        }
    }
}

// Deletes a selected item from the vector and qTable
void Kalender::deleteBlock(const calendarItem &selectedItem) {
    // Find the calendar item with the given ID
    auto it = std::find_if(calendarItems.begin(), calendarItems.end(), [&](const calendarItem &item) {
        return item.id == selectedItem.id;
    });

    // Delete from vector
    if (it != calendarItems.end()) {
        calendarItems.erase(it);
        updateCalendarTable();
    }
}

// Opens a dialog to edit specific elements of a calendar item
void Kalender::editCalendarItem(const calendarItem &item) {
    QDialog *editDialog = new QDialog(this);
    QVBoxLayout *layout = new QVBoxLayout(editDialog);

    QLineEdit *lineEditBlockName = new QLineEdit(item.name, editDialog);
    QDateTimeEdit *timeEditStart = new QDateTimeEdit(item.dateStart, editDialog);
    timeEditStart->setDisplayFormat("dd-MM-yyyy HH:mm");
    timeEditStart->setDateTime(item.dateStart);

    QDateTimeEdit *timeEditEnd = new QDateTimeEdit(item.dateEnd, editDialog);
    timeEditEnd->setDisplayFormat("dd-MM-yyyy HH:mm");
    timeEditEnd->setDateTime(item.dateEnd);

    layout->addWidget(new QLabel("Block Name:", editDialog));
    layout->addWidget(lineEditBlockName);

    layout->addWidget(new QLabel("Time Start:", editDialog));
    layout->addWidget(timeEditStart);

    layout->addWidget(new QLabel("Time End:", editDialog));
    layout->addWidget(timeEditEnd);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, editDialog);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, editDialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, editDialog, &QDialog::reject);

    if (editDialog->exec() == QDialog::Accepted) {
        // Create a copy of the item to update
        calendarItem updatedItem = item;
        updatedItem.name = lineEditBlockName->text();
        updatedItem.dateStart = timeEditStart->dateTime();
        updatedItem.dateEnd = timeEditEnd->dateTime();

        // Update the calendarItems vector with the updated item
        auto it = std::find_if(calendarItems.begin(), calendarItems.end(), [&](const calendarItem &existingItem) {
            return existingItem.id == item.id;
        });

        if (it != calendarItems.end()) {
            *it = updatedItem; // Update the item in the vector with the updated copy
        }

        updateCalendarTable(); // Update the calendar table after editing
    }

    delete editDialog;
}

// Saves the calendar items vector to a .txt file for future retrieval
void Kalender::saveCalendar()
{
    std::ofstream file("./calenderData.notecraft", std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        for (const auto& item : calendarItems) {
            file << "ID: " << item.id << "\n";
            file << "Location: " << item.location.toStdString() << "\n";
            file << "Date Start: " << item.dateStart.toString(Qt::ISODate).toStdString() << "\n";
            file << "Date End: " << item.dateEnd.toString(Qt::ISODate).toStdString() << "\n";
            file << "Name: " << item.name.toStdString() << "\n";
            file << "Block Color Hex: " << item.blockColorHex.toStdString() << "\n\n";
        }
        file.close();
    } else {
        // Handle file open error
    }
}

// Loads calendar items from a .txt file into the calendar items vector
void Kalender::loadCalendar() {
    std::ifstream file("./calenderData.notecraft");
    if (file.is_open()) {
        std::vector<calendarItem> importedItems;

        std::string line;
        std::string locationString;
        std::string dateStartString;
        std::string dateEndString;
        std::string nameString;
        std::string blockColorHexString;

        calendarItem newItem;

        while (std::getline(file, line)) {
            if (line.find("ID: ") != std::string::npos) {
                if (!newItem.location.isEmpty()) {
                    newItem.dateStart = QDateTime::fromString(QString::fromStdString(dateStartString), Qt::ISODate);
                    newItem.dateEnd = QDateTime::fromString(QString::fromStdString(dateEndString), Qt::ISODate);
                    newItem.name = QString::fromStdString(nameString);
                    newItem.blockColorHex = QString::fromStdString(blockColorHexString);
                    // Remove the line that increments calendarID for each item
                    importedItems.push_back(newItem);
                }

                newItem = calendarItem();
                newItem.id = std::stoi(line.substr(4));
                dateStartString.clear();
                dateEndString.clear();
                nameString.clear();
                blockColorHexString.clear();
            } else if (line.find("Location: ") != std::string::npos) {
                newItem.location = QString::fromStdString(line.substr(10));
            } else if (line.find("Date Start: ") != std::string::npos) {
                dateStartString = line.substr(12);
            } else if (line.find("Date End: ") != std::string::npos) {
                dateEndString = line.substr(10);
            } else if (line.find("Name: ") != std::string::npos) {
                nameString = line.substr(6);
            } else if (line.find("Block Color Hex: ") != std::string::npos) {
                blockColorHexString = line.substr(17);
            }
        }

        // Add the last item after the loop ends
        if (!newItem.location.isEmpty()) {
            newItem.dateStart = QDateTime::fromString(QString::fromStdString(dateStartString), Qt::ISODate);
            newItem.dateEnd = QDateTime::fromString(QString::fromStdString(dateEndString), Qt::ISODate);
            newItem.name = QString::fromStdString(nameString);
            newItem.blockColorHex = QString::fromStdString(blockColorHexString);
            // Remove the line that increments calendarID for the last item
            importedItems.push_back(newItem);
        }

        file.close();

        // Update the calendarItems vector
        calendarItems = importedItems;
    } else {
        // Handle file open error
        qDebug() << "Unable to open the file for importing.";
    }

    // Output imported items for debugging
    for (const auto& item : calendarItems) {
        qDebug() << "ID:" << item.id;
        qDebug() << "Location:" << item.location;
        qDebug() << "Date Start:" << item.dateStart.toString(Qt::ISODate);
        qDebug() << "Date End:" << item.dateEnd.toString(Qt::ISODate);
        qDebug() << "Name:" << item.name;
        qDebug() << "Color:" << item.blockColorHex << "\n";
    }
}

// Updates the qTable with all items present in the calendar items vector
void Kalender::updateCalendarTable() {

    // Clear all cell contents
    for (int i = 0; i < ui->calendarTable->rowCount(); ++i) {
        for (int j = 0; j < ui->calendarTable->columnCount(); ++j) {
            ui->calendarTable->setItem(i, j, nullptr);
        }
    }

    // Loop thru calendar items and add them to the QTable
    for (const auto& item : calendarItems) {
        int hourStart = item.dateStart.time().hour();
        int dayStart = item.dateStart.date().dayOfWeek();
        int hourEnd = item.dateEnd.time().hour();


        for (int hour = hourStart; hour <= hourEnd; ++hour) {
            int indexY = hour - 6; // Table starts at hour 6
            int indexX = dayStart - 1; // Days are represented as integers

            QTableWidgetItem *tableItem = new QTableWidgetItem("");
            if (hour == hourStart) {
                tableItem->setText(item.name);
            }

            tableItem->setData(Qt::BackgroundRole, QColor(item.blockColorHex));
            tableItem->setData(Qt::UserRole, QVariant(item.id));
            tableItem->setTextAlignment(Qt::AlignCenter);
            ui->calendarTable->setItem(indexY, indexX, tableItem);
        }

    }
}

