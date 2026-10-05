# 06 — Vector B: what the indicator VROL NMKA tells us about the Thetis key

Research date 2026-09-23. Scripts in `tools/indicator/`. All numbers below were run on this machine.

## TL;DR

1. **The procedure is confirmed exactly, in code.** In 1945 the M4 message key is the output of enciphering the
   4-letter *Verfahrenkenngruppe + filler* (here `SEDM`, or `OEDM` from the VA reading) at a 4-letter **Grund**,
   under the full daily key: plugboard, all four ring settings, stepping before each letter. With the 1 May 1945
   Potsdam key, py-enigma and our own model both reproduce Girard's results `OEDM@MNNS → ELKC` and
   `SEDM@DGUG → PUYY`. They also reproduce the operator's own 1945 trial decrypt, `PUYY` on the first 6 body
   letters `→ IPZAYK`, which he wrote on the form in rows 3–4 of the Buchgruppen column.
   **Condition:** the reproductions only work with the **original rings VCCH**. They fail with Hörenberg's
   normalised rings AACU (see §4).
2. **For Thetis the Grund is not known.** GC&CS's own histories (Mahon, *History of Hut Eight*, pp. 110–113;
   Alexander, *Cryptographic History of Work on the German Naval Enigma*, pp. 83–84) say:
   - The per-message Grund table was a **per-key document**. It was introduced on Plaice (= Potsdam) in April 1945
     as a table of 288 Grunds valid for a month.
   - On 1 May 1945 "Plaice went onto a new Grund table and **Dolphin started the new system with a table of its
     own**".
   - Sucker, Shark and Narwhal stayed on the old single daily Grund.

   So MNNS and DGUG are cells of the **Potsdam** May 1945 table. The cell the operator looked up, [14][9] (or
   [14][5]), says nothing about the Thetis Grund. The Thetis Grund is either one cell of an unknown Thetis table or
   an unknown Thetis daily Grundstellung. Either way it is a free 26⁴ unknown.
3. **Consequence: the indicator places essentially no constraint on the Thetis key or the message start.** For any
   hypothetical key there is a Grund for roughly half of all possible message keys (measured: 47–52 %, about 1 bit).
   The only usable rule that does not depend on the Grund is that Enigma never enciphers a letter to itself: message
   key letters 1–4 cannot be S/O, E, D, M. At body level this is almost worthless (§6).
4. **Recommendation:** don't build the solver around the indicator. Run the ciphertext-only body search as planned.
   Use `tools/indicator/` only as (a) a transcription check (the first 6 letters are confirmed), (b) a
   near-free sanity check (message key letters ≠ S/O, E, D, M), and (c) if a Thetis key sheet or Grund table ever
   turns up (see the archival lead in §7), the ready-made exact post-filter described in §5.

---

## 1. Sources read

- Hörenberg, *The Kenngruppen System*:
  https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+Kenngruppen+System
  Worked example P1030690: `FNHC GVET` → Quelle Tafel A → `KDUZ / YMUZ`. The Schlüsselkenngruppe DUZ sits at
  K-Buch column 12, row 9, and the Zuteilungsliste gives columns 1–20 = Potsdam. The Grund written on the form is
  IBFK, and "Verfahrenkenngruppe YMUZ → ODFF". The key uses rings **VCCH***. The note says Girard recovered the
  original rings, and that the message keys written on the forms work with VCCH.
- Hörenberg, P1030680, including Girard's full analysis:
  https://enigma.hoerenberg.com/index.php?cat=Unbroken&page=P1030680.
  Key sentence: "he consults his table of Grund settings and notes the 4-letter Grund listed in column 14 row 5".
  Also: "the actual Grund tables used by the German Navy do not appear to exist any longer".
- Scans downloaded from the site and inspected:
  - the P1030680 Schlüsselzettel;
  - the K-Buch Gruppenliste page ("AC H 645 14", so ACH is column 645, row 14);
  - the Zuteilungsliste Teil B ("621–653 M Thetis (M Tht)"; Verfahrenkenngruppe for Schlüssel M: "1–733 Allgemein");
  - Tauschtafelplan *Bruno*, Kennwort *Quelle* (Mai 45 = column "sechs", day 1 = table **A**);
  - Tauschtafel Quelle A, front half only.
