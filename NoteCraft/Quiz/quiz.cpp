#include "quiz.h"
#include "Quiz/quizcardbasictypeanswer.h"
#include "quizcardbasicreversed.h"
#include "quizcardmultiplechoice.h"
#include "ui_quiz.h"
#include <QFileDialog>
#include <QImageReader>
#include <QInputDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <fstream>
#include <levelSystem.h>
#include <qlabel.h>

Quiz::Quiz(QWidget *parent) : QDialog(parent), ui(new Ui::Quiz)
{
	ui->setupUi(this);
	LoadData("./quiz.notecraft");

	ui->table_decks->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ConnectSignals();

	OnTableDecksItem_SelectionChanged();

	// To make sure the first page is shown in case I forget to change it back in the editor
	ui->stack_pages->setCurrentIndex(0);
	qDebug() << QDateTime::currentSecsSinceEpoch();
}

Quiz::~Quiz()
{
	SaveAllData("./quiz.notecraft");
	delete ui;
}

void Quiz::CreateQuizDeck(const QString &name)
{
	auto deck = std::make_unique<QuizDeck>(name);
	AddToDeckTable(*deck);
	m_quizDecks.push_back(std::move(deck));
}

void Quiz::AddToDeckTable(QuizDeck &deck) const
{
	QTableWidget *table = ui->table_decks;
	QTableWidgetItem *nameItem = new QTableWidgetItem(deck.GetName());
	QTableWidgetItem *cardCountItem = new QTableWidgetItem(QString::number(deck.GetCardCount()));
	QTableWidgetItem *dueCountItem = new QTableWidgetItem(QString::number(deck.GetDueCardCount()));

	int row = table->rowCount();
	table->insertRow(row);
	table->setItem(row, 0, nameItem);
	table->setItem(row, 1, cardCountItem);
	table->setItem(row, 2, dueCountItem);

	// Select newly created item
	table->selectRow(table->rowCount() - 1);
}

void Quiz::AddCardToListWidget(QuizCard &card) const
{
	// Get card type and produce preview text

	switch (card.GetCardType())
	{
		case QuizCard::CardType::BasicReversed:
		{
			QuizCardBasicReversed &cardBasicReversed = dynamic_cast<QuizCardBasicReversed &>(card);

			if (cardBasicReversed.GetFront().isEmpty())
				ui->list_cardList->addItem(QuizCard::CardTypeToString(card.GetCardType()));
			else
				ui->list_cardList->addItem(cardBasicReversed.GetFront());

			break;
		}
		case QuizCard::CardType::BasicTypeAnswer:
		{
			QuizCardBasicTypeAnswer &cardBasicTypeAnswer = dynamic_cast<QuizCardBasicTypeAnswer &>(card);

			if (cardBasicTypeAnswer.GetQuestion().isEmpty())
				ui->list_cardList->addItem(QuizCard::CardTypeToString(card.GetCardType()));
			else
				ui->list_cardList->addItem(cardBasicTypeAnswer.GetQuestion());

			break;
		}

		default:
		{
			ui->list_cardList->addItem(QuizCard::CardTypeToString(card.GetCardType()));
		}
	}
}

