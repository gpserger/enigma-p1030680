// bombe.cc - garble-tolerant software bombe (Turing/Welchman with diagonal board) for Enigma M4.
//
// For every rotor setting (reflector x Greek x wheel order x Greek/left/middle/right core offsets x
// right-ring phase x middle-ring turnover class) and every crib spec (TEXT@OFFSET), the crib's
// letter graph is split into B = k+1 contiguous blocks (after excluding forced-garble positions,
// i.e. crib letter == cipher letter). By pigeonhole, with <= k further garbles at least one block
// is garble-free, so the true setting always produces a stop in that block. Each block is tested by
// hypothesising the stecker partner of its most-connected letter (26 hypotheses) and propagating
// implied pairs (involutive plugboard = diagonal board), rejecting on contradiction or > maxplugs
// pairs. A stop is then verified against the whole crib: the block's plugboard is extended by
// majority vote and the number g of inconsistent crib positions (= implied garbles) is reported.
//
// Scrambler model and ring semantics are those of tools/run_m4/src/core.h (validated against
// py-enigma on CFYZR, U-264, IPZAYK, P1030698). Greek and left rings are degenerate with position
// and fixed at A.
#include "../run_m4/src/core.h"
#include "../run_m4/src/hc.h"
int g_climb_order = 4;
#include <omp.h>
#include <immintrin.h>
#include <atomic>
#include <chrono>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <functional>

struct Edge { uint8_t pos, other; };  // pos = message position
struct Op { uint8_t pos, src, dst, check; };
struct Block {
  int pivot;
  std::vector<Edge> adj[26];
  int npos;
  std::vector<Op> prog;   // BFS spanning-tree propagation + loop-closure checks (vector prefilter)
};
static bool g_scalar = false;
struct Crib {
  std::string name, text; int off;
  std::vector<int> excl;          // message positions excluded (forced + manual)
  std::vector<int> used;          // message positions used
  std::vector<Block> blocks;
  uint8_t P[MAXLEN];              // plaintext letter at message position (valid for used)
};

static int g_len; static uint8_t g_ct[MAXLEN];
static int g_maxplugs = 10, g_gmax = 12;
static int g_span;  // number of letters for which stepping must be exact (max crib end, or full)

// ---------- stepping patterns ----------
struct Pattern { uint8_t dM[MAXLEN], dL[MAXLEN]; int wm0; };
// window-based simulation: r = right window, m = middle window before first key press
static void sim_pattern(int wR, int wM, int wr0, int wm0, int n, Pattern& p) {
  int r = wr0, m = wm0, dm = 0, dl = 0;
  for (int i = 0; i < n; i++) {
    bool mn = TB.notch[wM][m], rn = TB.notch[wR][r];
    if (mn) { m = md(m + 1); dm++; dl++; } else if (rn) { m = md(m + 1); dm++; }
    r = md(r + 1);
    p.dM[i] = dm % 26; p.dL[i] = dl % 26;
  }
  p.wm0 = wm0;
}

// ---------- per-job precomputation ----------
alignas(32) static uint8_t RF[8][26][32], RB[8][26][32];  // right wheel fwd/back at core shift (padded)
static void init_rtabs() {
  memset(RF, 0, sizeof RF); memset(RB, 0, sizeof RB);
  for (int w = 0; w < 8; w++) for (int s = 0; s < 26; s++) for (int c = 0; c < 26; c++) {
    RF[w][s][c] = rot_f(w, s, c); RB[w][s][c] = rot_b(w, s, c);
  }
}
// U[((g*26+l)*26+m)*26+x] = inner permutation (middle, left, greek, reflector and back)
static void build_U(int refl, int greek, int wL, int wM, uint8_t* U) {
  for (int g = 0; g < 26; g++) for (int l = 0; l < 26; l++) for (int m = 0; m < 26; m++) {
    uint8_t* u = U + ((g * 26 + l) * 26 + m) * 26;
    for (int c = 0; c < 26; c++) {
      int x = rot_f(wM, m, c); x = rot_f(wL, l, x); x = rot_f(greek, g, x);
      x = TB.refl[refl][x];
      x = rot_b(greek, g, x); x = rot_b(wL, l, x); x = rot_b(wM, m, x);
      u[c] = x;
    }
  }
}

struct Ctx {  // per-setting fast T_i(x)
  const uint8_t* rf[MAXLEN]; const uint8_t* rb[MAXLEN]; int base[MAXLEN]; const uint8_t* Ug;
  inline const uint8_t* ur(int i) const { return Ug + base[i]; }
  inline int T(int i, int x) const { return rb[i][ur(i)[rf[i][x]]]; }
};

