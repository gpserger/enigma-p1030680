// exp5: fast climber at true key: consistency with reference scorer, success, CPU time/eval.
#include "cases.h"
#include "fast.h"
#include <omp.h>
#include <map>
#include <array>
#include <ctime>
static double cpu() { timespec t; clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t); return t.tv_sec + 1e-9 * t.tv_nsec; }
int main(int argc, char** argv) {
  std::string cf = argv[1], ngdir = argv[2]; int R = atoi(argv[3]), trials = atoi(argv[4]), th = atoi(argv[5]);
  int poff = argc > 6 ? atoi(argv[6]) : 0; int K = getenv("KICK") ? atoi(getenv("KICK")) : 10;
  init_rtabs(); if (getenv("STALE")) g_stale = atoi(getenv("STALE")); if (getenv("NOREPAIR")) g_repair = false; if (getenv("U8")) g_u8 = true;
  std::vector<Case> cs = load_cases(cf, ngdir); for (auto& c : real_cases(ngdir)) cs.push_back(c);
  std::map<std::string, Tables2> tabs; std::map<std::string, FastLang> fl; std::vector<uint8_t> U(USZ + 64);
  int tot_ok = 0, tot = 0; double tev = 0, tcpu = 0; std::string per;
  for (size_t ci = 0; ci < cs.size(); ci++) {
    const Case& c = cs[ci];
    if (!tabs.count(c.ngprefix)) { Tables2& t2 = tabs[c.ngprefix]; t2.floor_cnt = getenv("FLOOR") ? atof(getenv("FLOOR")) : 10; t2.load(c.ngprefix); fl[c.ngprefix].build(t2, c.n); }
    const Tables2& tb = tabs[c.ngprefix]; const FastLang& FL = fl[c.ngprefix];
    build_U(c.J, U.data()); Setting s = c.s; s.wr0 = md(s.wr0 + poff); Pattern P; sim_pattern(c.J.wR, c.J.wM, s.wr0, s.wm0, c.n, P);
    alignas(64) static uint8_t T[MAXLEN][32]; make_T(c.J, U.data(), s, P, c.n, T);
    if (ci == 0) {  // consistency check vs reference
      Ctx2 cx{T, c.ct, c.n, &tb}; Fast F; F.init(c.ct, c.n, &FL); F.set_setting(T); std::mt19937 rng(1); double maxd = 0; float off = 0;
      for (int q = 0; q < 1000; q++) { int S[26]; for (int a = 0; a < 26; a++) S[a] = a; kick(S, q % 13, rng);
        Fast::St st; for (int a = 0; a < 64; a++) st.S[a] = a < 26 ? S[a] : 0; F.full(st);
        float ref = cx.eval<M_FUSED>(S) * (c.n - 3) * FL.scale; if (q == 0) off = st.score - ref; maxd = std::max(maxd, (double)fabs(st.score - ref - off));
        uint8_t p1[MAXLEN], p2[MAXLEN]; cx.dec(S, p1); F.decrypt(st.S, p2); if (memcmp(p1, p2, c.n)) { printf("DECRYPT MISMATCH\n"); return 1; } }
      printf("consistency: max |fast - ref| over 1000 boards = %.1f (score units; quantization)\n", maxd);
    }
    std::vector<int> ok(trials); std::vector<long> ev(trials); std::vector<double> cp(trials);
#pragma omp parallel for num_threads(th) schedule(dynamic, 1)
    for (int t = 0; t < trials; t++) {
      Fast* F = new Fast; F->init(c.ct, c.n, &FL); F->set_setting(T);
      uint64_t rs = 0x9E3779B97F4A7C15ull * (t + 1) + ci * 7919 + 1;
      double c0 = cpu(); int best = INT32_MIN; uint8_t bS[64];
      if (getenv("VOTE")) {  // R1 random climbs, vote plugs over top-half optima, R2 seeded climbs
        int R1 = atoi(getenv("R1")), R2 = atoi(getenv("R2")), PT_ = getenv("POOL") ? atoi(getenv("POOL")) : 6, M = getenv("M") ? atoi(getenv("M")) : 3, K2 = getenv("K2") ? atoi(getenv("K2")) : 4;
        double frac = getenv("FRAC") ? atof(getenv("FRAC")) : 0.5;
        std::vector<std::pair<int, std::array<uint8_t, 26>>> opt;
        for (int r = 0; r < R1; r++) { Fast::St st; for (int a = 0; a < 64; a++) st.S[a] = a < 26 ? a : 0; fast_kick(st.S, K, rs);
          int sc = fast_climb(*F, st, 10); if (sc > best) { best = sc; memcpy(bS, st.S, 64); } std::array<uint8_t, 26> A; memcpy(A.data(), st.S, 26); opt.push_back({sc, A}); }
        std::sort(opt.begin(), opt.end(), [](const auto& x, const auto& y) { return x.first > y.first; });
        double votes[26][26] = {{0}}; int use = std::max(1, (int)(opt.size() * frac));
        for (int k = 0; k < use; k++) for (int a = 0; a < 26; a++) if (opt[k].second[a] > a) votes[a][opt[k].second[a]] += 1;
        std::vector<std::pair<double, int>> pl; for (int a = 0; a < 26; a++) for (int b = a + 1; b < 26; b++) if (votes[a][b] > 0) pl.push_back({votes[a][b], a * 26 + b});
        std::sort(pl.rbegin(), pl.rend()); if ((int)pl.size() > PT_) pl.resize(PT_);
        for (int r = 0; r < R2; r++) {
          Fast::St st; for (int a = 0; a < 64; a++) st.S[a] = a < 26 ? a : 0;
          int idx[32]; int np_ = pl.size(); for (int q = 0; q < np_; q++) idx[q] = q;
          for (int q = 0; q < np_; q++) { int j = q + rs % (np_ - q); rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; std::swap(idx[q], idx[j]); }
          int placed = 0; for (int q = 0; q < np_ && placed < M; q++) { int a = pl[idx[q]].second / 26, b = pl[idx[q]].second % 26; if (st.S[a] == a && st.S[b] == b) { st.S[a] = b; st.S[b] = a; placed++; } }
          fast_kick(st.S, K2, rs);
          int sc = fast_climb(*F, st, 10); if (sc > best) { best = sc; memcpy(bS, st.S, 64); } }
      } else
      if (getenv("F3")) { int TK = getenv("TOPK") ? atoi(getenv("TOPK")) : 3; std::vector<std::pair<int, std::array<uint8_t, 64>>> pool;
        for (int r = 0; r < R; r++) { Fast::St st; for (int a = 0; a < 64; a++) st.S[a] = a < 26 ? a : 0; fast_kick(st.S, K, rs);
          int sc = fast_climb<3>(*F, st, 10); std::array<uint8_t, 64> A; memcpy(A.data(), st.S, 64); pool.push_back({sc, A}); }
        std::sort(pool.begin(), pool.end(), [](const auto& x, const auto& y) { return x.first > y.first; });
        for (int q = 0; q < TK && q < (int)pool.size(); q++) { if (q > 0 && pool[q].second == pool[q-1].second) continue;
          Fast::St st; memcpy(st.S, pool[q].second.data(), 64); int sc = fast_climb<4>(*F, st, 10); if (sc > best) { best = sc; memcpy(bS, st.S, 64); } }
      } else
      for (int r = 0; r < R; r++) { Fast::St st; for (int a = 0; a < 64; a++) st.S[a] = a < 26 ? a : 0; fast_kick(st.S, K, rs);
        int sc = fast_climb(*F, st, 10); if (sc > best) { best = sc; memcpy(bS, st.S, 64); } }
      cp[t] = cpu() - c0; ev[t] = F->evals;
      uint8_t p[MAXLEN]; F->decrypt(bS, p); int cr = 0; for (int i = 0; i < c.n; i++) cr += p[i] == c.ptv[i];
      int bl = 0; for (int a = 0; a < 26; a++) bl += bS[a] == c.S[a];
      ok[t] = cr >= c.n - 4 || bl >= 22; delete F;
    }
    int k = 0; for (int t = 0; t < trials; t++) { k += ok[t]; tev += ev[t]; tcpu += cp[t]; }
    tot_ok += k; tot += trials; per += c.id.substr(0, 10) + ":" + std::to_string(k) + " ";
  }
  printf("fast R=%d kick=%d poff=%d: success %d/%d = %.1f%%  evals/trial %.0f  CPU ns/eval %.1f  CPU ms/trial %.2f\n%s\n", R, K, poff, tot_ok, tot, 100.0 * tot_ok / tot,
         tev / tot, tcpu / tev * 1e9, tcpu / tot * 1e3, per.c_str());
}
