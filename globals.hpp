#pragma once
#include "hmain.hpp"

inline float inv_fp = 1 / 1000000.0f; // z jaka czestotliwoscia probki maja byc interpolowane
inline float dt = 1 / 100000.0f;      // dokladnosc i predkosc symulacji

inline std::vector<WindowPacket> gWinPackets;
inline int windows_number = 10; // ile okien symulacji ma przejsc

inline bool rewind_punkt = false;

inline double last_render_time = 0;
inline double last_physics_time = 0;
inline double last_frame_time = 0;

inline double total_render_time = 0;
inline double total_physics_time = 0;

inline double last_vertices_per_second = 0;

constexpr int RENDER_TIMES_KEPT = 60;
inline int render_times_index = 0;
inline double render_times[RENDER_TIMES_KEPT];

constexpr int PHYSICS_TIMES_KEPT = 60;
inline int physics_times_index = 0;
inline double physics_times[PHYSICS_TIMES_KEPT];

constexpr int VERTICES_PER_SEC_KEPT = 600;
inline int vertex_times_index = 0;
inline double vertex_times[VERTICES_PER_SEC_KEPT];

inline double vertices_per_sec_avg_max = 0;

inline int steps_per_frame = 1;