void Quiz::ConnectSignals()
{
	connect(ui->btn_deckList__startQuiz, SIGNAL(clicked(bool)), this, SLOT(OnBtnDeckListStartQuiz_Clicked()));
	connect(ui->btn_deckList__createDeck, SIGNAL(clicked(bool)), this, SLOT(OnBtnDeckListCreate_Clicked()));
	connect(ui->btn_deckList__deleteDeck, SIGNAL(clicked(bool)), this, SLOT(OnBtnDeckListDelete_Clicked()));
	connect(ui->btn_deckList__editDeck, SIGNAL(clicked(bool)), this, SLOT(OnBtnDeckListEdit_Clicked()));
	connect(ui->btn_deckList__exportAllDecks, &QPushButton::clicked, this, &Quiz::OnBtnDeckListExportAllDecks_Clicked);
	connect(ui->btn_deckList__exportSingleDeck, &QPushButton::clicked, this, &Quiz::OnBtnDeckListExportSingleDeck_Clicked);
	// TODO change all signatures to this vvv
	connect(ui->btn_deckList__importDeck, &QPushButton::clicked, this, &Quiz::OnBtnDeckListImportDecks_Clicked);
	connect(ui->table_decks, SIGNAL(itemSelectionChanged()), this, SLOT(OnTableDecksItem_SelectionChanged()));

	connect(ui->btn_cardList__pageBack, SIGNAL(clicked(bool)), this, SLOT(OnBtnCardListPageBack_Clicked()));
	connect(ui->btn_cardList__createCard, SIGNAL(clicked(bool)), this, SLOT(OnBtnCardListCreateCard_Clicked()));
	connect(ui->btn_cardList__deleteCard, SIGNAL(clicked(bool)), this, SLOT(OnBtnCardListDeleteCard_Clicked()));
	connect(ui->list_cardList, SIGNAL(currentRowChanged(int)), this, SLOT(OnListCardList_CurrentRowChanged(int)));

	connect(ui->btn_quizSession__pageBack, SIGNAL(clicked(bool)), this, SLOT(OnBtnQuizSessionPageBack_Clicked()));
	connect(ui->btn_quizSession__showAnswer, SIGNAL(clicked(bool)), this, SLOT(OnBtnQuizSessionShowAnswer_Clicked()));
	connect(ui->btn_quizSession_nextCard, SIGNAL(clicked(bool)), this, SLOT(OnBtnQuizSessionNext_Clicked()));
	connect(ui->btn_quizSession__quizFinished, SIGNAL(clicked(bool)), this, SLOT(OnBtnQuizSessionPageBack_Clicked()));

	connect(ui->btn_quizSession_bAnswer__submitAnswer, &QPushButton::clicked, this,
			&Quiz::OnBtnQuizSessionSubmitAnswer_Clicked);

	connect(ui->txtEdit_quizSession_bAnswer__answer, &QLineEdit::returnPressed,
			ui->btn_quizSession_bAnswer__submitAnswer, &QPushButton::click);
}

void Quiz::CreateInspectorForCard(QuizCard *card)
{
	QWidget *prevWidget = ui->scroll_cardList__inspector->takeWidget();

	if (prevWidget)
		prevWidget->deleteLater();

	switch (card->GetCardType())
	{
		case QuizCard::CardType::BasicReversed:
		{
			QuizCardBasicReversed *cardBasicReversed = dynamic_cast<QuizCardBasicReversed *>(card);

			QWidget *container = new QWidget();
			QVBoxLayout *vertical = new QVBoxLayout();
			QTextEdit *txtEditFront = new QTextEdit(cardBasicReversed->GetFront());
			QTextEdit *txtEditBack = new QTextEdit(cardBasicReversed->GetBack());

			vertical->setContentsMargins(0, 0, 0, 0);

			// TODO research whether never "disconnecting" this is a problem or not
			// https://github.com/KDE/clazy/blob/1.11/docs/checks/README-connect-3arg-lambda.md
			// ^ According to this, it should be fine (?)
			connect(txtEditFront, &QTextEdit::textChanged, txtEditFront, [=]() {
				cardBasicReversed->SetFront(txtEditFront->toPlainText());
				ui->list_cardList->currentItem()->setText(cardBasicReversed->GetFront());
			});

			connect(txtEditBack, &QTextEdit::textChanged, txtEditBack,
					[=]() { cardBasicReversed->SetBack(txtEditBack->toPlainText()); });

			// QSpacerItem *spacer = new QSpacerItem(0, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
			auto lblFront = new QLabel("Vorderseite");
			lblFront->setStyleSheet("QLabel{font-size: 16px; font: bold}");
			vertical->addWidget(lblFront);
			vertical->addWidget(txtEditFront);

			auto lblBack = new QLabel("Rückseite");
			lblBack->setStyleSheet("QLabel{font-size: 16px; font: bold}");
			vertical->addWidget(lblBack);
			vertical->addWidget(txtEditBack);

			// vertical->addItem(spacer);

			container->setLayout(vertical);
			ui->scroll_cardList__inspector->setWidget(container);
			break;
		}
		case QuizCard::CardType::BasicTypeAnswer:
		{
			// TODO
			QuizCardBasicTypeAnswer *cardBasicTypeAnswer = dynamic_cast<QuizCardBasicTypeAnswer *>(card);

			QWidget *container = new QWidget();
			QVBoxLayout *vertical = new QVBoxLayout();
			QTextEdit *txtEditFront = new QTextEdit(cardBasicTypeAnswer->GetQuestion());
			QTextEdit *txtEditBack = new QTextEdit(cardBasicTypeAnswer->GetAnswer());

			vertical->setContentsMargins(0, 0, 0, 0);

			connect(txtEditFront, &QTextEdit::textChanged, txtEditFront, [=]() {
				cardBasicTypeAnswer->SetQuestion(txtEditFront->toPlainText());
				ui->list_cardList->currentItem()->setText(cardBasicTypeAnswer->GetQuestion());
			});

			connect(txtEditBack, &QTextEdit::textChanged, txtEditBack,
					[=]() { cardBasicTypeAnswer->SetAnswer(txtEditBack->toPlainText()); });

			// QSpacerItem *spacer = new QSpacerItem(0, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
			auto lblFront = new QLabel("Frage");
			lblFront->setStyleSheet("QLabel{font-size: 16px; font: bold}");
			vertical->addWidget(lblFront);
			vertical->addWidget(txtEditFront);

			auto lblBack = new QLabel("Antwort");
			lblBack->setStyleSheet("QLabel{font-size: 16px; font: bold}");
			vertical->addWidget(lblBack);
			vertical->addWidget(txtEditBack);

			// vertical->addItem(spacer);

			container->setLayout(vertical);
			ui->scroll_cardList__inspector->setWidget(container);
			break;
		}
		case QuizCard::CardType::MultipleChoice:
		{
			// TODO
			break;
		}
		default:
		{
			break;
		}
	}
}

