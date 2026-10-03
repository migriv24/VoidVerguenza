#include "adapters/model/core_model.hpp"

#include "voidmaiz/embed.hpp"

#include "cJSON.h"
#include "voidcore.h"

namespace verguenza::adapters {

CoreModel::CoreModel() : core_(std::make_unique<maiz::Core>("")) {}
CoreModel::~CoreModel() = default;

Status CoreModel::reset(std::string_view state_json) {
    core_ = std::make_unique<maiz::Core>(state_json);
    return Status::ok();
}

std::string CoreModel::export_state() { return core_->export_state(); }

Status CoreModel::dispatch(std::string_view command) {
    const maiz::Result r = core_->dispatch(command);
    if (log_) log_(std::string(r.ok ? "ok   " : "FAIL ") + std::string(command));
    if (!r.ok) return Status::fail("Void Core refused `" + std::string(command) + "`: " + r.text());
    return Status::ok();
}

std::string CoreModel::quote(std::string_view value) {
    const std::string v(value);
    char* q = vc_arg_quote(v.c_str());
    if (!q) return "''";
    std::string out(q);
    vc_free_str(q);
    return out;
}

std::vector<app::Description> CoreModel::descriptions(const std::string& mantle) {
    std::vector<app::Description> out;
    const std::string state = core_->export_state();
    cJSON* root = cJSON_Parse(state.c_str());
    if (!root) return out;
    const cJSON* mantles = cJSON_GetObjectItemCaseSensitive(root, "mantles");
    const cJSON* m = nullptr;
    cJSON_ArrayForEach(m, mantles) {
        const cJSON* name = cJSON_GetObjectItemCaseSensitive(m, "name");
        if (!cJSON_IsString(name) || mantle != name->valuestring) continue;
        const cJSON* rune = nullptr;
        cJSON_ArrayForEach(rune, cJSON_GetObjectItemCaseSensitive(m, "runes")) {
            app::Description d;
            const cJSON* spirit = cJSON_GetObjectItemCaseSensitive(rune, "spirit");
            const cJSON* rname = cJSON_GetObjectItemCaseSensitive(spirit, "name");
            const cJSON* glyph = cJSON_GetObjectItemCaseSensitive(rune, "glyph");
            if (!cJSON_IsString(rname) || !cJSON_IsString(glyph)) continue;
            d.id = rname->valuestring;
            d.kind = glyph->valuestring;
            const cJSON* field = nullptr;
            cJSON_ArrayForEach(field, cJSON_GetObjectItemCaseSensitive(rune, "content")) {
                if (cJSON_IsString(field)) d.fields[field->string] = field->valuestring;
            }
            const cJSON* tag = nullptr;
            cJSON_ArrayForEach(tag, cJSON_GetObjectItemCaseSensitive(rune, "tags")) {
                if (cJSON_IsString(tag)) d.tags.push_back(tag->valuestring);
            }
            out.push_back(std::move(d));
        }
    }
    cJSON_Delete(root);
    return out;
}

} // namespace verguenza::adapters
