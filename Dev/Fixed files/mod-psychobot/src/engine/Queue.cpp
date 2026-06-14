/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 * ===========================================================================
 */

#include "Queue.h"

namespace psychobot
{
    Queue::~Queue() { Clear(); }

    void Queue::Push(ActionBasket* basket)
    {
        if (!basket)
            return;

        // If a basket for the same action node already exists, keep the higher
        // relevance one instead of queuing a duplicate.
        for (ActionBasket* existing : _baskets)
        {
            if (existing->GetAction() == basket->GetAction())
            {
                if (basket->GetRelevance() > existing->GetRelevance())
                    existing->SetRelevance(basket->GetRelevance());
                delete basket;
                return;
            }
        }
        _baskets.push_back(basket);
    }

    ActionBasket* Queue::Peek()
    {
        ActionBasket* best = nullptr;
        for (ActionBasket* b : _baskets)
            if (!best || b->GetRelevance() > best->GetRelevance())
                best = b;
        return best;
    }

    ActionNode* Queue::Pop(ActionBasket** outBasket)
    {
        ActionBasket* best = Peek();
        if (!best)
        {
            if (outBasket)
                *outBasket = nullptr;
            return nullptr;
        }

        _baskets.remove(best);
        ActionNode* node = best->GetAction();

        if (outBasket)
            *outBasket = best;       // caller takes ownership
        else
            delete best;

        return node;
    }

    int Queue::Size() const
    {
        return static_cast<int>(_baskets.size());
    }

    void Queue::RemoveExpired(time_t secs)
    {
        for (auto it = _baskets.begin(); it != _baskets.end(); )
        {
            if ((*it)->IsExpired(secs))
            {
                delete *it;
                it = _baskets.erase(it);
            }
            else
                ++it;
        }
    }

    void Queue::Clear()
    {
        for (ActionBasket* b : _baskets)
            delete b;
        _baskets.clear();
    }
}
