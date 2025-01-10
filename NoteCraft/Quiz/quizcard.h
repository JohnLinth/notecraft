#ifndef QUIZCARD_H
#define QUIZCARD_H

#include <QString>
#include <iostream>

class QuizCard
{
	public:
	enum class CardType
	{
		BasicReversed,
		BasicTypeAnswer,
		MultipleChoice
	};

	public:
	static QString CardTypeToString(CardType type);

	protected:
	qint64 m_CreationTime;
	qint64 m_NextDueTime;

	public:
	QuizCard();
	QuizCard(qint64 creationTime);
	virtual ~QuizCard(){};

	bool IsDue();

	qint64 GetCreationTime();
	qint64 GetNextDueTime();

	void SetCreationTime(qint64 time);
	void SetNextDueTime(qint64 time);

	friend std::ostream &operator<<(std::ostream &out, QuizCard &card)
	{
		card.ExportData(out);
		return out;
	}

	virtual CardType GetCardType() const = 0;

	protected:
	virtual void ExportData(std::ostream& stream) = 0;
};

#endif // QUIZCARD_H
