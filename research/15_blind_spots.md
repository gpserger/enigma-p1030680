# 15 — Two blind spots: capped short-crib rankings, and ct2 stage A on double-notch right wheels

Date: 2026-09-25. Resource rule followed throughout: at most 2 threads for anything run here, jobs run one after another.
Code:
- `tools/blind/blindrank.cc`: stop prefilter + full ranking.
- `tools/blind/bombe_ah.cc`: the bombe with the all-hypotheses fix.
- `tools/blind/gen_calib.py`, `calib_run.sh`, `calib_analyze.py`: prefilter calibration.
- `tools/blind/gen_paired.py`: paired single-/double-notch testset.
- `tools/ctonly2/dn/ct2dn.cc` + `dn/fast_dn.h` → binary `tools/ctonly2/ct2dn`.

Run scripts and outputs are in `runs/blind/`; Spot 2 outputs are in `runs/blind/s2/`.
Nothing running was touched: `runs/m4_ctonly`, `runs/ctx_excl`, `runs/r14`, `tools/bombe/bombe` and `tools/ctonly2/ct2` are all unchanged.

## Summary

**Spot 1 (capped rankings).**
- Two families had been ranked only in part:
  - F4 "An alle": 87,019,261 verified stops, of which 2.0 M (2.3%) were ranked;
  - `sender_fxdx` ("VVV/VON FXDXUUUAUSBXX", research/08 step 3b): about 160 M verified stops, of which 20 M (12.5%) were ranked. Its stop file had been deleted.
- `end_w22_k1` (35.6 M stops, 56% ranked) is also capped. research/08 already marked it "not diagnostic", and it is still not re-run (§1.6).
- **Every F4 stop has now been scored.** A cheap language statistic (`llr2`) was computed on all 87.0 M. The top 13.75% (11.96 M stops) were hill-climbed with 8 restarts, twice the old ranker's 4.
- **No stop reproduces its crib after the climb** (crib mismatch 0 or 1 = none; the best is mismatch 2).
- **No decrypt reads as German.** The best rest-of-message quadgram score is −6.6 to −7.5; real text scores about −4.8.
- **A second, more serious blind spot turned up in the bombe itself (§1.3).**
  - With weak menus, the block test keeps only the first surviving pivot hypothesis. At the true setting that hypothesis is often wrong, the stop then fails verification, and at `--gmax 0` the true key is simply never written.
  - Measured on 48 synthetic weak-menu cases: 7 of 48 (15%) lost.
  - Fixed in `bombe_ah --allhyp`, which loses 0 of 48 at the same bombe time, with 46% more stops.
- **Both reruns with the fix are done.**
  - `sender_fxdx`: 163.8 M stops, all looked at, 18.3 M climbed. 317 stops reproduce the 16-letter crib, but all are junk (best −7.6).
  - F4 complement: 38.0 M stops the old bombe had silently dropped, 4.95 M climbed. The best crib mismatch is 3.
- **Neither contains anything German.** `end_w22_k1` is re-running the same way (addendum A3).

**Spot 2 (ct2 on double-notch right wheels).**
- **Diagnosis.**
  - Measured at the true cores, a single climb is not worse for a double-notch right wheel: it succeeds 2.7% of the time against 2.2% for single-notch.
  - What differs is what happens one phase off. The middle wheel turns over about twice as often, so a ±1 right-phase error garbles 11–12 letters with a double-notch wheel, against 4.5–7 with a single-notch one. Climbs from those phases succeed half as often.
  - A double-notch unit also samples only 5 phases, against 9 for single-notch.
  - With stride 3, two of every three sampled phases are ±1 off, so the double-notch unit loses both its "neighbour chances" and total climbs.
- **The 18% vs 49% gap in research/12 is mostly test-set composition.** On a paired testset (identical keys and plaintexts, only the right wheel swapped), detection at the true cores is 48% for single-notch and 39% for double-notch (excluding the two real vectors, 40%). End-to-end (key hidden, rank 1 in the true unit), the paired twins give 8/16 single-notch against 6/16 double-notch.
- **Fixes tested at the same cost.**
  - Turnover-masked objective: worse (23%, from 39%).
  - Stride 1 with R=15: 44% at the true cores, but end-to-end 5/16 against 6/16 and 2/14 against 2/14.
  - Nothing beats "spend more climbs". R=72 on double-notch units (1.8× their cost, equal to a single-notch unit) gives 45% at the true cores.
  - **Honest conclusion: there is no free fix. Double-notch detection is about 0.8× single-notch at equal R, and the only lever is R.** A switch (`--RDN`, `--strideDN`, `--topkDN`, `--mask`) is in `ct2dn`, ready to use.