void Quiz::ShowQuizCard(const QuizCard &card)
{
	ui->lbl_progress->setText(
		QString("%1 / %2").arg(m_currentQuizSession->GetCurrentIndex() + 1).arg(m_currentQuizSession->GetCardCount()));
	switch (card.GetCardType())
	{
		case QuizCard::CardType::BasicReversed:
		{
			ui->stack_quizSession__cardContent->setCurrentIndex(0);

			const QuizCardBasicReversed &cardBasicReversed = dynamic_cast<const QuizCardBasicReversed &>(card);
			ui->lbl_quizSession_bRev__front->setText(cardBasicReversed.GetFront());
			ui->lbl_quizSession_bRev__back->setText(cardBasicReversed.GetBack());
			ui->lbl_quizSession_bRev__back->hide();
			ui->btn_quizSession__showAnswer->show();
			break;
		}

		case QuizCard::CardType::BasicTypeAnswer:
		{
			ui->stack_quizSession__cardContent->setCurrentIndex(1);

			const QuizCardBasicTypeAnswer &cardTypeAnswer = dynamic_cast<const QuizCardBasicTypeAnswer &>(card);
			ui->lbl_quizSession_bAnswer__question->setText(cardTypeAnswer.GetQuestion());
			ui->lbl_quizSession_bAnswer__answer->setText(cardTypeAnswer.GetAnswer());
			ui->lbl_quizSession_bAnswer__answer->hide();
			ui->lbl_quizSession_bAnswer__answerStatus->hide();
			ui->btn_quizSession_bAnswer__submitAnswer->show();
			ui->txtEdit_quizSession_bAnswer__answer->setText(QString());
			ui->txtEdit_quizSession_bAnswer__answer->show();
			ui->btn_quizSession__showAnswer->hide();

			ui->txtEdit_quizSession_bAnswer__answer->setFocus(Qt::OtherFocusReason);
			break;
		}
		case QuizCard::CardType::MultipleChoice:
			break;
	}
}

void Quiz::UpdateDueCounts()
{
	for (size_t i = 0; i < m_quizDecks.size(); i++)
	{
		ui->table_decks->item(i, 2)->setText(QString::number(m_quizDecks[i]->GetDueCardCount()));
	}
}

