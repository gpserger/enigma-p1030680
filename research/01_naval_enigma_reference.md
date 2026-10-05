# Kriegsmarine Enigma: Technical Reference for Cryptanalysis

Compiled for implementers building code to attack a genuine naval Enigma intercept.
Covers the machine (wirings, stepping), the key/indicator procedure, message format
and plaintext conventions, and operational constraints that shrink the keyspace.
All facts are sourced; where sources conflicted or coverage was thin, this is flagged
explicitly rather than smoothed over. See §7 for a candid list of what could not be
independently verified.

---

## 1. The machine

### 1.1 Signal path

Keyboard → Plugboard (Steckerbrett) → Entry wheel (ETW) → Rotors (right→middle→left,
i.e. fast→medium→slow) → Reflector (UKW) → Rotors (left→middle→right) → ETW →
Plugboard → Lamp. The plugboard sits *outside* the rotor set, so every letter passes
through it exactly twice (in and out) — this is why plugboard swaps are reciprocal and
why a self-steckered (unplugged) letter is unaffected on both passes.

Military/service Enigma (Enigma I, M3, M4 — i.e. everything the Wehrmacht and
Kriegsmarine used operationally) has a **straight-through entry wheel**:

```
ETW (military): ABCDEFGHIJKLMNOPQRSTUVWXYZ
```

(The commercial Enigma D/K entry wheel is wired in keyboard/QWERTZU order instead —
not relevant to Kriegsmarine service traffic, but easy to get wrong if you copy wiring
tables from a commercial-model simulator by mistake.)

### 1.2 Rotor wirings (letter-for-letter substitution, right-to-left / keyboard-to-reflector direction)

Verified consistently across Wikipedia's "Enigma rotor details" page and two
independent Cipher-Machines-and-Cryptology fetches (cross-checked against my own
training-data knowledge of these tables, which is high-confidence — these values are
extremely well established in the literature):

| Rotor | Wiring (A→Z maps to shown letter) | Used by |
|---|---|---|
| I | `EKMFLGDQVZNTOWYHXUSPAIBRCJ` | Wehrmacht + Kriegsmarine (M3/M4) |
| II | `AJDKSIRUXBLHWTMCQGZNPYFVOE` | Wehrmacht + Kriegsmarine |
| III | `BDFHJLCPRTXVZNYEIWGAKMUSQO` | Wehrmacht + Kriegsmarine |
| IV | `ESOVPZJAYQUIRHXLNFTGKDCMWB` | Wehrmacht + Kriegsmarine |
| V | `VZBRGITYUPSDNHLXAWMJQOFECK` | Wehrmacht + Kriegsmarine |
| VI | `JPGVOUMFYQBENHZRDKASXLICTW` | **Kriegsmarine only** |
| VII | `NZJHGRCXMYSWBOUFAIVLPEKQDT` | **Kriegsmarine only** |
| VIII | `FKQHTLXOCBJSPDZRAMEWNIUYGV` | **Kriegsmarine only** |
| Beta (β) | `LEYJVCNIXWPBQMDRTAKZGFUHOS` | M4 only — 4th-wheel, non-rotating |
| Gamma (γ) | `FSOKANUERHMBTIYCWLQPZXVGJD` | M4 only — 4th-wheel, non-rotating |

Rotors I–V are the same wirings used by Army/Luftwaffe Enigma I. Rotors VI, VII, VIII
were reserved for the Navy (M3 onward) and were issued as physical spares that could be
inserted in any of the three rotating positions alongside I–V. Beta and Gamma are
**thin** rotors specific to the M4 fourth position; they never rotate during
enciphering (see §1.8).

### 1.3 Reflectors (Umkehrwalze, UKW)

| Reflector | Wiring | Notes |
|---|---|---|
| UKW-A | `EJMZALYXVBWFCRQUONTSPIKHGD` | Earliest, largely obsolete by the war; include for completeness, low confidence it saw significant Kriegsmarine wartime use |
| UKW-B | `YRUHQSLDPXNGOKMIEBFZCWVJAT` | Standard wide reflector, thick — the normal M3 reflector |
| UKW-C | `FVPJIAOYEDRZXWGCTKUQSBNMHL` | Alternative wide reflector, thick — used occasionally |
| UKW-B thin (**Bruno**) | `ENKQAUYWJICOPBLMDXZVFTHRGS` | M4 only, thin, paired with Beta as the 4th wheel |
| UKW-C thin (**Caesar**) | `RDOBJNTKVEHMLFCWZAXGYIPSUQ` | M4 only, thin, paired with Gamma as the 4th wheel |

**Important for M4 emulation:** when the Beta/Gamma wheel is set to its 'A' position
and UKW‑B‑thin/UKW‑C‑thin is used, the electrical path through {Beta+B-thin} is
identical to plain UKW-B, and {Gamma+C-thin} identical to plain UKW-C. This is *by
design* — it's how M4 keeps backward compatibility with M3 traffic on the same net,
and it means an M4 message enciphered with the 4th wheel at 'A' is cryptographically a
three-rotor message wearing a fourth wheel for nothing. Don't assume the 4th wheel
position is never 'A' in real traffic.

### 1.4 Notch / turnover positions

The "notch" is a physical feature that causes the **rotor to its left** to step when
this rotor moves past it. The notch letter given below is the letter showing in the
window at the moment the turnover fires (see §1.5 for why this is ring-setting
independent).

| Rotor | Turnover letter(s) (window letter at which the next rotor left steps) |
|---|---|
| I | Q (turnover on Q→R) |
| II | E (turnover on E→F) |
| III | V (turnover on V→W) |
| IV | J (turnover on J→K) |
| V | Z (turnover on Z→A) |
| VI | **Z and M** (double notch — turns over twice per revolution) |
| VII | **Z and M** (double notch) |
| VIII | **Z and M** (double notch) |
| Beta, Gamma | none — these never rotate in normal use |

Rotors VI–VIII having *two* notches each (at Z and M, i.e. 13 positions apart) means
they cause the middle rotor to step roughly twice as often as rotors I–V do. This
matters a lot for period/statistics-based attacks: a message enciphered with VI, VII or
VIII in the right-hand position has a shorter effective run before the middle rotor
turns over, which changes the expected distribution of repeats compared to I–V.

