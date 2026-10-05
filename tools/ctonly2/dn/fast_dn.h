// fast.h - AVX-512 plugboard evaluator + climber for one rotor setting (tools/ctonly2).
// Objective (measured best in exp3): fused all-order table (quad 1, tri .6, bi .3, mono .15, symmetric
// log-linear fold, floor = FLOOR counts) summed over the 69 quad windows + LAMBDA * IC, integer
// quantized. Climb = trognes -J: score all 325 pair toggles, sort best-first, circular first
// improvement, then 2-plug re-pair barrier; random kick restarts. No mono pre-pass (measured useless).
#pragma once
#include "../src/climb.h"
#include <immintrin.h>

static bool g_u8 = false;
struct FastLang {   // quantized fused table
  std::vector<int16_t> A, A3; std::vector<uint8_t> A8; int lam_int = 0, lam3_int = 0, lam8_int = 0; float scale = 1000.f, scale8 = 1.f, mn8 = 0; int n = 0;
  void build(const Tables2& tb, int n_) {
    n = n_; A.assign(456976 + 64, 0); A3.assign(17576 + 64, 0);
    { float m3 = -1e30f; for (float v : tb.all3) m3 = std::max(m3, v);
      for (int i = 0; i < 17576; i++) A3[i] = (int16_t)std::max(-32000.f, (tb.all3[i] - m3) * scale);
      lam3_int = (int)lrintf(tb.lambda3 * scale * (n - 2) / (float)(n * (n - 1))); }
    float mx = -1e30f; for (float v : tb.all) mx = std::max(mx, v);
    for (int i = 0; i < 456976; i++) { float q = (tb.all[i] - mx) * scale; A[i] = (int16_t)std::max(-32000.f, q); }
    // per-symbol fused = sum/(n-3) + lambda*sum h(h-1)/(n(n-1))  ->  x (n-3)*scale
    lam_int = (int)lrintf(tb.lambda * scale * (n - 3) / (float)(n * (n - 1)));
    { float mx8 = -1e30f, m8 = 1e30f; for (float v : tb.all) { mx8 = std::max(mx8, v); m8 = std::min(m8, v); }
      scale8 = 255.f / (mx8 - m8); mn8 = m8; A8.assign(456976 + 64, 0);
      for (int i = 0; i < 456976; i++) A8[i] = (uint8_t)lrintf((tb.all[i] - m8) * scale8);
      lam8_int = (int)lrintf(tb.lambda * scale8 * (n - 3) / (float)(n * (n - 1))); }
  }
};

