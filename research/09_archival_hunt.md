# 09 — Archival hunt: Thetis key material, DEFE 3, TICOM, and current outside activity

Research date 2026-09-23. Web-only. Downloaded and rendered source files were kept in a scratch directory and are not in the repo; re-fetch from the URLs below if needed.
Every claim below has a URL. Where I could not verify something, I say so explicitly.

## 0. Headline result before the detail

**The message is now public.** Veritasium's YouTube channel (21.2M subscribers) published a video on
2026-09-21 that poses P1030680's exact ciphertext as an open challenge to its audience. See §4. This
doesn't change the cryptanalysis, but it changes the situation: expect chatter, possibly duplicate
independent attempts, and — usefully — Veritasium's own published reference list, which I checked and
which does **not** go beyond Hörenberg's page (no Thetis/Grund/TICOM references there), so this
archival hunt is not preempted.

**The single best concrete action is free.** The National Archives (Kew) holds the actual Bletchley
Park decrypt stream for U-boat traffic covering 1 May 1945 (DEFE 3/743, ZTPGU series) and it is a free
digital download, no order/fee required. See §2.

**No Thetis key sheet, Grund table, or wartime decrypt survives in any source I could find or reach.**
Hörenberg's own claim that "the actual Grund tables used by the German Navy do not appear to exist any
longer" (already quoted in research/06) held up under a much wider search. The strongest new material
is indirect: a captured June 1945 **Triton** (not Thetis) key sheet reproduced in full in a NARA
document (§1.1), which shows concretely what a surviving month-of-1945 Kriegsmarine key sheet looks
like and proves such things do sometimes survive capture — just not, as far as I can find, for Thetis.

---

## 1. Captured German naval key material: TICOM, NARA, Bundesarchiv

### 1.1 R.I.P. 401 — "German Naval Ciphers" (NARA RG 457, ed. Frode Weierud) — the best single find

Source: NARA, College Park, RG 457, NSA Historic Cryptographic Collection, Box 1393, NR 4457,
ARC Identifier 2811218 (https://catalog.archives.gov/id/2811218), reproduced with front matter by
Frode Weierud at **https://cryptocellar.org/enigma/files/nr-4457-rip-401.pdf** (34 MB scanned PDF,
114 pages, OCR text layer present — I pulled the OCR text and cross-checked the specific pages as
page images at 200–300 dpi). Weierud's note: a longer (~260 pp.) version of this same OP-20-G report,
titled *"German Ciphers Methods and Procedures for Decrypting"*, is at NARA RG 38 (see §1.2).

What it contains that matters here:

- **Figure 24b, "Changes in German Codes and Ciphers — December 1941 to VE Day" (PDF p. 68, printed
  page 92).** This is a time-chart, not prose, and I rendered it as an image to read it. Under
  **Home Waters**, one branch reads verbatim: **"U-BOAT EXERCISE (U-UEBUNGSSCHLUESSEL) becoming
  'THETIS' in 1944"**, running as an unbroken line from Dec 1941 straight through to VE Day, parallel
  to "MOTOR TORPEDO BOAT (MTB - becoming 'SLEIPNIR' in 1944)". Unlike the neighbouring Niobe/Narwhal
  and Potsdam/Plaice branches, **the Thetis and Sleipnir lines carry no Allied "fish" covername** —
  every other line on this chart that BP was reading currently has one. That is a second, independent
  piece of evidence (besides Hörenberg's own statement) that Thetis was never broken: BP evidently
  never assigned it a working covername, which BP did only for keys it was actually reading.
  This directly identifies who used Thetis: it is the continuation of the pre-war "U-boat exercise"
  cipher, i.e. the Baltic U-boat training organisation, not a front-line net — consistent with the
  task brief.
- **Caveat / conflict I did not resolve.** Figure 24a, "German Code and Cipher Areas at the Time of
  U.S. Declaration of War" (PDF p. 65, printed page 90), a **map legend dated December 1941**, already
  lists "Thetis, Sleipnir" as a named pairing at that date — i.e. the name "Thetis" appears to predate
  1944 in this same document, in tension with Figure 24b's "becoming 'Thetis' in 1944" caption. I can't
  resolve this from the document itself; it may mean the codename existed early but the chart-maker
  dated its formal adoption for the "U-Übungsschlüssel" line to 1944, or it may be a simple
  inconsistency between two figures drawn at different times. Take "continuously in the Home Waters/
  U-boat-training family from ~1941/42, named Thetis by 1944 at the latest, continuing to VE Day" as
  the safe reading.
- **A real, named, K-Buch Zuteilungsliste ("Cipher Allotment List for Indicator Book") for the
  Kennwort "Forelle"** is reproduced in full as Figure 13 (PDF p. 32, printed page 45; high-res image
  rendered). It lists **"671–703 M Thetis (M Tht)"** — i.e. the exact same table structure, and the
  same net, as the May 1945 table Hörenberg photographed from U-534's own papers (Kennwort presumably
  "Hering", see §3). Figure 14 (effective dates of allotment lists) shows Forelle was in force twice —
  10 Feb 1943 and again 15 Oct 1944 — so this specific scan could be either issue; either way it
  independently corroborates Thetis's column-range/name pattern outside Hörenberg's own material.
