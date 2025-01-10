#include "homepage.h"
#include "ui_homepage.h"
#include <QFileDialog>
#include <qmessagebox.h>
#include <levelsystem.h>
#include <fstream>
#include <qinputdialog.h>

HomePage::HomePage(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HomePage)
{
    ui->setupUi(this);
	LevelSystem::GetInstance().SetWidgets(ui->lbl_home_exp__level, ui->lbl_home_exp__bar);
	LevelSystem::GetInstance().UpdateWidgets();

    // Setting up connections
    connect(ui->actionKalender, &QAction::triggered, this, &HomePage::openCalendar);
    connect(ui->actionQuiz, &QAction::triggered, this, &HomePage::openQuiz);
    connect(ui->actionNotenliste, &QAction::triggered, this, &HomePage::openNotenliste);
    connect(ui->action_bg, &QAction::triggered, this, &HomePage::changeBgImg);
    connect(ui->kursHinzufuegenButton,&QPushButton::clicked,this, &HomePage::onKursHinzufuegenClicked);

    // Adjusts calendar table geometry to fit container
    ui->calendarTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->calendarTable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Loads and updates calendar table
    loadCalendarData();
    updateCalendarTable();

    //notizen load Buttons
    importQueue();
    initialisiereButtons();

    //create drop down menu to show all notizen
    initNotizMenu();

    slotUpdateNote();
    connect(notenliste, &Notenliste::aboutToBeClosed, this, &HomePage::slotUpdateNote);

}

HomePage::~HomePage()
{
    exportQueue();
    delete ui;
}

void HomePage::openCalendar()
{
    k->show();
}

void HomePage::openQuiz()
{
    quiz->show();
}

void HomePage::openNotenliste()
{
    notenliste->show();
}

void HomePage::changeBgImg(){
    QString filename = QFileDialog::getOpenFileName(this,"Choose new background image","C://","Pictures (*.jpg; *.jpeg; *.png)");
    QMessageBox::information(this,"Choose this file?",filename);
    QString styleSheet = "#centralwidget {"
                         "    border-image: url('"+filename+"') stretch 0 0 0 0;"
                                                            "    background-color: rgba(255,255,255,100);"
                                                            "}";
    this->setStyleSheet(styleSheet);
}

// Updates the Calendar qTable with calendar items with todays day of the week from the calendar items vector
void HomePage::updateCalendarTable() {
    // Clear all cell contents
    ui->calendarTable->clearContents();

    // Get the current day of the week (returns a number from 1 to 7, where 1 is Monday and 7 is Sunday)
    int currentDayOfWeek = QDate::currentDate().dayOfWeek();

    // Get the index of the current day column in the table (days are represented as integers)
    int columnIndex = currentDayOfWeek - 1;

    // Set the locale to German so we can fetch German Day Names
    QLocale germanLocale(QLocale::German);

    // Set the column header to display the current day of the week
    QStringList headerLabels;
    headerLabels << germanLocale.toString(QDate::currentDate(), "dddd");
    ui->calendarTable->setHorizontalHeaderLabels(headerLabels);

    // Loop through calendar items for today and add them to the QTable
    for (const auto& item : calendarItems) {
        int dayStart = item.dateStart.date().dayOfWeek();

        // Check if the item's start day matches the current day
        if (dayStart == currentDayOfWeek) {
            int hourStart = item.dateStart.time().hour();
            int hourEnd = item.dateEnd.time().hour();

            for (int hour = hourStart; hour <= hourEnd; ++hour) {
                int indexY = hour - 6; // Table starts at hour 6

                QTableWidgetItem *tableItem = new QTableWidgetItem("");
                if (hour == hourStart) {
                    tableItem->setText(item.name);
                }

                tableItem->setData(Qt::BackgroundRole, QColor(item.blockColorHex));
                tableItem->setData(Qt::UserRole, QVariant(item.id));
                tableItem->setTextAlignment(Qt::AlignCenter);
                ui->calendarTable->setItem(indexY, columnIndex, tableItem);
            }
        }
    }

}