// ---------- bombe test ----------
// Vector prefilter: all 26 pivot hypotheses at once (one byte lane each) using AVX-512 VBMI byte
// permutes; propagates along a BFS spanning tree of the block's menu and checks every loop
// closure. Returns the mask of hypotheses that survive (a superset of the full test's survivors).
static inline uint32_t vec_block(const Block& b, const Ctx& cx) {
  __m256i val[26];
  val[b.pivot] = _mm256_setr_epi8(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,0);
  uint32_t alive = (1u << 26) - 1;
  for (const Op& op : b.prog) {
    __m256i v = val[op.src];
    v = _mm256_permutexvar_epi8(v, _mm256_load_si256((const __m256i*)cx.rf[op.pos]));
    v = _mm256_permutexvar_epi8(v, _mm256_loadu_si256((const __m256i*)cx.ur(op.pos)));
    v = _mm256_permutexvar_epi8(v, _mm256_load_si256((const __m256i*)cx.rb[op.pos]));
    if (!op.check) val[op.dst] = v;
    else { alive &= (uint32_t)_mm256_cmpeq_epi8_mask(v, val[op.dst]); if (!alive) return 0; }
  }
  return alive;
}
static inline bool block_test(const Block& b, const Ctx& cx, int8_t* Sout, long& nT) {
  int8_t S[26]; int st[64];
  uint32_t cand = g_scalar ? (1u << 26) - 1 : vec_block(b, cx);
  for (int x = 0; x < 26; x++) {
    if (!(cand >> x & 1)) continue;
    memset(S, -1, 26); int sp = 0, np = 0; bool ok = true;
    auto assign = [&](int a, int v) -> bool {
      if (S[a] == v) return true;
      if (S[a] >= 0 || S[v] >= 0) return false;
      S[a] = v; S[v] = a; st[sp++] = a; if (v != a) { st[sp++] = v; if (++np > g_maxplugs) return false; }
      return true;
    };
    assign(b.pivot, x);
    while (ok && sp) {
      int u = st[--sp]; int v = S[u];
      for (const Edge& e : b.adj[u]) { nT++; if (!assign(e.other, cx.T(e.pos, v))) { ok = false; break; } }
    }
    if (ok) { memcpy(Sout, S, 26); return true; }  // first surviving hypothesis
  }
  return false;
}

// extend partial plugboard by majority vote over all used crib positions; return mismatches
struct Verify { int g, undet, npairs; int8_t S[26]; };
static void verify(const Crib& cr, const Ctx& cx, const int8_t* S0, Verify& out) {
  int8_t S[26]; memcpy(S, S0, 26);
  for (;;) {
    int votes[26][26]; memset(votes, 0, sizeof votes); bool any = false;
    for (int i : cr.used) {
      int a = cr.P[i], c = g_ct[i];
      if (S[a] >= 0 && S[c] < 0) { votes[c][cx.T(i, S[a])]++; any = true; }
      else if (S[c] >= 0 && S[a] < 0) { votes[a][cx.T(i, S[c])]++; any = true; }
    }
    if (!any) break;
    int best = 0, ba = -1, bv = -1;
    for (int a = 0; a < 26; a++) for (int v = 0; v < 26; v++)
      if (votes[a][v] > best && S[a] < 0 && (S[v] < 0 || v == a)) { best = votes[a][v]; ba = a; bv = v; }
    if (ba < 0) break;
    S[ba] = bv; S[bv] = ba;
  }
  int g = 0, u = 0, np = 0;
  for (int i : cr.used) {
    int a = cr.P[i], c = g_ct[i];
    if (S[a] < 0 || S[c] < 0) { u++; continue; }
    if (S[c] != cx.T(i, S[a])) g++;
  }
  for (int a = 0; a < 26; a++) if (S[a] > a) np++;
  out.g = g; out.undet = u; out.npairs = np; memcpy(out.S, S, 26);
}

