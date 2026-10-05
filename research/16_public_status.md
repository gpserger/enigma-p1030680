# Public status check: P1030680 / U-534 / Thetis, post-Veritasium (as of 27 Sep 2026)

**Evidence standard used below:** *Verified* = a claimed key, run through a simulator (ours
or the claimant's, with the claimant's tool inspected), decrypts the full ciphertext to
coherent naval German. *Claim* = someone asserts a result without that check being possible
from public material. *Noise* = unrelated or mis-attributed coverage. Nothing below reaches
"verified" for P1030680 itself — see verdict.

## Confirmed facts about the video

Official description (recovered via a third-party transcript archive,
a third-party transcript archive on GitHub, file
`data/transcripts/Veritasium _ The Insane Real Engineering of the Nazi Enigma Machine [6165cd9d].txt`,
cross-checked against the YouTube oEmbed title):

- Video: **"The Insane Real Engineering of the Nazi Enigma Machine"**, channel Veritasium,
  id `JsBZOcqZerk`. Chapter "16:45 Trying To Decrypt An Enigma Message".
- Verbatim challenge from the description: *"Can you decrypt this unbroken Naval Enigma
  message? Let us know in the comments! `JCRSAJTGSJEYEXYKKZZSHVUOCTRFRCRPFVYPLKPPLGRHVVBBTBRSXSWXGGTYTVKQNGSCHVGF`"*
  — this string is byte-identical to our local `ciphertext.txt`, confirming the video's
  challenge is exactly P1030680.
- Links to Daniel Palloks' "Universal Enigma simulator" for viewers to try it themselves.
- The YouTube comment section was not checked; there is no comment-level evidence either way.
  Treat "no one claimed a solve in the comments" as unverified.

## Findings