---

## 1. Blind spot 1: capped ranking of short-crib stops

### 1.1 Inventory: stops versus ranked, per log

"Verified stops" are the lines written with g ≤ gmax; "block stops" is the raw count in the log. The old ranker kept the first `--maxhits` verified stops **in enumeration order**, so the ranked part is the first jobs (B-thin/Beta first), not a random sample.

| family | log | verified stops | ranked | capped? | stop file |
|---|---|---|---|---|---|
| F1 `tf_von_fdua_k1` | runs/ctx/out | 80 | 80 | no | – |
| F2 `fdua_von_x_k1` | runs/ctx/out | 153,636 | 153,636 | no | – |
| F3, F5, R, T01 | runs/ctx/out | 0 / 0 / 0 / 12 | all | no | – |
| **F4 `alle_k0`** | runs/ctx/out | **87,019,261** | **2,000,442 (2.3%)** | **yes** | kept (5.0 GB, `runs/ctx/out/F4_alle_k0.tsv`) |
| hdr_fxdx, hdr_komx, body_b41, end_w22_k0, hdr_end | runs/bombe/step3 | ≤ 0.77 M each | all | no | – |
| **sender_fxdx** (VVV/VON+FXDXUUUAUSBXX, 27 crib-offsets) | runs/bombe/step3 | ~**160 M** (g0 histogram sum) | **20,004,302 (12.5%)** | **yes** | deleted → re-run |
| end_w22_k1 (`WEITEREBEFEHLEABWARTEN`@48–50, k=1) | runs/bombe/step3 | ~35.6 M | 20,001,098 (56%) | yes | deleted; see §1.6 |

- The running `runs/ctx_excl` A1 job uses `--climb … --maxhits 20000000`, so it can hit the same cap. It was not touched.
- The F4 stops are dominated by two nested cribs, `ALLEVVVZEHNXSIDIX(X)`:
  - they account for 84.7 M of the 87 M stops;
  - their tested menu component has 3 letters, 4 edges and 2 loops.
- In `sender_fxdx`, `VON…@3/@17/@10/@19` (components of 3–8 letters, 1–3 loops) give about 160 M stops.

### 1.2 The prefilter

For each stop, `blindrank` takes the key and the pinned partial plugboard from the bombe TSV. For every non-crib position where both the cipher letter and the scrambler output are pinned (a "determined" position), it computes the plaintext letter. The null hypothesis for a wrong key is that such letters are uniform over the pinned letter set.

Statistics tried:

| statistic | definition |
|---|---|
| `llr1` | Σ over determined positions of log10(P(p │ p ∈ pinned) / (1/│pinned│)), unigram model |
| **`llr2`** | the same with a first-order Markov (bigram-conditional) model restricted to the pinned set; the unigram term is used when the previous position is undetermined |
| `pt_bi`, `pt_quad`, `pt_ic` | pass-through decrypt (unpinned letters = identity), scored by bigram, quadgram or IC |

The statistic uses the navalblend tables for production, and held-out fold tables (or `navalloo` for P1030698) in calibration.

Cost: 1.9 µs per stop per thread after switching to a lazy per-letter scrambler (12× faster than a full path table). A climb costs about 1.4 ms per stop at R=8.

### 1.3 Calibration, and the bombe's first-hypothesis blind spot

**Calibration set.** 48 synthetic M4 cases (`gen_calib.py`), plus 4 P1030698 cribs:
- random key, 10 plugs, middle ring A so that the pinned space contains the key;
- real naval plaintexts from held-out folds;
- the crib is the first 16–18 plaintext letters at offset 0 (the address block);
- cases were chosen to match the production menus: tested component ≤ 8 letters, 1–3 loops.
- Each case runs the true job plus one random wrong order, with the same bombe settings as production (k=0, pinned).
- In total 28.7 M stops (41.8 M with the fix). The per-case stop counts (3k–2.1M per 2 jobs) span the production range.

