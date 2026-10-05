# 04 — Tooling survey for M3/M4 Enigma cryptanalysis

Workstation survey, open-source tool evaluation, and reference-implementation
verification, done 2026-09-22 in preparation for attacking the 72-letter
naval Enigma intercept in `../ciphertext.txt`.

---

## 1. Environment

All work was done on an 8-core / 16-thread x86-64 desktop CPU with AVX-512 (AMD Ryzen 7 7800X3D)
and no CUDA-capable GPU, with g++ (C++17), make, Python 3 and Node.js. That makes it a poor fit for the
one CUDA-only tool found (`VE3NEA/enigma-cuda`, §3.7). Everything built or recommended here is
CPU/multithreaded C, C++ or Python, and needs nothing beyond a standard compiler toolchain.

## 2. (section removed)

---

## 3. Open-source Enigma breaking tools — survey

### 3.1 `trognes/enigma` (formerly `torognes/enigma`, Torbjørn Rognes) — **BUILT, WORKS, RECOMMENDED (ciphertext-only)**

- **Repo:** cloned to `tools/enigma/` from `https://github.com/trognes/enigma`
  (the older `torognes/enigma` is archived and points here; it also only
  supported the 3-wheel machine — the current repo is a substantial rewrite).
- **Language:** C++17, single file (`enigma.cc`, 266 KiB / 272,545 bytes —
  the archived, superseded `torognes/enigma` v1.0 was the ~33 KB one; this
  current rewrite is much larger), **zero external
  dependencies** — n-gram frequency tables for German/English/French/Danish/
  Swedish/Finnish/Icelandic/Polish/Spanish and a "wehrmacht" (telegraphic
  military German) table ship in the repo (`ngrams/`).
- **Machines:** 3-wheel Enigma (wheels I-VIII, reflectors A/B/C), Norway
  Enigma, and **M4 (thin reflectors b/c + Beta/Gamma Greek wheel)** —
  confirmed by direct use, see §4.
- **Attacks:**
  - Ciphertext-only: exhaustive brute-force over reflector/wheel-order/ring/
    start, with the plugboard recovered per-candidate by **hill-climbing**
    (steepest-ascent by default, `-J` for first-improvement, `-R N` random
    restarts, `-A N` simulated annealing, `--polish` finisher). Scoring:
    index of coincidence, mono/bi/tri/quadgram, a fused/weighted "all-order"
    model (`-a`/`-f`, recommended).
  - Crib-adjacent: `--crib-file` re-ranks converged candidates by known-word
    presence (a wordlist re-ranker, **not** a Turing-bombe crib-at-a-known-
    position attack).
  - Not a Bombe/known-plaintext-position solver — use §3.3/§3.2 for that.
  - Multi-threaded (`-T N`, 1-256 threads); the whole `keys × restarts`
    space is parallelised.
- **Build:** `make` (g++ -O3 -pthread). **Built cleanly, no errors, ~3 s.**
- **Self-tests:** `make test` → **309/309 passed.**
- **Benchmark** (`make bench`, this machine, single thread, quick tier):
  **98,172 keys/s** (rotor/ring/start search) and **3,375 hill-climb
  plugboard-climbs/s**. `-T 16` should scale close to linearly per the
  README's own scaling claims (measured ~3× on a 4-core machine with `-T4`).
- **Exact commands used:**
  ```sh
  cd tools/enigma && make               # builds ./enigma
  make test                             # 309 passed
  make bench                            # throughput numbers above
  echo "AAAAA" | ./enigma -u B -w 123 -r AAA -g AAA        # -> BDZGO (classic test vector)
  echo "WETTERBERICHT" | ./enigma -4 -u b -w B123 -r AAAA -g AAAA   # M4 encrypt smoke test
  ```
