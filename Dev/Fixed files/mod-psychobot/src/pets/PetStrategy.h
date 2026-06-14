/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Pet STRATEGY + the pet-control trigger (S19). The "pet" strategy is added to
 *  the combat engine of every pet-using class (Hunter BM/Survival, Warlock,
 *  Unholy DK, Frost Mage) ON TOP of the spec rotation. It keeps the pet on the
 *  bot's current target each tick:
 *    trigger "pet idle"  (has living pet + valid target + pet not on it)
 *        -> action "pet attack"  (relevance ACTION_HIGH so it beats fillers)
 *  Registers its trigger + strategy + the shared pet actions.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_PETS_PETSTRATEGY_H
#define PSYCHOBOT_PETS_PETSTRATEGY_H

#include "../engine/Strategy.h"
#include "../engine/Trigger.h"
#include "../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Active when the bot has a living pet, a valid target, and the pet is NOT
    // already attacking that target -> time to (re)direct the pet.
    class PetIdleTrigger : public Trigger
    {
    public:
        PetIdleTrigger(PsychobotAI* ai) : Trigger(ai, "pet idle") { }
        bool IsActive() override;
    };

    // Generic pet-coordination strategy (layered on top of the spec rotation).
    class PetStrategy : public Strategy
    {
    public:
        PetStrategy(PsychobotAI* ai) : Strategy(ai) { }
        std::string GetName() const override { return "pet"; }
        int GetType() override { return STRATEGY_TYPE_COMBAT; }
    protected:
        void InitCombatTriggers(std::list<TriggerNode*>& triggers) override;
    };

    // Register the pet trigger + strategy + the shared pet actions.
    void RegisterPetSystem(AiObjectContext* context);
}

#endif // PSYCHOBOT_PETS_PETSTRATEGY_H
