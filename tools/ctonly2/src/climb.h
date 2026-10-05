// climb.h - plugboard hill climbing at a fixed rotor setting (tools/ctonly2).
// Score models follow trognes' m4f10 recipe (research/07): monogram pre-pass capped at 4 plugs, then
// the "fused" model = log-linear symmetric all-order table (quad 1, tri .6, bi .3, mono .15) per
// symbol + LAMBDA * IC, capped at 10 plugs; plus the 2-plug re-pair barrier move.
#pragma once
#include "engine.h"
#include <random>

enum { M_MONO = 0, M_BI = 1, M_TRI = 2, M_QUAD = 3, M_FUSED = 4, M_FUSED3 = 5, M_IC = 6, M_BIIC = 7 };

// load "GRAM count" file -> log10(count/total), unseen floored at FLOOR counts (trognes: hapax = 1)
static bool load_ng(const std::string& path, int n, std::vector<float>& t, double floor_cnt) {
  int sz = 1; for (int i = 0; i < n; i++) sz *= 26; std::vector<double> c(sz, 0); double tot = 0;
  FILE* f = fopen(path.c_str(), "r"); if (!f) return false; char g[64]; double v;
  while (fscanf(f, "%63s %lf", g, &v) == 2) { if ((int)strlen(g) != n) continue; int idx = 0; bool ok = true;
    for (int i = 0; i < n; i++) { int ch = toupper(g[i]); if (ch < 'A' || ch > 'Z') { ok = false; break; } idx = idx * 26 + ch - 'A'; }
    if (ok) { c[idx] += v; tot += v; } }
  fclose(f); t.resize(sz); for (int i = 0; i < sz; i++) t[i] = (float)log10(std::max(c[i], floor_cnt) / tot); return tot > 0;
}
struct Tables2 {   // float log10 tables
  std::vector<float> mono, bi, tri, quad, all, all3;
  float lambda = 30.f, lambda3 = 30.f; double floor_cnt = 1.0; float w3[3] = {1.f, .5f, .25f}; float aw[4] = {1.f, .6f, .3f, .15f};
  bool load(const std::string& prefix) {
    if (!load_ng(prefix + "_monograms.txt", 1, mono, floor_cnt) || !load_ng(prefix + "_bigrams.txt", 2, bi, floor_cnt) ||
        !load_ng(prefix + "_trigrams.txt", 3, tri, floor_cnt) || !load_ng(prefix + "_quadgrams.txt", 4, quad, floor_cnt)) return false;
    all.resize(456976);
    for (int A = 0; A < 26; A++) for (int B = 0; B < 26; B++) for (int C = 0; C < 26; C++) for (int D = 0; D < 26; D++) {
      int idx = ((A*26+B)*26+C)*26+D;
      all[idx] = aw[0] * quad[idx] + aw[1] * (tri[(A*26+B)*26+C] + tri[(B*26+C)*26+D]) / 2 + aw[2] * (bi[A*26+B] + bi[B*26+C] + bi[C*26+D]) / 3
                 + aw[3] * (mono[A] + mono[B] + mono[C] + mono[D]) / 4;
    }
    all3.resize(17576);
    for (int A = 0; A < 26; A++) for (int B = 0; B < 26; B++) for (int C = 0; C < 26; C++)
      all3[(A*26+B)*26+C] = w3[0] * tri[(A*26+B)*26+C] + w3[1] * (bi[A*26+B] + bi[B*26+C]) / 2 + w3[2] * (mono[A] + mono[B] + mono[C]) / 3;
    return true;
  }
};

struct Ctx2 {
  const uint8_t (*T)[32]; const uint8_t* ct; int n; const Tables2* tb;
  mutable long evals = 0;
  inline void dec(const int* S, uint8_t* p) const { for (int i = 0; i < n; i++) p[i] = (uint8_t)S[T[i][S[ct[i]]]]; }
  template <int M> inline float eval(const int* S) const {
    uint8_t p[MAXLEN]; dec(S, p); evals++;
    if (M == M_IC || M == M_BIIC) { int h[26] = {0}; for (int i = 0; i < n; i++) h[p[i]]++; int c = 0; for (int a = 0; a < 26; a++) c += h[a]*(h[a]-1);
      float ic = (float)c / (n * (n - 1)); if (M == M_IC) return ic;
      float s = 0; for (int i = 0; i + 1 < n; i++) s += tb->bi[p[i]*26+p[i+1]]; return s / (n-1) + tb->lambda * ic; }
    if (M == M_MONO) { float s = 0; for (int i = 0; i < n; i++) s += tb->mono[p[i]]; return s / n; }
    if (M == M_BI) { float s = 0; for (int i = 0; i + 1 < n; i++) s += tb->bi[p[i]*26+p[i+1]]; return s / (n-1); }
    if (M == M_TRI) { float s = 0; for (int i = 0; i + 2 < n; i++) s += tb->tri[(p[i]*26+p[i+1])*26+p[i+2]]; return s / (n-2); }
    if (M == M_FUSED3) { float s = 0; int idx = p[0]*26+p[1]; for (int i = 2; i < n; i++) { idx = (idx % 676) * 26 + p[i]; s += tb->all3[idx]; }
      s /= (n - 2); int h[26] = {0}; for (int i = 0; i < n; i++) h[p[i]]++; int c = 0; for (int a = 0; a < 26; a++) c += h[a]*(h[a]-1);
      return s + tb->lambda3 * (float)c / (n * (n - 1)); }
    const float* t = M == M_QUAD ? tb->quad.data() : tb->all.data();
    float s = 0; int idx = (p[0]*26+p[1])*26+p[2];
    for (int i = 3; i < n; i++) { idx = (idx % 17576) * 26 + p[i]; s += t[idx]; }
    s /= (n - 3);
    if (M == M_FUSED) { int h[26] = {0}; for (int i = 0; i < n; i++) h[p[i]]++; int c = 0; for (int a = 0; a < 26; a++) c += h[a]*(h[a]-1);
      s += tb->lambda * (float)c / (n * (n - 1)); }
    return s;
  }
};