// ---------- crib setup ----------
static void setup_crib(Crib& cr, int nblocks, const std::vector<int>& manual_excl) {
  int n = cr.text.size();
  std::vector<bool> ex(g_len, false);
  for (int j = 0; j < n; j++) {
    int i = cr.off + j; cr.P[i] = cr.text[j] - 'A';
    if (cr.text[j] == '?' ) { ex[i] = true; continue; }
    if (cr.P[i] == g_ct[i]) ex[i] = true;
  }
  for (int i : manual_excl) if (i >= cr.off && i < cr.off + n) ex[i] = true;
  for (int j = 0; j < n; j++) { int i = cr.off + j; if (ex[i]) cr.excl.push_back(i); else cr.used.push_back(i); }
  int nu = cr.used.size();
  // Partition the used positions into nblocks disjoint sets (pigeonhole needs only disjointness),
  // chosen by local search to maximise the loop count (cycle rank) of each block's best connected
  // component: min over blocks first, then the sum.
  auto rank_of = [&](const std::vector<int>& ps, int* pivot_out, std::vector<bool>* incomp) {
    int par[26]; for (int a = 0; a < 26; a++) par[a] = a;
    std::function<int(int)> fnd = [&](int a) { return par[a] == a ? a : par[a] = fnd(par[a]); };
    int ne[26] = {0}, nv[26] = {0}; bool present[26] = {false};
    for (int i : ps) { int a = cr.P[i], c = g_ct[i]; present[a] = present[c] = true; par[fnd(a)] = fnd(c); }
    for (int a = 0; a < 26; a++) if (present[a]) nv[fnd(a)]++;
    for (int i : ps) ne[fnd(cr.P[i])]++;
    int best = -1, br = -1;
    for (int a = 0; a < 26; a++) if (present[a] && fnd(a) == a) { int r = ne[a] - nv[a] + 1; if (r > best || (r == best && ne[a] > ne[br])) { best = r; br = a; } }
    if (pivot_out) {
      int deg[26] = {0};
      for (int i : ps) if (fnd(cr.P[i]) == br) { deg[cr.P[i]]++; deg[g_ct[i]]++; }
      *pivot_out = std::max_element(deg, deg + 26) - deg;
      if (incomp) { incomp->assign(MAXLEN, false); for (int i : ps) if (fnd(cr.P[i]) == br) (*incomp)[i] = true; }
    }
    return best;
  };
  std::vector<int> asg(nu); for (int q = 0; q < nu; q++) asg[q] = (long)q * nblocks / nu;
  auto objective = [&](const std::vector<int>& as) {
    std::vector<std::vector<int>> ps(nblocks); for (int q = 0; q < nu; q++) ps[as[q]].push_back(cr.used[q]);
    int mn = 1 << 30, sum = 0; for (auto& p : ps) { int r = rank_of(p, nullptr, nullptr); mn = std::min(mn, r); sum += r; }
    return (long)mn * 1000 + sum;
  };
  if (nblocks > 1) {
    uint64_t rs = 88172645463325252ull; auto rnd = [&]() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; };
    long cur = objective(asg);
    for (int it = 0; it < 30000; it++) {
      int q1 = rnd() % nu, q2 = rnd() % nu; if (asg[q1] == asg[q2]) continue;
      std::swap(asg[q1], asg[q2]); long o = objective(asg);
      if (o >= cur) cur = o; else std::swap(asg[q1], asg[q2]);
    }
  }
  for (int b = 0; b < nblocks; b++) {
    Block bl; bl.npos = 0;
    std::vector<int> ps; for (int q = 0; q < nu; q++) if (asg[q] == b) ps.push_back(cr.used[q]);
    std::vector<bool> incomp; rank_of(ps, &bl.pivot, &incomp);
    for (int i : ps) {
      bl.npos++;
      if (!incomp[i]) continue;  // edges outside the tested component carry no constraint on the pivot
      int a = cr.P[i], c = g_ct[i];
      bl.adj[a].push_back({(uint8_t)i, (uint8_t)c}); bl.adj[c].push_back({(uint8_t)i, (uint8_t)a});
    }
    { // greedy op order: close loops as early as possible (checks first, then the tree edge whose new
      // letter has the most edges back into the visited set)
      bool vis[26] = {false}; std::vector<bool> used(MAXLEN, false); vis[bl.pivot] = true;
      for (;;) {
        bool progress = false;
        for (int u = 0; u < 26; u++) if (vis[u]) for (const Edge& e : bl.adj[u])
          if (!used[e.pos] && vis[e.other]) { used[e.pos] = true; bl.prog.push_back({e.pos, (uint8_t)u, e.other, 1}); progress = true; }
        int bu = -1, bw = -1, bpos = -1, bscore = -1;
        for (int u = 0; u < 26; u++) if (vis[u]) for (const Edge& e : bl.adj[u]) {
          if (used[e.pos] || vis[e.other]) continue;
          int sc = 0; for (const Edge& f : bl.adj[e.other]) if (f.pos != e.pos && vis[f.other]) sc++;
          if (sc > bscore) { bscore = sc; bu = u; bw = e.other; bpos = e.pos; } }
        if (bu >= 0) { used[bpos] = true; vis[bw] = true; bl.prog.push_back({(uint8_t)bpos, (uint8_t)bu, (uint8_t)bw, 0}); progress = true; }
        if (!progress) break;
      } }
    cr.blocks.push_back(bl);
  }
}

// ---------- job list ----------
struct Job { int refl, greek, wL, wM, wR; };
static std::string wheel_str(const Job& j) { char b[8]; snprintf(b, 8, "%d%d%d", j.wL + 1, j.wM + 1, j.wR + 1); return b; }

struct Hit {
  int8_t raw[26]; int crib; Job j; int pG, pL, pM, pR, rM, rR; int g, undet, npairs, blocks; std::string S;
};

static std::string sstr(const int8_t* S) {
  std::string s;
  for (int a = 0; a < 26; a++) {
    if (S[a] < 0) continue;
    if (S[a] > a) { if (!s.empty()) s += ' '; s += char('A' + a); s += char('A' + S[a]); }
  }
  std::string self;
  for (int a = 0; a < 26; a++) if (S[a] == a) self += char('A' + a);
  return s + " | self:" + self;
}