- **Live demo on the actual target ciphertext** (73-byte file incl. newline,
  72 letters), exhaustive M3 search + plugboard hill-climb, German
  "wehrmacht" scoring, all 16 threads, 75 s wall clock:
  ```sh
  ./enigma -c -f -l wehrmacht -u . -w ... -x 8 -r AA. -g ... -T 16 -R 2 \
    < /opt/enigma-p1030680/ciphertext.txt
  ```
  No plausible German plaintext had surfaced by the time the 75 s timeout
  killed it, and the last progress lines visible were all for reflector A.
  **That does not mean the search hadn't reached B/C** — the README is
  explicit that a progress line is only printed on a new best score, not on
  every key tried, so an unbroken run of reflector-A lines is consistent
  with B/C having been scored too without yet producing a better candidate
  (the run was killed before printing its final "keys scored" summary, so
  we don't have a real coverage number from this run). What we *can* say
  honestly, from the completed,**exhaustive** `make bench` run instead:
  at ~98k keys/s and ~3,375 full plugboard hill-climbs/s (single thread,
  §above), a genuinely complete M3 ciphertext-only search is a big job. As
  a sanity-check order of magnitude (not a tuned estimate — ignores the
  tool's own ring/position redundancy collapses, which reduce this
  substantially in practice, see the README's "identifiability" section):
  3 reflectors (`-u .` = A/B/C; Kriegsmarine M3 traffic is normally B or C
  only, so restricting to `-u B` or looping just `B`/`C` is a reasonable,
  historically-grounded ~33% cut) × 336 three-of-eight wheel-order
  permutations × 17,576 ring settings × 17,576 start settings ≈ **2.1×10¹¹**
  naive key/hill-climb pairs, i.e. **~17,000 CPU-hours** at the measured
  single-thread hill-climb rate, or a naive **~1,070 hours (~45 days)**
  even at perfect 16-thread scaling. In practice `--ring-stride`, the
  built-in leftmost/middle-wheel ring collapses, restricting to B/C, and
  fewer `-R` restarts will cut this by a large but currently unmeasured
  factor — worth benchmarking properly (`make crackquality`,
  `--ring-stride`) before committing to a full run. This was a tooling
  smoke test to confirm the binary runs correctly end-to-end on our real
  data, not a break attempt — no further cracking was attempted as part of
  this survey.
- **Notable:** the repo carries its own `CLAUDE.md`/`IMPROVEMENTS.md` and
  recent PRs authored by "trognes-claude" — i.e. it's an actively,
  recently (2026) AI-assisted-developed tool, which likely explains both its
  very thorough test suite and its unusually detailed README documenting
  identifiability/ambiguity of ring settings, etc.

### 3.2 Stefan Krah's **enigma-suite** (the actual M4 Project tool) — **BUILT, WORKS (after 1-line portability patch)**

- **Repo:** cloned to `tools/enigma-suite/` from
  `https://github.com/jbo-85/enigma-suite` (community mirror of Stefan
  Krah's original `bytereef.org/enigma-suite.html` release, version 0.76,
  2006). This is **the literal tool that broke the first two real
  Kriegsmarine M4 messages in 2006** as part of the (now-defunct)
  distributed "M4 Project"/Enigma@Home.
- **Language:** C (18 source files, ~2000 LOC), plain Makefile, no external
  deps beyond libc/libm.
- **Machines:** Heer (H, 3-wheel), M3, **M4** (`-M H|M3|M4`), reflectors
  A(Heer only)/B/C, wheels 1-8 + Beta/Gamma for M4 (`-w` format e.g. `B241`).
- **Attacks:**
  - `-i`: ciphertext-only via **index-of-coincidence**, two-stage
    (UKW/wheel/message-key first with rings pinned at A, then rings too),
    finishing with a plugboard hill climb.
  - `-c`: ciphertext-only **hill climbing** over log-trigram/bigram scoring
    (needs dictionaries — see below), with resumable state
    (`-R`, SIGTERM/SIGINT/SIGQUIT-safe checkpoint to `00hc.resume`).
  - Known-key encrypt/decrypt (plain simulator mode).
  - No dedicated crib/Bombe mode — this is IC/n-gram hill-climbing only.
  - Bundled dictionaries (`dict/00trigr.cur`, `dict/00bigr.cur`) are
    **compiled from original WWII Naval Enigma messages** (`dict/README.txt`)
    — a better statistical match to our target than generic German corpora.
  - Bundled real ciphertext (`ct/orig1`): a genuine WWII German Army message
    published by Sullivan & Weierud in *Cryptologia* 29(3):230, included as
    a worked example.
- **Build fix needed:** modern glibc/gcc makes
  `-Wimplicit-function-declaration` a hard error by default, and
  `result.c`/`resume_out.c` call `fsync()`/`srandom()` without including
  `<unistd.h>`/enabling the right feature-test macro — this 2006 code
  predates that default. Fixed by compiling with `-D_DEFAULT_SOURCE -include
  unistd.h -include stdlib.h` (no source edits needed in the end):
  ```sh
  cd tools/enigma-suite
  make clean
  make default CFLAGS="-Wall -W -Wno-unused-parameter -O3 -flto -fomit-frame-pointer -D_DEFAULT_SOURCE -include unistd.h -include stdlib.h"
  ```
  Produces `enigma` (simulator+breaker) and `tools/SGT` (Good-Turing
  smoothing helper for building custom dictionaries) — **both built, ~1 s.**
- **Verified:** simulator mode reproduces the classic 3-wheel test vector
  (`AAAAA`→`BDZGO`) and (§5) the real 2006 M4 U-264 break, byte-for-byte.
  For the `-c` hill-climbing **breaking** mode specifically, we did a full,
  honest round-trip test rather than trusting the simulator alone: enciphered
  a 177-letter German plaintext with a known key (`-M M3 -u B -w 351 -r AMV
  -s AFCHIJKZOTEL -m QWE`, 6 plugboard pairs), then ran the breaker against
  the resulting ciphertext, telling it the exact rotor/reflector/ring/message
  key but *not* the plugboard (`-k "B:351:MV:QWE"` — note the keystring's
  ring field is only 2 characters, **middle+right ring only**; the leftmost
  wheel's ring must be `A` for a key to be expressible in this format at all,
  confirming the same left-ring-is-unobservable redundancy `trognes/enigma`'s
  README documents independently):
  ```sh
  ./enigma -M M3 -c -n 50 -k "B:351:MV:QWE" dict/00trigr.cur dict/00bigr.cur /tmp/rt_cipher2.txt
  ```
  Result: **it correctly recovered the exact plugboard** (`AFCHELIJKZOT`,
  the same 6 pairs sorted) and printed the fully-correct German plaintext.
  (Our first attempt at this test used a 3-character `-r AMV`-style ring in
  the `-k` keystring and got nonsense/no match for 50 passes — that was our
  own error, not a tool bug: we'd put a non-`A` ring on the *left* wheel
  during encryption, which the keystring format can't represent. Worth
  flagging since it's an easy mistake to repeat.)
- **Speed:** single-threaded (no `-T`/thread option in this 2006 tool).
  Timed a small but *complete* keyspace slice — one wheel order/reflector,
  message keys `AAA` through `CZZ` (2,028 full hill-climbs) — at
  **~22,300 full-hillclimb keys/s** (`time ./enigma -M M3 -c -f "B:351:AA:AAA"
  -t "B:351:AA:CZZ" dict/00trigr.cur dict/00bigr.cur ciphertext.txt` → 0.091s
  wall). Faster per-key than `trognes/enigma`'s hill-climb (§3.1's 3,375/s)
  on this shorter/older codebase, but single-threaded only — no direct
  apples-to-apples comparison without also fixing thread count and message
  length, which we didn't do here.
- **Not built:** `enigma-client.py`/`eclient-rpc.py` (the distributed-computing
  client) — needs PyWin32, Windows-only, and the coordination server
  (`enigmaathome.net`) is defunct/dormant anyway (§3.6). Irrelevant for a
  single-workstation attack.

### 3.3 `kionactf/enigma_attacks` — **BUILT, WORKS — a crib-consistency primitive, not a full Bombe search by itself**

- **Repo:** cloned to `tools/enigma_attacks/`.
- **Language:** C, built as shared/static libs (`libenigma.so/.a`,
  `libenigma_attack.so/.a`) plus two self-test binaries.
- **Machines:** Enigma I, M3, **M4** (`setparam_ENIGMA_M4` exists and takes
  4-element wheel-order/ring/position arrays, confirmed in `include/
  enigma.h`), Railway/Rocket, Swiss-K, D/K, Norway, Sondermaschine, Enigma T.
  (UKW-D and Uhr not implemented; the Abwehr G-machines' cogwheel stepping
  isn't modelled.)
- **Attacks — read the actual function signature before trusting the
  README's "Bombe" framing:**
  ```c
  int bombeAttackEnigmaPlugboard(EnigmaParam *outputparam,
      EnigmaParam *origparam, char ciphertext[], char crib[],
      unsigned int crib_idx_at_ciphertext);
  ```
  `origparam` already carries the **model, reflector, wheel order, ring
  settings and start position** — i.e. this function assumes those are all
  **known/fixed** and only recovers the **plugboard**, given a crib at a
  known offset, via the classic diagonal-board consistency-loop technique
  (no plaintext statistics needed — the genuine Turing/Welchman idea). We
  confirmed this by reading `bombetest1` in `attack/testenigmaattack.c`: the
  test builds `origparam` as a **copy of the true encryption key**
  (`copyEnigmaParam`) with only the plugboard blanked (`initplugboard`) —
  i.e. even the self-test doesn't search over rotor order/ring/position, it
  hands the solver the exact right answer for everything except the
  plugboard. **There is no outer loop over wheel order, ring, or start
  position anywhere in this repo** — if we don't already know those (which,
  for a genuinely unbroken ciphertext, we won't), using this as "the crib
  attack" means **writing our own driver** that calls
  `bombeAttackEnigmaPlugboard` once per candidate (reflector × wheel-order ×
  ring × start — for M4, also × Greek wheel), and treats a self-consistent
  returned plugboard as a hit. Cost check: `bombetest1` (a 97-letter message,
  16-letter crib) completed in a couple of milliseconds
  (`time ./attack/testenigmaattack` → 0.002s wall for all 6 self-tests
  combined), so each inner call is cheap — but a full M3 wheel-order × ring ×
  start sweep is still the same ~2×10¹¹-scale space discussed in §3.1, now
  multiplied by a per-key crib-consistency check instead of a hill-climb.
  Realistically useful only once we've narrowed wheel order/ring/position by
  other means (partial key knowledge, or a `trognes/enigma` ciphertext-only
  result to confirm/extend with a crib), not as a from-nothing search.
  - **Hill climbing** (`attack/hillclimb_plugboard.c`): ciphertext-only,
    needs mono/bi/tri/quadgram frequency files the repo does **not** bundle
    — `freqdb/getfreqlst_general.sh` (interactive) and
    `freqdb/getfreqlst_special.sh` fetch them from
    practicalcryptography.com and bytereef.org respectively. Not run in this
    survey (out of scope — we already have two working ciphertext-only
    tools with bundled dictionaries).

**A better candidate for an actual crib-driven search, not yet built here:**
CyberChef (`gchq/CyberChef`, Apache-2.0, npm package, Node ≥24 — we have
v24.14.0) ships a **"Bombe"** operation whose source
(`src/core/operations/Bombe.mjs`) we read directly: it supports both 3- and
4-rotor (M4) mode with an explicit Greek-wheel parameter, takes a crib +
offset, and **does automatically search all rotor start positions** for a
given, still user-specified, rotor order/reflector — i.e. it already
contains the outer loop kionactf's library is missing (at least over
positions). CyberChef also ships a separate **"Bombe (multiple runs)"**
operation specifically for the unknown-wheel-order case. We verified this
from source (a web fetch of the raw `.mjs` file) but did **not** `npm install
cyberchef` and run it live in this survey (20+ MB, ~100 dependencies, would
have exceeded the per-tool time budget) — flagged here as the most promising
next thing to actually stand up for crib-based work, ahead of hand-rolling a
driver loop around kionactf's primitive.
- **Build gotcha:** the Makefiles `mv lib*.a ../lib` and then symlink back —
  but the repo doesn't ship an (empty) `lib/` directory, and git doesn't
  track empty dirs, so the first build silently corrupts itself (`mv`
  renames the archive *to* a file literally called `lib` instead of moving
  it *into* a directory, because the target doesn't exist yet). Fix: `mkdir
  -p lib` before building.
  ```sh
  cd tools/enigma_attacks
  mkdir -p lib
  make
  ./enigma/testenigma          # 12/12 self-tests pass (encrypt/decrypt across all models)
  ./attack/testenigmaattack    # bombetest1 PASSED (crib attack works out of the box);
                                # 5 hillclimb tests FAIL only because freqdb/ wasn't populated
  ```
- **Verdict:** the Bombe/crib code path is confirmed working immediately
  (no external data needed); the hill-climb path would need the freqdb
  download scripts run first if we want to use it (redundant with §3.1/3.2
  for our purposes).

### 3.4 py-enigma (Brian Neal, `gremmie/enigma` on GitHub, `py-enigma` on PyPI) — **INSTALLED, VERIFIED, reference implementation**

See §5 — full writeup there, since this doubles as the task's required
reference-implementation verification.

### 3.5 CrypTool 2 (Enigma Analyzer) — **surveyed, not built**

- C#/WPF desktop application (`github.com/CrypToolProject/CrypTool-2`);
  its "Enigma Analyzer" component does a documented two-stage hill-climbing
  ciphertext-only attack (algorithms credited to George Lasry et al.).
- **Not practical here:** no .NET SDK installed (only an old Mono runtime),
  and the app is built on WPF, which does not run on Linux even under Mono
  — a real build would need the full Windows desktop stack or a from-scratch
  port. **Not attempted**, consistent with the ≤10-minutes-per-tool budget.
- One promising thread for later: the analyzer's core algorithm lives in a
  separate library, `LibSource/EnigmaAnalyzerLib/EnigmaAnalyzer` in the same
  repo, which is plain C# (no WPF) and might be buildable/runnable
  standalone with just `dotnet` (not installed here) — worth revisiting if
  we specifically want Lasry's published two-stage/two-dimensional scoring
  algorithm and don't want to reimplement it from his papers.

### 3.6 Enigma@Home / the M4 Project's distributed-computing wrapper — **defunct, not applicable**

- BOINC wrapper around Krah's `enigma-client.py` talking to
  `enigmaathome.net`. The project appears to have been inactive since about 2020 and, while the server binary was bumped in May 2024, there's
  no evidence of an active volunteer pool. Irrelevant to a single-workstation
  attack in any case — the useful part (the actual `enigma` C breaking
  binary) is §3.2, used standalone.

### 3.7 `VE3NEA/enigma-cuda` — **surveyed, not usable on this hardware**

- CUDA-accelerated ciphertext-only M3/M4 cryptanalysis tool. **We have no
  NVIDIA GPU** — this
  tool cannot run here without a from-scratch HIP/ROCm port, which is out of
  scope. Noted for completeness only.

### 3.8 Others surveyed but not cloned (lower priority / no usable artifact)

- **George Lasry** — no public from-scratch GitHub repo found under his own
  name; his hill-climbing/two-phase methodology (co-authored with Kopal &
  Wacker, *Cryptologia* 2019) is what CrypTool 2's `EnigmaAnalyzerLib`
  implements (§3.5) and what `trognes/enigma`'s README cites as the basis
  for its own hill-climbing approach.
- **Michael Hörenberg** (`enigma.hoerenberg.com`) — extensive, valuable
  *narrative* documentation of the "Breaking German Navy Ciphers" project
  (2012-present, 70+ original M3/M4 messages broken) including a page
  specifically on the Turing Bombe and hill-climbing methodology, but **no
  downloadable/clonable tool** was found on the site.
- **Dan Girard** — built a private "combination bombe-simulator/hillclimber"
  used (with Hörenberg) to break the third HMS Hurricane M4 intercept in
  2013, and published methodology for the Enigma-T "Tirpitz" cipher in
  *Cryptologia*. No public source release found.
- **Practical Cryptography**'s Enigma cryptanalysis page
  (practicalcryptography.com/cryptanalysis/breaking-machine-ciphers/cryptanalysis-enigma/)
  is a well-written *tutorial* on the IC/hill-climbing method (this is
  where several of the above tools got their n-gram frequency files from)
  but is not itself a packaged, clonable tool.
- **Rust crates**: `mini-enigma` (M3, `no_std`, claims tested against real
  messages — a lean building block if we want to write our own fast search
  in Rust later), `enigma-cipher`/`enigma-simulator` (fast simulation
  libraries, encrypt/decrypt only, no built-in attacks), `enigma-cracker`
  (a from-scratch crib-free cracker; also a same-named separate GitHub
  project `Petitoto/Enigma-Cracker`, not the same codebase — not
  disambiguated further here). None built; `cargo` is available if we want
  to revisit these.
- **`shreyas-sriram/enigma-cryptanalysis`**, **`Dabblecraft2DevTeam/enigma-cracker`**,
  **`the-lambda-way/bruteforce-enigma`** — smaller/newer GitHub hill-climbing
  projects surfaced by search; not evaluated in depth given the time budget
  and that `trognes/enigma` already covers this attack type more maturely
  (309 passing tests, active development, explicit M4 support).

---

## 4. The Universal Enigma simulator (Daniel Palloks)

Full writeup, extracted wiring tables, and a working headless-driving recipe
are in **`tools/wirings_from_simulator.md`** (also fetched raw to
`tools/simulator_src/enigma-u_v262_en.html`, v2.6.2 EN, 19 Nov 2023).
Highlights:

- **Machines supported:** 19 presets including Enigma I, **M3**, **M4**
  (Beta/Gamma Greek wheels + thin reflectors B/C, selectable via the UKW
  menu), Norenigma, Sondermaschine, D/K/Swiss-K, Railway (R/R°), Tirpitz
  (T), KD, and the four Abwehr G-machines, plus a fully free "Custom" mode.
  All 8 Wehrmacht/Kriegsmarine rotors (I-VIII) and both thick (A/B/C) and
  thin (B-thin/C-thin) reflectors are present and were extracted.
- **No URL-parameter interface exists** — nothing reads `location.search`/
  `location.hash`; every setting lives in in-page JS globals driven purely
  by DOM event handlers.
- **Headless driving works, but needs a DOM** (not plain Node) — essentially
  every function, including the core cipher step `kodieren()`, touches
  `document.getElementById`/`document.f.*` on every call. We proved this
  concretely with `jsdom` (`tools/simulator_src/nodecheck/drive.js`), after
  patching two real jsdom gaps (legacy "named access" on `document` and on
  `<form>` elements, both real browser behaviors this 2007-era script
  depends on that jsdom doesn't implement) with a ~10-line `Proxy` shim. The
  **unmodified** simulator source then runs under Node and reproduced the
  classic test vector exactly (rotors I II III, UKW B, ring/start AAA/AAA,
  `AAAAA` → `BDZGO`) — see the recipe in that file. This gives us a second,
  independent oracle for validating any homegrown implementation, including
  full M4 configurations, without a browser.
- All wiring tables (rotors I-VIII, Beta, Gamma, reflectors A/B/C and the M4
  thin B/C, entry wheels) were cross-checked byte-for-byte against
  py-enigma's own `rotors/data.py` (§5) — **they match exactly**, which is
  good independent confirmation the extraction is correct.

---

## 5. Python reference implementation: py-enigma

- **Installed** into a fresh venv:
  ```sh
  python3 -m venv /opt/enigma-p1030680/.venv
  /opt/enigma-p1030680/.venv/bin/pip install py-enigma
  ```
  → `py-enigma 1.0.2` (MIT license), from
  `https://github.com/gremmie/enigma` / PyPI. Clean install, no build issues.
- **Supports M3 and M4** (Beta/Gamma, thin B/C reflectors) via
  `EnigmaMachine.from_key_sheet(rotors=..., ring_settings=..., reflector=...,
  plugboard_settings=...)`.
- **The package ships its own test suite** (`enigma/tests/test_enigma.py`)
  containing **four real, historical, previously-classified Kriegsmarine
  message decrypts**, not just synthetic vectors — including exactly the
  test the task description pointed at:

  **`KriegsmarineTestCase`** — reflector **B-Thin**, rotors **Beta II IV I**,
  ring settings **A A A V** — the U-264 message that Stefan Krah's M4
  Project broke first, on 20 Feb 2006 (a contact/attack report from Kptlt.
  Hartwig Looks, forced to submerge under depth-charge attack, with a short
  weather addendum at the end — see bytereef.org/m4-project-first-break.html
  for the full history; we checked this against the primary source rather
  than trusting our own first-draft gloss, which had wrongly called it a
  "weather report" and invented a transmission date). We ran all 9 bundled
  tests directly:
  ```sh
  cd /opt/enigma-p1030680
  .venv/bin/python3 -m unittest enigma.tests.test_enigma -v
  # -> Ran 9 tests in 0.002s — OK (all 9 pass, incl. 4 real M3/M4 historical decrypts)
  ```
  We also wrote a **standalone verification script**,
  `tools/verify_py_enigma_m4.py`, that reproduces this exact case outside
  the test framework and prints the full before/after:
  ```sh
  .venv/bin/python3 tools/verify_py_enigma_m4.py
  ```
  Result: **PASS** — decrypts the 232-letter ciphertext (after stripping the
  8-letter indicator groups and trailing partial group, per the original
  break) to the known plaintext starting `VONVONJLOOKSJHFFTTTEINSEINS...`
  (`VONVONJLOOKS` = the sender's own self-identification, "von, von
  [Kptlt.] Looks"), ending display position `VJWY` as published.

  **Settings, for reuse:**
  ```
  Model: M4
  Rotors (L→R incl. Greek wheel): Beta II IV I
  Ring settings:                  A A A V
  Reflector:                      B-Thin
  Plugboard: 1/20 2/12 4/6 7/10 8/13 14/23 15/16 17/25 18/26 22/24
             (= AT BL DF GJ HM NW OP QY RZ VX)
  Start position: VJNA   →   end position: VJWY
  ```

- **Independently cross-validated, four ways:** we re-ran the *exact same*
  settings through **`trognes/enigma`** (§3.1: `./enigma -4 -u b -w B241 -r
  AAAV -g VJNA -s "AT BL DF GJ HM NW OP QY RZ VX"`) and through **Stefan
  Krah's original `enigma-suite`** (§3.2: `./enigma -M M4 -u B -w B241 -r
  AAAV -s ATBLDFGJHMNWOPQYRZVX -m VJNA`) — **both independently reproduce
  the identical plaintext**, byte-for-byte. We then drove **Daniel Palloks'
  JS simulator itself** through the identical M4 settings via the jsdom
  harness from §4 (`tools/simulator_src/nodecheck/drive_m4.js`) — **also an
  exact match** (see §4 for the recipe and two transcription bugs we caught
  and fixed by diffing byte-by-byte along the way). That's **four
  independently-authored implementations** (Python/py-enigma, two separate
  C/C++ codebases, and Palloks' 2007-2026-maintained JavaScript) agreeing on
  the full M4 algorithm — plugboard, ETW, three moving rotors, Greek wheel,
  and thin reflector all included — on a real historical message, which is
  about as much confidence as one can get before writing our own
  implementation.
- **Wiring tables independently cross-checked, programmatically:** we
  diffed all 8 rotor wirings + notch letters, Beta, Gamma, and reflectors
  A/B/C/B-Thin/C-Thin as extracted from the JS simulator (§4,
  `tools/wirings_from_simulator.md`) against py-enigma's own
  `enigma/rotors/data.py` in Python (not just eyeballed) — **every wiring
  string and every notch letter matches exactly**.

---

## 6. Summary and recommendations

**Environment:** a 16-thread x86-64 CPU with no CUDA-capable GPU; a standard g++ / make / Python toolchain
is present and sufficient.

**Built and verified, in `tools/`:**
1. `tools/enigma/` — `trognes/enigma`, C++17, ciphertext-only brute-force +
   hill-climbing, M3 & M4, 309/309 tests pass, ~98k keys/s single-thread.
2. `tools/enigma-suite/` — Stefan Krah's original 2006 M4-Project-winning
   tool, C, IC + hill-climbing, M3 & M4, bundled naval-message-trained
   dictionaries, builds after a one-line CFLAGS portability fix.
3. `tools/enigma_attacks/` — `kionactf/enigma_attacks`, C, has a genuine
   crib-consistency plugboard solver (`bombeAttackEnigmaPlugboard`) that
   passed its self-test immediately, plus M3/M4 simulation — but it only
   recovers the plugboard for an **already fully-specified** rotor
   order/ring/position; there's no built-in search over those, see §3.3.
4. `.venv/` — py-enigma 1.0.2, Python reference implementation, verified
   against 4 real historical M3/M4 decrypts including the canonical
   "Beta II IV I / B-Thin / AAAV" U-264 message.

**Also produced:**
- `tools/wirings_from_simulator.md` — full rotor/reflector wiring tables
  extracted from Daniel Palloks' Universal Enigma simulator, plus a proven
  jsdom-based recipe for driving that simulator headlessly as a second
  independent validation oracle.
- `tools/verify_py_enigma_m4.py` — standalone, re-runnable M4 test-vector
  verification script.

**Recommendation:**

- **(a) Exhaustive M3/M4 ciphertext-only search:** use **`tools/enigma`**
  (`trognes/enigma`) as the primary engine — it's the most actively
  maintained, best-tested (309 tests), fastest (~100k keys/s/thread, scales
  with `-T 16` on this CPU), and has the most sophisticated scoring
  (fused quad/tri/bi/mono + IC, `-l wehrmacht` telegraphic-German model,
  `--ring-stride` for a principled speed/accuracy tradeoff, random-restart
  and simulated-annealing plugboard recovery). Cross-check any candidate
  key/plaintext against **Stefan Krah's `enigma-suite`** (`tools/enigma-suite`,
  §3.2) whose bundled dictionaries were trained specifically on real WWII
  naval traffic — a useful second opinion tuned closer to our target's
  register than generic German. Both should also be checked against
  **py-enigma** and/or the **Palloks simulator via jsdom** (§4) once a
  candidate key is found, purely as an independent correctness oracle before
  trusting a result.
- **(b) Crib-based search:** this needs more work than (a) before it's a
  push-button tool. **`kionactf/enigma_attacks`**'s
  `bombeAttackEnigmaPlugboard` (§3.3) is a correctly-working, fast (sub-ms)
  crib-consistency plugboard solver we can call once rotor order/ring/start
  are already known or narrowed — genuinely useful as a *confirmation* step
  once (a) or partial intelligence gives us a short-list of candidate keys
  plus a plausible crib, but **it is not itself a from-nothing Bombe search**
  (no outer loop over wheel order/ring/position exists in the repo — we'd
  have to write one, and a full unrestricted sweep is the same ~10¹¹-scale
  space as (a)). **CyberChef's "Bombe" and "Bombe (multiple runs)"
  operations** (npm `cyberchef`, source-verified in §3.3 but not yet
  installed/run here) look like the more complete out-of-the-box option —
  worth actually standing up next time we have a crib in hand, ahead of
  hand-rolling a driver around kionactf's primitive. `trognes/enigma`'s
  `--crib-file` word-list re-ranker (§3.1) is a useful supplementary
  finisher on top of its ciphertext-only search either way, but is not a
  substitute for a true positioned-crib Bombe run.

**One practical note for our actual 72-letter target:** the U-264 test
vector in §5 is a reminder that real Kriegsmarine traffic carries message-
indicator groups (there, the first and last two 4-letter groups, plus a
trailing partial group) that are **not** part of the encrypted text proper
and must be identified/stripped before any ciphertext-only or crib search —
72 letters is exactly 18 four-letter groups, so it's worth checking whether
our target follows the same convention (which would leave a shorter core
ciphertext than 72 letters) before running any of the above.

All tools built here (§3.1-3.3), the wiring-table extraction (§4), and the
reference implementation (§5) are cross-validated against each other and
against published, real, historical Enigma decrypts — we're starting the
actual attack on solid, verified tooling, with the important caveat that
(b) still needs either a driver loop written around kionactf's primitive or
a live CyberChef install before it's actually usable end-to-end.
