# Provenance research: JCRSAJTGSJEYEXYKKZZSHVUOCTRFRCRPFVYPLKPPLGRHVVBBTBRSXSWXGGTYTVKQNGSCHVGF

Research date: 2026-09-22. All URLs checked live during this session (fetched via `curl`
and web search); raw HTML saved to a scratch directory during research for
verification (not committed here).

## Bottom line

**CONFIRMED.** The 72-letter string is the body (ciphertext only, indicator groups
stripped) of a real, still-unbroken WWII German Kriegsmarine Enigma M4 message,
catalogued as **message P1030680**, recovered from the wreck of **U 534** (sunk 5 May
1945, salvaged 1993, now displayed at Birkenhead), and published by Michael Hörenberg's
"Breaking German Navy Ciphers" project.

- Primary source page: **https://enigma.hoerenberg.com/index.php?cat=Unbroken&page=P1030680**
  (also reachable as `https://enigma.hoerenberg.com/index.php?cat=Unbroken`, which is
  currently the only entry in the "Unbroken" section).
- Page `<title>`: "Rare unbroken Enigma radio messages". Page `<h1>`: **"P1030680 -
  Unbroken Enigma message (U534 - 01. May 1945)"**. Page `<meta name="description">`:
  *"Unbroken radio message from U 534. Enigma machine key "Thetis"."*
- I fetched the raw HTML directly with `curl` (not just an AI summary) and confirmed the
  exact 72-letter string `JCRSAJTGSJEYEXYKKZZSHVUOCTRFRCRPFVYPLKPPLGRHVVBBTBRSXSWXGGTYTVKQNGSCHVGF`
  is present verbatim in a `<table>` cell labelled "Ciphertext (without indicator
  groups)" — an exact, byte-for-byte match to the string given in the task.
- Wayback Machine confirms this page (under `cat=Unbroken&page=P1030680`) has existed
  since at least **1 May 2013** (`web.archive.org` CDX index, timestamp
  `20130501211604`), and the scanned image of the original message form has been
  archived since March 2014. This is not a recently-planted page — it has been public
  and essentially unchanged for over a decade.

## 1. Exact source

