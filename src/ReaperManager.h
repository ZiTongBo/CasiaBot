#pragma once

#include "Common.h"
#include "MicroManager.h"

class CCBot;

struct ReaperInfo
{
	float m_hpLastSecond;

	ReaperInfo();
	ReaperInfo(float hp);
};

class ReaperManager : public MicroManager
{
	std::unordered_map<const sc2::Unit*, ReaperInfo> ReaperInfos;
public:

	ReaperManager(CCBot & bot);
	void    executeMicro(const std::vector<const sc2::Unit *> & targets);
	void    assignTargets(const std::vector<const sc2::Unit *> & targets);
	float TimeToFaceEnemy(const sc2::Unit * unit, const sc2::Unit * target) const;
	void SmartKiteTarget(const sc2::Unit * unit, const sc2::Unit * target) const;
	sc2::Point2D RetreatPosition(const sc2::Unit * unit);
	int     getAttackPriority(const sc2::Unit * rangedUnit, const sc2::Unit * target);
	const sc2::Unit * getTarget(const sc2::Unit * rangedUnit, const std::vector<const sc2::Unit *> & targets);
};