// Loads calendar items from a .txt file into the calendar items vector
void HomePage::loadCalendarData(){
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


void HomePage::on_quiz_button_clicked()
{
    quiz->show();
}
void HomePage::slotOeffneNotiz()
{
    QDynamicButton *button = (QDynamicButton*) sender();
    notizenFenster->initialisiereNotizen(findNoteByText(button->text()));
    notizenFenster->show();
}

void HomePage::onKursHinzufuegenClicked()
{
    //qDebug() << "TEST";
    QString buttonBeschreibung;
    QAction *openButton = new QAction(this);
    buttonBeschreibung = QInputDialog::getText(this, "Geben Sie den Kursname ein","Hier:");
    if(!buttonBeschreibung.isEmpty()){
        //drop down menu
        notizenFenster->kursnotizenHinzufuegen(buttonBeschreibung.toStdString());
        openButton->setText(buttonBeschreibung);
        connect(openButton, SIGNAL(triggered()), this, SLOT(slotOpenNotesDD()));
        connect(openButton, SIGNAL(triggered()), this, SLOT(slotActionUpdateRecent()));
        notesMenu->addAction(openButton);

        //recent notes
        notesQueue.erase(notesQueue.begin());
        notesQueue.push_back(findNoteByText(buttonBeschreibung));
        initRecentNotes();
    }
    else{
        delete openButton;
    }
}

void HomePage::initialisiereButtons()
{
    for(int i =0; i< notizenFenster->getAnzahlNotizen();i++){
        //init the contents of the dropdown menu
        QAction *openButton = new QAction(QString::fromStdString(notizenFenster->getNotizenName(i)), this);
        connect(openButton, SIGNAL(triggered()), this, SLOT(slotOpenNotesDD()));
        connect(openButton, SIGNAL(triggered()), this, SLOT(slotActionUpdateRecent()));
        notesMenu->addAction(openButton);
    }
    if(!notesQueue.empty()){
        for(size_t i=0; i<notesQueue.size(); i++){
            QDynamicButton *button = new QDynamicButton(this);
            button->setText(QString::fromStdString(notizenFenster->getNotizenName(notesQueue[i])));
            //button->setStyleSheet("background-color: rgb(10, 186, 181); border-style: outset; border-radius: 10px");
            ui->notizenLayout->addWidget(button);
            connect(button, SIGNAL(clicked()), this, SLOT(slotOeffneNotiz()));
            connect(button, SIGNAL(clicked()), this, SLOT(slotButtonUpdateRecent()));
        }
    }
}

void HomePage::initRecentNotes(){
    while (QLayoutItem *item = ui->notizenLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
    for(size_t i=0; i<notesQueue.size(); i++){
        QDynamicButton *button = new QDynamicButton(this);
        button->setText(QString::fromStdString(notizenFenster->getNotizenName(notesQueue[i])));
        ui->notizenLayout->addWidget(button);
        connect(button, SIGNAL(clicked()), this, SLOT(slotOeffneNotiz()));
        connect(button, SIGNAL(clicked()), this, SLOT(slotButtonUpdateRecent()));
    }
}

void HomePage::slotOpenNotesDD(){
    QAction *action = (QAction*) sender();
    notizenFenster->initialisiereNotizen(findNoteByText(action->text()));
    notizenFenster->show();
}

int HomePage::findNoteByText(QString text)
{
    for(int i =0; i< notizenFenster->getAnzahlNotizen();i++){
        if(text==QString::fromStdString(notizenFenster->getNotizenName(i))){
            return i;
        }
    }
    return -1;
}

void HomePage::initNotizMenu()
{
    dropdownNotes->setMenu(notesMenu);
    editToolbar->addWidget(dropdownNotes);
    ui->kursWahlLayout->addWidget(editToolbar);
}

void HomePage::exportQueue(){
    ofstream exportFile;
    exportFile.open("queue.txt", ios::out);
    if(exportFile.is_open()){
        while(!notesQueue.empty()){
            exportFile << to_string(notesQueue.front()) << ',';
            notesQueue.erase(notesQueue.begin());
        }
        exportFile << '!';
        exportFile.close();
        cout << "Erfolg!" << endl;
    }
    else{
        cout << "Exportfehler" << endl;
    }
    exportFile.close();
}

void HomePage::importQueue(){
    ifstream importFile;
    string zeil;
    notesQueue.clear();
    importFile.open("queue.txt");
    if(importFile.is_open()){
        getline(importFile, zeil);
        int i = 0;
        string notizNr = "";
        while(zeil[i]!='!'){
            if(zeil[i]==','){
                notesQueue.push_back(stoi(notizNr));
                notizNr = "";
                i++;
            }
            else{
                notizNr+=zeil[i];
                i++;
            }
        }
        importFile.close();
    }
    else{
        cout << "Importfehler" << endl;
    }
}

void HomePage::slotButtonUpdateRecent(){
    QDynamicButton *button = (QDynamicButton*) sender();
    int index = findNoteByText(button->text());
    for(size_t i = 0; i<notesQueue.size(); i++){
        if(index==notesQueue[i]){
            notesQueue.erase(notesQueue.begin()+i);
            notesQueue.insert(notesQueue.begin(),index);
            break;
        }
    }
    initRecentNotes();
}

void HomePage::slotActionUpdateRecent(){
    QAction *button = (QAction*) sender();
    int index = findNoteByText(button->text());
    bool found = false;
    for(size_t i = 0; i<notesQueue.size(); i++){
        if(index==notesQueue[i]){
            notesQueue.erase(notesQueue.begin()+i);
            notesQueue.insert(notesQueue.begin(),index);
            found = true;
            break;
        }
    }
    if(!found){
        notesQueue.pop_back();
        notesQueue.insert(notesQueue.begin(),index);
    }
    initRecentNotes();
}

void HomePage::slotUpdateNote(){
    QString text = "Dein Durchschnitt ist gerade " + notenliste->getDurchschnitt() + ". Fight on!!";
    QString label = "<html><head/><body><p align='center'><span style=' font-size:12.5pt;'>"+text+"</span></p></body></html>";
    ui->noteLabel->setText(label);
}

