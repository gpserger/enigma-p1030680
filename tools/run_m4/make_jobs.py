#!/usr/bin/env python3
"""Write the ordered job list: one job per (thin reflector, greek wheel, wheel order).
Priority: the three 1945 Potsdam wheel orders first, then VIII on the right (all three known
Potsdam keys have VIII right), then VIII elsewhere, then orders with VI/VII, then the rest.
Within an order the 4 reflector/greek combos go most-seen-in-Potsdam first (C/Gamma, B/Gamma,
C/Beta, B/Beta)."""
import itertools, sys
out = sys.argv[1] if len(sys.argv) > 1 else '/opt/enigma-p1030680/runs/m4_ctonly/jobs.txt'
potsdam = [(4,3,8), (5,6,8), (5,2,8)]
def tier(o):
    if o in potsdam: return 0
    if o[2] == 8: return 1
    if 8 in o: return 2
    if 6 in o or 7 in o: return 3
    return 4
orders = sorted(itertools.permutations(range(1, 9), 3),
                key=lambda o: (tier(o), potsdam.index(o) if o in potsdam else 0,
                               -sum(w >= 6 for w in o), -(o[2] >= 6), o))
combos = [('c', 'G'), ('b', 'G'), ('c', 'B'), ('b', 'B')]
n = 0
with open(out, 'w') as f:
    f.write('# prio jobid refl greek wheels tier\n')
    for o in orders:
        for refl, gk in combos:
            n += 1
            w = ''.join(map(str, o))
            f.write(f'{n:04d} {n:04d}_{refl}{gk}{w} {refl} {gk} {w} {tier(o)}\n')
print(f'{n} jobs -> {out}')
