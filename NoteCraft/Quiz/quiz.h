#ifndef QUIZ_H
#define QUIZ_H

#include "Quiz/quizsession.h"
#include "quizdeck.h"

#include <QDialog>

namespace Ui {
class Quiz;
}

class Quiz : public QDialog
{
    Q_OBJECT

public:
	explicit Quiz(QWidget *parent = nullptr);
	~Quiz();

  private:
	Ui::Quiz *ui;
	std::vector<std::unique_ptr<QuizDeck>> m_quizDecks;
	std::unique_ptr<QuizSession> m_currentQuizSession;

	int m_currentDeckIndex = -1;

private:
	void CreateQuizDeck(const QString &name);
	void AddToDeckTable(QuizDeck &deck) const;

	void AddCardToListWidget(QuizCard &card) const;

	void ConnectSignals();

	void CreateInspectorForCard(QuizCard *card);
	void ShowQuizCard(const QuizCard &card);

	void UpdateDueCounts();

	void SaveAllData(QString path);
	void LoadData(QString path);

	private slots:
	void OnBtnDeckListStartQuiz_Clicked();
	void OnBtnDeckListCreate_Clicked();
	void OnBtnDeckListEdit_Clicked();
	void OnBtnDeckListDelete_Clicked();
	void OnTableDecksItem_SelectionChanged();

	void OnBtnDeckListImportDecks_Clicked();
	void OnBtnDeckListExportAllDecks_Clicked();
	void OnBtnDeckListExportSingleDeck_Clicked();

	void OnBtnCardListPageBack_Clicked();
	void OnBtnCardListCreateCard_Clicked();
	void OnBtnCardListDeleteCard_Clicked();
	void OnListCardList_CurrentRowChanged(int row);

	void OnBtnQuizSessionPageBack_Clicked();
	void OnBtnQuizSessionShowAnswer_Clicked();
	void OnBtnQuizSessionNext_Clicked();

	void OnBtnQuizSessionSubmitAnswer_Clicked();
};

#endif // QUIZ_H
