/*
 * Copyright (C) 2016-2019 AtieshCore <https://at-wow.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Util.h"
#include "Unit.h"
#include "MapManager.h"
#include "World.h"

class VAS_AutoBalance_UnitScript : UnitScript
{
public:
    VAS_AutoBalance_UnitScript()
        : UnitScript("VAS_AutoBalance_UnitScript")
    {
    }

    uint32 VAS_Modifer_DealDamage(Unit* target, Unit* victim, uint32 damage)
    {
        if (target->ToPlayer())
            return damage;

        if (victim->ToPlayer() || victim->IsHunterPet() || victim->IsPet() || victim->IsSummon())
        {
            float damageMultiplier = sWorld->getRate(RATE_VAS_DAMAGE_PERCENT);
            float newdamage = CalculatePct(damage, damageMultiplier);
            damage = uint32(newdamage);
        }

        return damage;
    }

    uint32 VAS_Modifer_DealHeal(Unit* target, Unit* victim, uint32 damage)
    {
        if (target->ToPlayer() || victim->ToPlayer()) // Heal by player should be 100%
            return damage;

        float healMultiplier = sWorld->getRate(RATE_VAS_HEAL_PERCENT);
        float newheal = CalculatePct(damage, healMultiplier);
        damage = uint32(newheal);

        return damage;
    }

    void CreatureUpdateLevelDependantStatsWithMaxHealth(Unit* unit, uint32& maxhealth) override
    {
        if (!sWorld->getBoolConfig(CONFIG_VAS_AUTOBALANCE))
            return;

        if (!unit->ToPlayer() && !unit->IsPet() && !unit->IsTotem() && unit->GetMap()->IsDungeon())
        {
            float healthMultiplier = sWorld->getRate(RATE_VAS_MAXHP_PERCENT);
            float newhealth = CalculatePct(maxhealth, healthMultiplier);
            maxhealth = newhealth;
        }
    }

    void CreatureUpdateLevelDependantStatsWithMaxMana(Unit* unit, uint32& maxmana) override
    {
        if (!sWorld->getBoolConfig(CONFIG_VAS_AUTOBALANCE))
            return;

        if (!unit->ToPlayer() && !unit->IsPet() && !unit->IsTotem() && unit->GetMap()->IsDungeon())
        {
            float manaMultiplier = sWorld->getRate(RATE_VAS_MAXMP_PERCENT);
            float newmana = CalculatePct(maxmana, manaMultiplier);
            maxmana = newmana;
        }
    }

    void ModifyPeriodicDamageAurasTick(Unit* target, Unit* attacker, uint32& damage) override
    {
        if (!sWorld->getBoolConfig(CONFIG_VAS_AUTOBALANCE))
            return;

        if (target && attacker && target->IsInWorld() && attacker->IsInWorld())
            if (target->GetMap() && target->GetMap()->IsDungeon())
                damage = VAS_Modifer_DealDamage(attacker, target, damage);
    }

    void ModifyMeleeDamage(Unit* target, Unit* attacker, uint32& damage) override
    {
        if (!sWorld->getBoolConfig(CONFIG_VAS_AUTOBALANCE))
            return;

        if (target && attacker && target->IsInWorld() && attacker->IsInWorld())
            if (target->GetMap() && target->GetMap()->IsDungeon())
                damage = VAS_Modifer_DealDamage(attacker, target, damage);
    }

    void ModifySpellDamageTaken(Unit* target, Unit* attacker, int32& damage) override
    {
        if (!sWorld->getBoolConfig(CONFIG_VAS_AUTOBALANCE))
            return;

        if (target && attacker && target->IsInWorld() && attacker->IsInWorld())
            if(target->GetMap() && target->GetMap()->IsDungeon())
                damage = VAS_Modifer_DealDamage(attacker, target, damage);
    }

    void OnHeal(Unit* healer, Unit* reciever, uint32& gain) override
    {
        if (!sWorld->getBoolConfig(CONFIG_VAS_AUTOBALANCE))
            return;

        if (reciever && healer && reciever->IsInWorld() && healer->IsInWorld())
            if (healer->GetMap() && healer->GetMap()->IsDungeon())
                gain = VAS_Modifer_DealHeal(healer, reciever, gain);
    }
};

void AddSC_VAS_AutoBalance()
{
    new VAS_AutoBalance_UnitScript;
}
