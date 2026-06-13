/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  ActionNode - the graph node the engine actually executes. It binds a named
 *  Action (resolved lazily from the context) to its own Prerequisite /
 *  Alternative / Continuer NextAction chains, and merges those with whatever
 *  the underlying Action itself declares.
 *
 *    Prerequisites - must succeed first (e.g. "reach melee" before "rend").
 *    Alternatives  - tried in order if this action can't run.
 *    Continuers    - pushed to run next tick after this one succeeds.
 *
 *  Strategies declare nodes with the ACTION_NODE_* macros below so the per-class
 *  ActionNodeFactory wiring reads exactly like the reference engine.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_ACTIONNODE_H
#define PSYCHOBOT_ENGINE_ACTIONNODE_H

#include "Action.h"
#include "NextAction.h"
#include <string>

namespace psychobot
{
    class ActionNode
    {
    public:
        ActionNode(std::string name,
                   NextAction** prerequisites = nullptr,
                   NextAction** alternatives  = nullptr,
                   NextAction** continuers    = nullptr)
            : _action(nullptr), _name(std::move(name)),
              _prerequisites(prerequisites), _alternatives(alternatives), _continuers(continuers) { }

        virtual ~ActionNode()
        {
            NextAction::Destroy(_prerequisites);
            NextAction::Destroy(_alternatives);
            NextAction::Destroy(_continuers);
        }

        Action*            GetAction() const { return _action; }
        void               SetAction(Action* a) { _action = a; }
        std::string const& GetName() const { return _name; }

        // These merge the node's declared chains with the action's own chains.
        // The merged arrays are freshly allocated; caller owns/destroys them.
        NextAction** GetPrerequisites()
        {
            return NextAction::Merge(NextAction::Clone(_prerequisites),
                                     _action ? _action->GetPrerequisites() : nullptr);
        }
        NextAction** GetAlternatives()
        {
            return NextAction::Merge(NextAction::Clone(_alternatives),
                                     _action ? _action->GetAlternatives() : nullptr);
        }
        NextAction** GetContinuers()
        {
            return NextAction::Merge(NextAction::Clone(_continuers),
                                     _action ? _action->GetContinuers() : nullptr);
        }

    private:
        Action*     _action;
        std::string _name;
        NextAction** _prerequisites;
        NextAction** _alternatives;
        NextAction** _continuers;
    };

    // ----------------------------------------------------------------------
    // Declaration helpers used inside an ActionNodeFactory creator so per-class
    // wiring matches the reference: ACTION_NODE_A("rend", "reach melee") etc.
    // ----------------------------------------------------------------------
    #define ACTION_NODE_P(name, prereq) \
        new ActionNode(name, \
            /*P*/ NextAction::Array(1, new NextAction(prereq), nullptr), \
            /*A*/ nullptr, \
            /*C*/ nullptr)

    #define ACTION_NODE_A(name, alt) \
        new ActionNode(name, \
            /*P*/ nullptr, \
            /*A*/ NextAction::Array(1, new NextAction(alt), nullptr), \
            /*C*/ nullptr)

    #define ACTION_NODE_C(name, cont) \
        new ActionNode(name, \
            /*P*/ nullptr, \
            /*A*/ nullptr, \
            /*C*/ NextAction::Array(1, new NextAction(cont), nullptr))

    #define ACTION_NODE_NONE(name) \
        new ActionNode(name, nullptr, nullptr, nullptr)
}

#endif // PSYCHOBOT_ENGINE_ACTIONNODE_H
