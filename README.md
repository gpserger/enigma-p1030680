# P1030680: an unbroken Enigma M4 message from U-534. Tombstone and handover

**Status: not broken.** This repository is the complete record of a two-week attempt
(22 September to 5 October 2026) to decrypt the last unbroken message from the U-534
radio-room find. The attempt was run by an LLM agent (Claude, with sub-agents) working with one
human collaborator on a desktop PC and, for five days, one rented 32-vCPU cloud machine.
It ended without a plaintext. The repository exists so that the next person or agent
does not repeat the work, knows which negative results are strong and which are weak,
and can start from the open leads.

If you are an agent picking this up, read sections 1 to 5 of this file, then
`research/13`, `research/15` and `research/11` in that order. Everything else is reference.

Everything here was written by LLM agents and checked by running code where code could
check it. No Enigma historian has reviewed it. Treat archival and historical statements
as leads with citations, not as authority.

---

## 1. The target

| | |
|---|---|
| Message | P1030680 in Michael Hörenberg's "Breaking German Navy Ciphers" project, <https://enigma.hoerenberg.com/index.php?cat=Unbroken&page=P1030680>. It is the only entry on his "Unbroken" page. |
| Origin | A radio message form recovered from U-534 (sunk 5 May 1945, salvaged 1993). |
| Machine | Enigma M4 (thin reflector B or C, Beta or Gamma, three of wheels I to VIII, 10 plug pairs assumed). |
| Body | 72 letters, 18 four-letter groups, in `ciphertext.txt`: |

```
JCRS AJTG SJEY EXYK KZZS HVUO CTRF RCRP FVYP LKPP LGRH VVBB TBRS XSWX GGTY TVKQ NGSC HVGF
```

What is written on the form (details and confidence levels in `research/10`,
`archives/forms/hand_analysis.md`):

| Field | Reading | Notes |
|---|---|---|
| Datum | 1.5.45 | |
| Welle (circuit) | NB | Bletchley's decrypts equate this circuit with 4860 kc/s. |
| Gruppenzahl | 22 | 2 indicator groups + 18 body groups + 2 repeated indicator groups. |
| Indicator groups | `VROL NMKA` at the head, `VAOL NMKA` repeated at the tail | One of the two copies is a reception garble. The `R` at the head is overwritten in darker pencil. The tail could also read `VDOL`. |
| Reception time | first read as "10 30" (`research/10`); **almost certainly 16:30** | See section 3.1. |
| Uhrzeitgruppe (time of origin / serial) | `?55?/70` | First digit under a stain, probably 1. The hour's second digit (the first 5) rests on one reader; the last digit was read as 2, 3 or 8 by different readers. So the time of origin is probably about 15:52 (minutes 58 on the oldest reading), serial 70. |
| Operator's notes | `14`, `9`, `PUYY` (circled), `M-Thetis`, a struck-out `ELKC` | The operator tried twice to decrypt it with the Potsdam key, failed, then noted that the key net is Thetis. Dan Girard's reconstruction of these steps is on Hörenberg's page and reproduced in code in `research/06`. |

**Key net.** The indicator trigram decodes to a Kenngruppenbuch column allocated to
**Thetis**, the U-boat training net in the Baltic (secondary sources name FdU Ausbildung,
the training flotillas and the torpedo recovery boats as its users; no primary document we
found names the holders). Hörenberg labels this "probably Thetis".
U-534 did not hold the Thetis key, which is why its operator gave up. No Thetis key for any
date is known to survive, and there is no evidence that Bletchley Park ever broke Thetis. One outside researcher
raises the possibility that an earlier allocation list maps the same column to Aegir
instead; see section 6, lead 3.

**Why it is hard.** There is no key, no crib, and only 72 letters. Ciphertext-only attacks
on a fully plugged Enigma become unreliable below about 80 letters. The form was copied
by ear from a noisy broadcast: the published copy of the same operator's neighbouring message
has 4 letters in 88 that do not decrypt (at least one a reception error, one a transcription error). So the true ciphertext probably differs from the published one in two to five places.

