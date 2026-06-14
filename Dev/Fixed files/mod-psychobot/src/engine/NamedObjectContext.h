/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  NamedObjectContext - the string-keyed registry the AI uses to resolve named
 *  actions/values/triggers/strategies at runtime.
 *    Qualified              - base for objects that accept a "name::qualifier".
 *    NamedObjectFactory<T>  - maps name -> creator lambda; Create() builds one.
 *    NamedObjectContext<T>  - a factory that caches created objects by name.
 *    NamedObjectContextList<T> - chains several contexts (so a class context
 *                             ADDS to the shared base context).
 *  Clean-room re-implementation for TrinityCore 8.3.
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_NAMEDOBJECTCONTEXT_H
#define PSYCHOBOT_ENGINE_NAMEDOBJECTCONTEXT_H

#include "Define.h"
#include <string>
#include <map>
#include <set>
#include <list>
#include <functional>

namespace psychobot
{
    class PsychobotAI;

    // ----------------------------------------------------------------------
    // Qualified - lets a single named object carry a qualifier, e.g.
    // "cast::frostbolt" or "rti::skull". Parsed from "name::qualifier".
    // ----------------------------------------------------------------------
    class Qualified
    {
    public:
        Qualified() = default;
        explicit Qualified(std::string const& qualifier) : _qualifier(qualifier) { }

        virtual void Qualify(std::string const& qualifier) { _qualifier = qualifier; }
        std::string GetQualifier() const { return _qualifier; }
        void Reset() { _qualifier.clear(); }

    protected:
        std::string _qualifier;
    };

    // ----------------------------------------------------------------------
    // NamedObjectFactory<T> - name -> creator. Splits "name::qualifier" and,
    // if the created object is Qualified, applies the qualifier.
    // ----------------------------------------------------------------------
    template <class T>
    class NamedObjectFactory
    {
    public:
        virtual ~NamedObjectFactory() = default;

        using Creator = std::function<T* (PsychobotAI* ai)>;

        T* Create(std::string const& name, PsychobotAI* ai)
        {
            std::string base = name;
            std::string qualifier;
            if (size_t pos = name.find("::"); pos != std::string::npos)
            {
                qualifier = name.substr(pos + 2);
                base = name.substr(0, pos);
            }

            auto it = _creators.find(base);
            if (it == _creators.end())
                return nullptr;

            T* object = it->second(ai);
            if (!object)
                return nullptr;

            if (!qualifier.empty())
                if (Qualified* q = dynamic_cast<Qualified*>(object))
                    q->Qualify(qualifier);

            return object;
        }

        std::set<std::string> GetCreatorNames() const
        {
            std::set<std::string> names;
            for (auto const& pair : _creators)
                names.insert(pair.first);
            return names;
        }

    protected:
        std::map<std::string, Creator> _creators;
    };

    // ----------------------------------------------------------------------
    // NamedObjectContext<T> - a factory that caches the objects it creates so
    // each named object is built once per bot and reused.
    // ----------------------------------------------------------------------
    template <class T>
    class NamedObjectContext : public NamedObjectFactory<T>
    {
    public:
        NamedObjectContext(bool shared = false, bool supportsSiblings = false)
            : _shared(shared), _supportsSiblings(supportsSiblings) { }

        virtual ~NamedObjectContext() { Clear(); }

        T* GetObject(std::string const& name, PsychobotAI* ai)
        {
            auto it = _created.find(name);
            if (it != _created.end())
                return it->second;

            T* object = this->Create(name, ai);
            _created[name] = object;   // cache (may be nullptr -> "unknown")
            return object;
        }

        bool IsShared() const { return _shared; }
        bool IsSupportsSiblings() const { return _supportsSiblings; }

        void Clear()
        {
            for (auto& pair : _created)
                delete pair.second;
            _created.clear();
        }

        std::set<std::string> GetCreated() const
        {
            std::set<std::string> names;
            for (auto const& pair : _created)
                if (pair.second)
                    names.insert(pair.first);
            return names;
        }

    private:
        std::map<std::string, T*> _created;
        bool _shared;
        bool _supportsSiblings;
    };

    // ----------------------------------------------------------------------
    // NamedObjectContextList<T> - an ordered chain of contexts. GetObject walks
    // the list (most-recently-added first) so a class context can override /
    // extend the shared base context.
    // ----------------------------------------------------------------------
    template <class T>
    class NamedObjectContextList
    {
    public:
        ~NamedObjectContextList()
        {
            for (NamedObjectContext<T>* ctx : _contexts)
                delete ctx;
            _contexts.clear();
        }

        // Takes ownership of the context.
        void Add(NamedObjectContext<T>* context)
        {
            if (context)
                _contexts.push_front(context);   // newest first
        }

        T* GetObject(std::string const& name, PsychobotAI* ai)
        {
            for (NamedObjectContext<T>* ctx : _contexts)
                if (T* object = ctx->GetObject(name, ai))
                    return object;
            return nullptr;
        }

        std::set<std::string> GetAllNames() const
        {
            std::set<std::string> names;
            for (NamedObjectContext<T>* ctx : _contexts)
                for (std::string const& n : ctx->GetCreatorNames())
                    names.insert(n);
            return names;
        }

    private:
        std::list<NamedObjectContext<T>*> _contexts;
    };
}

#endif // PSYCHOBOT_ENGINE_NAMEDOBJECTCONTEXT_H
