# Rotor/reflector wiring tables extracted from the Universal Enigma simulator

Source: Daniel Palloks, "Universal Enigma" simulator, v2.6.2 EN (19 Nov 2023)
<https://people.physik.hu-berlin.de/~palloks/js/enigma/enigma-u_v262_en.html>
Fetched 2026-09-22 (the page itself is not redistributed here).
License note (from the page's own `info()` dialog): "This software and source
code may be used, distributed and modified freely as long as (1) my authorship
remains acknowledged, (2) any modification is properly indicated, (3) the
freeware/open source status ... remain unchanged."

All wiring strings below are given as the simulator stores them: a 26-character
string `w` such that plaintext letter `abc[i]` (0-indexed, a=0) maps to `w[i]`
when the signal enters the rotor from the "right" (ETW) side, in the rotor's
own reference frame (position 'A', ring setting '01'). This is the standard
"wiring table" representation used by essentially all Enigma literature/software
(Deavours & Kruh, Rijmenants, Wikipedia, py-enigma, etc.) — i.e. it is exactly
`wiring[i] = letter contacted by input contact i`, listed alphabetically A-Z.

## 1. Rotors I-VIII (Enigma I / Wehrmacht / M3 / M4) — the ones we need

| Rotor | Wiring (A→Z)                 | Notch(es) (turnover)      | Notch letter→next |
|-------|-------------------------------|----------------------------|--------------------|
| I     | `EKMFLGDQVZNTOWYHXUSPAIBRCJ`  | Q                           | Q→R |
| II    | `AJDKSIRUXBLHWTMCQGZNPYFVOE`  | E                           | E→F |
| III   | `BDFHJLCPRTXVZNYEIWGAKMUSQO`  | V                           | V→W |
| IV    | `ESOVPZJAYQUIRHXLNFTGKDCMWB`  | J                           | J→K |
| V     | `VZBRGITYUPSDNHLXAWMJQOFECK`  | Z                           | Z→A |
| VI    | `JPGVOUMFYQBENHZRDKASXLICTW`  | Z, M (two notches)          | Z→A, M→N |
| VII   | `NZJHGRCXMYSWBOUFAIVLPEKQDT`  | Z, M (two notches)          | Z→A, M→N |
| VIII  | `FKQHTLXOCBJSPDZRAMEWNIUYGV`  | Z, M (two notches)          | Z→A, M→N |

Rotors I-V are the original Enigma I / Heer&Luftwaffe set (also used by Navy
M3). Rotors VI, VII, VIII (double-notch) were Navy-specific, introduced for
M3/M4 boats. **All eight are selectable for M3 and M4 in the simulator**
(`m[3]`/`m[4]` presets both have `w1st:1, wlast:8`, i.e. rotor-table indices
1..8 = I..VIII).

## 2. Beta and Gamma ("Greek") wheels — M4 4th rotor, non-turning

These sit in the leftmost (4th) slot on the M4, do **not** step during
operation, and only exist paired with a "thin" reflector (see below).

| Wheel | Wiring (A→Z) |
|-------|---------------|
| Beta  | `LEYJVCNIXWPBQMDRTAKZGFUHOS` |
| Gamma | `FSOKANUERHMBTIYCWLQPZXVGJD` |

JS source variables: `walzebeta`, `walzegamma`.

## 3. Reflectors (UKW)

| Reflector | Wiring (A→Z) | Notes |
|---|---|---|
| UKW A | `EJMZALYXVBWFCRQUONTSPIKHGD` | Enigma I, early war |
| UKW B | `YRUHQSLDPXNGOKMIEBFZCWVJAT` | Standard M3 reflector ("thick" B) |
| UKW C | `FVPJIAOYEDRZXWGCTKUQSBNMHL` | Standard M3 alt. reflector ("thick" C) |
| UKW B-thin (`B.dünn`) | `ENKQAUYWJICOPBLMDXZVFTHRGS` | **M4 only** — thin reflector, used together with Beta or Gamma in the 4th slot |
| UKW C-thin (`C.dünn`) | `RDOBJNTKVEHMLFCWZAXGYIPSUQ` | **M4 only** — thin reflector, paired with Beta or Gamma |

JS source variables: `ukw[1]`=A, `ukw[2]`=B, `ukw[5]`=C, `ukbduenn`=B-thin,
`ukcduenn`=C-thin. The simulator represents "UKW B-thin + Beta" etc. as a
*virtual combined reflector*, built from two separate 26-letter tables
composed per keystroke — **the Greek wheel is traversed first (forward),
then the fixed thin reflector, then the Greek wheel again (inverse)** —
i.e. physically the same arrangement as the real M4: rotor III (leftmost
moving rotor) → Beta/Gamma → thin reflector → Beta/Gamma (reverse) → rotor
III (reverse). See §6 below for the exact signal-path trace (function
`kodieren()`) and §9 for a live, verified run of this exact path. **If your
own implementation precomputes a single effective reflector permutation for
M4** (Greek⁻¹ ∘ thin ∘ Greek, at the Greek wheel's ring/position offset),
**you can validate it by composing these two tables in that order** and
diffing against your composed table — do **not** apply the thin reflector
first, that is backwards (an earlier draft of this document had it
backwards; corrected after tracing `kodieren()` line-by-line and confirming
empirically against a real M4 decrypt in §9).

## 4. Entry wheel (ETW)

For M3 and M4 (and Enigma I) the entry wheel is the **identity** permutation
(`ETW = ABCDEFGHIJKLMNOPQRSTUVWXYZ`, i.e. keyboard letter A enters the rotor
stack at contact A — no scrambling). JS: `etw = abc` (selected via `etwsel(1)`,
which every M3/M4/I/N/S preset uses; `m[x].etw:1`). The QWERTZU-wired ETW
(`etq`) and the Enigma-T ETW (`ett`) are for the commercial/Abwehr/Tirpitz
machines only — **not relevant to M3/M4**, but included below for completeness.

| ETW variant | Wiring (A→Z) | Used by |
|---|---|---|
| Identity (`abc`) | `ABCDEFGHIJKLMNOPQRSTUVWXYZ` | Enigma I, M3, M4, Norenigma, Sondermaschine |
| QWERTZU (`etq`) | `JWULCMNOHPQZYXIRADKEGVBTSF` | D, K, Swiss-K, R, A28/G31, G-111/260/312/401 |
| Enigma-T (`ett`) | `ILXRZTKGJYAMWVDUFCPQEONSHB` | Enigma T ("Tirpitz") |

## 5. Plugboard (Steckerbrett) and Uhr

The plugboard is a simple involution (up to 13 letter-pairs swapped,
identity on unplugged letters) — no fixed table, it's whatever pairs are
configured. The "Enigma-Uhr" accessory (a non-reciprocal plug adaptor, rare)
is implemented via lookup arrays `eu`/`ue`/`ab`/`ba` in the source; not
relevant for standard M3/M4 U-boat traffic (plain plugboard, no Uhr), so not
reproduced in full here — see source lines ~223-226 if ever needed.

## 6. Signal path / algorithm (for validating your own implementation logic, not just tables)

From `kodieren()` (the actual per-character encipher function), forward
path for a keypress `s`:
1. Plugboard (if `stecker` active): `s0 = swp[s]`
2. ETW (fixed): `se = etw[s0]` (identity for M3/M4)
3. Rotor 1 (rightmost/fast), forward: index into `w[1]` at `se`'s position
4. Rotor 2 (middle), forward, offset by rotor-1→rotor-2 relative rotation
5. Rotor 3 (leftmost/slow), forward, similarly offset
6. Reflector stage, `w[0]`:
   - **M3** (`ukm4==''`): straight lookup in `w[0]` (the selected A/B/C
     table) — done.
   - **M4** (`ukm4!=''`, i.e. a thin reflector + Greek wheel is selected):
     `w[0]` here actually holds the **Greek wheel's** wiring (Beta or
     Gamma), *not* the thin reflector — confusingly reused from the M3
     "UKW slot". The exact order, traced from `kodieren()`:
     1. `su = w[0].charAt(...)` — signal enters the **Greek wheel forward**
        (offset by `rw[3]`, the left rotor's effective rotation — the same
        "enter stage N with previous stage's rotation" pattern used
        throughout).
     2. `thin = ukm4.charAt(... su ... offset rw[0] ...)` — the Greek
        wheel's *output* enters the **fixed thin reflector** (`ukm4` =
        `ukbduenn`/`ukcduenn`), offset by `rw[0]` (the Greek wheel's own
        ring/position offset, since it doesn't step but does have a ring
        setting).
     3. `su = ... w[0].indexOf(...thin...) ... offset rw[0] ...` — the thin
        reflector's output is passed back **through the Greek wheel in
        reverse** (again offset by `rw[0]`), producing the final `su` that
        continues back out through rotors 3, 2, 1.

     So the M4 reflector stage is a sandwich: **Greek wheel (forward) →
     thin reflector → Greek wheel (inverse)** — structurally identical to
     the real M4 hardware, where Beta/Gamma physically sits between rotor
     III and the thin reflector. (An earlier version of this document had
     this backwards — thin-reflector-then-Greek-wheel — which is wrong;
     fixed after re-tracing the source and confirming against a real M4
     decrypt, see §9.)
