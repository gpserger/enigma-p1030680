# Handwriting calibration for P1030680 (U-534, 1 May 1945)

Method: cropped 3–20x enlargements of the Funkgruppen column from `archives/forms/*.jpg`
(scratch crops not kept), read with the
Read tool, cross-checked letter by letter against `corpus/messages.json` decrypts via
`tools/decrypt_form.py` (and a locally-patched copy that fixes a `greek` rotor-letter bug —
the corpus uses `B`/`C` for Beta/Gamma, but `decrypt_form.py` expects `B`/`G`; this only
matters for the M4-438-wheel messages P1030682/83/etc. and does not affect the committed
tool or any corpus data).

## 1. Do the six forms share hands?

**No — three different hands, not one.**

- **P1030679 and P1030680: the same hand.** Both show a fast, connected cursive with:
  - **Q** written as a tight closed loop with a diagonal tail, essentially a script "2"
    (P1030679 row 3 col 1, row 24 col 1; P1030680 row 18 col 4, row 1 col 3 of the tail
    indicator).
  - **E** written as an unmistakable double loop ("figure-8"/pretzel), very different from
    a simple oval — P1030679 row 7 col 3 (verified E in TWEW) is essentially a pixel-for-pixel
    match for P1030680 row 6 col 1 (E in EXYK).
  - **H, T and F all collapse to the same minimal single diagonal stroke** with no reliable
    crossbar or serif — visible in both forms (P1030679 row 3 col 3 H, row 5 col 4 H;
    P1030680 row 8 col 1 H, row 9 col 2 T, row 17/18 T, row 20 H/F).
  - **U** gets a small breve/arc mark over a double-arc bowl, distinguishing it from N/M
    (P1030679 row 9 col 4; P1030680 row 8 col 3).
  - Same overall slant, loop size, header numeral style, and both use the same "M‑Thetis"/
    plain-block annotation style in the Bedeutung column.
  - This also matches the task's working hypothesis (16:15 and 16:30 receptions, same
    operator) and the human reader’s independent read.

- **P1030668, P1030672 and P1030683: a second, shared hand.** Noticeably more upright and
  block-like than the 679/680 hand: **Z** is written almost like a printed Z with a clear
  crossbar (P1030668 row 1 col 2/col 4, "OZNZ"), **O** is a clean round circle, strokes are
  shorter and less looped, and the writing is smaller/tighter overall. P1030672's and
  P1030683's indicator rows (row 1–2) and body letterforms (C as a simple open curve, D/G as
  plain diagonal strokes) match this same style closely.

- **P1030681: a third, distinct hand.** Much larger, more sprawling loops throughout (see
  the DUHF/TETO indicator rows), a different pen pressure, and none of the diagnostic
  679/680 shapes (no figure-8 E, no minimal H/T/F stroke, no breve-marked U). Clearly not
  the same writer as either other group.

Summary: **{P1030679, P1030680}**, **{P1030668, P1030672, P1030683}**, **{P1030681}** —
three hands across six forms.

## 2. Confusion table for the P1030680 hand (= P1030679's hand)

| Pair | Why confusable | Evidence |
|---|---|---|
| **T / F / H** | All three reduce to a single unadorned diagonal stroke with no distinguishing crossbar/serif in this hand — the single biggest confusion family. | P1030679 has no clean 3-way contrast in one row, but P1030680 rows 8, 9, 17, 18, 20 all show this same bare-stroke shape for published H, T and F alike; the human reader independently flagged exactly these positions (20, 58, 60, 68) with F/T alternates. |
| **Q / Z** | Both are loop-plus-diagonal-tail shapes ("2"-like); Q's loop is usually a bit tighter/closed, Z's a bit more open/hooked, but the family resemblance is strong. | P1030679 row 3 col 1 (Q) vs. row 23 col 1 (Z, verified) — similar general shape, different closure. |
| **B / D** | Both are loop + rising-diagonal-stroke shapes of similar silhouette. | P1030679 row 8: col 3 (verified D) and col 4 (flagged B→D) look alike; this is the clearest case of a genuine transcription misreading found in the corpus. |
| **C / E** | Normally distinct (C = single open curve, E = double loop) but a hurried/small E can lose its second loop and look like a rounded C, or vice versa. | P1030680 row 19 col 4 (pos 67, published C) — a small rounded loop that is not a clean match for either the sharp open "c" (row 3 col 2, row 9 col 1) or the full double-loop E (row 6 col 1). |
| **E / A / G / Q (loop family)** | All are closed- or partly-closed-loop shapes; a compressed or multi-stroke loop can drift between them. | P1030680 row 5 col 3 (pos 10, published E) is an atypical multi-stroke "fan" shape that matches neither the clean E archetype nor the simple oval A archetype cleanly — genuinely ambiguous, and it sits exactly where the human reader also flagged low confidence with Q/G alternates. |
| **V / L / P** | All loop-based ascender/descender shapes; ink bleed between rows (long descenders into the row below) can merge them visually. | P1030680 rows 17–18: the descender of row 17's T/col3 stroke runs into row 18's col 1, making the two look like one continuous mark. |