- Other U-534 pages on the site. Per-message Grunds were written on the forms on the **same day, same net**:
  30 Apr: P1030660 `VGWP` (AAYL→MVZQ), P1030666 `MZAU` (JCAP→TUYB), P1030681 `NEAM` (QEOB→CDSZ, per
  cryptomuseum.com). All with rings **EPEL**, not the normalised AAEL. 1 May: P1030690 `IBFK`.
  Several different Grunds per day proves that the Grund was taken from a per-message table.
- GC&CS histories on ellsbury.com:
  - Mahon, *History of Hut Eight* pp. 110–114: http://www.ellsbury.com/hut8/hut8-110.htm … `-114.htm`
  - Alexander, *Cryptographic History…* pp. 80, 83–84: http://www.ellsbury.com/gne/gne-083.htm, `gne-084.htm`
  Verbatim key passages:
  - Alexander p. 83: "the introduction on Plaice of a set of 288 grundstellung, valid for a month, instead of
    having one grund for each day; the decision of which grund to use was automatically made for the operator by
    his choice of message trigrams."
  - Mahon p. 111: "The encircled number giving the position of the trigram in the column of the reciprocal part of
    the K book was clearly the key to the system … it was 26 x 12 = 288 [sic], columns 1 and 2 being identical".
    26 × 12 = 312, so the arithmetic is off. 24 × 12 = 288 matches the K-Buch row numbers 1–24 and is the likely
    reading. Mahon p. 110 dates the Plaice problem to "the beginning of March", while Alexander and Mahon p. 112
    say April. The discrepancy doesn't matter here, because the May tables were new either way.
  - Mahon p. 112 / Alexander p. 84: "In May, a new table appeared on Plaice and Dolphin also took up the system
    using an **independent grund table**." Sucker, Shark and Narwhal "had remained on the old grund system".
  - Mahon p. 113: "work was stopped when we heard that the Grund tables had been captured". Also: after the
    surrender there turned out to be 15 bigram tables, not 9.
- Erskine, "The Kenngruppenbuch indicator system" (ciphermachinesandcryptology.com/files/kmindicator.pdf). The
  pre-1945 rule: "each naval keylist contained a series of daily Grundstellungen, which were employed for all
  signals using the cipher in question". So the Grund was always per-cipher keylist material.
- M.Dv. 32/1 (1940), translation at codesandciphers.org.uk `egenproc/page10.htm` §47. The message key is obtained
  by keying the Verfahrenkenngruppe once at the Cipher of the Day. In the 1940 M3 text this is a 3-letter group.
  On the 1945 M4 the group is 4 letters, the trigram plus the filler letter, and every reproduction below confirms
  that the filler is enciphered too.
- Frode Weierud's cryptocellar has no 1945 naval procedure document beyond the Erskine material above. I found no
  published Grund table for any net.

## 2. The indicator, decoded

Form P1030680: Funkgruppen rows 1–2 are `VROL NMKA`. Rows 21–22 are the repeat, which the operator read as
`VAOL NMKA`, and he pencilled "k" and "o" beside them. The Buchgruppen column holds `EACH / SEDM`, with the E and S
overwriting an earlier K and O. The right-hand column has "14", then "9" overwriting "5", then "PUYY" and
"M-Thetis". "ELKC" is struck through at the top of the form.

Bigram pairs are taken horizontally and decoded with Quelle A, which is reciprocal. I checked each pair against the
scanned table:

| radio bigram | Quelle A | goes to (row1 col, row2 col) |
|---|---|---|
| VR | ES (table: E·S = VR) | E = SKG filler, S = VKG 1st letter |
| *VA (repeat)* | *KO (K·O = VA)* | *K = SKG filler, O = VKG 1st letter* |
| OL | AE (A·E = OL) | A, E |
| NM | CD (C·D = NM) | C, D |
| KA | HM (H·M = KA) | H, M = VKG filler |

The rows are `E A C H` (filler + Schlüsselkenngruppe **ACH**) and `S E D M` (Verfahrenkenngruppe **SED** + filler M).
ACH is K-Buch column 645, row 14, and column 645 falls in 621–653 = **M Thetis**. SED/OED give row 9/5.

## 3. The exact equation

Notation: A=0…Z=25. Wheels are ordered [greek, left, middle, right]. `p` = window letter, `r` = ring setting,
core offset `o = p − r`. The daily key is D = (reflector, greek wheel, wheel order, r₀..r₃, plugboard σ).

