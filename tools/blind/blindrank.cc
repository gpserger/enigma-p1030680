// blindrank.cc - first-pass filter + full ranking of bombe stops (research/15, blind spot 1).
//
// The bombe (tools/bombe) writes one TSV line per verified stop: key + the partial plugboard pinned
// by the crib block (pairs + self-steckered letters). Its built-in ranker (--climb) keeps only the
// first --maxhits stops in enumeration order. This tool looks at EVERY stop:
//   1. cheap statistic per stop: decrypt the non-crib positions with the pinned plugs only; a
//      position is "determined" when both the cipher letter and the scrambler output are pinned.
//      llr1 = sum over determined positions of log10( P_lang(p | p in pinned set) / (1/|pinned set|) )
//      llr2 = same with a first-order Markov model (bigram conditional, restricted to the pinned set)
//      plus pass-through statistics (unpinned letters = identity): pt_bi, pt_quad (mean log10), pt_ic.
//   2. keep the top fraction per stratum (crib, npairs) by the chosen statistic (thresholds from a
//      histogram pass over the same file, or from a pilot thresholds file for streamed input);
//   3. climb the kept stops exactly like bombe --climb (fixed pinned letters, crib penalty W=3,
//      navalblend quadgrams, R restarts) and apply the acceptance test crib_mismatch <= k.
//
// usage:
//  blindrank stats --ct CT --cribs FILE|--crib T@O.. [--excl i,j] --in TSV --lang PREFIX
//                  [--truth-plugs "AB CD.." --truth-pt TEXT] [--climb-true R] [--quad Q] > stats.tsv
//  blindrank thr   --ct .. --cribs .. --in TSV --lang PREFIX --stat llr2 --keep F --thr-out THR
//  blindrank rank  --ct .. --cribs .. --in TSV|- --lang PREFIX --quad Q --stat llr2 (--keep F | --thr THR)
//                  --restarts R --k K --out RANK.tsv --surv SURV.tsv [--topn 200] [--threads 2]
#include "../run_m4/src/core.h"
#include "../run_m4/src/hc.h"
int g_climb_order = 4;
#include <omp.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <chrono>

static int g_len; static uint8_t g_ct[MAXLEN];
static int g_maxplugs = 10;
struct Crib { std::string text, name; int off; std::vector<int> used; bool inused[MAXLEN]; uint8_t P[MAXLEN]; };
static std::vector<Crib> g_cribs;

static void add_crib(const std::string& spec, const std::vector<int>& excl) {
  auto at = spec.find('@'); std::string text = spec.substr(0, at), offs = at == std::string::npos ? "0" : spec.substr(at + 1);
  for (auto& ch : text) ch = toupper(ch);
  std::string o = offs;
  for (char* p = strtok(&o[0], ","); p; p = strtok(nullptr, ",")) {
    Crib c; c.text = text; c.off = atoi(p); c.name = text + "@" + std::to_string(c.off);
    if (c.off + (int)text.size() > g_len) continue;          // bombe skips these too
    memset(c.inused, 0, sizeof c.inused); memset(c.P, 0, sizeof c.P);
    for (int j = 0; j < (int)text.size(); j++) {
      int i = c.off + j; if (text[j] == '?') continue; c.P[i] = text[j] - 'A';
      if (c.P[i] == g_ct[i]) continue;
      if (std::find(excl.begin(), excl.end(), i) != excl.end()) continue;
      c.used.push_back(i); c.inused[i] = true;
    }
    g_cribs.push_back(c);
  }
}

