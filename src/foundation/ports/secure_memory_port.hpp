/* foundation/ports/secure_memory_port.hpp — where plaintexts and keys are kept.
 *
 * A port, because locking memory and wiping it in a way the compiler will not remove
 * are the operating system's facilities (VirtualLock / SecureZeroMemory, mlock /
 * explicit_bzero), and the foundation knows no operating system. The Windows adapter
 * is adapters/platform/win32/secure_memory.cpp; the fallback below is plain heap memory
 * wiped through a volatile pointer, for tests and for platforms without an adapter yet.
 *
 * Installed once per process (`install_secure_memory`), because every SecretBuffer
 * anywhere must come from the same place. */
#pragma once

#include <cstddef>

namespace verguenza {

class SecureMemoryPort {
public:
    virtual ~SecureMemoryPort() = default;
    /* `len` bytes, zeroed, kept out of swap where the platform allows. nullptr on failure. */
    virtual void* allocate(std::size_t len) = 0;
    /* Wipe, unlock and free what allocate returned. */
    virtual void release(void* p, std::size_t len) = 0;
    /* A sentence for the screen: what this memory protects against, honestly. */
    virtual const char* description() const = 0;
};

/* The process-wide port. Not owned: the caller keeps it alive for the process. */
void install_secure_memory(SecureMemoryPort* port);
SecureMemoryPort& secure_memory();

/* Overwrite memory in a way the compiler will not remove. */
void wipe(void* p, std::size_t len);

} // namespace verguenza
