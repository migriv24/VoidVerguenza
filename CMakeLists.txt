cmake_minimum_required(VERSION 3.20)
# Void Verguenza — secrets several people share. okf/index.md is the source of truth;
# okf/concepts/architecture.md is the layout this file builds.
project(voidverguenza VERSION 0.1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ── The toolchain the family's Reticulum forces ─────────────────────────────
# Void Palabra's Reticulum companion cannot build under MSVC (its vendored Crypto
# uses GCC statement expressions), and every key Verguenza makes goes through it.
if(MSVC)
  message(FATAL_ERROR "Void Verguenza builds with GCC or Clang (on Windows: MSYS2 UCRT64). "
                      "MSVC cannot build Void Palabra's Reticulum companion. See okf/concepts/architecture.md.")
endif()

# A static runtime on MinGW: a stale libstdc++ on PATH otherwise kills the program at
# load with no message (Void Hormiga learned this; one place, not nine).
set(VERGUENZA_STATIC_RUNTIME "")
if(MINGW)
  set(VERGUENZA_STATIC_RUNTIME -static)
endif()

# Warnings for OUR code only; a sibling's tree is not ours to police.
set(VERGUENZA_WARNINGS -Wall -Wextra -Wpedantic)

# ── Siblings, the family's way: add_subdirectory, paths as cache variables ──
set(VOIDMAIZ_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../VoidMaiz" CACHE PATH "Path to the Void Maiz repository")
set(VOIDPALABRA_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../VoidPalabra" CACHE PATH "Path to the Void Palabra repository")
foreach(sibling VOIDMAIZ_ROOT VOIDPALABRA_ROOT)
  if(NOT EXISTS "${${sibling}}/CMakeLists.txt")
    message(FATAL_ERROR "${sibling} (${${sibling}}) is not a checkout. Void Verguenza builds beside "
                        "../VoidMaiz and ../VoidPalabra; set ${sibling} if yours is elsewhere.")
  endif()
endforeach()

# Ground rule 4: microReticulum's trace level (8) prints plaintext and derived keys.
set(VOIDPALABRA_RNS_LOG_LEVEL 6 CACHE STRING "microReticulum's compiled-in log ceiling (0-9)")
if(VOIDPALABRA_RNS_LOG_LEVEL GREATER_EQUAL 8)
  message(FATAL_ERROR "VOIDPALABRA_RNS_LOG_LEVEL=${VOIDPALABRA_RNS_LOG_LEVEL}: level 8 and above log "
                      "plaintext and keys. Void Verguenza refuses to build with it (ground rule 4).")
endif()

# Void Maiz brings Void Core, and (when ../VoidPalabra is present) Void Palabra with its
# Reticulum companion. One copy of each in the build.
add_subdirectory("${VOIDMAIZ_ROOT}" voidmaiz)
if(NOT TARGET voidpalabra_reticulum)
  message(FATAL_ERROR "Void Palabra's Reticulum companion was not built. Void Verguenza's "
                      "cryptography is Reticulum's (ground rule 3).")
endif()
# A sibling's headers are theirs to keep warning-free: marked SYSTEM so our -Wall -Wextra
# reports only on our code (ground rule 7: we do not edit a sibling to quiet our build).
if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.25)
  foreach(t voidmaiz voidmaiz_imgui voidmaiz_headless voidpalabra voidpalabra_reticulum)
    if(TARGET ${t})
      set_target_properties(${t} PROPERTIES SYSTEM TRUE)
    endif()
  endforeach()
endif()
# Palabra's public header forward-declares cJSON; its copy comes from Void Maiz's vendor/.
target_include_directories(voidpalabra PUBLIC ${VOIDMAIZ_ROOT}/vendor)

# The entropy wiring Verguenza depends on (Palabra defect 11): fail at configure time,
# not at the first key, if the sibling checkout predates it.
if(NOT EXISTS "${VOIDPALABRA_ROOT}/include/voidpalabra/crypto.hpp")
  message(FATAL_ERROR "../VoidPalabra has no include/voidpalabra/crypto.hpp: it predates the identity "
                      "operations and OS-seeded randomness Void Verguenza needs. Update the checkout.")
endif()

# ── The hexagon (okf/concepts/architecture.md §"The code layout") ──────────
add_subdirectory(src/foundation)    # the domain, std only
add_subdirectory(src/application)   # use cases and their ports
add_subdirectory(src/adapters)      # each port met, one library each
add_subdirectory(src/desktop)       # the Void Maiz window, thin

# ── Tests and checks ────────────────────────────────────────────────────────
include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