static inline int npl(const int* S) { int k = 0; for (int a = 0; a < 26; a++) k += S[a] > a; return k; }

// trognes-style toggle climb. firstimp: circular first-improvement; else steepest ascent.
template <int M>
static float climb_toggle(const Ctx2& cx, int* S, int cap, bool firstimp) {
  float best = cx.eval<M>(S);
  for (;;) {
    bool any = false;
    if (firstimp) {   // trognes -J: score all 325 toggles once, sort best-first, then circular first-improvement
      static thread_local int PA[325], PB[325]; static thread_local bool init = false;
      if (!init) { int k = 0; for (int a = 0; a < 26; a++) for (int b = a + 1; b < 26; b++) { PA[k] = a; PB[k] = b; k++; } init = true; }
      int pairs = npl(S);
      auto blocked = [&](int a, int b) { return pairs >= cap && S[a] != b && S[a] == a && S[b] == b; };
      auto apply = [&](int a, int b) { if (S[a] == b) { S[a] = a; S[b] = b; pairs--; } else { int x = S[a], y = S[b];
          if (x != a) { S[x] = x; pairs--; } if (y != b && y != x) { S[y] = y; pairs--; } S[a] = b; S[b] = a; pairs++; } };
      float sc[325]; int visit[325];
      for (int mv = 0; mv < 325; mv++) {
        visit[mv] = mv; int a = PA[mv], b = PB[mv];
        if (blocked(a, b)) { sc[mv] = -1e30f; continue; }
        int keep[26]; memcpy(keep, S, sizeof keep); int kp = pairs; apply(a, b); sc[mv] = cx.eval<M>(S); memcpy(S, keep, sizeof keep); pairs = kp;
      }
      std::sort(visit, visit + 325, [&](int i, int j) { return sc[i] != sc[j] ? sc[i] > sc[j] : i < j; });
      int cursor = 0, stale = 0;
      while (stale < 325) {
        int mv = visit[cursor]; if (++cursor == 325) cursor = 0;
        int a = PA[mv], b = PB[mv];
        if (blocked(a, b)) { stale++; continue; }
        int keep[26]; memcpy(keep, S, sizeof keep); int kp = pairs; apply(a, b);
        float s2 = cx.eval<M>(S);
        if (s2 > best) { best = s2; stale = 0; } else { memcpy(S, keep, sizeof keep); pairs = kp; stale++; }
      }
    } else
    for (;;) {   // steepest ascent
      int pairs = npl(S); float mv = best; int ma = -1, mb = -1;
      for (int a = 0; a < 26; a++) for (int b = a + 1; b < 26; b++) {
        int sa = S[a], sb = S[b]; bool af = sa == a, bf = sb == b, paired = sa == b;
        if (pairs >= cap && !paired && af && bf) continue;
        int x = sa, y = sb, xx = S[x], yy = S[y];
        if (paired) { S[a] = a; S[b] = b; } else { S[x] = x; S[y] = y; S[a] = b; S[b] = a; }
        float s = cx.eval<M>(S);
        if (s > mv + 1e-6f) { mv = s; ma = a; mb = b; }
        if (paired) { S[a] = b; S[b] = a; } else { S[a] = sa; S[b] = sb; S[x] = xx; S[y] = yy; }
      }
      if (ma < 0) break;
      int a = ma, b = mb;
      if (S[a] == b) { S[a] = a; S[b] = b; } else { int x = S[a], y = S[b]; S[x] = x; S[y] = y; S[a] = b; S[b] = a; }
      best = mv; any = true;
    }
    // re-pair barrier: for two plugs a-x, b-y try a-b/x-y and a-y/x-b
    int plo[13], phi[13], np = 0;
    for (int a = 0; a < 26; a++) if (S[a] > a) { plo[np] = a; phi[np] = S[a]; np++; }
    float rb = best; int r[4] = {-1, -1, -1, -1}; bool found = false;
    for (int i = 0; i < np; i++) for (int j = i + 1; j < np; j++) {
      int a = plo[i], x = phi[i], b = plo[j], y = phi[j];
      S[a] = b; S[b] = a; S[x] = y; S[y] = x; float s1 = cx.eval<M>(S);
      if (s1 > rb + 1e-6f) { rb = s1; found = true; r[0] = a; r[1] = b; r[2] = x; r[3] = y; }
      S[a] = y; S[y] = a; S[x] = b; S[b] = x; float s2 = cx.eval<M>(S);
      if (s2 > rb + 1e-6f) { rb = s2; found = true; r[0] = a; r[1] = y; r[2] = x; r[3] = b; }
      S[a] = x; S[x] = a; S[b] = y; S[y] = b;
    }
    if (found) { S[r[0]] = r[1]; S[r[1]] = r[0]; S[r[2]] = r[3]; S[r[3]] = r[2]; best = rb; continue; }
    (void)any; break;
  }
  return best;
}
static inline void kick(int* S, int k, std::mt19937& rng) {
  int fr[26], nf = 0; for (int a = 0; a < 26; a++) if (S[a] == a) fr[nf++] = a;
  int want = std::min(2 * k, nf - (nf & 1));
  for (int i = 0; i < want; i++) { int j = i + rng() % (nf - i); std::swap(fr[i], fr[j]); }
  for (int i = 0; i + 1 < want; i += 2) { S[fr[i]] = fr[i+1]; S[fr[i+1]] = fr[i]; }
}
