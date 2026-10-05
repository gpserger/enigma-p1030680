# 08 — Vector D: garble-tolerant software bombe for M4, and the P1030698 re-broadcast test

Date: 2026-09-23. Target: `ciphertext.txt` (P1030680, 72 letters).
Tool: `tools/bombe/bombe` (source `tools/bombe/bombe.cc`, validator `tools/bombe/validate.py`).
Run outputs: `runs/bombe/`. Machine: Ryzen 7 7800X3D, 16 threads.

## TL;DR

- **The P1030698 re-broadcast hypothesis is dead.** The P1030698 text was run as a crib over the whole M4 space in every alignment variant listed below, with up to 3 unknown garbles. The search included the middle-ring turnover classes and covered 6.96×10^10 rotor settings. Result:
  - **zero survivors at any k ≤ 3**;
  - only 11 raw block stops in total, all at g ≥ 32 implied garbles;
  - on its own ciphertext, the same search recovers P1030698's key with 0 and 2 garbles (natural and artificial), and the true key is the only thing that stops (the other stops are right-ring neighbours of the true key).
- **The bombe works and is fast:**
  - It uses AVX-512 VBMI and runs all 26 pivot hypotheses at once.
  - Each crib is cut into k+1 disjoint blocks chosen to maximise menu loops, so by pigeonhole the true setting always stops.
  - A 72-letter crib at k=3 clears the whole M4 space in **2.8 min** with the middle ring pinned, and in about 15 min with all middle-ring turnover classes, on 16 threads.
  - Validation passed: 21/21 synthetic py-enigma cases (left wheel stepping inside the message, double-notch right wheel, 0–3 garbles) and P1030698 rehearsals (3 cribs in 2 runs).
- **Step 3 (shorter cribs) found nothing.** No decrypt reads as German.
  - The best rest-of-message quadgram score is −6.75. Real 72-letter naval plaintext scores about −4.8 with the same tables.
  - The body `TRAVEMUENDEBLEIBENXWEITEREBEFEHLEABWARTEN` at all 32 offsets (k ≤ 2) produced no setting that reproduces the crib.
  - The header+ending template and `WEITEREBEFEHLEABWARTEN` at offsets 40–50 (k=0) are also null.
- **Background run:** it was stopped once, for 1 h 57 min, and resumed at 11:14. `top.py --status` shows 15 workers alive; 119 units were done at the time of writing.

---

## 1. Bombe design (`tools/bombe/bombe.cc`)

### Scrambler

- The scrambler reuses `tools/run_m4/src/core.h`: py-enigma wirings, double stepping, and the ring semantics validated in research/07.
- For each (reflector, Greek, left, middle) it precomputes an inner permutation U[g][l][m][x] (Greek, left, middle wheels, reflector and back; 457 KB per job).
- The right wheel is applied through shifted tables, so T_i(x) = RB[s_i][U[g][l_i][m_i][RF[s_i][x]]].
- Self-test (`bombe --selftest`): 0 mismatches against `make_path` in 1.25 M comparisons, over 400 random keys with random rings, including forced left-wheel steps.

### Enumeration

Per job (thin reflector B/C × Beta/Gamma × 336 wheel orders), the tool enumerates core offsets rather than (position, ring) pairs:
- Greek core: 26 (Greek ring = A).
- Left core: 26 (left ring = A; both are degenerate with position).
- Right core: 26 × right window at start.
  - The right window takes 26 values for single-notch wheels.
  - It takes 13 for VI/VII/VIII, using the (window+13, same core) equivalence.
- Middle core: 26 × middle-stepping class.
  - `--midring pinned` sets middle ring = A, the O&W shortcut used by research/07 pass 1.
  - `--midring all` enumerates every distinct (middle, left) stepping pattern over the message that a middle ring can produce.
  - This is the exact set of middle-ring turnover classes. It costs 5.4× pinned over the whole space (6.96e10 vs 1.30e10 settings).
- The output key is written as positions GLMR plus rings `AA<rM><rR>`, directly usable in py-enigma.

### Test per setting (Turing–Welchman with diagonal board)

1. **Forced garbles.** Crib positions where crib letter = cipher letter are excluded; if the crib is right, those positions must be garbles.
2. **Blocks.** The remaining positions are partitioned into B = k+1 disjoint blocks. With at most k further garbles, at least one block is clean (pigeonhole), so the true setting stops in it.
   - The partition is not contiguous. A local search picks it to maximise the loop count (cycle rank) of each block's largest menu component.
   - Contiguous blocks of a 72-letter crib had only 0–3 loops each; optimised blocks have 7–10. This made the test about 10× cheaper and removed false stops.
