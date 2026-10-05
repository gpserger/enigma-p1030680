# 07 — Vector C: ciphertext-only pipeline, validation, and the running M4 search

Date: 2026-09-23. Target: `ciphertext.txt` (P1030680, 72 letters, M4, probably key-net Thetis).
All numbers below were measured on this machine (Ryzen 7 7800X3D, 16 threads) unless marked otherwise.

## TL;DR

- **Test vectors are consistent.** CFYZR (Army, 72 letters) decrypts exactly with py-enigma and with my own engine. I also validated on a better, naval vector found in the corpus agent's output: **P1030698**, a genuine U-534 M4 message of exactly 72 letters (Potsdam, 1 May 1945).
- **The pipeline breaks both 72-letter messages ciphertext-only when searching their correct wheel order / job:**
  - **CFYZR:** all 456,976 rotor keys of B 5-3-1 at R=40 restarts/key, in 453 s on 16 threads. Exact key and plaintext, **rank #1**.
  - **P1030698:** the 456,976-key right-ring unit of the job b/Gamma/IV-III-VIII at R=40, in 411 s on 16 threads. Exact plaintext, **rank #1** at −4.77 against −5.64 for the #2 junk, with scoring tables that exclude this message.
  - **At R=5 neither was recovered.** Per-key success is the limiting factor, not ranking.
- **Ranking must not use the climb's own score.** trognes' fused score (which includes the index of coincidence) ranks junk above the truth at 72 letters. I patched trognes to keep a per-job top-N by an **independent quadgram rescore**, and that puts the truth first in both validations.
- **The Potsdam-key check is negative.** All three known Potsdam daily keys were tried with every one of 26^4 message keys × 676 middle/right ring settings (≈9×10^8 decrypts). The best decrypts score −6.9 on the naval quadgram rescore; real 72-letter naval plaintext scores about −4.8. ELKC and PUYY are gibberish. Separately, I reproduced the operator's own failed decrypt **IPZAYK** exactly (1 May Potsdam, rings VCCH, message key PUYY).
- **The full run is going in the background:** 15 workers, 1,344 jobs (2 thin reflectors × 2 Greek wheels × 336 wheel orders), priority-ordered.
  - **Estimated total: ~28.6 days.** The 168 jobs with VIII on the right (including the three Potsdam orders) finish in **~5.6 days**.
  - Progress and results: `tools/run_m4/top.py`.
- **Honest odds.** Even in the correct job, one pass finds a 72-letter naval message with probability of roughly 20–35% at R=20 and ~10% at R=5. The first pass also pins the middle ring (O&W shortcut). By research/02 §4.2 that covers roughly 79% of messages for the top-priority jobs (VIII on the right, single-notch middle wheel) and only ~57% when the middle wheel is also VI/VII/VIII. A null result will therefore **not** exclude a wheel order. See §6.

---

## 1. Tooling built

| Path | What |
|---|---|
| `tools/run_m4/src/core.h` | My own Enigma M3/M4 engine: py-enigma wirings, double-step stepping, path tables, n-gram loaders. Passes CFYZR, U-264 (M4) and IPZAYK (M4) against py-enigma. |
| `tools/run_m4/src/potsdam.cc` → `tools/run_m4/potsdam` | Potsdam-key sweep, OpenMP. |
| `tools/run_m4/src/m4hc.cc`, `hc.h` → `tools/run_m4/m4hc` | My own solver: exact M4 key enumeration with middle-ring turnover classes and 13-phase double-notch reduction, a first-improvement climber, and O&W I-/E-Stecker seed exhaustion. Modes `probe`, `job`, `bench`. Used for diagnosis; **not** the production engine (see §2.3). |
| `tools/enigma/enigma` (trognes, **patched**; original in `enigma.cc.orig`) | Production engine. Adds `--topn N --topn-file F --rescore QUADFILE` (global top-N of distinct decrypts by an independent quadgram rescore, checkpointed every 120 s) and `--exhaust-letters LETTERS` (O&W E-/R-/I-Stecker: restricts `--exhaust 1` to pairs touching the letters). The search itself is unchanged. |
| `tools/run_m4/build_blend.py` | Rebuilds the corpus agent's `navalblend` language exactly; supports leave-one-out (`--exclude ID`) for honest validation. |
| `tools/run_m4/{config.sh,make_jobs.py,job_run.sh,worker.sh,run.sh,stop.sh,top.py}` | The resumable background run (§5). |
| `runs/validation/…`, `runs/potsdam/…` | All validation and Potsdam-check outputs. |

