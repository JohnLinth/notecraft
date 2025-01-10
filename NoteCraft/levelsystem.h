#ifndef LEVELSYSTEM_H
#define LEVELSYSTEM_H

#include "qprogressbar.h"
#include <qlabel.h>

// Singleton class
class LevelSystem
{
public:
	struct Info
	{
		Info(int level, int remainingExp, int expThreshold)
			: Level(level), RemainingExp(remainingExp), ExpThreshold(expThreshold)
		{
		}

		int Level;
		int RemainingExp;
		int ExpThreshold;
	};

	LevelSystem(const LevelSystem&) = delete;

	static LevelSystem& GetInstance()
	{
		static LevelSystem instance;
		return instance;
	}

	void AddExperiencePoints(int amount);

	void SetExp(int amount);

	void SetWidgets(QLabel* levelLabel, QProgressBar* levelBar);

	int GetLevel();
	int GetExperienceOfCurrentLevel();
	int GetLevelThreshold();
	int GetTotalExperience();

	Info GetInfo();

	void UpdateWidgets();

private:
	LevelSystem(){};

	// Change these values as needed
	const int c_StartExpTreshold = 100;
	const int c_ExpGrowth = 50;

	int m_CurrentExp = 0;

	QLabel* m_levelLabel;
	QProgressBar* m_levelBar;
};

#endif // LEVELSYSTEM_H