7. Rotor 3, 2, 1 backward (inverse permutation of the same wirings)
8. ETW inverse
9. Plugboard inverse (same table, since plugboard is an involution)

Rotor stepping (`engage_lever()`, used by all M3/M4/I/N/S — "mech:false",
i.e. lever/pawl mechanism with the historically-accurate **double-stepping
anomaly**, not a clean odometer): rotor 1 always steps; rotor 2 steps if
rotor 1 is *at* its notch; rotor 3 steps if rotor 2 is *at* its notch (this
"if rotor 2 is at its notch, it also steps itself" is exactly the
double-stepping quirk). The 4th slot (Beta/Gamma on M4) never steps
(`urot:false` for M4). Ring settings shift the notch position and the
internal wiring's rotational offset relative to the position indicator, per
the classic `rot()`/effective-rotation (`effRot()`) computation. This
matches the standard, well-documented Enigma stepping model — useful as a
cross-check if your own stepping logic disagrees with the simulator.

## 7. Model presets relevant to us

| Model | JS `ukwhash` decode → allowed reflectors | Rotor slots (walze index range) | Plugboard | Notes |
|---|---|---|---|---|
| M3 (`m[3]`) | bits 2,5 → UKW B, UKW C only | 1..8 (I-VIII) | yes (13 pairs) | 3 rotors + reflector, no Greek wheel |
| M4 (`m[4]`) | bits 3,4,6,7,8 → B-thin+Beta, B-thin+Gamma, C-thin+Beta, C-thin+Gamma, **and rewirable UKW-D** | 1..8 (I-VIII) for the 3 moving rotors; 4th (Beta/Gamma) selected via the reflector menu | yes (13 pairs) | 4th (Beta/Gamma) rotor is non-turning |
| Enigma I (`m[2]`) | bits 1,2,5,8 → A, B, C, UKW-D | 1..5 (I-V only) | yes | Army/Luftwaffe, no VI-VIII, no Greek wheels |

