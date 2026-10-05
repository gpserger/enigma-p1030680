// ct2.cc - ciphertext-only Enigma M3/M4 solver for short messages (tools/ctonly2).
//
// Search organisation (see research/12_ctonly2.md):
//   unit = (job = reflector x Greek x wheel order, right-wheel core cR)
//   stage A: for every Greek/left/middle core and every SAMPLED right-ring phase (window wr0 on a
//            stride grid; right-ring offsets between samples are treated as a few garbled letters),
//            with the middle wheel in the no-left-step stepping class: R kicked hill climbs of the
//            plugboard (fast.h), best board -> independent quadgram rescore -> per-unit top-K.
//   stage B: each top-K survivor is re-climbed (seeded with its board) at every right-ring phase
//            within +-stride/2 and every middle-ring stepping class (left-wheel turnover cases),
//            which recovers the exact key; per-unit results are written sorted by rescore.
#include "cases.h"
#include "fast.h"
#include <omp.h>
#include <map>
#include <set>
#include <mutex>
#include <atomic>
#include <chrono>
#include <ctime>
#include <unistd.h>

static double wall() { return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count(); }

struct Params {
  int R = 20, stride = 3, topk = 20, refineR = 8, threads = 8, kick = 10; uint64_t seed = 1;
  std::string midmode = "nostep";   // stage-A middle classes: nostep | all
};
struct Cand {
  float resc; int fused; Setting s; uint8_t S[26]; int stage;  // stage 0 = A, 1 = refined
  std::string pt;
};

struct Solver {
  float lastA_best = -99, lastA_kth = -99; int lastA_count = 0;
  int n; uint8_t ct[MAXLEN]; Tables2 tb; FastLang FL; std::vector<float> Q; Params P;
  void init(const uint8_t* c, int n_, const std::string& lang, const std::string& resc, const Params& p) {
    n = n_; memcpy(ct, c, n); P = p;
    tb.floor_cnt = 10; if (!tb.load(lang)) { fprintf(stderr, "cannot load %s_*\n", lang.c_str()); exit(1); }
    FL.build(tb, n);
    std::vector<float> dummy; if (!load_ng(resc, 4, Q, 0.01)) { fprintf(stderr, "cannot load %s\n", resc.c_str()); exit(1); }
  }
  float rescore(const uint8_t* p) const { float s = 0; for (int i = 0; i + 3 < n; i++) s += Q[((p[i]*26+p[i+1])*26+p[i+2])*26+p[i+3]]; return s / (n - 3); }

