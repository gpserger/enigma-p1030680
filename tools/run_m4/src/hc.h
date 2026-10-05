// hc.h - plugboard hill climber (first-improvement, Krah-style pair moves) with
// optional fixed seed plug (Ostwald-Weierud partial stecker exhaustion).
#pragma once
#include "core.h"

extern int g_climb_order; // 3 or 4
struct Scorer {
  const float* tri;   // 26^3 or 26^4 table (climb model)
  int len; const uint8_t* ct; const uint8_t (*T)[26];
  inline float eval(const int* S) const {
    uint8_t p[MAXLEN];
    for (int i = 0; i < len; i++) p[i] = (uint8_t)S[T[i][S[ct[i]]]];
    float s = 0;
    if (g_climb_order == 4) {
      int idx = (p[0]*26 + p[1])*26 + p[2];
      for (int i = 3; i < len; i++) { idx = (idx % 17576) * 26 + p[i]; s += tri[idx]; }
    } else {
      int idx = p[0]*26 + p[1];
      for (int i = 2; i < len; i++) { idx = (idx % 676) * 26 + p[i]; s += tri[idx]; }
    }
    return s;
  }
};

static inline int nplugs(const int* S){ int n=0; for(int i=0;i<26;i++) if(S[i]>i) n++; return n; }

// Climb from S (modified in place). fixed[c]=true => letter c's plug is frozen.
// order: letter visiting order. Returns final score.
static inline float climb(const Scorer& sc, int* S, const bool* fixed, const int* order, int maxplugs, long& evals) {
  float best = sc.eval(S); evals++;
  bool improved = true;
  int T2[26];
  while (improved) {
    improved = false;
    for (int ii = 0; ii < 26; ii++) {
      int a = order[ii]; if (fixed[a]) continue;
      for (int kk = ii+1; kk < 26; kk++) {
        int b = order[kk]; if (fixed[b]) continue;
        int x = S[a], z = S[b];
        if (x == b) { // plugged together -> try unplug
          S[a]=a; S[b]=b; float s = sc.eval(S); evals++;
          if (s > best + 1e-4f) { best = s; improved = true; continue; }
          S[a]=b; S[b]=a; continue;
        }
        if ((x != a && fixed[x]) || (z != b && fixed[z])) continue;
        memcpy(T2, S, sizeof T2);
        // variant 1: a-b plugged, old partners freed
        if (x != a) { S[x]=x; } if (z != b) { S[z]=z; }
        S[a]=b; S[b]=a;
        int np = nplugs(S);
        float bs = -1e30f; int bestv = 0;
        if (np <= maxplugs) { float s = sc.eval(S); evals++; if (s > best + 1e-4f) { bs = s; bestv = 1; } }
        // variant 2: additionally x-z plugged
        if (x != a && z != b) {
          S[x]=z; S[z]=x;
          if (np + 1 <= maxplugs) { float s = sc.eval(S); evals++; if (s > best + 1e-4f && s > bs) { bs = s; bestv = 2; } }
          S[x]=x; S[z]=z;
        }
        if (bestv == 0) { memcpy(S, T2, sizeof T2); }
        else { if (bestv == 2) { S[x]=z; S[z]=x; } best = bs; improved = true; }
      }
    }
  }
  return best;
}
