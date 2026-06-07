add_subdirectory(raylib)

# ImGUI
add_library(imgui STATIC
  imgui/imgui.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp
)

target_include_directories(imgui
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/imgui/
)

# rlImGUI
add_library(rlimgui STATIC
  rlImGui/rlImGui.cpp
)

target_include_directories(rlimgui
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/rlImGui/
)

target_link_libraries(rlimgui
    PUBLIC
        imgui
        raylib
)
