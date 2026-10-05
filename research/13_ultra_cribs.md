# 13 — Bletchley (DEFE 3/577) decrypts as a crib source for P1030680

Date: 2026-09-24.
Inputs:
- `archives/transcripts/DEFE-3-577_*` (737 files at the final pass, covering 23 Apr – 2 May 1945 by TOO);
- Hörenberg's ULTRA page and the 50 U-534 form photos (fetched to a scratch directory, not kept);
- `534-texts.pdf`;
- `corpus/messages.json`.

Scripts and outputs are in `runs/ultra13/`:
- `gen.py` is the p082 variant generator plus the collision screen;
- `p082_variants.tsv`;
- `run.sh` and `run2.sh`;
- `R*.log`, `R*.tsv` and `R*.rank.tsv`.

## 0. Bottom line

1. **Correction to research/10 and research/11: P1030680 was almost certainly not received at 10:30, and was not sent during the morning Festungsalarm.**
   - The broadcast serial after the slash in the Uhrzeitgruppe, `/70`, rises approximately monotonically with reception time on U-534's forms (table in §1): serials 53–55 at 10:10–10:46, 69 at 16:15, 80–86 at 18:12–19:52.
   - Hörenberg's photo frame numbers are also chronological, and they bracket P1030680 between P1030679 (received 16:15) and P1030681 (18:xx).
   - So the message came in at about **16:15–17:30 on 1 May, most probably 16:30**. The handwritten hour looks like "10" at 800 px, but "16" is the reading consistent with everything else.
   - Its TOO is `??53` or `??58`, and ≤ reception. Serials 60–69 carry TOOs 1123–1157, so the candidate TOOs are 1153/1158 (only just before the noon key change) or 1253…1558.
2. **The Potsdam key changed at 12:00 (noon), not midnight.** This is now settled from Bletchley's TOOs and U-534's own forms (§2).
   - The "30 Apr" key in Hörenberg's labelling runs 30 Apr 12:00 – 1 May 12:00.
   - For P1030668 (TOO 1153), the operator used the "30 Apr" key. For P1030680 he tried the "1 May" key (IPZAYK).
   - If he applied the same rule, P1030680's TOO is **≥ 12:00**, which points to 1253/1258 … 1553/1558. That would make it a message on the first Thetis key of the May sheet.
3. **The DEFE 3/577 transcripts do not reach that window.**
   - Measured by TOO, which is the only field that matters here, 577 stops at 1159 on 1 May.
   - A few late TOIs are not counter-examples. 577_4_p172 is TOO 1156 with TOI 1424. 577_4_p2 ("TOI 1843") is a mistyped duplicate of p002 (TOI 0843). Correction notes also appear.
   - The 737 files include about 40 unpadded duplicates (`577_4_p1…p40`) next to `p001…p040`. That explains 737 against 696 pages. My screens globbed both sets, so nothing was missed.
   - Afternoon traffic of 1 May in the home-waters ZTPG series continues in DEFE 3/578. Hörenberg's P1030673, TOO 1143 on 1 May, is DEFE 3/578 p. 722, so 578 overlaps even the late morning.
   - DEFE 3/743 is on disk, with OCR in `archives/ocr/`. It is the operational U-boat series (ZTPGU 38000–38999, 9 Apr–4 May), and it does reach 1 May afternoon (743_05 p174–p205).
   - I screened it. Its 1 May TOOs of 1200–1630 are 1432, 1443, 1451, 1543, 1545 and 1636. All come from Captain U/B West, 5./11./15. Flott or Norway boats. None ends in 53 or 58, and none is training-net.
   - **So the prime candidate for a sister decrypt is in DEFE 3/578, which has not been transcribed.**