- **Actual captured June 1945 key sheets — but for Triton, not Thetis.** PDF pp. 37–38 (printed pages
  39–40) reproduce, as full-page facsimiles, a genuine captured **"Schlüsseltafel M Allgemein, Innere
  Einstellung"** (Inner Settings) and **"Äussere Einstellung"** (Outer Settings / Steckers + Grund)
  sheet for **Schlüssel M "Triton", Monat Juni 1945, Prüfnummer 123**, plus an **Offizier key sheet for
  Triton, June 1945** (PDF p. 45, printed page 51). These are exactly the kind of document we want for
  Thetis and don't have: they show rotor orders, ring settings, 10 days of plugboard wirings, and a
  **Grundstellung column** per day, in the German original. This proves the Allies captured at least
  one net's key material for a month *after* the surrender (Triton, Atlantic U-boats) and that OP-20-G
  reproduced it in this report. It is not Thetis, but it is the concrete template for what a Thetis
  sheet would look like if one turns up, and it is good evidence that late-war/post-surrender key
  material for *some* nets did survive into the RG 457 holdings.
- **The Grund-table system, corroborated with a new detail.** PDF p. 44 (printed page 46): *"during
  the last month of the war the Mediterranean users had a table of grunds that changed from message to
  message... Actual contents of the Key Sheets recovered or captured are to be found in R.I.P. 475."*
  I rendered this exact page at 300 dpi to check the digit string, because the OCR read it as
  "Ris-MV. 475" — confirmed, the number is clearly **475**, not a misread of 403 or another number.
  Two things follow: (a) this is an **independent, US-side confirmation that late-war per-message Grund
  tables were real and that captured key-sheet material existed and was catalogued** somewhere as
  R.I.P. 475; (b) unlike research/06's account (Mahon/Alexander, which puts the 288-Grund system on
  Plaice/Potsdam and Dolphin from 1 May 1945), this US document attributes a per-message Grund practice
  to "the Mediterranean" in the war's last month — a different theatre/net family. These are not
  necessarily contradictory (multiple nets could have independently adopted per-message Grunds in
  spring 1945), but I flag the discrepancy rather than harmonising it.
  **I could not locate R.I.P. 475** in the public finding aid for the NARA collection it should be in
  (§1.2) — see that section for the actionable next step.
- **Bigram tables.** PDF p. 44: the tables "actually used are found in the Library, No. 74" (i.e.
  OP-20-G's internal GYA library numbering) — a lead for the *back half* of the Quelle table (Hörenberg
  only has the front half published; see §3) if anyone can get NARA to pull GYA Library item 74.
  Also of interest: a sixth bigram-table generation, **"Meer" ("Sea"), was distributed May 1945 but
  never used** (PDF p. 44) — a nice period detail, not directly useful here.
- **Setting reuse across nets is a documented German practice, but not demonstrated for Thetis.**
  PDF pp. 91–92 lists nets built by "sliding" a new key out of an old one (same rotor-order pool,
  different Stecker/Grund): Triton out of Heimische Gewässer (Nov 1942), **Niobe out of Hydra**
  (June 1944), Brenessel/Schaltenhalm/Strandmuschel/Hydra I/Hydra II out of Hydra (Sept 1944),
  Wotan/Athena out of Hermes (Sept 1944), Tibet out of Aegir (July 1943). Thetis does not appear in
  this list as either a parent or a child net, so there is no documented "slide" relationship to
  exploit for it. Separately, the captured June 1945 **Offizier** key sheet for Triton (PDF p. 45)
  explicitly says its Inner Settings *are* the Triton Allgemein Inner Settings for that month — i.e.
  reuse is documented *within* a net between its Allgemein and Offizier keys, not *across* different
  nets. I found no documented case of inner-setting reuse between two different Home Waters nets in
  1945 that would let a Thetis attack borrow a Potsdam or Dolphin rotor order/ring set.

### 1.2 NARA RG 38 finding aid (maritime.org / USS Pampanito, Internet Archive copy)

