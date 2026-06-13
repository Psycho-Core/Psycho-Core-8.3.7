/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Warlock TRIGGERS (S15). On top of the generic vocabulary, the Warlock adds
 *  Soul-Shard / DoT / pet conditions the BfA 8.3 specs key off:
 *    "soul shards high"     - >= 60% Soul Shards (spend: Chaos Bolt / UA / HoG)
 *    "agony absent"         - target lacks Agony (Affliction shard-gen DoT)
 *    "corruption absent"    - target lacks Corruption (Affliction DoT)
 *    "immolate absent"      - target lacks Immolate (Destruction DoT)
 *    "demonic core"         - Demonic Core proc up (Demo: instant Demonbolt)
 *    "no warlock pet"       - the bot has no living demon (needs to summon one)
 *  Soul Shards read as a percent (POWER_SOUL_SHARDS=7). All else by NAME / pet.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_WARLOCK_TRIGGERS_H
#define PSYCHOBOT_WARLOCK_TRIGGERS_H

#include "../../engine/Trigger.h"
#include "../../engine/NamedObjectContext.h"

namespace psychobot
{
    class AiObjectContext;

    // Soul Shards >= threshold percent (POWER_SOUL_SHARDS = 7, max 5).
    class SoulShardsAboveTrigger : public Trigger
    {
    public:
        SoulShardsAboveTrigger(PsychobotAI* ai, std::string name, float pct)
            : Trigger(ai, std::move(name)), _pct(pct) { }
        bool IsActive() override;
    private:
        float _pct;
    };

    // Target MISSING a (named) DoT.
    class WarlockTargetMissingAuraTrigger : public Trigger
    {
    public:
        WarlockTargetMissingAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // Self HAS a (named) proc aura up (Demonic Core).
    class WarlockSelfHasAuraTrigger : public Trigger
    {
    public:
        WarlockSelfHasAuraTrigger(PsychobotAI* ai, std::string name, std::string auraName)
            : Trigger(ai, std::move(name)), _auraName(std::move(auraName)) { }
        bool IsActive() override;
    private:
        std::string _auraName;
    };

    // The bot has NO living demon pet.
    class NoWarlockPetTrigger : public Trigger
    {
    public:
        NoWarlockPetTrigger(PsychobotAI* ai) : Trigger(ai, "no warlock pet") { }
        bool IsActive() override;
    };

    void RegisterWarlockTriggers(AiObjectContext* context);
}

#endif // PSYCHOBOT_WARLOCK_TRIGGERS_H
