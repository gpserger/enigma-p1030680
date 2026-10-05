// exp2: plugboard solver success at the TRUE setting (and phase-offset settings) vs budget.
#include "cases.h"
#include "score.h"
#include <omp.h>
#include <random>
#include <map>
#include <chrono>
static double now() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
struct Out { int correct; long evals; float score; };
// simulated annealing on the plugboard
static Out sa(const Eval& ev, const Case& c, int iters, float T0, float T1, int maxplugs, std::mt19937& rng, const uint8_t (*T)[32]) {
  int S[26]; for (int a = 0; a < 26; a++) S[a] = a;
  float cur = ev(S); float best = cur; int bS[26]; memcpy(bS, S, sizeof bS);
  std::uniform_real_distribution<float> U01(0, 1);
  for (int it = 0; it < iters; it++) {
    float temp = T0 * powf(T1 / T0, (float)it / iters);
    int a = rng() % 26, b = rng() % 25; if (b >= a) b++;
    int old[26]; memcpy(old, S, sizeof old);
    int x = S[a], z = S[b];
    if (x == b) { S[a] = a; S[b] = b; }
    else {
      if (x != a) S[x] = x; if (z != b) S[z] = z; S[a] = b; S[b] = a;
      if (x != a && z != b && (rng() & 1)) { S[x] = z; S[z] = x; }
      if (nplugs(S) > maxplugs) { memcpy(S, old, sizeof old); continue; }
    }
    float s = ev(S);
    if (s >= cur || U01(rng) < expf((s - cur) / temp)) { cur = s; if (s > best) { best = s; memcpy(bS, S, sizeof bS); } }
    else memcpy(S, old, sizeof old);
  }
  uint8_t p[MAXLEN]; decrypt32(T, bS, c.ct, c.n, p); int cr = 0; for (int i = 0; i < c.n; i++) cr += p[i] == c.ptv[i];
  return {cr, ev.cnt, best};
}
// hill climb with random restarts (random initial board with k plugs)
static Out hcr(const Eval& ev, const Case& c, int restarts, int k0, int maxplugs, std::mt19937& rng, const uint8_t (*T)[32], const Eval* ev2) {
  float best = -1e30f; int bS[26];
  for (int r = 0; r < restarts; r++) {
    int S[26]; for (int a = 0; a < 26; a++) S[a] = a;
    int L[26]; for (int a = 0; a < 26; a++) L[a] = a; std::shuffle(L, L + 26, rng);
    for (int q = 0; q < k0; q++) { S[L[2*q]] = L[2*q+1]; S[L[2*q+1]] = L[2*q]; }
    int order[26]; for (int a = 0; a < 26; a++) order[a] = a; std::shuffle(order, order + 26, rng);
    bool fx[26] = {0};
    float s = climb(ev, S, fx, order, maxplugs);
    if (ev2) s = climb(*ev2, S, fx, order, maxplugs);
    if (s > best) { best = s; memcpy(bS, S, sizeof bS); }
  }
  uint8_t p[MAXLEN]; decrypt32(T, bS, c.ct, c.n, p); int cr = 0; for (int i = 0; i < c.n; i++) cr += p[i] == c.ptv[i];
  return {cr, ev.cnt + (ev2 ? ev2->cnt : 0), best};
}
int main(int argc, char** argv) {
  if (argc < 5) { fprintf(stderr, "exp2 CASES|real NGDIR METHOD PARAMS(comma) [trials] [threads] [phaseoff]\n"); return 1; }
  std::string cf = argv[1], ngdir = argv[2], m = argv[3], par = argv[4]; int trials = argc > 5 ? atoi(argv[5]) : 10, th = argc > 6 ? atoi(argv[6]) : 6;
  int poff = argc > 7 ? atoi(argv[7]) : 0;
  std::vector<float> pv; { std::string t = par; for (char* q = strtok(&t[0], ","); q; q = strtok(nullptr, ",")) pv.push_back(atof(q)); }
  init_rtabs();
  std::vector<Case> cs = cf == "real" ? real_cases(ngdir) : load_cases(cf, ngdir);
  if (cf != "real") for (auto& c : real_cases(ngdir)) cs.push_back(c);
  std::map<std::string, Lang> langs; std::vector<uint8_t> U(USZ + 64);
  int tot_ok = 0, tot = 0; double tot_ev = 0, t0 = now();
  for (size_t ci = 0; ci < cs.size(); ci++) {
    const Case& c = cs[ci];
    if (!langs.count(c.ngprefix)) { Lang L; L.load(c.ngprefix); langs[c.ngprefix] = L; }
    const Lang& L = langs[c.ngprefix];
    build_U(c.J, U.data()); Setting s = c.s; s.wr0 = md(s.wr0 + poff); Pattern P; sim_pattern(c.J.wR, c.J.wM, s.wr0, s.wm0, c.n, P);
    alignas(64) static uint8_t T[MAXLEN][32]; make_T(c.J, U.data(), s, P, c.n, T);
    // truth score in quad
    uint8_t p[MAXLEN]; decrypt32(T, c.S, c.ct, c.n, p); float tq = sc_quad(L.quad.data(), p, c.n);
    std::vector<Out> outs(trials);
#pragma omp parallel for num_threads(th) schedule(dynamic, 1)
    for (int t = 0; t < trials; t++) {
      std::mt19937 rng(t * 1000003 + ci);
      Eval e3{T, c.ct, c.n, &L, 3}, e4{T, c.ct, c.n, &L, 4}, e2{T, c.ct, c.n, &L, 2};
      if (m == "sa4") outs[t] = sa(e4, c, (int)pv[0], pv[1], pv[2], 10, rng, T);
      else if (m == "sa3") outs[t] = sa(e3, c, (int)pv[0], pv[1], pv[2], 10, rng, T);
      else if (m == "hc3") outs[t] = hcr(e3, c, (int)pv[0], (int)pv[1], 10, rng, T, nullptr);
      else if (m == "hc4") outs[t] = hcr(e4, c, (int)pv[0], (int)pv[1], 10, rng, T, nullptr);
      else if (m == "hc34") outs[t] = hcr(e3, c, (int)pv[0], (int)pv[1], 10, rng, T, &e4);
      else if (m == "hc24") outs[t] = hcr(e2, c, (int)pv[0], (int)pv[1], 10, rng, T, &e4);
    }
    int ok = 0; double ev = 0; for (auto& o : outs) { ok += o.correct >= c.n - 4; ev += o.evals; }
    tot_ok += ok; tot += trials; tot_ev += ev;
    printf("%-12s ok %2d/%d  evals/trial %8.0f  truthq %.1f  best-found %s\n", c.id.c_str(), ok, trials, ev / trials, tq,
           [&]{ std::string s; for (auto& o : outs) s += std::to_string(o.correct) + ","; return s; }().c_str());
    fflush(stdout);
  }
  double el = now() - t0;
  printf("TOTAL %s %s: success %d/%d = %.1f%%  evals/trial %.0f  us/trial %.0f (thread)\n", m.c_str(), par.c_str(), tot_ok, tot, 100.0 * tot_ok / tot, tot_ev / tot, el * th / tot * 1e6);
}