```
message key  K = (k0,k1,k2,k3) = E_D( start windows = Grund G ; plaintext = V1 V2 V3 F )
```

Here V1V2V3 is the Verfahrenkenngruppe and F its filler, i.e. `SEDM` or `OEDM`. E_D is ordinary Enigma
encipherment. It is reciprocal, so "encipher" and "decipher" are the same operation. Stepping happens before each
of the 4 key presses, double step included. Letter i is:

`k_i = σ · Π(o^(i)) · σ (v_i)`, where `o^(i) = step^i(G) − r`

Then the body is deciphered from start windows K, with core offsets `step^j(K) − r`. So:

- **The rings enter the indicator step fully**, through `G − r`. In the 72-letter body they mostly cancel,
  because only `K − r` is visible there.
- **The plugboard enters the indicator step as well.** K depends on σ(S), σ(E), σ(D), σ(M) and on the σ-images of
  the output letters.
- **Greek wheel (task item 4).** It never steps, in either phase. Its core offset is `g0 − r0` at the Grund and
  `k0 − r0` in the body, and the difference `g0 − k0` does not involve the ring. So r₀ is a completely free
  parameter for the body: it just relabels k₀. It is pinned only by the complete 4-letter indicator relation, the
  same way r₁ (left wheel) is. The greek wheel contributes no separate or extra constraint. The same holds for the
  left wheel whenever it doesn't step during the message.

## 4. Verified reproduction (py-enigma + independent numpy model)

`tools/indicator/verify_girard.py` (run with `PYTHONPATH=.venv/lib/python3.14/site-packages python3 …`, because
numpy is only in system Python):

```python
from enigma.machine import EnigmaMachine
def pye(refl, greek, order, rings, plugs, start, text):
    r = dict(zip('12345678', ['I','II','III','IV','V','VI','VII','VIII']))
    m = EnigmaMachine.from_key_sheet(rotors=[{'B':'Beta','C':'Gamma'}[greek]] + [r[c] for c in order],
        ring_settings=' '.join(rings), reflector=refl + '-Thin', plugboard_settings=plugs)
    m.set_display(start); return m.process_text(text)
P = 'CH EJ NV OU TY LG SZ PK DI QB'                       # Potsdam 1 May 1945
pye('B','C','438','VCCH',P,'MNNS','OEDM')   # -> 'ELKC'
pye('B','C','438','VCCH',P,'DGUG','SEDM')   # -> 'PUYY'
pye('B','C','438','VCCH',P,'PUYY', ciphertext[:6])   # -> 'IPZAYK'  (operator's trial decrypt on the form)
```

Output:

```
P1030680 1st try (Girard)    OEDM@MNNS rings VCCH: ours=ELKC py-enigma=ELKC expected=ELKC OK | with rings AACU: DJZV
P1030680 2nd try (Girard)    SEDM@DGUG rings VCCH: ours=PUYY py-enigma=PUYY expected=PUYY OK | with rings AACU: KTRP
P1030690 (Hoerenberg)        YMUZ@IBFK rings VCCH: ours=ODFF py-enigma=ODFF expected=ODFF OK | with rings AACU: MRIF
P1030660 30 Apr              AAYL@VGWP rings EPEL: ours=MVZQ py-enigma=MVZQ expected=MVZQ OK | with rings AAEL: WGXU
P1030666 30 Apr              JCAP@MZAU rings EPEL: ours=TUYB py-enigma=TUYB expected=TUYB OK | with rings AAEL: BVZZ
Operator trial decrypt PUYY, first 6 body letters: IPZAYK (form row 3-4: IPZA YK)
Operator trial decrypt ELKC, first 6 body letters: DACEZB
```

What this settles:

- **Direction and content.** The group enciphered is the 4-letter Verfahrenkenngruppe **including the filler**.
  The Schlüsselkenngruppe is never enciphered. The output is the 4 window letters, greek wheel first.
- **Ring semantics.** The procedure works only with the **true** rings. Hörenberg's AACU (and AAEL for 30 Apr)
  are body-equivalent normalisations: the greek and left rings are shifted, and the right ring is moved by 13,
  which keeps the turnover phase because wheel VIII has notches Z and M. They give wrong message keys at the Grund.
  **Trap for the solver:** hill-climb tools that output normalised rings will fail any indicator test unless the
  test searches the whole ring-equivalence class.
