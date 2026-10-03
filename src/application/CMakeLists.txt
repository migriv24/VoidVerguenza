# The application: use cases and the ports they need (okf/concepts/architecture.md).
# Depends on the foundation only; adapters meet its ports from outside.
add_library(verguenza_app STATIC
  vault_service.cpp
)
add_library(verguenza::app ALIAS verguenza_app)
target_link_libraries(verguenza_app PUBLIC voidverguenza)
target_compile_options(verguenza_app PRIVATE ${VERGUENZA_WARNINGS})
