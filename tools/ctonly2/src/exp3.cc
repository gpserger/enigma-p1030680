// exp3: true-key success of climb recipes vs evaluation budget.
#include "cases.h"
#include "climb.h"
#include <omp.h>
#include <map>
#include <array>
#include <chrono>
static double now() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
int main(int argc, char** argv) {
  std::string cf = argv[1], ngdir = argv[2], recipe = argv[3]; int R = atoi(argv[4]); int trials = atoi(argv[5]); int th = atoi(argv[6]);
  int poff = argc > 7 ? atoi(argv[7]) : 0; int kk = argc > 8 ? atoi(argv[8]) : 10;
  init_rtabs();
  std::vector<Case> cs = load_cases(cf, ngdir); for (auto& c : real_cases(ngdir)) cs.push_back(c);
  std::map<std::string, Tables2> tabs; std::vector<uint8_t> U(USZ + 64);
  int tot_ok = 0, tot = 0, tot_sf = 0; double tot_f3 = 0; static long hist[27] = {0}; double tot_ev = 0, t0 = now();
  std::string perline;
  for (size_t ci = 0; ci < cs.size(); ci++) {
    const Case& c = cs[ci];
    if (!tabs.count(c.ngprefix)) { Tables2& t2 = tabs[c.ngprefix]; if (getenv("FLOOR")) t2.floor_cnt = atof(getenv("FLOOR")); if (getenv("LAMBDA")) t2.lambda = t2.lambda3 = atof(getenv("LAMBDA")); if (getenv("AW")) sscanf(getenv("AW"), "%f,%f,%f,%f", &t2.aw[0], &t2.aw[1], &t2.aw[2], &t2.aw[3]); if (getenv("W3")) sscanf(getenv("W3"), "%f,%f,%f", &t2.w3[0], &t2.w3[1], &t2.w3[2]); t2.load(c.ngprefix); }
    const Tables2& tb = tabs[c.ngprefix];
    build_U(c.J, U.data()); Setting s = c.s; s.wr0 = md(s.wr0 + poff); Pattern P; sim_pattern(c.J.wR, c.J.wM, s.wr0, s.wm0, c.n, P);
    alignas(64) static uint8_t T[MAXLEN][32]; make_T(c.J, U.data(), s, P, c.n, T);
    std::vector<int> ok(trials), sf(trials, 0); std::vector<long> ev(trials), f3e(trials, 0);
#pragma omp parallel for num_threads(th) schedule(dynamic, 1)
    for (int t = 0; t < trials; t++) {
      std::mt19937 rng(t * 1000003 + ci * 17 + 5);
      Ctx2 cx{T, c.ct, c.n, &tb};
      float best = -1e30f; int bS[26]; long f3ev = 0;
      int S[26]; for (int a = 0; a < 26; a++) S[a] = a;
      if (recipe == "sa") {  // simulated annealing on fused+IC, R = iterations (thousands)
        float T0 = atof(getenv("T0")), T1 = atof(getenv("T1")); int iters = R * 1000;
        int S[26]; for (int a = 0; a < 26; a++) S[a] = a; kick(S, 10, rng); float cur = cx.eval<M_FUSED>(S); best = cur; memcpy(bS, S, sizeof bS);
        std::uniform_real_distribution<float> U01(0, 1);
        for (int it = 0; it < iters; it++) {
          float temp = T0 * powf(T1 / T0, (float)it / iters);
          int a = rng() % 26, b = rng() % 25; if (b >= a) b++;
          int old[26]; memcpy(old, S, sizeof old);
          if (S[a] == b) { S[a] = a; S[b] = b; } else { int x = S[a], y = S[b]; S[x] = x; S[y] = y; S[a] = b; S[b] = a; if (npl(S) > 10) { memcpy(S, old, sizeof old); continue; } }
          float sc = cx.eval<M_FUSED>(S);
          if (sc >= cur || U01(rng) < expf((sc - cur) / temp)) { cur = sc; if (sc > best) { best = sc; memcpy(bS, S, sizeof bS); } } else memcpy(S, old, sizeof old);
        }
        memcpy(S, bS, sizeof S); best = climb_toggle<M_FUSED>(cx, S, 10, true); memcpy(bS, S, sizeof bS);
      } else if (recipe == "f3top") {
        int K = getenv("TOPK") ? atoi(getenv("TOPK")) : 3; std::vector<std::pair<float, std::array<int, 26>>> pool;
        for (int r = 0; r < R; r++) { int S[26]; for (int a = 0; a < 26; a++) S[a] = a; kick(S, getenv("KICK") ? atoi(getenv("KICK")) : 10, rng);
          float s3 = climb_toggle<M_FUSED3>(cx, S, 10, true); std::array<int, 26> A; memcpy(A.data(), S, sizeof S); pool.push_back({s3, A}); }
        f3ev = cx.evals;
        std::sort(pool.begin(), pool.end(), [](const auto& x, const auto& y) { return x.first > y.first; });
        for (int q = 0; q < K && q < (int)pool.size(); q++) { int S[26]; memcpy(S, pool[q].second.data(), sizeof S);
          if (q > 0 && pool[q].second == pool[q-1].second) continue;
          float sc = climb_toggle<M_FUSED>(cx, S, 10, true); if (sc > best) { best = sc; memcpy(bS, S, sizeof bS); } }
      } else
      for (int r = 0; r < R; r++) {
        if (recipe == "ils" && r > 0) { memcpy(S, bS, sizeof S); /* perturb: kk random toggles */
          for (int q = 0; q < kk; q++) { int a = rng() % 26, b = rng() % 25; if (b >= a) b++;
            if (S[a] == b) { S[a] = a; S[b] = b; } else { int x = S[a], y = S[b]; S[x] = x; S[y] = y; S[a] = b; S[b] = a; } } }
        else { for (int a = 0; a < 26; a++) S[a] = a;
          if (getenv("SEEDK")) { int K = atoi(getenv("SEEDK")); int pl[13], np = 0; for (int a = 0; a < 26; a++) if (c.S[a] > a) pl[np++] = a;
            std::shuffle(pl, pl + np, rng); for (int q = 0; q < K && q < np; q++) { S[pl[q]] = c.S[pl[q]]; S[c.S[pl[q]]] = pl[q]; } }
          kick(S, getenv("KICK") ? atoi(getenv("KICK")) : 10, rng); }
        bool fi = recipe != "m4f10s";
        int mcap = getenv("MCAP") ? atoi(getenv("MCAP")) : 4;
        const char* pre = getenv("PRE") ? getenv("PRE") : "mono";
        if (mcap > 0) { if (!strcmp(pre, "mono")) climb_toggle<M_MONO>(cx, S, mcap, fi); else if (!strcmp(pre, "ic")) climb_toggle<M_IC>(cx, S, mcap, fi);
          else if (!strcmp(pre, "bi")) climb_toggle<M_BI>(cx, S, mcap, fi); else if (!strcmp(pre, "biic")) climb_toggle<M_BIIC>(cx, S, mcap, fi);
          else if (!strcmp(pre, "tri")) climb_toggle<M_TRI>(cx, S, mcap, fi); }
        float sc; if (recipe == "f3q") { long e0 = cx.evals; climb_toggle<M_FUSED3>(cx, S, 10, fi); f3ev += cx.evals - e0; sc = climb_toggle<M_FUSED>(cx, S, 10, fi); }
        else sc = recipe == "f3" ? climb_toggle<M_FUSED3>(cx, S, 10, fi) : climb_toggle<M_FUSED>(cx, S, 10, fi);
        if (sc > best) { best = sc; memcpy(bS, S, sizeof bS); }
        { int q = 0; for (int a = 0; a < 26; a++) q += S[a] == c.S[a]; if (getenv("HIST")) { 
#pragma omp atomic
          hist[q]++; } }
      }
      uint8_t p[MAXLEN]; cx.dec(bS, p); int cr = 0; for (int i = 0; i < c.n; i++) cr += p[i] == c.ptv[i];
      int bl = 0; for (int a = 0; a < 26; a++) bl += bS[a] == c.S[a]; ok[t] = cr >= c.n - 4 || bl >= 22; f3e[t] = f3ev; ev[t] = cx.evals; { int St[26]; memcpy(St, c.S, sizeof St); float ts = recipe == "f3" ? cx.eval<M_FUSED3>(St) : cx.eval<M_FUSED>(St); if (!ok[t] && best > ts) sf[t] = 1; }
    }
    int k = 0; for (int t = 0; t < trials; t++) { k += ok[t]; tot_ev += ev[t]; tot_sf += sf[t]; tot_f3 += f3e[t]; }
    tot_ok += k; tot += trials; perline += c.id.substr(0, 10) + ":" + std::to_string(k) + " ";
  }
  double el = now() - t0;
  if (getenv("HIST")) { for (int q = 0; q <= 26; q++) printf("%d:%ld ", q, hist[q]); printf("\n"); }
  printf("f3 evals/trial %.0f\n", tot_f3 / tot); printf("scorefail(best>truth, wrong) %d\n", tot_sf); printf("%s R=%d poff=%d k=%d: success %d/%d = %.1f%%  evals/trial %.0f  ns/eval %.0f (thread, contended)\n%s\n", recipe.c_str(), R, poff, kk, tot_ok, tot, 100.0 * tot_ok / tot,
         tot_ev / tot, el * th / tot_ev * 1e9, perline.c_str());
}
