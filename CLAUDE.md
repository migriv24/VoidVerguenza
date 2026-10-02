# Void Verguenza — agent instructions

You are working on **Void Verguenza**, a native C++20 desktop application for
keeping **secrets that several people share**: passwords, API keys, recovery
codes, Wi-Fi passwords, the admin login of a camera recorder. It is built on
**Void Core** (`../VoidCore`, the engine), **Void Maiz** (`../VoidMaiz`, the
view library) and **Void Palabra** (`../VoidPalabra`, sync, and the family's
networking over Reticulum).

**Vergüenza** — shame. The things you would be ashamed to have leaked. Code and
paths spell it `verguenza`; documents may write *Vergüenza*.

## Ground rules

1. **OKF first.** `okf/` is the source of truth. Code follows concepts. Every
   session that changes design or code records it in `okf/log.md` and trues up
   the concept pages it touched. Read `okf/index.md` before doing anything.
2. **The repo root is a public artifact.** No secret, no vault file, no test
   fixture holding a real password, no key file, ever, in any commit. Test
   secrets are generated at test time and are obviously fake.
3. **Cryptography comes from Reticulum, through Void Palabra** (the author,
   2026-10-01: *"reticulum as much as possible, not libsodium"*). Never hand-roll
   a primitive. Never add a fallback secret, a master backdoor or a "recovery"
   that works without the people the design says it needs. Where Reticulum has
   no primitive the design needs (a password-hardening function, threshold
   sharing), the choice is an open question for the author
   (`okf/developer_questions.md`), not a quiet implementation; libsodium appears
   only where such a question's answer says so. Operating-system facilities
   (locked and wiped memory, the system's random generator, the key store) are
   not primitives and may be called directly.
3a. **Palabra first.** Anything that can be stated without the words *person*,
   *secret*, *consent* or *knowledge* (history, merging, storage, transport,
   identity, capabilities as a mechanism) is Void Palabra's, and Verguenza asks
   for it rather than building it (`okf/concepts/boundary.md`).
4. **A plaintext secret lives in memory only, and only as long as it is used.**
   It is never written to the state document, the dispatcher log, a crash
   report, the clipboard for longer than a set time, or a temporary file. Every
   path that touches one says so in a comment.
5. **The model lives in Void Core; the dispatcher is the only door.** Every
   change — a secret added, a device enrolled, an approval given, a reveal — is a
   logged, replayable dispatcher command. The *value* of a secret is never in a
   command; its sealed envelope is referenced by content address.
6. **Honest about what crypto enforces and what software enforces.** A policy
   that a modified client could ignore is labelled as software-enforced in the
   design, the code and the user interface. Only math gets to be called a
   guarantee.
7. **Upstream messages, not upstream edits.** Void Core, Void Maiz and Void
   Palabra gaps are drafted at the repo root as
   `MESSAGE_FOR_<RECIPIENT>_verguenza-<topic>-<YYYY-MM-DD>.md` for the author to
   relay. (The author may grant direct edits to a sibling; that grant is
   recorded in the log when given.)
8. **The author decides.** Open decisions go to `okf/developer_questions.md`
   with a lean. Answers fold into concepts and clear from there.

## Where it came from

Founded 2026-10-01 out of a Void Hormiga session. Void Hormiga's Antfarm v2 has
a **Key** node: a shared credential whose value lives sealed in each member's
vault (`../VoidHormiga/okf/concepts/platform/antfarm/v2/keys.md`). The author:
*"eventually the antfarm will reference Verguenza's structures to use instead of
our current api key node."* So Hormiga is Verguenza's first client, and nothing
in Verguenza may depend on Hormiga.
