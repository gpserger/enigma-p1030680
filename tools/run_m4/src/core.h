// core.h - Enigma M3/M4 engine + n-gram scoring for the ciphertext-only pipeline.
// Wirings copied from py-enigma 1.0.2 rotors/data.py (cross-checked in research/04).
#pragma once
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

static const char* ROTOR_W[10] = {
  "EKMFLGDQVZNTOWYHXUSPAIBRCJ", "AJDKSIRUXBLHWTMCQGZNPYFVOE",
  "BDFHJLCPRTXVZNYEIWGAKMUSQO", "ESOVPZJAYQUIRHXLNFTGKDCMWB",
  "VZBRGITYUPSDNHLXAWMJQOFECK", "JPGVOUMFYQBENHZRDKASXLICTW",
  "NZJHGRCXMYSWBOUFAIVLPEKQDT", "FKQHTLXOCBJSPDZRAMEWNIUYGV",
  "LEYJVCNIXWPBQMDRTAKZGFUHOS" /*Beta*/, "FSOKANUERHMBTIYCWLQPZXVGJD" /*Gamma*/ };
static const char* ROTOR_NAME[10] = {"I","II","III","IV","V","VI","VII","VIII","Beta","Gamma"};
// notch letters (window letter at which the rotor kicks its left neighbour)
static const char* ROTOR_NOTCH[8] = {"Q","E","V","J","Z","ZM","ZM","ZM"};
// reflectors: 0=B-thin 1=C-thin 2=B(thick) 3=C(thick)
static const char* REFL_W[4] = {"ENKQAUYWJICOPBLMDXZVFTHRGS","RDOBJNTKVEHMLFCWZAXGYIPSUQ",
                                "YRUHQSLDPXNGOKMIEBFZCWVJAT","FVPJIAOYEDRZXWGCTKUQSBNMHL"};
static const char* REFL_NAME[4] = {"B-thin","C-thin","B","C"};

static const int MAXLEN = 256;

struct Tables {
  int fwd[10][26], bwd[10][26], refl[4][26];
  bool notch[8][26];
  Tables() {
    for (int w = 0; w < 10; w++) for (int c = 0; c < 26; c++) {
      fwd[w][c] = ROTOR_W[w][c]-'A'; bwd[w][fwd[w][c]] = c; }
    for (int r = 0; r < 4; r++) for (int c = 0; c < 26; c++) refl[r][c] = REFL_W[r][c]-'A';
    for (int w = 0; w < 8; w++) { for (int c=0;c<26;c++) notch[w][c]=false;
      for (const char* p = ROTOR_NOTCH[w]; *p; p++) notch[w][*p-'A'] = true; }
  }
};
static const Tables TB;

static inline int md(int x) { x %= 26; return x < 0 ? x + 26 : x; }

// Machine key. Greek wheel & left wheel rings are fixed at A (degenerate with
// position); middle/right ring matter (wiring offset + notch timing).
struct MKey {
  int refl;        // 0..3
  int greek;       // -1 (3-rotor machine), 8=Beta, 9=Gamma
  int wL, wM, wR;  // 0..7
  int pG, pL, pM, pR; // window positions at start
  int rL, rM, rR;  // rings (rL only used for display/compat; effect folded into pL)
};

// Stepping trajectory: window positions after the pre-keypress step, for each letter.
struct Traj { uint8_t L[MAXLEN], M[MAXLEN], R[MAXLEN]; };

static inline void make_traj(const MKey& k, int len, Traj& t) {
  int l = k.pL, m = k.pM, r = k.pR;
  for (int i = 0; i < len; i++) {
    bool mn = TB.notch[k.wM][m], rn = TB.notch[k.wR][r];
    if (mn) { m = md(m+1); l = md(l+1); }       // double step: middle + left
    else if (rn) { m = md(m+1); }
    r = md(r+1);
    t.L[i]=l; t.M[i]=m; t.R[i]=r;
  }
}

static inline int rot_f(int w, int sh, int c) { return md(TB.fwd[w][md(c+sh)] - sh); }
static inline int rot_b(int w, int sh, int c) { return md(TB.bwd[w][md(c+sh)] - sh); }