// ---------- language ----------
struct Lang {
  double P1[26]; double P2[26][26];   // probabilities
  double l1[26]; double l2[26][26];   // log10
  NGram Q; bool haveQ = false; NGram B2; 
  void load(const std::string& prefix) {
    auto rd = [&](const std::string& f, int n, std::vector<double>& v) {
      v.assign(n == 1 ? 26 : 676, 0.5); FILE* fp = fopen(f.c_str(), "r"); if (!fp) { perror(f.c_str()); exit(1); }
      char g[64]; double c; while (fscanf(fp, "%63s %lf", g, &c) == 2) { if ((int)strlen(g) != n) continue; int idx = 0; bool ok = true;
        for (int i = 0; i < n; i++) { int ch = toupper(g[i]); if (ch < 'A' || ch > 'Z') ok = false; idx = idx * 26 + ch - 'A'; } if (ok) v[idx] += c; }
      fclose(fp); };
    std::vector<double> m, b; rd(prefix + "_monograms.txt", 1, m); rd(prefix + "_bigrams.txt", 2, b);
    double tm = 0, tb = 0; for (double x : m) tm += x; for (double x : b) tb += x;
    for (int a = 0; a < 26; a++) { P1[a] = m[a] / tm; l1[a] = log10(P1[a]); }
    for (int a = 0; a < 26; a++) for (int c = 0; c < 26; c++) { P2[a][c] = b[a * 26 + c] / tb; l2[a][c] = log10(P2[a][c]); }
    B2.load(prefix + "_bigrams.txt", 2, 'c');
  }
};
static Lang g_L;

// ---------- one stop ----------
struct Stop { int crib; MKey k; int8_t S[26]; int g, undet, npairs; std::string raw; };
static bool parse_line(const char* line, Stop& h) {
  if (line[0] == '#' || !isdigit(line[0])) return false;
  char rf[16], gk[16], wh[8], ps[8], rg[8], st[256]; int ci, blocks;
  if (sscanf(line, "%d\t%15s\t%15s\t%7s\t%7s\t%7s\t%d\t%d\t%d\t%d\t%255[^\n]", &ci, rf, gk, wh, ps, rg, &h.g, &h.undet, &h.npairs, &blocks, st) != 11) return false;
  h.crib = ci;
  h.k = MKey{rf[0] == 'B' ? 0 : 1, gk[0] == 'B' ? 8 : 9, wh[0] - '1', wh[1] - '1', wh[2] - '1', ps[0] - 'A', ps[1] - 'A', ps[2] - 'A', ps[3] - 'A', 0, rg[2] - 'A', rg[3] - 'A'};
  memset(h.S, -1, 26); std::string S = st; auto bar = S.find('|'); if (bar == std::string::npos) return false;
  for (size_t q = 0; q + 1 < bar; q++) if (isupper(S[q]) && isupper(S[q + 1])) { h.S[S[q] - 'A'] = S[q + 1] - 'A'; h.S[S[q + 1] - 'A'] = S[q] - 'A'; q++; }
  for (size_t q = S.find("self:") + 5; q < S.size(); q++) if (isupper(S[q])) h.S[S[q] - 'A'] = S[q] - 'A';
  return true;
}

// single-letter scrambler output at position i (same math as core.h make_path), for the cheap statistic
struct Lazy { MKey k; Traj t; void init(const MKey& kk, int n) { k = kk; make_traj(k, n, t); }
  inline int out(int i, int c) const {
    int shR = t.R[i]-k.rR, shM = t.M[i]-k.rM, shL = t.L[i]-k.rL, shG = k.pG;
    int x = rot_f(k.wR, shR, c); x = rot_f(k.wM, shM, x); x = rot_f(k.wL, shL, x);
    if (k.greek >= 0) x = rot_f(k.greek, shG, x);
    x = TB.refl[k.refl][x];
    if (k.greek >= 0) x = rot_b(k.greek, shG, x);
    x = rot_b(k.wL, shL, x); x = rot_b(k.wM, shM, x); return rot_b(k.wR, shR, x); } };
enum { ST_LLR1, ST_LLR2, ST_PTBI, ST_PTQ, ST_PTIC, ST_N };
static const char* ST_NAME[ST_N] = {"llr1", "llr2", "pt_bi", "pt_quad", "pt_ic"};
struct Stats { float v[ST_N]; int ndet, nadj, npin; };
struct TabT { const uint8_t (*T)[26]; inline int operator()(int i, int c) const { return T[i][c]; } };
struct LazyT { const Lazy* L; inline int operator()(int i, int c) const { return L->out(i, c); } };