4. **p082 (Captain (U/B) Training, TOO 1119, "chart outfits for Norway") is not P1030680 as a same-text copy.** It fails three independent checks:
   - its TOO 1119 does not fit `??5[3/8]`;
   - it falls on the wrong key day by the operator's rule;
   - the Rosetta length ratio (German/Bletchley-English letters = 1.31 pooled over 7 pairs) predicts a German original of about 176 letters. The shortest possible German rendering of p082's full content, with its address, is 84 letters; stripped to the bone it is 91. A 72-letter version must be abridged and re-addressed.

   Abridged, re-addressed reconstructions (direct to "Otto Wünsche" / the Fangboote) were generated, screened and run through the bombe (§5–6). **Result: none of them decrypts.**
5. No plaintext was found. The table of what remains untested is in §7.

## 1. Reception time and the broadcast serial: re-reading all 50 U-534 forms

The `um … Uhr` field (reception) and the Uhrzeitgruppe (`TOO/serial`) were read on every U-534 form photo (800 px). `?` marks an unreadable digit. Where the DEFE twin is known, its TOO is given.

| form | received | Uhrzeitgruppe | DEFE twin TOO |
|---|---|---|---|
| P1030659 | 00:50 "1.5." | 23?4/**3** | – |
| P1030660 | 08:50 | 2359/50 | 2359/30/4 (577_4_p145) |
| P1030661 | 10:04 | ???5/54 | – |
| P1030662 | 10:10 | 0904/53 | 0904 (577_4_p160) |
| P1030663 | 10:20 | ?912/54 | 0912 (577_5_p025) |
| P1030664 | 10:46 | 1004/55 | 1004 (577_5_p018) |
| P1030665 | 11:50 | ?51/57 | – |
| P1030666 | 11:59 | 11??/58 | – |
| P1030667 | 12:20 | 1130/59 | 1130 (577_5_p036) |
| P1030668 | 12:33 | **1153**/60 | **1152** (577_5_p004) |
| P1030669 | 12:46 | 1123/61 | 1123 (577_5_p005) |
| P1030676 | 14:08 | ?57/62 | – |
| P1030671 | 13:11 | 10??/63 | – |
| P1030672 | 13:18 | 1157/64 | 1157 (577_4_p139) |
| P1030673 | 13:21 | 1143/65 | DEFE 3/578 p722 (Hörenberg) |
| P1030675 | 13:45 | 2212/66 | – |
| P1030679 | **16:15** | 1142/**69** | 1142 (577_5_p126) |
| **P1030680** | **"10 30"** | **??5[3/8]/70** | ? |
| P1030681 | 18:?? | ?55/? | – |
| P1030683 | 18:12? | ?/80 | – |
| P1030682 | 18:46 | research/10: `…/100`; my reading `1?38/1?0?` (uncertain, torn) | – |
| P1030684 | 19:25 | 1831/82 | – |
| P1030685 | 19:30 | ?/83 | – |
| P1030686 | 19:52 | 1842/86 | – |
| P1030687 | 20:20 | 1910/88 | – |
| P1030688 | 21:00 | 1927/89 | – |
| P1030689 | 21:16 | 2046/90 | – |
| P1030690 | 21:30 | 2044/92 | – |
| P1030691 | 21:30 | 2059/93 | – |
| P1030692 | 21:45 | 2042/94 | – |
| P1030693 | 21:56 | 2011/95 | – |
| P1030694 | 22:02 | 195?/96 | – |
| P1030696 | 22:56 | 2205/99 | – |
| 2 May forms (P1030700–709) | 00:45–04:35 | serials 6–17 | serial restarts at midnight |

What the table shows:
- **The serial is circuit-wide and restarts at midnight.** It rises approximately monotonically with reception, with local swaps of ±2–3 (676 /62 at 14:08 against 671 /63 at 13:11) and one unresolved outlier (682, read by research/10 as /100 at 18:46).
- P1030659 (00:50 "1.5.", serial 3) is unresolved. On 2 May, P1030699 is /4 at 00:02 and P1030700 is /6 at 00:45, so /3 at 00:50 is out of order either way.
- **Bletchley's TOO equals the German Uhrzeitgruppe, to ±1 minute** (668: 1153 on the form, 1152 at Bletchley). Reception comes 0.5–4.5 h after TOO, on the same clock.
- **P1030680's serial 70 is the next number after P1030679's 69 (received 16:15).** It falls well before serial 80 (about 18:12). A 10:30 reception would need serial ≈ 54.
  - Unless serials were counted separately per key net, the "10 30" is a misreading: the "6" in this hand is a small-looped glyph. Otherwise it is an operator slip.
  - The photo frame order (P1030679 → **P1030680** → P1030681) independently places the form in the late afternoon. There are local frame swaps (682/683), so this supports the serial evidence rather than proving it.
  - Even allowing ±3 of serial noise, 70 is about 15 away from the 53–55 of 10:00–11:00. research/10's alternative reading "/30" fits no time of day near 10:30 either.
- The research/10 argument ("serial 70 at 10:30 fits about 3 per hour") was made without the morning serials and is superseded.
- **Question for a high-resolution image request** (research/10 §end): is the hour "10" or "16", and what are the first two TOO digits?

## 2. The Potsdam key period is noon to noon

Messages on Hörenberg's "30 Apr" key with 1 May TOOs, confirmed by DEFE or by the forms:

| form | TOO on 1 May |
|---|---|
| P1030662 | 0904 |
| P1030663 | 0912 |
| P1030664 | 1004 |
| P1030667 | 1130 |
| P1030669 | 1123 |
| P1030668 | 1153 |
| P1030673 | 1143 |
| P1030679 | 1142 |
| P1030672 | 1157 |

Two 30 Apr TOOs sit on the same key: P1030675 (2212) and P1030660 (2359).

Messages on the "1 May" key include evening TOOs from 1831 on, and 2 May TOOs up to 0316 (P1030709).

- So the "30 Apr" key runs 30 Apr 12:00 → 1 May 12:00, and the "1 May" key runs 1 May 12:00 → 2 May 12:00. This matches P1030699's "ab 1.5. 1200 Uhr Schlüssel …" and the site's DGZ note.
- The only apparent exception is P1030659 (TOO 23?4, "1 May" key). It would be resolved if its "00 50 / 1.5." were really 2 May 00:50, but the serial does not settle this. Nine messages against one still favour noon.
- Research/05 left this question open. It is now closed.

**Consequences:**
- A P1030680 TOO of 1153 or 1158 would be on the Thetis key issued for **30 April** (April sheet). A TOO of 12:00 or later would be on the key for **1 May**, the first day of the May sheet.
- The operator, who could read the TOO, tried the 1 May Potsdam settings. That favours ≥ 12:00.
- For research/09 (key-sheet hunt), both the last April and the first May Thetis settings matter, and 1 May is the better bet.

## 3. Rosetta: German originals against Bletchley's English

Seven U-534 messages have DEFE twins in the transcripts (plus P1030673 on Hörenberg's page, in 3/578). Letters are counted without spaces or punctuation.

| U-534 | German letters | DEFE page | Bletchley letters (header addr + body) | G/E |
|---|---|---|---|---|
| P1030660 U-1103 → FdU Ost | 140 | 577_4_p145 | 112 | 1.25 |
| P1030662 4. Flott → U-4701 | 160 | 577_4_p160 | 101 | 1.58 |
| P1030667 Hertha | 176 | 577_5_p036 | 131 | 1.34 |
| P1030668 TF 19 | 172 | 577_5_p004 | 166 | 1.04 |
| P1030669 FdU Ausb → 5. Flott (SSD) | 288 | 577_5_p005 | 211 | 1.36 |
| P1030672 U-1103 → FdU Ost (Ing) | 156 | 577_4_p139 | 88 | 1.77 |
| P1030679 Krefeld | 88 (German truncated) | 577_5_p126 | 91 | 0.97 |
| **pooled** | | | | **1.31** |

**Mapping rules learned:**

| German (corpus spelling) | Bletchley rendering |
|---|---|
| `FXDXUUUAUSB`, `FDUUUAUSB`, `FXDXUUUAUSBILDUNG` | CAPTAIN (U/B) TRAINING |
| `FXDXUUUOST`, `FDUUUOST` | CAPTAIN (U/B) BALTIC ("(ENG.)" = `KKINGXKK`) |
| `KOMXADMXUUUBOOTE` | ADMIRAL (U/B). The staff-branch suffix `PPPEINS` becomes "P 1"; "U 1", "N 3" and "F 4" are the same kind of suffix, and p082's "CAPTAIN (U/B) TRAINING U 1" is one. |
| `CHEFFUNFXUUUFLOTT`, `CHEFVIERXUUUFLOTT` | S.O. 5TH / 4TH U/B FLOTILLA |
| `ZWOSECHSXUUUFLOTT` | 26TH U/B FLOTILLA |
| `SSSMMMHHHSSSOST` | SHIP MOVEMENT MAIN REPORTING STATION BALTIC |
| `HAKAXKIEL` | NAVAL HARBOURMASTER KIEL |
| `STUEKOWARNEMUENDE` | BASE COMMANDANT WARNEMUENDE |
| `UUUSTUETZPUNKTLUEBECK` | U/BOAT BASE LUEBECK |
| `TTTFFFEINSACHT`, `TORPXFANGBOOTEINSNEUN` | TF 18, TF 19 |
| `NACHR` | "RPTD. TO:" / "REPEATED" |
| `SSD` | EMERGENCY |
| `KR KR` | MOST IMMEDIATE (577_4_p061, the 5. Flott Festungsalarm "to all boats") |
| `J…J` around names (`JSAARJ`, `JNYBORGJ`, `VONVONJOTTOWUENSCHEXX`) | 'quoted' for ships ('SAAR', 'KURIER'). Place names drop the quotes and the repetition (`JNYBORGJJNYBORG` → NYBORG). |
| numbers spelled out (`EINSEINSDREINULUHR`) | digits (1130). `EINSKK … ZWOKK` → "1) … 2)". `KK…KK` → ( … ). |
| sign-off at the end (`XXFXDXUUUAUSBXKF`) | moved into the FROM: header; the operator filler `KF` is dropped |
| `BESTAETIGUNGERBETEN` | PLEASE ACKNOWLEDGE |
| `FFFTTTBLEIBTBESETZT` | W/T WATCH IS BEING MAINTAINED |
| `NB BESETZT` (Hertha) | "W/T WATCH ON 4860 KC/S" |
| `ZWOSTELLKARTENAUSRUESTUNG…` | TWO SETS OF CHARTS |
| `LFDXBEFXBBBDDDUUUOOOPPPNRXSIEBEN` | SERIAL ORDER OF B.D.U. (OPS.) NO. 7 |

The last two rows need comment:
- **NB is 4860 kc/s.** Bletchley itself equates the two.
- Almost every U-534 NB message with a DEFE twin is on 4860 kc/s. P1030679 (Krefeld) was intercepted on 382 kc/s and relayed on NB, so re-broadcast across circuits did happen.

Reading p082 through these rules gives the following probable German:
- "EMERGENCY" = `SSD`.
- "ENSURE AND CONFIRM" = `SICHERSTELLEN UND MELDEN`, or `… BESTAETIGEN`. FdU Ost uses `SICHERSTELLEN` in 534-texts 1.5.1303.
- "'OTTO WUENSCHE'" = `JOTTOWUENSCHEJ` (or `…EX`).
- "RECOVERY VESSELS ORDERED" = `BEFOHLENE FANGBOOTE` (no numbers, so not `TTTFFF nn`).
- "CHART OUTFITS FOR NORWAY" = `KARTENAUSRUESTUNG NORWEGEN` or `STELLKARTENAUSRUESTUNG …`. Otto Wünsche's own reply, P1030693 (TOO 2011), uses `NORWEGENKARTEN`.
- "ON BOARD" = `AN BORD (HABEN)`.

**P1030693 is the answer to p082:** "Norwegen-Karten in Warnemünde und Rostock nicht bekommen".

## 4. Candidate decrypts in DEFE 3/577

**Coverage.** The final list has 737 transcripts. 1 May TOOs run to 1159. The latest 1 May TOI is 12:34 (577_4_p116), apart from correction notes (577_4_p001, TOI 2210) and 577_4_p003 (TOI 2300/1/5, TOO lost; a 10-boat Type XXIII order, far over 72 letters).

The re-done pages 577_3_p192–p213 were included:
- the 67th V-Flotilla fuel table p200–p203;
- the Norway/Biscay situation report p204–p205.

These are Norway and Biscay circuits, long, and irrelevant.

**Screening criteria:**
- TOO `??5[3/8]`;
- TOO ≤ reception (16:15–17:30);
- key day;
- plausible Thetis community (FdU Ausb, training flotillas, TF boats, UAK, TVA);
- a 72-letter German original possible.

| DEFE | TOO | kc/s | from → to | content | verdict |
|---|---|---|---|---|---|
| 577_4_p082 | 1119 | 4282 | Capt (U/B) Training "U 1" → 25/26 Flott, EMERGENCY | ensure/confirm Otto Wünsche + ordered recovery vessels have Norway chart outfits | TOO pattern ✗, key day ✗ (operator's rule), length ≈176 ✗. Only an abridged re-addressed form is possible; tested in §6 |
| 577_5_p005 | 1123 | 4860 | Capt (U/B) Training → 5. Flott | = P1030669 (288 letters, own twin on U-534 form 1123/61) | ✗ (already on U-534, different text) |
| 577_5_p004 | **1152** (form: **1153**) | 4860 | TF 19 → Kom Adm, FdU Ausb, FdU Ost, SMHS, 25./5. Flott, HaKa Kiel | = P1030668, "12 Uhr Kiel eingelaufen, FT bleibt besetzt", 172 letters | **Only TOO match (`1153`).** A straight re-encipherment is impossible (172 ≠ 72, and KOMXADM@0 collides). A separately written short Thetis note by TF 19 was screened (§6) |
| 577_4_p195 | 1153 | 4580 | T 33 → Fleet, Naval Chief Cmd Baltic | fuel consumption table | ✗ fleet net, long |
| 577_3_p153/154 | 1158 | 4020 | 'Kurier' Bokel → SKL 2, BdU, SKL 4, MND | 4-point report | ✗ Kurier net, 4 items ≫ 72 |
| 577_4_p175 | 1158 | 382 | Ice-breaker Pregel → Hafenkapitän Sassnitz, 10. Sich.Div | typhus deaths, most immediate | ✗ eastern Baltic, long |
| 577_4_p103 | 1157 | 382 | Capt (U/B) Training (personal) → Adm U/B | 99 groups Offizier | ✗ (Offizier, 396 letters) |
| 577_4_p139 | 1157 | 4860 | U-1103 → FdU Ost (Ing) | = P1030672 (1157/64) | ✗ already on U-534 |
| 577_5_p126 | 1142 | 382 | Krefeld → Adm U/B, …, Mosel | = P1030679 (1142/69) | ✗ already on U-534, serial 69 |
| 577_5_p036 | 1130 | 4860 | Hertha → Stüko Warnemünde, 26. Flott, rptd FdU Ausb | = P1030667 | ✗ already on U-534 |
| 577_4_p160 / 5_p025 / 5_p018 | 0904 / 0912 / 1004 | 4860 | 4. Flott, U-4701 | = P1030662/663/664 | ✗ already on U-534 (serials 53–55) |
| 577_4_p061 | 0415 | 3764 | 5. Flott → all boats, MOST IMMEDIATE | Festungsalarm Kiel | ✗ time; `KRKRALLE`@0 impossible (research/08) |
| 577_5_p022 | 1754/26 Apr | 4282 | Adm U/B via Capt (U/B) Training → 4./25. Flott | detach 15 boats to Travemünde roads | ✗ date |
| 577_4_p123 | 0111 | 3565 | 25. Flott → Adm U/B, EMERGENCY | Messina / Kurassier | ✗ time, long |
| 577_4_p003 | ? (TOI 2300) | ? | ? (26. Flott named) | detach 10 Type XXIII to Swinemünde | ✗ ≫72 letters |

Other 1 May "TO ALL" items are dismissed:
- 577_3_p170, 3rd Coastal Defence Unit, TOO 0817;
- 577_3_p177, 2nd Coastal Defence Unit, crash-boat overdue, TOO 1139;
- the Norway V-boat and Northern Waters circuits (577_3_p171–p213).

None of them is a Baltic training net or fits `??5[3/8]`.

**Nothing in 577 matches TOO `??5[3/8]` except TF 19 (1152/1153), T 33, Kurier and Pregel.** Only TF 19 belongs to the Thetis community.

The corpus German texts of the same day (534-texts.pdf, 1 May) with TOO in 1200–1630 are 1303 (FdU Ost → U-291 and TF17), 1516 and 1636. None ends in 53 or 58.

## 5. p082 reconstructions and letter counts (`runs/ultra13/gen.py`)

**Blocks:**
- Priority: `SSD` or none.
- Address as in p082 (25./26. Flott): 5 spellings, e.g. `ZWOFUENFXUNDZWOSECHSXUUUFLOTTXX` (31) or `ZWOFUNFXZWOSECHSXUUUFLOTTXX` (27).
- Or address direct: `JOTTOWUENSCHEJYFANGBOOTEXX`, `OTTOWUENSCHEYFANGBOOTEXX`, `JOTTOWUENSCHEJXX`, `OTTOWUENSCHEXX`.
- Verb: `SICHERSTELLEN(UND MELDEN|UND BESTAETIGEN)(DASS)`, or verb-last `…SICHERSTELLENXMELDEN`.
- Subject: `(J)OTTOWUENSCHE(J)UND(BEFOHLENE)FANGBOOTE` / `…TTTFFFBOOTE`.
- Object: `NORWEGENKARTENANBORD(HABEN)`, `KARTENNORWEGENANBORD`, `KARTENAUSRUESTUNGNORWEGENANBORD`, `STELLKARTENNORWEGENANBORD`.
- Short direct bodies: `NORWEGENKARTENANBORDNEHMENXMELDEN`, `…XBESTAETIGEN`, `SOFORT…`, `STELLKARTENAUSRUESTUNGNORWEGENANBORDNEHMEN`, and others.
- Sign-off: none, `FXDXUUUAUSB`, `VVVFXDXUUUAUSB`, `X(X)FXDXUUUAUSB`, or `VVVFXDXUUUAUSBXX` as a header.

**Letter counts:**
- **As addressed (25./26. Flott), every variant is 84–145 letters.** The barest full-content form, `ZWOFUNFXZWOSECHSXUUUFLOTTXX OTTOWUENSCHEUNDFANGBOOTE NORWEGENKARTENANBORD SICHERSTELLENXMELDEN`, is **91**. So p082 as addressed cannot be 72 letters.
- The only 66–72-letter forms are **re-addressed direct to Otto Wünsche (+ Fangboote)**, with a short imperative body: 95 generated, 38 (variant, offset) pairs collision-free.
- **Structural finding:** every variant opening with `J` (`JOTTOWUENSCHEJ…`) collides at position 0 (ciphertext J). A J-bracketed ship name cannot be the first word.
- The offset-0 survivors all start `SSDOTTOWUENSCHE…`.

**The 10 offset-0 survivors (run R1):**
```
SSDOTTOWUENSCHEXXNORWEGENKARTENANBORDNEHMENXBESTAETIGENFXDXUUUAUSB        66
SSDOTTOWUENSCHEXXNORWEGENKARTENANBORDNEHMENXBESTAETIGENVVVFXDXUUUAUSB     69
SSDOTTOWUENSCHEXXSOFORTNORWEGENKARTENANBORDNEHMENXMELDENFXDXUUUAUSB       67
SSDOTTOWUENSCHEXXSOFORTNORWEGENKARTENANBORDNEHMENXMELDENVVVFXDXUUUAUSB    70
SSDOTTOWUENSCHEXXSOFORTNORWEGENKARTENANBORDNEHMENXMELDENXXFXDXUUUAUSB     69
SSDOTTOWUENSCHEYFANGBOOTEXXNORWEGENKARTENANBORDNEHMENXMELDENFXDXUUUAUSB   71
SSDOTTOWUENSCHEYFANGBOOTEXXNORWEGENKARTENANBORDNEHMENXMELDENXFXDXUUUAUSB  72
SSDOTTOWUENSCHEYFANGBOOTEXXNORWEGENKARTENANBORDXMELDENXFXDXUUUAUSB        66
SSDOTTOWUENSCHEYFANGBOOTEXXSTELLKARTENAUSRUESTUNGNORWEGENANBORDNEHMEN     69
SSDOTTOWUENSCHEYFANGBOOTEXXVVVFXDXUUUAUSBXXNORWEGENKARTENANBORDXMELDEN    70
```

**Partial cribs (clean offsets, i.e. no self-encipherment):**

| fragment | len | clean offsets |
|---|---|---|
| `STELLKARTENAUSRUESTUNGNORWEGEN` | 30 | 9, 13, 15, 22, 25, 27, 31, 39, 41 |
| `KARTENAUSRUESTUNGNORWEGEN` | 25 | 2, 4, 10, 13, 14, 18, 20, 27, 29, 30, 32, 36, 38, 42, 44, 46 |
| `OTTOWUENSCHEUNDBEFOHLENEFANGBOOTE` | 33 | 2, 7, 9, 12, 13, 22, 25, 26, 28, 35, 36, 37, 39 |
| `JOTTOWUENSCHEJUNDBEFOHLENEFANGBOOTE` | 35 | 2, 7, 11, 20, 21, 24, 26, 31, 33, 34, 35, 37 |
| `SICHERSTELLENUNDBESTAETIGEN` | 27 | 0, 5, 7, 10, 11, 12, 14, 15, 16, 20, 24, 28, 34, 42, 43, 44 |
| `SICHERSTELLENUNDMELDEN` | 22 | 25 offsets (weak menus, 3–6 loops) |
| `NORWEGENKARTENANBORD` | 20 | 24 offsets (weak menus, 1–6 loops) |
| `NORWEGENKARTEN` / `KARTENNORWEGEN` | 14 | 33 / 33 of 59 (too short for the bombe alone) |
| `OTTOWUENSCHE` / `JOTTOWUENSCHEJ` | 12 / 14 | 39 / 38 (too short alone) |
| `ZWOFUENFXUNDZWOSECHSXUUUFLOTTXX` | 31 | 10, 14, 16, 19, 27, 28, 37, 39, 40 (never @0) |
| `ZWOFUNFXZWOSECHSXUUUFLOTTXX` | 27 | 14, 20, 22, 23, 28, 31, 32, 33, 41, 43, 44 (never @0) |

The 25./26.-Flott address blocks collide at offset 0 in both spellings, so p082's own addressee cannot open the message.

## 6. Bombe tests and results

Setup:
- `tools/bombe/bombe`, full wheel space (B/C thin × Beta/Gamma × 336 orders), middle ring pinned (about 80% of middle-ring cases), 4 threads.
- Machine load was 21–27 because another agent's bombe and the ct-only run were active.
- Survivors are climb-ranked with navalblend quadgrams. Real 72-letter naval text scores about −4.8.

| run | cribs | k | loops/block | wall | block stops | survivors (g ≤ k) | best decrypt | verdict |
|---|---|---|---|---|---|---|---|---|
| R1 `R1_p082_full_off0_k2` | the 10 offset-0 re-addressed p082 reconstructions (66–72 letters, §5) | 2 | 11–12 (3 blocks of 22–24) | 7,273 s | **0** | 0 | – | **null** |
| R2 `R2_p082_frags_k0` | `STELLKARTENAUSRUESTUNGNORWEGEN` ×9 offsets, `KARTENAUSRUESTUNGNORWEGEN` ×16, `OTTOWUENSCHEUNDBEFOHLENEFANGBOOTE` ×13, `JOTTOWUENSCHEJUNDBEFOHLENEFANGBOOTE` ×12 (all clean offsets) | 0 | 6–15 | 12,027 s | **0** | 0 | – | **null** (but one garble inside the fragment defeats k=0) |
| R3 `R3_tf19_k1` | TF 19 short-note body `EINSZWOUHRJKIELJEINGELAUFENYFFFTTTBLEIBTBESETZT` @18,20,23 and `EINSZWOUHRKIELEINGELAUFENXFFFTTTBLEIBTBESETZT` @2,20,22,25 | 1 | 9–10 (2 blocks) | 2,318 s | **0** | 0 | – | **null** |

**Positive control (same binary, same session).** The P1030698 text crib run at k=2 on P1030698's own ciphertext, one job (B-thin, Gamma, 438), gives 2 stops, both the true key: WPVE/AAAH, g=2, with the exact published board. So the tool was working. Zero block stops for these strong menus is the expected outcome when a crib is wrong, as in research/08 §3.

**Self-encipherment screen (seconds, all offsets).**
- The 95 p082 variants of length 66–72 leave only 38 (variant, offset) pairs.
- All 25./26.-Flott address forms collide at offset 0.
- All `J…`-opening forms collide at offset 0.

## 7. What remains untested / recommendations

1. **Obtain and transcribe DEFE 3/578 for 1 May TOO 1150–1630 (top priority).** DEFE 3/743 (on disk) has been screened and has no candidate.
   - Look for decrypts with TOO x53/x58, especially FdU Ausb, 25./26./5. Flott, TF boats, UAK, TVA, Otto Wünsche, Mosel/Krefeld, and "TO ALL" orders. Also look for anything Bletchley notes as a re-encipherment ("ALSO IN KEY …").
   - Hörenberg's P1030673 (TOO 1143) is at 3/578 p722, so the relevant pages start below about p722.
   - A decrypt with TOO 1253/1258/…/1558 whose English body is about 45–60 letters (72 German / 1.31 minus address) is the target profile.
2. **High-resolution form image.** Settle "10"/"16" and the first two TOO digits. A known TOO turns the DEFE search into a single lookup.
3. The 1 May afternoon German originals in 534-texts.pdf (1303, 1516, 1636) do not fit `??5[3/8]`. No crib is available from them.
4. **Not run:**
   - p082 variants at offsets 1–6 (28 pairs, all re-addressed `JOTTO…`/`OTTO…` forms);
   - `SICHERSTELLENUNDBESTAETIGEN`, `SICHERSTELLENUNDMELDEN` and `NORWEGENKARTENANBORD` as stand-alone fragments;
   - all-middle-ring versions of R1/R2.

   Given §1–2, their prior is low.
5. **Key-period consequence for research/09.** Look for the Thetis settings of **1 May 1945** (first day of the May sheet), and secondarily 30 April.

## Summary

- The Rosetta pairs (7 in 577, 1 in 578) give a clean German↔Bletchley mapping (§3). The pooled length ratio is 1.31.
- The main result is a timing correction from U-534's own forms:
  - serial 70 and the frame order place P1030680's reception at about 16:30, not 10:30;
  - its TOO is `??53`/`??58`;
  - the operator's choice of the 1 May key suggests a TOO of 12:00 or later.
- DEFE 3/577 ends at about 12:00 on 1 May, so the sister decrypt, if Bletchley has one, is in **DEFE 3/578**.
- The prime 577 candidate, p082 (TOO 1119), fails on TOO, key day and length.
- Its only 72-letter-capable forms (re-addressed direct to Otto Wünsche) and its content fragments were tested over the full wheel and position space with the middle ring pinned (about 80% of middle-ring cases; k=2 for full texts, k=0 for fragments) with null results (§6). So was the one TOO-matching Thetis-community message, TF 19 (1152/1153).
- The noon-to-noon key period is now established.
