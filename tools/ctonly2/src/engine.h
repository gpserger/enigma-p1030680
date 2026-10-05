// engine.h - M3/M4 scrambler in "core" coordinates for fast enumeration (tools/ctonly2).
// Reuses wirings/stepping of tools/run_m4/src/core.h (validated vs py-enigma).
//
// A setting is (cG, cL, cM, cR) = wiring core offsets (position - ring) of Greek/left/middle/right
// wheels, plus the middle and right WINDOW letters before the first key press (wm0, wr0), which fix
// the stepping. Greek and left rings are degenerate with position. Key display: positions
// G=cG L=cL M=wm0 R=wr0, rings A A (wm0-cM) (wr0-cR).
#pragma once
#include "../../run_m4/src/core.h"
#include <immintrin.h>

struct Job { int refl, greek, wL, wM, wR; };   // refl 0..3 (B-thin,C-thin,B,C); greek 8/9 or -1 (M3)
struct Setting { int cG, cL, cM, cR, wm0, wr0; };

static inline std::string wheel_str(const Job& j) { char b[4] = {char('1'+j.wL), char('1'+j.wM), char('1'+j.wR), 0}; return b; }
static inline std::string job_str(const Job& j) {
  std::string s = std::string(j.refl==0?"b":j.refl==1?"c":j.refl==2?"B":"C") + (j.greek==8?"B":j.greek==9?"G":"-") + wheel_str(j);
  return s;
}

alignas(64) static uint8_t RF[8][26][32], RB[8][26][32];
static void init_rtabs() {
  memset(RF, 0, sizeof RF); memset(RB, 0, sizeof RB);
  for (int w = 0; w < 8; w++) for (int s = 0; s < 26; s++) for (int c = 0; c < 26; c++) {
    RF[w][s][c] = rot_f(w, s, c); RB[w][s][c] = rot_b(w, s, c); }
}
// U[((g*26+l)*26+m)*32 + x]: inner permutation (middle, left, greek, reflector and back), padded to 32
static void build_U(const Job& J, uint8_t* U) {
  int ng = J.greek >= 0 ? 26 : 1;
  for (int g = 0; g < ng; g++) for (int l = 0; l < 26; l++) for (int m = 0; m < 26; m++) {
    uint8_t* u = U + ((g * 26 + l) * 26 + m) * 32;
    for (int c = 0; c < 26; c++) {
      int x = rot_f(J.wM, m, c); x = rot_f(J.wL, l, x); if (J.greek >= 0) x = rot_f(J.greek, g, x);
      x = TB.refl[J.refl][x];
      if (J.greek >= 0) x = rot_b(J.greek, g, x); x = rot_b(J.wL, l, x); x = rot_b(J.wM, m, x);
      u[c] = x;
    }
    for (int c = 26; c < 32; c++) u[c] = 0;
  }
}
static const size_t USZ = 26 * 26 * 26 * 32;

// stepping pattern: cumulative middle / left steps at each letter (after the pre-keypress step)
struct Pattern { uint8_t dM[MAXLEN], dL[MAXLEN]; };
static void sim_pattern(int wR, int wM, int wr0, int wm0, int n, Pattern& p) {
  int r = wr0, m = wm0, dm = 0, dl = 0;
  for (int i = 0; i < n; i++) {
    bool mn = TB.notch[wM][m], rn = TB.notch[wR][r];
    if (mn) { m = md(m + 1); dm++; dl++; } else if (rn) { m = md(m + 1); dm++; }
    r = md(r + 1);
    p.dM[i] = dm; p.dL[i] = dl;
  }
}

// Full path table T[i][x] (32-byte rows) for a setting. U must be built for J.
static inline void make_T(const Job& J, const uint8_t* U, const Setting& s, const Pattern& P, int n, uint8_t (*T)[32]) {
  for (int i = 0; i < n; i++) {
    int sh = md(s.cR + i + 1);
    const uint8_t* u = U + ((s.cG * 26 + md(s.cL + P.dL[i])) * 26 + md(s.cM + P.dM[i])) * 32;
    __m256i v = _mm256_load_si256((const __m256i*)RF[J.wR][sh]);
    v = _mm256_permutexvar_epi8(v, _mm256_loadu_si256((const __m256i*)u));
    v = _mm256_permutexvar_epi8(v, _mm256_load_si256((const __m256i*)RB[J.wR][sh]));
    _mm256_store_si256((__m256i*)T[i], v);
  }
}

// Key conversions
static inline Setting setting_from_key(const Job& J, const char* pos4, const char* rings4) {
  auto L = [](char c) { return (c >= 'a' ? c - 'a' : c - 'A'); };
  Setting s; s.cG = J.greek >= 0 ? md(L(pos4[0]) - L(rings4[0])) : 0;
  s.cL = md(L(pos4[1]) - L(rings4[1])); s.cM = md(L(pos4[2]) - L(rings4[2])); s.cR = md(L(pos4[3]) - L(rings4[3]));
  s.wm0 = L(pos4[2]); s.wr0 = L(pos4[3]); return s;
}
static inline std::string key_str(const Job& J, const Setting& s) {
  char b[64]; snprintf(b, sizeof b, "%s %c%c%c%c AA%c%c", job_str(J).c_str(), 'A'+s.cG, 'A'+s.cL, 'A'+s.wm0, 'A'+s.wr0,
                       'A'+md(s.wm0-s.cM), 'A'+md(s.wr0-s.cR));
  return b;
}
static inline Job parse_job(const char* refl, const char* greek, const char* wheels) {
  Job J; char r = refl[0];
  J.refl = (r=='b'||r=='B') ? 0 : 1;
  if (!strcmp(refl, "BT") || !strcmp(refl, "B3")) J.refl = 2;   // thick B (M3)
  if (!strcmp(refl, "CT") || !strcmp(refl, "C3")) J.refl = 3;
  char g = greek[0]; J.greek = (g=='b'||g=='B') ? 8 : (g=='g'||g=='G') ? 9 : -1;
  J.wL = wheels[0]-'1'; J.wM = wheels[1]-'1'; J.wR = wheels[2]-'1'; return J;
}
