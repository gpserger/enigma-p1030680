#!/usr/bin/env python3
"""Crib generator + self-encipherment screen for research/14 (578 cribs).
Collisions at the human reader’s ten doubtful positions do not kill a crib (the bombe excludes them)."""
import itertools, sys
CT = open('/opt/enigma-p1030680/ciphertext.txt').read().strip()
EXCL = {1, 10, 20, 25, 32, 58, 60, 67, 68, 71}
def coll(c, o):
    return [o + i for i, ch in enumerate(c) if ch != '?' and o + i < len(CT) and CT[o + i] == ch]
def clean(c, o):
    return all(p in EXCL for p in coll(c, o)) and o + len(c) <= len(CT)
def offsets(c, lo=0, hi=None):
    hi = len(CT) - len(c) if hi is None else min(hi, len(CT) - len(c))
    return [o for o in range(lo, hi + 1) if clean(c, o)]
if __name__ == '__main__':
    for c in sys.argv[1:]:
        print(c, len(c), offsets(c))
