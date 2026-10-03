"""check_okf_links.py — every OKF-absolute link resolves to a file that exists.

Ground rule 1 says okf/ is the source of truth, and a source of truth with dead pointers
lies with confidence: the next reader follows a link, finds nothing, and either invents
what the page said or works without it. So a dead link fails the build.

Checked: markdown links of the form `](/concepts/x.md)`, this OKF's way of pointing at
itself. Sibling references (`VoidPalabra:/concepts/...`) are plain text by convention and
are not checked: a sibling may be absent from a checkout.

    python tools/check_okf_links.py        # non-zero if any link is dead
"""
import os
import re
import sys

LINK = re.compile(r"\]\((/[A-Za-z0-9_./-]+\.md)\)")


def main() -> int:
    root = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
    okf = os.path.join(root, "okf")
    dead = []
    for folder, _, files in os.walk(okf):
        for name in files:
            if not name.endswith(".md"):
                continue
            path = os.path.join(folder, name)
            with open(path, encoding="utf-8") as f:
                for lineno, line in enumerate(f, 1):
                    for target in LINK.findall(line):
                        if not os.path.isfile(os.path.join(okf, target.lstrip("/"))):
                            dead.append(f"{os.path.relpath(path, root)}:{lineno}: dead link {target}")
    for d in dead:
        print(d)
    print(f"check_okf_links: {len(dead)} dead link(s)")
    return 1 if dead else 0


if __name__ == "__main__":
    sys.exit(main())
