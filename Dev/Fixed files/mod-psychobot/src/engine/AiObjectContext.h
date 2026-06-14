/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  AiObjectContext - the per-bot registry of every named engine object. It owns
 *  four NamedObjectContextLists (strategies / triggers / actions / values) so a
 *  class context can ADD to the shared base context. Strategies and triggers
 *  resolve names through here; the engine reads strategies/actions; values are
 *  read by triggers/actions via GetValue<T>().
 *
 *  The base context (generic values/triggers/actions/strategies) is registered
 *  in S4-S6; class contexts (S7+) push additional contexts onto each list.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_AIOBJECTCONTEXT_H
#define PSYCHOBOT_ENGINE_AIOBJECTCONTEXT_H

#include "NamedObjectContext.h"
#include "Action.h"
#include "Trigger.h"
#include "Strategy.h"
#include "Value.h"
#include <set>
#include <string>

namespace psychobot
{
    class PsychobotAI;

    class AiObjectContext
    {
    public:
        explicit AiObjectContext(PsychobotAI* ai) : _ai(ai) { }
        virtual ~AiObjectContext() = default;

        // --- name -> object resolution (lazily created + cached) -------------
        Strategy*     GetStrategy(std::string const& name) { return _strategyContexts.GetObject(name, _ai); }
        Trigger*      GetTrigger(std::string const& name)  { return _triggerContexts.GetObject(name, _ai); }
        Action*       GetAction(std::string const& name)   { return _actionContexts.GetObject(name, _ai); }
        UntypedValue* GetUntypedValue(std::string const& name) { return _valueContexts.GetObject(name, _ai); }

        template <class T>
        Value<T>* GetValue(std::string const& name)
        {
            return dynamic_cast<Value<T>*>(GetUntypedValue(name));
        }

        template <class T>
        Value<T>* GetValue(std::string const& name, std::string const& param)
        {
            return GetValue<T>(name + "::" + param);
        }

        std::set<std::string> GetSupportedStrategies() const { return _strategyContexts.GetAllNames(); }

        // --- context registration (base first, then class contexts) ---------
        // Each Add takes ownership of the context.
        void AddStrategyContext(NamedObjectContext<Strategy>* ctx) { _strategyContexts.Add(ctx); }
        void AddTriggerContext(NamedObjectContext<Trigger>* ctx)   { _triggerContexts.Add(ctx); }
        void AddActionContext(NamedObjectContext<Action>* ctx)     { _actionContexts.Add(ctx); }
        void AddValueContext(NamedObjectContext<UntypedValue>* ctx) { _valueContexts.Add(ctx); }

        PsychobotAI* GetAI() const { return _ai; }

    protected:
        PsychobotAI* _ai;

        NamedObjectContextList<Strategy>     _strategyContexts;
        NamedObjectContextList<Trigger>      _triggerContexts;
        NamedObjectContextList<Action>       _actionContexts;
        NamedObjectContextList<UntypedValue> _valueContexts;
    };
}

#endif // PSYCHOBOT_ENGINE_AIOBJECTCONTEXT_H
