# 11 — Vector E: context reconstruction and context-derived cribs for P1030680

Date: 2026-09-23. Companion to research/10 (transcription).
Tools: `tools/bombe/bombe` (research/08); scripts and outputs in `runs/ctx/` (`cc.py` collision test, `gen.py` variant generator, `prune.py`, `run.sh`, `batch*.sh`, results in `runs/ctx/out/`).

## 1. What the form itself tells us (details in research/10)

- **Received 10:30 on 1 May 1945, Welle NB.** "10 30" is written above the "um ___ Uhr" line, where every other U-534 form checked carries the reception time. Datum is 1.5.45.
- **Uhrzeitgruppe `??5[3/8]/[70/30]`.** On the other NB forms the field is TOO/serial, and the serial is circuit-wide (95, 96, 97, 100 around 20–22h on 1 May, from different originators). A serial of 70 fits a morning reception.
  - The TOO is ≤ 10:30. Most probably it is 0853–0958; earlier is possible.
  - **The message was sent in the early-to-mid morning of 1 May, during the Kiel Festungsalarm** (declared 04:15, stood down 13:10 per the 534-texts.pdf decipherments).
- **22 groups**: 2 indicator + 18 body + 2 repeated indicator.
  - The repeat was received as VAOL against VROL. That is 1 reception error in 8 indicator letters, so this copy is noisy.
  - Corpus reception-garble rate is about 2–5 per 72 letters (P1030698: 2 in 72; P1030697: about 5 in 76).
- **No address callsigns** are written in the Anschriften field.
- **Key**: the trigram ACH lies in the K-Buch column that U-534's Zuteilungsliste allots to **M-Thetis**. The operator tried Potsdam (1 May) twice, found the Thetis allocation, and stopped. U-534 held the allocation list but not the Thetis key sheets.

## 2. Who holds Thetis on 1 May 1945, and why a Thetis message is on NB

