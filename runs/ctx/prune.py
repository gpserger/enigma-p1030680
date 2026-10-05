#!/usr/bin/env python3
"""Read CRIB@OFF lines on stdin; drop any crib having a shorter crib (same offset, len>=MINP) as prefix."""
import sys
MINP=int(sys.argv[1]) if len(sys.argv)>1 else 22
L=[l.strip() for l in sys.stdin if l.strip() and not l.startswith('#')]
L=sorted(set(L), key=lambda s: len(s))
keep=[]
for s in L:
    c,o=s.rsplit('@',1)
    if any(k.rsplit('@',1)[1]==o and len(k.rsplit('@',1)[0])>=MINP and c.startswith(k.rsplit('@',1)[0]) for k in keep): continue
    keep.append(s)
for s in keep: print(s)
print(f'# kept {len(keep)} of {len(L)}',file=sys.stderr)
