#include "adapters/platform/win32/clipboard.hpp"

#include <windows.h>

namespace verguenza::adapters {

namespace {

bool put_dword(const wchar_t* format_name, DWORD value) {
    const UINT format = RegisterClipboardFormatW(format_name);
    if (!format) return false;
    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE, sizeof(DWORD));
    if (!h) return false;
    *static_cast<DWORD*>(GlobalLock(h)) = value;
    GlobalUnlock(h);
    if (!SetClipboardData(format, h)) {
        GlobalFree(h);
        return false;
    }
    return true;
}

/* Overwrite the text the clipboard holds before it is released: EmptyClipboard frees
 * that memory without wiping it. */
void wipe_clipboard_text() {
    HANDLE h = GetClipboardData(CF_UNICODETEXT);
    if (!h) return;
    void* p = GlobalLock(h);
    if (p) {
        SecureZeroMemory(p, GlobalSize(h));
        GlobalUnlock(h);
    }
}

} // namespace

Win32Clipboard::Win32Clipboard() {
    window_ = CreateWindowExW(0, L"STATIC", L"verguenza-clipboard", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                              GetModuleHandleW(nullptr), nullptr);
}

Win32Clipboard::~Win32Clipboard() {
    if (window_) DestroyWindow(static_cast<HWND>(window_));
}

// value: plaintext (UTF-8), read only; converted straight into the clipboard's own memory,
// with no intermediate copy in this process.
Status Win32Clipboard::put_secret(const SecretBuffer& value) {
    if (!window_) return Status::fail("the clipboard is not available");
    if (value.empty()) return Status::fail("there is nothing to copy");
    const int chars = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, reinterpret_cast<const char*>(value.data()),
                                          static_cast<int>(value.size()), nullptr, 0);
    if (chars <= 0) return Status::fail("the value is not valid text, so it was not copied");

    HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (static_cast<SIZE_T>(chars) + 1) * sizeof(wchar_t));
    if (!h) return Status::fail("no memory for the clipboard");
    auto* w = static_cast<wchar_t*>(GlobalLock(h));
    if (chars > 0)
        MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(value.data()), static_cast<int>(value.size()), w,
                            chars);
    GlobalUnlock(h);

    if (!OpenClipboard(static_cast<HWND>(window_))) {
        SecureZeroMemory(GlobalLock(h), GlobalSize(h));
        GlobalUnlock(h);
        GlobalFree(h);
        return Status::fail("another program is holding the clipboard; try again");
    }
    EmptyClipboard();
    if (!SetClipboardData(CF_UNICODETEXT, h)) {
        CloseClipboard();
        SecureZeroMemory(GlobalLock(h), GlobalSize(h));
        GlobalUnlock(h);
        GlobalFree(h);
        return Status::fail("Windows refused the clipboard data");
    }
    // the history and cloud exclusions; best effort, as Windows honours them
    put_dword(L"ExcludeClipboardContentFromMonitorProcessing", 0);
    put_dword(L"CanIncludeInClipboardHistory", 0);
    put_dword(L"CanUploadToCloudClipboard", 0);
    CloseClipboard();
    sequence_ = GetClipboardSequenceNumber();
    return Status::ok();
}

void Win32Clipboard::clear_if_unchanged() {
    if (!sequence_ || GetClipboardSequenceNumber() != sequence_) {
        sequence_ = 0; // someone copied something else since: leave it alone
        return;
    }
    if (OpenClipboard(static_cast<HWND>(window_))) {
        wipe_clipboard_text();
        EmptyClipboard();
        CloseClipboard();
    }
    sequence_ = 0;
}

} // namespace verguenza::adapters
