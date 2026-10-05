// exp1: stage-1 statistic separation. For each case, compute a method's rank statistics at the true
// setting (+ phase-offset neighbours) and at N random settings of the same ciphertext.
#include "cases.h"
#include "score.h"
#include <omp.h>
#include <random>
#include <map>
#include <chrono>
static double now() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
struct Res { float stat[3]; int correct; };  // stat: climb score, tri rescore, quad rescore
static const int E_ = 4;
static Res run_method(const std::string& m, const uint8_t (*T)[32], const Case& c, const Lang& L, std::mt19937& rng) {
  int S[26]; for (int a = 0; a < 26; a++) S[a] = a; bool fixed[26] = {0}; int order[26]; for (int a = 0; a < 26; a++) order[a] = a;
  Eval e2{T, c.ct, c.n, &L, 2}, e3{T, c.ct, c.n, &L, 3}, e4{T, c.ct, c.n, &L, 4}, e0{T, c.ct, c.n, &L, 0};
  float cs = 0;
  if (m == "ic0") cs = e0(S);
  else if (m == "hcbi") cs = climb(e2, S, fixed, order, 10);
  else if (m == "hctri") cs = climb(e3, S, fixed, order, 10);
  else if (m == "hcquad") cs = climb(e4, S, fixed, order, 10);
  else if (m == "hcic") cs = climb(e0, S, fixed, order, 10);
  else if (m == "hcbitri") { climb(e2, S, fixed, order, 10); cs = climb(e3, S, fixed, order, 10); }
  else if (m == "hcicbitri") { climb(e0, S, fixed, order, 10); climb(e2, S, fixed, order, 10); cs = climb(e3, S, fixed, order, 10); }
  else if (m == "estkbi" || m == "estktri") {
    Eval& ev = m == "estkbi" ? e2 : e3; float best = -1e30f; int bS[26];
    for (int x = 0; x < 26; x++) {
      int S2[26]; for (int a = 0; a < 26; a++) S2[a] = a; bool fx[26] = {0};
      S2[E_] = x; S2[x] = E_; fx[E_] = fx[x] = true;
      float s = climb(ev, S2, fx, order, 10);
      if (s > best) { best = s; memcpy(bS, S2, sizeof bS); }
    }
    memcpy(S, bS, sizeof bS); cs = best;
  } else { fprintf(stderr, "unknown method %s\n", m.c_str()); exit(1); }
  uint8_t p[MAXLEN]; decrypt32(T, S, c.ct, c.n, p);
  Res r; r.stat[0] = cs; r.stat[1] = sc_tri(L.tri.data(), p, c.n); r.stat[2] = sc_quad(L.quad.data(), p, c.n);
  r.correct = 0; for (int i = 0; i < c.n; i++) r.correct += p[i] == c.ptv[i]; return r;
}
int main(int argc, char** argv) {
  if (argc < 5) { fprintf(stderr, "exp1 CASES.tsv|real NGDIR METHOD NRAND [threads] [caseidx]\n"); return 1; }
  std::string cf = argv[1], ngdir = argv[2], m = argv[3]; int N = atoi(argv[4]); int th = argc > 5 ? atoi(argv[5]) : 6;
  int only = argc > 6 ? atoi(argv[6]) : -1;
  init_rtabs();
  std::vector<Case> cs = cf == "real" ? real_cases(ngdir) : load_cases(cf, ngdir);
  std::map<std::string, Lang> langs;
  std::vector<std::vector<uint8_t>> Ub(th, std::vector<uint8_t>(USZ + 64));
  printf("#case method truth_climb truth_tri truth_quad correct | pct_climb pct_tri pct_quad (fraction of random >= truth) | us/setting | off+-1,2 quad pct\n");
  for (size_t ci = 0; ci < cs.size(); ci++) {
    if (only >= 0 && (int)ci != only) continue;
    const Case& c = cs[ci];
    if (!langs.count(c.ngprefix)) { Lang L; if (!L.load(c.ngprefix)) { fprintf(stderr, "no lang %s\n", c.ngprefix.c_str()); return 1; } langs[c.ngprefix] = L; }
    const Lang& L = langs[c.ngprefix];
    std::mt19937 rng(ci * 7 + 1);
    // truth
    build_U(c.J, Ub[0].data()); Pattern P; sim_pattern(c.J.wR, c.J.wM, c.s.wr0, c.s.wm0, c.n, P);
    alignas(64) uint8_t T[MAXLEN][32]; make_T(c.J, Ub[0].data(), c.s, P, c.n, T);
    Res tr = run_method(m, T, c, L, rng);
    // phase neighbours: right window offset d (turnover moves), cores fixed
    float offq[4]; int offs[4] = {-2, -1, 1, 2};
    for (int k = 0; k < 4; k++) { Setting s2 = c.s; s2.wr0 = md(s2.wr0 + offs[k]); Pattern P2; sim_pattern(c.J.wR, c.J.wM, s2.wr0, s2.wm0, c.n, P2);
      make_T(c.J, Ub[0].data(), s2, P2, c.n, T); offq[k] = run_method(m, T, c, L, rng).stat[2]; }
    // random settings
    std::vector<Res> rr(N);
    double t0 = now();
#pragma omp parallel num_threads(th)
    {
      int tid = omp_get_thread_num(); std::mt19937 r2(1000 + ci * 131 + tid * 7919);
      alignas(64) uint8_t T2[MAXLEN][32];
      int per = 200;   // settings per random job
#pragma omp for schedule(dynamic, 1)
      for (int b = 0; b < (N + per - 1) / per; b++) {
        Job J = c.J;
        if (c.J.greek >= 0) { J.refl = r2() % 2; J.greek = 8 + r2() % 2; }
        do { J.wL = r2() % (c.J.greek >= 0 ? 8 : 5); J.wM = r2() % (c.J.greek >= 0 ? 8 : 5); J.wR = r2() % (c.J.greek >= 0 ? 8 : 5); } while (J.wL == J.wM || J.wL == J.wR || J.wM == J.wR);
        build_U(J, Ub[tid].data());
        for (int q = b * per; q < std::min(N, (b + 1) * per); q++) {
          Setting s{J.greek >= 0 ? (int)(r2() % 26) : 0, (int)(r2() % 26), (int)(r2() % 26), (int)(r2() % 26), (int)(r2() % 26), (int)(r2() % 26)};
          Pattern P3; sim_pattern(J.wR, J.wM, s.wr0, s.wm0, c.n, P3); make_T(J, Ub[tid].data(), s, P3, c.n, T2);
          rr[q] = run_method(m, T2, c, L, r2);
        }
      }
    }
    double el = now() - t0;
    double pct[3]; for (int k = 0; k < 3; k++) { long g = 0; for (auto& x : rr) g += x.stat[k] >= tr.stat[k]; pct[k] = (double)g / N; }
    double po[4]; for (int k = 0; k < 4; k++) { long g = 0; for (auto& x : rr) g += x.stat[2] >= offq[k]; po[k] = (double)g / N; }
    std::vector<float> q; for (auto& x : rr) q.push_back(x.stat[2]); std::sort(q.begin(), q.end());
    printf("%-12s %-8s %8.2f %8.2f %8.2f %2d | %.2e %.2e %.2e | %6.1f | %.1e %.1e %.1e %.1e | nullq max %.2f p999 %.2f\n", c.id.c_str(), m.c_str(), tr.stat[0], tr.stat[1], tr.stat[2], tr.correct,
           pct[0], pct[1], pct[2], el * th / N * 1e6, po[0], po[1], po[2], po[3], q.back(), q[(size_t)(0.999 * (N - 1))]);
    fflush(stdout);
  }
}
