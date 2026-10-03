/* foundation/core/status.hpp — success, or a sentence saying why not.
 *
 * Every refusal in Verguenza is a sentence a person can read (the family's "never a
 * silent wrong answer"). A Status is never thrown: the foundation does not use
 * exceptions across its boundary, so a port implemented in C, or a test, sees every
 * failure as a value. */
#pragma once

#include <string>
#include <utility>

namespace verguenza {

class Status {
public:
    static Status ok() { return Status(); }
    static Status fail(std::string why) { return Status(std::move(why)); }

    bool is_ok() const { return ok_; }
    explicit operator bool() const { return ok_; }
    const std::string& error() const { return error_; }

private:
    Status() = default;
    explicit Status(std::string why) : ok_(false), error_(std::move(why)) {}
    bool ok_ = true;
    std::string error_;
};

} // namespace verguenza
