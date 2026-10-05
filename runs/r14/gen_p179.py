import itertools, sys
sys.path.insert(0,'.')
from gen14 import coll, clean, EXCL
ADDR=['','AGRUFRONT','AGRUXFRONT','AGRUFRONTBORNHOLM','AGRUFRONTXBORNHOLM','AGRUXFRONTXBORNHOLM']
SND=['','VONVONFXDXUUUAUSB','VVVFXDXUUUAUSB']
SEP=['','X','XX']
NOUN=['UEBERFUEHRUNG','VERLEGUNG']
SHIP=['JPETERVONDANZIGJ','PETERVONDANZIG']
VERB=['BESTAETIGEN','BESTAETIGT','WIRDBESTAETIGT','GENEHMIGT']
TAIL=['AUSLAUFENMELDEN']
out=[]
for a,s,sp,n,sh,v,sp2 in itertools.product(ADDR,SND,SEP,NOUN,SHIP,VERB,SEP):
    if not a and not s and sp: continue
    head=a+s+(sp if (a or s) else '')
    for body in [n+sh+'NACHWARNEMUENDE'+v+sp2+'AUSLAUFENMELDEN',
                 ('BESTAETIGE' if v.startswith('BEST') else v)+n+sh+'NACHWARNEMUENDE'+sp2+'AUSLAUFENMELDEN']:
        c=head+body
        if 60<=len(c)<=72: out.append(c)
out=sorted(set(out))
ok=[c for c in out if clean(c,0)]
print(f'# {len(out)} variants 60-72, {len(ok)} clean at offset 0', file=sys.stderr)
for c in ok: print(f'{c}@0  # {len(c)} coll={coll(c,0)}')
