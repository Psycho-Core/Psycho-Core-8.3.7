/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Shaman STRATEGIES (S13) - Elemental/Enhancement dps + Restoration heal +
 *  totems / weapon imbues.
 *    GenericShamanStrategy - shared: Wind Shear interrupt, Healing Surge self-
 *                            heal, Healing Stream Totem upkeep.
 *    ElementalShamanStrategy - ranged: Flame Shock DoT, Lava Burst (auto-crit),
 *                            Earth Shock (Maelstrom spender), Lightning Bolt
 *                            filler, Earthquake/Chain Lightning AoE, Fire
 *                            Elemental CD; Lightning Shield upkeep.
 *    EnhancementShamanStrategy - melee: Stormstrike, Lava Lash, Crash Lightning
 *                            AoE, Lightning Bolt at high Maelstrom Weapon, Flame
 *                            Shock; Windfury/Flametongue weapon imbues.
 *    RestorationShamanStrategy - healer: Riptide HoT + Healing Wave/Surge +
 *                            Chain Heal (group) + Healing Rain; Spirit Link CD.
 *    GenericShamanNonCombatStrategy - weapon imbues + Lightning Shield upkeep.
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_SHAMAN_STRATEGIES_H
#define PSYCHOBOT_SHAMAN_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericShamanStrategy : public Strategy
    {
    public:
        GenericShamanStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "shaman"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ElementalShamanStrategy : public GenericShamanStrategy
    {
    public:
        ElementalShamanStrategy(PsychobotAI* ai) : GenericShamanStrategy(ai) { }
        std::string GetName() const override { return "elemental"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class EnhancementShamanStrategy : public GenericShamanStrategy
    {
    public:
        EnhancementShamanStrategy(PsychobotAI* ai) : GenericShamanStrategy(ai) { }
        std::string GetName() const override { return "enhancement"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_MELEE; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class RestorationShamanStrategy : public GenericShamanStrategy
    {
    public:
        RestorationShamanStrategy(PsychobotAI* ai) : GenericShamanStrategy(ai) { }
        std::string GetName() const override { return "restoration"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_HEAL | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericShamanNonCombatStrategy : public Strategy
    {
    public:
        GenericShamanNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "shaman nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterShamanStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_SHAMAN_STRATEGIES_H
