#include "visualizer.h"
#include <cmath>
#include <cstdlib>
#include <raylib.h>
#include <raymath.h>

void Visualizer::Run() {
  Init();
  while (!WindowShouldClose()) {
    Update();
    Draw();
  }
  Cleanup();
}

void Visualizer::Init() {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  InitWindow(1280, 750, "Visualizer");
  InitAudioDevice();
  SetAudioStreamBufferSizeDefault(m_ctx.chunk_size);
  SetTargetFPS(m_ctx.target_fps);
  Visualizer::SceneInit();
  m_dMenu.Init();
  DisableCursor();
  mesh = GenMeshSphere(1.f, 50.f, 50.f);
  model = LoadModelFromMesh(mesh);
}

bool mouse{true};

void Visualizer::Update() {
  if (IsKeyPressed(KEY_M)) {
    if (m_ctx.mouse_lock) {
      EnableCursor();
      m_ctx.mouse_lock = false;
    } else {
      DisableCursor();
      m_ctx.mouse_lock = true;
    }
  }

  if (IsKeyPressed(KEY_I))
    m_show_debug = !m_show_debug;

  if (m_ctx.mouse_lock)
    UpdateCamera(&m_ctx.camera, CAMERA_FREE);

  if (IsKeyPressed(KEY_P)) {
    if (IsAudioStreamPlaying(m_ctx.stream)) {
      PauseAudioStream(m_ctx.stream);
    } else if (m_ctx.cursor == 0) {
      PlayAudioStream(m_ctx.stream);
    } else {
      ResumeAudioStream(m_ctx.stream);
    }
  }

  if (IsKeyPressed(KEY_Z)) {
    m_ctx.camera.position = {15.f, m_ctx.center_point.y, 0.f};
    m_ctx.camera.target = m_ctx.center_point;
  }

  if (m_wWidth != GetScreenWidth() || m_wHeight != GetScreenHeight()) {
    m_wWidth = GetScreenWidth();
    m_wHeight = GetScreenHeight();
    UnloadRenderTexture(m_rTex);
    m_rTex = LoadRenderTexture(m_wWidth, m_wHeight);
    SetTextureWrap(m_rTex.texture, TEXTURE_WRAP_CLAMP);
  }

  while (IsAudioStreamProcessed(m_ctx.stream)) {
    for (int i = 0; i < m_ctx.chunk_size; i++) {
      m_window[i] = m_ctx.samples[(m_ctx.cursor + i) * m_ctx.wave.channels];
    }
    UpdateAudioStream(m_ctx.stream,
                      &m_ctx.samples[m_ctx.cursor * m_ctx.wave.channels],
                      m_ctx.chunk_size);
    m_ctx.cursor += m_ctx.chunk_size;
    if (m_ctx.cursor >= m_ctx.wave.frameCount)
      break;
  }

  if (!m_ctx.blocks.empty()) {
    for (int index{0}; index < m_ctx.blocks.size(); ++index) {
      int sampleIndex =
          index * m_ctx.chunk_size / static_cast<int>(m_ctx.blocks.size());
      m_ctx.amplitude = m_window[sampleIndex];

      float a = index * (360.f / static_cast<float>(m_ctx.blocks.size()));
      float radian = a * (M_PI / 180.0f);

      m_ctx.blocks[index].angle = -a;
      float des = 1.f + m_ctx.amplitude * 2.f;
      float boundsSpeed = m_ctx.blocks[index].bounds.z < des ? 40.f : 8.f;
      m_ctx.blocks[index].bounds.z =
          Lerp(m_ctx.blocks[index].bounds.z, des, GetFrameTime() * boundsSpeed);

      m_ctx.blocks[index].pos = {
          m_ctx.center_point.x,
          Lerp(m_ctx.center_point.y + m_ctx.radius * std::sin(radian),
               m_ctx.center_point.y + m_ctx.amplitude * 1.5f +
                   m_ctx.radius * std::sin(radian),
               GetFrameTime() * 5.f),
          Lerp(m_ctx.center_point.z + m_ctx.radius * std::cos(radian),
               m_ctx.center_point.z + m_ctx.amplitude * 1.5f +
                   m_ctx.radius * std::cos(radian),
               GetFrameTime() * 5.f),
      };
    }
    m_ctx.average_amplitude = 0.f;
    int step = m_ctx.chunk_size / static_cast<int>(m_ctx.blocks.size());
    int samples = 8;

    for (int i = 0; i < samples; i++) {
      int idx = (step * i) % m_ctx.chunk_size;
      m_ctx.average_amplitude += std::abs(m_window[idx]);
    }
    m_ctx.average_amplitude /= samples;

    float targetFov = m_ctx.desired_fov + m_ctx.average_amplitude * 30.f;

    float lerpSpeed = m_ctx.camera.fovy < targetFov ? 20.f : 4.f;
    m_ctx.camera.fovy =
        Lerp(m_ctx.camera.fovy, targetFov, GetFrameTime() * lerpSpeed);

    m_dMenu.Update(GetFrameTime());
  }
}