Results:

| | original bombe | `bombe_ah --allhyp` |
|---|---|---|
| cases where the exact true key is among the stops | **41/48** | **48/48** |
| cases with a recoverable true stop (climb R=4 → crib mismatch 0 and ≥ 66/72 letters right) | 38/48 | 45/48 |
| total stops | 28.7 M | 41.8 M (+46%) |
| bombe wall time (sum over the 48 cases) | 288 s | 272 s |

**The 7 lost cases.** `bombe --key <true key> --gmax 99` shows that the true setting does stop, but with g = 1, 2 or 4.
- `block_test` returns the first surviving pivot hypothesis only. With 1–2 loops, a wrong hypothesis with a lower letter often survives too.
- Its board fails the whole-crib verification, and at `--gmax 0` the stop is not written.
- `--allhyp` verifies every surviving hypothesis and keeps the minimum g. It restores all 7 cases, for example `0 C-thin Beta 785 DATK AAAH g=0`.
- **Every short-crib family run so far at k=0 (and those at k=1 at gmax=k) carries this loss, about 15% for weak menus.** The loss is smaller for strong menus, where wrong hypotheses rarely survive.

**Retention of recoverable true stops versus keep fraction.** Thresholds are per crib (flat). Stratifying per (crib, npairs) was worse, because npairs itself is informative: true stops have npairs 6–10 and 23–53 determined positions.

| keep fraction | 0.5% | 1% | 2% | 5% | **10%** | 15% | 20% |
|---|---|---|---|---|---|---|---|
| llr2, original-bombe set (n=42, incl. P1030698) | 0.88 | 0.93 | 0.93 | 0.95 | **0.976** | 0.976 | 1.00 |
| llr2, allhyp set (n=45) | 0.91 | 0.93 | 0.93 | 0.96 | **0.98** | 1.00 | 1.00 |
| pt_ic (best pass-through statistic), allhyp set | 0.56 | 0.60 | 0.69 | 0.87 | 0.91 | 0.93 | 0.93 |
| llr2 ∪ pt_ic (f/2 each) | – | – | 0.93 | 0.95 | 0.98 | 1.00 | 1.00 |

- **Chosen: `llr2`, keep 10% per crib. The filter loses about 2% of true keys, under the 5% target.**
  - The residual losses are stops with only 2 pinned pairs and no determined positions. The statistic cannot see those at all.
  - The P1030698 cribs (`TRAVEMUENDEBLE@30`, `BLEIBENXWEITER@41`, `FXDXUUUXAUSBXX@16`, `TRAVEMUENDEBLEIB@30`) need a keep fraction of ≤ 1%.
- End-to-end rehearsal on P1030698: `TRAVEMUENDEBLE@30`, 323,079 stops over 2 jobs. The top 10% was climbed in 38 s, and the true decrypt ranked #1 at −4.92, against −6.79 for #2.
- **The binding factor is not the filter.** It is the bombe (15% lost, now fixed) and the fill-in climb: 3 of 48 true stops do not climb to the plaintext at R=4, which is why production uses R=8.

Chance that a weak-menu k=0 family with the right crib would have been found:

| setup | chance |
|---|---|
| before (first-hypothesis bombe, capped ranking) | 38/48 × ~2–12% ≈ **2–10%** |
| now, filter on existing stops (F4) | 38/48 × 0.98 ≈ **78%** |
| now, with the fix (sender re-run, F4 complement) | 45/48 × 0.98 ≈ **92%** |

### 1.4 F4 "An alle", all 87,019,261 stops ranked

Command: `runs/blind/rank_F4.sh`.
- Pass 1 is a histogram of `llr2` per crib (4,542 s).
- Pass 2 climbs every stop at or above the per-crib 90th percentile, including the whole tie bin.
- Result: 11,962,876 stops climbed (13.75%) in 16,333 s total on 2 threads.
- Per crib, seen/climbed:
  - crib 0 `ALLEVVVZEHNXSIDIX`: 47.3 M / 6.42 M;
  - crib 1 `…X`: 37.3 M / 5.29 M;
  - crib 5: 2.26 M / 240 k;
  - crib 10: 76 k / 8.3 k;
  - crib 17: 17 k / 1.7 k;
  - cribs 2–4: 15 stops, 4 climbed.