template <class TT> static void compute_stats(const Stop& h, const TT& T, Stats& o) {
  const Crib& cr = g_cribs[h.crib];
  bool pin[26]; int m = 0; for (int a = 0; a < 26; a++) { pin[a] = h.S[a] >= 0; m += pin[a]; }
  double Z1 = 0; for (int a = 0; a < 26; a++) if (pin[a]) Z1 += g_L.P1[a];
  double Z2[26]; for (int a = 0; a < 26; a++) { Z2[a] = 0; if (pin[a]) for (int c = 0; c < 26; c++) if (pin[c]) Z2[a] += g_L.P2[a][c]; }
  double lu = log10(1.0 / std::max(m, 1));
  double llr1 = 0, llr2 = 0; int nd = 0, nadj = 0; int prev = -1;
  uint8_t pt[MAXLEN]; int cnt[26] = {0}; int nrest = 0;
  for (int i = 0; i < g_len; i++) {
    int c = g_ct[i]; int sc = h.S[c] >= 0 ? h.S[c] : c; int x = T(i, sc); int p = h.S[x] >= 0 ? h.S[x] : x; pt[i] = p;
    bool det = h.S[c] >= 0 && h.S[x] >= 0;
    if (cr.inused[i]) { prev = -1; continue; }
    cnt[p]++; nrest++;
    if (!det) { prev = -1; continue; }
    nd++;
    double u = log10(g_L.P1[p] / Z1) - lu; llr1 += u;
    if (prev >= 0 && Z2[prev] > 0) { llr2 += log10(g_L.P2[prev][p] / Z2[prev]) - lu; nadj++; } else llr2 += u;
    prev = p;
  }
  // pass-through statistics over rest windows (windows not fully inside the crib)
  double sb = 0, sq = 0; int nb = 0, nq = 0;
  for (int i = 0; i + 1 < g_len; i++) { if (cr.inused[i] && cr.inused[i + 1]) continue; sb += g_L.l2[pt[i]][pt[i + 1]] - log10(g_L.P1[pt[i]]) ; nb++; }
  if (g_L.haveQ) for (int i = 0; i + 3 < g_len; i++) { if (cr.inused[i] && cr.inused[i + 1] && cr.inused[i + 2] && cr.inused[i + 3]) continue;
    sq += g_L.Q.t[((pt[i] * 26 + pt[i + 1]) * 26 + pt[i + 2]) * 26 + pt[i + 3]]; nq++; }
  double ic = 0; for (int a = 0; a < 26; a++) ic += cnt[a] * (cnt[a] - 1.0); ic = nrest > 1 ? ic / (nrest * (nrest - 1.0)) * 26 : 0;
  o.v[ST_LLR1] = llr1; o.v[ST_LLR2] = llr2; o.v[ST_PTBI] = nb ? sb / nb : 0; o.v[ST_PTQ] = nq ? sq / nq : -9; o.v[ST_PTIC] = ic;
  o.ndet = nd; o.nadj = nadj; o.npin = m;
}

// ---------- climb (identical objective to bombe rank_hits) ----------
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
struct ClimbRes { float rest, full; int mm; std::string pt, plugs; };
static void climb_stop(const Stop& h, const uint8_t (*T)[26], const NGram& Q, int restarts, uint64_t seed, ClimbRes& out) {
  bool fixed[26]; int S0[26];
  for (int a = 0; a < 26; a++) { fixed[a] = h.S[a] >= 0; S0[a] = fixed[a] ? h.S[a] : a; }
  const Crib& cr = g_cribs[h.crib];
  CribScorer sc{Q.t.data(), g_len, g_ct, T, cr.P, cr.used.data(), (int)cr.used.size(), 3.0f};
  uint64_t rs = 0x9E3779B97F4A7C15ull ^ (seed * 2654435761ull); if (!rs) rs = 1;
  auto rnd = [&]() { rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; return rs; };
  int best[26]; float bs = -1e30f;
  for (int r = 0; r < restarts; r++) {
    int S[26]; memcpy(S, S0, sizeof S);
    int order[26]; for (int a = 0; a < 26; a++) order[a] = a;
    for (int a = 25; a > 0; a--) std::swap(order[a], order[rnd() % (a + 1)]);
    if (r > 0) for (int t = 0; t < 2; t++) {
      int a = rnd() % 26, b = rnd() % 26;
      if (a != b && !fixed[a] && !fixed[b] && S[a] == a && S[b] == b && nplugs(S) < g_maxplugs) { S[a] = b; S[b] = a; }
    }
    float s = climbT(sc, S, fixed, order, g_maxplugs);
    if (s > bs) { bs = s; memcpy(best, S, sizeof best); }
  }
  uint8_t p[MAXLEN]; decrypt(T, best, g_ct, g_len, p);
  float rest = 0; int nr = 0;
  for (int i = 0; i + 3 < g_len; i++) {
    if (cr.inused[i] && cr.inused[i + 1] && cr.inused[i + 2] && cr.inused[i + 3]) continue;
    rest += Q.t[((p[i] * 26 + p[i + 1]) * 26 + p[i + 2]) * 26 + p[i + 3]]; nr++;
  }
  int mm = 0; for (int i : cr.used) if (p[i] != cr.P[i]) mm++;
  out.rest = nr ? rest / nr : -99.f; out.full = Q.score(p, g_len); out.mm = mm; out.pt = to_str(p, g_len); out.plugs = plug_str(best);
}

