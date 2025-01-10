#include "levelsystem.h"

void LevelSystem::SetExp(int amount)
{
	m_CurrentExp = amount;
}

void LevelSystem::SetWidgets(QLabel *levelLabel, QProgressBar *levelBar)
{
	m_levelLabel = levelLabel;
	m_levelBar = levelBar;
}

void LevelSystem::AddExperiencePoints(int amount)
{
	m_CurrentExp += amount;
}

int LevelSystem::GetLevel()
{
	int remainingExp = m_CurrentExp;
	int currentTreshold = c_StartExpTreshold;
	int level = 1;

	while (remainingExp >= currentTreshold)
	{
		level++;
		remainingExp -= currentTreshold;
		currentTreshold += c_ExpGrowth;
	}

	return level;
}

int LevelSystem::GetExperienceOfCurrentLevel()
{
	int remainingExp = m_CurrentExp;
	int currentTreshold = c_StartExpTreshold;

	while (remainingExp >= currentTreshold)
	{
		remainingExp -= currentTreshold;
		currentTreshold += c_ExpGrowth;
	}

	return remainingExp;
}

int LevelSystem::GetLevelThreshold()
{
	int remainingExp = m_CurrentExp;
	int currentTreshold = c_StartExpTreshold;

	while (remainingExp >= currentTreshold)
	{
		remainingExp -= currentTreshold;
		currentTreshold += c_ExpGrowth;
	}

	return currentTreshold;
}

int LevelSystem::GetTotalExperience()
{
	return m_CurrentExp;
}

LevelSystem::Info LevelSystem::GetInfo()
{
	int remainingExp = m_CurrentExp;
	int currentTreshold = c_StartExpTreshold;
	int level = 1;

	while (remainingExp >= currentTreshold)
	{
		level++;
		remainingExp -= currentTreshold;
		currentTreshold += c_ExpGrowth;
	}

	return LevelSystem::Info(level, remainingExp, currentTreshold);
}

void LevelSystem::UpdateWidgets()
{
	if (m_levelLabel != nullptr && m_levelBar != nullptr)
	{
		LevelSystem::Info info = GetInfo();
		m_levelLabel->setText(QString::number(info.Level));
		m_levelBar->setValue(info.RemainingExp);
		m_levelBar->setMaximum(info.ExpThreshold);
	}
}
