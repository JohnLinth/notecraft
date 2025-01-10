#ifndef QUIZCARDMULTIPLECHOICE_H
#define QUIZCARDMULTIPLECHOICE_H

#include "quizcard.h"

class QuizCardMultipleChoice : public QuizCard
{
	public:
	QuizCardMultipleChoice();
	QuizCardMultipleChoice(qint64 creationTime);
	virtual ~QuizCardMultipleChoice();

	// QuizCard interface
	public:
	CardType GetCardType() const override
	{
		return CardType::MultipleChoice;
	}

	// QuizCard interface
	protected:
	void ExportData(std::ostream &stream) override;
};

#endif // QUIZCARDMULTIPLECHOICE_H
