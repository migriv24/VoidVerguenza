"""check_plaintext_paths.py — ground rule 4, checked: "every path that touches [a plaintext
secret] says so in a comment."

Every function declared or defined under src/ that takes a SecretBuffer, or returns one,
must have a comment in the few lines above it that says what it does with the plaintext:
one of "plaintext", "read only", "receives", "key material". The SecretBuffer type's own
files are exempt (they ARE the path, and say so at length).

A rule nobody checks is a wish (okf/concepts/architecture.md).

    python tools/check_plaintext_paths.py        # non-zero if a path is unexplained
"""
import os
import re
import sys

# a function signature: a return type, a name (possibly Class::name), an opening paren
SIGNATURE = re.compile(r"^\s*(?:virtual\s+|static\s+|inline\s+)*[\w:<>]+[\s\*&]+(?:[\w]+::)*~?\w+\s*\(")
SAYS = re.compile(r"plaintext|read only|receives|key material", re.IGNORECASE)
EXEMPT = ("src/foundation/secret/secret_buffer.hpp", "src/foundation/secret/secret_buffer.cpp")
LOOKBACK = 6


def signature_text(lines, i):
    """The signature starting at line i, up to its ';' or '{' (it may span lines)."""
    text = ""
    for j in range(i, min(i + 6, len(lines))):
        text += lines[j]
        if ";" in lines[j] or "{" in lines[j]:
            break
    return text.split("{")[0]


def check_file(rel, lines):
    problems = []
    for i, line in enumerate(lines):
        if not SIGNATURE.match(line) or line.strip().startswith(("return", "if", "for", "while", "else")):
            continue
        # in a .cpp only top-level definitions count; an indented line is a statement in a
        # body (`SecretBuffer plain(n);` declares a variable, it is not a signature)
        if rel.endswith(".cpp") and line[:1].isspace():
            continue
        sig = signature_text(lines, i)
        params = sig[sig.find("(") :]
        touches = "SecretBuffer" in params or re.match(r"^\s*(?:static\s+)?SecretBuffer\s+\w", line)
        if not touches:
            continue
        above = " ".join(lines[max(0, i - LOOKBACK) : i])
        if not SAYS.search(above):
            name = sig.strip().split("(")[0].split()[-1]
            problems.append(f"{rel}:{i + 1}: {name} touches a SecretBuffer and does not say what it does with the plaintext")
    return problems


def main() -> int:
    root = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
    problems = []
    for folder, _, files in os.walk(os.path.join(root, "src")):
        for name in files:
            if not name.endswith((".hpp", ".cpp")):
                continue
            path = os.path.join(folder, name)
            rel = os.path.relpath(path, root).replace(os.sep, "/")
            if rel in EXEMPT:
                continue
            with open(path, encoding="utf-8") as f:
                problems += check_file(rel, f.read().splitlines())
    for p in problems:
        print(p)
    print(f"check_plaintext_paths: {len(problems)} unexplained path(s)")
    return 1 if problems else 0


if __name__ == "__main__":
    sys.exit(main())
