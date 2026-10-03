/* adapters/model/core_model.hpp — the model port, met by Void Core through Void Maiz's
 * embedded manager (maiz::Core). Ground rule 5: the model lives in Void Core, and every
 * change is a dispatcher command; this adapter is how the application reaches it.
 *
 * Arguments are quoted with Void Core's own codec (vc_arg_quote, SPEC §6.1), never by
 * hand: five implementations got §6.1 wrong before Core shipped it as code. */
#pragma once

#include "application/ports/ports.hpp"

#include <functional>
#include <memory>

namespace maiz {
class Core;
}

namespace verguenza::adapters {

class CoreModel final : public app::ModelPort {
public:
    CoreModel();
    ~CoreModel() override;

    Status reset(std::string_view state_json) override;
    std::string export_state() override;
    Status dispatch(std::string_view command) override;
    std::string quote(std::string_view value) override;
    std::vector<app::Description> descriptions(const std::string& mantle) override;

    /* Every dispatched line and its reply, for the window's log strip. The lines carry
     * descriptions only: no command ever carries a value (commands.md). */
    void set_log(std::function<void(const std::string& line)> log) { log_ = std::move(log); }

private:
    std::unique_ptr<maiz::Core> core_;
    std::function<void(const std::string&)> log_;
};

} // namespace verguenza::adapters
