#pragma once
#include <filesystem>
#include "common.h"

class DebugImGUI {
public:
  explicit DebugImGUI(VisCtx *ctx) : m_ctx(ctx) {}

  void Init();
  void Draw();
  void Update(float dt);
  void Cleanup();

private:
  void m_RegenerateBlocks(int count);
  void m_LoadMusic(const char *path);
  std::vector<std::string> m_ReturnMusicList(const std::filesystem::path &path);
  VisCtx *m_ctx;
  int m_selected_music{0};
  std::vector<std::string> m_music_entries;
  std::vector<const char *> cfiles{};
};
