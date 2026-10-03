/* adapters/platform/system_clock.hpp — the clock port: the system's wall clock, recorded
 * on objects for people and never consulted for order (okf/concepts/formats.md). */
#pragma once

#include "application/ports/ports.hpp"

#include <chrono>

namespace verguenza::adapters {

class SystemClock final : public app::ClockPort {
public:
    std::int64_t now_ms() override {
        using namespace std::chrono;
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    }
};

} // namespace verguenza::adapters
