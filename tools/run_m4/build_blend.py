#!/usr/bin/env python3
"""Rebuild the corpus 'navalblend' trognes language (recipe in corpus/ngrams/README.md:
50% wehrmacht + 15% german + 35% naval-1945 corpus, each normalised over its observed grams,
mixed, rescaled to integer counts summing to 1e7), optionally EXCLUDING given message ids
from the naval corpus (leave-one-out validation).

usage: build_blend.py OUTDIR LANGNAME [--exclude ID,ID,...]
"""
import sys, os, collections
ROOT = '/opt/enigma-p1030680'
out, lang = sys.argv[1], sys.argv[2]
excl = set(sys.argv[4].split(',')) if len(sys.argv) > 4 and sys.argv[3] == '--exclude' else set()
os.makedirs(out, exist_ok=True)
texts = []
for line in open(f'{ROOT}/corpus/ngrams/naval534_plaintext_corpus.txt'):
    if '\t' not in line: continue
    i, t = line.rstrip('\n').split('\t', 1)
    if i in excl: continue
    texts.append(''.join(c for c in t.upper() if 'A' <= c <= 'Z'))
names = {1: 'monograms', 2: 'bigrams', 3: 'trigrams', 4: 'quadgrams'}
def load(path):
    d = {}
    for l in open(path):
        p = l.split()
        if len(p) >= 2 and p[0].isalpha(): d[p[0].upper()] = d.get(p[0].upper(), 0) + float(p[1])
    return d
def norm(d):
    s = sum(d.values()); return {k: v / s for k, v in d.items()}
for n, nm in names.items():
    nav = collections.Counter()
    for t in texts:
        for i in range(len(t) - n + 1): nav[t[i:i+n]] += 1
    srcs = [(0.50, norm(load(f'{ROOT}/tools/enigma/ngrams/wehrmacht_{nm}.txt'))),
            (0.15, norm(load(f'{ROOT}/tools/enigma/ngrams/german_{nm}.txt'))),
            (0.35, norm(dict(nav)))]
    mix = collections.defaultdict(float)
    for w, d in srcs:
        for k, v in d.items(): mix[k] += w * v
    with open(f'{out}/{lang}_{nm}.txt', 'w') as f:
        for k, v in sorted(mix.items(), key=lambda x: -x[1]):
            c = round(v * 1e7)
            if c >= 1: f.write(f'{k} {c}\n')
print(f'built {lang} in {out} from {len(texts)} naval texts ({sum(map(len,texts))} letters), excluded {sorted(excl)}')
