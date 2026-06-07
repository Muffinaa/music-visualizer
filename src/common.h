#pragma once
#include <string>
#include <vector>
#include <raylib.h>

struct Block {
  Vector3 pos{0.f, 0.f, 0.f};
  Vector3 bounds{1.f, 1.f, 1.f};
  float angle{90};
  Color color{GREEN};
};

struct VisCtx {
  Camera3D camera{};
  Vector3 center_point{0.f, -20.f, 0.f};
  Shader shader;
  Wave wave;
  AudioStream stream;
  std::vector<Block> blocks{};
  std::string music_name{};
  bool mouse_lock{false};
  float* samples{};
  int chunk_size{4096};
  int target_fps{60};
  int start_blocks{100};
  int block_count{start_blocks};
  int cursor{0};
  float amplitude{};
  float average_amplitude{};
  float radius{9.f};
  float desired_fov{80.f};
};
