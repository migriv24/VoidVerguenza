# The foundation: voidverguenza, the domain (okf/concepts/architecture.md, okf/concepts/products.md).
# Links nothing but the standard library. Cryptography and memory arrive through ports.
add_library(voidverguenza STATIC
  core/bytes.cpp
  secret/secret_buffer.cpp
  record/fields.cpp
  record/object.cpp
  record/record.cpp
  sealing/envelope.cpp
  access/formula.cpp
  secrets/kinds.cpp
  secrets/generator.cpp
)
add_library(verguenza::foundation ALIAS voidverguenza)
# Includes are written from src/: "foundation/record/object.hpp".
target_include_directories(voidverguenza PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_features(voidverguenza PUBLIC cxx_std_20)
target_compile_options(voidverguenza PRIVATE ${VERGUENZA_WARNINGS})
