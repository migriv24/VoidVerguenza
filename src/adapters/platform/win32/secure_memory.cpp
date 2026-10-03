#include "adapters/platform/win32/secure_memory.hpp"

#include <windows.h>

namespace verguenza::adapters {

/* Each buffer gets its own pages. Wasteful for small buffers, and deliberately so: it
 * keeps every plaintext on pages nothing else shares, which is what a later step needs
 * to make them unreadable between uses (secret_buffer.hpp, "not done yet"). */
void* Win32SecureMemory::allocate(std::size_t len) {
    if (len == 0) len = 1;
    void* p = VirtualAlloc(nullptr, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!p) return nullptr;
    VirtualLock(p, len); // may fail past the working-set limit; the memory is still wiped on release
    return p;            // VirtualAlloc returns zeroed pages
}

void Win32SecureMemory::release(void* p, std::size_t len) {
    if (!p) return;
    if (len == 0) len = 1;
    SecureZeroMemory(p, len);
    VirtualUnlock(p, len);
    VirtualFree(p, 0, MEM_RELEASE);
}

const char* Win32SecureMemory::description() const {
    return "locked against the page file where Windows allows, and wiped after use";
}

} // namespace verguenza::adapters