One source (an initial Cryptomuseum fetch) rendered this as a confusing "notch: HU /
turnover: ZM" table with swapped-looking columns; I discarded that rendering in favor
of the Wikipedia table above, which agrees with the Cipher-Machines-and-Cryptology
table and with well-established secondary literature. Treat Z/M as solid.

### 1.5 Ring setting (Ringstellung) — precise semantics

Each rotor is physically two parts: an internal wired **core** and an outer **alphabet
ring** (bearing the letters visible in the window and the physical turnover notch).
The ring setting is the rotational offset between the ring and the core.

Two things that are easy to get backwards when coding this:

1. **The notch is fixed to the ring, not to the core wiring.** Consequently, the
   window letter at which a given rotor triggers the next rotor's step is **the same
   regardless of ring setting** (Q for rotor I, always, whatever the ring setting is).
   What changes with ring setting is *which internal wiring position* corresponds to
   that window letter — i.e., ring setting rotates the substitution alphabet relative
   to the window, but not the window-letter-to-turnover mapping.
2. Ring setting is **not** the same as rotor starting position (Grundstellung /
   message key). Ring setting is part of the *daily key* (fixed for the crypto period);
   starting position is set per-message (or per-period, for the shared Grundstellung —
   see §2.1).

Standard implementation formula (0-indexed, A=0..Z=25), forward pass through a rotor
with wiring table `W`, current window position `p` (rotates during operation), and
ring setting `r` (fixed for the key period):

```
out = (W[(in + p - r) mod 26] - p + r) mod 26
```

and the reverse pass (going back toward the plugboard after the reflector) uses the
inverse permutation of `W` with the same `(p - r)` offset convention. The turnover
check is on `p` (window letter) directly, independent of `r`.

### 1.6 Stepping mechanism and the double-stepping anomaly

Three pawls, one per rotating rotor position, are all engaged on every keypress. A
pawl only succeeds in advancing "its" rotor if it can drop into that rotor's notch (or,
for the rightmost rotor, it steps unconditionally every keypress since nothing gates
it). The anomaly: because the *middle* rotor's pawl sits against the *left* rotor's
teeth as well as being itself steppable by the *right* rotor's notch, if the middle
rotor is sitting at its own notch when the keypress happens, its pawl engages the left
rotor **and** the right rotor's step pushes the middle rotor forward too — so the
middle rotor steps twice in a row (once on the keypress that also advances the left
rotor, and once on the immediately preceding condition). Net effect: the middle rotor
sometimes advances on two consecutive keypresses instead of the "expected" one advance
per 26 keypresses of the right rotor.

Standard, widely-used pseudocode (evaluate before enciphering each character; indices
0=right/fast, 1=middle, 2=left/slow, for a 3-rotor stack — extend trivially for M4 by
leaving the 4th/leftmost thin wheel untouched):

```python
def step():
    if rotor[1].at_notch():          # middle rotor sitting on its own notch
        rotor[1].advance()
        rotor[2].advance()           # left rotor steps too (double-step)
    elif rotor[0].at_notch():        # right rotor sitting on its notch
        rotor[1].advance()
    rotor[0].advance()               # right rotor always steps
```

Cryptographic consequence: a naive 3-rotor odometer would have period 26³ = 17,576
before the substitution sequence repeats. Because of double-stepping, the actual
period is **25 × 26 × 26 = 16,900** — the middle rotor effectively "loses" one of its
26 positions from the full cycle (well-established figure; I'm confident in it, though
I did not find a single primary source deriving it during this pass — it follows
directly from the stepping logic above and is widely reported).

For M4: the fourth (leftmost, thin Beta/Gamma) wheel is **excluded** from stepping
entirely — it is manually set and stays fixed for the whole message. Only the three
rotating wheels step as in M3.

### 1.7 Plugboard (Steckerbrett)

A set of reciprocal letter-pair swaps applied once before the rotors and once after (on
the way back), i.e. involution applied twice in series with the rest of the circuit
sandwiched in between. Kriegsmarine standard from January 1940 onward: **10 plugged
pairs** (20 of 26 letters steckered, 6 self-steckered) — see §4.4.

### 1.8 M3 vs M4

- **M3**: three rotating rotors chosen from I–VIII (Navy) or I–V (Army/Luftwaffe),
  thick reflector B or C. This is the machine used for all naval traffic before 1 Feb
  1942, and continued to be used after that date by networks that were *not*
  Triton/Shark (see §5).
- **M4**: adds a fourth, non-rotating "Zusatzwalze" position between the leftmost
  rotating rotor and the reflector, occupied by a thin Beta or Gamma wheel, paired with
  a correspondingly thin reflector (UKW-B-thin/Bruno with Beta; UKW-C-thin/Caesar with
  Gamma — see §1.3 for the pairing convention and A-position backward compatibility).
  Physically the M4 also has a removable top lid, separate lamp panel, single rotor-cover
  lock (M3 had two), longer plugboard pins (~4mm longer), and a 4V DC power socket for
  submarine mains power.
- **Introduction**: M4 entered U-boat service on **1 February 1942** for the
  Atlantic/Mediterranean U-boat network **Triton** (British codename **Shark**),
  producing a roughly nine-to-ten-month Bletchley Park blackout on that traffic (not
  fully broken again until captured Wetterkurzschlüssel/short-signal material in
  Dec 1942 enabled cribs). Some sources (Wikipedia's Enigma-M4 article) date M4's
  *first* operational use earlier, October 1941, on the **Neptun** key network for
  surface warships — i.e., M4 reached surface units about four months before it
  reached U-boats. I could not fully reconcile whether this is "M4 in general use from
  Oct 1941" vs. "M4 rolled out network-by-network with U-boats last"; both framings
  appear in the sources and are not necessarily in conflict.
- **Networks that stayed on M3 throughout**: **Hydra/Dolphin** (home waters, the
  principal three-rotor net used by U-boats in training/home waters and surface units),
  **Medusa/Turtle** (Mediterranean U-boats, from June 1943), **Aegir** (overseas
  surface raiders/auxiliary cruisers). See the network table in §5.

### 1.9 Keyspace (order of magnitude, for sanity-checking a search)

Per the Wikipedia Enigma-M4 article's figures (order-of-magnitude, not independently
re-derived here):

