#ifndef QUIZCARDBASICTYPEANSWER_H
#define QUIZCARDBASICTYPEANSWER_H

#include "quizcard.h"

class QuizCardBasicTypeAnswer : public QuizCard
{
	public:
	QuizCardBasicTypeAnswer();
	QuizCardBasicTypeAnswer(qint64 creationTime);
	virtual ~QuizCardBasicTypeAnswer();

	void SetQuestion(const QString &text);
	void SetAnswer(const QString &text);

	QString GetQuestion() const;
	QString GetAnswer() const;

	public:
	virtual CardType GetCardType() const override
	{
		return CardType::BasicTypeAnswer;
	}

	protected:
	void ExportData(std::ostream &stream) override;

	private:
	QString m_question;
	QString m_answer;
};

#endif // QUIZCARDBASICTYPEANSWER_H
