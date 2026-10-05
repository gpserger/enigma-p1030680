#!/usr/bin/env python3
"""Apply the drop rule (min block <10 letters AND min block loops <6), the flood cut (probe stops over
3 jobs > CUT, i.e. > CUT/3 per job) and the prefix prune (drop a crib whose kept shorter prefix at the
same offset is run, since the prefix's stops include the longer crib's true setting)."""
import json, sys
CUT = int(sys.argv[1]) if len(sys.argv) > 1 else 15000
out = json.load(open('probe_info.json'))
fam = {}
for f in ['T01', 'F1', 'F2', 'F3', 'F4', 'F5', 'F6']:
    for l in open(f + '.clean.txt'): fam.setdefault(l.strip(), f)
kept = {}; rep = open('selection.tsv', 'w')
rep.write('family\tcrib\tused\tblocks(size,loops)\tprobe_stops_3jobs\tprobe_hits_g<=1\tdecision\n')
rows = []
for c, d in out:
    mb = min(b[0] for b in d['bl']); ml = min(b[1] for b in d['bl'])
    if mb < 10 and ml < 6: dec = 'drop:weak-blocks'
    elif d['stops'] > CUT: dec = 'drop:flood'
    else: dec = 'keep'
    rows.append([fam[c], c, d, dec])
keepset = sorted([r for r in rows if r[3] == 'keep'], key=lambda r: len(r[1].split('@')[0]))
final = []
for r in keepset:
    t, o = r[1].split('@')
    if any(k.split('@')[1] == o and t.startswith(k.split('@')[0]) for k in final): r[3] = 'prune:prefix-run'; continue
    final.append(r[1])
for f, c, d, dec in rows:
    rep.write(f"{f}\t{c}\t{d['used']}\t{' '.join(f'{a},{b}' for a, b in d['bl'])}\t{d['stops']}\t{d['hits']}\t{dec}\n")
    if dec == 'keep': open(f'{f}.run.txt', 'a').write(c + '\n')
import collections
cnt = collections.Counter((f, dec.split(':')[0] + (':' + dec.split(':')[1] if ':' in dec else '')) for f, c, d, dec in rows)
for k in sorted(cnt): print(k, cnt[k])
