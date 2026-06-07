#pragma once
#include <raylib.h>
#include "common.h"
#include "debug.h"

class Visualizer {
public:
  explicit Visualizer(int w, int h, char *title)
      : m_wWidth(w), m_wHeight(h), m_wTitle(title), m_dMenu(&m_ctx) {}

  void Run();

private:
  void Init();
  void Update();
  void Draw();
  void Cleanup();
  void SceneInit();

  VisCtx m_ctx;
  RenderTexture2D m_rTex;
  DebugImGUI m_dMenu;
  bool m_show_debug{true};

  // Window
  int m_wWidth;
  int m_wHeight;
  char *m_wTitle;

  // Scene
  float m_window[4096]{};
  Mesh mesh; 
  Model model;
};