void Quiz::SaveAllData(QString path)
{
	if (m_quizDecks.size() == 0)
	{
		return;
	}

	std::ofstream outputStream;

	outputStream.open(path.toStdString());

	if (!outputStream)
	{
		QMessageBox::critical(this, "Quiz Export", "An error has occured while saving data.");
		return;
	}

	try
	{
		for (size_t i = 0; i < m_quizDecks.size(); i++)
		{
			size_t count = m_quizDecks[i]->GetCardCount();

			outputStream << m_quizDecks[i]->GetName().toStdString() << ';' << count;

			if (count != 0)
				outputStream << '\n';

			for (size_t j = 0; j < count; j++)
			{
				outputStream << m_quizDecks[i]->GetCard(j);

				if (j + 1 < count)
					outputStream << '\n';
			}

			if (i + 1 < m_quizDecks.size())
				outputStream << '\n';
		}

		outputStream.close();
	}
	catch (std::exception e)
	{
		outputStream.close();

		QMessageBox::critical(this, "Quiz Error",
							  QString("An error has occured while writing to the output file.\n%1").arg(e.what()));
		return;
	}
}

void Quiz::LoadData(QString path)
{
	std::ifstream inputStream;

	std::string file = path.toStdString();
	if (!std::filesystem::exists(file))
	{
		return;
	}

	inputStream.open(file);

	if (!inputStream)
	{
		QMessageBox::critical(this, "Import", "An error has occured while opening the input file.");
		return;
	}

	try
	{
		std::string values[5];

		while (inputStream.good() && !inputStream.eof())
		{
			std::getline(inputStream, values[0], ';');	// Deck name
			std::getline(inputStream, values[1], '\n'); // Deck card count

			std::unique_ptr<QuizDeck> deck = std::make_unique<QuizDeck>(QString::fromStdString(values[0]));
			int cardCount = std::stoi(values[1]);

			for (int i = 0; i < cardCount && inputStream.good(); i++)
			{
				std::getline(inputStream, values[0], ';'); // CardType

				QuizCard::CardType type = (QuizCard::CardType)std::stoi(values[0]);
				switch (type)
				{
					case QuizCard::CardType::BasicReversed:
					{
						auto card = std::make_unique<QuizCardBasicReversed>();
						std::getline(inputStream, values[1], ';');	// Front
						std::getline(inputStream, values[2], ';');	// Back
						std::getline(inputStream, values[3], ';');	// Creation Time
						std::getline(inputStream, values[4], '\n'); // Next Due Time

						qint64 creationTime = stoi(values[3]);
						qint64 nextDueTime = stoi(values[4]);

						card->SetFront(QString::fromStdString(values[1]));
						card->SetBack(QString::fromStdString(values[2]));

						card->SetCreationTime(creationTime);
						card->SetNextDueTime(nextDueTime);

						deck->AddCard(std::move(card));
						break;
					}
					case QuizCard::CardType::BasicTypeAnswer:
					{
						auto card = std::make_unique<QuizCardBasicTypeAnswer>();
						std::getline(inputStream, values[1], ';');	// Question
						std::getline(inputStream, values[2], ';');	// Answer
						std::getline(inputStream, values[3], ';');	// Creation Time
						std::getline(inputStream, values[4], '\n'); // Next Due Time

						qint64 creationTime = stoi(values[3]);
						qint64 nextDueTime = stoi(values[4]);

						card->SetQuestion(QString::fromStdString(values[1]));
						card->SetAnswer(QString::fromStdString(values[2]));

						card->SetCreationTime(creationTime);
						card->SetNextDueTime(nextDueTime);

						deck->AddCard(std::move(card));
					}
					default:
						continue;
				}
			}
			AddToDeckTable(*deck);
			m_quizDecks.push_back(std::move(deck));
		}
	}
	catch (const std::exception &e)
	{
		inputStream.close();
		QMessageBox::critical(
			this, "Import", QString("An error has occured while reading from the file.\nwhat(): \"%1\"").arg(e.what()));
	}
}

// Slots
void Quiz::OnBtnDeckListStartQuiz_Clicked()
{
	// TODO small hack, need to update the var automatically
	m_currentDeckIndex = ui->table_decks->currentRow();

	if (m_quizDecks[m_currentDeckIndex]->GetCardCount() == 0)
	{
		QMessageBox::critical(this, "Fehler", "Das Deck beinhaltet keine Karten!");
		return;
	}
	m_currentQuizSession = std::make_unique<QuizSession>(*m_quizDecks[m_currentDeckIndex]);

	ui->stack_pages->setCurrentIndex(2);
	ui->stack_quizSession__bottomControls->setCurrentIndex(0);
	ui->stack_quizSession__bottomControls->show();
	ui->stack_quizSession__cardContent->setCurrentIndex(0);
	ShowQuizCard(m_currentQuizSession->GetCurrentCard());
}

