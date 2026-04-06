#pragma once
#include "hmain.hpp"

inline std::string file = "data/kryzys.csv";           // plik wejsciowy
inline std::string write_file = "output/kryzys_out.csv"; // plik wyjsciowy

constexpr float INITIAL_WAVE_RADIUS = 0.02f;
constexpr float R0_ATTEN = 0.0001f; // [m] � near-field cap (np. 5 cm)
constexpr float SOUND_V = 1440.0f / 1.0f; // C++