`ukwhash` is a bitmask (`2^menu_index`) computed once and decoded by
`buildUMenu()` to build the reflector `<select>` for that model — I decoded
it above by hand (472 = 2^3+2^4+2^6+2^7+2^8, i.e. reflector-menu items 3,4,6,7,8).

## 8. Non-M3/M4 machines (extracted for completeness, not needed for this project)

Extraction script: `tools/simulator_src/extract_wirings.py` (parses the
UTF-8 copy of the page with regexes; reproducible — run it again any time to
re-derive everything in this document). Its raw output is saved verbatim at
`tools/simulator_src/extraction_raw_output.txt`. Full tables below.

**Rotors** (`walze[9..57]`, wiring / notch letter(s) / rotor ID):

| # | Machine | ID | Wiring | Notch |
|---|---|---|---|---|
| 9  | N (Norenigma) | 1N | `WTOKASUYVRBXJHQCPZEFMDINLG` | Q |
| 10 | N | 2N | `GJLPUBSWEMCTQVHXAOFZDRKYNI` | E |
| 11 | N | 3N | `JWFMHNBPUSDYTIXVZGRQLAOEKC` | V |
| 12 | N | 4N | `FGZJMVXEPBWSHQTLIUDYKCNRAO` | J |
| 13 | N | 5N | `HEJXQOTZBVFDASCILWPGYNMURK` | Z |
| 14 | S (Sondermaschine) | 1S | `VEOSIRZUJDQCKGWYPNXAFLTHMB` | Q |
| 15 | S | 2S | `UEMOATQLSHPKCYFWJZBGVXIDNR` | E |
| 16 | S | 3S | `TZHXMBSIPNURJFDKEQVCWGLAOY` | V |
| 17 | D (commercial 1926) | 1D | `LPGSZMHAEOQKVXRFYBUTNICJDW` (=`com1`) | Z (trivial RS) |
| 18 | D | 2D | `SLVGBTFXJQOHEWIRZYAMKPCNDU` (=`com2`) | Z (trivial RS) |
| 19 | D | 3D | `CJGDPSHKTURAWZXFMYNQOBVLIE` (=`com3`) | Z (trivial RS) |
| 20 | K (commercial 1927) | 1K | `com1` | Y |
| 21 | K | 2K | `com2` | E |
| 22 | K | 3K | `com3` | N |
| 23 | Swiss-K | 1SK | `PEZUOHXSCVFMTBGLRINQJWAYDK` | Y |
| 24 | Swiss-K | 2SK | `ZOUESYDKFWPCIQXHMVBLGNJRAT` | E |
| 25 | Swiss-K | 3SK | `EHRVXGAOBQUSIMZFLYNWKTPDJC` | N |
| 26 | R "Rocket" (BP wiring) | 1R | `JGDQOXUSCAMIFRVTPNEWKBLZYH` | N |
| 27 | R | 2R | `NTZPSFBOKMWRCJDIVLAEYUXHGQ` | E |
| 28 | R | 3R | `JVIUBHTCDYAKEQZPOSGXNRMWFL` | Y |
| 29 | R° (original wiring) | 1R° | `EVLPKUDJHTGSZFRABWYICOXNMQ` | Y |
| 30 | R° | 2R° | `HXMQKGJTSCZFLBERNAWYIDOVPU` | E |
| 31 | R° | 3R° | `JHDBSKYPZNMVXURECLIGQOAWTF` | N |
| 32 | T "Tirpitz" | 1T | `KPTYUELOCVGRFQDANJMBSWHZXI` | W,Z,E,K,Q |
| 33 | T | 2T | `UPHZLWEQMTDJXCAKSOIGVBYFNR` | W,Z,F,L,R |
| 34 | T | 3T | `QUDLYRFEKONVZAXWHMGPJBSICT` | W,Z,E,K,Q |
| 35 | T | 4T | `CIWTBKXNRESPFLYDAGVHQUOJZM` | W,Z,F,L,R |
| 36 | T | 5T | `UAXGISNJBVERDYLFZWTPCKOHMQ` | Y,C,F,K,R |
| 37 | T | 6T | `XFUZGALVHCNYSEWQTDMRBKPIOJ` | X,E,I,M,Q |
| 38 | T | 7T | `BJVFTXPLNAYOZIKWGDQERUCHSM` | Y,C,F,K,R |
| 39 | T | 8T | `YMTPNZHWKODAJXELUQVGCBISFR` | X,E,I,M,Q |
| 40 | KD | 1KD | `VEZIOJCXKYDUNTWAPLQGBHSFMR` | S,U,Y,A,E,H,L,N,Q |
| 41 | KD | 2KD | `HGRBSJZETDLVPMQYCXAOKINFUW` | S,U,Y,A,E,H,L,N,Q |
| 42 | KD | 3KD | `NWLHXGRBYOJSAZDVTPKFQMEUIC` | S,U,Y,A,E,H,L,N,Q |
| 43 | A28/G31 | 1A8 | `com1` | 17 notches (A-865) |
| 44 | A28/G31 | 2A8 | `com2` | 15 notches |
| 45 | A28/G31 | 3A8 | `com3` | 11 notches |
| 46 | G-111 | 1G1 | `WLRHBQUNDKJCZSEXOTMAGYFPVI` | 17 notches |
| 47 | G-111 | 2G1 | `TFJQAZWMHLCUIXRDYGOEVBNSKP` | 15 notches |
| 48 | G-111 | 5G1 | `QTPIXWVDFRMUSLJOHCANEZKYBG` | 7 notches |
| 49 | G-260 | 1G2 | `RCSPBLKQAUMHWYTIFZVGOJNEXD` | 17 notches |
| 50 | G-260 | 2G2 | `WCMIBVPJXAROSGNDLZKEYHUFQT` | 15 notches |
| 51 | G-260 | 3G2 | `FVDHZELSQMAXOKYIWPGCBUJTNR` | 11 notches |
| 52 | G-312 | 1G3 | `DMTWSILRUYQNKFEJCAZBPGXOHV` | 17 notches |
| 53 | G-312 | 2G3 | `HQZGPJTMOBLNCIFDYAWVEUSRKX` | 15 notches |
| 54 | G-312 | 3G3 | `UQNTLSZFMREHDPXKIBVYGJCWOA` | 11 notches |
| 55 | G-401 | 1G4 | `WRVSUEJFPTLXHBIQYOKCAGNZDM` | 17 notches |
| 56 | G-401 | 2G4 | `KGRZMSIBANYHTQXFWCLOVEDJPU` | 15 notches |
| 57 | G-401 | 3G4 | `FNWJUHPZXRGOKASIBYTDLCQEMV` | 11 notches |

