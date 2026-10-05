#!/usr/bin/env python3
# True-key plugboard-recovery benchmark for trognes/enigma (patched) on random M4 keys,
# 10 plugs, 72-letter windows of real telegraphic plaintexts. Reports success rate and
# mean plugboards scored per key (cost proxy).
import random, subprocess, sys, re, shlex
from concurrent.futures import ThreadPoolExecutor
E='tools/enigma/enigma'; env={'ENIGMA_DATA':'tools/enigma/ngrams'}
P=[l.strip() for l in open('runs/validation/bench/plaintexts.txt') if len(l.strip())>=72]
N=int(sys.argv[1]); configs=sys.argv[2:]
AL='ABCDEFGHIJKLMNOPQRSTUVWXYZ'
def case(t):
    rng=random.Random(1000+t)
    pl=rng.choice(P); o=rng.randrange(len(pl)-71); pt=pl[o:o+72]
    refl=rng.choice('bc'); g=rng.choice('BG'); w=rng.sample('12345678',3)
    ring='AA'+rng.choice(AL)+rng.choice(AL); st=''.join(rng.choice(AL) for _ in range(4))
    l=list(AL); rng.shuffle(l); plugs=' '.join(l[2*i]+l[2*i+1] for i in range(10))
    base=[E,'-4','-u',refl,'-w',g+''.join(w),'-r',ring,'-g',st]
    ct=subprocess.run(base+['-s',plugs],input=pt,capture_output=True,text=True,env=env).stdout.strip()
    return pt,base,ct
cases=[case(t) for t in range(N)]
for cfg in configs:
    args=shlex.split(cfg)
    def run(c):
        pt,base,ct=c
        r=subprocess.run(base+['-c','-J','-l','wehrmacht','-T','1','-e','11']+args,input=ct,capture_output=True,text=True,env=env)
        out=r.stdout.strip(); m=re.search(r'scored (\d+) plugboard',r.stderr)
        return (sum(a==b for a,b in zip(out,pt))>=69), int(m.group(1)) if m else 0
    with ThreadPoolExecutor(16) as ex: res=list(ex.map(run,cases))
    ok=sum(r[0] for r in res); cost=sum(r[1] for r in res)/N
    print(f"{cfg:55s} success {ok}/{N} = {100*ok/N:5.1f}%   boards/key {cost:10.0f}   success per Mboard {100*ok/N/(cost/1e6):7.2f}%",flush=True)
