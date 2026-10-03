# Void Verguenza

A desktop application for keeping secrets that several people share —
passwords, API keys, recovery codes — encrypted, local-first, synced between
the people who share them over Reticulum, with actions that can require more
than one device to approve.

**Status: a first desktop vault runs** (one device, Windows). The design lives in
[`okf/`](okf/index.md), starting at [`okf/index.md`](okf/index.md); what is built and what
is not is in [`okf/roadmap.md`](okf/roadmap.md) under "Progress".

## Build and run (Windows)

Verguenza builds with GCC under MSYS2 UCRT64 (MSVC cannot build Void Palabra's Reticulum
companion), beside `../VoidMaiz` and `../VoidPalabra`:

```
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build
ctest --test-dir build -L verguenza      # Verguenza's own tests and checks
build/bin/voidverguenza-app.exe
```

The app keeps its vault at `%APPDATA%/VoidVerguenza/default.verguenza` and this device's
key at `%APPDATA%/VoidVerguenza/device.identity`, protected by Windows for your account
(DPAPI). The lock screen says what that does and does not stop.

## Layout

A hexagon ([`okf/concepts/architecture.md`](okf/concepts/architecture.md)):
`src/foundation` (the domain, standard library only), `src/application` (use cases and
their ports), `src/adapters` (Reticulum through Palabra, the Palabra archive, Void Core
through Maiz, Windows), `src/desktop` (the thin Maiz window), `tests/`, `tools/`.

Part of the Void family: built on [Void Core](https://github.com/migriv24/void-core),
[Void Maiz](https://github.com/migriv24/VoidMaiz) and
[Void Palabra](https://github.com/migriv24/VoidPalabra). Its first client will
be [Void Hormiga](https://github.com/migriv24/VoidHormiga).