  // phases (right window wr0 values) sampled on the stride grid
  std::vector<int> phase_samples(const Job& J) const {
    int nph = J.wR >= 5 ? 13 : 26; std::vector<int> v; for (int w = 0; w < nph; w += P.stride) v.push_back(w); return v;
  }
  // middle-window representatives of the distinct stepping classes for right window wr0
  std::vector<int> mid_classes(const Job& J, int wr0, bool nostep_only) const {
    std::vector<int> reps; std::vector<Pattern> pats;
    for (int wm = 0; wm < 26; wm++) {
      Pattern p; sim_pattern(J.wR, J.wM, wr0, wm, n, p);
      if (nostep_only && p.dL[n-1] != 0) continue;
      bool dup = false; for (auto& q : pats) if (!memcmp(q.dM, p.dM, n) && !memcmp(q.dL, p.dL, n)) { dup = true; break; }
      if (!dup) { pats.push_back(p); reps.push_back(wm); }
      if (nostep_only) break;
    }
    return reps;
  }
  void keep(std::vector<Cand>& v, Cand&& c, int k) {
    v.push_back(std::move(c));
    if ((int)v.size() > 4 * k) { std::sort(v.begin(), v.end(), [](const Cand& a, const Cand& b) { return a.resc > b.resc; }); v.resize(k); }
  }
  // best-of-R climb at one setting; seed board optional
  int solve_setting(Fast& F, const uint8_t (*T)[32], int R, uint64_t& rs, const uint8_t* seedS, int seedKick, uint8_t* bestS) {
    F.set_setting(T); int best = INT32_MIN;
    for (int r = 0; r < R; r++) {
      Fast::St st;
      if (seedS) { for (int a = 0; a < 64; a++) st.S[a] = a < 26 ? seedS[a] : 0; if (r > 0) { // small kick: unplug 1-2 random pairs + add random
          for (int q = 0; q < seedKick; q++) { int a = rs % 26; rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17; int b = st.S[a]; st.S[a] = a; st.S[b] = b; }
          fast_kick(st.S, seedKick, rs); } }
      else { for (int a = 0; a < 64; a++) st.S[a] = a < 26 ? a : 0; fast_kick(st.S, P.kick, rs); }
      int s = fast_climb(F, st, 10);
      if (s > best) { best = s; memcpy(bestS, st.S, 26); }
    }
    return best;
  }
  // run one unit (job, cR); returns sorted candidates (refined first)
  std::vector<Cand> run_unit(const Job& J, int cR, long& nset, long& nevals, const Setting* only = nullptr) {
    std::vector<uint8_t> U(USZ + 64); build_U(J, U.data());
    auto phs = phase_samples(J);
    struct Item { int wr0, cM; };
    std::vector<Item> items; for (int w : phs) for (int m = 0; m < 26; m++) items.push_back({w, m});
    int ng = J.greek >= 0 ? 26 : 1;
    std::vector<std::vector<Cand>> per(P.threads);
    std::atomic<long> aset(0), aev(0);
#pragma omp parallel num_threads(P.threads)
    {
      int tid = omp_get_thread_num(); Fast* F = new Fast; F->init(ct, n, &FL);
      alignas(64) uint8_t T[MAXLEN][32];
#pragma omp for schedule(dynamic, 1)
      for (size_t it = 0; it < items.size(); it++) {
        int wr0 = items[it].wr0, cM = items[it].cM;
        auto cls = mid_classes(J, wr0, P.midmode == "nostep");
        for (int wm0 : cls) {
          Pattern Pt; sim_pattern(J.wR, J.wM, wr0, wm0, n, Pt);
          for (int cL = 0; cL < 26; cL++) for (int cG = 0; cG < ng; cG++) {
            Setting s{cG, cL, cM, cR, wm0, wr0};
            if (only && (only->cG != cG || only->cL != cL || only->cM != cM)) continue;
            make_T(J, U.data(), s, Pt, n, T);
            uint64_t rs = P.seed * 0x9E3779B97F4A7C15ull ^ ((uint64_t)((((J.refl*16+J.greek)*8+J.wL)*8+J.wM)*8+J.wR) << 40) ^ ((uint64_t)cR << 32) ^ (cM << 16) ^ (cL << 8) ^ cG ^ ((uint64_t)wr0 << 24);
            rs = rs ? rs : 1;
            Cand c; c.s = s; c.stage = 0;
            c.fused = solve_setting(*F, T, P.R, rs, nullptr, 0, c.S);
            uint8_t p[MAXLEN]; for (int i = 0; i < n; i++) p[i] = c.S[T[i][c.S[ct[i]]]];
            c.resc = rescore(p);
            keep(per[tid], std::move(c), P.topk);
            aset++;
          }
        }
      }
      aev += F->evals; delete F;
    }
    std::vector<Cand> all; for (auto& v : per) all.insert(all.end(), v.begin(), v.end());
    std::sort(all.begin(), all.end(), [](const Cand& a, const Cand& b) { return a.resc > b.resc; });
    // dedupe identical decrypt boards at same cores, keep topk
    if ((int)all.size() > P.topk) all.resize(P.topk);
    lastA_best = all.empty() ? -99 : all[0].resc; lastA_kth = all.empty() ? -99 : all.back().resc; lastA_count = all.size();
    // stage B: refine survivors over neighbouring phases and all middle classes
    std::vector<Cand> ref(all.size());
#pragma omp parallel num_threads(P.threads)
    {
      Fast* F = new Fast; F->init(ct, n, &FL); alignas(64) uint8_t T[MAXLEN][32];
#pragma omp for schedule(dynamic, 1)
      for (size_t k = 0; k < all.size(); k++) {
        const Cand& c0 = all[k]; Cand best = c0; best.stage = 1; best.fused = INT32_MIN;
        int nph = J.wR >= 5 ? 13 : 26; int h = P.stride / 2;
        for (int wr0 = 0; wr0 < nph; wr0++) {   // all phases: seeded climbs converge fast when the board is (nearly) right
          int d = std::min(md(wr0 - c0.s.wr0) % nph, nph - md(wr0 - c0.s.wr0) % nph);
          int Rr = d <= h ? P.refineR : 1;
          for (int wm0 : mid_classes(J, wr0, false)) {
            Pattern Pt; sim_pattern(J.wR, J.wM, wr0, wm0, n, Pt);
            Setting s = c0.s; s.wr0 = wr0; s.wm0 = wm0;
            make_T(J, U.data(), s, Pt, n, T);
            uint64_t rs = (P.seed + 77) * 0x9E3779B97F4A7C15ull ^ (k << 20) ^ (wr0 << 8) ^ wm0; rs = rs ? rs : 1;
            uint8_t S[26]; int f = solve_setting(*F, T, Rr, rs, c0.S, 2, S);
            if (f > best.fused) { best.fused = f; best.s = s; memcpy(best.S, S, 26); }
          }
        }
        Pattern Pt; sim_pattern(J.wR, J.wM, best.s.wr0, best.s.wm0, n, Pt); make_T(J, U.data(), best.s, Pt, n, T);
        uint8_t p[MAXLEN]; for (int i = 0; i < n; i++) p[i] = best.S[T[i][best.S[ct[i]]]];
        best.resc = rescore(p); best.pt = to_str(p, n); ref[k] = best;
        aev += 0;
      }
      aev += F->evals; delete F;
    }
    std::sort(ref.begin(), ref.end(), [](const Cand& a, const Cand& b) { return a.resc > b.resc; });
    nset = aset; nevals = aev;
    return ref;
  }
};

