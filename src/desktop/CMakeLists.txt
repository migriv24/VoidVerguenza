# The desktop application: a thin Void Maiz window (Q15: the application uses Maiz;
# the foundation never does). Screens only; every rule is in the application layer.
if(NOT WIN32)
  return() # the platform adapters exist for Windows only so far (src/adapters/CMakeLists.txt)
endif()

add_executable(voidverguenza-app
  main.cpp
  shell.cpp
  screens/lock_screen.cpp
  screens/secrets_screen.cpp
  screens/secret_form.cpp
  screens/record_screens.cpp
)
target_link_libraries(voidverguenza-app PRIVATE
  verguenza_app
  verguenza_adapter_reticulum
  verguenza_adapter_storage
  verguenza_adapter_model
  verguenza_adapter_platform
  voidmaiz
  voidmaiz_imgui
)
target_compile_options(voidverguenza-app PRIVATE ${VERGUENZA_WARNINGS})
target_link_options(voidverguenza-app PRIVATE ${VERGUENZA_STATIC_RUNTIME} -mwindows)