---

## 2. What is in this repository

```
README.md                 this file
NOTICE.md                 sources, rights, and what was deliberately left out
ciphertext.txt            the 72 letters
research/01 ... 16        the working notes, one per line of attack (see section 7)
corpus/                   63 messages (49 broken U-534 messages and 12 other broken Enigma messages with keys,
                          a U-534 hand-cipher message, and the target; facts only), naval n-gram tables
archives/                 transcripts of 943 Bletchley Park decrypt pages, OCR text of 3,349 pages,
                          the handwriting analysis of the form; SOURCES.md says how to get the scans
tools/                    our own code: software bombe, ciphertext-only search, rankers, OCR, indicator maths
runs/                     compact outputs of every search, the state of the big sweep, and a list of what was dropped
```

The working directory was about 4.5 GB. This repository is about 36 MB. What was dropped,
and how to regenerate it, is listed in `NOTICE.md`, `archives/SOURCES.md` and
`runs/README.md`. Scripts assume the repository sits at `/opt/enigma-p1030680`. Either
put it there, symlink it, or run `grep -rl /opt/enigma-p1030680 . | xargs sed -i "s#/opt/enigma-p1030680#$PWD#g"`.

---

## 3. What we established

These are positive findings. Each has its evidence in the note cited.

### 3.1 The message was received at about 16:30, not 10:30 (`research/13` section 1)

The number after the slash in the Uhrzeitgruppe is a running serial shared by the whole
circuit, restarting at midnight. On the other U-534 forms of 1 May, serials 53 to 55 were
received between 10:10 and 10:46, serial 69 at 16:15, and serials 80 to 86 between 18:12
and 19:52. P1030680 carries serial 70. It is also photographed between the form received
at 16:15 and one received after 18:00. A human reader later read the reception box as "16 30" with an
ornate 6. Crib hypotheses built on a morning origin have lost their basis.

### 3.2 The Potsdam key day ran noon to noon (`research/13` section 2)

Nine messages with times of origin between 09:04 and 11:57 on 1 May decrypt with the key
Hörenberg labels "30 April". Evening messages decrypt with the "1 May" key. The operator
tried the 1 May key on P1030680, which by his own practice suggests an origin after noon.
The message is therefore probably on the first day of the May key sheet.

### 3.3 Field conventions in Bletchley's decrypts (`research/14` section 1)

"T O O" on a decrypt equals the German Uhrzeitgruppe to within a minute. "T O I" is GMT,
two hours behind German time, and matched the reception time on two U-534 forms (a third
message reached U-534 by relay hours later). `research/13` section 3 has a table of seven messages for which both the German
original and the Bletchley English survive, with the translation conventions
(SSD = EMERGENCY, KR = MOST IMMEDIATE, FdU Ausb = CAPTAIN (U/B) TRAINING, and so on).
German originals run to about 1.31 times the letter count of the English.

### 3.4 The indicator gives no usable constraint (`research/06`)

The 1945 indicator procedure is reproduced exactly in code, including the operator's two
failed attempts (`ELKC`, `PUYY`) and his trial decrypt of the first six letters, `IPZAYK`.
That last item confirms the first six ciphertext letters. But the message key depends on a
Grund from a per-net table, and the Thetis table is unknown, so the indicator constrains
nothing. The VR/VA discrepancy changes only a filler letter and the first letter of the
Verfahrenkenngruppe (SEDM or OEDM), and does not affect the key-net identification.

### 3.5 Handwriting (`archives/forms/hand_analysis.md`)

