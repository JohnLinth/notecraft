#include "quizcardbasictypeanswer.h"

QuizCardBasicTypeAnswer::QuizCardBasicTypeAnswer()
{
}

QuizCardBasicTypeAnswer::QuizCardBasicTypeAnswer(qint64 creationTime) : QuizCard(creationTime)
{
}

QuizCardBasicTypeAnswer::~QuizCardBasicTypeAnswer()
{
}

void QuizCardBasicTypeAnswer::SetQuestion(const QString &text)
{
	m_question = text;
}

void QuizCardBasicTypeAnswer::SetAnswer(const QString &text)
{
	m_answer = text;
}

QString QuizCardBasicTypeAnswer::GetQuestion() const
{
	return m_question;
}

QString QuizCardBasicTypeAnswer::GetAnswer() const
{
	return m_answer;
}

void QuizCardBasicTypeAnswer::ExportData(std::ostream &stream)
{
	stream << static_cast<int>(GetCardType()) << ';' << GetQuestion().toStdString() << ';' << GetAnswer().toStdString()
		   << ';' << m_CreationTime << ';' << m_NextDueTime;
}
