# 10 — Transcription check of P1030680 (form image vs published ciphertext)

Date: 2026-09-23. Source image: `https://enigma.hoerenberg.com/cms/download.php?cat=75_Unbroken&file=P1030680_VROL_NMKA_WS.jpg`.
It is 800×1153 px, a 2008 Panasonic DMC-FX8 photo. It is the **only** resolution the site serves: there is no gallery copy, and Wayback holds the same file (same SHA, 2014–2025).
Crops and enlargements were made with PIL (5–7× Lanczos, contrast; blue channel / stain-suppression for the Uhrzeitgruppe). Working files were not kept.

## Bottom line

- **The published 72-letter string cannot be independently re-verified letter by letter from this scan.**
  - Each letter is about 20×25 px of pencil.
  - The operator wrote **lowercase German-style Latin cursive**, not block capitals: `v r o l / n m k a / j c r s / a j t g / …`.
  - In that hand, u/n/v/w, c/e/l, g/q/y/z, h/k/t, f/long-s/l and r/v/n are routinely ambiguous even at full resolution.
  - What I can do is (a) check that every published letter is a *plausible* reading of its glyph, and (b) flag the glyphs where another letter is at least as plausible at this resolution.
  - (a) holds for all 72 positions: no published letter is implausible. (b) gives the table below.
