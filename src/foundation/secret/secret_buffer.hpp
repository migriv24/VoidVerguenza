/* foundation/secret/secret_buffer.hpp — the one type that may hold a plaintext.
 *
 * Ground rule 4: a plaintext secret lives in memory only, and only as long as it is
 * used. Every value, value key, share and private key in Verguenza is a SecretBuffer,
 * so a review can find every place one is held by searching for this name, and
 * tools/check_plaintext_paths.py makes every function that takes or returns one say
 * what it does with the plaintext.
 *
 *   - Memory from the installed SecureMemoryPort (locked, wiped on release).
 *   - Move-only: a copy would be a second plaintext nobody wipes.
 *   - No conversion to std::string. `view()` hands out a string_view for exactly as
 *     long as the caller holds the buffer, and the callers that pass it outward (the
 *     clipboard, the screen, Palabra's crypto calls) carry the ground-rule-4 comment.
 *   - A live count, so a test can check that locking left no plaintext alive
 *     (roadmap step 1B.1).
 *
 * Not done yet, and said so: the founding sketch also made the memory unreadable
 * between uses (VirtualProtect / mprotect). That needs page-granular allocation per
 * buffer and is left for when the memory scan (roadmap 1B.12) shows it matters. */
#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace verguenza {

class SecretBuffer {
public:
    SecretBuffer() = default;
    /* `capacity` zeroed bytes; size() starts at `capacity`. */
    explicit SecretBuffer(std::size_t capacity);
    ~SecretBuffer();

    SecretBuffer(SecretBuffer&& other) noexcept;
    SecretBuffer& operator=(SecretBuffer&& other) noexcept;
    SecretBuffer(const SecretBuffer&) = delete;
    SecretBuffer& operator=(const SecretBuffer&) = delete;

    /* Plaintext enters here: copies `len` bytes from `data` into a new buffer. The
     * caller is responsible for wiping its own source. */
    static SecretBuffer copy_of(const void* data, std::size_t len);

    std::uint8_t* data() { return data_; }
    const std::uint8_t* data() const { return data_; }
    char* chars() { return reinterpret_cast<char*>(data_); }
    std::size_t size() const { return size_; }
    std::size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

    /* Shrink or regrow the logical size within capacity; bytes past the new size are
     * wiped. */
    void resize(std::size_t n);
    /* The length of a NUL-terminated string written into the buffer (by a text box). */
    std::size_t c_string_length() const;

    /* Valid only while this buffer lives and is not resized. */
    std::string_view view() const { return {reinterpret_cast<const char*>(data_), size_}; }

    /* Wipe and release now, leaving an empty buffer. */
    void clear();

    /* Buffers currently holding memory, process-wide. */
    static std::size_t live_count();

private:
    std::uint8_t* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
};

} // namespace verguenza