The form shares a hand with P1030679, received 15 minutes earlier, whose ciphertext is
verified by its decrypt. In this hand T, F and H are the same bare diagonal stroke, and C
and a small E are confusable. Ten positions are doubtful (0-based):
**1, 10, 20, 25, 32, 58, 60, 67, 68, 71**, with alternates in
`archives/forms/P1030680_readings_human.txt`. Position 1 is in this set although the
operator's trial decrypt confirms positions 0 to 5 (section 3.4). On P1030679 one published letter is a
transcription error (position 23, B for D) and at least one is a genuine reception error.
Only an 800-pixel photo of the form is public. The original is a 3072 x 2304 camera file.

### 3.6 A flaw in our own bombe, found and fixed (`research/15`)

On weak menus (short cribs with few loops) the first version of the bombe kept only the
first surviving pivot hypothesis and could silently drop the true key. It lost 7 of 48
synthetic true keys. `tools/blind/bombe_ah.cc` keeps all hypotheses and recovers 48 of 48.
The address-block families of `research/11` and the three families with truncated rankings
were re-run with the fixed version. The other short-crib runs of `research/08` section 4 and the
fragment runs of `research/13` and `research/14` were not. The tables in `research/11`
section 7 and `research/15` say which bombe produced each row.

### 3.7 No copy found in the Bletchley decrypts (`research/09`, `13`, `14`)

All 3,349 pages of The National Archives pieces DEFE 3/577, 3/578, 3/743 and 3/744 were
OCR'd, and the 943 pages around 1 May were transcribed. No decrypt from the
training community has a time of origin between 12:00 and 16:30 on 1 May. Since Thetis was
unbroken, only a copy of the same text re-enciphered in a broken key could have appeared,
and none did. Bletchley decrypted only part of that afternoon's traffic, so this is a limit
of the source, not proof that no such copy was sent (`research/14` section 5).

---

## 4. What was tried, and how much each null is worth

"Strong" means a correct hypothesis would almost certainly have been detected, verified
with a positive control. "Weak" means a miss is likely even if the hypothesis is right.

| # | Attack | Coverage | Result | Strength | Note |
|---|---|---|---|---|---|
| 1 | Known Potsdam daily keys (three days), every message key, 676 middle/right ring settings | about 9 x 10^8 decrypts | Best score -6.9; real text scores about -4.8 | Strong | `research/07` section 3 |
| 2 | P1030680 is a Thetis re-broadcast of P1030698 (another 72-letter U-534 message of 1 May) | Whole M4 space, all middle-ring classes, up to 3 garbles | Zero survivors | Strong | `research/08` section 3 |
| 3 | Body phrases from P1030698 and header/ending templates | Body at all offsets with up to 2 garbles; header+ending at offset 0, ending at offsets 40 to 50 | Nothing reproduces the crib | Medium | `research/08` section 4 |
| 4 | Address-block cribs at the start of the message (training command as sender or addressee, flotillas, recovery boats, "an alle") | Hundreds of crib/offset pairs, whole M4 space, middle ring pinned | Nothing German | Weak to medium | `research/11` sections 4 and 7 |
| 5 | Ten full-length 72-letter reconstructions from context | Up to 3 garbles | Zero stops | Strong for those exact wordings only | `research/11` |
| 6 | German reconstructions of Bletchley decrypts from the same day (Norway chart outfits order, TF 19 arrival, "Peter von Danzig" transfer, U 2356 arrival) | Full texts up to 3 garbles, fragments at 0 or 1 | Null | Strong for exact texts, weak for fragments | `research/13`, `14` |
| 7 | The row 4 families, 19 body/tail phrase placements and the U 2356 fragments again with the ten doubtful letters excluded and the fixed bombe | About 390 crib/offset pairs; weak or flooding ones dropped | Null | Weak to medium | `research/11` section 7 |
| 8 | Every stop of the three families whose ranking had been truncated | 330 million stops scored, 39 million hill-climbed | Nothing German | Medium | `research/15` |
| 9 | Ciphertext-only hill-climb over wheel orders | 424 of 1,344 jobs: all orders with wheel VIII on the right, and 76% of orders with VIII elsewhere | 308,642 distinct candidates, best -5.15 | **Weak** | `research/07`, `12`, `runs/m4_ctonly` |