- Post-climb crib-mismatch histogram: mm2:3, mm3:14, mm4:145, mm5:1,454, …; **mm0 = mm1 = 0**.

**No stop passes the acceptance test (mismatch ≤ k = 0), nor even ≤ 1.** The best candidates (`runs/blind/F4_full.rank.tsv`, top 300):

| rest-quad | crib mm | crib | key (refl Greek wheels pos rings) | decrypt |
|---|---|---|---|---|
| −7.475 | 2 | ALLEVVVZEHNXSIDIX | C-thin Gamma 358 LKWD AAAN | ALLEVICZEHNXSIDIXJPJKTHDINGSGEIOMINRSRERCBPOWIDIEKZPWZRAFHLBUMVPADHORMDS |
| −7.658 | 2 | ALLEVVVFXDXUUUOSTX(X) | C-thin Beta 826 FLRK AAAR | ALEEVVVUXDXUUUOSTXXVORVDQFADELDNUYGOHRESYDTXLMXLSYUUMJTTJHSNXKAXGSINZUDV |
| −6.952 | 3 | ALLEVVVZEHNXSIDIX | B-thin Beta 347 XVAB AAAV | AELEVVXVEHNXSIDIXRRIGBATIPDHALTABLEBAGDUZEUMTMPVQUEDRIINZUVSNIEDVZQPTTKE |
| −7.281 | 3 | ALLEVVVZEHNXSIDIX | C-thin Beta 621 PYCI AAAB | ALLEVNLZEHNTSIDIXLLYANWNBWTINAZBEGFFPIRSIWXVIAEYSDANLDNUDPHENFLIIFFRIPOO |
| −7.316 | 3 | ALLEVVVZEHNXSIDIX | B-thin Gamma 763 JJYH AAAR | SLLEVVVZEHNXSRDIVJPUQNDFILFIGLTMLIEZUMOTONZJFTVOJUBGETAALTEALQJCADWMTBSN |
| −7.413 | 3 | ALLEVVVZEHNXSIDIX | B-thin Beta 123 XDSF AAAP | ALLEVVVZEHNKSIDHENDCZULBXEXGABKKSCDCERZKFXFJAPQNNZDIMIESROLDSCOZRUBUNNON |
| −7.438 | 3 | ALLEVVVZEHNXSIDIX | B-thin Beta 743 ILWK AAAI | ALLEVVVZEHNXSILALEGIZILALFXNNQXVQDUHEUXZYUHESEIGGWEIHFHAHLEMJUUIRIQFUNNU |
| −6.598 (best rest, mm 5) | 5 | ALLEVVVZEHNXSIDIXX | – | ALLEVLVZEHNOSMSXXXXIAFORDGILLITTELNZCVDTBOELPRDESDENHTPATMIKEYJTSAMAFXJJ |

- **Nothing reads as German**; there are only chance fragments.
- For comparison, the true P1030698 decrypt in the same pipeline scores rest-quad −4.92 with mismatch 0.
- Verdict: **the "An alle / Alle von 10. Sidi / FdU Ausb / 5. Flott" address family (F4 cribs) at offset 0, k=0, is now null over every stop the original bombe wrote.**
  - The residual chance that it was missed: filter about 2%, climb about 5%, and the bombe's first-hypothesis loss about 15% (being closed by the complement run, §1.5).
  - Middle-ring turnover inside the crib: pinned mode, as in research/11.
- `runs/ctx/out/F4_alle_k0.tsv` (5.0 GB) is no longer needed once the complement run has finished. It was left in place because it is not a `runs/blind` file; delete it to reclaim disk.

### 1.5 `sender_fxdx` re-run and the F4 complement (both with `bombe_ah`)

- `runs/blind/rank_sender.sh`:
  - pilot on 8 wheel orders × 4 (reflector, Greek) → per-crib `llr2` thresholds for keep 10% (`sender.thr`);
  - then the full M4 space with `bombe_ah --allhyp --threads 1`, streamed (`--out -`) into `blindrank rank --thr … --threads 1` (2 threads total);
  - no stop file is written to disk.
- `runs/blind/rank_F4_new.sh`: `bombe_ah --allhyp-new`, which writes **only** the stops the old bombe dropped, streamed into the same filter with thresholds from the full F4 file (`F4.thr`).

