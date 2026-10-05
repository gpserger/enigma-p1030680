# Sources, rights, and what was left out

This repository contains our own notes, code and derived text. It deliberately does not
contain other people's images, scans or software. Each omission is listed here with the
way to obtain the original.

## Not included, and where to get it

| Material | Why it is not here | How to get it |
|---|---|---|
| Scans of DEFE 3/577, 3/578, 3/743, 3/744 (15 PDF files, about 1.5 GB) and page images rendered from them | Images supplied by The National Archives are subject to its terms of use | Free digital download from TNA Discovery with a free account. Catalogue links, page counts and checksums are in `archives/SOURCES.md`. |
| Photographs of the U-534 message forms | Photographs by Michael Hörenberg | <https://enigma.hoerenberg.com>, "The U534 messages" and "Unbroken". File names are given in `archives/forms/README_readings.txt` and `research/10`. |
| German interpretations and English translations of the broken U-534 messages | Hörenberg's own text | The `source_url` field of each entry in `corpus/messages.json`. |
| The Universal Enigma simulator page | Software by Daniel Palloks | <https://people.physik.hu-berlin.de/~palloks/js/enigma/>. `tools/wirings_from_simulator.md` holds the wiring tables we extracted and checked. |
| `trognes/enigma` (ciphertext-only breaker) | Third-party, GPL-3.0 | <https://github.com/trognes/enigma> at commit `3bbde93373a3b0e4bfeda18edebe9113b803904a`. Our changes are in `tools/third_party/trognes-enigma-topn-exhaust.patch`. |
| `jbo-85/enigma-suite` | Third-party | <https://github.com/jbo-85/enigma-suite> at commit `680959dc9852e3fd54c20251828acea99939c094`. Only its `dict/00trigr.cur` is used, by `tools/run_m4/top.py`. |
| `kionactf/enigma_attacks` | Third-party, Apache-2.0 | <https://github.com/kionactf/enigma_attacks> at commit `11d2e11b87ea5f4c3696cd46c3c6439eb4917316`. Evaluated in `research/04`, not used in production. |
| Raw bombe stop lists and calibration statistics (about 2.3 GB compressed) | Size; all regenerable | `runs/OMITTED_LARGE_FILES.tsv` lists them. Each run directory has the script and crib list that produced them. |
| Per-unit candidate lists of the ciphertext-only sweep | Size | The per-job merged lists and completion markers are in `runs/m4_ctonly/pass1_state.tar.zst`. |
| Compiled binaries, the Python virtual environment | Regenerable | `tools/README.md`. |

## Included, with attribution

- **`corpus/messages.json`**: identifiers, ciphertexts, key settings and decrypted plaintexts
  of 63 messages: 51 from U-534 (49 broken, one hand-cipher message, and the target) and 12
  other broken Enigma messages (the 2006 M4 Project, Norrköping, a Spanish Enigma message),
  all as published on Michael Hörenberg's site. These are historical facts established by
  his project "Breaking German Navy Ciphers" and its contributors. Please credit that project.
- **`archives/transcripts/`, `archives/ocr_text.tar.zst`**: machine-made transcriptions of
  Crown copyright public records held by The National Archives, references DEFE 3/577,
  DEFE 3/578, DEFE 3/743 and DEFE 3/744. They are unverified and contain errors. Cite the
  original record, not these files.
- **`corpus/ngrams/`**: letter statistics computed from 54 of the decrypted plaintexts above.
  The `navalblend` tables (and the blend variants and copies under `tools/run_m4/ngrams/` and
  `tools/ctonly2/ngrams/`) also blend in n-gram statistics from enigma-suite's
  `dict/00trigr.cur` and `00bigr.cur` and from trognes/enigma's `wehrmacht` and `german`
  tables (`corpus/ngrams/README.md`).
- **`tools/third_party/*.patch`**: a patch against GPL-3.0 code, offered under the same
  licence.

## Licence of this repository's own material

No licence has been chosen yet for the notes and code written during this project. Until
the owner adds one, ordinary copyright applies. If you want to build on it, open an issue.

## How this was produced

An LLM agent (Anthropic's Claude) planned the work, wrote the code and notes, and delegated
to sub-agents: larger models for cryptanalysis and archival reasoning, smaller ones for
reading scanned pages. A human collaborator supplied direction, downloaded the archive
files, and read the handwriting on the form. Nothing here was peer reviewed.