- Rotor selection (M3, Navy: 3 of 8 rotors, ordered): 8×7×6 = 336
- Ring settings (3 rotating rotors): 26³ = 17,576
- Starting positions (Grundstellung, 3 rotating rotors): 26³ = 17,576 (but see §4.6 —
  for the non-rotating 4th wheel on M4, ring setting and starting position are *not*
  independently observable, only their difference matters, so M4 doesn't multiply this
  by another 676)
- Plugboard, 10 pairs from 26 letters: 150,738,274,937,250 (~1.5×10¹⁴)
- **M3 total**: order 10²³ (≈77 bits equivalent)
- **M4 total**: order 3×10²⁵ (≈84 bits equivalent) — roughly 291× M3's keyspace, driven
  mostly by the extra rotor choice (Beta/Gamma × effectively 26 usable positions) and
  the wider rotor pool, *not* by an additional 676-fold ring/position multiplier.

---

## 2. Naval key procedure

### 2.1 The daily/period key (Schlüsseltafel M)

The Kriegsmarine key sheet had **two logically separate parts issued on different
schedules** — this split is itself a keyspace-reducing constraint, see §4.2:

- **"Innere Einstellung" (internal settings)** — Walzenlage (rotor choice and order),
  Ringstellung (ring settings), and (on M4) the Beta/Gamma wheel and thin-reflector
  choice. Historically documented as changing only **every two days**, and as being
  the officers' responsibility (vs. the outer settings, which cipher clerks handled).
  One source frames this as "provided only for odd-numbered days" of the key sheet.
- **"Äussere Einstellung" (external settings)** — Steckerverbindungen (plugboard pairs)
  and Grundstellung (basic rotor start position used to encipher the indicator), issued
  **daily**.

Additional key variants existed layered on top of the general (Allgemein) key:
**Offizier** keys (monthly officer-only keylists, for a superencipherment step — see
§2.6) and a specially restricted **Schlüssel M NIXE** for flag-officer/command-only
traffic. Exact NIXE mechanics were not found in the sources consulted here.

### 2.2 The Kenngruppenbuch (K-Book, M.Dv.Nr. 98)

The K-Book was the naval indicator-trigram directory, introduced **1 May 1937**. Its
structure (per the Discriminant Book / Erskine literature):

- **Spaltenliste (column list)**: all 17,576 three-letter trigrams (Kenngruppen), in
  randomized order, arranged into 733 numbered columns of 24 trigrams each.
- **Gruppenliste (group list)**: the same trigrams listed alphabetically, each with a
  pointer (column number + position within column) back into the Spaltenliste — lets
  an operator go trigram→location as well as location→trigram.
- **Zuteilungsliste (assignment list)**: maps cipher-net names to the specific
  table/column ranges valid for that net.
- **Tauschtafelplan (table-selection plan)**: a calendar-driven pointer telling the
  operator which column/table applies on a given date.

Two trigrams were drawn per message (from ranges specified by the Zuteilungsliste for
that net/date):

- **Schlüsselkenngruppe** ("Cipher Indicator Group" in the primary-source English
  translation): identifies *which key/cipher* applies — e.g. distinguishes Home Waters
  vs. Non-Home-Waters vs. a Special Ship Cipher. This trigram is **not** enciphered
  through the Enigma; it's a lookup/identification value.
- **Verfahrenkenngruppe** ("Procedure Indicator Group"): this trigram *is* run through
  the Enigma, set to the shared daily Grundstellung, to derive the actual message key
  (the "Message Cipher" — the three/four letters the operator then sets on the rotors
  to begin enciphering the real message text).

**Security rule stated explicitly in the primary-source manual**: a different pair of
Kenngruppen must be used for every message — reusing an indicator pair was considered
a serious security lapse.

### 2.3 Bigram tables (Doppelbuchstabentauschtafeln) and the indicator substitution

Before transmission, the two Kenngruppen trigrams (6 letters) underwent an *additional*
reciprocal substitution keyed by one of several named bigram tables, changed
periodically. Sources here give overlapping but not identical name lists — treat the
set as "roughly nine tables, rotated by calendar/schedule," names attested across
sources include **Quelle, Meer (two different serials), Flußlauf/Fluss(z), Bach, Strom,
Teich, Ufer**. Physically these tables were printed in red water-soluble ink on pink
blotting-type paper specifically so they could be destroyed by dunking in water if
capture was imminent (a detail corroborated for both the bigram tables and the
Kurzsignale/weather codebooks).

**Mechanics (corroborated by two independent secondary sources describing the same
process with matching structure, worked with different example letters):**

1. Take the two trigrams. Prepend one random "dummy" letter to the first trigram and
   append one random dummy letter to the second, producing two 4-letter rows, e.g.:
   ```
   row1 = [random] + trigram1     e.g. A H L G
   row2 = trigram2 + [random]     e.g. K Q K Z
   ```
2. Read the two rows as a 2×4 grid and take the four **vertical** letter pairs
   (bigrams): column 1 = (row1[0], row2[0]), column 2 = (row1[1], row2[1]), etc.
   Example: A+K, H+Q, L+K, G+Z.
3. Look each bigram up in the current bigram table (a reciprocal/self-inverse
   substitution over letter-pairs) to get a substituted output bigram.
   Example (Table "B" in the worked source): AK→BD, HQ→BJ, LK→EM, GZ→EJ.
4. Concatenate the four output bigrams into two 4-letter groups — these are the
   **transmitted indicator groups**. Example result: `BDBJ EMEJ`.

A second, independently-worded source (summarizing the Discriminant Book / Erskine
material) describes the identical row/column/vertical-bigram/table-substitution
mechanic with different sample letters (`XSWQ`/`RAFP` → `VIGF VTXT`), which gives me
reasonable confidence the mechanism itself (as opposed to the specific table contents)
is right. I was not able to recover the actual contents of any specific bigram table
(e.g., the real substitution table for "Quelle" or "Bach") from the sources fetched in
this pass — only the *mechanism* by which they're applied. **If you need the literal
table values (not just the mechanism) for a specific date/table name, that requires
further targeted research** (Erskine's "Naval Enigma: The Breaking of Heimisch and
Triton" and the Bletchley Park/Ralph Erskine papers on cryptocellar.org are the leads
to chase next).

