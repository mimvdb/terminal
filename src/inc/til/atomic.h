// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

namespace til
{
    template<typename T>
    inline void atomic_wait(const std::atomic<T>& atomic, const T& current, DWORD waitMilliseconds = INFINITE)
    {
        static_assert(sizeof(atomic) == sizeof(current));
        WaitOnAddress(const_cast<std::atomic<T>*>(&atomic), const_cast<T*>(&current), sizeof(current), waitMilliseconds);
    }

    template<typename T>
    inline void atomic_notify_one(const std::atomic<T>& atomic)
    {
        WakeByAddressSingle(const_cast<std::atomic<T>*>(&atomic));
    }

    template<typename T>
    inline void atomic_notify_all(const std::atomic<T>& atomic)
    {
        WakeByAddressAll(const_cast<std::atomic<T>*>(&atomic));
    }
}
