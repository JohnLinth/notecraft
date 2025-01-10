#ifndef QUIZCARDBASICREVERSED_H
#define QUIZCARDBASICREVERSED_H

#include "quizcard.h"

class QuizCardBasicReversed : public QuizCard
{
	public:
	QuizCardBasicReversed();
	QuizCardBasicReversed(qint64 creationTime);
	virtual ~QuizCardBasicReversed();

	void SetFront(const QString &text);
	void SetBack(const QString &text);

	QString GetFront() const;
	QString GetBack() const;

	// QuizCard interface
	public:
	virtual CardType GetCardType() const override
	{
	    return CardType::BasicReversed;
	}

	private:
	QString m_front;
	QString m_back;

	// QuizCard interface
protected:
	void ExportData(std::ostream &stream) override;
};

#endif // QUIZCARDBASICREVERSED_H
