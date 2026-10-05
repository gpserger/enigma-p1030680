# Tools

Everything here was written for this project. Third-party programs are not vendored;
`../NOTICE.md` gives their repositories and commits. Paths inside scripts assume the
repository is at `/opt/enigma-p1030680` (see the top-level README for the one-line fix).

Python: `pip install py-enigma numpy` for everything except OCR, which also needs
`rapidocr-onnxruntime` and the `pdftoppm` and `pdfinfo` commands from poppler.

| Directory | What it does | Build | Described in |
|---|---|---|---|
| `bombe/` | Garble-tolerant software bombe for M4. Takes cribs as `TEXT@OFFSET`, splits each into k+1 blocks so that k garbles cannot hide the true key, and writes verified stops. Has a built-in climb-and-rank for short cribs. | `g++ -O3 -march=native -fopenmp -std=c++17 -include mutex -o tools/bombe/bombe tools/bombe/bombe.cc` (needs AVX-512 VBMI) then `tools/bombe/bombe --selftest` | `research/08` section 5 |
| `blind/` | `bombe_ah.cc`: the bombe with the all-hypotheses fix (`--allhyp`, `--allhyp-new`). Use this one for short cribs. `blindrank.cc`: scores every stop with a cheap statistic, keeps the top fraction, hill-climbs those, and applies the crib-mismatch test, reading stops from a pipe so nothing large touches the disk. Calibration scripts. | Same flags as the bombe for both `.cc` files | `research/15` section 1 |
| `run_m4/` | The resumable ciphertext-only sweep: job list generator, worker loop, per-job runner, `top.py` for status and candidate lists. It drives a patched `trognes/enigma`. `src/m4hc.cc` is an independent solver used for diagnosis. | Apply `third_party/trognes-enigma-topn-exhaust.patch` to trognes/enigma at the commit in `NOTICE.md`, run `make`, and put the result at `tools/enigma/enigma`. `top.py` in every mode except `--status` also needs `tools/enigma-suite/dict/00trigr.cur`. | `research/07` sections 4 and 7 |
| `ctonly2/` | `ct2`: a faster two-stage ciphertext-only searcher written from scratch. `dn/ct2dn.cc`: the same with switches for double-notch right wheels (`--RDN`, `--strideDN`, `--topkDN`, `--mask`, and a `phasescan` diagnostic). Synthetic test sets with held-out scoring tables. | `make` in `tools/ctonly2`; `ct2dn` builds with the same flags from `dn/ct2dn.cc` | `research/12`, `research/15` section 2 |
| `indicator/` | The 1945 naval indicator procedure in code: reproduces the operator's failed attempts, measures how little the indicator constrains the key, and contains the exact filter to apply if a Thetis key or Grund ever turns up. | Python | `research/06` |
| `ocr/` | Renders PDF pages and OCRs them one file per page. | Python | `../archives/SOURCES.md` |
| `decrypt_form.py` | Decrypts any corpus message with its known key, with optional letter overrides (`P1030679 23=D`), and reports which plaintext positions change. Used to tell transcription errors from reception errors. | Python | `../archives/forms/README_readings.txt` |
| `verify_py_enigma_m4.py` | Checks py-enigma's M4 against known vectors. | Python | `research/04` section 5 |
| `wirings_from_simulator.md`, `simulator_src/` | Rotor and reflector wirings extracted from the Universal Enigma simulator and cross-checked. | | `research/04` section 4 |
| `third_party/` | Our patch to trognes/enigma: a global top-N by an independent quadgram rescore, and an optional exhaustive first-pair filter. | | `research/07` section 1 |

## Things to know before trusting a result

- **Use `blind/bombe_ah` with `--allhyp` for any crib under about 25 letters.** The original
  `bombe/bombe` keeps only the first surviving pivot hypothesis per block and can drop the
  true key on weak menus.
- **The bombe's `g` is not the acceptance test for short cribs.** It only checks crib
  positions that the partial plugboard pins down. Accept a stop only if, after the climb,
  the decrypt reproduces the crib with at most k mismatches (`research/11` section 3).
- **`--excl i,j,...`** removes ciphertext positions from the crib test. The ten doubtful
  letters of P1030680 are `1,10,20,25,32,58,60,67,68,71`.
- **Scores.** "Rescore" is mean log10 quadgram probability under `corpus/ngrams/navalblend_quadgrams.txt`.
  Real 72-letter naval text scores about -4.8. Junk tops out around -5.1 to -5.2 across
  hundreds of thousands of candidates.
- **Controls.** P1030698 in `corpus/messages.json` is a real 72-letter M4 message from the
  same boat and day. Every crib method here recovered it before its null on the target was
  believed. The ciphertext-only searchers recover it only some of the time (the sweep found it
  at 40 restarts per key but not at 5; `ct2` in 2 of 8 trials at the true cores), which is
  why their nulls are weak.
