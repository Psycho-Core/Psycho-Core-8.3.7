/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Pet ACTIONS (S19 - pet system depth). Shared by every pet-using class
 *  (Hunter, Warlock, DK Unholy ghoul, Mage water elemental). These drive the
 *  bot's already-summoned pet via the ServerFacade pet-control seam:
 *    "pet attack"   - sic the pet on the bot's current target
 *    "pet follow"   - recall the pet to defensive/follow
 *  Pet ABILITY casts use the existing keystone "cast pet::<spell>" action.
 *  (Summoning a pet is the class non-combat job, e.g. Hunter "Call Pet 1".)
 * ===========================================================================
 */

#ifndef PSYCHOBOT_PETS_PETACTIONS_H
#define PSYCHOBOT_PETS_PETACTIONS_H

#include "../engine/Action.h"
#include "../engine/NamedObjectContext.h"

class Unit;

namespace psychobot
{
    // Sic the pet on the current target (aggressive react + attack).
    class PetAttackAction : public Action
    {
    public:
        PetAttackAction(PsychobotAI* ai) : Action(ai, "pet attack") { }
        bool  Execute(Event const& event) override;
        bool  IsUseful() override;
        Unit* GetTarget() override;
    };

    // Recall the pet (defensive react + stop attacking).
    class PetFollowAction : public Action
    {
    public:
        PetFollowAction(PsychobotAI* ai) : Action(ai, "pet follow") { }
        bool Execute(Event const& event) override;
        bool IsUseful() override;
    };

    class AiObjectContext;
    void RegisterPetActions(AiObjectContext* context);
}

#endif // PSYCHOBOT_PETS_PETACTIONS_H