void Visualizer::Draw() {
  BeginTextureMode(m_rTex);
  {
    ClearBackground(BLACK);
    BeginMode3D(m_ctx.camera);
    {
      DrawGrid(10, 5.f);
      for (const auto &b : m_ctx.blocks) {
        DrawModelEx(model, b.pos, {1.f, 0.f, 0.f}, b.angle, b.bounds, b.color);
      }
      DrawModel(model, m_ctx.center_point,
                m_ctx.average_amplitude + 0.2 * m_ctx.radius, GREEN);
    }
    EndMode3D();
  }
  EndTextureMode();

  BeginDrawing();
  {
    ClearBackground(BLACK);
    BeginShaderMode(m_ctx.shader);
    {
      int locTime = GetShaderLocation(m_ctx.shader, "time");
      float t = GetTime();
      SetShaderValue(m_ctx.shader, locTime, &t, SHADER_UNIFORM_FLOAT);
      DrawTextureRec(m_rTex.texture,
                     {0, 0, static_cast<float>(m_rTex.texture.width),
                      static_cast<float>(m_rTex.texture.height)},
                     {0, 0}, WHITE);
    }
    EndShaderMode();
    if (m_show_debug) {
      Color col = m_ctx.mouse_lock ? RED : WHITE;
      DrawText("[P] toggle playing", 20, 34, 16, WHITE);
      DrawText("[M] toggle cursor", 20, 54, 16, col);
      DrawText("[Z] reset position", 20, 74, 16, WHITE);
      DrawText("[I] toggle settings", 20, 94, 16, WHITE);
      DrawFPS(10, 10);
      m_dMenu.Draw();
    }
  }
  EndDrawing();
}
void Visualizer::Cleanup() {
  m_dMenu.Cleanup();
  UnloadRenderTexture(m_rTex);
  UnloadAudioStream(m_ctx.stream);
  UnloadWave(m_ctx.wave);
  CloseAudioDevice();
  CloseWindow();
}

void Visualizer::SceneInit() {
  for (int i{0}; i <= m_ctx.start_blocks; ++i) {
    m_ctx.blocks.emplace_back(Block{});
  }

  m_ctx.camera = {
      .position = {15.f, m_ctx.center_point.y, 0.f},
      .target = m_ctx.center_point,
      .up = {0.f, 1.0f, 0.f},
      .fovy = m_ctx.desired_fov,
      .projection = CAMERA_PERSPECTIVE,
  };

  m_rTex = LoadRenderTexture(m_wWidth, m_wHeight);
  SetTextureWrap(m_rTex.texture, TEXTURE_WRAP_CLAMP);
  m_ctx.shader =
      LoadShader("resources/shaders/fun.vs", "resources/shaders/fun.fs");

  m_ctx.stream = LoadAudioStream(m_ctx.wave.sampleRate, m_ctx.wave.sampleSize,
                                 m_ctx.wave.channels);
  m_ctx.samples = static_cast<float *>(m_ctx.wave.data);
  for (int i = 0; i < 4; i++) {
    UpdateAudioStream(m_ctx.stream,
                      &m_ctx.samples[m_ctx.cursor * m_ctx.wave.channels],
                      m_ctx.chunk_size);
    m_ctx.cursor += m_ctx.chunk_size;
  }
  m_ctx.cursor = 0;
}