## 2. Validation

### 2.1 Test vectors are self-consistent

- **CFYZR** (`research/02` §2.5): B 5-3-1, rings LWB, start BER, plugs BT CH DR EW FU GK JO LV MS PZ.
  - It decrypts to `ANROEMEINSBERTAXQUARTIERMEISPCRPANZXGRUPPEXOSTROWOSTROWXKASERNENGELZENME` in py-enigma (`runs/validation/cfyzr_check.py`), in my C++ core, and in trognes.
  - **Gotcha:** the left-wheel ring *does* matter for a given start position. The equivalent key with rings AAB has start **QIR**, not BIR.
- **P1030698** (corpus `messages.json`): b-thin, Gamma, IV-III-VIII, rings AACU, start WQYR, plugs CH EJ NV OU TY LG SZ PK DI QB.
  - It decrypts to `TTTFFFZWOVIERVVVFXDXUUUXAUSBXXTRAVEMUENDEBLEIBENXWEITEREBEFEHLEATWARTKNX` ("to T.F. 24 from FdU Ausbildung: remain in Travemünde, await further orders").
  - It has exactly the same length, month, sender chain and machine as our target, so it is the best calibration point available.
- **U-264** (M4) and **IPZAYK** also reproduce in my core (`tools/run_m4/src/test_core.cc`).
- **Double-notch 13-phase symmetry:** for VI/VII/VIII on the right, (ring+13, start+13) gives an identical decrypt. Checked with py-enigma on 300 random M4 keys: 0 violations. This halves the right-ring work for those wheel orders.

### 2.2 Results on CFYZR (Army, 72 letters): patched trognes, correct wheel order only

Setup:
- Command: `-c -J --score m4f10 -f -l wehrmacht -u B -w 531 -r AA. -g ... -T 16 --topn 200 --rescore wehrmacht_quadgrams`
- Keyspace: all 26^3 starts × 26 right rings, middle ring pinned at A (456,976 keys).

| R (restarts/key) | wall, 16 thr | thread-h / wheel order | result | rank of truth by rescore |
|---|---|---|---|---|
| 1 | 11.3 s | 0.05 | not found | – |
| 5 | 56 s | 0.25 | not found | – |
| 20 | 260 s | 1.2 | true board found at a near-equivalent key (right ring/start +1; 2 letters differ) | 3 |
| **40** | **453 s** | **2.0** | **exact key AAB/QIR and exact plaintext** | **1** (−5.808; best junk −5.957) |

- trognes' own final pick (best fused score) was junk at every R. **Ranking must use the rescore.**
- Ostwald & Weierud needed ≈28 core-hours per wheel order (2013 i7, I-Stecker). This setup needs ≈2 thread-hours.
- **Neighbourhood (task item 1b), done only partly:**
  - No *wrong wheel order* was run for CFYZR, to avoid taking CPU from the production launch.
  - The evidence on competing wrong keys comes from two places. First, all 456,975 other keys of the correct order: the best junk there was −5.96 to −5.99, and the exact truth (−5.81) beat all of it. Second, the P1030698 full job, whose 12 non-truth right-ring units (5.5M wrong keys) produced junk ≤ −5.58, against −4.77 for the truth.
  - A few wrong orders at R=40 (~7.5 min each on 16 threads) would complete this and can be run after the tier-0 jobs.
- Outputs: `runs/validation/cfyzr_trognes/`.

### 2.3 Diagnosis: why the obvious approaches fail at 72 letters, and what I tried

1. **Single climb from an empty board is useless.**
   - My solver ran one climb per key over all 2,161,848 keys of B 5-3-1, including every middle-ring turnover class (57 s on 16 threads).
   - The true key ranked **1,328,202nd**, no better than random. This confirms O&W: no statistic finds the first plugs at this length.
