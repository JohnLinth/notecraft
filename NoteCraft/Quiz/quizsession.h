#ifndef QUIZSESSION_H
#define QUIZSESSION_H

#include "quizdeck.h"

class QuizSession
{
public:
	QuizSession(QuizDeck& deck);
	bool MoveNext();
	QuizCard &GetCurrentCard();

	int GetCardCount() const;

	size_t GetCurrentIndex() const;

	private:
	QuizDeck& m_currentDeck;
	size_t currentIndex = 0;
};

#endif // QUIZSESSION_H