// ---------- strata / thresholds ----------
static const int NBIN = 4000; static const double BLO = -100, BHI = 100;   // histogram over the stat
static int binof(double v) { int b = (int)((v - BLO) / (BHI - BLO) * NBIN); return std::max(0, std::min(NBIN - 1, b)); }
static double binlo(int b) { return BLO + (BHI - BLO) * b / NBIN; }
static bool g_strat_np = false;   // default: one stratum per crib (calibration: llr2 flat beats per-npairs)
static int stratum(const Stop& h) { return h.crib * 16 + (g_strat_np ? std::min(h.npairs, 15) : 0); }

static std::vector<std::string> read_batch(FILE* f, size_t n) {
  std::vector<std::string> v; v.reserve(n); static char line[1024];
  while (v.size() < n && fgets(line, sizeof line, f)) { if (line[0] == '#') continue; v.emplace_back(line); }
  return v;
}

int main(int argc, char** argv) {
  if (argc < 2) { fprintf(stderr, "see header of blindrank.cc\n"); return 1; }
  std::string mode = argv[1], ctarg, cribsf, in, lang, quad, truthplugs, truthpt, out = "-", surv, thrin, throut, statname = "llr2";
  std::vector<std::string> cribspecs; std::vector<int> excl; int climbtrue = 0, restarts = 4, k = 0, topn = 200, threads = 2, stat = ST_LLR2; double keep = 1.0;
  long maxlines = -1; int survmm = -1;
  for (int a = 2; a < argc; a++) {
    std::string s = argv[a]; auto nx = [&]() { return std::string(argv[++a]); };
    if (s == "--ct") ctarg = nx(); else if (s == "--cribs") cribsf = nx(); else if (s == "--crib") cribspecs.push_back(nx());
    else if (s == "--excl") { std::string v = nx(); for (char* p = strtok(&v[0], ","); p; p = strtok(nullptr, ",")) excl.push_back(atoi(p)); }
    else if (s == "--in") in = nx(); else if (s == "--lang") lang = nx(); else if (s == "--quad") quad = nx();
    else if (s == "--truth-plugs") truthplugs = nx(); else if (s == "--truth-pt") truthpt = nx(); else if (s == "--climb-true") climbtrue = atoi(nx().c_str());
    else if (s == "--out") out = nx(); else if (s == "--surv") surv = nx(); else if (s == "--thr") thrin = nx(); else if (s == "--thr-out") throut = nx();
    else if (s == "--stat") statname = nx(); else if (s == "--keep") keep = atof(nx().c_str()); else if (s == "--restarts") restarts = atoi(nx().c_str());
    else if (s == "--k") k = atoi(nx().c_str()); else if (s == "--topn") topn = atoi(nx().c_str()); else if (s == "--threads") threads = atoi(nx().c_str());
    else if (s == "--strat") g_strat_np = nx() == "npairs"; else if (s == "--maxlines") maxlines = atol(nx().c_str()); else if (s == "--surv-mm") survmm = atoi(nx().c_str());
    else { fprintf(stderr, "unknown arg %s\n", s.c_str()); return 1; }
  }
  if (survmm < 0) survmm = k + 1;
  for (int i = 0; i < ST_N; i++) if (statname == ST_NAME[i]) stat = i;
  { FILE* f = fopen(ctarg.c_str(), "r"); if (f) { fclose(f); g_len = read_text(ctarg.c_str(), g_ct); }
    else { g_len = 0; for (char c : ctarg) if (isalpha(c)) g_ct[g_len++] = toupper(c) - 'A'; } }
  if (!cribsf.empty()) { FILE* f = fopen(cribsf.c_str(), "r"); char l[1024]; while (fgets(l, sizeof l, f)) { std::string s = l; while (!s.empty() && isspace(s.back())) s.pop_back();
      if (s.empty() || s[0] == '#') continue; cribspecs.push_back(s); } fclose(f); }
  for (auto& s : cribspecs) add_crib(s, excl);
  g_L.load(lang);
  NGram Q; if (!quad.empty()) { if (!Q.load(quad, 4, 'c')) { fprintf(stderr, "cannot load %s\n", quad.c_str()); return 1; } g_L.Q = Q; g_L.haveQ = true; }
  omp_set_num_threads(threads);
  FILE* fi = in == "-" ? stdin : fopen(in.c_str(), "r"); if (!fi) { perror(in.c_str()); return 1; }
  auto t0 = std::chrono::steady_clock::now();
  auto el = [&]() { return std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count(); };

  if (mode == "stats") {   // per-stop statistics (calibration); optional truth flag + climb of true stops
    int TS[26]; bool havetruth = !truthplugs.empty(); uint8_t TP[MAXLEN] = {0};
    if (havetruth) { parse_plugs(truthplugs.c_str(), TS); for (int i = 0; i < g_len && i < (int)truthpt.size(); i++) TP[i] = truthpt[i] - 'A'; }
    FILE* fo = out == "-" ? stdout : fopen(out.c_str(), "w");
    fprintf(fo, "#crib\tnpairs\tundet\tnpin\tndet\tnadj\tllr1\tllr2\tpt_bi\tpt_quad\tpt_ic\ttruematch\tclimb_mm\tclimb_rest\tclimb_match\tkey\n");
    long nl = 0;
    for (;;) {
      auto batch = read_batch(fi, 65536); if (batch.empty()) break;
      std::vector<std::string> outl(batch.size());
#pragma omp parallel for schedule(dynamic, 256)
      for (size_t q = 0; q < batch.size(); q++) {
        Stop h; if (!parse_line(batch[q].c_str(), h) || h.crib >= (int)g_cribs.size()) continue;
        uint8_t T[MAXLEN][26]; make_path(h.k, g_len, T); Stats st; compute_stats(h, TabT{T}, st);
        int tm = -1, cmm = -1, cmatch = -1; float crest = 0;
        if (havetruth) { uint8_t p[MAXLEN]; decrypt(T, TS, g_ct, g_len, p); tm = 0; for (int i = 0; i < g_len; i++) tm += p[i] == TP[i];
          if (climbtrue && tm >= g_len - 12) { ClimbRes cr; climb_stop(h, T, Q, climbtrue, q + 7, cr); cmm = cr.mm; crest = cr.rest; cmatch = 0; for (int i = 0; i < g_len; i++) cmatch += cr.pt[i] - 'A' == TP[i]; } }
        char b[512]; snprintf(b, sizeof b, "%d\t%d\t%d\t%d\t%d\t%d\t%.3f\t%.3f\t%.4f\t%.4f\t%.4f\t%d\t%d\t%.3f\t%d\t%s%s%d%d%d%c%c%c%c%c%c\n", h.crib, h.npairs, h.undet, st.npin, st.ndet, st.nadj,
                 st.v[0], st.v[1], st.v[2], st.v[3], st.v[4], tm, cmm, crest, cmatch, h.k.refl ? "c" : "b", h.k.greek == 8 ? "B" : "G", h.k.wL + 1, h.k.wM + 1, h.k.wR + 1,
                 'A' + h.k.pG, 'A' + h.k.pL, 'A' + h.k.pM, 'A' + h.k.pR, 'A' + h.k.rM, 'A' + h.k.rR);
        outl[q] = b;
      }
      for (auto& s : outl) fputs(s.c_str(), fo);
      nl += batch.size(); if (maxlines > 0 && nl >= maxlines) break;
    }
    if (fo != stdout) fclose(fo);
    fprintf(stderr, "stats: %ld lines in %.1fs\n", nl, el());
    return 0;
  }

  // thresholds: histogram pass (mode thr, or rank with --keep on a file)
  std::map<int, std::vector<long>> hist; std::map<int, double> thr;
  auto hist_pass = [&](FILE* f) {
    long nl = 0;
    for (;;) {
      auto batch = read_batch(f, 1 << 18); if (batch.empty()) break;
      std::vector<int> sb(batch.size(), -1), bb(batch.size(), 0);
#pragma omp parallel for schedule(dynamic, 1024)
      for (size_t q = 0; q < batch.size(); q++) {
        Stop h; if (!parse_line(batch[q].c_str(), h) || h.crib >= (int)g_cribs.size()) continue;
        Lazy Lz; Lz.init(h.k, g_len); Stats st; compute_stats(h, LazyT{&Lz}, st);
        sb[q] = stratum(h); bb[q] = binof(st.v[stat]);
      }
      for (size_t q = 0; q < batch.size(); q++) if (sb[q] >= 0) { auto& v = hist[sb[q]]; if (v.empty()) v.assign(NBIN, 0); v[bb[q]]++; }
      nl += batch.size(); if (nl % (1 << 24) < (1 << 18)) fprintf(stderr, "[hist] %ld lines %.0fs\n", nl, el());
      if (maxlines > 0 && nl >= maxlines) break;
    }
    for (auto& [s, v] : hist) {   // threshold = lower edge of the bin where the top 'keep' fraction starts
      long tot = 0; for (long c : v) tot += c; long want = (long)ceil(keep * tot), acc = 0; int b = NBIN - 1;
      for (; b > 0; b--) { acc += v[b]; if (acc >= want) break; }
      thr[s] = keep >= 1.0 ? -1e30 : binlo(b);
    }
    fprintf(stderr, "[hist] done: %ld lines, %zu strata, %.0fs\n", nl, hist.size(), el());
  };
  if (mode == "thr") {
    hist_pass(fi);
    FILE* fo = fopen(throut.c_str(), "w"); fprintf(fo, "#stratum(crib*16+npairs)\tthreshold\tcount\tstat=%s keep=%.4f\n", ST_NAME[stat], keep);
    for (auto& [s, v] : hist) { long tot = 0; for (long c : v) tot += c; fprintf(fo, "%d\t%.4f\t%ld\n", s, thr[s], tot); }
    fclose(fo); return 0;
  }
  if (mode == "rank") {
    if (!thrin.empty()) { FILE* f = fopen(thrin.c_str(), "r"); char l[256]; while (fgets(l, sizeof l, f)) { if (l[0] == '#') continue; int s; double t; if (sscanf(l, "%d %lf", &s, &t) == 2) thr[s] = t; } fclose(f); }
    else if (keep < 1.0) { if (fi == stdin) { fprintf(stderr, "--keep needs a file (two passes) or use --thr\n"); return 1; } hist_pass(fi); rewind(fi); }
    FILE* fs = surv.empty() ? nullptr : fopen(surv.c_str(), "w");
    if (fs) fprintf(fs, "#rest_quad\tfull_quad\tcrib_mismatch\tcrib\tstat\tkey\tnpairs\tplugs\tdecrypt\n");
    struct R { float rest, full; int mm; float st; std::string line; };
    std::vector<R> top; long nl = 0, nkept = 0, nsurv = 0; std::vector<long> mmhist(80, 0); std::map<int, long> seen, kept;
    for (;;) {
      auto batch = read_batch(fi, 1 << 16); if (batch.empty()) break;
      std::vector<R> res(batch.size()); std::vector<char> did(batch.size(), 0); std::vector<int> sbs(batch.size(), -1);
#pragma omp parallel for schedule(dynamic, 16)
      for (size_t q = 0; q < batch.size(); q++) {
        Stop h; if (!parse_line(batch[q].c_str(), h) || h.crib >= (int)g_cribs.size()) continue;
        int s = stratum(h); sbs[q] = s; float sv = 0;
        if (!thr.empty() || keep < 1.0) { Lazy Lz; Lz.init(h.k, g_len); Stats st; compute_stats(h, LazyT{&Lz}, st); sv = st.v[stat];
          auto it = thr.find(s); double t = it == thr.end() ? -1e30 : it->second;   // unseen stratum: climb (conservative)
          if (binof(sv) < binof(t)) continue; }
        uint8_t T[MAXLEN][26]; make_path(h.k, g_len, T);
        ClimbRes cr; climb_stop(h, T, Q, restarts, nl + q + 1, cr);
        char kb[64]; snprintf(kb, sizeof kb, "%s\t%s\t%d%d%d\t%c%c%c%c\tAA%c%c", REFL_NAME[h.k.refl], ROTOR_NAME[h.k.greek], h.k.wL + 1, h.k.wM + 1, h.k.wR + 1,
                              'A' + h.k.pG, 'A' + h.k.pL, 'A' + h.k.pM, 'A' + h.k.pR, 'A' + h.k.rM, 'A' + h.k.rR);
        char b[600]; snprintf(b, sizeof b, "%.3f\t%.3f\t%d\t%s\t%.2f\t%s\t%d\t%s\t%s\n", cr.rest, cr.full, cr.mm, g_cribs[h.crib].name.c_str(), sv, kb, h.npairs, cr.plugs.c_str(), cr.pt.c_str());
        res[q] = {cr.rest, cr.full, cr.mm, sv, b}; did[q] = 1;
      }
      for (size_t q = 0; q < batch.size(); q++) {
        if (sbs[q] >= 0) seen[sbs[q] / 16]++;
        if (!did[q]) continue; nkept++; kept[sbs[q] / 16]++; R& r = res[q]; mmhist[std::min(r.mm, 79)]++;
        if (fs && r.mm <= survmm) { fputs(r.line.c_str(), fs); nsurv++; }
        top.push_back(std::move(r));
        if (top.size() > (size_t)topn * 20) { std::sort(top.begin(), top.end(), [](const R& a, const R& b) { return a.mm != b.mm ? a.mm < b.mm : a.rest > b.rest; }); top.resize(topn); }
      }
      nl += batch.size();
      if (nl % (1 << 20) < (1 << 16)) { fprintf(stderr, "[rank] %ld lines, %ld climbed, %ld surv(mm<=%d) %.0fs\n", nl, nkept, nsurv, survmm, el()); if (fs) fflush(fs); }
      if (maxlines > 0 && nl >= maxlines) break;
    }
    std::sort(top.begin(), top.end(), [](const R& a, const R& b) { return a.mm != b.mm ? a.mm < b.mm : a.rest > b.rest; });
    FILE* fo = out == "-" ? stdout : fopen(out.c_str(), "w");
    fprintf(fo, "#rest_quad\tfull_quad\tcrib_mismatch\tcrib\tstat(%s)\trefl\tgreek\twheels\tpos(GLMR)\trings(GLMR)\tnpairs\tplugs\tdecrypt\n", ST_NAME[stat]);
    std::set<std::string> dd; int nw = 0; for (auto& r : top) { std::string pt = r.line.substr(r.line.rfind('\t') + 1); if (!dd.insert(pt).second) continue; fputs(r.line.c_str(), fo); if (++nw >= topn) break; }
    if (fo != stdout) fclose(fo); if (fs) fclose(fs);
    fprintf(stderr, "DONE lines=%ld climbed=%ld (%.2f%%) surv(mm<=%d)=%ld time=%.0fs restarts=%d stat=%s\n", nl, nkept, 100.0 * nkept / std::max(1L, nl), survmm, nsurv, el(), restarts, ST_NAME[stat]);
    fprintf(stderr, "per crib seen/climbed:"); for (auto& [c, n] : seen) fprintf(stderr, " %d:%ld/%ld", c, n, kept[c]); fprintf(stderr, "\n");
    fprintf(stderr, "post-climb crib mismatch histogram:"); for (int i = 0; i < 80; i++) if (mmhist[i]) fprintf(stderr, " mm%d:%ld", i, mmhist[i]); fprintf(stderr, "\n");
    return 0;
  }
  fprintf(stderr, "unknown mode\n"); return 1;
}