Results: addendum A1 (sender) and A2 (F4 complement).

### 1.6 Not done

- `end_w22_k1` (35.6 M stops, 56% ranked) was not re-run. research/08 called it non-diagnostic (11-letter blocks, P1030698 ending wording already dead at k=0). Scripts exist if wanted: `rank_sender.sh` with the cribs replaced.
- Middle ring: all runs are pinned, as in the originals.

---

## 2. Blind spot 2: ct2 stage A on double-notch right wheels

### 2.1 What research/12's numbers actually measure

- The "true cores" trials run stage A only at the true Greek/left/middle cores, so the unit's top-K cut does not enter them.
- In failed double-notch cases, the logged stage-A best is −6.2 to −6.6, below the unit cutoff of about −5.95. **The failure is at climb level, not ranking.**
- End-to-end confirms this. On the paired double-notch cases, rank-1-in-unit (6/16 = 38%) almost equals true-core detection (39%), so the stage-A top-20 cut costs little.

### 2.2 Paired testset

`tools/ctonly2/testset/paired_sd.tsv` has 32 pairs:
- same plaintext (16 held-out messages × 2 keys), plugs, reflector, Greek wheel, left and middle wheels, rings and positions;
- the right wheel is from I–V in one case of the pair and from VI–VIII in the other;
- no left step inside the message for either.

### 2.3 Diagnosis: per-phase single-climb success at the true cores (`ct2dn phasescan`)

Setup: 80 independent single climbs per right phase, nostep middle class as in stage A. Success means at least 8 of the 10 true plugs recovered, so stage B converges. d is the signed right-phase error; "garbled" is the number of letters that are wrong under the true board at that phase.

| d | SN garbled | SN success/climb | DN garbled | DN success/climb |
|---|---|---|---|---|
| 0 | 0 | 2.15% | 0 | **2.66%** |
| −1 | 4.5 | 1.37% | **11.0** | 0.90% |
| +1 | 6.8 | 1.91% | **12.3** | 1.21% |
| −2 | 8.8 | 0.94% | 15.2 | 0.59% |
| +2 | 9.2 | 1.45% | 19.2 | 0.39% |
| │d│ ≥ 3 | ~28 | 0.12–0.23% | ~30 | 0.03–0.06% |

1. **At the exact phase, double-notch is not harder.**
2. **A phase error of 1 garbles about 2.2× as many letters with a double-notch right wheel.** The middle wheel steps every 13 letters instead of every 26, so about 5.5 turnovers fall inside 72 letters instead of about 2.8, and each misplaced turnover costs about 2 letters.
3. **Off-phase climbs therefore succeed about 35–50% less often.**
4. **Stride 3 samples a phase that is exact only 1 time in 3.** The expected number of successful climbs per unit is R × Σ over the sampled phases of p(d):
   - single-notch (9 samples of 26): ≈ R × 0.030;
   - double-notch (5 samples of 13): ≈ R × 0.018.
   - Ratio ≈ 0.6 at equal R per setting. The double-notch unit costs 5/9 of the single-notch one.

This is the whole mechanism:
- a stepping-driven garble penalty on the off-phase samples;
- fewer phases and fewer climbs per unit;
- per unit cost, fewer total chances.

