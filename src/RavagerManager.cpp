#include "RavagerManager.h"
#include "Util.h"
#include "CCBot.h"

RavagerInfo::RavagerInfo() :
m_hpLastSecond(45)
{
	
}

RavagerInfo::RavagerInfo(float hp) :
	m_hpLastSecond(hp)
{

}

RavagerManager::RavagerManager(CCBot & bot)
	: MicroManager(bot)
{

}

void RavagerManager::executeMicro(const std::vector<const sc2::Unit *> & targets)
{
	assignTargets(targets);
}

void RavagerManager::assignTargets(const std::vector<const sc2::Unit *> & targets)
{
	const std::vector<const sc2::Unit *> & Ravagers = getUnits();

	// figure out targets
	std::vector<const sc2::Unit *> RavagerTargets;
	for (auto target : targets)
	{
		if (!target) { continue; }
		if (target->unit_type == sc2::UNIT_TYPEID::ZERG_EGG) { continue; }
		if (target->unit_type == sc2::UNIT_TYPEID::ZERG_LARVA) { continue; }

		RavagerTargets.push_back(target);
	}

	// for each meleeUnit
	bool refreshInfo = m_bot.Map().frame() % 16;
	for (auto Ravager : Ravagers)
	{
		BOT_ASSERT(Ravager, "ranged unit is null");
		if (RavagerInfos.find(Ravager) == RavagerInfos.end())
		{
			RavagerInfos[Ravager] = RavagerInfo(Ravager->health);
		}
		float currentHP = Ravager->health;
		bool beingAttack = currentHP < RavagerInfos[Ravager].m_hpLastSecond;
		if (refreshInfo) RavagerInfos[Ravager].m_hpLastSecond = currentHP;
		
		// if the order is to attack or defend
		if (order.getType() == SquadOrderTypes::Attack || order.getType() == SquadOrderTypes::Defend)
		{
			if (!RavagerTargets.empty())
			{
				// find the best target for this meleeUnit
				const sc2::Unit * target = getTarget(Ravager, RavagerTargets);
				if (!target) continue;
				
				if (m_bot.State().m_stimpack)
				{
					auto abilities = m_bot.Query()->GetAbilitiesForUnit(Ravager);
					bool stimpack = false;
					
					for (auto & ab : abilities.abilities)
					{
						if (ab.ability_id.ToType() == sc2::ABILITY_ID::EFFECT_STIM)
						{
							stimpack = true;
							
						}
					}
					for (auto buff : Ravager->buffs) {
						if (buff == sc2::BUFF_ID::STIMPACK) {
							stimpack = false;
						}
					}
					if (stimpack && (beingAttack || Ravager->weapon_cooldown >0))
					{
						Micro::SmartAbility(Ravager, sc2::ABILITY_ID::EFFECT_STIM,m_bot);
					}
					continue;
				}
				// kite attack it
				if (Util::IsMeleeUnit(target) && Ravager->weapon_cooldown > 0) {
					auto p1 = target->pos, p2 = Ravager->pos;
					auto tp = p2 * 2 - p1;
					Micro::SmartMove(Ravager, tp, m_bot);
				}
				else {
					Micro::SmartAttackMove(Ravager, target->pos, m_bot);
				}
			}
			// if there are no targets
			else
			{
				// if we're not near the order position
				if (Util::Dist(Ravager->pos, order.getPosition()) > 4)
				{
					// move to it
					Micro::SmartMove(Ravager, order.getPosition(), m_bot);
				}
			}
		}

		if (m_bot.Config().DrawUnitTargetInfo)
		{
			// TODO: draw the line to the unit's target
		}
	}
}

// get a target for the ranged unit to attack
// TODO: this is the melee targeting code, replace it with something better for ranged units
const sc2::Unit * RavagerManager::getTarget(const sc2::Unit * rangedUnit, const std::vector<const sc2::Unit *> & targets)
{
	BOT_ASSERT(rangedUnit, "null melee unit in getTarget");

	int highPriority = 0;
	double closestDist = std::numeric_limits<double>::max();
	const sc2::Unit * closestTarget = nullptr;

	// for each target possiblity
	for (auto targetUnit : targets)
	{
		BOT_ASSERT(targetUnit, "null target unit in getTarget");

		int priority = getAttackPriority(rangedUnit, targetUnit);
		float distance = Util::Dist(rangedUnit->pos, targetUnit->pos);

		// if it's a higher priority, or it's closer, set it
		if (!closestTarget || (priority > highPriority) || (priority == highPriority && distance < closestDist))
		{
			closestDist = distance;
			highPriority = priority;
			closestTarget = targetUnit;
		}
	}

	return closestTarget;
}

// get the attack priority of a type in relation to a zergling
int RavagerManager::getAttackPriority(const sc2::Unit * attacker, const sc2::Unit * unit)
{
	BOT_ASSERT(unit, "null unit in getAttackPriority");
	if (Util::IsPsionicUnit(unit))
	{
		return 11;
	}
	if (Util::IsMeleeUnit(unit))
	{
		return 10;
	}
	if (Util::IsCombatUnit(unit, m_bot))
	{
		return 9;
	}

	if (Util::IsWorker(unit))
	{
		return 8;
	}

	return 1;
}