// Scorer with crib penalty: quadgram sum minus W per crib position not reproduced.
struct CribScorer {
  const float* tri; int len; const uint8_t* ct; const uint8_t (*T)[26]; const uint8_t* P; const int* pos; int npos; float W;
  inline float eval(const int* S) const {
    uint8_t p[MAXLEN];
    for (int i = 0; i < len; i++) p[i] = (uint8_t)S[T[i][S[ct[i]]]];
    float s = 0; int idx = (p[0] * 26 + p[1]) * 26 + p[2];
    for (int i = 3; i < len; i++) { idx = (idx % 17576) * 26 + p[i]; s += tri[idx]; }
    for (int q = 0; q < npos; q++) if (p[pos[q]] != P[pos[q]]) s -= W;
    return s;
  }
};
template <class SC>
static float climbT(const SC& sc, int* S, const bool* fixed, const int* order, int maxplugs) {
  float best = sc.eval(S); bool improved = true; int T2[26];
  while (improved) {
    improved = false;
    for (int ii = 0; ii < 26; ii++) {
      int a = order[ii]; if (fixed[a]) continue;
      for (int kk = ii + 1; kk < 26; kk++) {
        int b = order[kk]; if (fixed[b]) continue;
        int x = S[a], z = S[b];
        if (x == b) { S[a] = a; S[b] = b; float s = sc.eval(S);
          if (s > best + 1e-4f) { best = s; improved = true; continue; } S[a] = b; S[b] = a; continue; }
        if ((x != a && fixed[x]) || (z != b && fixed[z])) continue;
        memcpy(T2, S, sizeof T2);
        if (x != a) S[x] = x; if (z != b) S[z] = z; S[a] = b; S[b] = a;
        int np = nplugs(S); float bs = -1e30f; int bestv = 0;
        if (np <= maxplugs) { float s = sc.eval(S); if (s > best + 1e-4f) { bs = s; bestv = 1; } }
        if (x != a && z != b) { S[x] = z; S[z] = x;
          if (np + 1 <= maxplugs) { float s = sc.eval(S); if (s > best + 1e-4f && s > bs) { bs = s; bestv = 2; } }
          S[x] = x; S[z] = z; }
        if (bestv == 0) memcpy(S, T2, sizeof T2); else { if (bestv == 2) { S[x] = z; S[z] = x; } best = bs; improved = true; }
      }
    }
  }
  return best;
}

// ---------- ranking of survivors: fill the plugboard by hill climb, score by quadgrams ----------
static void rank_hits(std::vector<Hit>& hits, const std::vector<Crib>& cribs, const std::string& qfile,
                      const std::string& outpath, int topn, int restarts, int threads) {
  NGram Q; if (!Q.load(qfile, 4, 'c')) { fprintf(stderr, "cannot load %s\n", qfile.c_str()); return; }
  fprintf(stderr, "ranking %zu hits with %s (restarts=%d)\n", hits.size(), qfile.c_str(), restarts);
  struct R { float rest, full; size_t h; std::string pt, plugs; int mm; };
  std::vector<std::vector<R>> per(threads);
  auto t0 = std::chrono::steady_clock::now();
#pragma omp parallel for schedule(dynamic, 64) num_threads(threads)
  for (size_t hi = 0; hi < hits.size(); hi++) {
    const Hit& h = hits[hi]; int tid = omp_get_thread_num();
    MKey k{h.j.refl, h.j.greek, h.j.wL, h.j.wM, h.j.wR, h.pG, h.pL, h.pM, h.pR, 0, h.rM, h.rR};
    uint8_t T[MAXLEN][26]; make_path(k, g_len, T);
    bool fixed[26]; int S0[26];
    for (int a = 0; a < 26; a++) { fixed[a] = h.raw[a] >= 0; S0[a] = fixed[a] ? h.raw[a] : a; }
    const Crib& cr0 = cribs[h.crib];
    CribScorer sc{Q.t.data(), g_len, g_ct, T, cr0.P, cr0.used.data(), (int)cr0.used.size(), 3.0f};
    uint64_t rs = 0x9E3779B97F4A7C15ull ^ (hi * 2654435761ull);
    auto rnd = [&]() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; };
    int best[26]; float bs = -1e30f; long ev = 0;
    for (int r = 0; r < restarts; r++) {
      int S[26]; memcpy(S, S0, sizeof S);
      int order[26]; for (int a = 0; a < 26; a++) order[a] = a;
      for (int a = 25; a > 0; a--) std::swap(order[a], order[rnd() % (a + 1)]);
      if (r > 0) for (int t = 0; t < 2; t++) {  // random kick among free letters
        int a = rnd() % 26, b = rnd() % 26;
        if (a != b && !fixed[a] && !fixed[b] && S[a] == a && S[b] == b && nplugs(S) < g_maxplugs) { S[a] = b; S[b] = a; }
      }
      float s = climbT(sc, S, fixed, order, g_maxplugs); ev++;
      if (s > bs) { bs = s; memcpy(best, S, sizeof best); }
    }
    uint8_t p[MAXLEN]; decrypt(T, best, g_ct, g_len, p);
    const Crib& cr = cribs[h.crib];
    std::vector<bool> incrib(g_len, false); for (int i : cr.used) incrib[i] = true;
    float rest = 0; int nr = 0;
    for (int i = 0; i + 3 < g_len; i++) {
      if (incrib[i] && incrib[i + 1] && incrib[i + 2] && incrib[i + 3]) continue;
      rest += Q.t[((p[i] * 26 + p[i + 1]) * 26 + p[i + 2]) * 26 + p[i + 3]]; nr++;
    }
    int mm = 0; for (int i : cr.used) if (p[i] != cr.P[i]) mm++;
    per[tid].push_back({nr ? rest / nr : -99.f, Q.score(p, g_len), hi, to_str(p, g_len), plug_str(best), mm});
    if (per[tid].size() > (size_t)topn * 40) {  // keep memory bounded
      std::sort(per[tid].begin(), per[tid].end(), [](const R& a, const R& b) { return a.mm != b.mm ? a.mm < b.mm : a.rest > b.rest; });
      per[tid].resize(topn * 10);
    }
  }
  std::vector<R> all; for (auto& v : per) all.insert(all.end(), v.begin(), v.end());
  std::sort(all.begin(), all.end(), [](const R& a, const R& b) { return a.mm != b.mm ? a.mm < b.mm : a.rest > b.rest; });
  FILE* f = fopen(outpath.c_str(), "w");
  fprintf(f, "#rest_quad\tfull_quad\tcrib_mismatch\tcrib\trefl\tgreek\twheels\tpos(GLMR)\trings(GLMR)\tg\tundet\tplugs\tdecrypt\n");
  std::set<std::string> seen; int n = 0;
  for (auto& r : all) {
    if (!seen.insert(r.pt).second) continue;
    const Hit& h = hits[r.h];
    fprintf(f, "%.3f\t%.3f\t%d\t%s\t%s\t%s\t%s\t%c%c%c%c\tAA%c%c\t%d\t%d\t%s\t%s\n", r.rest, r.full, r.mm, cribs[h.crib].name.c_str(), REFL_NAME[h.j.refl],
            ROTOR_NAME[h.j.greek], wheel_str(h.j).c_str(), 'A' + h.pG, 'A' + h.pL, 'A' + h.pM, 'A' + h.pR, 'A' + h.rM, 'A' + h.rR, h.g, h.undet,
            r.plugs.c_str(), r.pt.c_str());
    if (++n >= topn) break;
  }
  fclose(f);
  fprintf(stderr, "ranking done in %.1fs -> %s\n", std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count(), outpath.c_str());
}