## 3. Verdicts for the unverified reference positions

Verdict key: **(a)** ink clearly/plausibly matches the *published* (transcribed) letter →
likely a genuine reception/operator garble, not a transcription error; **(b)** ink
clearly/plausibly matches the *fixing* letter → likely a transcription misreading;
**(c)** undecidable from the available scan.

### P1030679 (same hand as P1030680 — most reliable comparisons)

| pos | row/letter | published→fix | verdict | confidence | evidence |
|---|---|---|---|---|---|
| 0 | row 3, l1 | Q → Z | (a), weakly | moderate | Glyph is a tight closed loop matching this hand's clean Q archetype (row 24 col 1) more closely than the more open verified Z (row 23 col 1) — but Q/Z are a recognized confusable pair here, so not fully certain. |
| 10 | row 5, l3 | E → A | **(a)** | **high** | Glyph is an unmistakable double-loop ("figure-8"), essentially identical to the verified E at row 7 col 3 (TWEW). This is clearly a genuine reception garble, not a misreading — the paper really does say E. |
| 23 | row 8, l4 | B → D | **(b)** | moderate–high | Glyph closely matches the verified D in the very same row (col 3): same loop + rising-diagonal structure, just re-inked/bolder. Classic misreading. |
| 58 | row 17, l3 | R → P | (b), weakly | low–moderate | Heavy ink bleed with row 18 obscures the shape; on balance its rounded-loop character resembles this row's own P glyphs (cols 1–2) more than the clean minimal-stroke R seen elsewhere (row 9 col 1). Image quality here is poor. |

### P1030668 (668/672/683 hand)

| pos | row/letter | published→fix | verdict | confidence | evidence |
|---|---|---|---|---|---|
| 55 | row 16, l4 | I → S | (a), weakly | low | Glyph is a minimal tick/dash, consistent with a bare "I"; no resemblance to the more elaborate squiggle-loop letterforms elsewhere on the form. Too minimal for real confidence. |
| 139 | row 37, l4 | S → H | (c) | low | Rounded loop+tail shape; no clean, unambiguous H example elsewhere on this form to calibrate against. |

### P1030672 (668/672/683 hand)

| pos | row/letter | published→fix | verdict | confidence | evidence |
|---|---|---|---|---|---|
| 5 | row 4, l2 | S → I | (c), weak lean (b) | low | Glyph is a small closed round loop sitting high in the cell — atypical for this hand's more open S-squiggle, but not a clean match for a bare "I" stroke either. |
| 129 | row 35, l2 | U → A | (b), weakly | low–moderate | Small single loop, high in the cell — more consistent with a compact "a" than a double-bowed "u". |

### P1030681 (distinct third hand)

| pos | row/letter | published→fix | verdict | confidence | evidence |
|---|---|---|---|---|---|
| 82 | row 23, l3 | L → R | (a), weakly | moderate | Glyph closely resembles this row's own verified L (col 1) — same loop shape. |
| 96 | row 27, l1 | B → M | (a), weakly | moderate | Glyph resembles this row's own verified B (col 2) — same loop. |
| 143 | row 38, l4 | T → U | (c) | low | Rounded double-loop shape; this hand has no calibrated T/U contrast available nearby. |
| 148 | row 40, l1 | P → G | (c) | low | Descending-hook shape, atypical of this hand's usual loop-heavy letterforms; no comparison available. |
| 190, 314, 346, 367 | rows 50, 81, 89, 94 | — | **not checkable** | — | P1030681 is a 372-letter message (93 body rows); the printed form template only runs to row 40 (positions 0–151), which is all that is visible in `archives/forms/P1030681.jpg`. Positions 190/314/346/367 are on later physical sheets that are not present in the archive — no image exists to check them against. |

