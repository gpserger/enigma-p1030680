// score.h - n-gram tables (log10 prob, from count files) and plugboard climbers.
#pragma once
#include "engine.h"
struct Lang {
  std::vector<float> mono, bi, tri, quad;
  bool load(const std::string& prefix) {   // prefix e.g. .../navalblend -> prefix_bigrams.txt
    NGram a, b, c, d;
    if (!a.load(prefix + "_monograms.txt", 1, 'c') || !b.load(prefix + "_bigrams.txt", 2, 'c') ||
        !c.load(prefix + "_trigrams.txt", 3, 'c') || !d.load(prefix + "_quadgrams.txt", 4, 'c')) return false;
    mono = a.t; bi = b.t; tri = c.t; quad = d.t; return true;
  }
};
static inline void decrypt32(const uint8_t (*T)[32], const int* S, const uint8_t* ct, int n, uint8_t* p) {
  for (int i = 0; i < n; i++) p[i] = (uint8_t)S[T[i][S[ct[i]]]];
}
static inline float sc_bi(const float* t, const uint8_t* p, int n) { float s = 0; for (int i = 0; i + 1 < n; i++) s += t[p[i]*26+p[i+1]]; return s; }
static inline float sc_tri(const float* t, const uint8_t* p, int n) { float s = 0; for (int i = 0; i + 2 < n; i++) s += t[(p[i]*26+p[i+1])*26+p[i+2]]; return s; }
static inline float sc_quad(const float* t, const uint8_t* p, int n) { float s = 0; for (int i = 0; i + 3 < n; i++) s += t[((p[i]*26+p[i+1])*26+p[i+2])*26+p[i+3]]; return s; }
static inline float sc_ic(const uint8_t* p, int n) { int h[26] = {0}; for (int i = 0; i < n; i++) h[p[i]]++; long s = 0; for (int a = 0; a < 26; a++) s += h[a]*(h[a]-1); return (float)s / (n*(n-1)); }

static inline int nplugs(const int* S) { int k = 0; for (int a = 0; a < 26; a++) k += S[a] > a; return k; }
struct Eval {   // evaluates a plugboard at a fixed setting
  const uint8_t (*T)[32]; const uint8_t* ct; int n; const Lang* L; int order;  // order 0=ic,2,3,4
  mutable long cnt = 0;
  inline float operator()(const int* S) const {
    uint8_t p[MAXLEN]; decrypt32(T, S, ct, n, p); cnt++;
    switch (order) { case 2: return sc_bi(L->bi.data(), p, n); case 3: return sc_tri(L->tri.data(), p, n);
      case 4: return sc_quad(L->quad.data(), p, n); default: return 1000.f * sc_ic(p, n); }
  }
};
// First-improvement pair-move hill climb (Krah / research/07 hc.h), generic evaluator.
template <class EV>
static float climb(const EV& ev, int* S, const bool* fixed, const int* order, int maxplugs) {
  float best = ev(S); bool improved = true; int T2[26];
  while (improved) {
    improved = false;
    for (int ii = 0; ii < 26; ii++) {
      int a = order[ii]; if (fixed[a]) continue;
      for (int kk = ii + 1; kk < 26; kk++) {
        int b = order[kk]; if (fixed[b]) continue;
        int x = S[a], z = S[b];
        if (x == b) { S[a] = a; S[b] = b; float s = ev(S);
          if (s > best + 1e-4f) { best = s; improved = true; continue; } S[a] = b; S[b] = a; continue; }
        if ((x != a && fixed[x]) || (z != b && fixed[z])) continue;
        memcpy(T2, S, sizeof T2);
        if (x != a) S[x] = x; if (z != b) S[z] = z; S[a] = b; S[b] = a;
        int np = nplugs(S); float bs = -1e30f; int bestv = 0;
        if (np <= maxplugs) { float s = ev(S); if (s > best + 1e-4f) { bs = s; bestv = 1; } }
        if (x != a && z != b) { S[x] = z; S[z] = x;
          if (np + 1 <= maxplugs) { float s = ev(S); if (s > best + 1e-4f && s > bs) { bs = s; bestv = 2; } }
          S[x] = x; S[z] = z; }
        if (bestv == 0) memcpy(S, T2, sizeof T2); else { if (bestv == 2) { S[x] = z; S[z] = x; } best = bs; improved = true; }
      }
    }
  }
  return best;
}