- **Transcription.** `IPZAYK` from our transcribed ciphertext matches the operator's pencilled 1945 decrypt. So
  the published ciphertext's first 6 letters (JCRSAJ) agree with what he received, and the body starts directly
  after the two indicator groups.
- **Uniqueness.** Exhaustive search over all 26⁴ Grunds finds **DGUG as the only Grund** with SEDM→PUYY,
  confirming Girard.

## 5. What a *known* Grund would buy (the conditional analysis, for completeness)

This section applies only if the Thetis Grund were known. §2 of the TL;DR says it is not.

**Body-level search space.** For each (reflector ∈ {b,c}, greek ∈ {β,γ}, order ∈ 336), the body sees:
- the core offsets c = K − r at the start (26⁴);
- the right-wheel turnover phase (26; effectively 13 for VI/VII/VIII on the right);
- occasionally whether and when the left wheel steps.

That is about 1344 × 26⁵ ≈ 1.6·10¹⁰ rotor hypotheses, each needing a plugboard climb.

**With a known Grund.** Enumerate (reflector, greek, order, r ∈ 26⁴) = 6.1·10⁸ hypotheses. K = f(σ, r) is then
deterministic and there is no free position. This is a *reparametrisation*, not the removal of a dimension: it
gains one factor of about 26 (4 letters of crib worth 26⁴, minus 26³ ring values the body can't see). The cost is
that K jumps whenever the climb changes a plug that touches S, E, D, M or the output letters, which makes the
hill-climb landscape discontinuous.

**Measured as a post-filter** (`tools/indicator/filter_strength.py`). The input is a body-level candidate
(rotors + core-offset trajectory + correct σ). The test: is there any ring set r whose indicator-derived K
reproduces the candidate's trajectory?

| case | true key passes | ring sets matching true trajectory | random wrong candidate passes |
|---|---|---|---|
| Potsdam 438, SEDM@DGUG | yes | exactly 1 (VCCH) | 5.0 % |
| Potsdam 438, OEDM@MNNS | yes | exactly 1 | 4.3 % |
| Potsdam 438, YMUZ@IBFK | yes | exactly 1 | 4.7 % |
| order 432 (single-notch right) | yes | exactly 1 | 2.8 % |

Body-equivalence class size ≈ 26 · 26 · ~20 · 2 = 27 040 ring sets with VIII on the right, and about 13 520 with a
single-notch right wheel. It collapses to 26 · 26 · 2 = 1352 when the left wheel steps inside the message. Mahon
p. 110 gives the same 26 × 26 × 2 = 1,352 clip count. So a known Grund would be a filter of about 1/20–1/35 per
reading, roughly 1/10–1/18 when testing both readings. It would also **pin all four true ring settings** of a
found key, and it works only if σ is fully right on the indicator letters.

## 6. What the indicator actually gives for Thetis (Grund unknown)

- **Existence constraint.** "Some Grund G gives E_G(SEDM) = K" holds for 47–52 % of all K for a given key
  (`tools/indicator/unknown_grund.py`), i.e. about 1 bit. Most of that is the random-function image effect,
  which depends on the key and can't be used.
- **No self-encipherment.** The part that doesn't depend on the key is: k₀ ≠ S (O for the VA reading), k₁ ≠ E,
  k₂ ≠ D, k₃ ≠ M. This is the "crash" rule Hut 8 used to "boil out" clips. For a single message:
  - The rings are unobservable, so the only thing it touches in the body is k₃, via the right-wheel turnover phase.
  - With VI/VII/VIII on the right the two body-equivalent phases can't both equal M, so nothing is excluded.
  - Otherwise it rules out 1 of 26 phases.

  Use it as a free sanity check, not a pruning tool.
- **Thetis on the old system.** If Thetis was still on a single daily Grund (like Sucker, Shark and Narwhal), all
  Thetis messages of 1 May 1945 would share it. Any second Thetis message from that day would then give a strong
  multi-message constraint. None is known. I checked all 57 U-534 pages on Hörenberg's site: every other Enigma
  message there is broken under the 30 Apr, 1 May or 2 May Potsdam key, and P1030670 is RHV (a hand cipher).

## 7. VR vs VA, bigram tables, K-Buch

- **VR/VA does not affect the net identification.** The discrepancy touches only the Schlüsselkenngruppe filler
  (E/K) and the first letter of the Verfahrenkenngruppe (S/O). ACH, and therefore Thetis, is the same under both
  readings.
- **Which reading?** In Morse, R (·−·) and A (·−) differ by one trailing dot. A dot dropped at the start or a dot
  gained in the repeat are both plausible, and I see no strong prior. The first-received VR, which the operator
  settled on, is marginally preferred. With an unknown Grund the question barely matters: it changes only whether
  k₀ ≠ S or k₀ ≠ O. Test both readings if a Grund ever becomes available.
- **Bigram table.** Quelle Tafel A (Prüf-Nr. 1772) and Tauschtafelplan *Bruno* are published as scans on
  Hörenberg's site, front half only (first letters A–M). Because the table is reciprocal, much of the back half
  can be recovered. It is net-independent by necessity, since it's used *before* the net is known. It is fully
  sufficient for this indicator.
- **K-Buch.** Hörenberg holds a physical K-Buch (M.Dv. 98, 1941 edition, No. 1073) and scanned the relevant
  Gruppenliste page and the Zuteilungsliste. The **Grund tables are not in the K-Buch** and, per Girard, don't
  survive in public.
- **Archival lead, not verified.** Mahon records that the Grund tables were *captured* in May 1945 and that
  naval cipher material was surrendered. If the Thetis keylist and Grund table for May 1945 were among the
  captured or surrendered material, they may exist in UK archives (TNA HW-series / ADM 223 captured-document
  files). That would be the one route by which this indicator becomes decisive, and a key sheet would make the
  whole problem trivial. Worth a targeted archive/catalogue search by the context vector.

## 8. Recommendation for the solver

1. **Treat the message start as fully unknown.** Run the planned unconstrained ciphertext-only M4 search:
   reflector × greek × order × 26⁴ core offsets × right-wheel phase, plus a plugboard climb. The indicator does
   not reduce it.
2. **Output rings in a normalised form, and never compare them against indicator-derived rings without expanding
   the equivalence class** (§4 trap).
3. **Free sanity check on finalists:** message key letters 1–4 ≠ (S|O), E, D, M. This is almost always
   satisfiable.
4. **If a Thetis Grund (or its table) is ever found:** use `tools/indicator/filter_strength.py`'s logic as an exact
   post-filter, which must cover the full body-equivalence class of about 13.5k–27k ring sets per finalist. Then,
   for each top candidate's σ, enumerate all 26⁴ ring sets, derive K, decrypt and score. This "constrained polish"
   costs about 4.6·10⁵ 72-letter decrypts per candidate. Test both (SEDM) and (OEDM)
   readings.
5. **Don't spend compute on DGUG/MNNS as Thetis Grunds.** Primary sources say the tables were per-key, so they
   are Potsdam cells. If someone insists on the hypothesis, the §5 post-filter costs about 14k–27k
   four-letter encipherments per finalist (the whole ring-equivalence class) and is harmless to run.

## Files

- `tools/indicator/m4ind.py`: vectorised numpy M4 (stepping, rings, plugboard), cross-checked against py-enigma.
- `tools/indicator/verify_girard.py`: reproduction of ELKC / PUYY / ODFF / MVZQ / TUYB / IPZAYK.
- `tools/indicator/filter_strength.py`: known-Grund post-filter strength and equivalence-class sizes.
- `tools/indicator/unknown_grund.py`: how many message keys are reachable when the Grund is free.

## Summary

- **The constraint.** message key = E_D(Grund; Verfahrenkenngruppe + filler), e.g. `SEDM` → K at the Grund. It runs
  under the full daily key: plugboard, all four *true* rings, normal stepping. Verified: ELKC, PUYY, ODFF, MVZQ, TUYB
  and IPZAYK all reproduce, but only with the original rings VCCH/EPEL.
- **The Thetis Grund is unknown.** The 1945 Grund tables were per key-net (Mahon pp. 111–113; Alexander p. 84), so
  DGUG, MNNS and IBFK are cells of the Potsdam May 1945 table. The indicator gives about 1 bit about the Thetis key.
- **Recommendation.** Run the unconstrained body search. Use the indicator only as a sanity check (message key
  letters ≠ S/O, E, D, M). If a Thetis key sheet or Grund table is ever found, use it as an exact post-filter plus a
  26⁴ ring "constrained polish", expanding the whole ring-equivalence class.
