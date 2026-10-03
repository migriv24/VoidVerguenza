# Adapters: each meets one port, each its own library (okf/concepts/architecture.md).
# They point inward: they include foundation/ and application/ ports, never each other.

# Crypto port → Reticulum, through Void Palabra's crypto.hpp (ground rule 3).
add_library(verguenza_adapter_reticulum STATIC reticulum/reticulum_crypto.cpp)
target_link_libraries(verguenza_adapter_reticulum PUBLIC voidverguenza PRIVATE voidpalabra::reticulum)
target_compile_options(verguenza_adapter_reticulum PRIVATE ${VERGUENZA_WARNINGS})

# Vault store port → a Void Palabra Archive (the .verguenza file). cJSON comes from Void
# Maiz's vendor/, the one copy in the build.
add_library(verguenza_adapter_storage STATIC storage/archive_vault_store.cpp)
target_link_libraries(verguenza_adapter_storage PUBLIC verguenza_app PRIVATE voidpalabra voidmaiz)
target_include_directories(verguenza_adapter_storage PRIVATE ${VOIDMAIZ_ROOT}/vendor)
target_compile_options(verguenza_adapter_storage PRIVATE ${VERGUENZA_WARNINGS})

# Model port → Void Core, through Void Maiz's embedded manager.
add_library(verguenza_adapter_model STATIC model/core_model.cpp)
target_link_libraries(verguenza_adapter_model PUBLIC verguenza_app PRIVATE voidmaiz)
target_include_directories(verguenza_adapter_model PRIVATE ${VOIDMAIZ_ROOT}/vendor)
target_compile_options(verguenza_adapter_model PRIVATE ${VERGUENZA_WARNINGS})

# Platform ports: secure memory, key store, clipboard, clock.
if(WIN32)
  add_library(verguenza_adapter_platform STATIC
    platform/win32/secure_memory.cpp
    platform/win32/dpapi_key_store.cpp
    platform/win32/clipboard.cpp
  )
  target_link_libraries(verguenza_adapter_platform PUBLIC verguenza_app PRIVATE crypt32 user32)
  target_compile_definitions(verguenza_adapter_platform PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX UNICODE _UNICODE)
  target_compile_options(verguenza_adapter_platform PRIVATE ${VERGUENZA_WARNINGS})
else()
  # Honest about scope: the key store, clipboard and secure memory exist for Windows only so
  # far. The foundation and application build everywhere; the window does not, yet.
  message(WARNING "Void Verguenza's platform adapters exist for Windows only; the desktop app is not built here.")
endif()