Three caveats apply to almost every row:

- **The middle ring was pinned** in the ciphertext-only sweep and in every crib run except
  row 2. Depending on the wheels and the crib position that covers roughly 57 to 94% of
  messages (79% for the likeliest wheel orders of the sweep, about 80% for full-length cribs).
- **The ciphertext-only sweep misses more often than it hits.** Calibrated on real
  72-letter messages, one pass finds the message in its correct job about 20 to 35% of the
  time at 20 restarts per key and about 10% at 5. A finished job with no hit does not
  exclude that wheel order. See `research/07` section 6.
- **A body that is not plain German would defeat the scoring.** An officer-grade double
  encipherment, a short-signal codebook body, or heavy garbling are all possible and
  untested.

---

## 5. Why we stopped

The crib route needs a sister text, and the Bletchley volumes most likely to hold one have been read.
The compute route has covered the likeliest wheel orders once and found nothing, and
covering the rest at a detection rate of one in four would take months of desktop time for
the least likely configurations. Nothing that we could do with more agent time changes
either fact. What would change them is outside material, listed next.

---

## 6. Open leads, in order of expected value

1. **A full-resolution photo of the form.** Hörenberg photographed the originals (a
   Panasonic DMC-FX8, 3072 x 2304). The public copy is 800 pixels wide. A sharper image would
   settle the two stained digits of the time of origin, the reception hour, and the ten
   doubtful letters, and with them every crib family can be re-run in about a day
   (`runs/ctx_excl/` has the scripts). The physical forms are in the U-534 collection in
   Birkenhead (Merseyside Maritime Museum / Western Approaches).