static void usage() {
  fprintf(stderr,
    "bombe --ct FILE_OR_TEXT --crib TEXT@OFF[,OFF..] [--crib ...] [--k K] [--blocks B]\n"
    "      [--maxplugs 10] [--gmax G] [--orders all|W1W2W3,...] [--refl BC] [--greek bg]\n"
    "      [--midring pinned|all] [--span full|crib] [--threads N] [--out FILE] [--excl i,j]\n"
    "      [--key REFL,GREEK,WHEELS,POS4,RINGS4]   (test a single setting)\n"
    "      [--climb QUADGRAMFILE --rank-out F --topn N --restarts R --maxhits M]  (rank survivors)\n"
    "  Crib letter '?' = unknown/ignored. Positions are 0-based message positions.\n"
    "  k = number of unknown garbles tolerated (default 0 -> 1 block). Forced garbles\n"
    "  (crib letter == cipher letter) are excluded automatically and do not count in k.\n");
  exit(1);
}

int main(int argc, char** argv) {
  std::string ctarg, orders = "all", refls = "BC", greeks = "bg", midring = "pinned", out = "-", span = "full", keyarg;
  std::vector<std::string> cribspecs; int k = 0, nblocks = -1, threads = omp_get_max_threads();
  std::vector<int> manual_excl; bool selftest = false; std::string climbfile, rankfrom, rankout = "rank.tsv"; int topn = 50, restarts = 4; long maxhits = 20000000;
  std::vector<Hit> allhits;
  for (int a = 1; a < argc; a++) {
    std::string s = argv[a]; auto nx = [&]() { if (a + 1 >= argc) usage(); return std::string(argv[++a]); };
    if (s == "--selftest") selftest = true; else if (s == "--ct") ctarg = nx(); else if (s == "--crib") cribspecs.push_back(nx());
    else if (s == "--k") k = atoi(nx().c_str()); else if (s == "--blocks") nblocks = atoi(nx().c_str());
    else if (s == "--maxplugs") g_maxplugs = atoi(nx().c_str()); else if (s == "--gmax") g_gmax = atoi(nx().c_str());
    else if (s == "--orders") orders = nx(); else if (s == "--refl") refls = nx(); else if (s == "--greek") greeks = nx();
    else if (s == "--midring") midring = nx(); else if (s == "--threads") threads = atoi(nx().c_str());
    else if (s == "--out") out = nx(); else if (s == "--span") span = nx(); else if (s == "--key") keyarg = nx(); else if (s == "--scalar") g_scalar = true;
    else if (s == "--climb") climbfile = nx(); else if (s == "--rank-from") rankfrom = nx(); else if (s == "--rank-out") rankout = nx(); else if (s == "--topn") topn = atoi(nx().c_str());
    else if (s == "--restarts") restarts = atoi(nx().c_str()); else if (s == "--maxhits") maxhits = atol(nx().c_str());
    else if (s == "--excl") { std::string v = nx(); for (char* p = strtok(&v[0], ","); p; p = strtok(nullptr, ",")) manual_excl.push_back(atoi(p)); }
    else usage();
  }
  if (selftest) {  // lazy T vs core.h make_path on random keys with random rings
    init_rtabs(); std::vector<uint8_t> U(26 * 26 * 26 * 26); srand(12345); long bad = 0, tot = 0;
    for (int t = 0; t < 400; t++) {
      MKey key; key.refl = rand() % 2; key.greek = 8 + rand() % 2;
      do { key.wL = rand() % 8; key.wM = rand() % 8; key.wR = rand() % 8; } while (key.wL == key.wM || key.wM == key.wR || key.wL == key.wR);
      key.pG = rand() % 26; key.pL = rand() % 26; key.pM = rand() % 26; key.pR = rand() % 26; key.rL = 0; key.rM = rand() % 26; key.rR = rand() % 26;
      if (t % 4 == 0) key.pM = (TB.notch[key.wM][md(key.rM + 3)] ? 0 : 0), key.pM = md(ROTOR_NOTCH[key.wM][0] - 'A' - 1);  // force left steps
      int n = 120; static uint8_t T[MAXLEN][26]; make_path(key, n, T);
      build_U(key.refl, key.greek, key.wL, key.wM, U.data());
      Pattern P; sim_pattern(key.wR, key.wM, key.pR, key.pM, n, P);
      int cR0 = md(key.pR - key.rR), cM0 = md(key.pM - key.rM), cL0 = key.pL, cG = key.pG;
      for (int i = 0; i < n; i++) { int sh = md(cR0 + i + 1);
        const uint8_t* ur = U.data() + ((cG * 26 + md(cL0 + P.dL[i])) * 26 + md(cM0 + P.dM[i])) * 26;
        for (int c = 0; c < 26; c++) { tot++; if (RB[key.wR][sh][ur[RF[key.wR][sh][c]]] != T[i][c]) bad++; } }
    }
    printf("selftest: %ld/%ld mismatches\n", bad, tot); return bad != 0;
  }
  if (ctarg.empty() || cribspecs.empty()) usage();
  bool rankonly = !rankfrom.empty();
  if (nblocks < 0) nblocks = k + 1;
  { FILE* f = fopen(ctarg.c_str(), "r");
    if (f) { fclose(f); g_len = read_text(ctarg.c_str(), g_ct); }
    else { g_len = 0; for (char c : ctarg) if (isalpha(c)) g_ct[g_len++] = toupper(c) - 'A'; } }
  init_rtabs();
  std::vector<Crib> cribs;
  int maxend = 0;
  for (auto& spec : cribspecs) {
    auto at = spec.find('@'); std::string text = spec.substr(0, at), offs = at == std::string::npos ? "0" : spec.substr(at + 1);
    for (auto& ch : text) ch = toupper(ch);
    for (char* p = strtok(&offs[0], ","); p; p = strtok(nullptr, ",")) {
      Crib c; c.text = text; c.off = atoi(p); c.name = text.substr(0, 12) + (text.size() > 12 ? "..(" + std::to_string(text.size()) + ")" : "") + "@" + std::to_string(c.off);
      if (c.off + (int)text.size() > g_len) { fprintf(stderr, "skip %s: beyond ciphertext\n", c.name.c_str()); continue; }
      setup_crib(c, nblocks, manual_excl);
      maxend = std::max(maxend, c.off + (int)text.size());
      fprintf(stderr, "crib %zu %s used=%zu excluded=[", cribs.size(), c.name.c_str(), c.used.size());
      for (int i : c.excl) fprintf(stderr, "%d ", i);
      fprintf(stderr, "] blocks=%zu sizes:", c.blocks.size());
      for (auto& b : c.blocks) { int nl = 0; for (auto& op : b.prog) nl += op.check; fprintf(stderr, " %d(pivot %c, loops %d)", b.npos, 'A' + b.pivot, nl); }
      fprintf(stderr, "\n");
      cribs.push_back(c);
    }
  }
  g_span = span == "full" ? g_len : maxend;
  if (rankonly) {  // re-rank hits from an earlier --out TSV (same --ct and --crib list, same order)
    FILE* fi = fopen(rankfrom.c_str(), "r"); char line[1024];
    while (fgets(line, sizeof line, fi)) {
      if (line[0] == '#') continue;
      char rf[16], gk[16], wh[8], ps[8], rg[8], st[256]; Hit h; int ci;
      if (sscanf(line, "%d\t%15s\t%15s\t%7s\t%7s\t%7s\t%d\t%d\t%d\t%d\t%255[^\n]", &ci, rf, gk, wh, ps, rg, &h.g, &h.undet, &h.npairs, &h.blocks, st) != 11) continue;
      h.crib = ci; h.j = {rf[0] == 'B' ? 0 : 1, gk[0] == 'B' ? 8 : 9, wh[0] - '1', wh[1] - '1', wh[2] - '1'};
      h.pG = ps[0] - 'A'; h.pL = ps[1] - 'A'; h.pM = ps[2] - 'A'; h.pR = ps[3] - 'A'; h.rM = rg[2] - 'A'; h.rR = rg[3] - 'A';
      memset(h.raw, -1, 26); std::string S = st; auto bar = S.find('|');
      for (size_t q = 0; q + 1 < bar; q++) if (isupper(S[q]) && isupper(S[q + 1])) { h.raw[S[q] - 'A'] = S[q + 1] - 'A'; h.raw[S[q + 1] - 'A'] = S[q] - 'A'; q++; }
      for (size_t q = S.find("self:") + 5; q < S.size(); q++) if (isupper(S[q])) h.raw[S[q] - 'A'] = S[q] - 'A';
      allhits.push_back(h);
      if ((long)allhits.size() >= maxhits) break;
    }
    fclose(fi);
    rank_hits(allhits, cribs, climbfile, rankout, topn, restarts, threads);
    return 0;
  }
  bool allmid = midring == "all";

  // jobs
  std::vector<Job> jobs;
  auto wanted_order = [&](int a, int b, int c) {
    if (orders == "all") return true;
    char w[4] = {char('1' + a), char('1' + b), char('1' + c), 0};
    return orders.find(w) != std::string::npos;
  };
  for (char rc : refls) for (char gc : greeks) for (int a = 0; a < 8; a++) for (int b = 0; b < 8; b++) for (int c = 0; c < 8; c++) {
    if (a == b || b == c || a == c || !wanted_order(a, b, c)) continue;
    jobs.push_back({rc == 'B' ? 0 : 1, gc == 'b' ? 8 : 9, a, b, c});
  }
  // single-key mode
  int only[8]; bool single = false;
  if (!keyarg.empty()) {  // REFL,GREEK,WHEELS,POS4,RINGS4 e.g. B,g,438,WQYR,AACU
    char r, g; char wh[4], pos[5], rg[5];
    if (sscanf(keyarg.c_str(), "%c,%c,%3s,%4s,%4s", &r, &g, wh, pos, rg) != 5) usage();
    jobs = {{r == 'B' ? 0 : 1, (g == 'b' || g == 'B') ? 8 : 9, wh[0] - '1', wh[1] - '1', wh[2] - '1'}};
    // cores: greek core = pG - rG, left core = pL - rL
    only[0] = md(pos[0] - rg[0]); only[1] = md(pos[1] - rg[1]); only[2] = md(pos[2] - rg[2]); only[3] = md(pos[3] - rg[3]);
    only[4] = pos[3] - 'A'; only[5] = pos[2] - 'A';  // wr0, wm0
    single = true; allmid = true;
  }

  FILE* fo = out == "-" ? stdout : fopen(out.c_str(), "w");
  fprintf(fo, "#crib\trefl\tgreek\twheels\tpos(GLMR)\trings(GLMR)\tg\tundet\tnpairs\tblocks\tstecker\n");
  std::atomic<long> nsettings(0), nstops(0), nT(0);
  std::vector<long> stops_by_crib(cribs.size(), 0);
  std::vector<std::vector<long>> hist(cribs.size(), std::vector<long>(80, 0));  // g histogram of verified stops
  auto t0 = std::chrono::steady_clock::now();
  std::vector<uint8_t> Ubuf((size_t)threads * (26 * 26 * 26 * 26 + 64));
  omp_set_num_threads(threads);

  // work items: (job, wr0)
  struct Item { int job, wr0; };
  std::vector<Item> items;
  if (single) items.push_back({0, only[4]});
  else for (int j = 0; j < (int)jobs.size(); j++) {
    int nwr = jobs[j].wR >= 5 ? 13 : 26;  // double-notch right: (window+13, same core) is equivalent
    for (int w = 0; w < nwr; w++) items.push_back({j, w});
  }
  // Sort so that each thread handles all wr0 of one job consecutively (U rebuilt when job changes).
  std::vector<int> cur_job(threads, -1);
  std::mutex mtx;
  long done_items = 0;
#pragma omp parallel for schedule(dynamic, 1)
  for (int it = 0; it < (int)items.size(); it++) {
    int tid = omp_get_thread_num();
    const Job& J = jobs[items[it].job]; int wr0 = items[it].wr0;
    uint8_t* U = &Ubuf[(size_t)tid * (26 * 26 * 26 * 26 + 64)];
    if (cur_job[tid] != items[it].job) {
      // cache key: U depends on refl, greek, wL, wM only
      static thread_local int ck[4] = {-1, -1, -1, -1};
      if (ck[0] != J.refl || ck[1] != J.greek || ck[2] != J.wL || ck[3] != J.wM) {
        build_U(J.refl, J.greek, J.wL, J.wM, U); ck[0] = J.refl; ck[1] = J.greek; ck[2] = J.wL; ck[3] = J.wM;
      }
      cur_job[tid] = items[it].job;
    }
    // middle-turnover classes for this wr0
    std::vector<Pattern> pats(26); for (int w = 0; w < 26; w++) sim_pattern(J.wR, J.wM, wr0, w, g_span, pats[w]);
    std::vector<int> classrep;  // distinct patterns over middle window
    for (int w = 0; w < 26; w++) {
      bool dup = false;
      for (int c : classrep) if (!memcmp(pats[c].dM, pats[w].dM, g_span) && !memcmp(pats[c].dL, pats[w].dL, g_span)) { dup = true; break; }
      if (!dup) classrep.push_back(w);
    }
    long lset = 0, lstops = 0, lT = 0;
    std::vector<Hit> lhits;
    std::vector<long> lstopc(cribs.size(), 0);
    std::vector<std::vector<long>> lhist(cribs.size(), std::vector<long>(80, 0));
    Ctx cx;
    for (int cR0 = 0; cR0 < 26; cR0++) {
      if (single && cR0 != only[3]) continue;
      for (int i = 0; i < g_len; i++) { int sh = md(cR0 + i + 1); cx.rf[i] = RF[J.wR][sh]; cx.rb[i] = RB[J.wR][sh]; }
      for (int cM0 = 0; cM0 < 26; cM0++) {
        if (single && cM0 != only[2]) continue;
        std::vector<int> cls;
        if (allmid) cls = classrep; else cls = {cM0};  // pinned: middle ring A -> window == core
        if (single) cls = {only[5]};
        for (int wm : cls) {
          const Pattern& P = pats[wm];
          for (int cL0 = 0; cL0 < 26; cL0++) {
            if (single && cL0 != only[1]) continue;
            for (int i = 0; i < g_len; i++) cx.base[i] = (md(cL0 + P.dL[i]) * 26 + md(cM0 + P.dM[i])) * 26;
            for (int cG = 0; cG < 26; cG++) {
              if (single && cG != only[0]) continue;
              cx.Ug = U + cG * 17576;
              lset++;
              for (size_t ci = 0; ci < cribs.size(); ci++) {
                const Crib& cr = cribs[ci];
                int8_t S[26]; Verify best; best.g = 1 << 30; int nb = 0;
                for (const Block& b : cr.blocks) {
                  if (!block_test(b, cx, S, lT)) continue;
                  nb++; Verify v; verify(cr, cx, S, v);
                  if (v.g < best.g) best = v;
                }
                if (!nb) continue;
                lstops++; lstopc[ci]++; lhist[ci][std::min(best.g, 79)]++;
                if (best.g <= g_gmax && best.npairs <= g_maxplugs) {
                  Hit h; h.crib = ci; h.j = J; h.pG = cG; h.pL = cL0; h.pM = wm; h.pR = wr0;
                  h.rM = md(wm - cM0); h.rR = md(wr0 - cR0); h.g = best.g; h.undet = best.undet; h.npairs = best.npairs;
                  h.blocks = nb; h.S = sstr(best.S); memcpy(h.raw, best.S, 26); lhits.push_back(h);
                }
              }
            }
          }
        }
      }
    }
    nsettings += lset; nstops += lstops; nT += lT;
    std::lock_guard<std::mutex> lk(mtx);
    for (size_t ci = 0; ci < cribs.size(); ci++) { stops_by_crib[ci] += lstopc[ci]; for (int g = 0; g < 80; g++) hist[ci][g] += lhist[ci][g]; }
    if (!climbfile.empty() && allhits.size() < (size_t)maxhits) allhits.insert(allhits.end(), lhits.begin(), lhits.end());
    for (auto& h : lhits) {
      fprintf(fo, "%d\t%s\t%s\t%s\t%c%c%c%c\tAA%c%c\t%d\t%d\t%d\t%d\t%s\n", h.crib, REFL_NAME[h.j.refl], ROTOR_NAME[h.j.greek],
              wheel_str(h.j).c_str(), 'A' + h.pG, 'A' + h.pL, 'A' + h.pM, 'A' + h.pR, 'A' + h.rM, 'A' + h.rR, h.g, h.undet, h.npairs, h.blocks, h.S.c_str());
    }
    fflush(fo);
    done_items++;
    if (done_items % 200 == 0) {
      double el = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
      fprintf(stderr, "[%ld/%zu items] %.0fs settings=%ld (%.2fM/s) stops=%ld\n", done_items, items.size(), el, nsettings.load(), nsettings.load() / el / 1e6, nstops.load());
    }
  }
  double el = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  fprintf(stderr, "DONE settings=%ld time=%.1fs rate=%.3fM settings/s (%.3fM/s/thread, %d threads) T-evals/setting=%.1f stops=%ld\n",
          nsettings.load(), el, nsettings.load() / el / 1e6, nsettings.load() / el / 1e6 / threads, threads, (double)nT.load() / std::max(1L, nsettings.load()), nstops.load());
  for (size_t ci = 0; ci < cribs.size(); ci++) {
    fprintf(stderr, "crib %zu %s: block-stops=%ld; verified g histogram:", ci, cribs[ci].name.c_str(), stops_by_crib[ci]);
    for (int g = 0; g < 80; g++) if (hist[ci][g]) fprintf(stderr, " g%d:%ld", g, hist[ci][g]);
    fprintf(stderr, "\n");
  }
  if (fo != stdout) fclose(fo);
  if (!climbfile.empty()) rank_hits(allhits, cribs, climbfile, rankout, topn, restarts, threads);
}