| Date | Source | Who | Claim | Assessment |
|---|---|---|---|---|
| checked 27 Sep 2026 | [enigma.hoerenberg.com …cat=Unbroken&page=P1030680](https://enigma.hoerenberg.com/index.php?cat=Unbroken&page=P1030680) | Michael Hörenberg (site owner) | Plaintext still "?"; page unchanged; still credits Enigma@Home | **No claim** — authoritative status-keeper confirms still unbroken |
| checked 27 Sep 2026 | [enigma.hoerenberg.com …cat=Unbroken](https://enigma.hoerenberg.com/index.php?cat=Unbroken) | — | P1030680 is still the *only* entry in the Unbroken category | Confirms nothing was moved to "broken" |
| site last-modified 22.09.2026 18:15 | [enigma.hoerenberg.com …cat=Welcome&page=News](https://enigma.hoerenberg.com/index.php?cat=Welcome&page=News) | Hörenberg, citing Frode **Weierud** | "For the first time in history: AI broke an original Enigma message from WW2" — links to cryptocellar.org | **Verified, but a different message** (MVUEH — see below). Site's news feed has no entry about P1030680 itself |
| repo created 2026‑08‑07, latest push 2026‑09‑26T19:10Z | [github.com/Digital-Defiance/HELUT](https://github.com/Digital-Defiance/HELUT) — real, live repo (confirmed via the GitHub API, 2 stars); `BREAK_P1030680.md` (4,740 lines), `AI_DISCLOSURE.md`, ~50+ P1030680-specific fixtures/logs found via GitHub code search | An independent developer (AI-assisted, per the repository's own disclosure file) + a credited archival contributor "Selm Merel Wenselaers" (self-described historian/curator, Amsterdam/Antwerp) | An extremely extensive campaign (research "Phases" 1 through 71+, predates the video by 6 weeks and continued through/after it) covering: ciphertext-only hill-climbing (Ostwald/Weierud-style), a Welchman-diagonal-board software Bombe with a garble-tolerant variant, a middle/right-ring catalog sweep, dozens of crib families (training/Übung headers, "Regenbogen"/scuttle-order phrases, Kenngruppe/Thetis-register drills, an archivally-sourced crib batch), and archival leads: Kenngruppentafel Prüfnr **1772a** and **2499**, a possible alternate key-net attribution (**Aegir**, via an earlier "Forelle" allocation list mapping column 645, vs. the later list's ACH→Thetis mapping Hörenberg uses), citations of Bauer's *Ultra versus Enigma* and *Enigmafunk*, an NID 1/PW/REP/17 POW interrogation report (does **not** name Thetis), and open archive requests to NARA RG 457, Nasjonalarkivet, Marinemuseet, and others | **No break. Explicitly and repeatedly stated: "P1030680 is not decrypted."** Every documented arm (crib-free climb, all short cribs, catalog ring sweep — parked at fixture-placement 417 of 2,513, not a fraction of 676 as I first misread — garble-tolerant board, indel/splice hypothesis, "Regenbogen" scuttle-phrase family) reports a **clean negative**, not a near-miss. Self-graded: "every negative below is a real negative" (blind controls with known keys do break via the same pipeline). Independently reaches several of the same dead ends we have (Potsdam-key exhaustion, no broken Thetis message anywhere in the corpus, Girard's garble hypothesis) — useful convergent confirmation, not new proof |
| repo created 2026‑09‑25, latest commit 2026‑09‑26T19:39Z | [github.com/levan07082009/ENIGMA](https://github.com/levan07082009/ENIGMA) — real, live repo (confirmed via the GitHub API) | GitHub user levan07082009 | New Rust Enigma I/M3/M4 simulator + ciphertext-only cracker + crib-driven software Bombe, built explicitly around this ciphertext. Exhaustive no-plugboard search (3-rotor and full M4, 336 orders × Greek wheels × reflectors) found "no … setting decrypts it to readable German or English"; with the plugboard climb enabled, top results are "statistics-shaped gibberish" (overfitting on 72 letters). Concludes breaking "realistically needs either a crib or a very large compute budget" | **No break claimed.** `ciphertext.txt` in this repo is byte-identical to ours. Independent confirmation that the no-plugboard search space is exhausted and dead — consistent with our own findings. Useful benchmark: their Bombe does a full M4 search with a 12-letter crib at one position in ~25 min on 4 cores (~17 h for all ~40 positions on one machine) |
| — | [github.com/Digital-Defiance/MuleinLabs](https://github.com/Digital-Defiance/MuleinLabs), path `helut-videos/captures/ep04-p1030680-bombe/` | Same author (companion repo) | Screen-capture/script assets for a "concept video" (episode 4) about their P1030680 Bombe tooling | Explainer material for the HELUT tool, not a research claim; not fetched in full (low priority, no indication it differs from BREAK_P1030680.md) |
| 22–26 Sep 2026 | [schneier.com](https://www.schneier.com/blog/archives/2026/09/gpt-6-astra-breaks-an-old-enigma-message.html), [cryptocellar.org/bgac/the-mvueh-break.html](https://www.cryptocellar.org/bgac/the-mvueh-break.html) (page dated 17/09/2026, verified by direct fetch), Tom's Hardware, Gigazine, Notebookcheck, HN [#49801324](https://news.ycombinator.com/item?id=49801324) | A named individual directed OpenAI's GPT‑6 Astra; break confirmed by Frode **Weierud** (Crypto Cellar) | GPT‑6 Astra autonomously broke **Crypto Cellar Nr. 172, "MVUEH"** — a German **Army** message from **10 July 1941**, unsolved since 2005, using crib "ROSENOW ROSENOW"; separately, Claude Opus 5 broke a second Crypto Cellar message ("FMNGI", 31 Jul 1941) with a different crib | **Verified real break, but categorically NOT our message.** No connection whatsoever to U‑534, Thetis, P1030680, or VROL NMKA in any primary source. This is the one genuinely new "AI cracks WWII Enigma" story this week, and it is easy to mis-skim as being about our message — it is not. Flagging explicitly as the main confusion/noise risk |
| checked 27 Sep 2026, via HN Algolia API | Hacker News, comments under story #49801324 (the MVUEH story) | Various HN commenters | One comment posts our exact ciphertext as a "try this [next]" suggestion; another discusses it being possibly a short/garbled message near the U-boat's sinking, notes the "KKZZ"/"VVBB" repetitions look non-random to humans but aren't necessarily meaningful | **Discussion/speculation only**, no solution attempt or claim. No dedicated 2026 HN story about P1030680 itself exists (the only HN item specifically about it, #36483445, is from June 2023) |
| checked 27 Sep 2026, via GitHub search | GitHub-wide search for `P1030680`, the ciphertext string, "veritasium enigma" | — | Beyond HELUT and levan07082009/ENIGMA, no other repository contains the ciphertext string or `P1030680`, except one incidental README hit (an unrelated repository, not inspected — looked unrelated/low-signal) and the MuleinLabs video-capture repo above. GitHub repository searches for "P1030680" and "veritasium enigma" both returned **zero** results (i.e. it's not in any repo *description*, only in file content) | Confirms the two campaigns above are the only substantive new GitHub activity |
| checked 27 Sep 2026 | Reddit (r/crypto, r/codes, r/enigma, r/veritasium), Crypto Stack Exchange, Enigma@Home/BOINC forums, Twitter/X, Mastodon | — | — | **No hits found via web search.** These sites were not searched directly, so treat this whole row as **unverified**, not "confirmed nothing happened" |
| — | General news coverage of the video | Various tech outlets | Coverage of the video's engineering content and of the unrelated GPT‑6 Astra/MVUEH break | No article found reporting a solution to the video's actual challenge message |

## Verdict

**Not solved.** Six days after the Veritasium video, P1030680 remains unbroken by every
source checked, including Hörenberg's own site — the authoritative status-keeper — which
shows no change. The video's challenge is confirmed word-for-word and ciphertext-for-ciphertext
to be our message. It appears to have energized (or run in parallel with) at least two serious,
independent public efforts: `Digital-Defiance/HELUT`, a large pre-existing (since 7 Aug 2026)
solo campaign that has run dozens of research "phases" of ciphertext-only search, crib-based
Bombe runs, and real archival inquiries, and reports "P1030680 is not decrypted" as of its
latest commit (26 Sep 2026); and `levan07082009/ENIGMA`, a smaller new (created 25 Sep 2026)
Rust tool that independently reaches the same "no-plugboard search is exhausted and dead"
conclusion. Neither claims a break. The one genuinely new, well-verified "AI cracks a WWII
Enigma message" story this week — OpenAI's GPT‑6 Astra (and separately Claude Opus 5) breaking
two 1941 German **Army** messages from Frode Weierud's Crypto Cellar Research site — is
completely unrelated to U-534/Thetis/P1030680 and should not be conflated with it, despite
superficially similar headlines. No new photo, corrected transcription, or Thetis key material
surfaced anywhere; the Reddit/social-media front is a genuine research gap (tooling couldn't
reach it) rather than a confirmed silence.

## Material worth pulling into our own work

- **HELUT's `BREAK_P1030680.md`** (https://github.com/Digital-Defiance/HELUT/blob/main/BREAK_P1030680.md,
  4,740 lines) is an unusually deep independent research log and worth a closer read against
  our own `research/09` (archival), `05`/`11`/`13`/`14` (cribs), and `15` (blind spots). Points
  of possible new value, all unverified/unconfirmed by us:
  - Kenngruppentafel **Prüfnr 1772a** (1 May Tafel A, Tauschtafelplan Bruno) and
    Doppelbuchstabentauschtafeln **Prüfnr 2499** (photographed Tafel A pairs) — specific
    document identifiers we have not cross-checked against our own archival hunt.
  - A possible **alternate key-net name**: an earlier "Forelle" allocation list reportedly
    maps column 645 to **Aegir** rather than Thetis; HELUT treats this as probably reconciled
    by list succession but flags the later list's exact date/codename/provenance as unresolved.
    Worth a sanity check — if the net were actually Aegir rather than Thetis, that changes
    the daily-key search space.
  - Citations to Bauer's *Ultra versus Enigma* (ch. 3) and *Enigmafunk* (ch. 9) for
    midnight/noon key changeover and Kenngruppen procedure — both say "no Thetis key,"
    consistent with our own conclusion, but worth confirming we've seen the same passages.
  - An NID 1/PW/REP/17 (April 1945) POW interrogation report, and named archive requests
    to NARA RG 457, Nasjonalarkivet, Marinemuseet, RM45 III-120 KTB, and U-977 Reel 84 —
    none of these appear in our `research/09`; worth checking whether they're genuinely new
    leads or dead ends HELUT already tried.
  - A crib batch (`site/public/selm/P1030680_JESSICA_CRIB_BATCH_A.csv`) of ~14+ "Tier 1 —
    Direct" candidate cribs sourced from real broken U-534/Potsdam messages (e.g.
    `NORWEGENKARTEN` / Otto Wünsche, `WEITEREBEFEHLE`/`BEFEHLEATWART` from P1030698 FdU
    Ausbildung → TF 24). These substantially **overlap with leads we already tested and
    ruled out** (our PLAN.md Wave 4–5: Otto Wünsche/Norway-charts, P1030698 sister-message
    hypothesis) — mostly confirms we've been looking in the same right place, not a new
    lead, but worth a line-by-line diff in case one or two entries differ from our list.
  - Their own conclusion echoes ours almost exactly: "no broken Thetis message exists in
    the corpus," Potsdam keys decrypt other U-534 traffic but not this message (shown by
    exhaustion), and the garble hypothesis rests on Girard's documented degarbling of the
    sister message.
- **levan07082009/ENIGMA**'s crib-Bombe speed (≈25 min per 12-letter crib per position, 4
  cores) is a clean external benchmark to sanity-check our own `tools/bombe` throughput
  (`research/08`) against.
- No new photo, key, or transcription correction turned up anywhere — the archival and
  public-crowd fronts are both still empty; compute-only and crib-based search remain the
  only live vectors, unchanged from before this check.
