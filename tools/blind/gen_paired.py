#!/usr/bin/env python3
"""Paired single-notch / double-notch synthetic M4 cases (research/15, blind spot 2).
For each base key (plaintext, plugs, reflector, Greek, left+middle wheels, all rings and positions) two cases
are written that differ ONLY in the right wheel: one of I-V (single notch) and one of VI-VIII (double notch).
Plaintext = first 72 letters of a held-out corpus message (fold tables tools/ctonly2/ngrams/fold{A,B} exclude it).
Middle-wheel window is kept away from its notch so that, for BOTH right wheels, the left wheel does not step
inside the message (the nostep stage-A class is exact); pass --allowleft to drop that restriction.
usage: gen_paired.py OUT.tsv keys_per_msg seed [--allowleft]
"""
import sys, random
sys.path.insert(0, '/opt/enigma-p1030680/.venv/lib/python3.14/site-packages')
from enigma.machine import EnigmaMachine
ROOT = '/opt/enigma-p1030680'
FOLD = {'A': 'P1030660 P1030661 P1030667 P1030671 P1030689 P1030693 P1030684 P1030696'.split(),
        'B': 'P1030702 P1030695 P1030663 P1030676 P1030701 P1030708 P1030673 P1030686'.split()}
pt = {}
for l in open(f'{ROOT}/corpus/ngrams/naval534_plaintext_corpus.txt'):
    if '\t' in l:
        i, t = l.rstrip('\n').split('\t', 1); pt[i] = ''.join(c for c in t.upper() if 'A' <= c <= 'Z')
AL = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'; RN = ['I','II','III','IV','V','VI','VII','VIII']
NOTCH = {'I':'Q','II':'E','III':'V','IV':'J','V':'Z','VI':'ZM','VII':'ZM','VIII':'ZM'}
def left_steps(wm, wr, pm, pr, n=72):
    m, r = pm, pr
    for i in range(n):
        mn = AL[m] in NOTCH[RN[wm]]; rn = AL[r] in NOTCH[RN[wr]]
        if mn: return True
        if rn: m = (m + 1) % 26
        r = (r + 1) % 26
    return False
out, K, seed = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]); allowleft = '--allowleft' in sys.argv
rng = random.Random(seed)
with open(out, 'w') as f:
    f.write('#id\tfold\trefl\tgreek\twheels\trings\tpos\tplugs\tplaintext\tciphertext\n')
    n = 0
    for fold, ids in FOLD.items():
        for mid in ids:
            for k in range(K):
                p = pt[mid][:72]
                while True:
                    refl = rng.choice(['B-Thin', 'C-Thin']); g = rng.choice(['Beta', 'Gamma'])
                    sn = rng.randrange(5); dn = rng.randrange(5, 8)
                    rest = [x for x in range(8) if x not in (sn, dn)]; wl, wm = rng.sample(rest, 2)
                    rings = [rng.randrange(26) for _ in range(4)]; pos = [rng.randrange(26) for _ in range(4)]
                    if allowleft or not (left_steps(wm, sn, pos[2], pos[3]) or left_steps(wm, dn, pos[2], pos[3])): break
                l = list(AL); rng.shuffle(l); plugs = ' '.join(l[2*i] + l[2*i+1] for i in range(10))
                for tag, wr in (('S', sn), ('D', dn)):
                    w = [wl, wm, wr]
                    m = EnigmaMachine.from_key_sheet(rotors=[g] + [RN[x] for x in w], reflector=refl, ring_settings=rings, plugboard_settings=plugs)
                    m.set_display(''.join(AL[x] for x in pos)); c = m.process_text(p)
                    f.write(f"{mid}#{k}{tag}\t{fold}\t{'B' if refl[0]=='B' else 'C'}\t{'b' if g=='Beta' else 'g'}\t{''.join(str(x+1) for x in w)}\t"
                            f"{''.join(AL[r] for r in rings)}\t{''.join(AL[x] for x in pos)}\t{plugs}\t{p}\t{c}\n"); n += 1
print(f'wrote {n} cases to {out}')