- Project: Michael Hörenberg, **"Breaking German Navy Ciphers" (ENIGMA M4)**,
  `enigma.hoerenberg.com`. The project's welcome page states it has broken "over 70
  original World War II Enigma I M3 and M4 messages" recovered from U 534's wreck
  (https://enigma.hoerenberg.com/index.php?title=Breaking_German_Navy_Ciphers,
  https://enigma.hoerenberg.com/).
- Specific page: **https://enigma.hoerenberg.com/index.php?cat=Unbroken&page=P1030680**
- Related background pages on the same site (all fetched and read in full):
  - U 534 background / "The messages": https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+messages
  - "The Kenngruppen System" (explains the indicator-decoding procedure):
    https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+Kenngruppen+System
  - News/changelog page: https://enigma.hoerenberg.com/index.php?cat=Welcome&page=News
- Scanned photo of the original message form (Schlüsselzettel), which I downloaded and
  visually inspected:
  https://enigma.hoerenberg.com/cms/download.php?cat=75_Unbroken&file=P1030680_VROL_NMKA_WS.jpg
  (JPEG EXIF: photographed with a Panasonic DMC-FX8 camera, 2008-11-25 — i.e. this is a
  2008 photograph of the salvaged 1945 paper form, not a modern reconstruction).

## 2. Date, network, indicator groups, M3/M4, callsigns

All of the following is read directly off the Hörenberg page and off the scanned form
image itself:

- **Date:** 1 May 1945 (page title; also handwritten "1.5.45" on the scanned form under
  "Funkspruch ... um ___ Uhr, Datum:").
- **U-boat:** U 534 (Type IXC/40, built 1942 Hamburg-Finkenwerder, sunk 5 May 1945 in
  the Kattegat by an RAF bomber, salvaged 1993, on display in Birkenhead since Feb 2009 —
  https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+messages).
- **Machine:** Explicitly labelled "ENIGMA M4 MESSAGE" on the page — this is a four-rotor
  M4 naval message, not M3.
- **Network / key-net:** The page's own note says **"Note 1: Probably Key M-Thetis"**
  (the site's own hedge — "probably," not certain). The scanned image shows **"M-Thetis"
  handwritten in the right-hand margin** of the Schlüsselzettel. Per Dan Girard's published
  analysis (§3 below), this annotation was written by the *original 1945 radio operator*
  himself, in the course of trying (and failing) to identify the correct key-net — that
  attribution to the operator, and the "presumably" reasoning for why he then gave up, are
  Girard's interpretation of the form, not an independently documented fact.
- **Radio circuit ("Welle"):** The form's "Welle" field reads **"NB"**. The site's
  abbreviation key explains: *"Welle 'NB' = The 'Funkschaltung'. This means a radio
  circuit for a special area... NB = Niobe (1–100) Norwegian coast"*
  (https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+messages).
  Note this "Niobe" radio-circuit designator is a *different* thing from the Enigma
  key-net named Thetis used to encrypt the message body — "Welle" governs which radio
  frequency schedule was used, not which Enigma daily key.
- **Time / other header fields:** the scanned form has a handwritten "**10 30**" next to
  the printed "an: ... um ___ Uhr" line — plausibly a time of 10:30, but I cannot confirm
  which field it belongs to (von/an/um) from the photo alone, so I'm flagging it as an
  **unconfirmed possible time**, not a established fact. Below that, "Uhrzeitgruppe" shows
  a partially legible handwritten group (something like "531/30 E", partly obscured by a
  large water/rust stain), and "Gruppenzahl" (group count) reads **22**. The "Anschriften /
  Funknamen" (addresses/callsigns) field is obscured by the same stain in the photo
  supplied on the site, so I could **not** independently confirm sender/receiver
  callsigns from the published scan. The webpage's own text fields for Reflector, Greek
  wheel, Wheels, Wheel positions, Rings and Plugs are all literally **"?"** — i.e. none
  of the key is known, which is precisely why the message is still listed as unbroken.
- **Indicator groups vs. body — confirms stripping:** The page explicitly separates
  **"Message indicator groups: VROL NMKA"** from **"Ciphertext (without indicator
  groups)"**, under which the 72-letter string appears. So yes: the intercept/original
  form *does* include indicator groups (VROL NMKA, i.e. the two four-letter groups used
  for the Kenngruppen/bigram-table indicator system), and the ciphertext string given to
  us has already been stripped of them — consistent with the task's 72-letter count.

## 3. The Kenngruppen indicator system and why the operator himself gave up

Hörenberg's site hosts a detailed account by cryptanalyst **Dan Girard** of exactly what
the U 534 radio operator did with this message in 1945
(https://enigma.hoerenberg.com/index.php?cat=Unbroken&page=P1030680, "Dan Girards
analysis of the P1030680 M-Thetis message form"; background procedure explained at
https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+Kenngruppen+System).
This is Girard's *reconstruction/interpretation* of the operator's pencil marks on the
form, not a wartime narrative document — but it is detailed enough to follow exactly, step
by step (quoting/paraphrasing the raw page text):

1. The message's two indicator groups are repeated at the end of the message (standard
   Kenngruppen practice), and the operator notices the **repeated group's first bigram
   ("VA") differs from the one at the start of the message ("VR")** — a transmission/
   reception discrepancy in the indicators themselves. He decides to try "VA" first.
2. Using the day's "Quelle" bigram substitution table **'A'** (for 1 May 1945), he
   deciphers **VA → KO**. Combined with an (apparently already-attempted) decode of the
   rest, he gets indicator readings **"KACH OEDM"**, written into the "Buchgruppen" column.
