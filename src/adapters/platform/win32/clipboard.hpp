/* adapters/platform/win32/clipboard.hpp — the clipboard port on Windows.
 *
 * okf/concepts/sealing.md §"Plaintext in memory": a copied value is marked so Windows'
 * clipboard history and cloud clipboard skip it (the formats
 * ExcludeClipboardContentFromMonitorProcessing, CanIncludeInClipboardHistory = 0,
 * CanUploadToCloudClipboard = 0, set beside the text), and is wiped and cleared after the
 * timer if the clipboard still holds it. GLFW's clipboard call cannot set those formats,
 * which is why this is Win32 code. Other programs can still read the clipboard while
 * the value is on it; the threat model says so. */
#pragma once

#include "application/ports/ports.hpp"

namespace verguenza::adapters {

class Win32Clipboard final : public app::ClipboardPort {
public:
    Win32Clipboard();
    ~Win32Clipboard() override;
    /* value: plaintext, read only; converted into the clipboard's own memory. */
    Status put_secret(const SecretBuffer& value) override;
    void clear_if_unchanged() override;

private:
    void* window_ = nullptr;        // a message-only window: SetClipboardData needs an owner
    unsigned long sequence_ = 0;    // the clipboard's sequence number after our put
};

} // namespace verguenza::adapters
