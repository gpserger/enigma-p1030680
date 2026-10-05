// exp6: early-abort feasibility: score at eval checkpoints for successful vs failed climbs at the truth,
// and for climbs at random wrong settings.
#include "cases.h"
#include "fast.h"
#include <omp.h>
#include <map>
#include <array>
int main(int argc, char** argv) {
  std::string cf = argv[1], ngdir = argv[2]; int NC = atoi(argv[3]); int th = atoi(argv[4]);
  init_rtabs();
  std::vector<Case> cs = load_cases(cf, ngdir); for (auto& c : real_cases(ngdir)) cs.push_back(c);
  const int nchk = 6; int chk[nchk] = {325, 450, 600, 800, 1000, 1300};
  std::map<std::string, Tables2> tabs; std::map<std::string, FastLang> fl; std::vector<uint8_t> U(USZ + 64);
  // per case: relative score = score - (median final score at truth climbs)  [ciphertext-specific normalization]
  long keep_s[nchk][40] = {{0}}, keep_f[nchk][40] = {{0}}; long ns = 0, nf = 0;
  for (size_t ci = 0; ci < cs.size(); ci++) {
    const Case& c = cs[ci];
    if (!tabs.count(c.ngprefix)) { Tables2& t2 = tabs[c.ngprefix]; t2.floor_cnt = 10; t2.load(c.ngprefix); fl[c.ngprefix].build(t2, c.n); }
    const FastLang& FL = fl[c.ngprefix];
    build_U(c.J, U.data()); Pattern P; sim_pattern(c.J.wR, c.J.wM, c.s.wr0, c.s.wm0, c.n, P);
    alignas(64) static uint8_t T[MAXLEN][32]; make_T(c.J, U.data(), c.s, P, c.n, T);
    std::vector<std::array<int, nchk>> tr(NC); std::vector<int> ok(NC);
#pragma omp parallel for num_threads(th) schedule(dynamic, 16)
    for (int t = 0; t < NC; t++) {
      static thread_local Fast* F = nullptr; if (!F) F = new Fast; F->init(c.ct, c.n, &FL); F->set_setting(T);
      uint64_t rs = 0x9E3779B97F4A7C15ull * (t + 1) + ci * 7919 + 1;
      Fast::St st; for (int a = 0; a < 64; a++) st.S[a] = a < 26 ? a : 0; fast_kick(st.S, 10, rs);
      int trace[nchk]; fast_climb(*F, st, 10, trace, chk, nchk);
      for (int k = 0; k < nchk; k++) tr[t][k] = trace[k];
      int bl = 0; for (int a = 0; a < 26; a++) bl += st.S[a] == c.S[a]; ok[t] = bl >= 22;
    }
    // reference: 99th percentile of final scores of failed climbs (junk ceiling for this case)
    std::vector<int> fin; for (int t = 0; t < NC; t++) if (!ok[t]) fin.push_back(tr[t][nchk-1]); std::sort(fin.begin(), fin.end());
    int ref = fin[(size_t)(0.5 * fin.size())];
    int nok = 0; for (int t = 0; t < NC; t++) nok += ok[t];
    printf("%-12s success %d/%d  junk-median-final %d\n", c.id.c_str(), nok, NC, ref);
    for (int t = 0; t < NC; t++) for (int k = 0; k < nchk; k++) for (int th2 = 0; th2 < 40; th2++) {
      int thr = ref - 20000 + th2 * 1000;   // threshold relative to junk median (score units = 1000/log10 * 69)
      if (tr[t][k] >= thr) { if (ok[t]) keep_s[k][th2]++; else keep_f[k][th2]++; } }
    ns += nok; nf += NC - nok;
  }
  printf("checkpoint(evals) threshold(rel junk median) -> kept successes / kept failures\n");
  for (int k = 0; k < nchk; k++) { printf("chk %d:", chk[k]);
    for (int th2 = 10; th2 < 40; th2 += 2) printf(" [%+d: %.2f/%.3f]", -20000 + th2 * 1000, (double)keep_s[k][th2] / ns, (double)keep_f[k][th2] / nf); printf("\n"); }
}
