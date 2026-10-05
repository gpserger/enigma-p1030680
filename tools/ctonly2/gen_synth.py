#!/usr/bin/env python3
"""Generate held-out synthetic 72-letter M4 test cases with py-enigma (ground truth).
Each case: random thin reflector, Greek wheel, 3 distinct wheels of I-VIII, ALL FOUR rings random,
random start, 10 random plugs; plaintext = first 72 letters of a corpus message that is EXCLUDED
from the case's n-gram fold (tools/ctonly2/ngrams/foldA_* / foldB_*).
usage: gen_synth.py OUT.tsv [keys_per_msg] [seed] [--forceleft]
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
out = sys.argv[1]; K = int(sys.argv[2]) if len(sys.argv) > 2 else 2; seed = int(sys.argv[3]) if len(sys.argv) > 3 else 1
forceleft = '--forceleft' in sys.argv
rng = random.Random(seed); AL = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'; RN = ['I','II','III','IV','V','VI','VII','VIII']
NOTCH = {'I':'Q','II':'E','III':'V','IV':'J','V':'Z','VI':'ZM','VII':'ZM','VIII':'ZM'}
with open(out, 'w') as f:
    f.write('#id\tfold\trefl\tgreek\twheels\trings\tpos\tplugs\tplaintext\tciphertext\n')
    n = 0
    for fold, ids in FOLD.items():
        for mid in ids:
            for k in range(K):
                p = pt[mid][:72]; assert len(p) == 72
                refl = rng.choice(['B-Thin', 'C-Thin']); g = rng.choice(['Beta', 'Gamma'])
                w = rng.sample(range(8), 3)
                rings = [rng.randrange(26) for _ in range(4)]
                pos = [rng.randrange(26) for _ in range(4)]
                if forceleft:   # middle window reaches its notch within the message
                    nm = NOTCH[RN[w[1]]][0]; kk = 6 if RN[w[2]] in ("VI","VII","VIII") else 3; pos[2] = (AL.index(nm) - rng.randrange(1, kk + 1)) % 26
                l = list(AL); rng.shuffle(l); plugs = ' '.join(l[2*i] + l[2*i+1] for i in range(10))
                m = EnigmaMachine.from_key_sheet(rotors=[g] + [RN[x] for x in w], reflector=refl,
                        ring_settings=rings, plugboard_settings=plugs)
                m.set_display(''.join(AL[x] for x in pos))
                c = m.process_text(p)
                f.write(f"{mid}#{k}\t{fold}\t{'B' if refl[0]=='B' else 'C'}\t{'b' if g=='Beta' else 'g'}\t{''.join(str(x+1) for x in w)}\t"
                        f"{''.join(AL[r] for r in rings)}\t{''.join(AL[x] for x in pos)}\t{plugs}\t{p}\t{c}\n"); n += 1
print(f'wrote {n} cases to {out}')
