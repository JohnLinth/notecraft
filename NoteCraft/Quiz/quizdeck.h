#ifndef QUIZDECK_H
#define QUIZDECK_H

#include <QString>
#include "Quiz/quizcard.h"
#include <memory>
#include <vector>

class QuizDeck
{
	public:
		QuizDeck(const QString &name);

		const QString &GetName();
		QuizCard &GetCard(int index);
		const QuizCard &GetCard(int index) const;
		QuizCard *GetCardPointer(int index);
		void AddCard(std::unique_ptr<QuizCard> card);
		void RemoveCardAt(int index);

		size_t GetCardCount();
		int GetDueCardCount();

		private:
		QString m_Name;
		std::vector<std::unique_ptr<QuizCard>> m_Cards;
};

#endif // QUIZDECK_H
