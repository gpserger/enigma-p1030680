#!/usr/bin/env python3
"""Synthetic calibration cases for the bombe-stop prefilter (research/15, blind spot 1).
Each case: random M4 key (thin refl, Greek, 3 of I-VIII, Greek/left/right rings random, MIDDLE RING = A so
the bombe's --midring pinned space contains the key exactly), random start, 10 random plugs, plaintext =
first 72 letters of a held-out corpus message (fold tables in tools/ctonly2/ngrams exclude it).
Crib = plaintext[0:L] at offset 0, L in 15..20 chosen so that the single k=0 block has the same weak menu
as the production families (tested component <= VMAX letters and 1-3 loops; F4's ALLEVVVZEHNXSIDIX has 3 letters/4 edges/2 loops).
usage: gen_calib.py OUT.tsv N seed [VMAX=8]
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

def loops(p, c):
    """cycle rank of the best connected component of the crib menu (as tools/bombe setup_crib)."""
    par = list(range(26))
    def f(a):
        while par[a] != a:
            par[a] = par[par[a]]; a = par[a]
        return a
    pos = [i for i in range(len(p)) if p[i] != c[i]]
    pres = set()
    for i in pos:
        a, b = AL.index(p[i]), AL.index(c[i]); pres |= {a, b}; par[f(a)] = f(b)
    ne, nv = {}, {}
    for a in pres: nv[f(a)] = nv.get(f(a), 0) + 1
    for i in pos: r = f(AL.index(p[i])); ne[r] = ne.get(r, 0) + 1
    b = max(nv, key=lambda r: (ne[r] - nv[r] + 1, ne[r]))
    return ne[b] - nv[b] + 1, ne[b], nv[b]

out, N, seed = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]); want = int(sys.argv[4]) if len(sys.argv) > 4 else 8
rng = random.Random(seed); msgs = [(f, m) for f, ids in FOLD.items() for m in ids]
with open(out, 'w') as fo:
    fo.write('#id\tfold\trefl\tgreek\twheels\trings\tpos\tplugs\tcrib\tloops\tplaintext\tciphertext\n')
    n = 0; tries = 0
    while n < N and tries < 100000:
        tries += 1
        fold, mid = msgs[n % len(msgs)]
        p = pt[mid][:72]
        refl = rng.choice(['B-Thin', 'C-Thin']); g = rng.choice(['Beta', 'Gamma']); w = rng.sample(range(8), 3)
        rings = [rng.randrange(26), rng.randrange(26), 0, rng.randrange(26)]
        pos = [rng.randrange(26) for _ in range(4)]
        l = list(AL); rng.shuffle(l); plugs = ' '.join(l[2*i] + l[2*i+1] for i in range(10))
        m = EnigmaMachine.from_key_sheet(rotors=[g] + [RN[x] for x in w], reflector=refl, ring_settings=rings, plugboard_settings=plugs)
        m.set_display(''.join(AL[x] for x in pos)); c = m.process_text(p)
        # weak menus like the production families: best component with <= VMAX letters, 1..3 loops
        L = next((L for L in range(16, 19) if loops(p[:L], c[:L])[0] in (1, 2, 3) and loops(p[:L], c[:L])[2] <= want), None)
        if L is None: continue
        fo.write(f"{mid}#c{n}\t{fold}\t{'B' if refl[0]=='B' else 'C'}\t{'b' if g=='Beta' else 'g'}\t{''.join(str(x+1) for x in w)}\t"
                 f"{''.join(AL[r] for r in rings)}\t{''.join(AL[x] for x in pos)}\t{plugs}\t{p[:L]}\t{loops(p[:L], c[:L])}\t{p}\t{c}\n"); n += 1
print(f'wrote {n} cases to {out}')