2. **Thetis key material.** None found, but these were not exhausted (`research/09`
   section 5): NARA R.I.P. 475 ("actual contents of the Key Sheets recovered or captured";
   not in the RG 38 finding aid, possibly in RG 457), R.I.P. 403, 604 and 610 in RG 38; Ralph Erskine, "Captured Kriegsmarine Enigma Documents
   at Bletchley Park", Cryptologia 32:3 (2008); TICOM I-38; Hinsley volume 2 appendix 4 and
   volume 3(1) appendix 3; the Bundesarchiv RM series. An independent campaign
   (<https://github.com/Digital-Defiance/HELUT>, file `BREAK_P1030680.md`) cites further
   identifiers we did not check: Kenngruppentafel Prüfnr 1772a and Doppelbuchstabentauschtafeln
   Prüfnr 2499 (bigram tables, not key sheets), NID 1/PW/REP/17 (which does not name Thetis),
   RM45 III-120 KTB, U-977 reel 84. With a Thetis daily key
   the indicator maths in `tools/indicator/` finishes the job in seconds.
3. **Is the net really Thetis?** The same campaign reports an earlier allocation list that
   maps the Kenngruppenbuch column to Aegir. If that were right, the question becomes
   whether Aegir key material for May 1945 survives. Unchecked by us (`research/16`). Note
   that U-534's own allocation list, the one its operator used, gives this column to Thetis,
   and the campaign itself treats the conflict as probably explained by one list replacing
   the other (`research/09` section 3, `research/16`).
4. **What was sent at about 15:52 on 1 May 1945.** War diaries of FdU Ausbildung, the 25th,
   26th and 5th U-boat Flotillas, and the boats and tenders around Kiel, Travemünde,
   Neustadt and Warnemünde that afternoon could name the message. On 1 and 2 May these units
   were being told to move west or to Norway, to report arrivals, and to lay up recovery
   boats (see the transcripts). A text known from a diary is a full-length crib, and a
   72-letter crib clears the whole machine space in minutes.
5. **Later Bletchley volumes.** DEFE 3/579 onward, in case some 1 May afternoon traffic was
   decrypted after 17 May. Low prior: 1 May items had dwindled to four pages in the last
   part of 3/578.
6. **More ciphertext-only compute.** In order of value: repeat the wheel-VIII-right jobs
   with different seeds and at least 100 restarts per key; run the middle-ring turnover
   classes that pass 1 skipped; finish tier 2 and run tiers 3 and 4. `tools/ctonly2/ct2`
   is a faster searcher that was validated but never used in the sweep. `research/12`
   recommended it for the 840 jobs with a single-notch right wheel, where it found the message
   in its true unit about half the time; its superset `ct2dn` comes close to that on
   double-notch wheels at 72 restarts (`research/15` section 2). The state of the sweep is
   in `runs/m4_ctonly/` and it resumes from there (see `runs/README.md` first).
7. **Crib runs left undone** (`research/11` section 7): 326 body-phrase placements at zero
   garbles, 9 cribs that flooded, and short cribs with the middle ring unpinned.
8. **Other receivers.** Any surviving radio log from a boat or shore station that held
   Thetis and copied the same broadcast.

---

## 7. The research notes

| Note | Subject | Use it for |
|---|---|---|
| `01_naval_enigma_reference` | M4 wiring, stepping, key procedure, message format, key nets | Implementing or checking a simulator |
| `02_breaking_methods` | Survey of historical and modern attacks, keyspace, odds | Choosing a method |
| `03_provenance` | Exact source of the message, Girard's reconstruction of the operator's attempts, known Potsdam keys | The facts in section 1 |
| `04_tooling` | Survey of open-source Enigma breakers, py-enigma verification | What exists, what builds |
| `05_corpus_cribs` | The 63-message corpus, naval plaintext conventions, crib candidates, n-gram tables | Writing German cribs in authentic form |
| `06_indicator_constraint` | The indicator procedure in code, and why it gives nothing for Thetis | Section 3.4; the ready filter if a key turns up |
| `07_ctonly_pipeline` | The ciphertext-only pipeline, its validation, the sweep configuration | Row 9; resuming the sweep |
| `08_bombe_crib_search` | The software bombe, validation, the P1030698 test | Rows 2 and 3; running a new crib |
| `09_archival_hunt` | Search for Thetis key material; DEFE 3; ranked actions | Lead 2 |
| `10_transcription` | Per-letter confidence of the published ciphertext from the photo | Superseded in part by `hand_analysis.md`; its reception-time reading is wrong |
| `11_context_cribs` | Context reasoning, crib families, results; section 7 is the rerun with excluded letters | Rows 4, 5, 7. Sections 1 to 5 assume a morning origin, which section 3.1 above overturns |
| `12_ctonly2` | A faster ciphertext-only searcher and its measured detection rates | Lead 6 |
| `13_ultra_cribs` | Reception-time correction, key-day rule, German/English table, DEFE 3/577 candidates | Sections 3.1 to 3.3 |
| `14_578_cribs` | DEFE 3/578 and 3/744 screened for the 1 May afternoon | Section 3.7 |
| `15_blind_spots` | The bombe flaw, complete ranking of flooded families, double-notch detection | Sections 3.6 and row 8 |
| `16_public_status` | Whether anyone else had solved it by 27 September 2026 | Lead 2 and 3 pointers |

Notes are kept as written at the time, apart from normalising machine-specific paths and
replacing participants' names. They still mention some working paths that are not in this
repository (for example `archives/ocr/` and `archives/transcripts578/`, now
`archives/ocr_text.tar.zst` and `archives/transcripts/`, and a `PLAN.md` that is not included). Later
notes correct earlier ones. Where they disagree, the later note and this file win.

---

## 7a. Where the notes disagree

The notes were written as the work went, by different agents, and were not reconciled.
Know these before quoting any of them.

- **Order of the decrypt volumes.** `research/09` says DEFE 3 is bound by time of origin.
  `research/13` and `14` found it is in order of issue. The later notes are right.
- **Time of origin on the form.** `research/10` reads `??5[3/8]`, the handwriting analysis
  reads "…53/", and the human reader `?552` or `?553`. Every "15:5x" statement rests on
  that one reader's hour digit.
- **Doubtful letters.** `research/10` proposed excluding positions 10, 12, 20, 25, 32, 47,
  60, 67. All later runs used the human reader's set 1, 10, 20, 25, 32, 58, 60, 67, 68, 71.
  Position 1 is in that set although the operator's own trial decrypt confirms positions 0
  to 5 and the handwriting analysis rates it high confidence. Position 12 is rated low by
  `research/10` and high by the handwriting analysis.
- **Middle-ring coverage** of a pinned search is quoted as 57, 79, 80, 89 and 94 percent in
  different places. It depends on which wheels are in the middle and right positions and on
  the message length considered.
- **Double-notch detection.** `research/12` reports 18% against 49%. `research/15` shows
  most of that gap came from the composition of the test set; the like-for-like figures are
  39% against 48%.
- **`research/11` sections 1 to 5** reason from a 10:30 reception. Its crib results stand as
  tests of those cribs, but its account of what the message probably says does not.
- **Which bombe.** Short-crib nulls in `research/08` section 4 and the fragment runs in
  `research/13` and `14` used the original bombe and carry its weak-menu loss, about 15% on
  the weakest menus and less on loop-rich ones.

---

## 8. Reproducing and extending

Requirements: Linux, g++ with C++17, Python 3.10 or later, `pip install py-enigma numpy`.
The bombe uses AVX-512 VBMI (AMD Zen 4, Intel Ice Lake or later). Build commands and
usage for each tool are in `tools/README.md`.

A minimal session:

```bash
# 1. check your setup against a real 72-letter message with a known key
python3 tools/decrypt_form.py P1030698

# 2. build the bombe and test a crib of your own against the target
g++ -O3 -march=native -fopenmp -std=c++17 -include mutex -o tools/bombe/bombe tools/bombe/bombe.cc
tools/bombe/bombe --ct ciphertext.txt --crib YOURGERMANTEXT@0 --k 2 --midring all \
    --excl 1,10,20,25,32,58,60,67,68,71 --threads 8 --out stops.tsv

# 3. for short cribs use the fixed bombe and the streaming ranker instead (research/15)
```

A full-length crib (60 letters or more) with up to three garbles is decisive in minutes.
A short crib (under about 25 letters) floods with false stops and needs the prefilter and
the post-climb mismatch test described in `research/11` section 3 and `research/15`.

Always run a positive control first. `corpus/messages.json` has 61 real broken messages with keys.
P1030698 is 72 letters, from the same boat and day, and is the control used throughout.

---

## 9. Lessons that cost us time

- **Check when the message was received before guessing what it says.** The first round of
  crib work (`research/11`) assumed a morning origin because one digit on the form was misread.
- **A bombe stop count of zero is only as good as the menu.** Count loops per block, run a
  control with the same crib length and exclusions, and keep all pivot hypotheses.
- **Never cap a ranking silently.** One family had 98% of its stops unscored.
- **Do not rank hill-climb output by the climb's own score** at this length. Use an
  independent quadgram score.
- **Machine reading of the scans.** Open-source OCR is reliable for serial numbers, dates
  and keyword search, and garbles words. A small vision model transcribes these pages well
  but runs out of context after about 45 page images, so give it at most 40 per task, and
  score its output against the OCR text to catch stubs (`archives/transcripts_QA.tsv`).
- **The decrypt volumes are not chronological by time of origin.** They are in order of
  issue. Traffic from the afternoon of 1 May is scattered over the following five days.

---

## 10. Credits

Michael Hörenberg, for the photographs, transcriptions and breaks of the U-534 messages,
and Dan Girard for the analysis of this form. The National Archives (Kew) for the DEFE 3
series. Olaf Ostwald and Frode Weierud, whose published method the ciphertext-only search
follows. The authors of the open-source tools listed in `NOTICE.md`. Daniel Palloks for
the Universal Enigma simulator used as a reference.
