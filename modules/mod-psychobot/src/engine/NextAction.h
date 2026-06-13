/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  NextAction - a (name, relevance) pair the engine resolves into an action.
 *  Strategies/triggers describe what to do next as NULL-terminated arrays of
 *  NextAction* (built with NextAction::array(...)). The static helpers manage
 *  those raw arrays (size/clone/merge/destroy) the way the reference engine
 *  expects, so ported per-class wiring translates 1:1.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_NEXTACTION_H
#define PSYCHOBOT_ENGINE_NEXTACTION_H

#include "Define.h"
#include <string>

namespace psychobot
{
    class NextAction
    {
    public:
        NextAction(std::string name, float relevance = 0.0f)
            : _name(std::move(name)), _relevance(relevance) { }
        NextAction(NextAction const& o) = default;
        NextAction& operator=(NextAction const& o) = default;

        std::string const& GetName() const { return _name; }
        float GetRelevance() const { return _relevance; }
        void  SetRelevance(float r) { _relevance = r; }

        // --- raw NULL-terminated array helpers --------------------------------
        static int          Size(NextAction** actions);
        static NextAction** Clone(NextAction** actions);
        static NextAction** Merge(NextAction** what, NextAction** with);
        static NextAction** Array(uint32 n, ...);   // n entries, NULL-terminated
        static void         Destroy(NextAction** actions);

    private:
        std::string _name;
        float       _relevance;
    };
}

#endif // PSYCHOBOT_ENGINE_NEXTACTION_H
