#pragma once

#include "Common.h"
#include "MicroManager.h"

class CCBot;

struct BattlecruiserInfo
{
	float m_hpLastSecond;

	BattlecruiserInfo();
	BattlecruiserInfo(float hp);
};

class BattlecruiserManager : public MicroManager
{
	std::unordered_map<const sc2::Unit*, BattlecruiserInfo> BattlecruiserInfos;
public:

	BattlecruiserManager(CCBot & bot);
	void    executeMicro(const std::vector<const sc2::Unit *> & targets);
	void    assignTargets(const std::vector<const sc2::Unit *> & targets);
	int     getAttackPriority(const sc2::Unit * rangedUnit, const sc2::Unit * target);
	const sc2::Unit * getTarget(const sc2::Unit * rangedUnit, const std::vector<const sc2::Unit *> & targets);
};
#pragma once
