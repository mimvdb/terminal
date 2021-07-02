// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "atomic.h"

namespace til
{
    template<std::memory_order AcquireOrder = std::memory_order_acquire, std::memory_order ReleaseOrder = std::memory_order_release>
    struct gate
    {
        constexpr explicit gate(const bool desired) noexcept :
            _state(desired)
        {
        }

        gate(const gate&) = delete;
        gate& operator=(const gate&) = delete;

        void acquire() noexcept
        {
            while (!_state.exchange(false, AcquireOrder))
            {
                til::atomic_wait(_state, false);
            }
        }

        void release() noexcept
        {
            _state.store(true, ReleaseOrder);
            til::atomic_notify_all(_state);
        }

        void lower() noexcept
        {
            _state.store(false, ReleaseOrder);
        }

        void raise() noexcept
        {
            _state.store(true, ReleaseOrder);
            til::atomic_notify_all(_state);
        }

        void pass() noexcept
        {
            while (!_state.load(AcquireOrder))
            {
                til::atomic_wait(_state, false);
            }
        }

    private:
        std::atomic<bool> _state;
    };

    using gate_relaxed = gate<std::memory_order_relaxed, std::memory_order_relaxed>;

    struct ticket_lock
    {
        void lock()
        {
            const auto ticket = _next_ticket.fetch_add(1, std::memory_order_relaxed);

            while (true)
            {
                auto current = _now_serving.load(std::memory_order_acquire);
                if (current == ticket)
                {
                    break;
                }

                til::atomic_wait(_now_serving, current);
            }
        }

        void unlock()
        {
            _now_serving.fetch_add(1, std::memory_order_release);
            til::atomic_notify_all(_now_serving);
        }

    private:
        std::atomic<uint32_t> _next_ticket{ 0 };
        std::atomic<uint32_t> _now_serving{ 0 };
    };
}
