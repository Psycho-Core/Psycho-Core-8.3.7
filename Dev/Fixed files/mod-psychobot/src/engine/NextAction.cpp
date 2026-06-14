/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "NextAction.h"
#include <cstdarg>

namespace psychobot
{
    int NextAction::Size(NextAction** actions)
    {
        if (!actions)
            return 0;
        int size = 0;
        while (actions[size])
            ++size;
        return size;
    }

    NextAction** NextAction::Clone(NextAction** actions)
    {
        if (!actions)
            return nullptr;

        int size = Size(actions);
        NextAction** out = new NextAction*[size + 1];
        for (int i = 0; i < size; ++i)
            out[i] = new NextAction(*actions[i]);
        out[size] = nullptr;
        return out;
    }

    NextAction** NextAction::Merge(NextAction** what, NextAction** with)
    {
        int sizeWhat = Size(what);
        int sizeWith = Size(with);

        NextAction** out = new NextAction*[sizeWhat + sizeWith + 1];
        int idx = 0;
        for (int i = 0; i < sizeWhat; ++i)
            out[idx++] = new NextAction(*what[i]);
        for (int i = 0; i < sizeWith; ++i)
            out[idx++] = new NextAction(*with[i]);
        out[idx] = nullptr;

        // The two inputs were owned by the caller's chain; free them now that
        // their contents have been copied into the merged result.
        Destroy(what);
        Destroy(with);
        return out;
    }

    NextAction** NextAction::Array(uint32 n, ...)
    {
        NextAction** out = new NextAction*[n + 1];

        va_list args;
        va_start(args, n);
        for (uint32 i = 0; i < n; ++i)
            out[i] = va_arg(args, NextAction*);
        va_end(args);

        out[n] = nullptr;
        return out;
    }

    void NextAction::Destroy(NextAction** actions)
    {
        if (!actions)
            return;
        for (int i = 0; actions[i]; ++i)
            delete actions[i];
        delete[] actions;
    }
}
