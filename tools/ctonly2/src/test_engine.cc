#include "cases.h"
int main() {
  init_rtabs(); std::vector<uint8_t> U(USZ + 64);
  auto cs = load_cases("/opt/enigma-p1030680/tools/ctonly2/testset/synth32.tsv", "x");
  for (auto& c : real_cases("x")) cs.push_back(c);
  int bad = 0;
  for (auto& c : cs) {
    build_U(c.J, U.data()); Pattern P; sim_pattern(c.J.wR, c.J.wM, c.s.wr0, c.s.wm0, c.n, P);
    alignas(64) uint8_t T[MAXLEN][32]; make_T(c.J, U.data(), c.s, P, c.n, T);
    std::string out; for (int i = 0; i < c.n; i++) out += char('A' + c.S[T[i][c.S[c.ct[i]]]]);
    int lstep = P.dL[c.n-1];
    bool ok = out == c.pt; bad += !ok;
    printf("%-12s %s %s len=%d leftsteps=%d %s\n", c.id.c_str(), key_str(c.J, c.s).c_str(), ok ? "OK " : "BAD", c.n, lstep, ok ? "" : out.c_str());
  }
  printf("bad=%d\n", bad); return bad;
}