struct alignas(64) Fast {
  // per ciphertext
  int n; uint8_t ct[128]; uint64_t ML[26], MH[26]; const FastLang* L;
  // per setting
  alignas(64) uint8_t TT[26][128];      // TT[v][i] = T_i[v]
  alignas(64) int8_t H[26][26][32];     // H[a][v][x] = #{i: c_i = a, T_i[v] = x}
  long evals = 0;
  // --- window mask (research/15): drop quad windows touching letters whose middle-wheel offset is
  // ambiguous between the sampled right-ring phase and its +-stride/2 neighbours ---
  bool usemask = false; __mmask16 wmk[4] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}; bool keepw[128]; uint8_t lanewin[4][16]; int nkeep = 0;

  void init(const uint8_t* ct_, int n_, const FastLang* L_) {
    n = n_; L = L_; memset(ct, 0, sizeof ct); memcpy(ct, ct_, n);
    for (int a = 0; a < 26; a++) ML[a] = MH[a] = 0;
    for (int i = 0; i < n; i++) { if (i < 64) ML[ct[i]] |= 1ull << i; else MH[ct[i]] |= 1ull << (i - 64); }
    // lane -> window map of the u8 gather layout (marker test: window w has first letter 1, others 0)
    for (int w = 0; w < 64; w++) { alignas(64) uint8_t pb[128] = {0}; pb[w] = 1; __m512i xs[4];
      xs_of(_mm512_load_si512(pb), _mm512_load_si512(pb + 64), xs);
      for (int k = 0; k < 4; k++) { alignas(64) int32_t v[16]; _mm512_store_si512(v, xs[k]); for (int l = 0; l < 16; l++) if (v[l] == 17576) lanewin[k][l] = w; } }
    for (int i = 0; i < 128; i++) keepw[i] = true; nkeep = n - 3;
  }
  void set_mask(const bool* drop_letter) {   // nullptr = no mask
    usemask = drop_letter != nullptr; nkeep = n - 3;
    for (int w = 0; w < n - 3; w++) { keepw[w] = true; if (usemask) for (int j = 0; j < 4; j++) if (drop_letter[w + j]) keepw[w] = false; nkeep -= !keepw[w]; }
    for (int k = 0; k < 4; k++) { wmk[k] = 0; for (int l = 0; l < 16; l++) if (keepw[lanewin[k][l]]) wmk[k] |= (__mmask16)(1u << l); }
  }
  static inline void xs_of(__m512i p0, __m512i p1, __m512i* xs) {
    const __m512i id = _mm512_set_epi8(63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,
                                       31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0);
    const __m512i i1 = _mm512_add_epi8(id, _mm512_set1_epi8(1)), i2 = _mm512_add_epi8(id, _mm512_set1_epi8(2)), i3 = _mm512_add_epi8(id, _mm512_set1_epi8(3));
    __m512i q1 = _mm512_permutex2var_epi8(p0, i1, p1), q2 = _mm512_permutex2var_epi8(p0, i2, p1), q3 = _mm512_permutex2var_epi8(p0, i3, p1);
    const __m512i z = _mm512_setzero_si512(), c26 = _mm512_set1_epi16(26), cq = _mm512_set1_epi32((1 << 16) | 676);
    __m512i al = _mm512_unpacklo_epi8(p0, z), ah = _mm512_unpackhi_epi8(p0, z), bl = _mm512_unpacklo_epi8(q1, z), bh = _mm512_unpackhi_epi8(q1, z);
    __m512i cl = _mm512_unpacklo_epi8(q2, z), ch = _mm512_unpackhi_epi8(q2, z), dl = _mm512_unpacklo_epi8(q3, z), dh = _mm512_unpackhi_epi8(q3, z);
    __m512i Bl = _mm512_add_epi16(_mm512_mullo_epi16(al, c26), bl), Bh = _mm512_add_epi16(_mm512_mullo_epi16(ah, c26), bh);
    __m512i Cl = _mm512_add_epi16(_mm512_mullo_epi16(cl, c26), dl), Ch = _mm512_add_epi16(_mm512_mullo_epi16(ch, c26), dh);
    xs[0] = _mm512_madd_epi16(_mm512_unpacklo_epi16(Bl, Cl), cq); xs[1] = _mm512_madd_epi16(_mm512_unpackhi_epi16(Bl, Cl), cq);
    xs[2] = _mm512_madd_epi16(_mm512_unpacklo_epi16(Bh, Ch), cq); xs[3] = _mm512_madd_epi16(_mm512_unpackhi_epi16(Bh, Ch), cq);
  }
  void set_setting(const uint8_t (*T)[32]) {
    memset(TT, 0, sizeof TT); memset(H, 0, sizeof H);
    for (int i = 0; i < n; i++) { const uint8_t* t = T[i]; int a = ct[i];
      for (int v = 0; v < 26; v++) { TT[v][i] = t[v]; H[a][v][t[v]]++; } }
  }
  // ---- board state ----
  struct St { alignas(64) uint8_t S[64]; __m512i y0, y1; __m256i h; int score; };
  static inline __m512i loadS(const uint8_t* S) { return _mm512_load_si512(S); }
  template <int ORD = 4> inline void full(St& st) const {
    __m512i y0 = _mm512_setzero_si512(), y1 = _mm512_setzero_si512(); __m256i h = _mm256_setzero_si256();
    for (int a = 0; a < 26; a++) { int v = st.S[a];
      y0 = _mm512_mask_loadu_epi8(y0, ML[a], TT[v]); y1 = _mm512_mask_loadu_epi8(y1, MH[a], TT[v] + 64);
      h = _mm256_add_epi8(h, _mm256_load_si256((const __m256i*)H[a][v])); }
    st.y0 = y0; st.y1 = y1; st.h = h; st.score = score_of<ORD>(st.S, y0, y1, h);
  }
  static inline int sumsq(__m256i h) {
    __m256i sq = _mm256_maddubs_epi16(h, h);
    __m256i s32 = _mm256_madd_epi16(sq, _mm256_set1_epi16(1));
    __m128i t = _mm_add_epi32(_mm256_castsi256_si128(s32), _mm256_extracti128_si256(s32, 1));
    t = _mm_add_epi32(t, _mm_shuffle_epi32(t, 0x4E)); t = _mm_add_epi32(t, _mm_shuffle_epi32(t, 0xB1));
    return _mm_cvtsi128_si32(t);
  }
  inline int score3(__m512i p0, __m512i p1, __m256i h) const {
    const __m512i id = _mm512_set_epi8(63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,
                                       31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0);
    const __m512i i1 = _mm512_add_epi8(id, _mm512_set1_epi8(1)), i2 = _mm512_add_epi8(id, _mm512_set1_epi8(2));
    __m512i q1 = _mm512_permutex2var_epi8(p0, i1, p1), q2 = _mm512_permutex2var_epi8(p0, i2, p1);
    const __m512i z = _mm512_setzero_si512();
    const __m512i c676 = _mm512_set1_epi16(676), c26 = _mm512_set1_epi16(26);
    __m512i xl = _mm512_add_epi16(_mm512_add_epi16(_mm512_mullo_epi16(_mm512_unpacklo_epi8(p0, z), c676), _mm512_mullo_epi16(_mm512_unpacklo_epi8(q1, z), c26)), _mm512_unpacklo_epi8(q2, z));
    __m512i xh = _mm512_add_epi16(_mm512_add_epi16(_mm512_mullo_epi16(_mm512_unpackhi_epi8(p0, z), c676), _mm512_mullo_epi16(_mm512_unpackhi_epi8(q1, z), c26)), _mm512_unpackhi_epi8(q2, z));
    int nw = n - 2; const void* A = L->A3.data(); __m512i sum = z;
    if (nw >= 64) {
      __m512i xs[4] = {_mm512_cvtepu16_epi32(_mm512_castsi512_si256(xl)), _mm512_cvtepu16_epi32(_mm512_extracti64x4_epi64(xl, 1)),
                       _mm512_cvtepu16_epi32(_mm512_castsi512_si256(xh)), _mm512_cvtepu16_epi32(_mm512_extracti64x4_epi64(xh, 1))};
      for (int k = 0; k < 4; k++) { __m512i g = _mm512_i32gather_epi32(xs[k], A, 2); sum = _mm512_add_epi32(sum, _mm512_srai_epi32(_mm512_slli_epi32(g, 16), 16)); }
      if (nw > 64) {
        __m512i r0 = _mm512_cvtepu8_epi32(_mm512_castsi512_si128(p1));
        __m512i r1 = _mm512_cvtepu8_epi32(_mm512_castsi512_si128(_mm512_permutexvar_epi8(i1, p1)));
        __m512i r2 = _mm512_cvtepu8_epi32(_mm512_castsi512_si128(_mm512_permutexvar_epi8(i2, p1)));
        const __m512i c = _mm512_set1_epi32(26);
        __m512i ix = _mm512_add_epi32(_mm512_mullo_epi32(_mm512_add_epi32(_mm512_mullo_epi32(r0, c), r1), c), r2);
        __m512i g = _mm512_mask_i32gather_epi32(z, (__mmask16)((1u << (nw - 64)) - 1), ix, A, 2);
        sum = _mm512_add_epi32(sum, _mm512_srai_epi32(_mm512_slli_epi32(g, 16), 16));
      }
    } else {
      alignas(64) uint8_t p[128]; _mm512_store_si512(p, p0); _mm512_store_si512(p + 64, p1);
      int s2 = 0; for (int i = 0; i < nw; i++) s2 += L->A3[(p[i]*26+p[i+1])*26+p[i+2]];
      sum = _mm512_castsi128_si512(_mm_cvtsi32_si128(s2));
    }
    return _mm512_reduce_add_epi32(sum) + L->lam3_int * (sumsq(h) - n);
  }
  template <int ORD = 4> inline int score_of(const uint8_t* S, __m512i y0, __m512i y1, __m256i h) const { return score_v<ORD>(loadS(S), y0, y1, h); }
  template <int ORD = 4> inline int score_v(__m512i Sv, __m512i y0, __m512i y1, __m256i h) const {
    __m512i p0 = _mm512_permutexvar_epi8(y0, Sv), p1 = _mm512_permutexvar_epi8(y1, Sv);
    if (ORD == 3) return score3(p0, p1, h);
    // quad window indices in registers: q_k[j] = p[j+k]
    const __m512i id = _mm512_set_epi8(63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48,47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32,
                                       31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0);
    const __m512i i1 = _mm512_add_epi8(id, _mm512_set1_epi8(1)), i2 = _mm512_add_epi8(id, _mm512_set1_epi8(2)), i3 = _mm512_add_epi8(id, _mm512_set1_epi8(3));
    __m512i q1 = _mm512_permutex2var_epi8(p0, i1, p1), q2 = _mm512_permutex2var_epi8(p0, i2, p1), q3 = _mm512_permutex2var_epi8(p0, i3, p1);
    const __m512i z = _mm512_setzero_si512(), c26 = _mm512_set1_epi16(26), cq = _mm512_set1_epi32((1 << 16) | 676);
    __m512i al = _mm512_unpacklo_epi8(p0, z), ah = _mm512_unpackhi_epi8(p0, z), bl = _mm512_unpacklo_epi8(q1, z), bh = _mm512_unpackhi_epi8(q1, z);
    __m512i cl = _mm512_unpacklo_epi8(q2, z), ch = _mm512_unpackhi_epi8(q2, z), dl = _mm512_unpacklo_epi8(q3, z), dh = _mm512_unpackhi_epi8(q3, z);
    __m512i Bl = _mm512_add_epi16(_mm512_mullo_epi16(al, c26), bl), Bh = _mm512_add_epi16(_mm512_mullo_epi16(ah, c26), bh);
    __m512i Cl = _mm512_add_epi16(_mm512_mullo_epi16(cl, c26), dl), Ch = _mm512_add_epi16(_mm512_mullo_epi16(ch, c26), dh);
    __m512i xs[4] = {_mm512_madd_epi16(_mm512_unpacklo_epi16(Bl, Cl), cq), _mm512_madd_epi16(_mm512_unpackhi_epi16(Bl, Cl), cq),
                     _mm512_madd_epi16(_mm512_unpacklo_epi16(Bh, Ch), cq), _mm512_madd_epi16(_mm512_unpackhi_epi16(Bh, Ch), cq)};
    int nw = n - 3;   // n in 65..72 assumed by the production driver (72); generic fallback below
    __m512i sum = z;
    const void* A = L->A.data();
    if (g_u8 && nw >= 64) {
      const void* B = L->A8.data(); const __m512i ff = _mm512_set1_epi32(0xFF);
      if (!usemask) for (int k = 0; k < 4; k++) sum = _mm512_add_epi32(sum, _mm512_and_si512(_mm512_i32gather_epi32(xs[k], B, 1), ff));
      else for (int k = 0; k < 4; k++) sum = _mm512_add_epi32(sum, _mm512_and_si512(_mm512_mask_i32gather_epi32(z, wmk[k], xs[k], B, 1), ff));
      if (nw > 64) {
        alignas(64) uint8_t p[128]; _mm512_store_si512(p + 64, p1);
        const uint8_t* b8 = L->A8.data(); int t = 0; for (int i = 64; i < nw; i++) if (keepw[i]) t += b8[((p[i]*26+p[i+1])*26+p[i+2])*26+p[i+3]];
        sum = _mm512_add_epi32(sum, _mm512_castsi128_si512(_mm_cvtsi32_si128(t)));
      }
      int s8 = _mm512_reduce_add_epi32(sum);
      return s8 + L->lam8_int * (sumsq(h) - n);
    }
    if (nw >= 64) {
      for (int k = 0; k < 4; k++) { __m512i g = _mm512_i32gather_epi32(xs[k], A, 2); sum = _mm512_add_epi32(sum, _mm512_srai_epi32(_mm512_slli_epi32(g, 16), 16)); }
      if (nw > 64) {   // windows 64..nw-1 from p1
        __m512i r0 = _mm512_cvtepu8_epi32(_mm512_castsi512_si128(p1));
        __m512i r1 = _mm512_cvtepu8_epi32(_mm512_castsi512_si128(_mm512_permutexvar_epi8(i1, p1)));
        __m512i r2 = _mm512_cvtepu8_epi32(_mm512_castsi512_si128(_mm512_permutexvar_epi8(i2, p1)));
        __m512i r3 = _mm512_cvtepu8_epi32(_mm512_castsi512_si128(_mm512_permutexvar_epi8(i3, p1)));
        const __m512i c = _mm512_set1_epi32(26);
        __m512i ix = _mm512_add_epi32(_mm512_mullo_epi32(_mm512_add_epi32(_mm512_mullo_epi32(_mm512_add_epi32(_mm512_mullo_epi32(r0, c), r1), c), r2), c), r3);
        __m512i g = _mm512_mask_i32gather_epi32(z, (__mmask16)((1u << (nw - 64)) - 1), ix, A, 2);
        sum = _mm512_add_epi32(sum, _mm512_srai_epi32(_mm512_slli_epi32(g, 16), 16));
      }
    } else {  // short texts: scalar
      alignas(64) uint8_t p[128]; _mm512_store_si512(p, p0); _mm512_store_si512(p + 64, p1);
      int s2 = 0; for (int i = 0; i < nw; i++) s2 += L->A[((p[i]*26+p[i+1])*26+p[i+2])*26+p[i+3]];
      sum = _mm512_castsi128_si512(_mm_cvtsi32_si128(s2));
    }
    int s = _mm512_reduce_add_epi32(sum);
    // IC: sum h^2 - n  (h = histogram of y == histogram of p up to relabeling)
    __m256i sq = _mm256_maddubs_epi16(h, h);                // h in 0..72 fits u8/s8
    __m256i s32 = _mm256_madd_epi16(sq, _mm256_set1_epi16(1));
    __m128i t = _mm_add_epi32(_mm256_castsi256_si128(s32), _mm256_extracti128_si256(s32, 1));
    t = _mm_add_epi32(t, _mm_shuffle_epi32(t, 0x4E)); t = _mm_add_epi32(t, _mm_shuffle_epi32(t, 0xB1));
    int hh = _mm_cvtsi128_si32(t) - n;
    return s + L->lam_int * hh;
  }
  // score of st with S changed at letters in chg (their new values already in S2); y/h updated incrementally
  template <int ORD = 4> inline int trial(const St& st, const uint8_t* S2, const int* chg, int nc, __m512i& y0o, __m512i& y1o, __m256i& ho) {
    __m512i y0 = st.y0, y1 = st.y1; __m256i h = st.h;
    for (int k = 0; k < nc; k++) { int a = chg[k]; int vo = st.S[a], vn = S2[a];
      y0 = _mm512_mask_loadu_epi8(y0, ML[a], TT[vn]); y1 = _mm512_mask_loadu_epi8(y1, MH[a], TT[vn] + 64);
      h = _mm256_add_epi8(_mm256_sub_epi8(h, _mm256_load_si256((const __m256i*)H[a][vo])), _mm256_load_si256((const __m256i*)H[a][vn])); }
    y0o = y0; y1o = y1; ho = h; evals++;
    return score_of<ORD>(S2, y0, y1, h);
  }
  // register-only trial: letters chg[k] get new values nv[k]
  template <int ORD = 4> inline int trial_v(const St& st, __m512i Sv, const int* chg, const int* nv, int nc, __m512i& y0o, __m512i& y1o, __m256i& ho, __m512i& S2o) {
    __m512i y0 = st.y0, y1 = st.y1; __m256i h = st.h; __m512i S2 = Sv;
    for (int k = 0; k < nc; k++) { int a = chg[k], vo = st.S[a], vn = nv[k];
      S2 = _mm512_mask_mov_epi8(S2, 1ull << a, _mm512_set1_epi8((char)vn));
      y0 = _mm512_mask_loadu_epi8(y0, ML[a], TT[vn]); y1 = _mm512_mask_loadu_epi8(y1, MH[a], TT[vn] + 64);
      h = _mm256_add_epi8(_mm256_sub_epi8(h, _mm256_load_si256((const __m256i*)H[a][vo])), _mm256_load_si256((const __m256i*)H[a][vn])); }
    y0o = y0; y1o = y1; ho = h; S2o = S2; evals++;
    return score_v<ORD>(S2, y0, y1, h);
  }
  void decrypt(const uint8_t* S, uint8_t* out) const {   // scalar reference
    for (int i = 0; i < n; i++) out[i] = S[TT[S[ct[i]]][i]];
  }
};