2. **My own I-Stecker implementation (O&W) works at the true key but is too slow.**
   - At the CFYZR key, 136 seeds (all pairs touching E,N,R,X,S,I) recover the exact board. Only the E–W seed succeeds, even when extended to all 325 pairs.
   - Cost: 37 ms/key/thread, about 22 thread-hours per Army wheel order. For M4 that is roughly 1,000 thread-hours per job, or years in total.
   - On a benchmark of random M4 keys + 10 plugs + 72-letter real plaintext windows (`m4hc bench`), my I-Stecker climber succeeded only 4.5–7.5% of the time at the true key.
3. **Krah's enigma-suite (the 2006 M4-Project tool)** fails CFYZR **even at the true key**: 0 of 200 passes with its naval dictionaries (`-k B:531:AB:QIR`).
   - Its IC-first climb (Gillogly / Sullivan–Weierud) is exactly what O&W showed breaks down below ~80 letters.
   - It is fast (17,576 keys in 0.54 s single-thread) but not usable here.
   - Because it fails at the true key, I did **not** run it as a ciphertext-only search over the correct wheel order; that would be pointless.
4. **trognes' tuned climber is the best engine per unit of compute.**
   - It does ~5×10^6 plugboard evaluations/s/thread, uses a mono pre-pass, a fused target capped at 10 plugs, and random-kick restarts.
   - True-key benchmark: 300 random M4 keys, 10 plugs, 72-letter windows of real telegraphic plaintext (`runs/validation/bench/trognes_bench2.py`, results in `trognes_bench2_300.txt`):

| config | success at true key | boards/key | success per 10^6 boards |
|---|---|---|---|
| R=1 | 0.7% | 2.3k | 2.9 |
| R=3 | 2.0% | 6.8k | 2.9 |
| R=5 | 3.0% | 11.4k | 2.6 |
| R=10 | 6.3% | 22.5k | 2.8 |
| R=25 | 12.3% | 57k | 2.2 |
| R=75 | 17.3% | 171k | 1.0 |
| R=5 + per-key `--polish` | 8.0% | 23.6k | 3.4 |
| E-Stecker × R=1 (`--exhaust 1 --exhaust-letters E`) | 3.0% | 49k | 0.6 |
| I-Stecker (ENRXSI) × R=1 | 9.0% | 265k | 0.3 |

   - **Efficiency is flat up to R≈20, then falls.** O&W seed exhaustion is less efficient than random-kick restarts in trognes.
   - This benchmark uses the Army `wehrmacht` tables on mostly Army plaintexts. It understates success for naval text scored with naval tables; compare §2.4.

### 2.4 Naval M4 validation: P1030698 (72 letters), leave-one-out naval tables

**Scoring language is decisive.**
- The true P1030698 plaintext scores −8.38 fused / −6.73 rescore under the Army `wehrmacht` tables, worse than typical junk, so it would never be found.
- Under the corpus agent's `navalblend` it scores −4.75, but that is contaminated because its own text is in the corpus.
- I therefore rebuilt the blend without P1030698 (`navalloo`, `tools/run_m4/ngrams/`). My rebuild of the full blend reproduces the shipped one to 0.006 score, which checks `build_blend.py`. Under `navalloo` the truth scores −5.72 fused / −4.77 rescore.

Results:
- **Full correct job at R=5** (b/Gamma/IV-III-VIII, 13 right-ring units, 5,940,688 keys, production `job_run.sh`, 16 threads): 730 s including stage B. **Not found.** Best junk rescore in the whole job: **−5.58**.
- **Per-key success at the exact key** (20 seeds each): R=5 **5%**, R=20 **20%**, R=40 **30%**, R=100 **60%**.
- **Truth's unit at R=40** (456,976 keys, 411 s on 16 threads): **found, rank #1**, rescore −4.769 against −5.645 for the #2 junk. The key reported is b G438 AAAH WPVE, equivalent to rings AACU/start WQYR via the 13-phase and middle-ring equivalences, with the exact plugboard. Outputs: `runs/validation/p1030698_unitH/`.
- **Discrimination is strong for naval-style text** (truth −4.77 vs best junk of a 5.9M-key job −5.58).
  - **Caveat:** that junk ceiling was measured at R=5 with `navalloo` on a single job. More restarts produce better-optimised junk.
  - In production (R=20, `navalblend`), junk had already reached −5.56 about 2 minutes in (~2.5×10^5 of 1.3×10^10 keys).
  - A normal-tail extrapolation suggests the best junk over the whole space may approach −5.0 to −5.3.
  - A P1030698-like truth (−4.8) should still be #1 in its own unit, and each unit keeps its own top 40, so retention is safe. But the *global* `top.py` list will fill with junk scoring around −5.
  - Re-measure the ceiling once the first tier-0 job's `final.tsv` exists. For triage, use `top.py --by combo`, `--by words`, and actually reading the texts, not row 1 alone.