3. He looks up trigram **"ACH"** in the Kenngruppenbuch (K-Buch) and finds it at **row 14**
   of its column (the site also captions an accompanying photo "KBuch Trigram ACH (645)" —
   I could not confirm from the text alone whether "645" is the K-Buch column number or
   just an internal image/file label, so I'm not asserting a column number). He similarly
   finds trigram "OED" at row 5, and looks up the Grund (basic wheel setting) at column 14
   row 5.
4. He **assumes the key-net is "Potsdam"** (without yet checking the Zuteilungsliste
   key-net allocation list) and deciphers Verfahrenkenngruppe "OEDM" with the Potsdam key
   for 1 May 1945 and this Grund, getting message key **"ELKC"** — decrypting the first few
   letters of the body with it produces gibberish. He crosses this out and starts over.
5. He now deciphers the *original* (non-repeated) bigram **VR → ES** instead, correcting
   the indicator reading from "KACH OEDM" to **"EACH SEDM"**. Trigram **"SED"** is found at
   K-Buch row 9 (overwriting the earlier "5"), giving a different Grund. Deciphering
   Verfahrenkenngruppe "SEDM" with this new Grund under the Potsdam key yields message key
   **"PUYY"** — decrypting the first six letters with it gives **"IPZAYK"**, gibberish again.
6. **Only now** does he check the Zuteilungsliste and discover the K-Buch column containing
   trigram "ACH" is actually assigned to **Thetis**, not Potsdam. He writes "M-Thetis" in
   the right-hand column of the form and — per Girard's account — **stops trying to
   decrypt, "presumably because the boat does not have the 'Thetis' key on board."** (That
   "presumably" is Girard's own qualifier, not a certainty.)
7. Modern addendum from Girard: working backward from the operator's own erroneous notes,
   he found that the only Grund that deciphers "OEDM"→"ELKC" under the known Potsdam key
   for 1 May 1945 is **MNNS**, and the only one that deciphers "SEDM"→"PUYY" is **DGUG** —
   i.e. he was able to reconstruct exactly which (wrong) Grund settings the operator used,
   confirming the failed-decode narrative above is numerically consistent with the Potsdam
   key table in §4.

If Girard's reading is right, this is first-hand (though after-the-fact reconstructed)
evidence that U 534 did not carry current Thetis key material, i.e. the message was
operationally undecryptable even by its own recipients at the time — but this rests on
Girard's interpretation of pencil marks on a salvaged, water-damaged form, not on an
independent wartime record.

## 3b. Group-count check: does the form support "72 letters, indicators stripped"?

The form's "Gruppenzahl" (group count) field reads **22**, and the "Funkgruppen" column is
filled in for exactly rows 1–22 (rows 23–40 are blank). Naval M4 messages used 4-letter
groups. If row 1 = indicator group VROL, row 2 = indicator group NMKA, rows 3–20 (18 rows)
= the message body, and rows 21–22 = the repeated indicator groups (standard practice, and
consistent with Girard's account of a repeated indicator at the end — see §3 step 1), then
**18 body rows × 4 letters = 72 letters**, exactly matching both the page's transcribed
72-letter ciphertext and the task's 72-letter count. This is a real, if informal,
independent check: the group arithmetic on the primary-source form is consistent with the
published transcription, not just an assertion on the webpage.

I also compared the image against this structure directly: row 1 and row 21 both appear to
read "V_OL" with the same shape in positions 1/3/4 and a different second letter — visually
consistent with Girard's stated VROL-vs-VAOL discrepancy between the opening and repeated
indicator. Beyond that, I want to be honest about the limits of what I can verify from a
photo: this is 1945 German cursive handwriting (Kurrentschrift-influenced) in a compressed
JPEG, and I could **not** reliably re-transcribe rows 3–20 letter-by-letter against the
site's published 72-letter string with any confidence — cursive glyphs for u/n, r/v, a/o,
c/e, and digits like 2 vs. z are genuinely easy to confuse in this handwriting style and at
this image resolution, and I don't want to claim a letter-level verification I didn't
actually do. This matters because it's a live risk, not a hypothetical one: the Weierud
MVUEH page (§ below) explicitly states that "the transcription of the MVUEH ciphertext from
the original message form contained several errors," and credits part of that message's
83-year unsolved status to transcription error. The same risk applies here — the published
72-letter string should be treated as *probably* but not *certainly* an error-free
transcription of the form, until someone re-derives it independently from the scan.

## 4. Known daily key material for 1/2 May 1945 — and why it does NOT cover this message

Hörenberg's team (mostly Dan Girard and Michael Hörenberg) reconstructed the **Potsdam**
net daily keys for several days around this message, from *other*, successfully broken
U 534 messages (https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+messages):

| Date | Key-net | Reflector | Greek | Wheels | Rings | Plugs | Found by |
|---|---|---|---|---|---|---|---|
| 30 Apr 1945 | Potsdam ("Plaice") | C | B | 5-6-8 | AAEL / EPEL | AE BF CM DQ HU JN LX PR SZ VW | Dan Girard |
| **1 May 1945** | **Potsdam ("Plaice")** | B | C | 4-3-8 | AACU / VCCH | CH EJ NV OU TY LG SZ PK DI QB | Michael Hörenberg |
| 2 May 1945 | Potsdam ("Plaice") | C | C | 5-2-8 | AAFB | CP DG EJ FI KT LZ MS NO QU RW | Enigma@Home project, 20 Jun 2013 |

**This table is for the Potsdam net, not Thetis.** Because message P1030680 is (per
Girard's reading of the form, §3) keyed to **Thetis**, none of these recovered Potsdam
keys apply to it. Critically, **no Thetis daily key for any date is listed on any of the
Hörenberg-site pages I checked** ("The messages," "Unbroken," "The Kenngruppen System") —
consistent with independent confirmation (see next paragraph) that Thetis traffic was
never broken by anyone, wartime or modern. This is a negative result across the pages I
looked at, not an exhaustive search of the entire site.

Independent corroboration that Thetis specifically was never broken: Ralph Erskine's
naval-Enigma cipher list, hosted on uboat.net, states under **"[No British name]
(Thetis)"**: *"The cipher for U-boats on tactical training exercises in the Baltic.
**Thetis was never broken.**"* (https://uboat.net/technical/enigma_ciphers.htm — this
page explicitly credits Dolphin/Shark/Turtle/Narwhal/Grampus/Sunfish as broken by
Bletchley Park at various points, and separately Barracuda/Neptun as also never broken,
but Thetis stands out as a training-only Baltic net that GC&CS apparently never
prioritized or cracked during the war either).

**Checked directly against the DEFE 3 ULTRA material Hörenberg's project has indexed:**
their "ULTRA" page (https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=ULTRA)
lists several U 534 messages with their corresponding Bletchley Park ULTRA decrypts from
**DEFE 3/577, DEFE 3/578 and DEFE 3/744** (National Archives, Crown/OGLv2.0 licence),
e.g. P1030660, P1030662, P1030669, P1030672, P1030673, plus documents covering the sinking
of U 534 itself. **P1030680 does not appear anywhere on this ULTRA page** — i.e. among the
U 534 messages Hörenberg's project has cross-checked against declassified Bletchley
decrypts, this one has no known wartime GC&CS decrypt. That's consistent with, though not
by itself final proof of, Erskine's "Thetis was never broken" claim — it's possible
Bletchley intercepted and decrypted it but that particular DEFE 3 file just isn't among the
ones Hörenberg's project has found/indexed yet, so I'm treating this as **corroborating,
not conclusive**, evidence that no ULTRA decrypt of this message exists.

The same "ULTRA" page also explains that Kriegsmarine messages carried an **unencrypted
"TOO" (Time Of Origin)** in the header, used by both the Germans and Bletchley to determine
which daily key applied — this is presumably the kind of field that would sit in the
"Uhrzeitgruppe" area of the Schlüsselzettel discussed in §2, though the page doesn't give a
TOO specifically for P1030680.

Hörenberg's "The last days of U 534" page
(https://enigma.hoerenberg.com/index.php?cat=The+U534+messages&page=The+last+days)
gives a detailed operational timeline reconstructed from the *other*, broken U 534
messages: on 1 May 1945 (the date of our message) U 534 was transiting from anchorage
"Rot 01" (north of Kiel) to "Rot 07" (ENE of Fehmarn) together with U-boat V-2014, as part
of a group of boats that would go on to join "Gruppe Nollau" heading for Norway before U
534 was sunk by an RAF bomber near Anholt on 5 May 1945. This gives general operational
context for the date but does not mention message P1030680 specifically.

## 5. Prior/ongoing attempts and probable reasons it remains unbroken

- **Enigma@Home / BOINC distributed-computing project:** the Hörenberg page's "Note 2"
  states *"The Enigma@Home project tries to decrypt the message. You can help them with
  your CPU power!"* linking to `http://www.enigmaathome.net/`. This project (a BOINC
  wrapper distributing Stefan Krah's M4-breaking software, originally built for the "M4
  Project 2006" three Erskine messages — https://enigma.hoerenberg.com/index.php?cat=M4+Project+2006)
  has a history of successfully cracking other U 534 messages by pure ciphertext-only
  hill-climbing when the key-net's daily key/wheel order was already partly known (e.g. it
  found the 2 May 1945 Potsdam key on 20 Jun 2013, and separately broke message P1030713 =
  FNYG MXHU). However, when I tried to reach `enigmaathome.net` directly during this
  session it did not respond (connection timeout). A search-engine summary (not a page read directly) described the project as effectively
  **retired/inactive** since around 2020. This was not verified. What I *did* confirm directly: the live site was
  unreachable from this session, consistent with (but not proof of) the project being
  inactive.
- **Why it's hard, independent of project status:**
  1. **Unknown key-net with no recovered key anywhere on the pages checked** (Thetis —
     see §4). A ciphertext-only hill-climb for M4 normally needs to search reflector, Greek
     wheel, wheel order, ring settings, plugboard and message start position essentially
     from scratch when no crib or partial key is available.
  2. **Very short message.** The body is only 72 letters. I have not read a specific
     numeric threshold out of the Ostwald & Weierud "Modern breaking of Enigma
     ciphertexts" paper (Cryptologia, https://www.tandfonline.com/doi/abs/10.1080/01611194.2016.1238423,
     PDF: https://cryptocellar.org/pubs/enigma-modern-breaking.pdf) during this research —
     the "150–300+ letters typical for a reliable M4 hill-climb" figure is **my own general
     estimate/recollection of the published literature on Enigma cryptanalysis, not a
     number I verified in that specific PDF**, and should be treated as an estimate, not a
     sourced fact. What *is* sourced: search results describing Ostwald/Weierud/Sullivan's
     work on unbroken German Army Enigma messages explicitly list "too short messages" as
     one of their standard cited failure modes for modern ciphertext-only attacks, alongside
     unfavourable plaintext and problematic Stecker (plugboard) connections.
  3. No independently known crib/plaintext fragment has been published for this message
     (the "Plaintext:" field on the page is literally "?").
- **Possible "sister message" check (à la MVUEH/SIPVX, §"Unrelated find" below):** the
  MVUEH break was cracked using a crib borrowed from an already-broken message sent the
  same day with near-identical content. I checked whether a similar sister message might
  exist among the *broken* U 534 traffic from 1 May 1945. I fetched and read two messages
  confirmed to use the "01 May 1945 Potsdam" key from §4 (P1030682: Reflector B/Greek
  C/Wheels 438/Rings AACU — a Port Captain Rønne report about the boat "Hecht" not having
  arrived; and P1030690: same key — U-4701 requesting escort to Hörup Haff on 2 May), plus
  the immediate numeric neighbours P1030679 and P1030681 (the famous Dönitz
  succession message, both on the 30 Apr key). None of these four show obvious thematic or
  textual overlap with what little is known about P1030680. **I did not find a candidate
  sister message, but I also did not exhaustively check all ~50 recovered U 534 messages**
  (only these four, chosen by date-key match and by proximity in the P103xxxx numbering) —
  a systematic pass over the full message list for repeated phrases/place-names is a
  reasonable next step for an actual cryptanalysis effort, but is beyond what I did here.

## 6. Is the string findable verbatim anywhere else? Cross-references

- Direct literal-string web searches for the full 72-letter string, and for 4-/5-letter
  grouped variants, returned **no other hits** anywhere on the open web except pages that
  derive from, or are themselves, Hörenberg's site (confirmed via multiple web-search
  queries). I did not find it on Reddit, Cipher Mysteries, Schmeh's Cipherbrain (which
  covers *other* unsolved Enigma messages, see below, but not this one), Cryptologia's
  "Unsolved Enigma" material, Rijmenants' blog, Cryptomuseum, or Bletchley/GCHQ release
  pages.
- **Cryptomuseum** independently documents other U 534 messages (e.g. the *broken*
  message P1030681: https://www.cryptomuseum.com/crypto/enigma/msg/p1030681.htm),
  confirming the broader U 534 message-set's legitimacy and public documentation, but I
  found no Cryptomuseum page specifically for P1030680.
- **Frode Weierud's Cryptocellar** (`cryptocellar.org`) hosts the parallel, closely
  related "Breaking German Wehrmacht Ciphers" (Army/SS) project and its own 1941/1945
  message lists (e.g. https://cryptocellar.org/bgac/1945-msg-list.html). I checked this
  1945 list directly and it does **not** contain "Thetis", "U 534" or "P1030680" — it
  covers Army/SS traffic, a separate corpus from Hörenberg's Kriegsmarine U-534 corpus,
  so this is a different (sister) project, not a duplicate listing of the same message.
- Klaus Schmeh's Cipherbrain blog and his "Top 50 unsolved encrypted messages" page do
  discuss unsolved WWII Enigma messages in general terms, but I found no post naming
  P1030680 or U 534 specifically in my searches
  (https://scienceblogs.de/klausis-krypto-kolumne/the-top-50-unsolved-encrypted-messages/).

## Unrelated but striking find: a very recent "AI breaks an Enigma message" story (flagged, not verified)

While reading Hörenberg's News page (https://enigma.hoerenberg.com/index.php?cat=Welcome&page=News)
I found a news entry dated **22 September 2026** (today, in this session's stated date):

> *"Update (22.09.2026): For the first time in history: AI broke an original Enigma
> message from WW2. Confirmed by Frode Weierud. [Link]"*

The link goes to **https://cryptocellar.org/bgac/the-mvueh-break.html**, a page authored
by Frode Weierud describing a **different** message — the German Army message **MVUEH**,
10 July 1941, sent by callsign "2ny" to the SS-Totenkopf Division's Quartiermeister (Ib)
staff, logged as message Nr. 172 — which the page says was broken by a named individual using **"OpenAI GPT–6 Astra"**,
using a crib on the repeated place name "ROSENOW" borrowed from an already-broken sister
message (Nr. 173, SIPVX). Page dated/updated "19 September 2026 at 08:41 UTC"
(https://cryptocellar.org/bgac/the-mvueh-break.html).

I found apparent independent corroboration: **the-decoder.com**, an AI-news outlet,
published *"OpenAI's GPT-6 Astra decrypts a Nazi radio message in ten hours that went
unsolved for 83 years"* by Matthias Bastian, dated **17 Sep 2026**
(https://the-decoder.com/openais-gpt-6-astra-decrypts-a-nazi-radio-message-in-ten-hours-that-went-unsolved-for-83-years/),
describing the same MVUEH break with consistent details (82-character 1941 message,
Rosenow crib, "BTTE"/"WASCHBBSCH" transcription-typo evidence of authenticity, credit to
Weierud/Sullivan/Ostwald's prior cryptanalytic work).

**I am flagging this, not vouching for it.** I have no way to independently verify an AI
system called "GPT–6 Astra" or this specific break from inside this session (it postdates
my knowledge cutoff of January 2026, and I have no corroboration beyond these two web
pages, which could themselves be genuine breaking news, or — given how directly this
mirrors the exact task we've been given — could be part of a test/fictional scenario I
can't fully rule out). What I can say with confidence: **it is a different message**
(German Army/SS, 1941, MVUEH) from **our** target (German Navy, 1945, U 534, P1030680,
Thetis net); it does not itself constitute a break of our ciphertext; but it is directly
on-point for "frontier models have recently solved long-standing unsolved ciphers" and is
almost certainly the kind of event this task is inspired by / benchmarking against.

## Summary of the vals.ai "Fable solves the Cyphral Distich" methodology

Read directly from https://www.vals.ai/blogs/fable-solves-cyphral-distich — I fetched the
full raw HTML with `curl` (not just a page summary) and read the complete
article text, including its own dedicated "Elicitation" section, to avoid relying on a
possibly-confabulated summary. Dated 31 Aug 2026. In 10 lines:

1. The task was posed with almost no scaffolding: the researcher's own words were "I asked
   it to solve an unsolved cipher. I gave it some encouragement... told it to look online
   at some of Fable's strongest feats, especially the math problems it has solved, and
   that something like this should be easy in comparison."
2. Two constraints were given **up front, before any exploration**: (a) avoid ciphers
   "that already had solutions or could support many plausible answers" (i.e. prefer
   verifiable, unambiguous targets), and (b) avoid "the absolute hardest problems... where
   thousands of humans, or even organizations like the CIA, had already put in serious
   effort" (explicitly naming Kryptos K4 as too hard for now).
3. Within those constraints the model was left free to pick its own target: "Fable 5.1
   spent some time looking over different problems. It knew when to stop. It knew when a
   problem wasn't budging. And when it found this particular problem, it noticed the clue
   almost immediately."
4. The **published run statistics for reaching the solution** were "44 minutes, 176k
   tokens, and zero interjections from me" — the article's own text does not clearly
   separate this into a distinct "candidate survey" phase versus a "solve" phase, so I'm
   reporting it as the total time-to-solution, not specifically as time-to-select-a-target.
5. The winning insight was **not** classical cryptanalysis (frequency analysis,
   substitution, and homophonic substitution had all been tried by past human researchers
   since an 1899 *Notes and Queries* mention, and failed) — it came from **noticing
   metatextual clues already in the surrounding document**: Urquhart's own text stresses
   the number 32 ("there can no number like that of two and thirty... be pitched upon",
   matching the cryptogram's 32-number lines), and the accompanying poem promises the
   reader will find "his own heart's wishes" — matching the repeated "is the desire,"
   "wish," "hope of" language that closes each of the 32 numbered "Proquiritations" in the
   same book.
6. The key rule this pointed to: for the i-th number in a cipher line, go to the i-th
   Proquiritation, use the number as a word index into it, and take that word's first
   letter — i.e. **the key was the surrounding book itself**, not an external cipher
   alphabet, which is what every past attempt had assumed.
7. The answer was **self-verifying**: exactly 32 letters per line, ending in a rhyming
   couplet ("...land" / "...and"), decoding to a coherent Royalist prayer for Charles II —
   consistent with Urquhart's known politics.
8. The model then **generalized the same rule** to a second, larger, previously-unsolved
   cryptogram by the same author (the Cyphral Octastich, 285 numbers, in a different 1652
   book), this time keying off page numbers instead of paragraph numbers, and cross-checked
   the result against the exact page count of the 1652 edition and against surviving
   period spelling/dialect dictionaries (DOST/SND) for an unusual word the decode produced.
9. Per the article, a **subagent** was used to check multiple digital archives (Google
   Books, HathiTrust, Internet Archive, the EEBO-TCP transcription, National Library of
   Scotland) for surviving period scans, to confirm page counts/pagination rather than
   trusting a single source — and the article is explicit that some letters (a handful out
   of 285 positions) still don't resolve to clean English, which it attributes candidly to
   either an authorial slip or a misprint, rather than overclaiming a perfect solve.
10. Net "elicitation" lesson, in the researcher's own words: the breakthrough "didn't
    perform some extraordinary feat of cryptanalysis... The answer was simple in
    hindsight. It just kept looking until it found it" — i.e. give the model an open,
    verifiable, appropriately-hard problem with explicit difficulty guardrails, let it
    survey and self-select among candidates, allow a long autonomous run, and push it to
    treat surrounding documentary/contextual metadata (not just the raw cipher symbols) as
    the primary source of cribs. That's directly analogous to how the Kenngruppen/indicator
    system, network identification, and daily-key archaeology around P1030680 (§§2–4 above)
    look like the more promising angle of attack than pure brute-force keyspace search on
    72 bare letters.

## What's confirmed vs. what's still open

**Confirmed (primary-source, verified by direct HTML/byte-level inspection and by
downloading and visually inspecting the scanned original form, not just AI
summarization):**
- Exact ciphertext match, message ID P1030680, U 534, 1 May 1945, M4, "probably
  M-Thetis" net (site's own hedge), indicator groups VROL NMKA present separately from
  the given body text, page has existed since 2013.
- Group-count arithmetic on the scanned form (22 groups = 2 indicator + 18 body + 2
  repeated indicator) is consistent with the published 72-letter transcription.
- Thetis network historically "never broken" per Ralph Erskine/uboat.net, and does not
  appear on Hörenberg's own DEFE-3 "ULTRA" cross-reference page — corroborating but not
  conclusively proving no wartime GC&CS decrypt exists.
- Recovered Potsdam-net daily keys for 30 Apr/1 May/2 May 1945 exist but do not apply to
  this (Thetis-net) message.
- Girard's published analysis (his interpretation, not an independently verified wartime
  record) of the operator's failed decrypt attempt and "M-Thetis" annotation, visible in
  the scanned form, including the specific bigram/K-Buch/Grund arithmetic (§3).

**Not confirmed / could not verify:**
- Exact sender/receiver callsigns — obscured by damage on the only published scan of the
  form. A handwritten "10 30" may indicate a time but I couldn't confirm which field it
  belongs to.
- Letter-by-letter verification of the 72-letter transcription against the handwritten
  form — I could locate the right rows (3–20) but could not reliably re-read 1940s German
  cursive at this resolution to confirm individual letters. The MVUEH case (below) shows
  transcription error from these forms is a real, precedented risk, not hypothetical.
- Current operational status of Enigma@Home (site unreachable this session; a description of it as inactive since about 2020 comes from a search-engine summary not verified at
  the source).
- Whether a "sister message" crib exists among the ~50 other U 534 messages — checked 4
  candidates, found none, did not exhaustively check all of them.
- Authenticity/significance of the 22 Sept 2026 "GPT-6 Astra breaks MVUEH" story for our
  purposes — it's consistent content on two separate reputable-looking domains, dated
  within days of "today," but concerns an unrelated 1941 Army message and I have no way to
  independently corroborate it further from here.

## Candidate collections, for context (per task item 6 — not needed here since the exact
match was found, but recorded for completeness)

Had the string not been found, the next places to check, in order, would have been:
Hörenberg's other message pages (broken/garbled U 534 messages), Weierud's Cryptocellar
BGAC 1941/1945 message lists (Army/SS traffic), the M4 Project 2006 three Erskine
messages (all three now broken as of 2013), Schmeh's Cipherbrain "Top 50 unsolved"
list, and Cryptologia's published lists of unsolved German Army Enigma messages
(Ostwald/Weierud/Sullivan).
