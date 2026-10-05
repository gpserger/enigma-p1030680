# N-gram frequency tables for P1030680 (Vector A, part 4)

Built 2026-09-23 from the naval-German plaintext corpus in `../messages.json`
(54 messages with usable plaintext: 47 U-534 Enigma M4 messages + 3 M4 Project
2006 messages + 4 Norrköping naval M3 intercepts; Luftwaffe and Spanish-language
messages excluded, and 2 verbatim duplicate messages -- P1030706/P1030707,
byte-identical to P1030702/P1030704 -- dropped so they aren't double-counted).
Total corpus size: **8,494 letters** (A-Z only, all punctuation/number-words/
abbreviations kept as literal letters exactly as the Enigma operators typed
them -- i.e. this is genuine telegraphic-naval-German, not prose German).

Source text used to build these tables: `naval534_plaintext_corpus.txt` (one
message per line, `id<TAB>plaintext`).

## Important corpus finding: this traffic does NOT write CH as Q

The task brief's working assumption was that the naval convention substitutes
`Q` for `CH`. **Checked directly against this corpus and found false for
this traffic.** Counts in the 8,494-letter corpus: `NICHT` 7 / `NIQT` 0,
`SECHS` 17 / `SEQS` 0, `ACHT` 20 / `AQT` 0, `NACH` 19 / `NAQ` 0, `MARSCH` 9 /
`MARSQ` 0, `SCHLUESSEL` 2 / `SQLUESSEL` 0. Overall: 152 literal `CH`
occurrences, only 23 raw `Q`s in the whole corpus (nearly all of them either
`MARQU` = "Marquadrat", the standard naval grid-square abbreviation, or
transcription noise in the lower-quality 1939-40 Norrköping intercepts, not
genuine operator Q-for-CH substitution). **Independent second source**:
Stefan Krah's own `dict/00trigr.cur` (trained on real WWII naval Enigma
messages for the original 2006 M4 Project) ranks trigram `CHT` **#2** overall
(60,434) and has `SEQ` at **0** (never observed). Two independent real-WWII-
naval sources agree: **write CH cribs literally** (`NICHT`, `SECHS`, `ACHT`,
`NACH`, `MARSCH`...), not Q-substituted, for this net/period. (The Q-for-CH
convention may be real for some other Enigma network or era -- it does
appear, messily, in some of the 1939-40 Norrköping Luftwaffe intercepts -- but
it is not what this 1945 Kriegsmarine corpus shows.) **This corrects
`research/05_corpus_cribs.md` §3a**, which originally repeated the task
brief's assumption without checking it against the scraped corpus.

## Files, by consuming tool

### `trognes/enigma` (`tools/enigma/`) -- language name `navalfortyfive`

Format: one file per order, `<GRAM> <count>` (uppercase, space-separated, one
entry per line, sorted by descending count). trognes floors any n-gram not
listed as a "hapax" automatically, so these files only need to list observed
n-grams -- no need to enumerate the full 26^n space.

- `navalfortyfive_monograms.txt` (26 entries)
- `navalfortyfive_bigrams.txt` (584 entries)
- `navalfortyfive_trigrams.txt` (2,835 entries)
- `navalfortyfive_quadgrams.txt` (4,869 entries)

**Note:** trognes requires `-l` names to be 1-32 *letters only* (no digits) --
that's why this is `navalfortyfive`, not `naval534`.

Usage (from anywhere, pointing `-d` at this directory):
```sh
cd /opt/enigma-p1030680/tools/enigma
./enigma -4 -u b -w G438 -r AACU -g <START> -c -f \
  -l navalfortyfive -d /opt/enigma-p1030680/corpus/ngrams \
  -T 16 -R 40 < ciphertext.txt
```

### Blended version -- language name `navalblend`

Because 8,494 letters is a small sample (sparse at the quadgram level: 4,869
distinct quadgrams observed out of 26^4 = 456,976 possible), we also built a
**blended** table. Two orders needed different treatment:

- **Trigrams & bigrams** blend **4 sources**: 35% **Krah-naval**
  (`tools/enigma-suite/dict/00trigr.cur`/`00bigr.cur`, converted -- see below
  -- real WWII naval traffic, CH-literal, larger sample than ours), 25%
  trognes' bundled `wehrmacht` (telegraphic-military-German register --
  correct for spelled-out numbers/X-Y punctuation, but is **Q-for-CH**, a
  register mismatch for this corpus -- kept at reduced weight for its
  genuine telegraphic-number signal only), 25% our own `navalfortyfive`, 15%
  trognes' bundled `german` (broad prose-German smoothing floor).
