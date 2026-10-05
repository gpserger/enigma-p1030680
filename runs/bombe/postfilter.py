#!/usr/bin/env python3
"""Filter a bombe rank file to decrypts that reproduce their crib with <= k mismatches; print top N."""
import sys
path, k, n = sys.argv[1], int(sys.argv[2]), int(sys.argv[3]); cribs = sys.argv[4:]  # full crib texts
ct = open('/opt/enigma-p1030680/ciphertext.txt').read().strip()
out = []
for line in open(path):
    if line.startswith('#'): continue
    f = line.rstrip('\n').split('\t'); name, dec = f[2], f[11]
    off = int(name.rsplit('@', 1)[1]); pre = name.split('..')[0].split('@')[0]
    crib = next(c for c in cribs if c.startswith(pre) and (len(c) <= 12 or f"({len(c)})" in name))
    mm = sum(1 for j, c in enumerate(crib) if c != '?' and dec[off + j] != c and ct[off + j] != c)
    if mm <= k: out.append((float(f[0]), mm, f))
print(f"{len(out)} of rows reproduce crib with <= {k} mismatches")
for s, mm, f in out[:n]:
    print(f"{s:.3f}\t{f[2]}\t{f[3]} {f[4]} {f[5]} pos={f[6]} rings={f[7]}\tmm={mm}\t{f[10]}\t{f[11]}")