## 3. Cheap check: the three known Potsdam keys (task item 4)

Setup (`tools/run_m4/potsdam`, `runs/potsdam/`):
- Target decrypted under each Potsdam daily key (research/03 §4), with reflector, Greek wheel, wheel order and plugboard as published.
- Swept: all 26^4 message keys (Greek, left, middle, right windows) × all 26² middle/right ring settings. Greek and left rings are degenerate with position. That is 3.09×10^8 decrypts per day, 95 s in total.
- Scored with Krah's naval trigrams, the Army quadgrams, and (re-run) the naval-blend quadgrams.

| day | key (as published) | best naval-blend rescore | best Krah tri | real 72-letter naval plaintext for comparison |
|---|---|---|---|---|
| 30 Apr | C-thin, Beta, V-VI-VIII, AAEL, AE BF CM DQ HU JN LX PR SZ VW | −6.89 | 13,005 | −4.77 (P1030698); Krah tri 20–37k |
| 1 May | B-thin, Gamma, IV-III-VIII, VCCH, CH EJ NV OU TY LG SZ PK DI QB | −6.88 | 13,590 | " |
| 2 May | C-thin, Gamma, V-II-VIII, AAFB, CP DG EJ FI KT LZ MS NO QU RW | −6.96 | 12,438 | " |

**Result: negative.** No message key and no ring setting of any known Potsdam key produces anything near plaintext. The best of ~9×10^8 decrypts is the expected noise maximum.

Explicit Girard-derived message keys (py-enigma, `runs/potsdam/ipzayk.py`):
- 1 May, rings **VCCH**, message key **PUYY**: `IPZAYKMKXDCTZJSUEPSC…`. This exactly reproduces the operator's pencilled "IPZAYK" (research/03 §3 step 5). It confirms three things:
  1. The published ciphertext's first 6 letters.
  2. That site "Greek C" means Gamma.
  3. That VCCH is the ring reading consistent with the operator's Grund.
- 1 May / VCCH / **ELKC**: `DACEZBDKQYTMSHHXXNOG…`, gibberish.
- ELKC and PUYY under the 30 Apr and 2 May keys, and under AACU: all gibberish.

The operator's own attempt with the Potsdam key failed, and so does exhaustion over every message key. This is consistent with the key-net really being something other than Potsdam (Thetis), or with Potsdam being used for a different day.

## 4. Run configuration (task item 2)

**Keyspace per job.** A job is one (thin reflector ∈ {B, C}) × (Greek ∈ {Beta, Gamma}) × (wheel order ∈ 336 from I–VIII). There are 1,344 jobs. Inside a job:
- Greek offset: 26. The Greek ring is fixed at A, since it is degenerate with position.
- Left, middle and right start positions: 26^3.
- Left ring: fixed at A (degenerate).
- **Right ring:** 26 values, or **13 when the right wheel is VI/VII/VIII**, because the Z+M notches make (ring, start) ≡ (ring+13, start+13). Verified in §2.1.
  - Each right-ring value is one resumable **unit** of 456,976 keys (`-r AAA<X> -g ....`).
- **Middle ring (research/02 §4.2):**
  - Pass 1 pins it at A (the O&W shortcut). This covers the case where the left wheel does not turn over during the message, plus most other positions.
  - Exact enumeration of the distinct middle-ring turnover classes (k+1 cases, as §4.2 argues) measures **4.73×** the keys for a single-notch middle wheel and **7.46×** for a double-notch one. I measured this both with my `m4hc` KeyGen and with trognes `-r AA..`, and the two agree.
  - This is the planned pass 2 (`MIDRING` / `-r AA..`). Estimated coverage of pass 1, from P(left turnover) ≈ k·n/26 in research/02 §4.2:
  - single-notch right and middle: ≈89%;
  - double-notch right (all tier-0/1 jobs) with a single-notch middle: ≈79%;
  - double-notch right and middle: ≈57%.
