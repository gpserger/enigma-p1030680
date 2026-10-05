#!/usr/bin/env python3
"""Regenerate the address-block families with the collision test ignoring the human reader’s ten doubtful
positions (a collision there may be a misread cipher letter; the bombe excludes them anyway).
usage: gen_excl.py FAM > FAM.txt   (stderr: counts, incl. revived = collided before, clean now)"""
import sys
sys.path.insert(0, '/opt/enigma-p1030680/runs/ctx')
from gen import FAM, expand, VON, FDUA, flott, TF
from cc import coll
EXCL = {1, 10, 20, 25, 32, 58, 60, 67, 68, 71}
# T01: FdU Ausb as sender after any recipient block
FAM['T01'] = [(c, o) for c in ['VONVONFXDXUUUAUSBXX', 'VONVONFXDXUUUXAUSBXX'] for o in range(3, 31)]
# F6: boat / T.F. sender openers
FAM['F6'] = expand([FDUA + flott('5') + flott('26') + flott('25') + flott('4'), VON, ['UUU']]) + \
    [c for n in ['15', '17', '18', '19', '21', '22', '23', '24'] for c in expand([TF(n), ['VONVON', 'VVV']])] + \
    expand([['ANALLE', 'ALLE'], VON, ['TTTFFF']])
fam = sys.argv[1]
seen = set(); n = rev = bad = 0
for item in FAM[fam]:
    c, o = item if isinstance(item, tuple) else (item, 0)
    if (c, o) in seen: continue
    seen.add((c, o))
    cl = coll(c, o)
    if [p for p in cl if p not in EXCL]: bad += 1; continue
    if cl: rev += 1; print(f'{c}@{o}  # revived: collides only at {cl}')
    else: print(f'{c}@{o}')
    n += 1
print(f'# {fam}: {n} usable ({rev} revived by exclusion), {bad} collide at non-excluded positions', file=sys.stderr)