static std::string plugs_of(const uint8_t* S) { std::string s; for (int a = 0; a < 26; a++) if (S[a] > a) { if (!s.empty()) s += ' '; s += char('A'+a); s += char('A'+S[a]); } return s; }

static void usage() {
  fprintf(stderr,
    "ct2 validate CASES.tsv|real NGDIR [--R 20] [--stride 3] [--topk 20] [--refineR 8] [--threads 8] [--cases i,j] [--lang PREFIX] [--only-truth]\n"
    "    runs the truth's unit (job, true right core) for each case with the key hidden, reports detection\n"
    "ct2 run --ct FILE --lang NGPREFIX [--rescore QUADFILE] --jobs JOBLIST --out DIR [--R-tier \"40 30 20 15 15\"] [--threads 15]\n"
    "    resumable run; unit = (job, right core); DIR/units/<job>_rNN.tsv; DIR/stop stops after the current unit;\n"
    "    DIR/threads (a number) changes the thread count at the next unit\n");
  exit(1);
}
int main(int argc, char** argv) {
  if (argc < 2) usage();
  std::string mode = argv[1]; Params P; std::string casesf, ngdir, lang, resc, ctf, jobs, crs = "all", out = "-", casesel, langover;
  bool only_truth = false; int a0 = 2; int trials = 1; int nwrong = 0; int wrong_fp = 0; std::string rtier = "20";
  if (mode == "validate") { if (argc < 4) usage(); casesf = argv[2]; ngdir = argv[3]; a0 = 4; }
  for (int a = a0; a < argc; a++) {
    std::string s = argv[a]; auto nx = [&]() { if (a + 1 >= argc) usage(); return std::string(argv[++a]); };
    if (s == "--R") P.R = atoi(nx().c_str()); else if (s == "--stride") P.stride = atoi(nx().c_str()); else if (s == "--topk") P.topk = atoi(nx().c_str());
    else if (s == "--refineR") P.refineR = atoi(nx().c_str()); else if (s == "--threads") P.threads = atoi(nx().c_str()); else if (s == "--kick") P.kick = atoi(nx().c_str());
    else if (s == "--seed") P.seed = atoll(nx().c_str()); else if (s == "--mid") P.midmode = nx();
    else if (s == "--cases") casesel = nx(); else if (s == "--trials") trials = atoi(nx().c_str()); else if (s == "--wrong") nwrong = atoi(nx().c_str()); else if (s == "--only-truth") only_truth = true; else if (s == "--lang") lang = nx(); else if (s == "--rescore") resc = nx();
    else if (s == "--ct") ctf = nx(); else if (s == "--jobs") jobs = nx(); else if (s == "--R-tier") rtier = nx();  else if (s == "--cr") crs = nx(); else if (s == "--out") out = nx();
    else usage();
  }
  init_rtabs(); g_u8 = true;
  if (mode == "validate") {
    std::vector<Case> cs = casesf == "real" ? real_cases(ngdir) : load_cases(casesf, ngdir);
    if (casesf != "real") for (auto& c : real_cases(ngdir)) cs.push_back(c);
    std::set<int> sel; if (!casesel.empty()) { std::string t = casesel; for (char* q = strtok(&t[0], ","); q; q = strtok(nullptr, ",")) sel.insert(atoi(q)); }
    int det = 0, det2 = 0, tot = 0; double tw = 0; long tset = 0, tev = 0;
    for (size_t ci = 0; ci < cs.size(); ci++) {
      if (!sel.empty() && !sel.count(ci)) continue;
      const Case& c = cs[ci]; Solver sv;
      std::string lp = lang.empty() ? c.ngprefix : lang;
      sv.init(c.ct, c.n, lp, lp + "_quadgrams.txt", P);
      double t0 = wall(); long ns, ne;
      if (trials > 1) {   // repeated only-truth runs: detection probability at the true cores
        int d = 0, d2 = 0; std::string rs; float cut = getenv("CUT") ? atof(getenv("CUT")) : -6.0f;
        for (int t = 0; t < trials; t++) { sv.P.seed = P.seed + 1000 * t + 1;
          auto r2 = sv.run_unit(c.J, c.s.cR, ns, ne, &c.s); int cr = 0; for (int i = 0; i < c.n; i++) cr += r2[0].pt[i] == c.pt[i];
          d += cr >= c.n - 2; d2 += (cr >= c.n - 2) && sv.lastA_best >= cut; char b[32]; snprintf(b, sizeof b, "%.2f/%.2f%s ", sv.lastA_best, r2[0].resc, cr >= c.n - 2 ? "*" : ""); rs += b; }
        int lstep = 0; { Pattern Pt; sim_pattern(c.J.wR, c.J.wM, c.s.wr0, c.s.wm0, c.n, Pt); for (int i = 0; i < c.n; i++) if (Pt.dL[i]) { lstep = i + 1; break; } }
        printf("%-12s wR=%-4s lstep@%-2d detected %d/%d (stageA>=cut %d)  (stageA-best/refined rescore, *=found): %s\n", c.id.c_str(), ROTOR_NAME[c.J.wR], lstep, d, trials, d2, rs.c_str()); det2 += d2;
        fflush(stdout); det += d; tot += trials; continue;
      }
      auto res = sv.run_unit(c.J, c.s.cR, ns, ne, only_truth ? &c.s : nullptr);
      double el = wall() - t0;
      // detection: top candidate decrypts to the plaintext (>= n-2 letters) ; also rank of first correct
      int rank = -1; for (size_t k = 0; k < res.size(); k++) { int cr = 0; for (int i = 0; i < c.n; i++) cr += res[k].pt[i] == c.pt[i]; if (cr >= c.n - 2) { rank = k; break; } }
      tot++; det += rank == 0;
      int cr0 = 0; for (int i = 0; i < c.n; i++) cr0 += res[0].pt[i] == c.pt[i];
      int lstep = 0; { Pattern Pt; sim_pattern(c.J.wR, c.J.wM, c.s.wr0, c.s.wm0, c.n, Pt); lstep = Pt.dL[c.n-1] ? 1 : 0; for (int i = 0; i < c.n; i++) if (Pt.dL[i]) { lstep = i + 1; break; } }
      printf("stageA best %.3f kth %.3f\n", sv.lastA_best, sv.lastA_kth);
      printf("%-12s %s wR=%s leftstep@%d  rank=%d  top: %.3f %s %s %s (%d/%d)  %.0fs %ld settings %.1fM evals\n", c.id.c_str(), key_str(c.J, c.s).c_str(),
             ROTOR_NAME[c.J.wR], lstep, rank, res[0].resc, key_str(c.J, res[0].s).c_str(), plugs_of(res[0].S).c_str(), res[0].pt.c_str(), cr0, c.n, el, ns, ne / 1e6);
      if (rank > 0) printf("             truth at rank %d: %.3f\n", rank, res[rank].resc);
      if (res.size() > 1) printf("             #2: %.3f %s\n", res[rank == 0 ? 1 : 0].resc, res[rank == 0 ? 1 : 0].pt.c_str());
      if (nwrong > 0) {   // false-positive check: random wrong-order units of the same ciphertext
        std::mt19937 wr(1234 + ci); float mx = -99; std::string mxs; std::vector<float> tops;
        for (int w = 0; w < nwrong; w++) {
          Job J2 = c.J;
          do { if (c.J.greek >= 0) { J2.refl = wr() % 2; J2.greek = 8 + wr() % 2; }
               int nw = c.J.greek >= 0 ? 8 : 5; J2.wL = wr() % nw; J2.wM = wr() % nw; J2.wR = wr() % nw; }
          while (J2.wL == J2.wM || J2.wL == J2.wR || J2.wM == J2.wR || (J2.wL == c.J.wL && J2.wM == c.J.wM && J2.wR == c.J.wR));
          int cR2 = wr() % 26; long ns2, ne2; double tw0 = wall();
          auto r2 = sv.run_unit(J2, cR2, ns2, ne2);
          tops.push_back(r2[0].resc); if (r2[0].resc > mx) { mx = r2[0].resc; mxs = key_str(J2, r2[0].s) + " " + r2[0].pt; }
          printf("   wrong unit %s cR=%d: top %.3f  (A-best %.3f, A-kth %.3f)  %.0fs %ld settings\n", job_str(J2).c_str(), cR2, r2[0].resc, sv.lastA_best, sv.lastA_kth, wall() - tw0, ns2);
          fflush(stdout); tset += ns2; tev += ne2;
        }
        printf("   WRONG-ORDER max %.3f vs truth-unit top %.3f (%s): %s\n", mx, res[0].resc, rank == 0 ? "truth" : "junk", mxs.c_str());
        wrong_fp += (rank == 0 && mx >= res[0].resc);
      }
      if (getenv("DUMP")) for (auto& r : res) printf("    %.3f f=%d %s %s %s\n", r.resc, r.fused, key_str(c.J, r.s).c_str(), plugs_of(r.S).c_str(), r.pt.c_str());
      fflush(stdout); tw += el; tset += ns; tev += ne;
    }
    printf("stageA>=cut: %d\n", det2); if (nwrong) printf("wrong-order units beating a detected truth: %d\n", wrong_fp);
    printf("DETECTED (rank 1 in unit) %d/%d   wall %.0fs  settings %ld  evals %.2fG  (%d threads)\n", det, tot, tw, tset, tev / 1e9, P.threads);
    return 0;
  }
  if (mode == "run") {
    // resumable production run over a job list; unit = (job, right core cR)
    if (ctf.empty() || lang.empty() || jobs.empty() || out == "-") usage();
    if (resc.empty()) resc = lang + "_quadgrams.txt";
    uint8_t ct[MAXLEN]; int n = read_text(ctf.c_str(), ct);
    std::vector<int> Rt; { std::string t = rtier; for (char* q = strtok(&t[0], " ,"); q; q = strtok(nullptr, " ,")) Rt.push_back(atoi(q)); }
    std::string cmd = "mkdir -p " + out + "/units"; if (system(cmd.c_str())) {}
    struct JL { std::string id; Job J; int tier; };
    std::vector<JL> jl; { FILE* f = fopen(jobs.c_str(), "r"); char line[256];
      while (fgets(line, sizeof line, f)) { if (line[0] == '#') continue; char pr[16], id[64], rf[8], gk[8], wh[8]; int tier;
        if (sscanf(line, "%15s %63s %7s %7s %7s %d", pr, id, rf, gk, wh, &tier) != 6) continue;
        jl.push_back({id, parse_job(rf, gk, wh), tier}); } fclose(f); }
    FILE* lg = fopen((out + "/run.log").c_str(), "a");
    fprintf(lg, "# start %ld pid %d: ct=%s n=%d lang=%s rescore=%s R_by_tier=%s stride=%d topk=%d refineR=%d kick=%d seed=%lu\n", (long)time(nullptr), getpid(),
            ctf.c_str(), n, lang.c_str(), resc.c_str(), rtier.c_str(), P.stride, P.topk, P.refineR, P.kick, (unsigned long)P.seed); fflush(lg);
    Solver sv; sv.init(ct, n, lang, resc, P);
    for (auto& jb : jl) {
      for (int cR = 0; cR < 26; cR++) {
        char fn[512]; snprintf(fn, sizeof fn, "%s/units/%s_r%02d.tsv", out.c_str(), jb.id.c_str(), cR);
        { FILE* f = fopen(fn, "r"); if (f) { fclose(f); continue; } }
        { FILE* f = fopen((out + "/stop").c_str(), "r"); if (f) { fclose(f); fprintf(lg, "# stop flag seen %ld\n", (long)time(nullptr)); fclose(lg); return 0; } }
        { FILE* f = fopen((out + "/threads").c_str(), "r"); if (f) { int t; if (fscanf(f, "%d", &t) == 1 && t >= 1 && t <= 64) sv.P.threads = t; fclose(f); } }
        sv.P.R = Rt[std::min<int>(jb.tier, Rt.size() - 1)];
        double t0 = wall(); long ns, ne;
        auto res = sv.run_unit(jb.J, cR, ns, ne);
        double el = wall() - t0;
        std::string tmp = std::string(fn) + ".tmp"; FILE* f = fopen(tmp.c_str(), "w");
        fprintf(f, "#rescore\tfused\tkey\tplugs\tdecrypt\n");
        for (auto& r : res) fprintf(f, "%.4f\t%d\t%s\t%s\t%s\n", r.resc, r.fused, key_str(jb.J, r.s).c_str(), plugs_of(r.S).c_str(), r.pt.c_str());
        fprintf(f, "#DONE job=%s cR=%d tier=%d R=%d wall=%.1f threads=%d settings=%ld evals=%ld stageA_best=%.4f stageA_kth=%.4f\n", jb.id.c_str(), cR, jb.tier, sv.P.R, el, sv.P.threads, ns, ne, sv.lastA_best, sv.lastA_kth);
        fclose(f); rename(tmp.c_str(), fn);
        fprintf(lg, "%ld %s r%02d tier=%d R=%d wall=%.1f thr=%d settings=%ld evals=%ld best=%.4f\n", (long)time(nullptr), jb.id.c_str(), cR, jb.tier, sv.P.R, el, sv.P.threads, ns, ne,
                res.empty() ? -99.f : res[0].resc); fflush(lg);
      }
    }
    fprintf(lg, "# all jobs done %ld\n", (long)time(nullptr)); fclose(lg); return 0;
  }
  usage();
}
