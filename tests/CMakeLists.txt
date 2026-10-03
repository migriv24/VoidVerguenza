# Tests, one folder per layer, plus the checks that keep the ground rules true
# (okf/concepts/architecture.md §"Tests that keep the ground rules true").

function(verguenza_test name source)
  add_executable(${name} ${source})
  target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
  target_link_libraries(${name} PRIVATE ${ARGN})
  target_compile_options(${name} PRIVATE ${VERGUENZA_WARNINGS})
  target_link_options(${name} PRIVATE ${VERGUENZA_STATIC_RUNTIME})
  add_test(NAME ${name} COMMAND ${name})
  set_tests_properties(${name} PROPERTIES LABELS verguenza)
endfunction()

# the foundation alone: no crypto, no Void Core
verguenza_test(test_codec foundation/codec_test.cpp voidverguenza)
verguenza_test(test_formula foundation/formula_test.cpp voidverguenza)

# the foundation with the real Reticulum adapter
verguenza_test(test_sealing integration/sealing_test.cpp voidverguenza verguenza_adapter_reticulum)

# the use cases end to end: real crypto, real Void Core, in-memory store and key store
verguenza_test(test_vault_service application/vault_service_test.cpp
  verguenza_app verguenza_adapter_reticulum verguenza_adapter_model)

# the real adapters on disk (Windows): DPAPI and the Palabra archive, grepped for plaintext
if(TARGET verguenza_adapter_platform)
  verguenza_test(test_platform adapters/platform_test.cpp
    verguenza_app verguenza_adapter_reticulum verguenza_adapter_model verguenza_adapter_storage
    verguenza_adapter_platform)
endif()

# ── the checks ──────────────────────────────────────────────────────────────
find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
  foreach(check check_okf_links check_layering check_plaintext_paths)
    add_test(NAME ${check} COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tools/${check}.py
             WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    set_tests_properties(${check} PROPERTIES LABELS verguenza)
  endforeach()
else()
  message(WARNING "Python 3 not found: the OKF, layering and plaintext-path checks are not run.")
endif()
