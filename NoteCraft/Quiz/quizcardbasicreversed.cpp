#include "quizcardbasicreversed.h"
#include <QString>

QuizCardBasicReversed::QuizCardBasicReversed()
{
}

QuizCardBasicReversed::QuizCardBasicReversed(qint64 creationTime) : QuizCard(creationTime)
{
}

QuizCardBasicReversed::~QuizCardBasicReversed()
{
}

void QuizCardBasicReversed::SetFront(const QString &text)
{
    m_front = text;
}

void QuizCardBasicReversed::SetBack(const QString &text)
{
    m_back = text;
}

QString QuizCardBasicReversed::GetFront() const
{
    return m_front;
}

QString QuizCardBasicReversed::GetBack() const
{
	return m_back;
}

void QuizCardBasicReversed::ExportData(std::ostream &stream)
{
	stream << static_cast<int>(GetCardType()) << ';' << GetFront().toStdString() << ';' << GetBack().toStdString()
		   << ';' << m_CreationTime << ';' << m_NextDueTime;
}
