#include "quizcard.h"

#include <QDateTime>

QuizCard::QuizCard()
{
}

QuizCard::QuizCard(qint64 creationTime) : m_CreationTime(creationTime)
{
}

bool QuizCard::IsDue()
{
	return QDateTime::currentSecsSinceEpoch() >= m_NextDueTime;
}

qint64 QuizCard::GetCreationTime()
{
	return m_CreationTime;
}

qint64 QuizCard::GetNextDueTime()
{
	return m_NextDueTime;
}

void QuizCard::SetCreationTime(qint64 time)
{
	m_CreationTime = time;
}

void QuizCard::SetNextDueTime(qint64 time)
{
	m_NextDueTime = time;
}

QString QuizCard::CardTypeToString(CardType type)
{
	switch (type)
	{
		case CardType::BasicReversed:
			return "Standard (mit Rückseite)";
		case CardType::BasicTypeAnswer:
			return "Standard (Eingabe)";
		case CardType::MultipleChoice:
			return "Multiple Choice";
	}
}
