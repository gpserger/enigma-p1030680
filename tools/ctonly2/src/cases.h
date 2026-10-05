// cases.h - test-case loading (synthetic TSV from gen_synth.py + fixed real vectors)
#pragma once
#include "engine.h"
#include <fstream>
#include <sstream>
struct Case {
  std::string id, fold, ngprefix; Job J; std::string pos, rings, plugs, pt, ct_s;
  uint8_t ct[MAXLEN], ptv[MAXLEN]; int n; Setting s; int S[26];
};
static void finish_case(Case& c) {
  c.n = c.ct_s.size(); for (int i = 0; i < c.n; i++) { c.ct[i] = c.ct_s[i]-'A'; c.ptv[i] = i < (int)c.pt.size() ? c.pt[i]-'A' : 0; }
  c.s = setting_from_key(c.J, c.pos.c_str(), c.rings.c_str()); parse_plugs(c.plugs.c_str(), c.S);
}
static std::vector<Case> load_cases(const std::string& path, const std::string& ngdir) {
  std::vector<Case> v; std::ifstream f(path); std::string line;
  while (std::getline(f, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::vector<std::string> t; std::stringstream ss(line); std::string x; while (std::getline(ss, x, '\t')) t.push_back(x);
    Case c; c.id = t[0]; c.fold = t[1]; c.J = parse_job(t[2].c_str(), t[3].c_str(), t[4].c_str());
    c.rings = t[5]; c.pos = t[6]; c.plugs = t[7]; c.pt = t[8]; c.ct_s = t[9];
    c.ngprefix = ngdir + "/fold" + c.fold; finish_case(c); v.push_back(c);
  }
  return v;
}
static std::vector<Case> real_cases(const std::string& ngdir) {
  std::vector<Case> v;
  Case a; a.id = "CFYZR"; a.fold = "-"; a.ngprefix = ngdir + "/navalblend"; a.J = parse_job("BT", "-", "531");
  a.pos = "ABER"; a.rings = "ALWB"; a.plugs = "BT CH DR EW FU GK JO LV MS PZ";
  a.ct_s = "NFOSOIFKXNEMBCXCWMSCMORVYWSVHFBZJHNEMQFWZQOLUIZBFFBSNKSQXSHRDAMFRSESGJJD";
  a.pt = "ANROEMEINSBERTAXQUARTIERMEISPCRPANZXGRUPPEXOSTROWOSTROWXKASERNENGELZENME"; finish_case(a); v.push_back(a);
  Case b; b.id = "P1030698"; b.fold = "-"; b.ngprefix = ngdir + "/navalloo"; b.J = parse_job("b", "g", "438");
  b.pos = "WQYR"; b.rings = "AACU"; b.plugs = "CH EJ NV OU TY LG SZ PK DI QB";
  { FILE* f = fopen("/opt/enigma-p1030680/runs/validation/p1030698.txt", "r"); char buf[512]; fgets(buf, sizeof buf, f); fclose(f);
    for (char* p = buf; *p; p++) if (isupper(*p)) b.ct_s += *p; }
  b.pt = "TTTFFFZWOVIERVVVFXDXUUUXAUSBXXTRAVEMUENDEBLEIBENXWEITEREBEFEHLEATWARTKNX"; finish_case(b); v.push_back(b);
  return v;
}
