#include "quizsession.h"

QuizSession::QuizSession(QuizDeck &deck) : m_currentDeck(deck)
{
}

bool QuizSession::MoveNext()
{
	if (++currentIndex < m_currentDeck.GetCardCount())
		return true;
	else
		return false;
}

QuizCard &QuizSession::GetCurrentCard()
{
	return m_currentDeck.GetCard(currentIndex);
}

int QuizSession::GetCardCount() const
{
	return m_currentDeck.GetCardCount();
}

size_t QuizSession::GetCurrentIndex() const
{
	return currentIndex;
}