// Path table T[i][c]: scrambler substitution (no plugboard) at letter i.
static inline void make_path(const MKey& k, int len, uint8_t T[][26]) {
  Traj t; make_traj(k, len, t);
  for (int i = 0; i < len; i++) {
    int shR = t.R[i]-k.rR, shM = t.M[i]-k.rM, shL = t.L[i]-k.rL, shG = k.pG;
    for (int c = 0; c < 26; c++) {
      int x = rot_f(k.wR, shR, c); x = rot_f(k.wM, shM, x); x = rot_f(k.wL, shL, x);
      if (k.greek >= 0) x = rot_f(k.greek, shG, x);
      x = TB.refl[k.refl][x];
      if (k.greek >= 0) x = rot_b(k.greek, shG, x);
      x = rot_b(k.wL, shL, x); x = rot_b(k.wM, shM, x); x = rot_b(k.wR, shR, x);
      T[i][c] = (uint8_t)x;
    }
  }
}

static inline void decrypt(const uint8_t T[][26], const int* S, const uint8_t* ct, int len, uint8_t* out) {
  for (int i = 0; i < len; i++) out[i] = (uint8_t)S[T[i][S[ct[i]]]];
}

static std::string to_str(const uint8_t* p, int len) { std::string s(len,'?'); for (int i=0;i<len;i++) s[i]='A'+p[i]; return s; }

static int read_text(const char* path, uint8_t* out) {
  FILE* f = fopen(path, "r"); if (!f) { perror(path); exit(1); }
  int n = 0, c; while ((c = fgetc(f)) != EOF) { if (c>='a'&&c<='z') c-=32; if (c>='A'&&c<='Z' && n<MAXLEN) out[n++]=c-'A'; }
  fclose(f); return n;
}

static void parse_plugs(const char* s, int* S) {
  for (int i=0;i<26;i++) S[i]=i;
  std::vector<int> v; for (const char* p=s; *p; p++) { int c=*p; if(c>='a'&&c<='z')c-=32; if(c>='A'&&c<='Z') v.push_back(c-'A'); }
  for (size_t i=0;i+1<v.size();i+=2){ S[v[i]]=v[i+1]; S[v[i+1]]=v[i]; }
}
static std::string plug_str(const int* S) {
  std::string s; for (int i=0;i<26;i++) if (S[i]>i) { if(!s.empty()) s+=' '; s+=char('A'+i); s+=char('A'+S[i]); } return s;
}

// ---------------- n-gram models ----------------
// Loaded from "NGRAM<ws>value" files. mode 'c' = raw counts (converted to log10 prob,
// unseen floored at log10(0.01/total)); mode 'l' = already log-like ints (Krah .cur files),
// rescaled so unseen gets min-1.
struct NGram {
  int n = 0; std::vector<float> t; std::string name;
  bool load(const std::string& path, int order, char mode) {
    n = order; int sz = 1; for (int i=0;i<n;i++) sz*=26; t.assign(sz, NAN); name = path;
    FILE* f = fopen(path.c_str(), "r"); if (!f) return false;
    char g[64]; double v; double total = 0; std::vector<std::pair<int,double>> ent;
    while (fscanf(f, "%63s %lf", g, &v) == 2) {
      if ((int)strlen(g) != n) continue; int idx=0; bool ok=true;
      for (int i=0;i<n;i++){ int c=g[i]; if(c>='a'&&c<='z')c-=32; if(c<'A'||c>'Z'){ok=false;break;} idx=idx*26+(c-'A'); }
      if (!ok) continue; ent.push_back({idx,v}); total += v;
    }
    fclose(f);
    if (mode=='c') {
      for (auto& e: ent) t[e.first] = (float)log10(e.second/total);
      float fl = (float)log10(0.01/total); for (auto& x: t) if (std::isnan(x)) x = fl;
    } else {
      double mn=1e30, mx=-1e30; for (auto& e: ent){ mn=std::min(mn,e.second); mx=std::max(mx,e.second);} 
      // Krah ints: larger = more frequent. Map to "log10-ish": divide by a scale so range ~ matches.
      for (auto& e: ent) t[e.first] = (float)e.second;
      for (auto& x: t) if (std::isnan(x)) x = (float)(mn - (mx-mn)*0.05);
    }
    return !ent.empty();
  }
  // mean score per n-gram
  inline float score(const uint8_t* p, int len) const {
    float s = 0; int cnt = len - n + 1; if (cnt <= 0) return -1e9f;
    if (n==3) { for (int i=0;i<cnt;i++) s += t[(p[i]*26+p[i+1])*26+p[i+2]]; }
    else if (n==4) { for (int i=0;i<cnt;i++) s += t[((p[i]*26+p[i+1])*26+p[i+2])*26+p[i+3]]; }
    else if (n==2) { for (int i=0;i<cnt;i++) s += t[p[i]*26+p[i+1]]; }
    else { for (int i=0;i<cnt;i++){int idx=0; for(int j=0;j<n;j++) idx=idx*26+p[i+j]; s+=t[idx];} }
    return s / cnt;
  }
};