3. **Vector prefilter.** Each block is tested with one AVX-512 VBMI byte lane per pivot hypothesis (all 26 at once).
   - Values propagate along a spanning tree of the menu with `vpermb`, and every loop closure is checked (`cmpeq`).
   - The order is greedy so that loops close as early as possible.
4. **Scalar test.** Surviving hypotheses go to the scalar test:
   - full propagation with the involutive plugboard (diagonal board);
   - rejection on any contradiction or on more than `--maxplugs 10` pairs.
   - Checked on a whole job: identical output to the scalar-only path (`--scalar`).
5. **Verification.** For every stop, the block's plugboard is extended over the whole crib by majority vote. The number g of inconsistent crib positions (implied garbles) and the number of still-undetermined positions are reported.
   - The true setting shows g ≈ number of garbles.
   - Right-ring and turnover neighbours of the true key (same wheels and core, middle turnover 1–3 letters off) show g ≈ 2–18.
   - Genuinely wrong settings showed g ≥ ~10 in the synthetic runs and g ≥ 32 in the target run.
   - The g histogram of all stops is printed per crib, so the separation can be read off directly.
6. **Ranking (for short cribs).** `--climb QUADFILE`:
   - fills the rest of the plugboard by hill climb, keeping crib-determined letters fixed, with a penalty for crib positions not reproduced;
   - scores by naval-blend quadgrams on the part of the message outside the crib;
   - ranks by (crib mismatches, rest score);
   - `--rank-from TSV` re-ranks saved hits without rerunning the bombe.

### Throughput (measured, 16 threads = 8 cores with SMT, ct-only run stopped)

| crib load | middle ring | settings | wall | settings/s/thread |
|---|---|---|---|---|
| 1 × 72-letter crib, k=3 (4 blocks) | pinned | 1.297e10 (full M4) | **166 s** | 4.9 M |
| 2 × 72-letter cribs, k=3 | pinned | 1.297e10 | 316 s | 2.6 M |
| 8 × 70–72-letter cribs, k=3 | **all classes** | 6.96e10 | 6,747 s | 0.64 M (= 5.2 M crib-settings/s) |

- **15 threads.** The task asked for 15-thread timings. Scale the 16-thread wall times by about ×16/15; this is SMT-bound, so it is an estimate, not a measurement. That gives 177 s for one crib pinned and about 2 h for run D.
- **The full M4 space for one long crib takes 2.8 min with the middle ring pinned, or about 15 min exhaustively including middle-ring classes, on 16 threads.**
- Short cribs are slower because many hypotheses survive the vector filter, e.g. `KRKRALLE` runs at 0.6 M/s/thread with 5 M stops per job.

## 2. Validation

1. **Unit:** `--selftest`: lazy T equals `make_path`.
2. **Synthetic (py-enigma), `tools/bombe/validate.py`: 21/21 PASS.** Outputs are in `runs/bombe/validate_synth.txt` plus an earlier 6-case run.
   - Each case uses a random M4 key with random rings on all four wheels, 10 random plugs, and a real 72-letter U-534 plaintext window.
   - 0–3 cipher letters are corrupted at random positions.
   - The bombe runs the whole job with `--midring all --k 3`.
   - A case passes if a reported key plus plugboard decrypts the uncorrupted ciphertext to the plaintext in py-enigma.
   - One third of the cases force the left wheel to step inside the message, and one third have a double-notch right wheel.
   - Every case was recovered, with g equal to the number of garbles (or fewer, when a garble hit an undetermined letter).
   - Other stops in the same job were either ring or turnover neighbours of the truth (g = 2–18; e.g. case 0 had g3 ×4 and case 12 had g2/g3) or wrong settings at g ≥ ~10.
3. **P1030698, true key hidden, full M4 space (pinned), 16 threads:**

