/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Mage STRATEGIES (S14) - Arcane / Fire / Frost dps + conjure / polymorph.
 *    GenericMageStrategy - shared: Counterspell interrupt, Ice Block self-save,
 *                          Frost Nova when an enemy is in melee.
 *    ArcaneMageStrategy  - Arcane Blast (builds Charges), Arcane Missiles on
 *                          Clearcasting, Arcane Barrage to dump Charges, Arcane
 *                          Explosion AoE, Arcane Power CD.
 *    FireMageStrategy    - Fireball filler, Pyroblast on Hot Streak, Fire Blast
 *                          to bank crits, Scorch execute, Combustion CD,
 *                          Flamestrike AoE.
 *    FrostMageStrategy   - Frostbolt filler, Flurry on Brain Freeze, Ice Lance
 *                          on Fingers of Frost, Frozen Orb CD, Blizzard AoE,
 *                          Icy Veins CD.
 *    GenericMageNonCombatStrategy - Arcane Intellect + Conjure Refreshment.
 *  All casts via the keystone CastSpell family, referenced by spell NAME.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_MAGE_STRATEGIES_H
#define PSYCHOBOT_MAGE_STRATEGIES_H

#include "../../engine/Strategy.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    class GenericMageStrategy : public Strategy
    {
    public:
        GenericMageStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "mage"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_RANGED; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class ArcaneMageStrategy : public GenericMageStrategy
    {
    public:
        ArcaneMageStrategy(PsychobotAI* ai) : GenericMageStrategy(ai) { }
        std::string GetName() const override { return "arcane"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class FireMageStrategy : public GenericMageStrategy
    {
    public:
        FireMageStrategy(PsychobotAI* ai) : GenericMageStrategy(ai) { }
        std::string GetName() const override { return "fire"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class FrostMageStrategy : public GenericMageStrategy
    {
    public:
        FrostMageStrategy(PsychobotAI* ai) : GenericMageStrategy(ai) { }
        std::string GetName() const override { return "frost"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT | STRATEGY_TYPE_DPS | STRATEGY_TYPE_RANGED; }
    protected:
        NextAction** GetDefaultCombatActions() override;
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    class GenericMageNonCombatStrategy : public Strategy
    {
    public:
        GenericMageNonCombatStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "mage nc"; }
        int GetType() override { return STRATEGY_TYPE_NONCOMBAT; }
    protected:
        void InitNonCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    void RegisterMageStrategies(AiObjectContext* context);
}

#endif // PSYCHOBOT_MAGE_STRATEGIES_H
