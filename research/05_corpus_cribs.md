# 05 — Corpus, sister messages, and cribs for P1030680 (Vector A)

Research date: 2026-09-23. All pages fetched live via `curl` from
`enigma.hoerenberg.com`, with polite ~0.5-1s delays between requests, raw HTML
saved to a scratch directory during the session (not kept).
Deliverables: `corpus/messages.json`, `corpus/messages.md`,
`corpus/ngrams/*` (see `corpus/ngrams/README.md` for tool-specific usage).

---

## 1. CORPUS

### What was scraped

- **U-534 messages** (the main corpus): every page linked from the sidebar
  under `cat=The+U534+messages`
  (https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+messages),
  **50 message pages**, `P1030659`–`P1030713` (with the numbering gaps
  `P1030674`, `P1030677`, `P1030678`, `P1030680` [moved to `cat=Unbroken`],
  `P1030703` not present under this category; one page, `P1030665`, is
  registered with a leading-space page name `" P1030665"`). Fetched from
  `https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=P103XXXX`
  for each id.
- **M4 Project 2006** (https://enigma.hoerenberg.com/index.php?cat=M4%20Project%202006):
  the 3 historic Erskine/*Cryptologia* 1995 intercepts —
  **Looks** (U-264, page=`Looks%20Message`), **Schroeder** (U-623,
  page=`Schroeder%20Message`), **Rasch** (U-106, page=`Rasch%20Message`,
  broken by Dan Girard in 2013) — plus the `Third%20Message`/`Explanation`
  background pages (no additional plaintext beyond Rasch).
- **Norrköping messages** (https://enigma.hoerenberg.com/index.php?cat=Norrk%C3%B6ping%20messages):
  19 sub-pages (Swedish police intercepts of German naval/Luftwaffe traffic,
  Dec 1939–Apr 1940). **8 have published plaintext** (broken): 4 naval
  (`RQCA`, `PAGE_49_DDVS`, `PAGE_49_TCQB`, `PAGE_81_IRLU`) and 4 Luftwaffe
  (`PAGE_40_AFDFX/AYDJM/PYCMW/ZBAQW`, different register, excluded from the
  naval n-gram corpus but kept in `messages.json`). **11 remain unbroken**
  (ciphertext only, no plaintext): `PAGE_23_COEW`, `PAGE_24_LMWP`,
  `PAGE_47_DOEP/KRNV/PHZH/RTQSX/WRCE/YEXP/ZWBA`, `PAGE_49_KRLR`,
  `PAGE_69_OWLS` (Graf Spee).
- **The Spanish Enigma message** (LRS10, Spanish Civil War era, Spanish
  language) — kept for completeness, excluded from the German corpus.
- **Reservehandverfahren** (RHV) category — background/explainer page only,
  no additional decrypted message text beyond what the RHV-referencing
  Enigma messages (P1030689, P1030695, P1030713, and the RHV-only stub
  P1030670) already give.
- **`534-texts.pdf`** (https://enigma.hoerenberg.com/cms/download.php?cat=60_The%20U534%20messages&file=534-texts.pdf,
  linked from "The messages" page) — turned out to be a **1-page curated
  example list**, "Original Decipherments on Schlüsselzettel (U534)": **16
  messages from 1 May 1945 with exact Uhrzeit (time-of-origin) stamps**,
  0353–2356. This is not the master corpus (it both duplicates some of the
  50 numbered messages and adds a few not otherwise numbered) — its content
  is folded into §2 below rather than into `messages.json`.
- Context/background pages read in full: "The messages" (daily-key table +
  naval abbreviation conventions), "The last days of U 534" (operational
  timeline), "The Kenngruppen System", "ULTRA" (DEFE 3 cross-references),
  "Degarbling the Dönitz Message P1030681" (Girard's letter-by-letter
  reconstruction of P1030681/P1030714).

### Results

**63 messages recorded** in `corpus/messages.json` / `corpus/messages.md`
(50 U-534 + 3 M4 Project + 8 Norrköping broken + 1 Spanish + P1030680 itself
kept for reference, marked unbroken). **61 have usable plaintext.** Per
message: id, source URL, machine, key-net, date (inferred — see below),
full key (reflector/Greek/wheels/wheel-positions/rings/plugs), indicator
groups, ciphertext, ciphertext length/group count, plaintext, German
interpretation, English translation, and notes.

**Dating method:** the U-534 pages do **not** carry an explicit per-message
date field. Hörenberg's team recovered exactly 3 Potsdam-net daily keys
(30 Apr / 1 May / 2 May 1945; see "The messages" page), so each message was
dated by matching its (reflector, Greek, wheels, rings) tuple against those
three. Result: **17 messages on the 30 Apr key, 31 on 1 May, 1 on 2 May**
(P1030713), plus P1030670 (RHV, no Enigma key, undated by this method — but
see §2, it is almost certainly part of the same 1224-hrs RHV thread as
P1030689/695/713, i.e. ~1–2 May). **Whether the daily key-day boundary falls
at midnight or (per the site's own claim) at noon is genuinely unresolved —
see §2 — so treat the 30-Apr/1-May split as approximate**, not a confirmed
clock boundary.

Two U534 messages, **P1030706 and P1030707, are verbatim duplicates** of
P1030702 and P1030704 (kept in `messages.json` as real, separately-
transmitted messages, but excluded from the n-gram training corpus — see §4).

**Pages found but not parsed / not broken** (recorded per the task
instruction): the 11 unbroken Norrköping pages listed above; the RHV
category background page (no message text); U-534 numbering gaps
P1030674/677/678/703 — **directly fetched and confirmed** these page names
do not exist as distinct entries: each request returns HTTP 200 but silently
falls back to the category's generic "U534" background page (moziloCMS's
behaviour for an unrecognised `page=` value), not a message page. No page
that *was* a genuine message page failed to parse — the HTML structure was
consistent enough across all ~90 pages fetched that a single parser handled
all of them (one message, P1030705, uses a slightly different heading,
"Possible interpretation" instead of "Interpretation" — that variant is
captured correctly in `messages.json`).

---

## 2. SISTER MESSAGES

### Method

Per the task, every message dated 30 Apr–2 May 1945 in the corpus was read
in full (all 17 + 31 + 1 = 49 keyed U-534 messages, not a sample), looking
for (a) an 18-group/72-letter body, and (b) thematic overlap with Thetis/
training-net-plausible content (FdU Ausbildung, evacuation, Dönitz
succession, Rot 01/Rot 07, Gruppe Nollau, Hörup Haff, Rønne, Kiel/Fehmarn).

### Length match: P1030698 is the only exact hit

Of 49 dated messages, **exactly one has an 18-group (72-letter) body**:
**P1030698**, 1 May 1945 (Potsdam key), plaintext
`TTTFFFZWOVIERVVVFXDXUUUXAUSBXXTRAVEMUENDEBLEIBENXWEITEREBEFEHLEABWARTKNX`
= *"[To] T.F. 24 from Comsubs Training [F.d.U. Ausbildung]: Remain in
Travemünde. Await further orders."* This is doubly notable: it is the
**only length match in the whole corpus**, and its sender, **FdU Ausbildung
(Führer der U-Boote, Ausbildung — "Commander of U-boats, Training")**, is
the organizational owner of exactly the kind of Baltic training net Thetis
is believed to be (per Ralph Erskine/uboat.net: *"[Thetis] the cipher for
U-boats on tactical training exercises in the Baltic"*).

**We tested it as a literal MVUEH-style crib, not just a format precedent.**
The MVUEH break (per `03_provenance.md` §"Unrelated find") used a crib
borrowed verbatim from a same-day sister message with near-identical
content. We ran the classical Enigma **no-self-encipherment test** (a plain
substitution property inherited by every Enigma rotor state: ciphertext
letter ≠ plaintext letter, at every position, for the true key) against
P1030680's real 72-letter ciphertext
(`JCRSAJTGSJEYEXYKKZZSHVUOCTRFRCRPFVYPLKPPLGRHVVBBTBRSXSWXGGTYTVKQNGSCHVGF`):

```
P1030698 plaintext, offset 0: collision at position 22 (U=U)
                               and position 33 (V=V)
P1030698 plaintext, offset 1: collisions at positions 12, 22, 46
```
(72-letter ciphertext, 71-letter crib ⇒ only offsets 0/1 are geometrically
possible.) **Both offsets collide — verbatim reuse of P1030698's own wording
is impossible.** So P1030680 is *not* a Potsdam-net re-transmission of the
literal P1030698 text. This matters because it rules out the single
strongest "free win" hypothesis cleanly, rather than leaving it as an open
maybe. What survives is the weaker, still useful claim: P1030698 is the
best available **register/format template** — same day, same 72-letter
length class, same likely-sender organization (FdU Ausbildung), same
telegraphic conventions (`TTT FFF <number>` = "T.F. <number>" =
Torpedofangboot; `FFF TTT` = "F.T." = Funktelegramm — these are *not*
interchangeable, see §3a; `VVV`/doubled `VON VON` = "von"; `X`/`XX` =
period/colon).

For Vector D, we also ran the collision test on **just the message body**
(`TRAVEMUENDEBLEIBENXWEITEREBEFEHLEABWARTEN`, 41 letters, i.e. P1030698's
text *without* its `TTTFFFZWOVIERVVVFXDXUUUXAUSBXX` header) at every offset
against the ciphertext: it survives at **offsets 7, 11, 14, and 17** (out of
32 possible). This is close to the ~6 survivors expected by pure chance for
a 41-letter string against a 72-letter ciphertext ((25/26)^41 × 32 ≈ 6.6), so
it is **not** meaningful evidence for any of those four offsets specifically
— recorded here only so a follow-on Vector D pass does not have to
re-derive it.

### Timing refinement — an open question, including a correction of our own

The scanned P1030680 form has a disputed handwritten "10 30" next to the
"um ___ Uhr" line (`03_provenance.md` §2: unconfirmed which field it
belongs to). If that is a genuine ~10:30 time on 1 May, which "day-bucket"
of Potsdam-net traffic is truly contemporaneous with it matters — so we
tried to pin down whether the Potsdam key changed at ordinary **midnight**
or at **noon**, as the site's own material suggests.

**First pass (in an earlier draft of this report) claimed a confirmed
midnight rollover — that claim was wrong and is retracted here.** We had
matched `534-texts.pdf`'s entry `1.5.0523 Mit U-3024 (Blaich) und U-2538
(Klapdor) Gjedser mit 7 sm Fahrt auf March nach Travemünde passiert. U-3014
(Marbach)` to message **P1030683** and called it a "word-for-word match."
It is not: P1030683's actual plaintext is `GRUPPENMARSCH MIT U-3024 UND
U-2538 NACH TRAVEMUENDE VON U-3014 (MARBACH)` (100 letters) — it contains
none of "Gjedser," "7 sm," "passiert," "Blaich," or "Klapdor." The PDF's
richer 0523 entry is almost certainly a *different*, more detailed message
from the same sender (U-3014/Marbach) about the same convoy, most likely one
not among our 50 numbered/keyed messages at all — so it gives **no** usable
key-vs-time crosscheck, and the "midnight, rejected" conclusion built on it
does not hold. (Caught on advisor review; flagging the error explicitly
rather than quietly fixing it, since it changes a downstream conclusion.)

**With that (sole) counter-evidence gone, the balance of what we actually
have points the other way, though still short of proof:**
- The site's own explicit claim stands unretracted: P1030699's translator
  note says *"The Germans changed the key worldwide every day at 12 o'clock
  midday. Reference: DGZ."*
- **P1030702/706** ("0120 [Uhr] bei Rot 07 geankert... erwarte Geleit Lech")
  is keyed with the **1 May** Potsdam key, yet Girard's "Last days" timeline
  places this exact anchoring event at **2 May**, 0100–0120 — i.e. the "1 May"
  key was apparently still in force at 2 May ~0120, which is hard to square
  with a plain midnight rollover (under which "2 May 0120" should already be
  on the 2-May key) and easier to square with a later, e.g. noon, rollover.
  This is real but not conclusive either: the "2 May" dating for that event
  is itself Girard's own timeline reconstruction, not a directly quoted TOO
  we independently verified.
- We attempted one more crosscheck via the PDF's `1.5.1647 F.d.U-Ausbildung
  von 5.U-Flott. FT1123,61 wird bestätigt. In Marchsetzung TF18...` entry,
  which reads as 5. U-Flottille's confirmation reply to **P1030669** (FdU
  Ausbildung's SSD order: TF18→Neustadt, TF19→stay in Kiel, "Bestätigung
  erbeten" — and P1030669 is on the **30 April** key). If "1123,61" is
  P1030669's own TOO+serial (11:23), and the reply arrived at 16:47 the same
  calendar day, that would need the 30-April key to still be valid past
  11:23 on 1 May — again favouring a delayed/noon rollover. But this
  inference assumes same-day reply turnaround, which we have no independent
  way to confirm (radio replies to "Bestätigung erbeten" requests plausibly
  took anywhere from minutes to over a day in this period) — so we record it
  as suggestive, not decisive.

**Net position: genuinely unresolved with the data gathered this session.**
If the rollover is near-midnight, a 1 May ~10:30 message sits in the
31-message "1 May" bucket (which includes P1030698). If it is near-noon (per
the site's own claim, and the two data points above), it sits in the
17-message "30 April" bucket instead — whose closest length matches are
P1030666 (68 letters, "Hänge mich an Geleit U-4701 an. U-2338 (Kaiser)") and
P1030679 (88 letters, Kom.Adm./FdU Ausbildung/Torpedoinspektion/Tender
Mosel), neither an exact 72-letter match. **This uncertainty does not affect
the P1030698 collision-test result above** (that rules out verbatim reuse
regardless of timing); it only affects which set of messages should be read
as "same few hours" versus "same calendar day." We recommend Vector B
re-derive the Uhrzeitgruppe field from the P1030680 form scan directly
(03_provenance.md flags it as illegible at the resolution available) as the
cleanest way to close this out, since in this corpus the number after the
TOO's separator is consistently a serial, not a date (`1123,61`, `1224/62`,
`1927/89`, `1152,25`) — meaning the disputed "...531/30" reading in
`03_provenance.md` is more likely `HHMM` plus a serial "30" than anything
resolving the date question directly.

The same PDF independently **triple-confirms** message P1030712's timing —
PDF: `1.5.?1832 5.U-Flott. Von U534 (Nollau) 18 Uhr im Geleit von 2014 von
Rot 01 bis Rot 07`; corpus P1030712 plaintext: same content; and Girard's
"Last days" timeline: *"1 May 1832: U-534 is on its way from Rot 01 to
Rot 07 together with V-2014"* — three independent sources, one event,
exact agreement. It also gives two genuinely new plaintext fragments not
otherwise in the 50-message set, both useful crib/context material (§3):
a Kiel **Festungsalarm** ("fortress alarm") declared at 0415 and stood down
at 1141 on 1 May (`Kr Kr An alle U-Boote von 5.U-Flott. Festungsalarm für
Kiel` / `Kr Kr Alle Boote 1310 Uhr Festungsalarm für Kiel beendet`), and a
2356 scuttling-readiness message from Seeoberleutnant Wogram (*"Klar zum
Versenken gem. Befehl"* — "ready to scuttle per order") — showing scuttling
orders were already circulating as early as 1 May, not only 4–5 May as the
"Last days" narrative's dedicated section implies.

### Other candidates read and ruled less likely

- **P1030694** (60 letters, 1 May, Potsdam key): *"[An] Alle [:] Von 10.
  Sicherungsdivision: Hafen Warnemünde feindbesetzt"* ("To all: from 10th
  Security Division: Port of Warnemünde is enemy-occupied"). A genuine
  `AN ALLE` general-distribution broadcast — exactly the *kind* of traffic
  most likely to be repeated across nets — but per `534-texts.pdf` its TOO
  is `1.5.195x` (~19:50–19:59), roughly 9 hours after the target's putative
  10:30, and 12 letters short of 72.
- **P1030699** (152 letters, 1 May, Potsdam key, garbled after ~char 100):
  *"An Alle von Funkleitung: Die grosse Anzahl unklarer Funksprüche gibt
  Veranlassung darauf hinzuweisen, dass ab 1.5. 1200 Uhr Schlüssel..."*
  Another `AN ALLE` broadcast, administrative in nature (about the key/
  indicator confusion itself) — too long (152 vs 72 letters) to be a direct
  match, but useful as a second confirmed `AN ALLE ... XX` opener.
- **P1030681** (the Dönitz-succession message, 372 letters, `KRKR ALLE XX
  FOLGENDES IST SOFORT BEKANNTZUGEBEN...`, 30 Apr key): far too long (372
  vs 72 letters) to be a same-content resend on Thetis. Ruled out on length
  alone; the `534-texts.pdf` also shows the same `KR KR ... AN ALLE` opener
  template reused independently for the Kiel Festungsalarm on 1 May — this
  looks like a recurring formal *marker for urgent fleet-wide messages*
  rather than being specific to the Dönitz text, so its **opener template**
  (not its content) remains a live crib candidate (§3).
- **P1030675** (724 letters, `SSD`/`BINE` urgent, FdU Ausbildung + 4th/25th/
  26th U-Flottillen, sailing-discipline order) and **P1030669** (288 letters,
  `SSD`, FdU Ausbildung ordering Torpedofangboote to Neustadt/Kiel): both
  confirm FdU Ausbildung is a very active, urgent-order-issuing sender on
  exactly these dates, but both are far too long for a length match.
- U-534's own outgoing traffic (**P1030702/706**, "0120 Rot 07 geankert,
  erwarte Lech"; **P1030712**, "1832 Rot01→Rot07 mit V-2014") confirms the
  boat's own operational timeline independently (matches "Last days")
  but is Potsdam-net, not a candidate for a Thetis-net message *received*
  by U-534.

### Bottom line for Vector A/D

No same-day message in the recovered corpus is a verbatim sister of
P1030680 — the collision test rules that out for the one length-matching
candidate we found. The most defensible, corroborated conclusions are:
(1) the message almost certainly originates from, or passes through, **FdU
Ausbildung** (the Baltic training command that plausibly owns Thetis), given
that command's disproportionate presence in same-day, similar-length,
similar-register traffic; (2) `AN ALLE ...`, `KR KR ... AN ALLE ...`, and
short administrative single-topic orders (`[RECIPIENT] VON [SENDER] XX
[order]. [order].`) are all attested, real openers in this exact command's
traffic on this exact date; (3) the true text is very unlikely to be
recoverable by literal reuse — Vector D (a positioned crib/Bombe search)
should treat P1030698's *template*, not its *wording*, as the working
hypothesis, and test short structural fragments (`FXDXUUUAUSB`, `ANALLE`,
`VONVON`, `XX`) at their surviving offsets (§3) rather than the full string.

---

## 3. CRIBS

### 3a. Naval message conventions (confirmed from the corpus + "The messages" page)

- **Numbers spelled out**: NUL(L)=0, EINS=1, ZWO/ZWEI=2, DREI=3, VIR/VIER=4,
  FUNF/FUENF=5, SECHS=6, SIBEN/SIEBEN=7, ACHT=8, NEUN=9. Confirmed at high
  frequency in the corpus (e.g. ACHT 20x, SECHS 17x, NACH 19x).
- **Punctuation substitution**: X = period/space, Y = comma, XX = colon,
  UD = question mark, YY = dash/slash, `KK...KK` = parentheses,
  `J......J` = stress mark (quotation-mark-like emphasis).
- **CH → Q: checked against this corpus and found FALSE for this traffic —
  correcting the task brief's assumption.** The task brief states naval
  convention writes CH as Q. Our corpus does not support that for this
  net/period: 152 literal `CH` occurrences vs. only 23 raw `Q`s in 8,494
  letters, and head-to-head word counts are unanimous — `NICHT` 7/`NIQT` 0,
  `SECHS` 17/`SEQS` 0, `ACHT` 20/`AQT` 0, `NACH` 19/`NAQ` 0, `MARSCH` 9/
  `MARSQ` 0. **Independently confirmed** by Stefan Krah's own naval-message-
  trained dictionary (`tools/enigma-suite/dict/00trigr.cur`, built from real
  WWII Kriegsmarine Enigma traffic for the 2006 M4 Project): trigram `CHT`
  ranks **#2** overall (60,434), while `SEQ` is **unattested** (value 0). Two
  independent real-WWII-naval sources agree: **cribs for this message should
  spell CH literally** (`NICHT`, `SECHS`, `ACHT`, `NACH`, `MARSCH`, ...), not
  Q-substituted. (Q-for-CH does appear, messily, in some of the lower-quality
  1939-40 Norrköping Luftwaffe intercepts in this same corpus — it may be a
  real convention elsewhere/elsewhen, just not evidenced here.) See
  `corpus/ngrams/README.md` for the full count breakdown; this also drove a
  correction to the n-gram blend in §4 (an earlier draft over-weighted
  trognes' `wehrmacht` table, which is explicitly Q-for-CH by its own
  `--help` text, i.e. the wrong register for this specific corpus).
- **Standard tokens**: `UUU` = U-boat/U- prefix (e.g. `UUU4701` = U-4701);
  `VVV` and `VON VON` (doubled) = "von" (from), used as a sender marker,
  often doubled for redundancy; **`TTT FFF <number>` = "T.F. <number>" =
  Torpedofangboot** (confirmed: P1030698 `TTTFFFZWOVIER` = "T.F. 24"; P1030659
  `...TTTFFFZWODREI` = "T.F. 23"), **while the reverse order, `FFF TTT`, =
  "F.T." = Funktelegramm** (a generic radio signal, confirmed: P1030686
  `...VONFFFTTTMIL...` = "von F.T. mit..."; P1030697 opens `FXDXUUUVSTVVVTTT`
  = "F.d.U.[O]st von T.F." i.e. Torpedofangboot 17 as sender) — **these two
  three-letter-doubled abbreviations are easy to conflate and mean different
  things depending on word order**; `FFF DDD UUU` (a third, distinct triple)
  = "F.D.U." (Führer der Unterseeboote); `SSD` = "Sehr Sehr Dringend"
  (extremely urgent), with `BINE`/`MUKE`/`WESPE` as alternate covert urgency
  codewords; `KR KR` = a war-emergency/high-priority marker seen opening
  both the Dönitz message and (per `534-texts.pdf`) a Kiel fortress-alarm
  message; `NNN`/`OOO`/`SSS` = weather-report section markers (barometer
  trend, cloud, etc., seen in the Looks/Schroeder M4-Project messages).
- **Address block, by far the dominant opening pattern**: `[RECIPIENT(S)]
  VON [SENDER] XX [text]` or `[RECIPIENT] VVV [SENDER] XX [text]` — "AN"
  ("to") is usually implicit (recipients simply lead the message) rather
  than spelled out, **except** for `AN ALLE` (confirmed twice literally:
  P1030699 `ANALLEVONVONFUNK...`, and the PDF's `Alle von 10.(sidi)` for
  P1030694, plus `KR KR ALLE` for the Dönitz message and the Kiel alarm).
- **Common recipients** (frequency, from all 52 messages with a German
  interpretation): U-Flottille 19x, F.d.U. Ausbildung 8x, U-Boote 8x,
  Kommandierender Admiral (U-Boote) 5x (`KOMXADMXUUUBOOTE` opens 5
  messages **byte-identically**), F.d.U. Ost 12x (as a word "Ost"), Leitung/
  Funkleitung 4x each.
- **Sign-offs**: highly idiosyncratic per message — usually a 2-4 letter
  code or a commander's name in `KK...KK` brackets (e.g. `KKMARBACHKK`,
  `KKNOLLAUKK`), not a fixed formula. No reliable end-of-message crib exists
  in this corpus (unlike the strong start-of-message clustering below).

### 3b. Word/phrase frequency (52 messages with a German gloss)

| word | count | | word | count |
|---|---|---|---|---|
| VON | 58 | | GEANKERT | 5 |
| AN | 43 | | SEHR | 5 |
| MIT | 25 | | KOMMANDIERENDER | 4 |
| UND | 22 | | BEFEHL | 4 |
| U-FLOTTILLE | 19 | | LEITUNG | 4 |
| NACH | 17 | | FUNKLEITUNG | 4 |
| UHR | 16 | | WEG | 4 |
| OST | 12 | | LECH | 4 |
| AUSBILDUNG | 8 | | U-1103 (EISELE) | 3 |
| U-BOOTE | 8 | | | |
| GELEIT | 7 | | | |
| TRAVEMÜNDE | 7 | | | |
| TORPEDOFANGBOOT | 6 | | | |
| WARNEMÜNDE | 6 | | | |
| KIEL | 6 | | | |
| ROT | 6 | | | |

*First-3-letter opener distribution (49 dated U-534 + 3 M4-Project
messages, literal ciphertext-equivalent plaintext prefixes)*: `UUU` opens
9/52 (17%), `KOM` 5/52, `FXD` 4/52, `LEI` 3/52, `VON`/`VVV`/`FFF`/`TRA` 2/52
each. **`KOMXADMXUUUBOOTE` (16 letters) opens 4 messages byte-identically**
(P1030668, P1030685, P1030693, P1030710) plus a 5th with one garbled letter
(P1030700, `KOMXBDMX...`).

### 3c. Ranked crib candidates for P1030680

For each we ran the no-self-encipherment (collision) test against the real
ciphertext (`JCRSAJTGSJEYEXYKKZZSHVUOCTRFRCRPFVYPLKPPLGRHVVBBTBRSXSWXGGTYTVKQNGSCHVGF`).
**Read this test as a weak, necessary-but-far-from-sufficient filter**: for
crib lengths under ~20 letters, a large fraction of offsets survive purely
by chance (survival probability ≈ (25/26)^len per offset); it only becomes
strongly diagnostic for full-message-length hypotheses (§3d) or once
combined with an actual stecker-consistency/Bombe search (Vector D,
`kionactf/enigma_attacks`' `bombeAttackEnigmaPlugboard`, once rotor
order/ring/start are narrowed by Vector B or C).

| # | Crib | Position hypothesis | Justification | Collision-test result |
|---|---|---|---|---|
| 1 | `FXDXUUUAUSB` (F.D.U. AUSB) | Start | Same-day, same-length-class sender (P1030698); FdU Ausbildung is the corpus's dominant urgent-order issuer this week | 37/60 offsets survive (weak alone) |
| 2 | `ANALLE` / `ALLE` | Start (after a short address prefix) | Attested 3x same-day/adjacent-day (P1030694, P1030699, PDF Festungsalarm); Thetis being a shared training net makes a fleet-wide broadcast plausible | many offsets survive |
| 3 | `KRKRALLE` | Start | Attested opener for both the Dönitz succession message and (per PDF) the Kiel Festungsalarm — a recurring "urgent fleet order" template, not just Dönitz-specific | 36/63 offsets survive |
| 4 | `KOMXADMXUUUBOOTE` | Start | Single most frequent literal 16-letter opener in the whole corpus (4 byte-identical hits) — plausible if P1030680 was copied "for information" to the top submarine command | 36/57 offsets survive; **caveat**: this corpus's instances are mostly *incoming reports to* Kom.Adm., a different traffic direction than a probable FdU-Ausbildung-authored broadcast |
| 5 | `VONVON` | Start (sender self-ID) | Doubled-VON self-identification opens Looks (1942) and P1030671; general naval-Enigma redundancy convention | 47/64 offsets survive |
| 6 | `LEITUNGVONVON` / `FUNKLEITUNG` | Start or mid-message | "Leitung"/"Funkleitung" (radio control) opens 3-4 messages, mostly administrative/procedural notices about garbled traffic — plausible register for a training-net housekeeping message | 35-42 offsets survive |
| 7 | `TRAVEMUENDE` | Anywhere, likely early-mid | 7 occurrences corpuswide; Travemünde is the 25./26. U-Flottille training-boat home port, directly tying to FdU Ausbildung/Thetis | not separately tested (too short at 11 letters to be diagnostic) |
| 8 | `WEITEREBEFEHLEABWARTEN` ("await further orders") | End (mirrors P1030698's closing clause) | Exact closing idiom of the one length-matched sister-template message | 22/51 offsets survive |
| 9 | `SSD` / `BINE` (urgency marker) | Start | FdU Ausbildung issued 2 different `SSD`-flagged urgent orders this same week (P1030669, P1030675) | too short to be diagnostic alone |
| 10 | `XUUUFLOTTX` (numbered flotilla, period-bounded) | Anywhere | Flotilla numbers (25./26./4./5. U-Flottille) are the most common addressee/subject after FdU Ausbildung itself | 43/63 offsets survive |

**Reasoning on who sent it and why, at ~10:30 on 1 May 1945:** U-534 held
the Potsdam key, not Thetis, and its operator gave up decrypting once he
identified the net (`03_provenance.md` §3) — so U-534 was *not* the intended
recipient in the normal sense; it simply logged a broadcast it happened to
intercept while monitoring the frequency (naval practice: radio rooms copied
everything on a scheduled Welle regardless of which key they held, in case
key material arrived later, per Girard's own account of the operator's
attempt). This favors a **one-to-many broadcast** over a bilateral message:
either (a) a `KR KR AN ALLE` or `AN ALLE` fleet-wide urgent notice (the Kiel
Festungsalarm precedent at 0415/1141 that day shows this genre existed on 1
May specifically), or (b) a routine FdU-Ausbildung administrative order to
one specific training unit that every boat on the Thetis net would still
copy down (the P1030698 precedent). Given the short length (72 letters, the
shortest of any "AN ALLE"-style message we found, and about half the length
of the two confirmed `AN ALLE` broadcasts in the Potsdam corpus), option (b)
— a short, single-topic operational order — is marginally favored by length
alone, but (a) cannot be excluded; both hypotheses are written up below.

### 3d. Full-length (72-letter) plausible plaintext reconstructions — HYPOTHESES ONLY

These are illustrative, register-authentic constructions built from real
corpus phrase fragments, **not** derived from any cryptanalysis of the
actual ciphertext. Each was run through the same collision test as a basic
due-diligence sanity check (not proof of anything) — and, as expected for
hand-written guesses of this length, **each one fails** at 1-4 positions,
meaning the *exact wording as written* is provably wrong. They are recorded
anyway because the *positions and structure* that survive/fail are useful
inputs to Vector D, and because the task asked for these explicitly as
hypotheses.

1. **FdU Ausbildung general stand-fast order** (theme: P1030698 + P1030699):
   `FXDXUUUAUSBXANXALLEXXWEITEREBEFEHLEABWARTENXVORLAEUFIGIMHAFENVERBLEIBENX`
   ("FdU Ausb to all: await further orders. Remain in harbour for now.")
   — 72 letters; collisions at positions 26 (R), 44 (V), 61 (V).

2. **Funkleitung administrative/key-procedure notice** (theme: P1030699):
   `ANALLEVONVONFUNKLEITUNGXXABEINSXFUENFXZWOLFUHRNEUERSCHLUESSELBEACHTENXX`
   ("To all from Radio Control: as of 1.5, 1200 hrs, note new key
   [instructions].") — 71 letters; collisions at positions 15, 32, 50, 51.

3. **Training-boat position report to FdU Ausbildung** (theme: P1030702/712):
   `FXDXUUUAUSBXVONVONUUUDREINULZWOXROTNULEINSVERLASSENXMARSCHNACHKIELXENDX`
   ("FdU Ausb, from U-302[?]: left [point] Rot 01, marching to Kiel. End.")
   — 71 letters; collision at position 62 (K).

4. **Urgent Kiel-threat notice, copy to Kom.Adm.** (theme: PDF Festungsalarm
   + P1030668/681): `KOMXADMXUUUBOOTEXFXDXUUUAUSBXXFESTUNGSALARMFUERKIELAUFGEHOBENXENDEMELD`
   ("Kom.Adm.U-Boote, FdU Ausb: fortress alarm for Kiel lifted. End of
   report.") — 70 letters; collisions at positions 4 (A), 22 (U).

None of these should be treated as more than a starting point for a
positioned-crib search — see the note on the collision test's weak power at
these lengths (§3c). We record which exact positions collide precisely so
that a follow-on search can avoid re-deriving that a given exact phrase is
impossible.

---

## 4. N-GRAMS

Built from the same 54-message, 8,494-letter naval-German corpus described
in §1/§3 (2 verbatim-duplicate messages excluded — see §1), in the exact
formats both existing solvers expect. Full detail, usage commands, the
CH-vs-Q correction's effect on the blend weights, and a **real validated
round-trip test** (both tools independently recover a known 72-letter U-534
message's correct plugboard and plaintext using only these files, with a
documented Greek-wheel-notation gotcha found along the way) are in
**`corpus/ngrams/README.md`**. Summary:

- `tools/enigma` (trognes) format, language names **`navalfortyfive`** (pure
  corpus) and **`navalblend`** — a 4-way blend for trigrams/bigrams (35%
  Krah's real-naval `00trigr.cur`/`00bigr.cur`, converted to probabilities;
  25% trognes' `wehrmacht`; 25% our own corpus; 15% trognes' `german`) and a
  3-way blend for monograms/quadgrams (55% our own corpus, 30% `german`, 15%
  `wehrmacht` — `wehrmacht`'s weight is cut hardest here because it is
  explicitly Q-for-CH, the register mismatch found in §3a, and a quadgram
  window is exactly where that matters most). Recommended default for
  Vector C, with a generous restart budget (see README — short messages
  needed up to `-R 800 --polish` to reliably converge in testing, though
  each run is well under a second).
- `tools/enigma-suite` (Krah) format, **`navalfortyfive_trigr.cur`** /
  **`navalfortyfive_bigr.cur`** (lowercase, tab-separated, all 26^n
  combinations enumerated, exactly replicating `mkdict.py`'s own math) —
  recommended as a second opinion alongside the bundled, larger
  `dict/00trigr.cur`/`00bigr.cur` (Krah's tool only accepts one trigram +
  one bigram file per run, so the two can't be blended in *that tool's own
  format* in one invocation — but their signal is not lost, since the
  `navalblend_*` files above already fold in a properly-converted copy of
  the Krah-naval trigram/bigram distributions; run both and compare).

---

## Summary

- **Messages scraped: 63** recorded in `corpus/messages.json`/`.md` (50
  U-534 + 3 M4 Project 2006 + 8 Norrköping broken + 1 Spanish + P1030680
  itself for reference); **61 with usable plaintext**; 11 further
  Norrköping pages found but confirmed still unbroken (listed in §1).
- **Best sister-message candidate: P1030698** (1 May 1945, 72 letters, FdU
  Ausbildung → T.F. 24, "Remain in Travemünde, await further orders") — the
  *only* 18-group message in the whole dated corpus, and from the command
  most likely to own the Thetis net. **Verbatim reuse is ruled out** by the
  no-self-encipherment test (collisions at position 22/33 vs. offset 0);
  its value is as a structural/register template, not a literal crib.
  Secondary candidates: **P1030694** (60 letters, `AN ALLE`, Warnemünde
  enemy-occupied, TOO ~19:50 same day) and **P1030699** (152 letters,
  `AN ALLE`, key-procedure notice) — both confirmed genuine `AN ALLE`
  broadcasts in the immediate corpus, wrong length for a direct match.
  A newly-found primary source, `534-texts.pdf` (a curated 16-message,
  time-stamped decipherment list for 1 May), independently confirmed
  P1030712's timing and surfaced two new same-day plaintexts (a Kiel
  fortress-alarm and a scuttling-readiness message) but no direct match.
  **Correction from an earlier draft**: we had also claimed the same PDF
  confirmed P1030683's timing and, from that, that the Potsdam daily key
  changed at ordinary midnight. Both claims were wrong (the PDF entry cited
  is a different, richer message than P1030683 — see §2) and are retracted;
  whether the key-day boundary is midnight or (per the site's own claim)
  noon is **left open**, which in turn leaves open whether the 17-message
  "30 April" or 31-message "1 May" Potsdam bucket is the better
  "contemporaneous" comparison set for a ~10:30 target — see §2 for the
  full, hedged writeup and next step (reading the form's TOO field directly).
- **Top 10 cribs** (full table with positions/justification/collision
  results in §3c): `FXDXUUUAUSB`, `ANALLE`/`ALLE`, `KRKRALLE`,
  `KOMXADMXUUUBOOTE`, `VONVON`, `LEITUNGVONVON`/`FUNKLEITUNG`,
  `TRAVEMUENDE`, `WEITEREBEFEHLEABWARTEN`, `SSD`/`BINE`, `XUUUFLOTTX`.
- **Corpus-derived correction to the task brief**: this corpus does **not**
  support "CH is written as Q" for this net/period — it writes CH literally
  (152 CH vs. 23 Q in 8,494 letters; zero instances of NIQT/SEQS/AQT/NAQ/
  MARSQ), independently confirmed by Stefan Krah's own real-WWII-naval
  dictionary (`CHT` is trigram #2, `SEQ` is unattested). Build cribs with
  literal CH spellings (§3a).
- **N-gram files**: `corpus/ngrams/navalfortyfive_*.txt` and
  `navalblend_*.txt` (trognes format) and `navalfortyfive_trigr.cur` /
  `navalfortyfive_bigr.cur` (Krah format), all in
  `/opt/enigma-p1030680/corpus/ngrams/`, documented and
  round-trip-validated (including a fix to the blend weights once the CH/Q
  finding above was made) in `corpus/ngrams/README.md`.