| run | ciphertext | crib | garbles | stops in 1.3e10 settings | result |
|---|---|---|---|---|---|
| R1 | P1030698 | published plaintext `…EATWARTKNX` | 0 | 6: g0 ×2, g5 ×4 | **true key, g=0** (B-thin Gamma IV-III-VIII, pos WPVE, rings AAAH ≡ AACU/WQYR), exact board CH EJ NV OU TY LG SZ PK DI QB |
| R1 | P1030698 | corrected `…EABWARTENX` | 2 (P1030698's own reception garbles at pos 64, 69) | 10: g2 ×2, g7/13/18 | true key, g=2 |
| R2 | P1030698 with cipher letters 9 and 53 corrupted | published | 2 | 4: g2 ×2, g7 ×2 | true key, g=2 |

   - Every stop in these runs is the true key or a right-ring neighbour of it: same wheels and core, right ring ±1–3, so the middle turnover lands 1–3 letters off, which gives g = 5–18.
   - **No wrong wheel order and no wrong position produced a single stop in 1.3×10^10 settings.** This is the empirical false-stop rate behind "decisive" below.
   - py-enigma check: `Gamma IV III VIII`, rings `A A A H`, B-Thin, display `WPVE`, plugs as above, decrypts P1030698 to `TTTFFFZWOVIERVVVFXDXUUUXAUSBXXTRAVEMUENDEBLEIBENXWEITEREBEFEHLEATWARTKNX`.
   - The same tool with the ranker, given only the 14-letter crib `FXDXUUUXAUSBXX@16` over P1030698's job, puts the true decrypt first (−4.73 vs −7.2 for the best junk, leave-one-out tables).

## 3. The decisive test: is P1030680 a Thetis re-broadcast of P1030698?

**Crib variants.**
- P1030698's plaintext is 72 letters. The published decrypt has two reception garbles, T for B and K for E in `ABWARTEN`, so the crib is the corrected text:
  `C72 = TTTFFFZWOVIERVVVFXDXUUUXAUSBXXTRAVEMUENDEBLEIBENXWEITEREBEFEHLEABWARTENX`
- **Filler.** The corpus pads bodies to a multiple of 4. 36 of 44 U-534 bodies are exact multiples of 4, and the last 1–3 letters are usually an operator sign-off or random letters (`XK`, `TN`, `NM`, `DBV`, `KNX`), not a fixed filler.
- The trailing `X` of C72 may therefore be text or filler, and a Thetis re-encipherment could differ there. Variants run:
  - C72 @0;
  - C71 = C72 without the final X, @0 and @1;
  - C71B = without the X in `UUUXAUSB` (the `FXDXUUUAUSB` form of other FdU messages), @0 and @1;
  - C70 = both removed, @0, @1 and @2.
- The corpus agent's 71-letter collision sets reproduce exactly: C71@1 forces {12, 22, 46}, and C72/C71@0 force {22, 33}.

**Definition of k.**
- Forced-garble positions (crib letter = cipher letter) are excluded and do not count against k.
- k is the number of additional unknown garbles, 0–3, handled with 4 optimised blocks of 16–18 letters and 7–10 loops each.

**Search.**
- Full M4: B/C thin × Beta/Gamma × 336 orders × 26^4 cores × right-ring phases × **all middle-ring turnover classes**.
- 6.96×10^10 settings, 1 h 52 min on 16 threads (`runs/bombe/D_target_p98.log`).

| crib | forced garbles (excluded) | block stops | min verified g | survivors k=0 / 1 / 2 / 3 |
|---|---|---|---|---|
| C72 @0 | 22, 33 | 0 | – | 0 / 0 / 0 / 0 |
| C71 @0 | 22, 33 | 0 | – | 0 / 0 / 0 / 0 |
| C71 @1 | 12, 22, 46 | 3 | 40 | 0 / 0 / 0 / 0 |
| C71B @0 | 22, 30 | 0 | – | 0 / 0 / 0 / 0 |
| C71B @1 | 12, 22, 33 | 0 | – | 0 / 0 / 0 / 0 |
| C70 @0 | 22, 30 | 6 | 33 | 0 / 0 / 0 / 0 |
| C70 @1 | 12, 22, 33 | 1 | 32 | 0 / 0 / 0 / 0 |
| C70 @2 | 22, 46 | 1 | 36 | 0 / 0 / 0 / 0 |

**Verdict: P1030680 is not a re-encipherment of P1030698's text.** Under the rehearsal conditions the true key would have shown up as a stop with g ≤ 3, as it did in every validation run (21 synthetic cases, 3 P1030698 cribs in 2 rehearsal runs). There is none, anywhere in the M4 space.

What this does and does not exclude:
- Every alignment above is excluded, with at most 2–3 forced plus 3 unknown letter substitutions, whether from ciphertext transcription errors in the water-stained form or from wording differences.
- **Not excluded:**
  - a rewording that shifts the alignment in more places than the indel variants listed;
  - a different address block (e.g. another T.F. number, or `AN ALLE`). §4 covers the 41-letter body at every offset, but with the middle ring pinned (about 80% of middle-ring cases) and k ≤ 2, which is weaker than run D;
  - more than 3 unknown garbles, or garbles falling into all 4 blocks;
  - a plugboard with more than 10 pairs;
  - a non-naval or unusual machine configuration.

## 4. Step 3: next-best cribs

Setup:
- Full wheel space with the middle ring pinned (about 80% coverage of middle-ring cases), 8 threads, run alongside the resumed ct-only run.
- Survivors (verified g ≤ k, at most 10 pairs) are ranked with `navalblend` quadgrams (`runs/bombe/step3/*.rank2.tsv`, top 200 each).
- For comparison, real 72-letter naval plaintext scores about −4.8 on these tables, and the best junk in research/07's 5.9 M-key job scored −5.6.

| crib (offsets) | k | wall (8 thr) | block stops | ranked survivors | best rest-quadgram (crib reproduced) | verdict |
|---|---|---|---|---|---|---|
| `FXDXUUUAUSBXX` / `FXDXUUUXAUSBXX` @0 | 0 | 412 s | 1.36 M | 768,805 | −6.75 | junk |
| `VVVFXDXUUUAUSBXX` (13 offsets) / `VONFXDXUUUAUSBXX` (14 offsets), FdU Ausb as sender, offsets 3–25 without collisions | 0 | 6,017 s | 843 M | 20 M ranked (capped) | −7.72 | **incomplete.** Six offsets (VON @3/10/17/19, VVV @3/16) have weak menus and produced 10^7–10^8 stops. The 20 M-hit ranking cap filled with the first jobs enumerated (B-thin/Beta), so only part of the space was ranked. No German among the ranked hits. |
| `KOMXADMXUUUBOOTE` @0 | – | 179 s | 0.87 M | 220,961 | −7.42 | **impossible outright**: collides at pos 4 (A=A), inside the transcription-confirmed JCRSAJ. The run treated pos 4 as a garble, which cannot be. |
| `FXDXUUUAUSBXX` + 37×? + `WEITEREBEFEHLEABWARTEN` (72, and the UUUX variant) @0 | 1 | 476 s | 269 | 0 (min g = 5) | – | **null** |
| `WEITEREBEFEHLEABWARTEN` @40…50 | 0 | 1,341 s | 58 | 5 | −7.7 (crib not reproduced) | null |
| `WEITEREBEFEHLEABWARTEN` @48–50 | 1 | 2,072 s | 2.2e9 | 20 M (capped) | – | **not diagnostic**: 11-letter blocks are too weak; do not repeat |
| `TRAVEMUENDEBLEIBENXWEITEREBEFEHLEABWARTEN` @0…31 (forced collisions excluded) | 2 | 11,680 s | 19.7 M | 504 | none reproduces the crib (≥ 22/40 mismatches) | **null at every offset.** Offsets 1 and 2 are impossible outright (collisions at confirmed pos 2 and 4). |
| `KRKRALLE` @0 | – | – | – | – | – | **impossible outright**: collides at confirmed pos 4 (A=A); it would also have been infeasible as a crib (5.2 M stops per job) |

**Top decrypts, header crib `FXDXUUUAUSB(X)XX` @0** (all reproduce the crib; none reads as German beyond chance fragments like `FUN`, `MIT`, `NICHT`):

```
-6.750 B-thin Gamma 785 pos XNDJ rings AAAW  AO BV CE FP GL HQ KN MW TY UZ  FXDXUUUAUSBXXBSSAAJDOBGBEBEIFUNVAMIVMINSXXENDLIERNOTYTICHKRUNNDSYRCDNOWC
-6.832 B-thin Gamma 785 XNDK AAAX (same board, ring neighbour)                FXDXUUUAUSBXXBSFAAJDOBGBEBEIFUNVAMIVMINSXVENDLIERNOTYTICHKRUNNDSYRCPNOWC
-6.989 B-thin Gamma 785 XNDI AAAV                                             FXDXUUUAUSBXXBSSEAJDOBGBEBEIFUNVAMIVMINSXXSNDLIERNOTYTICHKRUNNDSYRCDPOWC
-7.072 B-thin Gamma 785 XNDG AAAT                                             FXDXUUUAUSBXXBSSEKVDOBGBEBEIFUNVAMIVMINSXXSYFLIERNOTYTICHKRUNNDSYRCDPWMC
-7.082 B-thin Gamma 785 XNDL AAAY                                             FXDXUUUAUSBXXBFFAAJDOBGBEBEIFUNVAMIVMINSWVENDLIERNOTYTICHKRUNNDSYRZPNOWC
-7.085 C-thin Gamma 862 EEQA AAAT  AC EG HZ JK NP OQ RW SY TV                 FXDXUUUAUSBXXDURNGOHBOPRNITRUEAZUNIARMAJEMBANOUOORDTFYDKROCKRKDWTHWEIOKB
-7.086 C-thin Beta  847 CSNA AAAB  AI CP EH GJ KO LM NV SW UY XZ              FXDXUUUXAUSBXSSPRSUNIRRAROJANNEONNEYSCOMIEMARZTVPKFODUSIIPZHEPNPFTHKMSTW
-7.103 C-thin Beta  364 QPJS AAAH  BV CL DU ET GN IO JZ MY PW QX              FXDXUUUXAUSBXVUNEPKEIBKUNGIHIKCMEIJERYSOOFESFCEINPSBEMNVBORGERYOKVIYIRME
-7.123 B-thin Beta  741 LAPV AAAL  AS BR DM EO FP GN HU IJ QW XY              FXDXUUUAUSBXXBGEEUCCIAXLIOFTYSIAHHSMSCHCASVIEIHIPMDANULYXPLISWARITBYGFJZ
-7.131 B-thin Beta  126 ESXG AAAZ  AR BS CL DN EZ FV HP KW TU                 FXDXUUUAUSBXXSFDORWIEIAPEYJEGTSSSAMOFBSNHENIELIESQWIUMBLNXULRUFMHBWIENVZ
-7.157 B-thin Gamma 456 JDYB AAAA  AI CU FN GR JZ KT LW OP QV                 FXDXUUUXAUSBXVIEEEMILSIEUPOMFFNWICHHUMANNQAROTOPGWHJJUEURJWIBBEVLLCMODNC
-7.161 B-thin Beta  723 UGTJ AAAG  AO BM DF GU KR LY NX PV QT SW              FXDXUUUAUSBXXQEDFVMITUHNIOHRDYSIEWFHOWNNTAGAZOMRFDMATCJKDORALSPDLLYEDLMN
-7.167 C-thin Gamma 721 HMVN AAAK  AV BN DP EO FX GW HR JM LU ST              FXDXUUUXAUSBXNIAHLVKEALMQQQSBISFUNFITCCDBVKFUNFWLQWGIMSIPPDIVIOSKAUBSTWY
-7.169 C-thin Gamma 587 ZCEK AAAE  AO BP CW EI FQ GM HY LV RS TX              FXDXUUUAUSBXXTZQZBILIALABAEREUNGHCERACOUMIMEKANDFHQYIKVJZCPNKUMPZQCKTMRS
-7.175 C-thin Gamma 462 AKVA AAAG  AO BD CP EV FN GK HW IR MY QT              FXDXUUUAUSBXXHUMIFUNAXDIESIITSJUBUWXWCEODMETERTPOCIKUPOTOPDABZMBGYRTIKNO
-7.192 C-thin Gamma 734 QCNM AAAT  AI BZ CH ES FP JO LM RY TX UV              FXDXUUUXAUSBXPLOBSKTIMRIEAAOKRIHTXPEFORNSESCILLIRETOOTBOMTOIGAZKINAPDXVV
```

**`KOMXADMXUUUBOOTE` @0**, for the record only (ruled out by no-self-encipherment at a confirmed position): the best is B-thin Gamma V-II-IV, FKTL/AAAT, rest-quadgram −7.42:
`KOMXBDMXUUUBOOTEWWYEDCREEUTLCBQYTORGJBQOCEHERHKJJAJNDJSNFSXTNWSARSKUSSMC`
Everything below it is ring neighbours or equally random text. Full top-200 lists are in `runs/bombe/step3/*.rank2.tsv`.

Reading of step 3:
- The short header cribs produce hundreds of thousands of plugboard-consistent settings, as expected. With a 72-letter body, n-gram ranking of the remaining 58 letters puts nothing near the −4.8 band of real text.
- **This is weak evidence against these headers.** The fill-in climb (4 restarts) does not always converge, and research/07 measured per-key climb success of about 20–60%.
- The long cribs are strong evidence:
  - header+ending, 35 letters with 1 garble;
  - body, 41 letters at every offset with 2 unknown garbles (pinned middle ring);
  - ending, 22 letters with 0 garbles at offsets 40–50.
- Together these say the target is not a P1030698-template message: not the same body wording anywhere, and not FdU-Ausb-header + "WEITERE BEFEHLE ABWARTEN" ending.

## 5. How to run the tool on a new crib

```sh
cd /opt/enigma-p1030680
g++ -O3 -march=native -fopenmp -std=c++17 -include mutex -o tools/bombe/bombe tools/bombe/bombe.cc   # needs AVX-512 VBMI (Zen4)
tools/bombe/bombe --selftest

# long crib, exhaustive, garble tolerant (k unknown garbles -> k+1 blocks), all middle-ring classes:
tools/bombe/bombe --ct ciphertext.txt --crib MYCRIBTEXT@OFF[,OFF2,...] [--crib OTHER@OFF] \
    --k 3 --midring all --threads 16 --gmax 12 --out runs/bombe/x.tsv 2> runs/bombe/x.log
#   '?' in a crib = unknown letter (lets you splice header + ending into one menu).
#   Forced garbles (crib letter == cipher letter) are excluded automatically; the log lists them,
#   the block sizes and loop counts, and the verified-g histogram per crib. The true key shows g <= k.
#   More loops per block means fewer false stops: aim for >= 6 loops per block (see the "crib ... loops" line).

# short crib: rank survivors by hill-climbed plugboard + quadgrams
tools/bombe/bombe --ct ciphertext.txt --crib FXDXUUUAUSBXX@0 --k 0 --gmax 0 --threads 8 \
    --out x.tsv --climb corpus/ngrams/navalblend_quadgrams.txt --rank-out x.rank.tsv --topn 200
# re-rank a saved hit list (same --ct/--crib list, same order):
tools/bombe/bombe --ct ciphertext.txt --crib ... --rank-from x.tsv --climb ... --rank-out y.tsv

# restrict the space: --orders 438,568  --refl B|C|BC  --greek b|g|bg ; test one key: --key B,g,438,WQYR,AACU
# synthetic regression: tools/bombe/validate.py N [seed] [threads]
```

**Practical guidance:**
- Choose k so that each block keeps at least 6 loops. The log shows the loop counts.
- A 72-letter crib supports k=3. A 41-letter crib supports k=2. Below about 22 letters, use k=0.
- `--midring all` costs about 5.4× the pinned mode. Use it for decisive runs.
- The tool writes all verified stops with g ≤ `--gmax`. Short cribs at k≥1 with weak blocks produce billions of stops (see `end_w22_k1`), so check the per-job stop count first with `--orders <one order> --refl B --greek g`.
- CPU etiquette: the ct-only run is controlled with `tools/run_m4/stop.sh now` and `tools/run_m4/run.sh`.
  - **Do not use `pkill -f` with a pattern that also appears in your own command line.** An earlier `pkill -STOP -f validate.py` froze the controlling shell and cost 3.5 h of wall time; the ct-only run itself was unaffected.

## Summary

- Built a garble-tolerant, AVX-512 software bombe for the full M4 keyspace:
  - pigeonhole blocks chosen for maximal menu loops;
  - diagonal-board consistency and the naval 10-pair limit;
  - exact middle-ring turnover classes.
- It runs about 5 M settings/s/thread; one long crib covers the full M4 space in 2.8 min (pinned) or about 15 min (exhaustive) on 16 threads.
- It is validated on 21 synthetic py-enigma cases and on P1030698. From P1030698's own ciphertext it recovers the key uniquely at 0 and 2 garbles, with zero false stops in 1.3×10^10 settings.
- **The P1030698 re-broadcast hypothesis is refuted.** No setting in 6.96×10^10, including all middle-ring classes, is consistent with the P1030698 text at any tested alignment (offsets 0–2, with and without the `UUUX` X and the final X) with up to 3 unknown garbles.
- The P1030698 body at every offset, and the FdU-Ausb header + "await further orders" ending template, are also null.
- `FXDXUUUAUSB` was tested as the opener at offset 0, and as `VVV`/`VON`+FdU Ausb sender at clean offsets 3–25.
  - The sender ranking is incomplete (capped, see §4).
  - Nothing German came out (best −6.75 against about −4.8 for real text).
- `KOMXADMXUUUBOOTE`@0 and `KRKRALLE`@0 are impossible outright: they collide at the confirmed position 4.
- **No plaintext was found.** The ct-only run (research/07) is running again with 15 workers.