(`com1`/`com2`/`com3` = `LPGSZMHAEOQKVXRFYBUTNICJDW` /
`SLVGBTFXJQOHEWIRZYAMKPCNDU` / `CJGDPSHKTURAWZXFMYNQOBVLIE` — the shared
"commercial Enigma" wiring reused, at different notch positions, across
D/K/Swiss-K¹/A28/G-machines. ¹Swiss-K actually has its own distinct wiring,
not `com1-3` — see rows 23-25.)

**Reflectors** (`ukw[9..16]`):

| ID | Wiring | Machine |
|---|---|---|
| uG3 | `RULQMZJSYGOCETKWDAHNBXPVIF` | UKW(G-312) |
| uG4 | `YDUBJGFWSEORVZKXTLIQCMHPAN` | UKW(G-401) |
| u$ | `IMETCGFRAYSQBZXWLHKDVUPOJN` | UKW(commercial) |
| uN | `MOWJYPUXNDSRAIBFVLKZGQCHET` | UKW(N) |
| uR | `QYHOGNECVPUZTFDJAXWMKISRBL` | UKW(R) |
| uR° | `DNSAJQIPGEXRWBVHFLCZYOMKUT` | UKW(R°) |
| S | `CIAGSNDRBYTPZFULVHEKOQXWJM` | UKW S |
| uT | `GEKPBTAUMOCNILJDXZYFHWVQSR` | UKW(T) |
| X | `ONVSRGFJUHPMLBAKWEDZICQYXT` | Maintenance reflector (swaps adjacent keys) |