### P1030683 (668/672/683 hand)

| pos | row/letter | published→fix | verdict | confidence | evidence |
|---|---|---|---|---|---|
| 10 | row 5, l3 | K → V | (c) | low | Diagonal-stroke-with-hook shape; no dedicated K/V contrast available in this hand. |
| 11 | row 5, l4 | L → Z | (a), weakly | moderate | Clean rounded loop, matching this hand's general loop-based letterforms; looks like a plausible L. |
| 34 | row 11, l3 | K → X | (c) | low | Loop+cross-stroke shape; no dedicated K/X contrast available. |
| 37 | row 12, l2 | W → V | (c) | low | Row 12 is faint/low-contrast in the scan; not legible enough for a confident call. |
| 49 | row 15, l2 | V → X | (c) | low | Same — insufficient legibility in the available scan for this row. |
| 65 | row 19, l2 | D → A | (c) | low | Same — insufficient legibility in the available scan for this row. |

**Tally:** of the 22 flagged reference positions across all five broken forms, only
**2 are confident transcription misreadings** (P1030679 pos 23, and weakly pos 58), **1 is a
confident reception garble** (P1030679 pos 10, clearly E not A), and the rest are weak leans
or genuinely undecidable given scan resolution, ink bleed, or (for P1030681's last four)
sheets that simply are not in the archive. This is a much lower "misreading" rate than one
might hope for — most of these garbles look like they really were sent/received wrong, not
misread off the paper by the transcriber.

## 4. Full 72-position reading of P1030680

Read against the P1030679/P1030680 shared-hand gallery (Q="2"-loop, G="9"-loop, E="figure-8"
double loop, H/T/F=bare diagonal stroke, U=breve-marked double arc, S/R=hook shapes,
C=open curve, V/L/P=loop family). Confidence: H=high, M=moderate, M-H=moderate-high, L=low.
Every position below reads the same letter as published; no position produced positive
evidence for a different definite letter. The interesting result is *where* the reading is
low-confidence, not that it disagrees.

| pos | published | reading | alternates | confidence | note |
|---|---|---|---|---|---|
| 0 | J | J | — | M-H | small hook/dot mark, consistent with this hand's minimal J |
| 1 | C | C | — | **H** | clean simple open curve; does **not** resemble this hand's figure-8 E — disagrees with the tentative "E" alternate |
| 2 | R | R | — | H | hook shape |
| 3 | S | S | — | M-H | oval+tail |
| 4 | A | A | — | H | simple loop |
| 5 | J | J | — | M-H | stroke+dot |
| 6 | T | T | F | M-H | diagonal+cross; T/F/H family |
| 7 | G | G | — | H | "9"-loop |
| 8 | S | S | — | M | diagonal hook |
| 9 | J | J | — | M-H | stroke |
| 10 | E | E | Q, G | **L** | atypical multi-stroke "fan" shape; matches neither the clean E archetype nor a simple A oval cleanly (see confusion table) |
| 11 | Y | Y | — | M-H | zigzag |
| 12 | E | E | — | **H** | clean double loop, unambiguous match to this hand's E |
| 13 | X | X | — | H | cross shape |
| 14 | Y | Y | — | M | curl |
| 15 | K | K | — | M-H | hook |
| 16 | K | K | — | M-H | hook |
| 17 | Z | Z | — | M | open squiggle |
| 18 | Z | Z | — | M | open squiggle |
| 19 | S | S | — | M | loop |
| 20 | H | H | F, T | M | bare diagonal stroke; T/F/H family, matches the human reader’s low-confidence flag |
| 21 | V | V | — | M-H | loop |
| 22 | U | U | — | M-H | breve-marked double arc |
| 23 | O | O | — | H | clean circle |
| 24 | C | C | — | H | open curve |
| 25 | T | T | F | M | angular diagonal+cross; T/F/H family |
| 26 | R | R | — | M-H | hook |
| 27 | F | F | — | M-H | bare stroke |
| 28 | R | R | — | M | hook |
| 29 | C | C | — | M-H | open curve |
| 30 | R | R | — | M | ascender loop, slightly different flourish than pos 28's R but same row context |
| 31 | P | P | — | M-H | loop |
| 32 | F | F | — | M-H | bare diagonal stroke, matches F archetype well |
| 33 | V | V | — | M-H | loop |
| 34 | Y | Y | — | M | zigzag |
| 35 | P | P | — | M-H | loop |
| 36 | L | L | — | M-H | loop |
| 37 | K | K | — | M | hook+cross, bold |
| 38 | P | P | — | L-M | dark ink blob / overwritten stroke, hard to isolate cleanly |
| 39 | P | P | — | M | loop |
| 40 | L | L | — | M-H | loop |
| 41 | G | G | — | M-H | loop |
| 42 | R | R | — | M | zigzag "M"-ish flourish |
| 43 | H | H | — | M | bare diagonal stroke |
| 44 | V | V | — | M | hook |
| 45 | V | V | — | M-H | loop |
| 46 | B | B | — | L-M | blotted/ambiguous shape |
| 47 | B | B | — | M | rounded closed loop, matches this row's B family |
| 48 | T | T | — | M | diagonal+cross |
| 49 | B | B | — | M-H | loop |
| 50 | R | R | — | M | squiggle |
| 51 | S | S | — | M | loop |
| 52 | X | X | — | H | clean cross |
| 53 | S | S | O | M | closed loop, could be read as O-like but accepted as S in context |
| 54 | W | W | — | M-H | zigzag |
| 55 | X | X | — | M-H | cross-ish |
| 56 | G | G | — | M-H | loop |
| 57 | G | G | — | M-H | loop |
| 58 | T | T | F | M | long diagonal stroke bleeding into row 18; T/F/H family, matches the human reader’s flag |
| 59 | Y | Y | — | L-M | small mark/hook |
| 60 | T | T | F | M | continues the same stroke bleeding up from row 17 col 3; T/F/H family, matches the human reader’s flag |
| 61 | V | V | — | M | zigzag/loop |
| 62 | K | K | — | M | bold dark cluster |
| 63 | Q | Q | — | H | clean closed loop |
| 64 | N | N | — | M-H | small hook |
| 65 | G | G | — | M-H | loop |
| 66 | S | S | — | M | oval+tail |
| 67 | C | C | E | **L-M** | small rounded loop — genuinely ambiguous between C and E in this hand, matches the human reader’s C/E flag |
| 68 | H | H | F, T | M | bare diagonal stroke; T/F/H family, matches the human reader’s flag |
| 69 | V | V | — | M | diagonal variant |
| 70 | G | G | — | M-H | clean loop |
| 71 | F | F | T | M-H | diagonal stroke with small hook, reasonable F match |

## 5. Header readings (P1030680)

- **Reception time:** "um 1630 Uhr" — the digits are written straddling the "an:" and
  "um ___ Uhr" lines; first two digits read as "16" (the 6 is swirly and could in principle
  be misread as 0), second pair clearly "30". Consistent with the human reader’s reading and with the
  16:30 reception time already given in the task background.
- **Datum:** "1.5.45" — clear and unambiguous.
- **Uhrzeitgruppe:** badly damaged by a burn/water stain that covers most of the leading
  digits. What's legible: "…53/" then, just past the box divider, "70" and a separate "E"
  (most likely an "Eingang"/incoming-message marker — the same kind of "E" annotation appears
  in the Uhrzeitgruppe box on P1030679). Could not independently confirm whether the
  stain hides a repeated digit that would make this "552/70" rather than "53/70" — the ink
  under the stain is not recoverable at this resolution. Consistent with, but not fully
  confirming, the human reader’s "?552/70" reading.
- **"M-Thetis"** is written clearly in the Bedeutung Blatt A column, matching
  `corpus/messages.json`'s own hedge ("Note 1: Probably Key M-Thetis") for P1030680.
- **Gruppenzahl:** "22" — consistent with 2 head-indicator groups + 18 body groups (72
  letters) + 2 tail-indicator groups = 22 total four-letter groups on the form.
- **Indicator repeat:** the head (rows 1–2) reads clearly as **VROL** (bold, distinct R).
  The tail (rows 21–22) repeats V_OL but its second letter is a softer, rounder shape that
  looks more like A or D than the head's bold R — genuinely ambiguous, matching the human reader’s
  "VAOL or VDOL" note. Since the indicator must be identical top and bottom, VROL (matching
  the clearer head reading) is the more likely intended value, with the tail glyph simply a
  sloppier rewrite of the same R.