It is **not** a scoring problem at the exact key, and not the top-K cut.
- Left-wheel stepping is a separate factor. With a double-notch right wheel the middle wheel advances twice as often, so a left step inside the message (which stage A's nostep class misses) is about twice as likely: about 21% with a single-notch middle wheel, about 42% with a double-notch one. The paired set excludes this case. research/07's left-step coverage figures apply.

### 2.4 Fixes tested (true cores: stage A + B, 8 seeds; end-to-end: full unit, key hidden)

| configuration (DN units; SN unchanged) | stage-A cost vs baseline DN | paired: SN true-core | paired: DN true-core | synth_dn: DN true-core | end-to-end paired DN (16) | end-to-end synth_dn (14) |
|---|---|---|---|---|---|---|
| baseline: stride 3, R=40 | 1.0 | 128/264 = 48% | **104/264 = 39%** | 20/112 = 18% | **6/16** | **2/14** |
| turnover-masked objective (`--mask 2`), R=40 | 1.0 | – | 60/264 = 23% | 19/112 = 17% | – | – |
| mask + R=72 | 1.8 | – | 74/264 = 28% | 22/112 = 20% | – | – |
| stride 1, R=15 (`--strideDN 1 --RDN 15`) | 1.04 | – | 116/264 = 44% | 23/112 = 21% | 5/16 | 2/14 |
| R=72 (`--RDN 72`, same cost as an SN unit) | 1.8 | – | 118/264 = 45% | 19/112 = 17% | 7/16 | 2/14 |
| *reference: paired SN twins, baseline R=40* | (SN unit = 1.8) | | | | *8/16* | |

- The SN column includes CFYZR 6/8; the DN columns include P1030698 2/8. Excluding them, paired SN is 122/256 = 48% and paired DN is 102/256 = 40%.

Findings:
- **The masked objective fails.** Dropping the windows around the ~11 ambiguous letters removes about 27 of 69 quad windows. That costs more at the exact phase (success/climb falls from 2.66% to 0.27%) than it gains off-phase.
- **Stride 1 at equal cost** gives +5 points at the true cores, but nothing end-to-end (5/16 against 6/16, 2/14 against 2/14). Within noise, it is not a fix.
- **More R works roughly as per-climb statistics predict.** It is a cost, not a fix: 1.8× gives +6 points at the true cores on the paired set.
- **The synth_dn "18%" is a hard-case set.** Its plaintexts (P1030661, 693, 696, 695, 663) are hard for single-notch keys too: in the paired run, P1030667, 686, 695 and 693 give 0–1 of 16 with single-notch.

**Recommendation.**
- Do not expect a free multiplier on double-notch units. At equal R, expect detection about 0.8× that of a single-notch unit, and about 0.6× at equal per-unit cost.
- If double-notch jobs get ct2, give them `--RDN 72` (unit cost = a single-notch unit's).
- Keep stride 3; `--strideDN 1 --RDN 15` is equivalent within noise.

**Ready-to-use switch.**
- `tools/ctonly2/ct2dn` is a drop-in superset of `ct2`: same `run` and `validate` modes, identical behaviour with default flags.
- New flags: `--RDN R`, `--strideDN S`, `--topkDN K`, `--mask 0|1|2`, and the `phasescan` diagnostic mode.
- To use it: `CT2=tools/ctonly2/ct2dn` plus extra flags in `tools/ctonly2/run.sh`.
- It was not deployed.

Cost measured on this machine (2 threads, loaded): 165 s per double-notch unit at R=40 stride 3, and 150–170 s for stride 1 at R=15.

---

## Addendum (filled in after the queued runs)

### A1. `sender_fxdx` re-run: complete, all stops looked at (with the all-hypotheses fix)

- **Bombe.** `bombe_ah --allhyp`, full M4 space, pinned middle ring, 1 thread. It took 40,298 s and wrote 163,825,024 verified stops. The original run had about 160.3 M g0 stops, so the fix added about 2% here.
- **Filter.** The `llr2` thresholds came from the pilot run (32 jobs). Strata with fewer than 1,000 pilot stops were climbed in full.
- **Climb.** 18,255,523 stops were climbed (11.1%) at R=8, streamed on 1 thread.
  - The four weak crib-offsets carry the load: VON@3 had 100.7 M stops (11.7 M climbed), VON@17 43.7 M (4.5 M), VON@10 15.9 M (1.6 M) and VON@19 3.2 M (0.35 M).
  - Every other crib-offset was climbed in full.
- Post-climb crib mismatch counts: mm0 317, mm1 2,857, mm2 8,126.
- **317 stops reproduce the 16-letter crib exactly after the climb, so the acceptance test alone is not decisive for a crib this short.** All of them read as junk. The best rest-quadgram score is −7.60, against about −4.8 for real text and −4.92 for the P1030698 rehearsal.

| rest-quad | mm | crib@off | key | decrypt |
|---|---|---|---|---|
| −7.604 | 0 | VON…@4 | C-thin Beta 642 NANB AAAH | YHDAVONFXDXUUUAUSBXXQENMVNXLKOWAQCOLNVOMVIPKCEEXOQLUULICLCITMIFFVOASESCD |
| −7.624 | 0 | VON…@23 | C-thin Gamma 142 ZKPW AAAU | MIHTGMDBVINKNZESMRSEKOXVONFXDXUUUAUSBXXZATUMAEIFMGAKFALAIFIUQGCHINGRSUJG |
| −7.625 | 0 | VVV…@5 | C-thin Gamma 465 XPFO AAAM | EGFMCVVVFXDXUUUAUSBXXTRALJLSPYNAPDMECSQYXSTRBEWILVQUSHVWMINGUJQDRLLWIDAO |
| −7.680 | 0 | VVV…@10 | B-thin Beta 135 FPRH AAAL | AVCATNLLJBVVVFXDXUUUAUSBXXUXHLIGENHAFERGJTHRUNGCKGOZZJLCCFBBREMWOUYDXUIY |
| −7.755 | 0 | VVV…@18 | C-thin Gamma 364 CEOH AAAK | UHABREQVLOBFJIHEVEVVVFXDXUUUAUSBXXTRDVYQXPCUZQHQYOVOKZRRYPONWINTPFQLLDAS |
| −6.821 (best rest overall) | 2 | VVV…@3 | – | BIUVVVFXDXFKUAUSBXXXBGVMLHONUHESGEMEFUAMANYTJOXNSKIWITISERVALDZUWONLEUPA |

**Verdict.** FdU Ausb as sender after `VVV` or `VON` at offsets 3–25 (the research/08 step 3b family) is now **null over every stop, with the bombe fix applied**.
- Residual miss chance: filter about 2% and climb about 5%.
- A middle-ring turnover inside the crib is not covered, because the run used pinned mode.
- Files: `runs/blind/sender_full.{rank,surv}.tsv`, `sender_full.log`, `sender_full.bombe.log`.

### A2. F4 complement: the stops the original bombe dropped (`bombe_ah --allhyp-new`)

- **The old F4 bombe dropped a large share of the true-capable stops.** The complement contains 37,951,706 stops: settings where the first surviving pivot hypothesis failed verification but a later one passes with g = 0. That is 44% on top of the 87.0 M the old run wrote.
  - Almost all come from the two weak `ALLEVVVZEHNXSIDIX(X)` cribs: 20.8 M and 17.1 M.
  - The bombe took 37,729 s on 1 thread.
- **Filter.** The `llr2` per-crib thresholds were taken from the full F4 file, at keep 10%.
- **Climb.** 4,950,711 stops were climbed (13.0%) at R=8.
- Post-climb crib mismatch: **no stop reached mismatch ≤ 2.** The best was mm3 (5 stops), and the best rest-quadgram score was −7.25 (junk).
- Top decrypt: `AOQEVVVZEGNXSIDIXDCHRDIPTRLBGEFYEUTFRASAFLVGZERPOKTWKNNNULUTREVIWIWFMBEG`.
- **With A2 the F4 family is now covered with the bombe fix as well.** Between them, the two runs looked at 87.0 M + 38.0 M = 125 M stops. Nothing is German.
- Raw F4 stop file: compressed to `runs/ctx/out/F4_alle_k0.tsv.zst` (461 MB, from 5.0 GB). Calibration per-stop stats are zstd-compressed in `runs/blind/cal_*`.

### A3. end_w22_k1 re-run (`WEITEREBEFEHLEABWARTEN`@48,49,50, k=1, with the fix)

- **Bombe.** `bombe_ah --allhyp`, full space, pinned middle ring. It wrote 42,472,367 verified stops (g ≤ 1). The original run had about 35.6 M, so the fix added about 19%.
- **Filter and climb.** `llr2` at keep 10%; 4,488,715 stops were climbed at R=8, in 10,817 s on 2 threads in total.
- **Result.** Post-climb crib mismatch: the best is **6 of 22**, and there are no survivors at mm ≤ 3.
- The P1030698 ending "WEITERE BEFEHLE ABWARTEN" at offsets 48–50 with up to 1 garble is null over every stop.

**All three capped families are now fully looked at.** No bombe stop in them reads as German.

## Disk and threads

- Raw stop files were never written to `runs/blind`; the reruns streamed from the bombe into the ranker.
- The F4 raw file is compressed to `.zst` (5.0 GB → 461 MB). The calibration stats are compressed too (5.1 GB → about 1 GB).
- Every job used at most 2 threads: bombe 1 + ranker 1 in the streamed runs, and 2 in the others. Jobs ran one after another.
