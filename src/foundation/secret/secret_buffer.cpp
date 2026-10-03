#include "foundation/secret/secret_buffer.hpp"

#include "foundation/ports/secure_memory_port.hpp"

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <new>

namespace verguenza {

namespace {

/* The fallback: heap memory, zeroed, wiped through a volatile pointer on release. It
 * does not keep pages out of swap; its description says so. */
class HeapSecureMemory final : public SecureMemoryPort {
public:
    void* allocate(std::size_t len) override { return std::calloc(len ? len : 1, 1); }
    void release(void* p, std::size_t len) override {
        wipe(p, len);
        std::free(p);
    }
    const char* description() const override {
        return "wiped after use; not locked against the system's swap file";
    }
};

HeapSecureMemory g_heap;
std::atomic<SecureMemoryPort*> g_port{&g_heap};
std::atomic<std::size_t> g_live{0};

} // namespace

void install_secure_memory(SecureMemoryPort* port) { g_port.store(port ? port : &g_heap); }
SecureMemoryPort& secure_memory() { return *g_port.load(); }

void wipe(void* p, std::size_t len) {
    volatile std::uint8_t* v = static_cast<volatile std::uint8_t*>(p);
    for (std::size_t i = 0; i < len; ++i) v[i] = 0;
}

SecretBuffer::SecretBuffer(std::size_t capacity) {
    if (capacity == 0) return;
    data_ = static_cast<std::uint8_t*>(secure_memory().allocate(capacity));
    if (!data_) throw std::bad_alloc();
    size_ = capacity_ = capacity;
    ++g_live;
}

SecretBuffer::~SecretBuffer() { clear(); }

SecretBuffer::SecretBuffer(SecretBuffer&& other) noexcept
    : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
    other.data_ = nullptr;
    other.size_ = other.capacity_ = 0;
}

SecretBuffer& SecretBuffer::operator=(SecretBuffer&& other) noexcept {
    if (this != &other) {
        clear();
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        other.data_ = nullptr;
        other.size_ = other.capacity_ = 0;
    }
    return *this;
}

// plaintext: copied from the caller's memory into a fresh locked buffer.
SecretBuffer SecretBuffer::copy_of(const void* data, std::size_t len) {
    SecretBuffer b(len);
    if (len) std::memcpy(b.data_, data, len);
    return b;
}

void SecretBuffer::resize(std::size_t n) {
    if (n > capacity_) n = capacity_;
    if (n < size_) wipe(data_ + n, size_ - n);
    size_ = n;
}

std::size_t SecretBuffer::c_string_length() const {
    std::size_t n = 0;
    while (n < capacity_ && data_[n] != 0) ++n;
    return n;
}

void SecretBuffer::clear() {
    if (!data_) return;
    secure_memory().release(data_, capacity_);
    data_ = nullptr;
    size_ = capacity_ = 0;
    --g_live;
}

std::size_t SecretBuffer::live_count() { return g_live.load(); }

} // namespace verguenza
