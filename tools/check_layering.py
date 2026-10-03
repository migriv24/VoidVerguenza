"""check_layering.py — the hexagon's dependencies point inward, and nothing but the
Reticulum adapter names a cryptographic library (okf/concepts/architecture.md).

    foundation/   includes foundation/ and the standard library only
    application/  includes foundation/ and application/
    adapters/X/   includes foundation/, application/, adapters/X/ and its own outside
                  library; never another adapter, never desktop/
    desktop/      includes foundation/, application/, desktop/ and the view library;
                  only main.cpp (the composition root) includes adapters/

And ground rule 3, checked rather than trusted: no file outside adapters/reticulum/
includes Palabra's crypto.hpp or names a cipher or curve, and nothing anywhere includes
libsodium.

    python tools/check_layering.py        # non-zero on any violation
"""
import os
import re
import sys

INCLUDE = re.compile(r'^\s*#\s*include\s*([<"])([^>"]+)[>"]')
CRYPTO_WORDS = re.compile(r"\b(Curve25519|Ed25519|X25519|AES|ChaCha|Poly1305|HKDF|HMAC|sodium)\b")
OUTSIDE = ("voidpalabra/", "voidmaiz/", "voidcore", "imgui", "GLFW/", "cJSON", "windows.h", "dpapi.h")


def layer_of(rel):
    parts = rel.split("/")
    if parts[0] != "src":
        return None, None
    if parts[1] == "adapters":
        return "adapters", "/".join(parts[1:3])
    return parts[1], None


def violations_for(rel, text):
    out = []
    layer, adapter = layer_of(rel)
    if layer is None:
        return out
    for lineno, line in enumerate(text.splitlines(), 1):
        m = INCLUDE.match(line)
        if not m:
            continue
        inc = m.group(2)
        ours = m.group(1) == '"' and inc.split("/")[0] in ("foundation", "application", "adapters", "desktop")
        where = f"{rel}:{lineno}: "
        if layer == "foundation":
            if ours and not inc.startswith("foundation/"):
                out.append(where + f"the foundation includes {inc}")
            if not ours and any(inc.startswith(o) or o in inc for o in OUTSIDE):
                out.append(where + f"the foundation includes an outside library ({inc})")
        elif layer == "application":
            if ours and not inc.startswith(("foundation/", "application/")):
                out.append(where + f"the application includes {inc}")
            if not ours and any(inc.startswith(o) or o in inc for o in OUTSIDE):
                out.append(where + f"the application includes an outside library ({inc})")
        elif layer == "adapters":
            if ours and inc.startswith("adapters/") and not inc.startswith(adapter + "/"):
                out.append(where + f"one adapter includes another ({inc})")
            if ours and inc.startswith("desktop/"):
                out.append(where + f"an adapter includes the desktop ({inc})")
        elif layer == "desktop":
            if ours and inc.startswith("adapters/") and not rel.endswith("desktop/main.cpp"):
                out.append(where + f"only main.cpp (the composition root) may include adapters ({inc})")
        # ground rule 3
        if "voidpalabra/crypto.hpp" in inc and not rel.startswith("src/adapters/reticulum/"):
            out.append(where + "Palabra's crypto.hpp outside the Reticulum adapter")
        if "sodium" in inc:
            out.append(where + "libsodium (the author, 2026-10-01: no libsodium)")
    if not rel.startswith("src/adapters/reticulum/"):
        for lineno, line in enumerate(text.splitlines(), 1):
            code = line.split("//")[0]
            if code.lstrip().startswith("*") or code.lstrip().startswith("/*"):
                continue
            m = CRYPTO_WORDS.search(code)
            if m:
                out.append(f"{rel}:{lineno}: names {m.group(1)} outside the Reticulum adapter")
    return out


def main() -> int:
    root = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
    found = []
    for folder, _, files in os.walk(os.path.join(root, "src")):
        for name in files:
            if not name.endswith((".hpp", ".cpp", ".h")):
                continue
            path = os.path.join(folder, name)
            rel = os.path.relpath(path, root).replace(os.sep, "/")
            with open(path, encoding="utf-8") as f:
                found += violations_for(rel, f.read())
    for v in found:
        print(v)
    print(f"check_layering: {len(found)} violation(s)")
    return 1 if found else 0


if __name__ == "__main__":
    sys.exit(main())