**https://web.archive.org/web/20240914144754/https://maritime.org/doc/nara/ripdocs.php**
("NARA RG 38, Radio Intelligence Publications Finding Aid", stack area 370, College Park). This lists
box-by-box contents of the R.I.P. series at NARA. Confirmed:
- **R.I.P. 401 — "German Ciphers Methods and Procedures for Decrypting", Copy 25 — Box 168**
  (the longer, ~260-page version Weierud's note refers to).
- **R.I.P. 403 — "Special Report Vol. 1, Special British Reports on German Cryptography" — Box 169**,
  immediately next to 401. Not read (no digitised copy found online); worth ordering — it is explicitly
  British reporting on German crypto, the kind of place a Thetis/Grund note could sit.
- **R.I.P. 601–610, "Enigma Series" (OP-20-G technical papers) — Boxes 169–172**, including
  **R.I.P. 604 "Indicator Attacks"** and **R.I.P. 610 "Reports from England"** — US technical papers on
  Enigma indicator cryptanalysis and material received from BP; plausible home for Grund-system detail.
- **R.I.P. 475 does not appear anywhere in this finding aid** (I grepped the full extracted text: no
  match; the numbering jumps 401 → 403 → 480 in the box list). This means either it isn't part of this
  particular RG 38 sub-collection, or it's catalogued elsewhere (it's cited from an RG 457 document, so
  it may itself be filed in RG 457, not RG 38). **Action: ask NARA's reference desk (Archives2reference@
  nara.gov) directly for "R.I.P. 475" across RG 38 and RG 457** — this is the single most direct lead to
  an actual captured Kriegsmarine key-sheet holding I found, and I could not close it from the web.

### 1.3 TICOM archive (archive.org "ticom" item + ticomarchive.com)

The bulk TICOM corpus is mirrored at **https://archive.org/details/ticom** (4,593 files, 357 with an
OCR text layer). I pulled the full file list
(`https://archive.org/metadata/ticom`) and grepped every plausibly-relevant one by name, then
downloaded and full-text-searched the eight most on-topic reports for `Thetis`, `Übungsschlüssel`,
`Grundtafel`/`Grundstellungstafel`, `Kenngruppenbuch`, `Niobe`, `Narwhal`:

- `ADM 1-27186 Review of the Security of Naval Codes and Cyphers, Sept 1939-May 1945` — **turned out to
  be about the security of *British* naval codes** (enemy successes against the Royal Navy), not German
  ones. Negative, and a reminder the title alone can mislead.
- `D-41 Okm4sklIiiOnBritishNavalSystems` (TICOM D-41, OKM/4 SKL III on British naval systems) — negative.
- `Df-225 KmSigintServiceOrganization` (Kriegsmarine SIGINT service organisation) — negative.
- `Df-38 TheEnigma` — negative.
- `GermanNavalRiHistory` — negative.
- `I-165 BaumbachOfOkm` — negative.
- `I-197 ShulzeOfOkm-4SklIii` (interrogation of a member of OKM/4 SKL III, the department that actually
  produced German naval key material) — negative.
- `I-45 OkwchiCryptanalyticResearchOnEnigmaHagelinAndCipherTeleprinterMachines` — 3 hits for "Grund",
  all describing the pre-war 3-rotor Enigma double-trigram indicator system in general terms, nothing
  Thetis- or 1945-specific.

