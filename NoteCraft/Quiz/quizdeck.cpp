#include "quizdeck.h"

QuizDeck::QuizDeck(const QString &name) : m_Name(name)
{
}

const QString &QuizDeck::GetName()
{
	return m_Name;
}

QuizCard &QuizDeck::GetCard(int index)
{
	return *m_Cards[index];
}

QuizCard *QuizDeck::GetCardPointer(int index)
{
	return m_Cards[index].get();
}

const QuizCard &QuizDeck::GetCard(int index) const
{
	return *m_Cards[index];
}

void QuizDeck::AddCard(std::unique_ptr<QuizCard> card)
{
	m_Cards.push_back(std::move(card));
}

void QuizDeck::RemoveCardAt(int index)
{
	// TODO bounds checking
	m_Cards.erase(m_Cards.begin() + index);
}

size_t QuizDeck::GetCardCount()
{
	return m_Cards.size();
}

int QuizDeck::GetDueCardCount()
{
	size_t cardCount = m_Cards.size();
	int dueCount = 0;
	for (size_t i = 0; i < cardCount; i++)
	{
		if (m_Cards[i]->IsDue())
			dueCount++;
	}

	return dueCount;
}