**Entry wheels already covered in §4.** **Rewirable UKW-D presets**
(`upairs[]`, orig. key format): KD machine wiring (FRA/Sweden)
`AQBGCKDIELFXHZMWNVOTPURS`; GAF wiring 11 Mar 1945 [Ostwald/Weierud 2016]
`AVBOCTDMEZFNGXHQISKRLUPW`; Luftwaffe key sheet #2744 days 1-9
`HLKNFMEIACBGDSOWPZQXRUTV`; days 10-21 `AGIRBHCSDZEWFKLXMPOUNTQV`; days
22-31 `HKGLNQSVUXTZRWADBFCOEPIM`. UKW-D contact-label mapping (BP-style ↔
UKW-D labels): `udlabels = 'a-zxwvutsrqpon-mlkihgfedcb'`.

## 9. How settings are expressed in the UI / internal state (no URL-parameter interface)

The simulator has **no query-string or hash-based settings interface** — there
is no code anywhere that reads `location.search` or `location.hash`. All state
lives in in-memory JS globals (`walz0`, `wlz`, `w0`/`wout`, `r`, `pairs`,
`ukm4`, `mod`, etc.) manipulated exclusively through onClick/onChange handlers
on DOM form elements. There is no way to load a saved configuration via URL;
`"= New session ="` just does `window.location.href = window.location.href`
(a page reload back to defaults).

The one human-readable "settings string" the page produces is the `key1`
readout built by `viewKey()`, e.g.:
```
Wheels: B.-III-II-I (AAAA) / Rings: 01 01 01 01 / Plugged: AB CD ...
```
(reflector.-left-mid-right rotor names, current letter positions, ring
settings 1-26, plugboard pairs) — this is a display string only, not
re-parseable as input.

