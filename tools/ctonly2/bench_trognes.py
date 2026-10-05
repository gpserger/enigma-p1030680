#!/usr/bin/env python3
"""Baseline: patched trognes (the old run's engine) at the TRUE key of each test case, several seeds.
usage: bench_trognes.py CASES.tsv R SEEDS [extra trognes args]"""
import sys, subprocess, re, time
from concurrent.futures import ThreadPoolExecutor
ROOT='/opt/enigma-p1030680'; E=f'{ROOT}/tools/enigma/enigma'; NG=f'{ROOT}/tools/ctonly2/ngrams'
cases=[l.rstrip('\n').split('\t') for l in open(sys.argv[1]) if not l.startswith('#')]
R=sys.argv[2]; SEEDS=int(sys.argv[3]); extra=sys.argv[4:]
cases.append(['P1030698','loo','b','g','438','AACU','WQYR','CH EJ NV OU TY LG SZ PK DI QB','TTTFFFZWOVIERVVVFXDXUUUXAUSBXXTRAVEMUENDEBLEIBENXWEITEREBEFEHLEATWARTKNX',open(f'{ROOT}/runs/validation/p1030698.txt').read().strip()])
def run(arg):
    c,seed=arg; lang={'A':'foldA','B':'foldB','loo':'navalloo'}[c[1]]
    base=[E,'-4','-u',c[2],'-w',c[3].upper()+c[4],'-r',c[5],'-g',c[6]]
    t0=time.time()
    r=subprocess.run(base+['-c','-J','--score','m4f10','-f','-l',lang,'-d',NG,'-R',R,'-e',str(seed),'-T','1']+extra,input=c[9],capture_output=True,text=True)
    out=r.stdout.strip(); m=re.search(r'scored (\d+) plugboard',r.stderr)
    return c[0], sum(a==b for a,b in zip(out,c[8]))>=68, int(m.group(1)) if m else 0, time.time()-t0
jobs=[(c,s+1) for c in cases for s in range(SEEDS)]
with ThreadPoolExecutor(int(__import__('os').environ.get('TH','6'))) as ex: res=list(ex.map(run,jobs))
ok=sum(r[1] for r in res); n=len(res)
print(f"trognes R={R} {' '.join(extra)}: success {ok}/{n} = {100*ok/n:.1f}%  boards/key {sum(r[2] for r in res)/n:.0f}  wall/key {sum(r[3] for r in res)/n:.3f}s (1 thread, contended)")
per={}
for r in res: per.setdefault(r[0],[]).append(r[1])
print(' '.join(f"{k}:{sum(v)}/{len(v)}" for k,v in per.items()))
