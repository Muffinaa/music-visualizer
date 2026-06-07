add_executable(visualizer)

target_sources(visualizer
  PRIVATE
    main.cpp
    debug.cpp
    visualizer.cpp
)

target_include_directories(visualizer
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(visualizer
  PRIVATE
    raylib
    imgui
    rlimgui
)
