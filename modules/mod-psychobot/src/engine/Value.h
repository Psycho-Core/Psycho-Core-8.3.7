/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Value<T> - a named, cached piece of world knowledge the AI reads each tick
 *  (e.g. "current target", "health", "has aura"). The engine builds each value
 *  once per bot via the NamedObjectContext; values recompute on a check
 *  interval so triggers/actions can query them cheaply many times per tick.
 *
 *    UntypedValue          - non-templated base (so a context can hold any value)
 *    Value<T>              - the typed Get()/Set() interface
 *    CalculatedValue<T>    - caches Calculate() for `checkInterval` seconds
 *    SingleCalculatedValue - computes exactly once then caches forever
 *    MemoryCalculatedValue - CalculatedValue that also remembers its last value
 *
 *  Clean-room re-implementation of the ike3/cmangos value layer for TC 8.3.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_VALUE_H
#define PSYCHOBOT_ENGINE_VALUE_H

#include "AiObject.h"
#include "Define.h"
#include <ctime>
#include <string>

namespace psychobot
{
    class PsychobotAI;

    // ----------------------------------------------------------------------
    // UntypedValue - common, non-templated base so AiObjectContext can store
    // every value in one container and Update()/Reset() them uniformly.
    // ----------------------------------------------------------------------
    class UntypedValue : public AiNamedObject
    {
    public:
        UntypedValue(PsychobotAI* ai, std::string name) : AiNamedObject(ai, std::move(name)) { }
        virtual ~UntypedValue() = default;

        virtual void Update() { }
        virtual void Reset()  { }
        virtual std::string Format() { return "?"; }
        virtual bool Expired() { return false; }
    };

    // ----------------------------------------------------------------------
    // Value<T> - the typed accessor mixin.
    // ----------------------------------------------------------------------
    template <class T>
    class Value
    {
    public:
        virtual ~Value() = default;
        virtual T    Get()        = 0;
        virtual T    LazyGet()    = 0;   // last cached value, no recompute
        virtual void Set(T value) = 0;
        operator T() { return Get(); }
    };

    // ----------------------------------------------------------------------
    // CalculatedValue<T> - recomputes Calculate() at most every checkInterval
    // seconds and caches the result in between.
    // ----------------------------------------------------------------------
    template <class T>
    class CalculatedValue : public UntypedValue, public Value<T>
    {
    public:
        CalculatedValue(PsychobotAI* ai, std::string name = "value", int checkInterval = 1)
            : UntypedValue(ai, std::move(name)), _checkInterval(checkInterval), _lastCheckTime(0), _value() { }
        virtual ~CalculatedValue() = default;

        T Get() override
        {
            time_t now = time(nullptr);
            if (!_lastCheckTime || (now - _lastCheckTime) >= _checkInterval)
            {
                _lastCheckTime = now;
                _value = Calculate();
            }
            return _value;
        }

        T LazyGet() override
        {
            if (!_lastCheckTime)
                return Get();
            return _value;
        }

        void Set(T value) override { _value = value; }
        void Reset() override { _lastCheckTime = 0; }
        bool Expired() override { return (time(nullptr) - _lastCheckTime) >= _checkInterval; }

    protected:
        virtual T Calculate() = 0;

        int    _checkInterval;
        time_t _lastCheckTime;
        T      _value;
    };

    // ----------------------------------------------------------------------
    // SingleCalculatedValue<T> - calculated exactly once, then cached forever
    // (until Reset()). Good for things that never change for a bot's lifetime.
    // ----------------------------------------------------------------------
    template <class T>
    class SingleCalculatedValue : public CalculatedValue<T>
    {
    public:
        SingleCalculatedValue(PsychobotAI* ai, std::string name = "value")
            : CalculatedValue<T>(ai, std::move(name)) { this->Reset(); }

        T Get() override
        {
            if (!this->_lastCheckTime)
            {
                this->_lastCheckTime = time(nullptr);
                this->_value = this->Calculate();
            }
            return this->_value;
        }
    };

    // ----------------------------------------------------------------------
    // MemoryCalculatedValue<T> - a CalculatedValue that also tracks the last
    // value seen (so later steps can detect "it changed" reactions).
    // ----------------------------------------------------------------------
    template <class T>
    class MemoryCalculatedValue : public CalculatedValue<T>
    {
    public:
        MemoryCalculatedValue(PsychobotAI* ai, std::string name = "value", int checkInterval = 1)
            : CalculatedValue<T>(ai, std::move(name), checkInterval), _lastValue() { }

        virtual bool EqualToLast(T value) = 0;

        T Get() override
        {
            this->_value = CalculatedValue<T>::Get();
            if (!EqualToLast(this->_value))
                _lastValue = this->_value;
            return this->_value;
        }

        T GetLast() const { return _lastValue; }

    protected:
        T _lastValue;
    };
}

#endif // PSYCHOBOT_ENGINE_VALUE_H
