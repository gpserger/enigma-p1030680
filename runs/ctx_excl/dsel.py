#!/usr/bin/env python3
"""k=0 fallback for cribs dropped at k=1: keep those with <= 15000 block-stops over 3 probe jobs,
then prefix-prune (a kept shorter crib at the same offset covers the longer one)."""
import re
cribs = [l.strip() for l in open('D.k0cand.txt')]
txt = open('out/D_probe.log').read()
st = {int(m.group(1)): int(m.group(2)) for m in re.finditer(r'crib (\d+) .*?block-stops=(\d+)', txt)}
bl = {int(m.group(1)): m.group(2) for m in re.finditer(r'crib (\d+) \S+ used=\d+ excluded=\[[^\]]*\] blocks=\d+ sizes: (.*)', txt)}
ok = sorted([c for i, c in enumerate(cribs) if st.get(i, 10**9) <= 15000], key=len)
fin = []
for c in ok:
    t, o = c.split('@')
    if any(k.split('@')[1] == o and t.startswith(k.split('@')[0]) for k in fin): continue
    fin.append(c)
open('D.run.txt', 'w').write(''.join(c + '\n' for c in fin))
with open('D.selection.tsv', 'w') as f:
    for i, c in enumerate(cribs): f.write(f"{c}\t{bl.get(i)}\t{st.get(i)}\t{'keep' if c in fin else ('prune' if c in ok else 'drop:flood')}\n")
print(len(fin), 'of', len(cribs), 'kept for k=0')