**All eight came back negative for Thetis by name.** I did not exhaustively grep all 357 OCR'd TICOM
files (that's a much larger task); I targeted the ones whose filenames or TICOM series (D-, Df-, I-)
suggested Kriegsmarine crypto-organisation content. TICOM I-38 (interrogation of Lt. Frowein, OKM/4
SKL/III, on four-wheel Enigma security — the analyst who showed M4 could be broken on a 25-letter crib)
is repeatedly cited in secondary sources (e.g.
https://chris-intel-corner.blogspot.com/2020/03/here-is-list-of-ticom-i-reports-1-213.html) but I could
not find it in the archive.org "ticom" item's file list or get a working link on ticomarchive.com from
search results alone; it's a plausible next document to chase directly on ticomarchive.com.

### 1.4 Schlüsseleinsatz — Ciphers in Use, January–May 1945 (TICOM T-1467)

**https://cryptocellar.org/enigma/e-keys/schluesseleinsatz-jan-may-1945.pdf**, ed. Frode Weierud.
Source per its own front matter: *TICOM Document T-1467, Bestand Rückgabe TICOM, S8, Politisches Archiv
des Auswärtigen Amts, Berlin* (German Federal Foreign Office archive — captured TICOM material returned
to Germany after declassification). Author: OKW/WFSt/Ag WNV/Chi A III — this is a **Wehrmacht High
Command signals-cipher office document, not Kriegsmarine**. Its OCR text (596 lines, mostly cover
sheets and Wehrmacht-wide cipher tables — Enigma-G, teleprinter, hand systems for Abwehr/OT/diplomatic
traffic) has **no naval-key or Thetis content**, consistent with its OKW/Chi origin. Negative, but worth
recording as a checked, dated (Jan–May 1945) source so it isn't re-chased later.

### 1.5 Erskine, "Captured Kriegsmarine Enigma Documents at Bletchley Park" (Cryptologia 32:3, 2008, pp. 199–219)

This is, by title, the single most on-target published source for "what was captured and where did it
go" — but it sits behind Taylor & Francis paywall; I could only get abstract-level summaries, not the
text. DOI (confirmed resolving via https://doi.org/10.1080/01611190802088318):
https://www.tandfonline.com/doi/abs/10.1080/01611190802088318 (Cryptologia
index), https://www.researchgate.net/publication/249565883, https://www.semanticscholar.org/paper/
8fcfc1e8a670762120a7bac7bec261bb7d0a2bf9. What's confirmed from secondary summaries: the article
catalogues Enigma-related Kriegsmarine documents captured by Britain in WWII and describes **Naval
Section VI** at GC&CS, the section responsible for captured documents. It very likely contains the
answer to "where in HW/ADM did captured naval key material end up," but I did not get past the abstract.
**Action: get this specific paper** (institutional access, inter-library loan, or purchase — Taylor &
Francis single-article price is typically in the $40–50 range) before spending more archive-search time;
it may make several of the searches below unnecessary or redirect them precisely.

### 1.6 Bundesarchiv-Militärarchiv (RM series)

Not meaningfully searched — BArch's own online finding-aid search (invenio.bundesarchiv.de) is a
separate, harder-to-query system I did not get working results from in the time available, and I found
no secondary source pointing at a specific RM file for Thetis/Grund material. Flag as unexplored, not
as a negative result.

### 1.7 Cryptomuseum, Cryptocellar, Bletchley Park Trust, Christos blog, Klaus Schmeh, Dirk Rijmenants

- **cryptomuseum.com**: has general Enigma-M4/Schlüssel-M pages (cited by Veritasium too, see §4) but
  no page specific to Thetis or a naval key-net list turned up in search.
- **cryptocellar.org** (Frode Weierud): the two best documents found in this whole hunt (§1.1, §1.4)
  come from here. Its general Enigma-keys index (https://cryptocellar.org/enigma/dolphin.html and
  siblings) is worth a manual browse for anything Weierud has posted since these were indexed, but nothing
  Thetis-specific surfaced in search.
- **Christos military and intelligence corner** (chris-intel-corner.blogspot.com): active blog, has a
  TICOM I-report list (2020/03 post) but no Thetis-specific post found by search.
- **Klaus Schmeh's blog** (scienceblogs.de/klausis-krypto-kolumne, klausschmeh.net): no Thetis/P1030680
  post found by search — plausible he'll cover it now that Veritasium has made it a public challenge.
- **Dirk Rijmenants**: has a 2012 post on the U-534 messages generally
  (https://rijmenants.blogspot.com/2012/08/u-534-enigma-m4-messages-cracked.html, about the *other*,
  already-broken U-534 messages) but nothing specific to Thetis or P1030680's archival status.
- **Bletchley Park Trust**: no dedicated online finding aid found; the Trust is a museum/charity, not an
  archive holder for this kind of material (TNA and NARA are); its site is credited as a *consultant* on
  the Veritasium video (§4), which is a live, current point of contact if anyone wanted to ask.

---

## 2. Bletchley Park decrypts for 1 May 1945: DEFE 3 at The National Archives

Used TNA's Discovery API directly (`https://discovery.nationalarchives.gov.uk/API/...`, documented at
https://www.api.gov.uk/tna/discovery/) rather than the JS-rendered search UI.

**HW 25 (GC&CS technical histories) has nothing beyond what research/06 already uses.** I listed all
42 pieces in the series. HW 25/1 = Alexander's *Cryptographic History of Work on the German Naval
Enigma* (https://discovery.nationalarchives.gov.uk/details/r/C6136631) and HW 25/2 = Mahon's *History
of Hut Eight* (https://discovery.nationalarchives.gov.uk/details/r/C6136632) — the two sources
research/06 already quotes via ellsbury.com. The rest of HW 25 is Colossus/Tunny/bombe material (HW
25/17, 23, 24, 28, 29, 34) and Turing's wartime papers (HW 25/36–38) — nothing about naval key lists or
Grund tables specifically.

**Two extra paragraphs from Mahon (pp. 113–114) not in research/06, fetched directly from
ellsbury.com** (http://www.ellsbury.com/hut8/hut8-113.htm, `-114.htm`):
- The job of cataloguing all recoverable Grunds daily "was undertaken by **Mr. Freeborn's Section**"
  (Frederick Freeborn ran BP's Hollerith/punch-card section).
- **"If the April Grund sheet had remained in force in May, the catalogue would have had to cover
  nearly all the 288 Grunds but as both tables were new on May 1st it would cover only such Grunds as
  could be broken between May 1st and May 5th."** This confirms both the Dolphin/Plaice table and (by
  implication) the Potsdam table were brand-new on 1 May 1945 — exactly the day of our message —
  which matches research/06's DGUG/MNNS-are-Potsdam-cells finding.
- **"...work was stopped when we heard that the Grund tables had been captured... it was certainly more
  by luck than good management that we made a start on all keys in May and broke Dolphin, Plaice and
  Shark right up to the surrender."** Narwhal/Niobe and Thetis are not named in this sentence — Narwhal
  didn't need "starting" (already broken since Sept 1944, per B-Dienst/Wikipedia background), and
  Thetis's absence here is a third, independent data point (after §1.1's missing fish-name and
  Hörenberg's own statement) that it was never broken.

**DEFE 3 series structure, established via the Discovery API's children/parent hierarchy**
(top series: https://discovery.nationalarchives.gov.uk/details/r/C5761, "Admiralty: Operational
Intelligence Centre: Intelligence from Intercepted German, Italian and Japanese Radio Communications,
WWII"; confirmed digitised and **free** to download as "digital microfilm" — see below):

- Sub-series **C42164**, *"Teleprinted translations of decrypted German U-boat traffic, December
  1942 – December 1945"* — the **ZTPGU** series (per sub-series C42151's own scope note: "from December
  1942 U-boat traffic was issued in the ZTPGU series"). This is the series named in the task brief.
  - **DEFE 3/743** — covering **1945 Apr 9 – May 4**, former reference **ZTPGU 38000–38999**,
    digitised, catalogue id C1910640: https://discovery.nationalarchives.gov.uk/details/r/C1910640.
    **This is the top action.** ~1,000 decrypt serials over ~25 days ≈ 38/day; 30 Apr–2 May is a small,
    readable slice of the file.
- Sub-series **C42151**, general (non-Mediterranean, non-U-boat) German naval decrypt series — **ZTPG**
  (without the U/M suffix), likely covering Home Waters/shore-authority traffic, which is where a
  message *sent by* a shore command *to* U-534 might be filed if DEFE 3 files by originator rather than
  addressee.
  - **DEFE 3/577** — covering **1945 Apr 29 – May 3**, former reference **ZTPG 367000–367999**,
    digitised, catalogue id C1910474: https://discovery.nationalarchives.gov.uk/details/r/C1910474.
- Lower priority, checked and set aside: **DEFE 3/685** (parent C42161, "Mediterranean traffic",
  Apr 13–May 6 1945, former ref ZTPGM 102000-102528 — wrong theatre) and **DEFE 3/572** (parent C42158,
  "Main series of signals... continuation of BT series", Apr 30–May 15 1945, former ref KO 1751-2089 —
  finished intelligence summaries to commands, not raw decrypt translations).

**Cost: free.** DEFE 3 is explicitly listed among TNA's "digital microfilm" series that download at no
charge once you register a free Discovery account:
https://www.nationalarchives.gov.uk/help-with-your-research/research-guides/free-online-records-digital-microfilm/.
Expect ~400 MB PDFs of up to ~800 pages each; Adobe Reader recommended. This removes the "cost" variable
from the ranking entirely for DEFE 3 — it is the highest-payoff-per-effort action in this whole report.

**Recommended procedure (someone with hands, not just web access, needs to do this):**
1. **Calibrate first.** Find Bletchley's own translation of an already-broken U-534/Potsdam message from
   1 May 1945 — e.g. P1030690 (Hörenberg's worked example, `FNHC GVET` → `KDUZ/YMUZ`,
   https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+Kenngruppen+System) — inside
   DEFE 3/743 or DEFE 3/577. If found, this confirms which series/piece carries this circuit's traffic
   and anchors the time-of-origin ordering (DEFE 3 pieces are bound in German origin-time order, not
   decrypt order, per TNA's own scope note on C5761).
2. **Then search the same piece(s) around P1030680's time of origin** (radio circuit NB/Niobe,
   ~10:30 on 1 May 1945 per the task brief) for: (a) any note that a message was received but not
   readable/not on a known key (would confirm Thetis was seen-but-unbroken in real time, not just
   absent); (b) any other message from the same originator/time window that might be a full or partial
   crib via shared boilerplate; (c) anything in the accompanying OIC summary volumes (C42158's "KO"
   series, i.e. DEFE 3/571–577) that mentions Niobe, Thetis, or unreadable Baltic training traffic.

---

## 3. Thetis: who held it, M3/M4, reuse — direct answers

- **Who held it.** No document found names a specific flotilla/command. The best evidence is
  structural: RIP 401's Figure 24b (§1.1) places Thetis as the direct continuation of the pre-war
  "U-boat exercise" (U-Übungsschlüssel) cipher inside the **Home Waters** key family, i.e. it was the
  Baltic U-boat *training* organisation's cipher, not a front-line command's. This is consistent with —
  but does not independently confirm beyond — the task brief's own framing (25./26. U-Flottille, Agru
  Front, FdU Ausbildung). I found no primary document explicitly naming any of those units as the
  holder of the Thetis keylist.
- **M3 or M4 in 1945.** Confirmed **M4** directly from the message itself: research/06 already
  establishes the 4-letter Verfahrenkenngruppe+filler / Grund indicator mechanism for P1030680, which is
  the M4 procedure. No separate archival confirmation needed or found beyond that.
- **Did BP or OP-20-G ever recover a Thetis key?** No evidence found that they did. Three independent
  negative signals converge: Hörenberg's own statement that Grund tables don't survive; RIP 401's Figure
  24b giving Thetis no Allied covername (§1.1); Mahon's own list of keys "broken... right up to the
  surrender" (Dolphin, Plaice, Shark) not including Thetis or Narwhal by name (§2) — Narwhal's omission
  is explainable (already broken earlier), Thetis's is not.
- **Documented reuse of inner settings across nets.** Real practice existed (Niobe sliding out of
  Hydra, Triton out of Heimische Gewässer, etc. — §1.1) but Thetis never appears as a parent or child
  in that list, and I found no 1945-specific documentation of Thetis borrowing another net's rotor
  order/ring settings.
- **Hörenberg's own Thetis material, exact URLs** (fetched and checked directly against the live site
  rather than inferred from search-engine summaries):
  - P1030680 page: https://enigma.hoerenberg.com/index.php?cat=Unbroken&page=P1030680
  - Scanned images on that page:
    `https://enigma.hoerenberg.com/kategorien/75_Unbroken/dateien/KBook_ACH_Thetis_WS_k.jpg` (K-Buch
    Gruppenliste), `.../P1030680_VROL_NMKA_WS_k.JPG` (the Schlüsselzettel form),
    `.../Zuteilungsliste_Kenngruppen_k.jpg` (thumbnail of the May 1945 Zuteilungsliste).
  - **Full-resolution version of the Zuteilungsliste photo exists at the same path without the `_k`
    suffix**: `https://enigma.hoerenberg.com/kategorien/75_Unbroken/dateien/Zuteilungsliste_Kenngruppen.jpg`
    (800×1144, confirmed by direct download and inspection). It clearly shows, in the original German
    Fraktur-style table, columns **"621–653 M Thetis (M Tht)"** on U-534's own physical Zuteilungsliste
    — the primary-source confirmation the whole task is built on. The Kennwort (cover word) field at the
    top of the sheet is outside this crop, so I could not directly read whether it says "Hering" (the
    allotment-list name Mahon/RIP401 record as being in force from 4 March 1945, the last change before
    May — see RIP 401 Figure 14, §1.1); it is the best inference but not visually confirmed here.
  - Kenngruppen System worked-example page (P1030690):
    https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+Kenngruppen+System, with
    images `Gruppenliste_Kenngruppenbuch_k.jpg`, `Kenngruppenbuch_k.jpg`,
    `Tauschtafelplan_Bruno_Quelle_k.jpg`, `Tauschtafel_Quelle_A_k.jpg` under
    `.../kategorien/60_The%2520U534%2520messages/dateien/`.
  - Hörenberg's own external links page (https://enigma.hoerenberg.com/index.php?cat=Links) has no
    archival/TICOM/Bletchley links at all — just simulators, Cryptomuseum, cryptocellar, Enigma@Home,
    Schmeh's blog, uboat.net. Confirms he is not sitting on an unpublished archival lead.
- **Hinsley, *British Intelligence in the Second World War*.** Full-text-searched
  **Volume One only** (https://archive.org/details/HarryHinsleyBritishIntelligenceInTheSecondWorldWar,
  full text) —
  **zero hits for "Thetis"**, and zero for "Niobe"/"Narwhal" either, despite 554 hits for
  "Enigma"/"Potsdam" combined (so the search itself works). **Caveat: this is only Volume One** of the
  multi-volume original history; the technical appendices that list individual naval-Enigma key
  covernames are in **Volume 2, Appendix 4 (1981)** and **Volume 3 Part 1, Appendix 3 (1984)**
  (per secondary description at intellit.muskingum.edu), which I did not locate a full-text copy of.
  This is a real, open gap, not a settled negative — if anyone can get Vol 2/3 full text, re-run this
  grep.

---

## 4. Recent AI / amateur activity — the Veritasium challenge (2026-09-21)

This is genuinely new since the last research pass in this repo (23 Sep 2026, i.e. the video is two
days old at time of writing).

- **Video:** Veritasium, *"This Machine Made Wartime Germany (Almost) Impossible to Defeat"*, published
  2026-09-21, https://www.youtube.com/watch?v=JsBZOcqZerk. Confirmed via YouTube's oEmbed API (title,
  author) and the page's own metadata (`publishDate: 2026-09-21T10:49:57-07:00`). Chapter list includes
  "16:45 Trying To Decrypt An Enigma Message" and "41:39 The Naval Enigma". Credits Sir Dermot Turing,
  Bletchley Park, and The National Museum of Computing as consultants.
- **The video's description directly poses P1030680's ciphertext as a challenge**, quoted exactly from
  the video description: *"Can you decrypt this unbroken Naval Enigma message? Let us know in the
  comments! JCRSAJTGSJEYEXYKKZZSHVUOCTRFRCRPFVYPLKPPLGRHVVBBTBRSXSWXGGTYTVKQNGSCHVGF"*. I diffed this
  string character-for-character against this repo's `ciphertext.txt` — **identical, 72/72 characters**.
- **Confirmed as P1030680 specifically (not just a ciphertext coincidence)** via Veritasium's own
  published reference list, reached through the video description's `ve42.co/BletchleyRefs` link, which
  redirects to a public Google Doc
  (https://docs.google.com/document/d/17EH5UUvZeGWnVxB1dUkAIs3IDvxC8tLhmAmO53po45U/, exported as plain
  text via the `/export?format=txt` endpoint). Line 46 of that reference list reads verbatim:
  *"P1030680 - Unbroken Enigma message (U534 - 01. May 1945) via enigma.hoerenberg.com -
  ve42.co/UnbrokenMessage"*. **I checked the full reference list (58 entries) for any mention of
  Thetis, Grund, TICOM, K-Buch or Kenngruppen — none.** Veritasium's own research (credited to Darius
  Garewal & Gabe Strong) did not go past Hörenberg's page either, so nothing here has been preempted;
  if anything, this is now a live, time-sensitive situation worth tracking rather than a solved one.
- **A public attempt already exists and is empty.** A GitHub repo,
  a newly created public GitHub repository ("My attempt to decrypt the Unbroken Naval Enigma message
  JCRSAJ..."), was created 2026-09-22 (one day after the video). Checked via the GitHub API
  (the GitHub API): **repo size 0, zero stars,
  contains only an empty `Readme.md`** (blob hash of the empty file). No actual code or progress — a
  placeholder, not a threat to scoop or a source of leads.
- **Separately, and not about P1030680:** a Schneier on Security post the same week,
  https://www.schneier.com/blog/archives/2026/09/gpt-6-astra-breaks-an-old-enigma-message.html
  (2026-09-22), reports that **GPT-6 Astra** (directed by a named individual) autonomously broke an
  unrelated, already-published unbroken message from Frode Weierud's Crypto Cellar corpus: **Nr. 172
  "MVUEH"**, a 1941 **German Army** (Enigma I, 3-rotor, reflector B) message, using the repeated
  plaintext place-name "ROSENOW ROSENOW" as a crib, after building its own Python/C++ Enigma simulator
  and bombe (43,016 search batches, 60 rotor orders × 107 stepping classes, ~13,824 ciphertext reading
  variants tested, SAT cross-checks). A companion break, **"FMNGI" (Nr. 205/285, 1941)**, was done with
  Claude Opus 5 a couple of days earlier (2026-09-20) using crib "XHARTJENSTEINX". **Neither break
  targets a naval M4 message, U-534, or Thetis** — both are pre-war-style 3-rotor Army traffic with a
  strong known-plaintext crib, i.e. a much easier problem than P1030680's crib-free M4 case. Relevant
  here only as a capability signal (current frontier models can and do run full bombe-style search
  loops end-to-end once *any* crib exists) and as confirmation that PLAN.md's "crib first" prioritisation
  is the right one — these successes are crib-dependent, not evidence that ciphertext-only M4 search
  alone is becoming tractable.
- **Enigma@Home / Stefan Krah's M4 Project.** Per search-engine summaries (I did not find a dedicated,
  datable status page), the Enigma@Home distributed-computing project has had P1030680 as a listed
  unbroken target since around 2018 and has not solved it. Project home:
  http://www.enigmaathome.net/ (linked from Hörenberg's own links page). No indication it has tried a
  Thetis-specific approach or found partial results; treat as a long-running, unsuccessful brute-force
  effort, not a source of new constraints.

---

## 5. Ranked action list

1. **Download DEFE 3/743 (free).** https://discovery.nationalarchives.gov.uk/details/r/C1910640 —
   register a free TNA Discovery account, add to basket, download (~400 MB PDF). ZTPGU U-boat decrypts,
   9 Apr–4 May 1945. Calibrate against P1030690, then read 30 Apr–2 May for anything on the Niobe
   circuit or noting an unreadable Thetis signal. Highest payoff-to-effort ratio in this whole report;
   zero cost.
2. **Download DEFE 3/577 (free), same procedure.**
   https://discovery.nationalarchives.gov.uk/details/r/C1910474 — general ZTPG naval series,
   29 Apr–3 May 1945; covers the case that the message is filed by originator (a shore authority) rather
   than by addressee (U-534).
3. **Get Erskine 2008, "Captured Kriegsmarine Enigma Documents at Bletchley Park," Cryptologia 32:3.**
   Single article purchase or library access (~$40–50 via Taylor & Francis, or check
   ResearchGate/Semantic Scholar for an author-shared copy first:
   https://www.researchgate.net/publication/249565883,
   https://www.semanticscholar.org/paper/8fcfc1e8a670762120a7bac7bec261bb7d0a2bf9). This is the one
   secondary source most likely to say exactly where captured naval key material (if any survives) is
   catalogued at TNA.
4. **Ask NARA's reference desk for R.I.P. 475** (Archives2reference@nara.gov), citing RIP 401 p. 46's
   line "actual contents of the Key Sheets recovered or captured are to be found in R.I.P. 475," across
   both RG 38 and RG 457. Free to ask; this is the most direct documentary pointer to an actual captured
   key-sheet holding found anywhere in this search, and it does not appear in the one RG 38 finding aid
   checked (§1.2).
5. **Order/read R.I.P. 403 ("Special British Reports on German Cryptography") and R.I.P. 604/610
   ("Enigma Series" — Indicator Attacks / Reports from England), NARA RG 38 Box 169–172** (§1.2). Not
   free (NARA reproduction fees or an in-person visit), but a plausible home for exactly the kind of
   Anglo-American technical exchange note that would mention Grund-table captures or Thetis.
   Medium-confidence, medium-cost.
6. **Locate TICOM I-38** (Frowein interrogation, OKM/4 SKL/III, on M4 security) directly on
   ticomarchive.com and read it; also grep the remaining ~350 archive.org "ticom" OCR files not yet
   checked here for `Thetis`/`Grundtafel`/`Übungsschlüssel` if compute time allows — this report checked
   only the 8 most plausible by filename (§1.3).
7. **Try to get Hinsley Vol. 2 Appendix 4 / Vol. 3(1) Appendix 3 full text** and re-run the Thetis/Niobe/
   Narwhal grep — Volume One (checked, §3) does not include these appendices.
8. **Watch, don't chase, the Veritasium fallout.** Check the video's YouTube comments and Klaus Schmeh's
   blog over the next 1–2 weeks for any independent partial progress or archival leads surfaced by its
   ~21M-subscriber audience; low effort, plausible payoff given the sudden attention.
9. **Low priority / likely dead ends, checked and set aside:** DEFE 3/685 (Mediterranean, wrong theatre),
   DEFE 3/572 (finished summaries, not raw decrypts), Bundesarchiv-Militärarchiv RM series (not
   meaningfully searchable without German-archive tooling this session didn't have), Schlüsseleinsatz
   T-1467 (confirmed Wehrmacht/OKW, not naval), the eight TICOM reports in §1.3 (all negative).

---

## 6. Summary

I did not find a surviving Thetis key sheet, Grund table, or wartime decrypt — that negative result,
already stated on Hörenberg's own site, held up under a much wider search across TICOM, NARA RG 38/457,
TNA HW 25, and the secondary literature. What I did find: (1) a rich NARA/TICOM document (R.I.P. 401)
that pins down Thetis's identity and lineage precisely (the Baltic "U-boat exercise" cipher, Home
Waters family, continuous 1941/42→VE Day, never given an Allied covername — a third independent line of
evidence it was never broken) and reproduces an actual captured June 1945 key sheet for a *different*
net (Triton) as a template for what we're looking for; (2) the exact, free-to-download National
Archives piece most likely to contain Bletchley's decrypt stream for this circuit on this date
(DEFE 3/743, ZTPGU series, 9 Apr–4 May 1945); (3) a specific, unresolved documentary pointer to captured
key-sheet material (R.I.P. 475) that a NARA reference request could chase further; and (4) the fact that
this exact message became a public Veritasium challenge two days before this research was done, which I
confirmed by exact ciphertext match and which does not appear to have produced any real progress yet.
The single highest-value next step is free: pull DEFE 3/743 and read it against 1 May 1945.
