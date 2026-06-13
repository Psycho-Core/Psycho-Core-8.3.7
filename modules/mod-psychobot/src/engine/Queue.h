/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  Queue - the engine's relevance-ordered work list for one tick. Triggers and
 *  default actions push ActionBaskets in; the engine repeatedly Pop()s the
 *  highest-relevance basket until one executes (or the queue drains).
 * ===========================================================================
 */

#ifndef PSYCHOBOT_ENGINE_QUEUE_H
#define PSYCHOBOT_ENGINE_QUEUE_H

#include "ActionBasket.h"
#include <list>

namespace psychobot
{
    class ActionNode;

    class Queue
    {
    public:
        Queue() = default;
        ~Queue();

        // Take ownership of the basket.
        void Push(ActionBasket* basket);

        // Remove and return the highest-relevance basket's action node.
        // If `outBasket` is given it receives the popped basket (caller frees).
        ActionNode* Pop(ActionBasket** outBasket = nullptr);

        // Highest-relevance basket without removing it (nullptr if empty).
        ActionBasket* Peek();

        int  Size() const;
        bool Empty() const { return _baskets.empty(); }

        // Drop baskets older than `secs` seconds.
        void RemoveExpired(time_t secs);

        // Free everything.
        void Clear();

    private:
        std::list<ActionBasket*> _baskets;
    };
}

#endif // PSYCHOBOT_ENGINE_QUEUE_H
