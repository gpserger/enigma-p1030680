// exp4: funnel feasibility. Best-of-R m4f10 climb score at the true setting (and phase neighbours)
// vs the same statistic at NR random settings of the same ciphertext.
#include "cases.h"
#include "climb.h"
#include <omp.h>
#include <map>
#include <chrono>
static double now() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }
struct St { float fused, quad; int correct; };
static St solve(const Ctx2& cx, int R, std::mt19937& rng, const Case& c) {
  float best = -1e30f; int bS[26];
  for (int r = 0; r < R; r++) { int S[26]; for (int a = 0; a < 26; a++) S[a] = a; kick(S, 10, rng);
    climb_toggle<M_MONO>(cx, S, 4, true); float s = climb_toggle<M_FUSED>(cx, S, 10, true); if (s > best) { best = s; memcpy(bS, S, sizeof bS); } }
  uint8_t p[MAXLEN]; cx.dec(bS, p); St o; o.fused = best; o.quad = 0; for (int i = 0; i + 3 < c.n; i++) o.quad += cx.tb->quad[((p[i]*26+p[i+1])*26+p[i+2])*26+p[i+3]];
  o.quad /= (c.n - 3); o.correct = 0; for (int i = 0; i < c.n; i++) o.correct += p[i] == c.ptv[i]; return o;
}
int main(int argc, char** argv) {
  std::string cf = argv[1], ngdir = argv[2]; int R = atoi(argv[3]), NR = atoi(argv[4]), TT = atoi(argv[5]), th = atoi(argv[6]);
  init_rtabs();
  std::vector<Case> cs = load_cases(cf, ngdir); for (auto& c : real_cases(ngdir)) cs.push_back(c);
  std::map<std::string, Tables2> tabs; std::vector<std::vector<uint8_t>> Ub(th, std::vector<uint8_t>(USZ + 64));
  printf("#case  truth-trials: fused-pct quad-pct correct ... | null p99 p999 max (fused)\n");
  double sum1 = 0, sum01 = 0; int ntt = 0;
  for (size_t ci = 0; ci < cs.size(); ci++) {
    const Case& c = cs[ci];
    if (!tabs.count(c.ngprefix)) { Tables2& t2 = tabs[c.ngprefix]; t2.floor_cnt = 3; t2.load(c.ngprefix); }
    const Tables2& tb = tabs[c.ngprefix];
    std::vector<St> nul(NR);
#pragma omp parallel num_threads(th)
    { int tid = omp_get_thread_num(); std::mt19937 r2(ci * 7717 + tid); alignas(64) uint8_t T[MAXLEN][32];
#pragma omp for schedule(dynamic, 1)
      for (int b = 0; b < NR / 50; b++) {
        Job J = c.J; if (c.J.greek >= 0) { J.refl = r2() % 2; J.greek = 8 + r2() % 2; }
        int nw = c.J.greek >= 0 ? 8 : 5; do { J.wL = r2() % nw; J.wM = r2() % nw; J.wR = r2() % nw; } while (J.wL == J.wM || J.wL == J.wR || J.wM == J.wR);
        build_U(J, Ub[tid].data());
        for (int q = b * 50; q < b * 50 + 50; q++) {
          Setting s{J.greek >= 0 ? (int)(r2() % 26) : 0, (int)(r2() % 26), (int)(r2() % 26), (int)(r2() % 26), (int)(r2() % 26), (int)(r2() % 26)};
          Pattern P; sim_pattern(J.wR, J.wM, s.wr0, s.wm0, c.n, P); make_T(J, Ub[tid].data(), s, P, c.n, T);
          Ctx2 cx{T, c.ct, c.n, &tb}; nul[q] = solve(cx, R, r2, c);
        } } }
    std::vector<float> nf, nq; for (auto& x : nul) { nf.push_back(x.fused); nq.push_back(x.quad); }
    std::sort(nf.begin(), nf.end()); std::sort(nq.begin(), nq.end());
    auto pct = [&](const std::vector<float>& v, float x) { return (double)(v.end() - std::lower_bound(v.begin(), v.end(), x)) / v.size(); };
    build_U(c.J, Ub[0].data()); Pattern P; sim_pattern(c.J.wR, c.J.wM, c.s.wr0, c.s.wm0, c.n, P);
    alignas(64) uint8_t T[MAXLEN][32]; make_T(c.J, Ub[0].data(), c.s, P, c.n, T);
    printf("%-12s", c.id.c_str());
    std::vector<St> tr(TT);
#pragma omp parallel for num_threads(th)
    for (int t = 0; t < TT; t++) { std::mt19937 r3(t * 99991 + ci); Ctx2 cx{T, c.ct, c.n, &tb}; tr[t] = solve(cx, R, r3, c); }
    for (auto& o : tr) { double pf = pct(nf, o.fused), pq = pct(nq, o.quad); printf(" %.3f/%.3f/%d", pf, pq, o.correct); sum1 += pf <= 0.01; sum01 += pf <= 0.001; ntt++; }
    printf(" | %.3f %.3f %.3f\n", nf[(size_t)(0.99 * NR)], nf[(size_t)(0.999 * NR)], nf.back()); fflush(stdout);
  }
  printf("truth-trials in top 1%%: %.1f%%   top 0.1%%: %.1f%%   (R=%d)\n", 100 * sum1 / ntt, 100 * sum01 / ntt, R);
}
