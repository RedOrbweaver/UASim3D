#pragma once
#include "hmain.hpp"

inline float inv_fp = 1 / 1000000.0f; // z jaka czestotliwoscia probki maja byc interpolowane
inline float dt = 1 / 100000.0f;      // dokladnosc i predkosc symulacji


inline int ktore_odbicie = 0;

inline std::vector<WindowPacket> gWinPackets;
inline size_t gWinIdx = 0; // kt�ry 5 ms segment aktualnie nadajemy
inline int windows_number = 10; // ile okien symulacji ma przejsc

inline bool rewind_punkt = false;

inline float last_render_time = 0;
inline float last_physics_time = 0;
inline float last_frame_time = 0;