**Headless driving is possible but requires a DOM**, because essentially
every function (`model()`, `wsel()`, `steck()`, and even the core cipher
function `kodieren()` via its call to `effRot()`) reads/writes
`document.getElementById(...)`/`document.f.*`/`document.a.*` on almost every
call — none of it is a pure function of (state, DOM-free). Plain Node.js
without a DOM will not work. **We proved it works with `jsdom`**: see
`tools/simulator_src/nodecheck/drive.js`. Two jsdom gaps had to be patched
(both are real, load-bearing browser behaviors this 2007-era script relies
on that jsdom doesn't implement):
1. `document.<formName>` legacy named access (jsdom only supports
   `document.forms.namedItem(name)`).
2. `formElement.<controlName>` legacy named access (jsdom only supports
   `form.elements.namedItem(name)`).

Both were patched with a small `Proxy` wrapper (~10 lines), after which the
**unmodified** simulator source runs correctly under jsdom. Recipe:
```js
const { JSDOM } = require('jsdom');
const dom = new JSDOM(html, { runScripts: 'dangerously', resources: 'usable', pretendToBeVisual: true });
const doc = dom.window.document;
function wrapForm(form) {
  return new Proxy(form, { get(t,p,r){ if (p in t) return Reflect.get(t,p,r);
    const n = t.elements.namedItem(p); return n===null?undefined:n; }});
}
for (const f of ['a','f','s','k']) doc[f] = wrapForm(doc.forms[f]);
dom.window.neu(doc.a.preset.value);       // bootstrap (normally <body onload>)
dom.window.model(3);                       // select M3 preset (see model list above)
dom.window.wsel(3,1); dom.window.wsel(2,2); dom.window.wsel(1,3); // left/mid/right rotor
dom.window.wsel(0,2);                      // reflector UKW B
dom.window.wlzReset(); dom.window.rngReset(); // positions AAAA, rings 01 01 01 01
const ciphertext = dom.window.enigma('aaaaa'); // -> "bdzgo "
```
This was run for real (`npm install jsdom` in `tools/simulator_src/nodecheck/`,
`node drive.js`) and **reproduced the classic published test vector exactly**:
rotors I II III (left→right), reflector B, ring settings 01 01 01, start
position AAA, plaintext `AAAAA` → ciphertext `BDZGO` (simulator returned
`"bdzgo "`, lowercase with trailing group-space, which is its normal output
formatting). This gives us a second, independent oracle (beyond py-enigma)
to cross-check any homegrown Enigma implementation against — including full
M4 (Beta/Gamma + thin reflectors), all rotor choices, ring settings, and
plugboard, all reachable through `model()`/`wsel()`/`steck()`/`enigma()`
exactly as the GUI does it, just without a browser.

**We then pushed the full M4 case through the same harness**
(`tools/simulator_src/nodecheck/drive_m4.js`) — the real, historical U-264
message (see `research/04_tooling.md` §5 for provenance): reflector B-thin +
Beta, rotors Beta II IV I, ring settings A A A V, start position VJNA,
plugboard `AT BL DF GJ HM NW OP QY RZ VX`, driven via `model(4)`, `wsel(0,3)`
(UKW-menu value 3 = B-thin+Beta), `wsel(3,2)`/`wsel(2,4)`/`wsel(1,1)` for the
three moving rotors, `switchRngSettings(true)` + `setw(1, setr('22'))` +
`switchRngSettings(false)` for the right rotor's ring, `setw(w, set(letter))`
for each of the four start-position slots, and `document.getElementsByName
('stf')[i].value = pair` + `steck()` for the plugboard. **Result: exact
match** with the published plaintext (`VONVONJLOOKSJHFFTTTEINSEINS...`,
232 letters, byte-for-byte) — confirming both the jsdom-driving recipe and
the corrected M4 signal-path trace in §6 against real ciphertext, not just
the short synthetic `AAAAA` vector. (Two transcription typos in the
ciphertext string on the first two attempts caused partial mismatches,
diagnosed by diffing byte-by-byte against the authoritative string in
`tools/verify_py_enigma_m4.py` — a reminder to always diff long hand-copied
ciphertexts programmatically rather than eyeballing them.)