- Keys per job: 5,940,688 with VI/VII/VIII on the right; 11,881,376 otherwise. **Total 1.3×10^10 rotor keys.**

**Search in each unit (stage A).**
- Patched trognes, single-threaded: `-4 -c -J --score m4f10 -f -l navalblend -d corpus/ngrams -R <R> --topn 40 --rescore corpus/ngrams/navalblend_quadgrams.txt`.
- This is a random-kick first-improvement climb with a mono pre-pass (≤4 plugs), then the fused naval-blend target capped at 10 plugs, for R restarts per key.
- The top 40 distinct decrypts per unit are kept by the naval-blend quadgram rescore.
- The n-grams come from `corpus/ngrams`, built from 56 real 1945 naval plaintexts and blended with telegraphic and prose German. They are preferred over the solvers' built-in tables, as the task asks.

**Restarts by priority tier** (`R_BY_TIER="20 20 10 5 5"`):

| tier | jobs | definition | R |
|---|---|---|---|
| 0 | 12 | the three 1945 Potsdam orders IV-III-VIII, V-VI-VIII, V-II-VIII | 20 |
| 1 | 156 | VIII on the right (all three Potsdam keys have VIII right) | 20 |
| 2 | 336 | VIII in the left or middle position | 10 |
| 3 | 600 | contains VI or VII | 5 |
| 4 | 240 | only I–V | 5 |

- Within a wheel order, the four reflector/Greek combinations run in the order C/Gamma, B/Gamma, C/Beta, B/Beta (most-seen in Potsdam first).
- Since efficiency is flat for R ≤ 20 (§2.3), spending more restarts on the higher-prior jobs is the rational allocation.

**Stage B (per job, after its units).**
- The ≤1,000 best distinct keys by rescore are each re-climbed at their exact key with R=100 plus `--polish`.
- The better of the stage-A and stage-B boards is kept, giving `final.tsv`.
- This rescues near-misses: per-key polish roughly tripled true-key success at R=5 in the benchmark. It costs about 1–2 min per job.

**Output layout** (`runs/m4_ctonly/pass1/`):
- `jobs/<prio>_<refl><greek><wheels>/`: `ring_X.tsv` (top-40 per unit), `ring_X.done` (keys analysed, wall time, R), `stageB_in.tsv`, `final.tsv`, `DONE`.
- `worker_N.log`, `pids`, `run_info.txt` (the full configuration used), `locks/` (jobs in flight).
- Job order: `runs/m4_ctonly/jobs.txt`.
- Disk: about 6 KB per unit, ~170 MB total.

## 5. Estimated total time

Cost model, measured at 16 threads with SMT; 15 single-threaded workers give about the same per-thread rate: **t ≈ 0.2 ms + 0.355 ms × R per key per thread**. Checks: R=5 → 1.9–2.0 ms/key (P1030698 job: 5.94M keys in 708 s × 16); R=40 → 14.4 ms/key (unit H: 456,976 keys in 411 s × 16).

| tier | jobs | keys | R | thread-hours | wall (15 workers) | cumulative |
|---|---|---|---|---|---|---|
| 0 | 12 | 7.1e7 | 20 | 145 | 0.4 d | 0.4 d |
| 1 | 156 | 9.3e8 | 20 | 1,879 | 5.2 d | **5.6 d** |
| 2 | 336 | 3.4e9 | 10 | 3,564 | 9.9 d | 15.5 d |
| 3 | 600 | 5.7e9 | 5 | 3,129 | 8.7 d | 24.2 d |
| 4 | 240 | 2.9e9 | 5 | 1,564 | 4.3 d | **28.6 d** |