- **Monograms & quadgrams**: no Krah-naval source exists at these orders
  (Krah's tool only ships tri/bi dictionaries). Blend is 55%
  `navalfortyfive` + 30% `german` + 15% `wehrmacht` (wehrmacht's weight
  cut hardest here, since a quadgram window is exactly where a literal
  `CH` digraph vs. a substituted `Q` most changes which 4-grams are "real").

**Converting Krah's log-value dictionary into blendable probabilities:**
`mkdict.py` stores `v = round(10000 * ln(p * m))` where `p = count/samplelen`
and `m` is a per-file normalising constant (`e / min nonzero p`). Because
`ln(p*m) = ln(p) + ln(m)`, the unknown constant `ln(m)` is **additive** and
therefore cancels exactly when the recovered `p_i ∝ exp(v_i / 10000)`
values are renormalised to sum to 1 -- no need to know the original sample
size or `m`. (An earlier draft of this README wrongly called this
reconstruction "lossy" and skipped it; it isn't, up to the original
`round()`'s integer quantisation, which is negligible.)

Blend method: each source table is normalized to a probability distribution
over its own observed grams, the sources are mixed with the weights above,
then rescaled to integer counts (sum = 10,000,000) and written in the same
trognes format. Files: `navalblend_monograms.txt`, `navalblend_bigrams.txt`,
`navalblend_trigrams.txt`, `navalblend_quadgrams.txt`. Weights are a
subjective choice (documented here, not tuned against the target) -- worth
revisiting if Vector C's ciphertext-only search does not converge well.

**Recommendation:** use `navalblend` as the primary scoring language for the
real ciphertext-only run (Vector C) -- `-f -l navalblend`, **with a generous
restart budget** (see validation note below -- on this 72-letter test
message, `-R 40` was not always enough after the CH-fix rebalancing;
`-R 200`+`--polish` reliably converges, and even `-R 800 --polish` costs only
~0.1s wall on this ciphertext length, so budget is cheap here). Use plain
`wehrmacht` as a comparison baseline, and `navalfortyfive` alone as a third
opinion once a candidate key is found.

### Stefan Krah's `enigma-suite` (`tools/enigma-suite/`)

Format: **lowercase**, tab-separated `<gram>\t<logvalue>`, one line per
n-gram, **all 26^n combinations enumerated** (unseen grams get logvalue `0`).
This replicates `tools/enigma-suite/tools/mkdict.py`'s own algorithm exactly
(without its optional `-g` Good-Turing smoothing):
`prob = count/len(sample)`, `m = e / min(nonzero prob)`,
`logvalue = round(10000 * ln(prob * m))`.

Krah's breaker only consumes **trigram + bigram** tables (no quadgram/unigram
scoring in the `-c`/`-i` breaker):

- `navalfortyfive_trigr.cur` (17,576 = 26^3 entries)
- `navalfortyfive_bigr.cur` (676 = 26^2 entries)
- `navalfortyfive_1gr.cur` (26 entries; not consumed by the breaker, kept for
  reference/completeness only)

Usage:
```sh
cd /opt/enigma-p1030680/tools/enigma-suite
./enigma -M M4 -c -n 40 \
  -k "B:G438:CU:AAAA" \
  /opt/enigma-p1030680/corpus/ngrams/navalfortyfive_trigr.cur \
  /opt/enigma-p1030680/corpus/ngrams/navalfortyfive_bigr.cur \
  ciphertext.txt
```
**Gotcha found while smoke-testing:** this 2006-era C binary appears to choke
silently (falls through to printing the usage screen, exit code 0, no error
message) on **long absolute paths** to the dictionary files. Passing the same
files via **short relative paths** (or copies inside `tools/enigma-suite/dict/`)
works fine. Not yet root-caused (no explicit error message is printed); just
use relative/short paths as a workaround.

No single blended file is provided in Krah's own format, since his tool only
accepts one trigram file + one bigram file per run (no in-tool blending) --
but see the conversion note above: our `navalblend_trigrams.txt`/
`_bigrams.txt` (trognes format) numbers already incorporate a properly
converted Krah-naval component, so that source's real signal is not being
left out of the overall corpus effort, just not re-packaged into Krah's own
`.cur` format. **Recommendation:** run Krah's breaker twice -- once with the
bundled `dict/00trigr.cur`+`00bigr.cur` (larger, best single existing naval
table), once with our `navalfortyfive_trigr.cur`+`navalfortyfive_bigr.cur` as
a second opinion tuned specifically to this net/theatre/month -- and compare
the two runs' top candidates.

## Validation (both tools, both formats -- real round-trip test)

We did NOT just eyeball these files -- we ran each tool's actual hill-climb
breaker against a **known** U-534 message (P1030698, 72 letters, the same
length as our unbroken target) with the correct rotor order/ring/start
supplied but the **plugboard deliberately hidden**, and let the tool recover
it via `-c` using only these n-gram tables:

- **trognes**, `-l navalblend`: the true plugboard/plaintext
  (`TTTFFFZWOVIERVVVFXDXUUUXAUSBXXTRAVEMUENDEBLEIBENXWEITEREBEFEHLEATWARTKNX`)
  scores **-4.625** (fused) when given directly -- clearly separated from the
  local optima (-5.5 to -7.2) that random-restart climbs land on with modest
  restart budgets. `-R 40` and even `-R 200` (at `-T 16`) sometimes missed
  the global optimum on this short message; `-R 800 --polish` reliably found
  it, in 0.09s wall-clock. **Practical implication for Vector C: budget
  generous restarts (200-1000+) per candidate key when scoring short (~72
  letter) messages with this table** -- it is cheap, and low-restart runs can
  silently under-perform.
- **Krah's `enigma-suite`**, `navalfortyfive_trigr.cur`+`navalfortyfive_bigr.cur`:
  of 10 random-restart passes (`-n 10`), the best-scoring one (raw score
  2,050,910, vs. 725,209 and 757,278 for two representative others) is
  exactly the true plaintext/plugboard.

**Reproducibility gotcha found and documented along the way:** Hörenberg's
site labels the M4 "Greek wheel" column `B` or `C` (e.g. "Greek: C" for the
1 May Potsdam key) -- but both trognes and Krah's tool use the standard
Beta/Gamma notation `B`/`G`. Site `Greek: B` = tool `B` (Beta); **site
`Greek: C` = tool `G` (Gamma)** -- confirmed empirically above (using `G438`
recovers the correct P1030698 plaintext; `B438` or `C438` do not). This
matters for every Potsdam-key message in this corpus and would matter again
the moment a Thetis key is ever recovered.
