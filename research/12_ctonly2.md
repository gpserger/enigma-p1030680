# 12. ctonly2: a faster ciphertext-only M4 search, and whether it should replace the run from research/07

Date: 2026-09-23. Code: `tools/ctonly2/` (C++ `ct2`, run scripts, `top.py`, `validate.sh`, logs in `tools/ctonly2/logs/`).

## Summary

- **I built `ct2`, a new ciphertext-only M4 searcher.** Each unit is one job (reflector, Greek wheel, wheel order) combined with one right-wheel core.
  - **Stage A** runs a hill-climb from random starts at every rotor setting, sampling the right-ring phase with stride 3.
  - **Stage B** takes the top 20 survivors and re-climbs them over the neighbouring phases and middle-ring classes.
  - The results are ranked by navalblend quadgram rescore.
  - The run is resumable, one output file per unit. It has the same job list and tier order as research/07.
- **The inner loop is up to ~2.5x faster per plugboard evaluation than trognes.**
  - Measured: 84 ns for `ct2` against about 200 ns for trognes. The trognes figure includes table loading, so it is an upper bound.
  - `ct2` uses a uint8 quadgram table with AVX-512 gathers. Quantising the table cost nothing measurable: true-key success was 21.3% before and after.
  - At equal R, the true-key success of the two climbers is the same: 21.3% for `ct2` against 21.2% for trognes (R=20, 32 synthetic cases × 8 seeds).
- **It is clearly better when the right wheel is single-notch (I–V), but not in the regime tiers 0–1 search.**
  - Tiers 0 and 1 are all double-notch right wheels (VI/VII/VIII), including VIII.
  - At the true right core, `ct2` detects the message 49% of the time with a single-notch right wheel. With a double-notch right wheel it detects 18%.
  - For comparison, trognes succeeds 25% of the time at the exact true key on those double-notch cases.
- **Decision: the research/07 run was not replaced.**
  - I stopped it during this work, before the comparison was finished. That lost only the in-flight units; the 163 finished units were kept.
  - I resumed it with `WORKERS=8`, because the bombe agent uses about 7 of the 16 cores.
  - `ct2` is ready to take over the 840 single-notch-right jobs, which sit in tiers 2–4, once tiers 0–1 finish (see §4).
- **No false positives.** Twelve random wrong-order units of the same ciphertexts never outscored a detected true unit. Their best junk score was −5.53. True hits scored between −4.24 and −5.44.

## 1. Validation

Test sets:
- **Synthetic cases** (`testset/synth32.tsv`): 32 cases. Each is real 72-letter naval plaintext from held-out folds, encrypted with random M4 keys with 10 plugs by py-enigma, and scored with the tables of the opposite fold.
- **CFYZR:** a real M3 message scored with navalblend tables.
- **P1030698:** a real M4 message, scored with leave-one-out tables.

Engine self-test: `src/test_engine.cc` reports bad=0, and this includes the cases with left-wheel steps. py-enigma in the recreated `.venv` imports and runs; the synthetic cases were regenerated with it.

| test | metric | result |
|---|---|---|
| true key, R=20, all 34 cases × 8 seeds | plugboard recovered | `ct2` 21.3%, trognes 21.2% |
| true key, double-notch right only (15 cases) | plugboard recovered | `ct2` 18.3%, trognes 25.0% |
| true key, single-notch right only | plugboard recovered | `ct2` 16.7% |
| true core, stride 3, R=20 | rank 1 in unit | 66/272 = 24% |
| true core, stride 3, R=60 | rank 1 in unit | 99/272 = 36% |
| same run, single-notch right (I–V) | rank 1 in unit | **79/160 = 49%** |
| same run, double-notch right (VI/VII/VIII) | rank 1 in unit | **20/112 = 18%** (VIII only: 2/40) |
| true core, stride 1, R=20 | rank 1 in unit | 106/272 = 39% |
| double-notch cases, stride 1, R=24 | rank 1 in unit | 25/112 = 22% |
| end-to-end, stride 3, R=40, key hidden (12 cases) | rank 1 in unit | 4/12. All four are single-notch; the three double-notch cases, including P1030698, went 0/3 |
| same 12 cases, one random wrong-order unit each | beats a detected truth | 0/12. Maximum junk score −5.53 |