static int g_stale = 325; static bool g_repair = true;
struct PairTab { int a[325], b[325]; PairTab() { int k = 0; for (int x = 0; x < 26; x++) for (int y = x + 1; y < 26; y++) { a[k] = x; b[k] = y; k++; } } };
static const PairTab PT;

// Apply toggle (a,b) to S2 (copy of S); record changed letters. Returns false if blocked by cap.
static inline bool toggle(const uint8_t* S, uint8_t* S2, int a, int b, int pairs, int cap, int* chg, int& nc, int& npairs) {
  nc = 0; npairs = pairs;
  if (S[a] == b) { S2[a] = a; S2[b] = b; chg[nc++] = a; chg[nc++] = b; npairs--; return true; }
  bool af = S[a] == a, bf = S[b] == b;
  if (pairs >= cap && af && bf) return false;
  int x = S[a], y = S[b];
  if (x != a) { S2[x] = x; chg[nc++] = x; npairs--; }
  if (y != b) { S2[y] = y; chg[nc++] = y; npairs--; }
  S2[a] = b; S2[b] = a; chg[nc++] = a; chg[nc++] = b; npairs++;
  return true;
}

static inline bool toggle_v(const uint8_t* S, int a, int b, int pairs, int cap, int* chg, int* nv, int& nc, int& npairs) {
  nc = 0; npairs = pairs;
  if (S[a] == b) { chg[0] = a; nv[0] = a; chg[1] = b; nv[1] = b; nc = 2; npairs--; return true; }
  bool af = S[a] == a, bf = S[b] == b;
  if (pairs >= cap && af && bf) return false;
  int x = S[a], y = S[b];
  if (x != a) { chg[nc] = x; nv[nc++] = x; npairs--; }
  if (y != b) { chg[nc] = y; nv[nc++] = y; npairs--; }
  chg[nc] = a; nv[nc++] = b; chg[nc] = b; nv[nc++] = a; npairs++;
  return true;
}
// One climb from st (modified in place). Returns final score.
template <int ORD = 4> static int fast_climb(Fast& F, Fast::St& st, int cap, int* trace = nullptr, const int* chk = nullptr, int nchk = 0) {
  F.full<ORD>(st); long ev0 = F.evals; int ci = 0;
  auto tr = [&]() { while (trace && ci < nchk && F.evals - ev0 >= chk[ci]) trace[ci++] = st.score; };
  for (;;) {
    int pairs = 0; for (int a = 0; a < 26; a++) pairs += st.S[a] > a;
    // dynamic order: score all toggles once
    int sc[325]; int visit[325];
    alignas(64) uint8_t S2[64]; int chg[4], nc, np;
    __m512i y0, y1; __m256i h;
    __m512i Sv = _mm512_load_si512(st.S), S2v; int nv[4];
    for (int mv = 0; mv < 325; mv++) {
      visit[mv] = mv;
      if (!toggle_v(st.S, PT.a[mv], PT.b[mv], pairs, cap, chg, nv, nc, np)) { sc[mv] = INT32_MIN; continue; }
      sc[mv] = F.trial_v<ORD>(st, Sv, chg, nv, nc, y0, y1, h, S2v);
    }
    std::sort(visit, visit + 325, [&](int i, int j) { return sc[i] != sc[j] ? sc[i] > sc[j] : i < j; });
    int cursor = 0, stale = 0;
    while (stale < g_stale) {
      int mv = visit[cursor]; if (++cursor == 325) cursor = 0;
      if (!toggle_v(st.S, PT.a[mv], PT.b[mv], pairs, cap, chg, nv, nc, np)) { stale++; continue; }
      int s = F.trial_v<ORD>(st, Sv, chg, nv, nc, y0, y1, h, S2v);
      if (s > st.score) { Sv = S2v; _mm512_store_si512(st.S, Sv); st.y0 = y0; st.y1 = y1; st.h = h; st.score = s; pairs = np; stale = 0; }
      else stale++;
      tr();
    }
    // re-pair barrier
    if (!g_repair) break;
    int plo[13], phi[13], k = 0;
    for (int a = 0; a < 26; a++) if (st.S[a] > a) { plo[k] = a; phi[k] = st.S[a]; k++; }
    int best = st.score; alignas(64) uint8_t bestS[64]; bool found = false; __m512i by0, by1; __m256i bh;
    for (int i = 0; i < k; i++) for (int j = i + 1; j < k; j++) {
      int a = plo[i], x = phi[i], b = plo[j], y = phi[j]; int ch[4] = {a, b, x, y};
      memcpy(S2, st.S, 64); S2[a] = b; S2[b] = a; S2[x] = y; S2[y] = x;
      int s1 = F.trial<ORD>(st, S2, ch, 4, y0, y1, h);
      if (s1 > best) { best = s1; memcpy(bestS, S2, 64); by0 = y0; by1 = y1; bh = h; found = true; }
      memcpy(S2, st.S, 64); S2[a] = y; S2[y] = a; S2[x] = b; S2[b] = x;
      int s2 = F.trial<ORD>(st, S2, ch, 4, y0, y1, h);
      if (s2 > best) { best = s2; memcpy(bestS, S2, 64); by0 = y0; by1 = y1; bh = h; found = true; }
    }
    if (!found) break;
    tr();
    memcpy(st.S, bestS, 64); st.y0 = by0; st.y1 = by1; st.h = bh; st.score = best;
  }
  while (trace && ci < nchk) trace[ci++] = st.score;
  return st.score;
}
static inline void fast_kick(uint8_t* S, int k, uint64_t& rs) {
  auto rnd = [&]() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; };
  int fr[26], nf = 0; for (int a = 0; a < 26; a++) if (S[a] == a) fr[nf++] = a;
  int want = std::min(2 * k, nf - (nf & 1));
  for (int i = 0; i < want; i++) { int j = i + rnd() % (nf - i); std::swap(fr[i], fr[j]); }
  for (int i = 0; i + 1 < want; i += 2) { S[fr[i]] = fr[i+1]; S[fr[i+1]] = fr[i]; }
}