- **Thetis** (German name; no British codename, never broken) was introduced in September 1939 as the key for **U-boat training and exercises in the Baltic**. Its community:
  - FdU Ausbildung (Viktor Schütze; formally at Gotenhafen until war's end, in practice evacuated westward in spring 1945);
  - the training flotillas (18.–27., the Agru-Front, the 1./2. ULD);
  - their escort, target and torpedo-recovery craft (T.F. boats);
  - training establishments (UAK = U-Boots-Abnahmekommando, TVA, Schießschulen).
  - Sources: de.wikipedia "Thetis (Schlüsselnetz)"; uboat.net Enigma cipher list (Erskine).
- **By 30 Apr–1 May the same community is talking on Potsdam.** Every broken U-534 message of those days is Potsdam, including FdU Ausb ↔ T.F. boats, the 5./26./4./25. Flotillas, Hertha, Otto Wünsche, tender Mosel, and training U-boats of the 4. and 26. Flotilla. So Thetis was, by then, a **residual** net: units that still had Thetis settings but either lacked May Potsdam sheets or were addressed by a training-command authority that still used its own net.
  - P1030687 (T.F. 24): *"Kurier mit Schlüsselmittel noch nicht eingetroffen"*. Key material was physically in transit on exactly these days, and **1 May was the first day of a new monthly key period**.
- **NB was the working circuit of the western-Baltic transit traffic on 30 Apr–1 May.**
  - Hertha (Warnemünde→Nyborg) reports "NB besetzt".
  - Otto Wünsche (Warnemünde→Neustadt) reports "Schaltung Gr…en und NB".
  - U-10.. (Travemünde→Kiel) reports "Schaltung NB".
  - The P1030682/694/699 forms were received on NB.
  - So "NB" here does not imply Norway. It is the circuit U-534 was guarding in Kiel Bay, and the broadcast it copied carried traffic for **all** key nets. U-534's operator logged a Thetis message he could not read.
- **Funkleitung was fighting a flood of undecipherable messages that very day.** Examples:
  - 0353 "Die Funksprüche mit der Uhrzeit 2210 und 2359 sind unklar";
  - P1030686 / P1030692 / P1030709 "nicht gelöst … neu hergeben";
  - P1030699, received about 00:02 on 2 May: "An alle von Funkleitung: Die große Anzahl unklarer Funksprüche gibt Veranlassung darauf hinzuweisen, dass ab 1.5. 1200 Uhr Schlüssel …".
  - P1030680 is one of those "unklar" messages from the receiving side.

**Most probable senders/addressees, ranked by corpus evidence:**
1. **FdU Ausbildung** ↔ T.F. boats or training flotillas. FdU Ausb is the owner of the training net and the most frequent urgent-order issuer in the corpus (P1030669 SSD order TF18→Neustadt / TF19 stay Kiel; P1030698 TF24 stay Travemünde; P1030675 BINE order to FdU Ausb + 4./25./26. Flott).
2. **5. U-Flottille (Kiel)** as sender of Kiel-area notices. The 04:15 Festungsalarm "Kr Kr an alle U-Boote von 5.U-Flott" and the 11:41 "Kr Kr alle Boote … Festungsalarm für Kiel beendet" were Potsdam; a Thetis copy for non-Potsdam holders is plausible.
3. **Training flotillas 25./26.** (Travemünde/Warnemünde, moving west under Soviet pressure: Warnemünde feindbesetzt about 19:50).
4. A **training or support unit reporting** its move, arrival or anchorage (the genre that dominates the 1 May corpus: "… Uhr … eingelaufen / geankert / aus nach …, Schaltung NB").
5. **Funkleitung** housekeeping. This genre's openers (`FUNKSPRUCH…`, `DIEFUNKSPRUECHE…`, `SPRUCHSCHLUESSEL…`) all **collide** at offset 0 (positions 22, 26, 2). So only a leading address (`LEITUNG…`) or end sign-off form remains possible.

**Message shape.** 72 letters is short. It is either (a) a single-topic order (P1030698 style: address 30 letters + one sentence) or (b) a short report. In both cases the **address block is at offset 0** (44 of 49 dated corpus messages start with recipient(s), `VON`/`VVV`, sender, `XX`). This is the most exploitable structural fact, and it drives family F1–F5 below.

**Positional constraints from no-self-encipherment at offset 0** (confirmed positions 0–5 `JCRSAJ`):
- plaintext[4] ≠ A, which kills `KRKRALLE`, `KRKRANALLE`, `KOMXADM…`;
- plaintext[0] ≠ J, [1] ≠ C, [2] ≠ R, [3] ≠ S, [5] ≠ J.
- Position 22 (U) kills most `…UUU…` address variants whose U lands there, for example `ANALLEUUUBOOTEVONFUNF…`.
- Position 15 (K) kills `ANALLEVONVONFUNKLEITUNG`.

## 3. Method

1. **Collision test first** (`runs/ctx/cc.py`). Any crib that collides at a published position is dropped, not handed to the bombe's auto-exclusion. A collision needs a reception garble at exactly that position (prior about 3–7%).
   - Killed outright at offset 0: `KRKR…A`, `KOMXADM…`, `FUNKSPRUCH…` (pos 22), `DIEFUNKSPRUECHE…` (26), `SPRUCHSCHLUESSEL…` (2), `ANALLEVONVONFUNKLEITUNG` (15), and most `…UUU…` address variants that put a U on pos 22.
   - Of 704 generated address variants, 417 survived the collision test.
2. **Spelling variants generated as a product.** Variants: FUNF/FUENF, SIBEN/SIEBEN, VIR/VIER, VONVON/VVV, X/XX, UUUX/UUU, and T.F. numbers 15/17/18/19/21–24 (`runs/ctx/gen.py`). Where a crib of 22+ letters is a prefix of a longer one, only the shorter is kept (`prune.py`).
3. **Bombe settings.** `--midring pinned`, full reflector × Greek × 336 orders space. This covers about 94% of middle-ring cases for cribs lying within the first ~35 letters, and about 80% for full-length cribs.
   - Stops go to `--climb navalblend_quadgrams` ranking.
   - Cribs of 16 letters or more only. Crib-offsets whose menus give more than 500 stops per job were dropped (13 cases, listed in `K_probe.log`).
4. **Rehearsal on P1030698.**
   - Header crib `TTTFFFZWOVIERVVVFXDXUUUX@0`, k=0: stops only on the true key WPVE/AAAH.
   - At k=1 with climb: the true decrypt ranks first (rest −3.66, crib mismatch 1), and junk is at ≤ −7.5 with mismatch ≥16.

**About g versus crib_mismatch (checked, not a bug).** The bombe's `g` counts contradictions only among crib positions whose letters the block's partial plugboard determines. Positions that stay undetermined (`undet`, often 12–15 of a 28–35-letter crib) are unconstrained.

Example: F1 stop B-thin Beta 163 DKLI AAAK, g=0, undet=15. py-enigma with its 8 known pairs decrypts `TQTQSFZWRZOKURDFXDXFJXARVBXX…` against the crib `TTTFFFZWOZWOVVVFXDXUUUAUSBXX`. It agrees exactly at the determined positions and differs at 14 of the 15 undetermined ones. The climb cannot fill the board (at most 10 pairs) to reproduce them, so ranked `crib_mismatch` = 11–14.

**The real acceptance test for a short crib is therefore `crib_mismatch ≤ k` after the climb,** not `g`. The true P1030698 key passes it at mismatch 1, as the rehearsal shows.

## 4. Hypotheses and results

Real 72-letter naval plaintext scores about −4.8 rest-quadgram; junk scores −6.7 or worse. "Survivors" = ranked hits whose crib is reproduced within k letters.

| # | hypothesis (family, variants) | offsets | k | stops | survivors (mismatch ≤ k) | best rest-quad | verdict |
|---|---|---|---|---|---|---|---|
| T01 | FdU Ausb as **sender**: `VONVONFXDXUUU(X)AUSBXX` after any recipient block (28 crib-offsets) | 5–29 (clean ones, not 16) | 0 | 12 | 0 (best mismatch 1: `…EVONVONFXDXUUUAUSIXXTILZBK…`, −7.88) | −7.88 | null (weak: 19–20 letters, k=0) |
| F1 | **T.F. n von FdU Ausb** `TTTFFF{15,17,18,19,21–24}{VVV,VONVON}FXDXUUU(X)AUSBXX` (18) | 0 | 1 | 80 | 0 (best mismatch 11) | – | null; fairly strong (28–35 letters, 1 garble) |
| F2 | **FdU Ausb von X** `FXDXUUU(X)AUSB{VVV,VONVON}{TF18/19/24/17, 4./5./25./26. Flott}{X,XX}` (27) | 0 | 1 | 153,636 | 0 (best mismatch 9) | – | null |
| F3 | **flotilla (4./5./25./26., Chef 5.) von FdU Ausb** (68) | 0 | 0 | **0** | 0 | – | null |
| F4 | **An alle / Alle / Alle U-Boote / Alle Boote von {5. Flott, FdU Ausb, FdU Ost, 10. Sidi, Kom.Adm., Funkleitung}** (66) | 0 | 0 | 87 M (weak short `ALLE…` cribs) | 0 of 2 M ranked (cap); best mismatch 2 | −7.98 | **incomplete**: ranking capped at 2 M of 87 M stops. Nothing German among the ranked. |
| F5 | flotilla ↔ flotilla (26./25./4./FdU Ausb ↔ 5. Flott) (82) | 0 | 0 | 0 in the 11% searched | – | – | **stopped at 11% of the space** (thread limit); untested |
| F6 | boat or T.F. sender openers (`…VONVONUUU`, `TTTFFFnVONVON`, `ALLEVONVONTTTFFF`) (23) | 0 | 0 | – | – | – | **not run** |
| K | body/tail phrases at all clean offsets: `FESTUNGSALARMFUERKIEL(BEENDET/AUFGEHOBEN)`, `KLARZUMVERSENKEN(GEMXBEFEHL)`, `BOOTEKLARZUMVERSENKEN`, `BESTAETIGUNGERBETEN`, `MARSCHNACHNORWEGEN`, `SOFORTINMARSCHSETZEN`, `UHRKIELEINGELAUFEN`, `FXTX/FFFTTTBLEIBTBESETZT`, `SCHALTUNGNANNIBRUNO` (258 crib-offsets) | 6–58 | 0 | – | – | – | **not run** (built and probed only: `runs/ctx/K2run.txt`) |
| R01 | `TTTFFFEINSACHTVVVFXDXUUUAUSBXX SOFORTNEUSTADTINMARSCHSETZENXBESTAETIGEN` | 0 | 3 | 0 | 0 | – | dead as written |
| R02 | `TTTFFFEINSNEUNVVVFXDXUUUAUSBXX VORLAEUFIGKIELBLEIBENXWEITEREBEFEHLEABWARTEN` | 0 | 3 | 0 | 0 | – | dead |
| R03 | `FUENFXUUUFLOTTVVVFXDXUUUAUSBXX WANNISTTTTFFFEINSACHTAUSGELAUFENXXBESTAETIGEN` | 0 | 3 | 0 | 0 | – | dead |
| R04 | `FXDXUUUAUSBVONVONFUENFXUUUFLOTTXX TTTFFFEINSACHTSOFORTINMARSCHGESETZT…` | 0 | 3 | 0 | 0 | – | dead |
| R05 | `ANALLEBOOTEVONVONFUENFXUUUFLOTTXX FESTUNGSALARMFUERKIELXSCHALTUNGNB…` | 0 | 3 | 0 | 0 | – | dead |
| R06 | `ANALLEUUUBOOTEVONVONFUENFXUUUFLOTTXX FESTUNGSALARMFUERKIELAUSGELOEST…` | 0 | 3 | 0 | 0 | – | dead |
| R07 | `LEITUNGVONVONFXDXUUUAUSBXX FFFTTT0953UNKLARXNEUHERGEBEN…` | 0 | 3 | 0 | 0 | – | dead |
| R08 | `FXDXUUUAUSBVONVONTTTFFFZWOVIERXX UHRTRAVEMUENDEEINGELAUFENXFFFTTTBLEIBTBESET` | 0 | 3 | 0 | 0 | – | dead |
| R09 | `ANALLEVONVONFXDXUUUAUSBXX ALLEBOOTEKLARZUMVERSENKENHALTEN…` | 0 | 3 | 0 | 0 | – | dead |
| R10 | `FXDXUUUAUSBVONVONZWOSECHSUUUFLOTTXX WARNEMUENDEFEINDBESETZT…` | 0 | 3 | 0 | 0 | – | dead |

- The R rows are full 72-letter reconstructions, run as one batch in 6,788 s at 4 threads, with 0 block stops over 1.3×10^10 settings. Each is excluded as worded, up to 3 unknown garbles plus its forced collisions. Only the exact alignment is excluded, not the theme.
- Earlier exclusions (research/08) still stand and were not repeated: P1030698 verbatim, `FXDXUUU(X)AUSBXX@0`, `KOMXADMXUUUBOOTE@0`, `KRKRALLE@0`, the TRAVEMUENDE body, and the WEITERE BEFEHLE ending.

Totals: about 200 address-block crib variants run at offset 0 (T01/F1/F2/F3 complete, F4 partial), plus 10 decisive full-length reconstructions. **No decrypt reads as German**; nothing reached the py-enigma verification stage (§4 of the task).

## 5. Reading of the results

- **FdU Ausbildung ↔ T.F. boat / flotilla at offset 0 is now well covered.**
  - Recipient-first forms: F1 at k=1, F3 exhaustive at k=0 with zero stops.
  - FdU Ausb-first forms: F2 at k=1, plus `FXDXUUUAUSBXX@0` from research/08.
  - FdU Ausb as sender after an arbitrary recipient: T01.
  - The residual chance that the message has this shape and was missed comes from: >1 garble inside the ~30-letter header (about 25–35% given 2–5 garbles per 72), a middle-ring turnover in the header (about 6%), or an unlisted spelling or number (for example a T.F. number outside 15–24, `FDUUUUAUSB`, `FFFDDDUUUAUSBILDUNG`).
- **The "An alle / Kiel Festungsalarm" family is only weakly tested.** The `ALLE…` cribs are short (16–27 letters) and the 2 M-hit ranking cap covered about 2% of stops. The R05/R06 wordings are dead.
- The negative results shift weight toward:
  - (a) messages whose **first element is not a standard FdU/flotilla address**: a boat or ship name, a callsign-like unit name, `SSD`/`BINE` + address, `KK…KK` names, a T.F. number outside the list;
  - (b) **report-type** texts from a small unit, whose address block may be as short as `FUNFUUUFLOTTX`.

## 6. What remains untested (ready to run; files in `runs/ctx/`)

1. **K batch** (`K2run.txt`, 258 crib-offsets of body and tail phrases, k=0). At 8 threads it would take an estimated 6–8 h.
2. **F5** (flotilla ↔ flotilla, 82 cribs, k=0): only 11% was searched. **F6** (boat / T.F. sender openers, 23 cribs): not run.
3. **F4 re-ranking without the 2 M cap** (`--rank-from out/F4_alle_k0.tsv --maxhits 0`), or lengthened `ALLE` cribs (with sender + `XX` + first body word) to cut stops.
4. `SSD`/`BINE`/`WESPE`/`MUKE` + address prefixes at 0; `FFFDDDUUUAUSBILDUNG` / `FDUUUUAUSB` spellings; T.F. numbers 1–14, 20, 25–30; `UUUAGRUXFRONT`, `UAK`, `TVA`, `UUUSTUETZPUNKT(KIEL)`, `HAKAKIEL` (Hafenkapitän) recipients.
5. Sign-off-at-end cribs (`…XFXDXUUUAUSB` + 0–3 filler letters at offsets 58–61), combined with a `?`-gap and address hypotheses into single loop-rich menus.
6. A rerun of the best families with `--excl` on the doubtful transcription positions from research/10 (10, 12, 20, 25, 32, 47, 60, 67).

## Summary

- **The form** (research/10) gives reception at 10:30 on 1 May on circuit NB. The Uhrzeitgruppe reads ??5[3/8]/70; the serial matches the NB broadcast sequence, and TOO is most likely 08:5x–09:5x. That falls within the Kiel Festungsalarm (04:15 to about 13:10) and within the day Funkleitung complained of many "unklare Funksprüche".
- **Thetis** is the residual training net of FdU Ausbildung and its flotillas and T.F. boats. The most likely text is a short FdU-Ausb/flotilla order or a unit report with an address block at offset 0.
- **Tests run:** about 200 collision-clean address-block variants and 10 full-length reconstructions (k=3).
- **Result:** zero German decrypts; all full-length wordings are dead. FdU-Ausb ↔ T.F./flotilla headers are well covered, "An alle" only weakly, and body/tail phrases are not run (ready in `runs/ctx/`).

## 7. Rerun with the human reader’s excluded positions (24–25 Sep 2026)

**Why.** the human reader’s reading of the form (`archives/forms/P1030680_readings_human.txt`) lists ten ciphertext positions where the handwriting is ambiguous (0-based): 1, 10, 20, 25, 32, 58, 60, 67, 68, 71. The §4 runs used k=0 or k=1 with no exclusions, so one misread letter at any of these positions could have killed a correct crib. This section reruns the address-block families with `--excl 1,10,20,25,32,58,60,67,68,71` and k=1, which still allows one reception garble elsewhere.

Scripts and outputs are in `runs/ctx_excl/`:
- `run.sh` is `runs/ctx/run.sh` with the `--excl` set built in.
- `gen_excl.py` regenerates the families.
- `select.py` applies the drop and flood rules. `selection.tsv` gives every crib with its blocks, probe stops and decision.
- `batch*.sh` run the families in order.
- Results are in `out/*.rank.tsv`.

All runs use the pinned middle ring and the full reflector × Greek × 336-order space (1.30×10^10 settings), with `--climb navalblend_quadgrams --rank-out`.

**Positive control (passed).** `runs/bombe/p98_garbled2.txt` is P1030698 with cipher letters 9 and 53 corrupted. Setup:
- Crib: `TTTFFFZWOVIERVVVFXDXUUUXAUSBXX@0`.
- The same ten exclusions (1, 10, 20 and 25 fall in the crib), k=1.
- Scanned space: B-thin Gamma, 6 orders including the true one.

Blocks were 13+13 letters with 2+2 loops, weaker than any crib kept below. Result:
- The true key (IV-III-VIII, WQWE/AAAH, which is WPVE/AAAH up to ring equivalence) ranks **#1** with crib mismatch 1, the garble at position 9. Its rest-quadgram score is −3.83.
- Next is a ring neighbour at −5.26, mismatch 2. Junk is at ≤ −6.9 with mismatch ≥ 8.
- py-enigma with the ranked board decrypts `TTTFFFZWOGIERVVVFXDXUUUXAUSBXXTRAVEMUENDEBLEIBENXWEITSREBEFEHLEATWARTKNX`.
- So exclusion does not break detection, even with weak blocks.

**Regeneration.** The collision (self-encipherment) test now ignores the ten positions, which revives cribs whose only collision fell on one of them:
- F2: 14 revived, mostly a collision at 25 (`…VVVFUNFUUUFLOTT`).
- F4: 20 revived (collisions at 10 and 25).
- F5: 22 revived (at 25 and 32).
- T01: 2 revived (`@26`, collision at 32).
- F1, F3 and F6: none revived.

F6 was regenerated from its definition: 41 cribs instead of the 23 hand-listed ones. T01 was regenerated at offsets 3–30: 32 cribs.

**Drop rules.**
1. **Weak blocks.** A crib is dropped when its smallest k=1 block has fewer than 10 tested letters **and** fewer than 6 loops.
2. **Flood.** A crib is dropped when it gives more than 15,000 block-stops over 3 probe jobs (B-thin Gamma, orders 123/438/785), about 5,000 per job. The probe is `out/probe_all.log`. These cribs would have filled the ranking cap.
3. **Prefix prune.** A crib is not run when a shorter crib at the same offset that is its prefix is already being run.

Cribs dropped by rules 1 and 2 were re-queued at **k=0** with the same exclusions (row D below). A single 16–35-letter block has far more loops and far fewer stops than two half-blocks.

**Bombe fix mid-run.** research/15 §1.3 found that `tools/bombe/bombe` keeps only the first surviving pivot hypothesis per block. On weak menus it therefore drops some true keys, about 15% of cases at 1–3 loops.

Every family below was first run with `tools/bombe/bombe`. Every k=1 family has weak blocks of 2–5 loops. So each family was re-run in full with `tools/blind/bombe_ah --allhyp-new`, which writes only the stops the old bombe dropped.
- These complement stops were all climbed with `blindrank`: keep 1.0, R=8, same exclusions (`run_ah.sh`).
- The new U 2356 fragment family (E) was run fresh with `bombe_ah --allhyp`.
- The stored D stops were re-ranked in full with `blindrank` (`D_rerank`), because the old top-200 list had been filled entirely by the 11-letter `ALLEVVVTTTFFF@0`.
- None of the k=1 families reached the 2 M ranking cap. Every stop with g ≤ k was ranked.
- Threads: 4–6 for A1/A2 (before the 8-thread budget), 2 for everything after.

| # | family | cribs run (of generated) | k | excl | bombe | stops (block) | ranked g≤k | min crib mismatch after climb | best rest-quad | result |
|---|---|---|---|---|---|---|---|---|---|---|
| ctl | P1030698 header `TTTFFFZWOVIERVVVFXDXUUUXAUSBXX@0` on P1030698 with 2 garbles (6 orders) | 1 | 1 | 10 | both | 777 k | 462 | **1 (true key)** | **−3.83** | **true key #1**, verified in py-enigma |
| A1 | F1 T.F. n von FdU Ausb + F4 "An alle / Alle …" | 17 of 18 + 30 of 133 | 1 | 10 | old + ah-new | 48.2 M | 569,674 + 107 | 4 (F4); F1 ≥ 4 | −7.02 | null |
| B | F2 FdU Ausb von X | 31 of 68 | 1 | 10 | old + ah-new | 33.9 M | 82,643 + 12 | 6 | −7.01 | null |
| A2 | F3 flotilla von FdU Ausb | 65 of 68 | 1 | 10 | old + ah-new | 25.8 M | 52,075 + 0 | 7 | −6.59 | null |
| C | F5 flotilla ↔ flotilla | 85 of 174 (80 prefix-pruned) | 1 | 10 | old + ah-new | 15.7 M | 43,416 + 4 | 6 | −7.37 | null (first complete F5 run) |
| D | k=1 drops rerun at k=0: T01 (31), F6 (38), F4 (40), F2 (8), F5 (7), F3 (3) | 127 of 178 | 0 | 10 | old + ah-new, full re-rank | 9.1 M | 4,723,312 + 198 | 0 (short cribs, see below) | −6.55 | null: no German |
| K | body/tail phrases, clean offsets 6–58 | 19 of 345 | 1 | 10 | old + ah-new | 34.7 M | 99,786 + 33 | 7 | −7.57 | null |
| E | U 2356 fragments `(U)ZWODREIFUENFSECHS`, `KIELEINGELAUFEN`, all clean offsets | 13 of 108 | 0 | 10 | ah (fresh) | 21.4 M | 8,725,590 | 0 (short cribs) | −6.22 | null: no German |

Real 72-letter naval plaintext scores about −4.8 rest-quad on these tables, and junk scores −6.2 or worse.

**Acceptance test (mismatch ≤ k).**
- For A1, B, A2, C and K it gives **zero survivors**, both in the original stops and in the all-hypotheses complement. The best mismatch is 4–7 original and 8–13 complement.
- For D and E, mismatch 0 is reached, but only by the shortest cribs. With 11–16 tested letters, a climb fits the crib by chance:
  - D: `ALLEVVVTTTFFF@0` gives 870 k stops at mismatch 0, the 15-letter T.F. openers give 60 k each, and the 19-letter `VONVONFXDXUUUAUSBXX@n` gives 3–22 each.
  - E: 0.97 M at mismatch 0.
  - For these cribs the language score is the discriminator. The best mismatch-0 decrypt per crib is −6.55 (D) or −6.22 (E), and none reads as German.
  - Examples: `ALLEVVVTTTFFFREHAUTIAPMSEMENXNAHEZIF…`, `…KIELEINGELAUFEN` @51/@56 `YNMUMGERXOKJBOMBIMAB…`.
  - Nothing reached the py-enigma verification stage, because nothing reads as German. The pipeline's own py-enigma check (control) passed.

**Dropped** (crib lists and reasons in `selection.tsv`, `K.selection.tsv`, `D.selection.tsv`, `E.selection.tsv`, `E.probe.tsv`):
- **Weak blocks at k=1** (smallest block < 10 letters and < 6 loops):
  - T01 all 32: 16–17 used letters, blocks of 8–9 with 0–2 loops;
  - F6 40 of 41;
  - F4 29 of 133;
  - K 279 of 345 (all of `BESTAETIGUNGERBETEN`, `MARSCHNACHNORWEGEN`, `UHRKIELEINGELAUFEN`, `SCHALTUNGNANNIBRUNO`, `F(FF)XT(TT)BLEIBTBESETZT`, `FESTUNGSALARMKIEL`, `KLARZUMVERSENKEN`, and most of the rest);
  - E all 108.
- **Flood** (> 15,000 block-stops over 3 probe jobs): F1 1, F2 11, F3 3, F4 52, F5 9, F6 1, K 47.
- **k=0 rerun of the drops (D).** The T01, F6, F2, F3, F4 and F5 drops were rerun at k=0. Of these, 5 F4, 3 F6 and 1 T01 flooded even at k=0 and are **untested**. Examples: `ANALLEVVVFUENFUUUFLOTTX`, `ALLEBOOTEVONVONFXDXUUUOSTX`, `ALLEVVVZEHNXSIDIXX`, the last covered by research/15's full F4 re-rank without exclusions.
- **Not rerun at k=0:** the K drops (279 weak + 47 flood).
- **E, k=0.** The 90 E crib-offsets with < 2 loops even at k=0 were not run. Of the 18 probed, 5 flooded: `UZWODREIFUENFSECHS@12,29,30`, `ZWODREIFUENFSECHS@34`, `KIELEINGELAUFEN@22`.

**Reading.**
- One misread letter at the human reader’s ten doubtful positions, plus one garble elsewhere, does not rescue any of the §4 address-block families at offset 0:
  - T.F. n von FdU Ausb;
  - FdU Ausb von X;
  - flotilla von FdU Ausb;
  - flotilla ↔ flotilla;
  - An alle.
- Nor does it rescue the 19 longest body/tail phrase placements.
- F5 (flotilla ↔ flotilla) is now covered over the full space for the first time.
- **Weaknesses:**
  - Excluding 4–5 header letters leaves 2-block menus of 11–15 letters with 2–5 loops each. The control shows the pipeline still finds the true key on such menus, but the climb's fill-in success is 20–60% (research/07, research/15). So a k=1 null here is weaker than F3's old k=0 zero-stop null.
  - Middle ring pinned (about 94% of cases for header cribs).
  - At most one garble outside the ten positions.