### 2.4 Radio Indicator Groups — placement in the transmitted message

Per the primary-source 1940 procedure manual (M.Dv. 32/1, "Der Schlüssel M Verfahren M
Allgemein" — see §6 for the translated-page URLs):

- The two indicator groups (from §2.3) are transmitted **twice**: once as the
  **first and second** radio groups of the message ("Beginning Indicator Groups"),
  and again, in the same order, as the **next-to-last and last** radio groups of the
  message ("End Indicator Groups"). So every naval Enigma transmission, in full, is:
  `[Indicator1][Indicator2] [ciphertext groups...] [Indicator1][Indicator2]`.
- The **Message Cipher** (the enciphered Verfahrenkenngruppe, i.e. the actual rotor
  starting position for the message body) is derived by setting the rotors to the
  daily Grundstellung and typing the Verfahrenkenngruppe trigram; the three lit letters
  are the Message Cipher, which the operator then dials in (left-to-right) before
  enciphering the message text. Note the Message Cipher itself is *not* transmitted in
  clear anywhere — only its doubly-enciphered-and-bigram-substituted form (the
  indicator groups) is sent; the recipient reverses the whole process using their own
  copy of the K-Book, bigram table and daily key to recover it.

### 2.5 Body text grouping

Kriegsmarine Enigma plaintext, once prepared (see §3.3), was divided into
**four-letter groups** for enciphering and transmission. This is corroborated by the
primary-source manual ("divided up by letters into four-letter groups") and by a
search-synthesized comparison explicitly contrasting it with Army/Luftwaffe, which
used **five-letter groups**. Treat 4-letter grouping as the reliable rule for
Kriegsmarine general-procedure traffic; I could not get clean verbatim confirmation of
group size from one specific real-message example I tried to transcribe (a 1944
Hydra/Dolphin message from cryptocellar.org — my fetch tool garbled the transcription
across two attempts and gave inconsistent group counts each time, so I am explicitly
**not** treating that specific transcription as reliable; the 4-vs-5-letter rule above
rests on the primary-source manual instead, which I trust more).

If the last group is short, it's padded — **with consonants only**; padding with
procedure signs, standard abbreviations, or punctuation marks is explicitly forbidden
by the manual (this matters for crib placement: don't expect punctuation-like padding
at message ends).

For long messages split across multiple transmissions, the word **FORT** ("continued")
is inserted somewhere in the final third of every part except the last, and the
following part opens (in its first third) with FORT plus the time-group of the
preceding part it continues from.

### 2.6 Offizier / Stab superencipherment

Officer- and staff-only messages were enciphered **twice**: first under the
monthly Offizier (or Stab) key — described elsewhere as using plugboard connections
from a separate officer keylist plus a chosen indicator letter — and then the
*entire already-enciphered output*, together with urgency signs, address, signature,
and the literal word "Offizier" or "Stab" plus the (spelled-out) indicator letter, was
enciphered a **second time** under the normal Allgemein (general) daily key, exactly as
an ordinary message would be. The manual is explicit that sending Offizier/Stab
material without this second, general-procedure encryption was "absolutely forbidden."
For a cryptanalyst this means: Offizier traffic looks, at the outer layer, like
ordinary Allgemein-key traffic (same indicator mechanism, same K-book), but the
recovered "plaintext" is itself still Enigma ciphertext under a different, harder-to-
reach key. I did not find the exact Offizier-key mechanics (how the officer-only
plugboard/indicator-letter step works) in the sources fetched here; the manual
explicitly punts to a separate document (M.Dv. 32/2) I did not fetch.

### 2.7 Kurzsignale (short signal book)

Purpose: minimize transmission time (to defeat direction-finding) for routine tactical
messages (contact reports, course, weather, rendezvous, etc.) by converting whole
phrases to codes *before* Enigma encryption, rather than sending full German text.
Captured intact from **U-110 (9 May 1941)**, compromising it early; codebooks were
printed on pink paper in water-soluble red ink for emergency destruction, same as the
bigram tables.

**Three-stage conversion pipeline** (per the worked example found):

1. **Satzbuch (sentence book)**: converts a stock phrase/sentence into a four-digit
   number. Example: "convoy of 16–20 steamers" → `0516`.
2. **Schlüsselzahltafel (key-number table)**: a secret additive key number is added
   (non-carrying digit addition, i.e. modulo-10 per digit, no carry) to each four-digit
   group. Example additive `0384` applied to the digit groups from the sentence,
   quadrat/grid reference, and any numeric fields.
3. **Buchgruppenheft**: converts the resulting (keyed) digit groups into four-letter
   groups (e.g., the example's `0890 → ZLDP`, `4829 → OYAK`, `8436 → WIKW`).

The resulting four-letter groups — plus a three-letter callsign/signature group (e.g.
`LQX` for "U-999" in the worked example) — are what actually get put through the
**Enigma** using the normal Spruchschlüssel (message-key) mechanism, so a Kurzsignal is
still an ordinary-looking Enigma-enciphered transmission at the radio level; it's just
that its *plaintext* (before Enigma) is highly compressed non-German-looking four-
letter groups, not prose. Format markers described: an unencrypted introduction signal
("ββ"), an unencrypted Kenngruppe trigram identifying the key/procedure, the encrypted
signal groups, an encrypted signature, then the Kenngruppe repeated unencrypted. A
full transmission of this kind reportedly took about 20 seconds to send in Morse.

Total codebook/keylist load for the whole system: Satzbuch + Schlüsselzahltafel +
Buchgruppenheft (all in the Kurzsignalheft), plus the Marinefunknamenliste (radio
callsign list), plus two Schlüsseltafel M sheets, plus the Kenngruppenheft — "7 tables
or key sheets" in total per one source.

### 2.8 Wetterkurzschlüssel (short weather cipher)

A single-letter polyphonic substitution code (26 letters, X excluded) condensing a
full weather observation (air temperature, water temperature, pressure, humidity, wind
direction/force, visibility, cloud cover, swell direction/type, latitude, longitude)
into roughly a 7-letter group, resolved unambiguously using context (the U-boat's
known approximate position/time) since the same letter is deliberately reused for
different values in different parameter slots (e.g. "S" can mean 10°C or −15°C
depending on which field it's in). As with Kurzsignale, this compressed group was then
put through ordinary naval Enigma encryption using the standard indicator mechanism —
sources describe "standard Enigma indicators applied without modification," i.e. I
found no evidence of a materially different key/indicator scheme for weather shorts
versus ordinary Kurzsignale, beyond using a different (weather-specific) pre-Enigma
codebook. Captured intact from **U-559 (30 October 1942)**; this capture, combined with
the Kurzsignalheft, gave Bletchley Park the cribs that ended the Shark blackout in
December 1942.

Elsewhere a different figure is given — "23 or 24 letters" for weather short signals
"compressed from thirteen tables" — which does not obviously match the "7-letter
group" figure above. I suspect these describe two different things (a compact
"Wetterkurzschlüssel" proper vs. a longer combined weather short-signal transmission
format) but I could not reconcile them from the sources fetched in this pass. Flag this
as unresolved; don't rely on a specific fixed group length for weather shorts without
checking a primary source.

### 2.9 Grundstellung sharing — the key structural fact for attack

Unlike Army/Luftwaffe procedure (where, especially pre-1940, each operator picked
their own random Grundstellung per message), **the Kriegsmarine Grundstellung came
from the key sheet itself and was shared by every message enciphered under that
key/period** (daily, per §2.1). This is explicitly stated by the Tony Sale lecture
source. Combined with the "different indicator pair for every message" rule (§2.2),
this is exactly the structure that made bigram-table recovery (and, historically,
Banburismus) possible: many different messages on the same day encipher their
(different) Verfahrenkenngruppe trigrams starting from the *same* rotor position,
producing indicator-pair ciphertexts that are all permutations related through that one
shared starting state.

---

## 3. Message format as intercepted

### 3.1 Preamble / header

A naval Enigma radio message, as intercepted, has (per the primary-source manual and
a real example transcribed from cryptocellar.org) roughly: frequency, interception/
transmission time and date, a traffic-type marker, addressee/originator callsigns (from
the Funknamenliste/Marinefunknamenliste radio name list — see §3.3 for how these are
embedded), a serial or "time group," and a **group count**, followed by the body in
four-letter groups (§2.5). A real example (Hydra/Dolphin net, 11 Apr 1944, per
cryptocellar.org — treat the exact field layout below as illustrative, sourced from a
tool-summarized fetch rather than a verified verbatim transcription):

```
Frequency: 2308 kHz
Intercept time: 2023B / 11 Apr 1944
Key: Hydra (Dolphin)
Setting/indicator: 1 2 9 / O U K
Wheel setting: 0125 / 11 / 773
```
decrypting (per that same source) to a convoy-sighting report in grid square 8237
(≈71.27°N 7.10°E), signed by the Arctic U-boat command. I was not able to get a clean,
self-consistent letter-by-letter transcription of the actual cipher/plaintext groups
for this message from the fetch tool (two attempts gave different, garbled groupings —
see §3.4/§2.5) — **go to the primary page directly** (URL in §6) rather than trust my
transcription of the letters.

### 3.2 Addresses and signatures

Per the primary-source manual: addresses/signatures are normally taken from the Radio
Name List, prefixed with a stress-mark letter and inserted **after the time group or
after the last radio group** — i.e., they sit **outside** the enciphered part of the
message in the "General Procedure." (Fleet/acknowledgment traffic was an exception:
there, the address went at the very start and *was* enciphered with the rest of the
text, separated from a following address by "VON.") To vary traffic patterns and deny
cryptanalysts predictable structure, operators were told to alternate the order of
address/signature (using "AN"/"VON") and to vary abbreviation style message to
message — this is stated as a deliberate anti-cryptanalysis measure in the manual
itself, i.e. the Germans were consciously trying to defeat exactly the kind of
positional/crib attack a modern implementer would first reach for.

### 3.3 Plaintext conventions (primary-source-verified, M.Dv. 32/1)

Punctuation and symbol substitutions:

| Plaintext sign | Meaning |
|---|---|
| `X` | period |
| `XX` | colon |
| `Y` | comma |
| `YY` | dash / slant / hyphen; also used to separate consecutive numbers where German grammar would otherwise use a comma |
| `UD` | question mark |
| `KK ... KK` | parenthesis (opens and closes) |
| `J ... J` | stress mark — brackets an emphasized word, e.g. a proper name: `J CAPGRISNEZ J J CAPGRISNEZ J` |
| `QU` | "square" (as in a grid square) |
| `GRQU` | "large square" |
| `FBKT` | fixed point |
| `SM` | nautical miles / knots |

Letters and spelling:

- `CH` is written as `Q` (e.g. ACHT→AQT, RICHTUNG→RIQTUNG).
- Umlauts ä/ö/ü are written as plain A/O/U.
- Numbers are spelled out letter-by-letter: `NULL EINZ ZWO DREI VIER FUNF SEQS SIEBEN
  AQT NEUN` (note EINZ not EINS, and SEQS reflecting the CH→Q rule on SECHS). Certain
  round numbers form single spelled words (ZEHN, ELF, ZWOELF, ZWANZIG, DREISSIG,
  HUNDERT, TAUSEND, etc.).
- Repeating `NULL` several times in a row was **explicitly prohibited** (presumably to
  avoid a giveaway stereotyped crib); instead operators used **CENTA** for 00,
  **MILLE** for 000, **MYRIA** for 0000.
- Foreign/ambiguous place or ship names are delimited on both sides by a lone `X`, e.g.
  `XPARISXPARISX` (note the name is doubled inside the X-delimiters in that example).
- Where two service-station designations could be confused (e.g. similar abbreviations,
  or ship names that look alike once abbreviated), the distinguishing letter is
  **tripled**: `BDUUU` for B.d.U. (Befehlshaber der Unterseeboote), `FDTTT` for F.d.T.,
  and a boat number like U-18 is spelled `UUU EINS ACHT` (U tripled, then the digits
  spelled individually). **This is the documented meaning of a tripled letter like
  "UUU" in naval Enigma plaintext — a disambiguation marker on an abbreviation/callsign,
  not a fixed "submarine sighted" alarm code.** I searched specifically for a
  submarine-sighted-alarm meaning of "UUU" (as the task brief suggested might exist)
  and could not corroborate that reading from any source; treat the disambiguation-
  triple explanation above as the verified one and the "alarm signal" reading as
  unconfirmed/likely not applicable to Kriegsmarine Enigma plaintext specifically.
- Urgency: urgency signs go before the address/signature and are enciphered along with
  them. The code words **BIENE, WESPE, MUECKE** could substitute for the literal
  urgency marking "SSD" and are converted back to "SSD" on decipherment — another
  deliberate variation-for-security measure.
- General abbreviation policy: standard German/naval abbreviations are allowed (drop
  final syllables/letters, drop periods after abbreviations when unambiguous), but the
  manual explicitly **forbids uniform/systematic abbreviation** — operators are told to
  vary how they abbreviate specifically so an adversary cannot exploit predictable
  patterns. This is a real, stated design-level obstacle to crib-dragging, not
  incidental noise.

I did not find, in the sources fetched during this pass, an authoritative, explicit
definition of `FUEHRER` or `STANDORT`/`WETTER` as *procedure signs* with fixed codified
meanings distinct from their ordinary German dictionary meanings ("Führer" as a title/
name; "Standort" = position/location; "Wetter" = weather). My assessment: these are
very likely to appear as **ordinary German content words** within message bodies about
positions and weather (not, e.g., transmitted in the compressed Kurzsignale/
Wetterkurzschlüssel forms, which specifically exist to *avoid* spelling these words
out — see §2.7–2.8), rather than as defined abbreviations/procedure signs on the level
of X/YY/QU above. Treat this as a plausible inference, not a verified fact — worth a
further, more targeted research pass on real decrypted message transcripts (e.g. more
of the enigma.hoerenberg.com U-534 message pages, which I did not get to examine in
detail — see §7).

### 3.4 The naval grid (Quadrat) system

The Kriegsmarine covered the globe with a two-letter "sector," itself subdivided 3×3
into a two-digit "Grid," itself subdivided 3×3 into a further two-digit sub-square —
giving a 6-character reference (two letters + four digits, e.g. `AK 9721`) that pins
down a position with reasonable precision using very few transmitted characters. This
grid reference is the `QU`/`GRQU`/`QUADRAT` content referred to in §3.3's text-aid
table and appears spelled out letter-by-letter in plaintext per the normal number- and
letter-spelling rules (§3.3) — e.g. the worked cryptocellar example's grid square 8237
appearing in the plaintext as something like `...ZWO DREI SIEBEN...` style spelled
digits. On 11 September 1941 the Germans began additionally re-enciphering grid
coordinates with a **separate substitution layer on top of Enigma**, specifically to
deny Allied analysts an easy crib on the very regular, very guessable QUADRAT+digits
pattern — worth knowing if the intercept you're attacking post-dates that.

---

## 4. Keyspace-reducing operational constraints

Concrete, attack-relevant constraints, gathered from multiple sources:

### 4.1 Rotor selection pool and count

- **Navy (M3/M4)**: 3 rotors chosen, ordered, from **8** (I–VIII): 8×7×6 = **336**
  wheel orders. M4 adds the independent choice of Beta or Gamma for the 4th position
  (2-way), and correspondingly UKW-B-thin or UKW-C-thin.
- **Army/Luftwaffe** (context/comparison only): 3 chosen from **5** (I–V) early war:
  5×4×3 = **60** wheel orders; from July 1942 the Army additionally rotated which
  physical wheel occupied which slot every 8 hours during the day using a fixed
  permutation cycle (e.g. daily order I,IV,III → after 8h III,I,IV → after 8h IV,III,I)
  — a mechanism I found described only for Army, not attested for Navy.

### 4.2 Rotation schedule: two-day inner key vs daily outer key (Navy-specific)

As in §2.1: wheel order + ring settings (+ M4 4th-wheel/reflector choice) changed only
every **two days** and were an officer responsibility; plugboard + Grundstellung
changed **daily** and were a cipher-clerk responsibility. Practical consequence for an
attacker: if you can pin down the wheel order/ring settings for one day, you get the
*next* day free (or vice versa, whichever of the pair you crack first) — this halves
the effective work for that part of the key across any two-day span.

### 4.3 Army "no repeat rotor position on consecutive days" rule (context, not Navy-verified)

Well documented for **Army/Luftwaffe**: no rotor was allowed to occupy the same
physical slot (left/middle/right) on two successive days. Knowing yesterday's wheel
order eliminates a large fraction of today's candidates — the commonly cited figure is
that this rule cuts the 60 Army wheel orders down to 32 candidates for the next day
given the previous day's order. **I could not find a source confirming or denying
whether an equivalent rule applied to the Kriegsmarine's two-day inner-key rotation**
(§4.2). Given the Navy's inner key is explicitly documented as *unchanged* across its
own two-day pair (not merely "different from yesterday" but literally identical for
both days of the pair), the Army rule as stated doesn't directly transfer — but whether
there's an analogous "don't repeat the *previous pair's* wheel order/positions" rule
governing the transition between one two-day period and the next is not established
here. Flag as open; worth checking Erskine's "Naval Enigma" papers directly.

### 4.4 Stecker (plugboard) pair count

Standard service-wide from **January 1940** onward: **10 plugged pairs** (20 of 26
letters steckered). Before that (1936–39), 6–8 pairs was standard. One source notes a
few keys continued using fewer than 10 for unexplained reasons — don't hard-code
"always exactly 10" as an unbreakable assumption, but treat it as the strong default
for any Kriegsmarine traffic from 1940 onward, which covers essentially the entire
naval Enigma war (and certainly M4/Triton traffic, which postdates Feb 1942).

### 4.5 Reflector choice is fixed per radio network

Per the "Naval Enigma" technical essay (macs.hw.ac.uk): "Radio nets always used the
same reflector, as the use of different reflectors created logistical, procedural and
practical problems." If you've identified which net an intercept belongs to (from
callsigns/context), you can treat the reflector as a **known constant**, not a search
variable — this is a significant keyspace reduction in practice (it removes the B-vs-C,
or B-thin-vs-C-thin, choice entirely once the net is known), even though in the
abstract "the choice between B and C reflector would only double the key space" per the
same source.

### 4.6 M4's fourth wheel: ring setting and start position collapse to one degree of freedom

Because the Beta/Gamma wheel never rotates during a message, its ring setting and its
starting position are **not separately observable** from the ciphertext — only their
relative offset matters. So while formally there are 26 ring settings × 26 positions =
676 combinations, cryptanalytically there are only **26 distinguishable configurations**
of the 4th wheel for a given message. This is why M4's keyspace is described as "336×
larger" or "a factor of 26 harder" rather than a further 676× — see §1.9. Concretely:
if you're doing a bombe-style or brute-force search over the 4th wheel, you only need
to try 26 values, not 676.

### 4.7 Grundstellung is shared, not per-message-random

Already covered in depth in §2.9 — restated here because it is a keyspace/attack-surface
constraint, not just a procedural curiosity: all messages under one daily (Navy) key
share one Grundstellung, unlike Army practice. This means every intercepted message
from the same day is a data point about the *same* underlying rotor-position/K-book/
bigram-table relationship, which is exactly the structural redundancy historical
attacks (and any attack you build) should exploit across multiple intercepts from the
same day, not just within a single message.

---

## 5. Naval key networks (German name / British codename)

Table assembled from uboat.net's naval-Enigma-ciphers page and corroborated in part by
the Enigma-M4 Wikipedia article. Treat "Enigma version" and exact date boundaries as
approximate/best-available rather than fully cross-verified against a primary source
for every row.

| British codename | German name | Coverage | Approx. dates | Machine |
|---|---|---|---|---|
| Dolphin | Heimische Gewässer → **Hydra** | All U-boats, home waters/Atlantic (general/coastal net) | Start of war – throughout (Hydra name from ~1940) | M3 |
| Shark | **Triton** | Atlantic & (initially) Mediterranean U-boats | Net from 5 Oct 1941; M4 from 1 Feb 1942 | M3 until 1 Feb 1942, then M4 |
| Turtle | **Medusa** | Mediterranean U-boats (split off from Triton) | June 1943 – Oct 1944 | M3 (per Tony Sale lecture context; not independently re-confirmed here) |
| Narwhal | **Niobe** | Northern (Norway-based) U-boats | 25 June 1944 – war's end | not specified in sources |
| — | **Thetis** | Baltic tactical training exercises | duration unclear | not specified |
| Grampus | **Poseidon** | Black Sea U-boats | Oct 1943 – Aug 1944 | not specified |
| Sunfish | **Tibet** | Far East supply ships & U-boats | from Sept 1941 | not specified |
| Barracuda | **Neptun** | Surface fleet (battleships/heavy cruisers) | Board/network from Oct 1941; M4 use attested from ~Oct 1941 | M4 (per Enigma-M4 article — earliest attested M4 network) |
| Bonito | **Eichendorff** | Small battle units (midget submarines) | March 1944 – war's end | not specified |
| — | **Aegir** | Overseas surface raiders/auxiliary cruisers | — | M3 |

If your intercept is U-boat traffic from before Oct 1941, it's almost certainly Hydra/
Dolphin on M3. If it's Atlantic U-boat traffic from Feb 1942 onward, it's Triton/Shark
on M4 — the hardest case, and the one that caused the historical blackout. Match the
callsign/addressee pattern (§3.2, Marinefunknamenliste-derived) and date against this
table before assuming which machine/net you're dealing with.

---

## 6. Sources

Primary source (highest confidence, used for §2.1–2.6, §2.9, §3.1–3.3, §4.7):
- **"The Enigma General Procedure" (Der Schlüssel M Verfahren M Allgemein, M.Dv.
  32/1, 1940)**, translated and hosted page-by-page by Tony Sale / codesandciphers.org.uk:
  - Index: https://www.codesandciphers.org.uk/documents/egenproc/eniggnix.htm
  - General/abbreviations: https://www.codesandciphers.org.uk/documents/egenproc/page03.htm
  - Designation of stations / stress marks: https://www.codesandciphers.org.uk/documents/egenproc/page04.htm
  - Addresses and signatures: https://www.codesandciphers.org.uk/documents/egenproc/page05.htm
  - Numbers / punctuation: https://www.codesandciphers.org.uk/documents/egenproc/page06.htm
  - Summary of text aids / urgency signs: https://www.codesandciphers.org.uk/documents/egenproc/page07.htm
  - Subdivision / padding / making messages longer: https://www.codesandciphers.org.uk/documents/egenproc/page08.htm
  - Indicator groups / book indicator group: https://www.codesandciphers.org.uk/documents/egenproc/page09.htm
  - Message cipher / radio indicator groups: https://www.codesandciphers.org.uk/documents/egenproc/page10.htm
  - Ciphering / treatment of mistakes: https://www.codesandciphers.org.uk/documents/egenproc/page11.htm
  - Example message form (image, not machine-transcribed here): https://www.codesandciphers.org.uk/documents/egenproc/page14.htm
  - Offizier & Stab procedure: https://www.codesandciphers.org.uk/documents/egenproc/page15.htm
  - Ciphering Offizier & Stab: https://www.codesandciphers.org.uk/documents/egenproc/page16.htm
  - Full index / PDF also available at https://www.codesandciphers.org.uk/documents/egenproc/egenproc.pdf

Rotor/reflector wiring and machine mechanics:
- Wikipedia, "Enigma rotor details": https://en.wikipedia.org/wiki/Enigma_rotor_details
- Cryptomuseum, "Enigma wiring": https://www.cryptomuseum.com/crypto/enigma/wiring.htm
- Cryptomuseum, "How the Enigma machine works": https://www.cryptomuseum.com/crypto/enigma/working.htm
- Cipher Machines and Cryptology, "Enigma Tech Details": https://www.ciphermachinesandcryptology.com/en/enigmatech.htm
- Cryptomuseum, M4: https://www.cryptomuseum.com/crypto/enigma/m4/index.htm
- Wikipedia, "Enigma-M4": https://en.wikipedia.org/wiki/Enigma-M4
- Web search synthesis, double-stepping anomaly (Cryptomuseum PDF by David Hamer among cited results): https://www.cryptomuseum.com/people/hamer/files/double_stepping.pdf

Indicator system / Kenngruppenbuch / bigram tables:
- Wikipedia, "Discriminant Book": https://en.wikipedia.org/wiki/Discriminant_Book
- Cipher Machines and Cryptology, "Enigma Procedures": https://www.ciphermachinesandcryptology.com/en/enigmaproc.htm
- Cryptomuseum, bigram tables: https://www.cryptomuseum.com/crypto/codebook/bigram.htm
- R. Erskine, "The Principal (Kenngruppenbuch) Naval Enigma Indicator System" (PDF, fetched but not machine-readable in this pass): https://www.ciphermachinesandcryptology.com/files/kmindicator.pdf
- Tony Sale, "Lecture on Naval Enigma": https://www.codesandciphers.org.uk/lectures/naval1.htm
- "Naval Enigma" technical essay (reflector-per-net, keyspace notes): https://www.macs.hw.ac.uk/~foss/valentin/Naval%20Enigma.html
- Christos (blog), "Enigma security measures" (rotor-order rotation rules): http://chris-intel-corner.blogspot.com/2012/08/enigma-security-measures.html

Message format / plaintext example:
- Frode Weierud's CryptoCellar, German Navy Enigma message example (Hydra/Dolphin, Apr 1944): https://cryptocellar.org/enigma/dolphin.html — **note**: my automated transcription of the exact letter groups from this page was unreliable (see §3.1/§3.4); go to the page directly for the authentic text.
- Michael Hörenberg, "Breaking German Navy Ciphers" (M4 Project, U-534 messages, real decrypted traffic — not examined in depth this pass): https://enigma.hoerenberg.com/

Kurzsignale / Wetterkurzschlüssel:
- Wikipedia, "Kurzsignale": https://en.wikipedia.org/wiki/Kurzsignale
- Wikipedia, "Short Weather Cipher": https://en.wikipedia.org/wiki/Short_Weather_Cipher
- Cipher Machines and Cryptology, "Kurzsignale on U-Boats": https://www.ciphermachinesandcryptology.com/en/kurzsignale.htm
- Cipher Machines and Cryptology, "Enigma on U-Boats": https://www.ciphermachinesandcryptology.com/en/enigmauboats.htm

Networks:
- uboat.net, "Naval Enigma ciphers": https://uboat.net/technical/enigma_ciphers.htm
- uboat.net, "Allied breaking of Naval Enigma": https://uboat.net/technical/enigma_breaking.htm

Note on access: the Dirk Rijmenants pages at `users.telenet.be/d.rijmenants` (the
domain named in the task brief) are **dead** (DNS no longer resolves) — his material
appears to have migrated to ciphermachinesandcryptology.com, which is what's cited
above wherever I would otherwise have cited Rijmenants directly. The Universal Enigma
simulator page (people.physik.hu-berlin.de/~palloks/...) loaded but is a JS
application whose wiring tables are embedded in client-side code, not the fetched
HTML/text — I could not extract wiring data from it directly; the Wikipedia/
Cryptomuseum/Cipher-Machines-and-Cryptology tables were used instead and agree with
each other.

---

## 7. What I could not verify / would flag for follow-up

1. **Actual bigram table contents.** I have the *mechanism* (§2.3) solidly from two
   independent sources with matching worked examples, but not the literal
   letter-pair-substitution values of any specific named table (Quelle, Bach, Strom,
   etc.) for any specific date. Needed if you intend to actually decrypt a real
   indicator rather than just model the mechanism.
2. **Offizier/Stab key mechanics in detail** (§2.6) — the primary source explicitly
   defers to a separate manual (M.Dv. 32/2) that I did not locate/fetch.
3. **Whether an "Army-style no-repeat" rule constrained the transition between
   successive two-day Kriegsmarine inner-key periods** (§4.3) — not found either way.
4. **Precise date/scope of M4 rollout** — Neptun (surface fleet) M4 use from Oct 1941
   vs. Triton/Shark (U-boats) M4 from Feb 1942: sources are consistent on the dates but
   I could not fully pin down whether M4 was already "in general service" by Oct 1941
   or specifically limited to the Neptun network until Feb 1942.
5. **Exact meaning/length of Wetterkurzschlüssel output** — one source says a 7-letter
   group, another describes 23–24 letters from thirteen tables (§2.8); not reconciled.
6. **"UUU" and "STANDORT"/"WETTER" as the task brief framed them** — I found a solid,
   sourced meaning for tripled letters as a disambiguation device (BDUUU, FDTTT, UUU
   EINS ACHT) but no evidence of "UUU" as a dedicated submarine-alarm code, and no
   evidence STANDORT/WETTER are anything other than ordinary German words that show up
   in plaintext about position/weather. I've stated my best inference in §3.3 but flag
   it as inference, not a sourced fact — worth checking actual message transcripts
   (e.g., more enigma.hoerenberg.com U-534 pages, which I did not examine beyond the
   landing page) if this distinction matters to the attack.
7. **A clean, verbatim real message transcription.** I have a real historical example
   (Hydra/Dolphin, 11 Apr 1944, cryptocellar.org) at the level of "what it's about and
   roughly what the header fields were," but not a trustworthy letter-by-letter
   transcription — the fetch tool produced two different, mutually-inconsistent
   renderings of the same page's ciphertext groups. Get this by reading the page
   directly (not via an automated text-extraction summarizer) if an exact worked
   example is needed.
8. **Reflector A's Kriegsmarine usage** — wiring is well-attested, but I found no
   specific confirmation of how much (if at all) it was actually used operationally by
   the Navy during the war, versus B (standard) and C (occasional alternative).