void Quiz::OnBtnDeckListCreate_Clicked()
{
	bool ok;
	QString input = QInputDialog::getText(this, tr("Neues Deck erstellen"), tr("Name"), QLineEdit::Normal, "Neues Deck", &ok);

	if (!ok || input.isEmpty())
		return;

	CreateQuizDeck(input);
}

void Quiz::OnBtnDeckListEdit_Clicked()
{
	ui->stack_pages->setCurrentIndex(1);
	m_currentDeckIndex = ui->table_decks->currentIndex().row();

	ui->list_cardList->clear();
	for (size_t i = 0; i < m_quizDecks[m_currentDeckIndex]->GetCardCount(); i++)
	{
		AddCardToListWidget(m_quizDecks[m_currentDeckIndex]->GetCard(i));
	}
}

void Quiz::OnBtnDeckListDelete_Clicked()
{
	int index = ui->table_decks->currentIndex().row();

	if (index == -1 || index >= m_quizDecks.size())
		return;

	ui->table_decks->removeRow(index);
	m_quizDecks.erase(m_quizDecks.begin() + index);
}

void Quiz::OnTableDecksItem_SelectionChanged()
{
	int index = ui->table_decks->currentRow();
	ui->btn_deckList__startQuiz->setEnabled(index >= 0);
	ui->btn_deckList__editDeck->setEnabled(index >= 0);
	ui->btn_deckList__deleteDeck->setEnabled(index >= 0);
	ui->btn_deckList__exportSingleDeck->setEnabled(index >= 0);
}

void Quiz::OnBtnDeckListImportDecks_Clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Deck file"), "./",
													tr("Text file (*.txt);;Deck file (*.notecraft)"));

	std::ifstream inputStream;

	inputStream.open(fileName.toStdString());

	if (!inputStream)
	{
		QMessageBox::critical(this, "Import", "An error has occured while opening the input file.");
		return;
	}

	try
	{
		std::string values[5];

		while (inputStream.good() && !inputStream.eof())
		{
			std::getline(inputStream, values[0], ';');	// Deck name
			std::getline(inputStream, values[1], '\n'); // Deck card count

			std::unique_ptr<QuizDeck> deck = std::make_unique<QuizDeck>(QString::fromStdString(values[0]));
			int cardCount = std::stoi(values[1]);

			for (int i = 0; i < cardCount && inputStream.good(); i++)
			{
				std::getline(inputStream, values[0], ';'); // CardType

				QuizCard::CardType type = (QuizCard::CardType)std::stoi(values[0]);
				switch (type)
				{
					case QuizCard::CardType::BasicReversed:
					{
						auto card = std::make_unique<QuizCardBasicReversed>();
						std::getline(inputStream, values[1], ';');	// Front
						std::getline(inputStream, values[2], ';');	// Back
						std::getline(inputStream, values[3], ';');	// Creation Time
						std::getline(inputStream, values[4], '\n'); // Next Due Time

						qint64 creationTime = stoi(values[3]);
						qint64 nextDueTime = stoi(values[4]);

						card->SetFront(QString::fromStdString(values[1]));
						card->SetBack(QString::fromStdString(values[2]));

						card->SetCreationTime(creationTime);
						card->SetNextDueTime(nextDueTime);

						deck->AddCard(std::move(card));
						break;
					}
					case QuizCard::CardType::BasicTypeAnswer:
					{
						auto card = std::make_unique<QuizCardBasicTypeAnswer>();
						std::getline(inputStream, values[1], ';');	// Question
						std::getline(inputStream, values[2], ';');	// Answer
						std::getline(inputStream, values[3], ';');	// Creation Time
						std::getline(inputStream, values[4], '\n'); // Next Due Time

						qint64 creationTime = stoi(values[3]);
						qint64 nextDueTime = stoi(values[4]);

						card->SetQuestion(QString::fromStdString(values[1]));
						card->SetAnswer(QString::fromStdString(values[2]));

						card->SetCreationTime(creationTime);
						card->SetNextDueTime(nextDueTime);

						deck->AddCard(std::move(card));
					}
					default:
						continue;
				}
			}
			AddToDeckTable(*deck);
			m_quizDecks.push_back(std::move(deck));
		}

		QMessageBox::information(this, "Import", QString("Datei wurde importiert."));
	}
	catch (const std::exception &e)
	{
		inputStream.close();
		QMessageBox::critical(
			this, "Import", QString("An error has occured while reading from the file.\nwhat(): \"%1\"").arg(e.what()));
	}
}

