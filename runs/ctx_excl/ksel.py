#!/usr/bin/env python3
"""Flood cut for the K probe: keep cribs with <= CUT block-stops over the 3 probe jobs."""
import re, sys
CUT = 15000
cribs = [l.strip() for l in open('K.keep.txt')]
st = {int(m.group(1)): int(m.group(2)) for m in re.finditer(r'crib (\d+) .*?block-stops=(\d+)', open('out/K_probe.log').read())}
k = [c for i, c in enumerate(cribs) if st.get(i, 10**9) <= CUT]
open('K.run.txt', 'w').write(''.join(c + '\n' for c in k))
with open('K.selection.tsv', 'w') as f:
    for i, c in enumerate(cribs): f.write(f"{c}\t{st.get(i)}\t{'keep' if c in k else 'drop:flood'}\n")
print(len(k), 'of', len(cribs), 'K cribs kept after flood cut', file=sys.stderr)