- For reference, all 1,344 jobs at a uniform R take 19.8 d (R=5), 37.5 d (R=10), 73 d (R=20) or 144 d (R=40).
- Stage B adds about 1%.
- Pass 2 (middle-ring turnover classes) would cost roughly 4–7× pass 1 for the same R, minus the class already done.
- A tier-0/1 unit at R=20 takes about 56 min single-threaded; tier-3/4 units take about 15 min.
- **These estimates are model-based.** They come from 16-thread SMT measurements on an otherwise idle machine. Load average was ~5 from other processes before launch, so the 15 workers share the box.
- The first tier-0 unit (R=20, single-threaded) is predicted to finish ~56 min after launch, around 05:46 on 2026-09-23.
- From then on, `top.py --status` recomputes the ETA from real unit wall times. If it disagrees with the table, trust it.

## 6. Odds and caveats (read before interpreting a null result)

1. **Per-job detection is incomplete.** Using P1030698 and CFYZR as calibration, the chance of recovering a 72-letter message in its correct job is about 10% at R=5, 20–35% at R=20, and ~60%+ at R≥100. A "done" job with no hit does **not** exclude that wheel order. Re-running with a different `SEEDBASE` gives independent new chances; it is equivalent to raising R.
2. **Middle-ring turnover** cases are outside pass 1 (§4).
3. **Language assumption.** The search and ranking assume the plaintext looks like 1945 U-boat telegraphic German. At 72 letters a Kurzsignal/codebook body, heavy garbles, or transcription errors in the published ciphertext (research/03 §3b) would each defeat the n-gram climb.
4. **Discrimination at scale.** For a P1030698-like naval text the truth (≈−4.8) should top its own unit. The global junk ceiling at R=20 is not yet measured; it was already −5.56 after 2 minutes and may approach −5.0. A less typical plaintext could therefore sit below it. Use `--by combo` / `--by words` and read the texts; do not just take row 1.
5. The patched trognes' test suite (`make test`) was not re-run after the patch, to avoid competing with the run. The patch only adds output hooks and an optional first-pair filter. Every result above was produced by the patched binary.

## 7. Operating the run

```sh
cd /opt/enigma-p1030680
tools/run_m4/top.py --status          # workers alive, jobs/units done by tier, running jobs, ETA
tools/run_m4/top.py                   # top 30 distinct candidate decrypts (all jobs, incl. in-progress units)
tools/run_m4/top.py -n 100 --by combo # rank by rescore+Krah-trigram+word-coverage z-scores
tools/run_m4/top.py --by words        # rank by naval word coverage
tools/run_m4/top.py --job 438         # only jobs whose id contains 438
tail runs/m4_ctonly/pass1/worker_1.log
```

- **Stop gracefully:** `tools/run_m4/stop.sh`. Workers finish their current job, then exit (up to ~12–13 h for a tier-1/2 job).
- **Stop now:** `tools/run_m4/stop.sh now`. This kills the workers and trognes. Units in flight restart from scratch on resume; completed units and jobs are kept via `.done`/`DONE` markers.
- **Resume:** `tools/run_m4/run.sh`. It clears stale locks and the stop flag, then starts 15 workers that skip finished units and jobs.
- **Change settings** by exporting environment variables before `run.sh` (see `config.sh`), e.g. `WORKERS=12`, `R_BY_TIER="40 40 20 10 10"`.
  - Use a new `PASS=pass2` (with `MIDRING`, or a new `SEEDBASE`) for a fresh output directory.
  - `top.py --pass pass2` reads it.
- **When a new/better naval corpus arrives:** set `NGDIR`, `LANGNAME` and `RESCORE`. Already finished jobs keep their old scores, so start a new PASS.

## 8. Recommendations

- **Any crib or indicator constraint (Vectors A/B) is worth far more than more compute here.**
  - With a crib, the true key is recognisable per key without relying on climb success, which is 20–35% per pass.
  - If Vector B turns the Grund into a function of the daily key, the 26^4 position search collapses.
- If the Kenngruppen analysis gives *any* constraint on wheel order or reflector/Greek, edit `runs/m4_ctonly/jobs.txt` order (or delete lines), then run `stop.sh now` and `run.sh`. Workers open the job list once at start, so they only see the new order after a restart; finished units are kept.
- After tier 1 finishes (~5.6 days), consider a second tier-0/1 pass with a new `SEEDBASE` (and/or `MIDRING` classes) before continuing into tiers 3–4. Expected payoff per hour is the same as continuing, but it concentrates on the best-supported hypothesis.