- **What is verified:**
  1. Positions 0–5 `JCRSAJ`. The operator's own pencilled trial decrypt `IPZA YK` (Buchgruppen column, rows 3–4) is reproduced exactly from the published `JCRSAJ` with Potsdam 1 May / PUYY / rings VCCH (research/06).
  2. Group arithmetic: Gruppenzahl reads **22**, and rows 1–22 of the Funkgruppen column are filled: 2 indicator + 18 body + 2 repeated indicator = 72 body letters.
  3. Indicators: row 1 `vrol`, row 2 `nmka`, row 21 `vaol`, row 22 `nmka` (Girard's VR/VA discrepancy is visible). The `r` in row 1 is written over in darker pencil.
  4. The published string has been byte-identical on the site since at least 2014 (Wayback copies 2014-02-09, 2016-04-25, 2021-05-14).
- **Transcription errors vs reception garbles.**
  - The dominant risk is **not** transcription (Hörenberg transcribed from the physical forms or better photos, and his other 49 transcriptions decrypt cleanly apart from reception garbles).
  - It is **reception garbles by the 1945 operator**, which no image can reveal.
  - Corpus rate: P1030698 has 2 garbles in 72 letters, P1030697 about 5 in 76, P1030666 2 in 68, P1030694 1–2 in 60.
  - This message's own indicator repeat differs in 1 of 8 letters (VROL/VAOL).
  - **Expect 1–5 reception garbles in the 72-letter body.** That is why the bombe runs use k≥1 wherever the crib length allows.

## Header fields (read from the scan)

| field | reading | confidence | remarks |
|---|---|---|---|
| von: | a single slanted stroke "/" | – | no callsign |
| an: / um … Uhr | **`10 30`** (30 written raised) | high | written **above the "um ___ Uhr" line**. On every other form checked (P1030682 `18 46`, P1030690 `21 ..`, P1030693 `21 56`, P1030694 `22 02`, P1030698 `23 13`, P1030699 `00 02`) the same place holds the **reception time**. So this message was **received at 10:30**. |
| Datum | `1.5.45` | high | |
| Welle | **`NB`** followed by a small mark (`+`/`4`?) | high for NB | P1030682, P1030694 and P1030699 were also received on NB. P1030667 (Hertha), P1030693 (Otto Wünsche) and P1030701 (U-10..) report "Schaltung NB" for their Baltic transits. NB was the working circuit of the western-Baltic transit traffic on 30 Apr–1 May. |
| Uhrzeitgruppe | **`??5[3/8] / [70/30]`** | low | The first 1–2 digits are under the rust/water stain; blue-channel enhancement did not recover them. The digit after the 5 is a 3 or an 8. After the slash, `70` (flat-topped 7) or German-style `30`. |
| lfde. Nr. des Eingangs | `E` (Eingang) | high | same `E` on all forms checked |
| Anschriften / Funknamen | blank, under the stain | – | nothing legible; probably never filled in |
| Gruppenzahl | **22** | high | |
| Bemerkungen | a paraph / initials | – | |
| right-hand column | `14`, `9` (K-Buch rows), `PUYY` (circled), **`M-Thetis`**; at the top a struck-out `ELKC` | high | as Girard describes |

**Uhrzeitgruppe calibration.** On the other NB forms the Uhrzeitgruppe is `TOO/serial`:
- P1030694: `?195?/96`. The 534-texts.pdf gives its TOO as 195x.
- P1030693: `20?1/95`.
- P1030690: `20?4/97`.
- P1030682: `…/100`.

The serial after the slash is a running number shared by different originators around 20:00–22:00 on 1 May (95, 96, 97, 100). It looks like a circuit or broadcast serial, not a per-originator one.

A serial of **70 for a message received at 10:30** fits that sequence (about 3 messages per hour), and favours the `/70` reading over `/30`. TOO minus reception lag on the other forms is 0–2 h (195x → 22:02, 2011 → 21:56). So the TOO is most likely **`0953`/`0958` or `0853`/`0858`** (for a `??5[3/8]` reading with TOO ≤ reception time), with `05xx`–`07xx` possible if the backlog was longer. That puts it in the early-to-mid morning of 1 May, while Kiel was under the Festungsalarm (04:15 to about 13:10 per 534-texts.pdf).

## Per-position confidence table (72 body letters)

`row.col` = form row (3–20) and group column (1–4). Confidence:
- **H\*** = verified independently (operator's IPZAYK decrypt);
- **H** = the glyph is distinctive and I see no competing reading;
- **M** = published reading plausible, one alternate possible at this resolution;
- **L** = an alternate is at least as plausible as the published letter *at 800 px*. This does **not** mean the transcription is wrong: Hörenberg worked from better material.

| pos | row.col | published | conf | plausible alternates | note |
|---|---|---|---|---|---|
| 0 | 3.1 | J | H* |  | operator decrypt IPZAYK |
| 1 | 3.2 | C | H* |  |  |
| 2 | 3.3 | R | H* |  |  |
| 3 | 3.4 | S | H* |  | written darker (re-inked) |
| 4 | 4.1 | A | H* |  |  |
| 5 | 4.2 | J | H* |  |  |
| 6 | 4.3 | T | H |  |  |
| 7 | 4.4 | G | H |  |  |
| 8 | 5.1 | S | M | F (long-s form) |  |
| 9 | 5.2 | J | M | I | dot visible |
| 10 | 5.3 | E | L | G, Q | glyph is a looped descender form, closer to g/q than to a plain e |
| 11 | 5.4 | Y | M | Z | descender "ʒ"-like |
| 12 | 6.1 | E | L | G | double-loop "8"-like glyph, resembles row-17 g |
| 13 | 6.2 | X | M | T | thin cross |
| 14 | 6.3 | Y | M | G, Q |  |
| 15 | 6.4 | K | M | H | k/h ascender loop |
| 16 | 7.1 | K | M | H |  |
| 17 | 7.2 | Z | M | R | heavily over-written (correction?) |
| 18 | 7.3 | Z | M | R | "2"-like z |
| 19 | 7.4 | S | M | A, O | open s |
| 20 | 8.1 | H | L | F, L | very faint long ascender |
| 21 | 8.2 | V | L | W, A, U |  |
| 22 | 8.3 | U | H |  | u with breve (ŭ) - unambiguous |
| 23 | 8.4 | O | H |  |  |
| 24 | 9.1 | C | M | E |  |
| 25 | 9.2 | T | L | K, H | looks like k/h |
| 26 | 9.3 | R | M | V, N |  |
| 27 | 9.4 | F | M | S (long-s) |  |
| 28 | 10.1 | R | L | N |  |
| 29 | 10.2 | C | L | E, L |  |
| 30 | 10.3 | R | M | V, N | re-inked |
| 31 | 10.4 | P | H |  |  |
| 32 | 11.1 | F | L | L | no clear f cross-bar; resembles the l of rows 12/13 |
| 33 | 11.2 | V | M | U, W | no breve |
| 34 | 11.3 | Y | M | G |  |
| 35 | 11.4 | P | H |  |  |
| 36 | 12.1 | L | H |  |  |
| 37 | 12.2 | K | M | H |  |
| 38 | 12.3 | P | M |  | re-inked/over-written |
| 39 | 12.4 | P | H |  |  |
| 40 | 13.1 | L | M | E |  |
| 41 | 13.2 | G | M | Q, Y | re-inked |
| 42 | 13.3 | R | M | V, N |  |
| 43 | 13.4 | H | L | Y, G | descender form |
| 44 | 14.1 | V | M | U, W |  |
| 45 | 14.2 | V | L | O, R |  |
| 46 | 14.3 | B | M | H, K, L | re-inked |
| 47 | 14.4 | B | L | D, X |  |
| 48 | 15.1 | T | M | X |  |
| 49 | 15.2 | B | M | H, K |  |
| 50 | 15.3 | R | M | V |  |
| 51 | 15.4 | S | M | O, A |  |
| 52 | 16.1 | X | H |  | clear cross |
| 53 | 16.2 | S | H |  |  |
| 54 | 16.3 | W | M | U, M |  |
| 55 | 16.4 | X | H |  |  |
| 56 | 17.1 | G | M |  | 8-like g |
| 57 | 17.2 | G | H |  |  |
| 58 | 17.3 | T | M | F |  |
| 59 | 17.4 | Y | M | Z | "ʒ"-like |
| 60 | 18.1 | T | L | H, K |  |
| 61 | 18.2 | V | M | W, U |  |
| 62 | 18.3 | K | M | H | re-inked |
| 63 | 18.4 | Q | M | A, G |  |
| 64 | 19.1 | N | M | U, R |  |
| 65 | 19.2 | G | L | B, S |  |
| 66 | 19.3 | S | M |  |  |
| 67 | 19.4 | C | L | E, L, D | tall stroke |
| 68 | 20.1 | H | M | L, F |  |
| 69 | 20.2 | V | M | U, W |  |
| 70 | 20.3 | G | H |  |  |
| 71 | 20.4 | F | M | S, L |  |

**Summary.** 6 H\*, 13 H, 39 M, 14 L.

L positions (0-based): **10, 12, 20, 21, 25, 28, 29, 32, 43, 45, 47, 60, 65, 67**.

Recommended `?`/`--excl` set for bombe runs (at most 8, to keep menus loop-rich; the most doubtful glyphs): **10, 12, 20, 25, 32, 47, 60, 67**.

Glyphs that are re-inked or over-written in darker pencil: 3, 17, 30, 38, 41, 46, 62, plus the `r` of indicator row 1. That can mean the operator corrected his copy, for example during the repeat or with a second reception. These are not necessarily *less* reliable, but they are where he hesitated.

## Recommendations
- Treat the published string as the baseline. Use k≥1–3 garbles in bombe runs for **reception** errors, and use the L set above only as a secondary variant (`--excl`) for decisive long cribs.
- The only real fix is **higher-resolution imagery of the physical form**: it is at the Merseyside Maritime Museum / U-534 collection in Birkenhead, and Hörenberg photographed the originals. A request to Hörenberg for the original DMC-FX8 file (3072×2304) would settle the M/L positions and probably the Uhrzeitgruppe digits.
