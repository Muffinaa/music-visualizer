#include "debug.h"
#include <cstring>
#include <filesystem>
#include <iostream>
#include <vector>
#include <imgui.h>
#include <raylib.h>
#include <rlImGui.h>


void DebugImGUI::Init() {
  rlImGuiSetup(true);
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.FontGlobalScale = 1.f;

  ImGuiStyle &style = ImGui::GetStyle();
  style.WindowRounding = 0.f;

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    style.Colors[ImGuiCol_WindowBg].w = 0.5f;

  m_music_entries = m_ReturnMusicList("resources");
  cfiles.reserve(m_music_entries.size());
  for (const auto &f : m_music_entries)
    cfiles.emplace_back(f.c_str());
}

void DebugImGUI::Draw() {
  rlImGuiBegin();
  {

    ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
    ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport()->ID);
    ImGui::PopStyleColor(2);

    if (ImGui::Begin("Music List")) {
      ImGui::Text("Selected:");
      ImGui::Text("%s", m_ctx->music_name.c_str());
      ImGui::ListBox("##music", &m_selected_music, cfiles.data(),
                     cfiles.size());
      if (ImGui::Button("Play")) {
        std::string folder = "resources/";
        m_ctx->music_name = m_music_entries[m_selected_music];
        m_LoadMusic(folder.append(m_music_entries[m_selected_music]).c_str());
      }
      ImGui::SameLine();
      if (ImGui::Button("Refresh")) {
        std::string folder = "resources/";
        m_ctx->music_name = m_music_entries[m_selected_music];
        m_LoadMusic(folder.append(m_music_entries[m_selected_music]).c_str());
      }
    }
    ImGui::End();

    if (ImGui::Begin("Scene")) {
      if (ImGui::Button("Toggle Fullscreen")) {
        ToggleFullscreen();
      }
      ImGui::SameLine();
      if (ImGui::Button("Segfault lol")) {
        int* xd = new int;
        delete xd;
        *xd = 69420;
      }
      ImGui::Text("X: %.2f, Y: %.2f, Z: %.2f", m_ctx->camera.position.x,
                  m_ctx->camera.position.y, m_ctx->camera.position.z);
      ImGui::SliderFloat("Camera FOV", &m_ctx->desired_fov, 45.f, 140.f);
      ImGui::SliderFloat("Radius", &m_ctx->radius, 0, 20.f);
      if (ImGui::SliderInt("Blocks", &m_ctx->block_count, 0, 500)) {
        m_RegenerateBlocks(m_ctx->block_count);
      }
    }
    ImGui::End();
  }
  rlImGuiEnd();
}
void DebugImGUI::Update(float dt) {}

void DebugImGUI::Cleanup() { rlImGuiShutdown(); }

void DebugImGUI::m_RegenerateBlocks(int count) {
  m_ctx->blocks.clear();
  if (count <= 0)
    return;
  for (int i{0}; i < count; ++i) {
    m_ctx->blocks.emplace_back(Block{});
  }
}

void DebugImGUI::m_LoadMusic(const char *path) {
  PauseAudioStream(m_ctx->stream);
  UnloadWave(m_ctx->wave);
  m_ctx->wave.data = nullptr;
  UnloadAudioStream(m_ctx->stream);
  m_ctx->wave = LoadWave(path);
  m_ctx->stream = LoadAudioStream(m_ctx->wave.sampleRate,
                                  m_ctx->wave.sampleSize, m_ctx->wave.channels);
  m_ctx->samples = static_cast<float *>(m_ctx->wave.data);
  for (int i = 0; i < 4; i++) {
    UpdateAudioStream(m_ctx->stream,
                      &m_ctx->samples[m_ctx->cursor * m_ctx->wave.channels],
                      m_ctx->chunk_size);
    m_ctx->cursor += m_ctx->chunk_size;
  }
  m_ctx->cursor = 0;
}

std::vector<std::string>
DebugImGUI::m_ReturnMusicList(const std::filesystem::path &path) {
  namespace fs = std::filesystem;
  std::vector<std::string> files{};

  if (!fs::exists(path)) {
    std::cerr << "Music folder not found: " << path << std::endl;
    return files;
  }

  for (const auto &entry : fs::directory_iterator(path)) {
    if (entry.path().extension() == ".mp3") {
      files.emplace_back(entry.path().filename().string());
    }
  }

  return files;
}