void Quiz::OnBtnDeckListExportSingleDeck_Clicked()
{
	m_currentDeckIndex = ui->table_decks->currentRow();

	QuizDeck *deck = m_quizDecks[m_currentDeckIndex].get();

	std::ofstream outputStream;
	QString fileName =
		QString("quizdeck_%1_%2_%3.txt")
			.arg(deck->GetName(), QTime::currentTime().toString("HH-mm-ss"), QDate::currentDate().toString("yy-MM-dd"));

	outputStream.open(fileName.toStdString());

	if (!outputStream)
	{
		QMessageBox::critical(this, "Export", "An error has occured while opening the output file.");
		return;
	}

	try
	{
		size_t count = deck->GetCardCount();

		outputStream << deck->GetName().toStdString() << ';' << count;

		if (count != 0)
			outputStream << '\n';

		for (size_t i = 0; i < count; i++)
		{
			outputStream << deck->GetCard(i);

			if (i + 1 < count)
				outputStream << '\n';
		}

		outputStream.close();
		QMessageBox::information(this, "Export", QString("Datei wurde gespeichert als %1").arg(fileName));
	}
	catch (...)
	{
		outputStream.close();

		QMessageBox::critical(this, "Error", "An error has occured while writing to the output file.");
		return;
	}
}

void Quiz::OnBtnDeckListExportAllDecks_Clicked()
{
	if (m_quizDecks.size() == 0)
	{
		QMessageBox::critical(this, "Fehler", "Keine Decks wurden bisher erstellt!");
		return;
	}

	QString fileName = QString("quizdecks_%1_%2.txt")
						   .arg(QTime::currentTime().toString("HH-mm-ss"), QDate::currentDate().toString("yy-MM-dd"));

	SaveAllData(fileName);
}

// Card List
void Quiz::OnBtnCardListCreateCard_Clicked()
{
	bool ok;
	QStringList cardTypes = {"Standard (mit Rückseite)", "Frage/Antwort"};
	QString input = QInputDialog::getItem(this, "Karte Erstellen", "Kartentyp", cardTypes, 0, false, &ok);

	if (ok)
	{
		// TODO find a better way than these ugly string comparisons
		// probably requires a custom modal dialog
		if (input == cardTypes[0])
		{
			auto card = std::make_unique<QuizCardBasicReversed>(QDateTime::currentSecsSinceEpoch());
			AddCardToListWidget(*card);
			m_quizDecks[m_currentDeckIndex]->AddCard(std::move(card));
		}
		else if (input == cardTypes[1])
		{
			auto card = std::make_unique<QuizCardBasicTypeAnswer>(QDateTime::currentSecsSinceEpoch());
			AddCardToListWidget(*card);
			m_quizDecks[m_currentDeckIndex]->AddCard(std::move(card));
		}
		else if (input == cardTypes[2])
		{
			auto card = std::make_unique<QuizCardMultipleChoice>(QDateTime::currentSecsSinceEpoch());
			AddCardToListWidget(*card);
			m_quizDecks[m_currentDeckIndex]->AddCard(std::move(card));
		}

		// Select newly created item;
		ui->list_cardList->setCurrentRow(ui->list_cardList->count() - 1);
		ui->table_decks->item(m_currentDeckIndex, 1)->setText(QString::number(m_quizDecks[m_currentDeckIndex]->GetCardCount()));
	}
}

void Quiz::OnBtnCardListDeleteCard_Clicked()
{
	int index = ui->list_cardList->currentRow();

	if (index == -1 || index >= m_quizDecks[m_currentDeckIndex]->GetCardCount())
		return;

	delete ui->list_cardList->takeItem(index);
	m_quizDecks[m_currentDeckIndex]->RemoveCardAt(index);
	// TODO turn this into a method OR refresh this data whenever necessary
	ui->table_decks->item(m_currentDeckIndex, 1)->setText(QString::number(m_quizDecks[m_currentDeckIndex]->GetCardCount()));
}

