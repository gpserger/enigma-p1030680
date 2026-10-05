import random, subprocess, sys, time
E='tools/enigma/enigma'; os_env={'ENIGMA_DATA':'tools/enigma/ngrams'}
P=[l.strip() for l in open('runs/validation/bench/plaintexts.txt') if len(l.strip())>=72]
R=int(sys.argv[1]); N=int(sys.argv[2]); extra=sys.argv[3:] 
rng=random.Random(5); ok=0; tt=0
for t in range(N):
    pl=rng.choice(P); o=rng.randrange(len(pl)-71); pt=pl[o:o+72]
    refl=rng.choice('bc'); g=rng.choice('BG'); w=rng.sample('12345678',3)
    ring='A'+'A'+rng.choice('ABCDEFGHIJKLMNOPQRSTUVWXYZ')+rng.choice('ABCDEFGHIJKLMNOPQRSTUVWXYZ')
    st=''.join(rng.choice('ABCDEFGHIJKLMNOPQRSTUVWXYZ') for _ in range(4))
    l=list('ABCDEFGHIJKLMNOPQRSTUVWXYZ'); rng.shuffle(l); plugs=' '.join(l[2*i]+l[2*i+1] for i in range(10))
    base=[E,'-4','-u',refl,'-w',g+''.join(w),'-r',ring,'-g',st]
    ct=subprocess.run(base+['-s',plugs],input=pt,capture_output=True,text=True,env=os_env).stdout.strip()
    t0=time.time()
    out=subprocess.run(base+['-c','-J','--score','m4f10','--polish','-R',str(R),'-f','-l','wehrmacht','-T','16']+extra,input=ct,capture_output=True,text=True,env=os_env).stdout.strip()
    tt+=time.time()-t0
    good=sum(a==b for a,b in zip(out,pt))>=69; ok+=good
print(f"trognes true-key R={R}: success {ok}/{N}  avg wall {tt/N:.3f}s/key (16 threads)")