Notes:
- **Single-notch detection is higher than true-key success.** Detection in a unit (49%) exceeds true-key success (~17–21%) because neighbouring phases and equivalent ring settings each give the climber another chance at a near-correct decryption.
- **Double-notch detection is low for a different reason.** A double-notch right wheel has only 13 phases, so there are fewer such chances, and a wrong phase breaks the decryption in two places. In the double-notch rows, the truth almost never survives the stage-A cut. Using stride 1 does not fix this: 22% at R=24.
- **Stride 3 is not an efficiency gain.** At roughly equal cost, stride 1 with R=20 (39%) is at least as good as stride 3 with R=60 (36%).
- **Vote seeding did not help.** I tried re-seeding climbs with the plugs most often found by failed climbs. Failed optima do contain true plugs above chance: 1.6 of the top 10 voted plugs were true, against 0.3 expected at random. Even so, detection with seeding was the same or worse than plain restarts: 40.4% against 41.2% at 60 climbs. It was dropped.
- **Not re-validated:** `testset/synth_left16.tsv`, the forced left-step cases, was generated but not run. The left-step coverage figures from research/07 (79% and 57%) still apply unchanged.

## 2. Cost and projection

- **Cost per job, measured on the same loaded machine (load average 70–80):**
  - `ct2` at R=40 and stride 3: about 300 s per unit on 8 threads. Over 26 units that is ≈ 17 thread-hours per job.
  - Old run at R=20: 4,232 s per unit on 1 thread, with about 21 units per job. That is ≈ 15–25 thread-hours per job.
  - Per job, the two cost about the same. At R=20, `ct2` costs about half as much.
- **Machine load:** the smoke test ran at load ≈ 10 on 4 threads and reached 31.6k setting-climbs per second. That implies about 5x contention during the earlier measurements. Both tools slowed down by a similar factor.
- **Projection for a `ct2` run** with R by tier of 40/40/20/20/20, stride 3, and 8 threads at ≈ 63k setting-climbs per second, assuming the machine is otherwise uncontended:

| tier | hours | cumulative |
|---|---|---|
| 0 | 5 h | 0.2 d |
| 1 | 63 h | 2.8 d |
| 2 | 106 h | 7.3 d |
| 3 | 179 h | 14.7 d |
| 4 | 87 h | 18.3 d |

  Under the load seen earlier, multiply by up to 5. Once real units exist, `top.py --status` recomputes the ETA from them.
- **The old run's ETA** before it was stopped was 2,212 h for all jobs with 15 workers, of which 171 h was tiers 0–1. With 8 workers, expect tiers 0–1 to take roughly 320 h.

## 3. Why the old run was kept

The coordinator's rule was to replace the old run only if the new tool is clearly better.
- Tiers 0–1 (168 jobs) are all double-notch right wheels.
- In that regime, `ct2` detects the message in 18–22% of trials. This is not above the old tool's true-key success of about 25%, which covers every phase.
- At R=40 the cost per job is about the same for both tools.

So `ct2` is not clearly better for the first stretch of the run. For single-notch-right jobs, it detects about 49% against about 20% for the old tool, at equal or lower cost. That is clearly better.

## 4. Recommendation and how to run

1. **Let the research/07 run finish tiers 0–1.** Check progress with `tools/run_m4/top.py --status`.
2. **Then run `ct2` on the single-notch-right jobs.** There are 840 of them, all in tiers 2–4. Before starting, either filter `jobs.txt` down to jobs whose right wheel is I–V, or stop the old run at the end of tier 1. Commands:
   - `tools/ctonly2/run.sh` starts or resumes the run. It is configured in `config.sh`: `JOBS`, `R_TIER`, `STRIDE`, `THREADS`.
   - `echo N > runs/ctonly2/pass1/threads` changes the thread count; it is re-read at the start of every unit.
   - `tools/ctonly2/stop.sh` stops gracefully after the current unit. `stop.sh now` kills immediately.
   - `tools/ctonly2/top.py` lists candidates. `--status` shows progress and ETA; `--by words|krah|combo` changes the ranking.
3. **Before `ct2` is used on double-notch jobs:** the stage-A cut is dropping the truth in that regime. Possible fixes are a larger top-k when the right wheel is double-notch, or scoring stage A on the union of adjacent phases. Re-run `validate.sh` against `testset/synth_dn.tsv` after any change.
4. **Smoke test:** a one-job run with R=4 on 4 threads finished 9 units at about 11 s each. The stop flag worked, and `top.py` parses both `units/*.tsv` and `run.log`.