void Quiz::OnListCardList_CurrentRowChanged(int row)
{
	if (row == -1)
	{
		ui->btn_cardList__deleteCard->setEnabled(false);
		QWidget *inspectorWidget = ui->scroll_cardList__inspector->takeWidget();

		if (inspectorWidget)
			inspectorWidget->deleteLater();

		return;
	}

	ui->btn_cardList__deleteCard->setEnabled(true);

	CreateInspectorForCard(m_quizDecks[m_currentDeckIndex]->GetCardPointer(row));
}

void Quiz::OnBtnQuizSessionPageBack_Clicked()
{
	ui->stack_pages->setCurrentIndex(0);
}

void Quiz::OnBtnQuizSessionShowAnswer_Clicked()
{
	ui->stack_quizSession__bottomControls->setCurrentIndex(1);
	ui->lbl_quizSession_bRev__back->show();
}

void Quiz::OnBtnQuizSessionNext_Clicked()
{
	m_currentQuizSession->GetCurrentCard().SetNextDueTime(QDateTime::currentSecsSinceEpoch() + (1 * 60));

	if (m_currentQuizSession->MoveNext())
	{
		ShowQuizCard(m_currentQuizSession->GetCurrentCard());
		ui->stack_quizSession__bottomControls->setCurrentIndex(0);
	}
	else
	{
		// Quiz finished
		ui->stack_quizSession__bottomControls->hide();
		ui->stack_quizSession__cardContent->setCurrentIndex(2);

		int expAdded = m_currentQuizSession->GetCardCount() * 10;
		ui->lbl_quizSession__quizExp->setText(QString("⭐+%1EXP⭐").arg(expAdded));

		LevelSystem::GetInstance().AddExperiencePoints(expAdded);
		LevelSystem::GetInstance().UpdateWidgets();

		UpdateDueCounts();
	}
}

void Quiz::OnBtnQuizSessionSubmitAnswer_Clicked()
{
	ui->stack_quizSession__bottomControls->setCurrentIndex(1);
	ui->txtEdit_quizSession_bAnswer__answer->hide();
	ui->btn_quizSession_bAnswer__submitAnswer->hide();
	ui->lbl_quizSession_bAnswer__answer->show();

	auto card = dynamic_cast<const QuizCardBasicTypeAnswer &>(m_currentQuizSession->GetCurrentCard());

	if (ui->txtEdit_quizSession_bAnswer__answer->text() == card.GetAnswer())
	{
		ui->lbl_quizSession_bAnswer__answerStatus->setText("Richtige Antwort!");
		ui->lbl_quizSession_bAnswer__answerStatus->setStyleSheet("color: #388E3C");
	}
	else
	{
		ui->lbl_quizSession_bAnswer__answerStatus->setText("Falsche Antwort!");
		ui->lbl_quizSession_bAnswer__answerStatus->setStyleSheet("color: #D32F2F");
	}

	ui->lbl_quizSession_bAnswer__answerStatus->show();
}

void Quiz::OnBtnCardListPageBack_Clicked()
{
	QWidget *inspectorWidget = ui->scroll_cardList__inspector->takeWidget();

	if (inspectorWidget)
		inspectorWidget->deleteLater();

	ui->stack_pages->setCurrentIndex(0);
}

// void Quiz::ToolsAddImage()
// {
// 	QString file = QFileDialog::getOpenFileName(this, tr("Select an image"), ".",
// 												tr("PNG (*.png)\n"
// 												   "Bitmap Files (*.bmp)\n"
// 												   "JPEG (*.jpg *jpeg)\n"
// 												   "GIF (*.gif)\n"));

// 	QUrl Uri(QString("%1").arg(file));
// 	QImage image = QImageReader(file).read();

// 	QTextDocument *textDocument = ui->textEdit->document();
// 	textDocument->addResource(QTextDocument::ImageResource, Uri, QVariant(image));
// 	QTextCursor cursor = ui->textEdit->textCursor();
// 	QTextImageFormat imageFormat;
// 	imageFormat.setWidth(128);
// 	imageFormat.setHeight(128);
// 	imageFormat.setName(Uri.toString());
// 	cursor.insertImage(imageFormat);
// }
