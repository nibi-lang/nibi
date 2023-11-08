#pragma once

#include <atomic>

namespace machine {

class spin_lock_c {
public:
    void lock() {
        while (true) {
            bool expected{UNLOCKED};
            if (_flag.compare_exchange_strong(expected, LOCKED))
                break;
        }
    }
    void unlock() {
        _flag.store(UNLOCKED);
    }
private:
    static constexpr bool UNLOCKED = false;
    static constexpr bool LOCKED = true;
    std::atomic<bool> _flag {UNLOCKED};
};

} // namespace
