/* adapters/platform/win32/secure_memory.hpp — the secure memory port on Windows:
 * VirtualAlloc'd pages, VirtualLock'd so they stay out of the page file where Windows
 * allows it (a request, not a guarantee: okf/concepts/sealing.md), SecureZeroMemory'd
 * and unlocked before they are freed. */
#pragma once

#include "foundation/ports/secure_memory_port.hpp"

namespace verguenza::adapters {

class Win32SecureMemory final : public SecureMemoryPort {
public:
    void* allocate(std::size_t len) override;
    void release(void* p, std::size_t len) override;
    const char* description() const override;
};

} // namespace verguenza::adapters
