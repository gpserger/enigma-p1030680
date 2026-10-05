#!/usr/bin/env python3
"""Generate spelling-variant cribs, filter by no-self-encipherment. Prints 'CRIB@OFF' lines.
Usage: gen.py FAMILY  (see FAMILIES dict)"""
import itertools, sys
sys.path.insert(0,'/opt/enigma-p1030680/runs/ctx')
from cc import coll, CT
N = {'1':['EINS'],'2':['ZWO'],'3':['DREI'],'4':['VIER','VIR'],'5':['FUENF','FUNF'],
     '6':['SECHS'],'7':['SIEBEN','SIBEN'],'8':['ACHT'],'9':['NEUN'],'0':['NUL','NULL']}
def num(s):  # spelled digits, all variant combos
    out=['']
    for d in s: out=[o+v for o in out for v in N[d]]
    return out
def expand(parts):
    """parts: list of lists of alternatives -> all concatenations"""
    return [''.join(p) for p in itertools.product(*parts)]
VON=['VONVON','VVV']
FDUA=['FXDXUUUAUSB','FXDXUUUXAUSB']
def flott(n): return [a+b+'UUUFLOTT' for a in num(n) for b in ['X','']]
TF=lambda n: ['TTTFFF'+x for x in num(n)]
SEP=['XX','X']
FAM={}
# F1: T.F. n  von FdU Ausb  (P1030698 form)
FAM['F1']=[c for n in ['15','17','18','19','23','24','21','22'] for c in expand([TF(n),VON,FDUA,['XX']])]
# F2: FdU Ausb von <sender>
senders=TF('18')+TF('19')+TF('24')+TF('17')+flott('5')+flott('25')+flott('26')+flott('4')
FAM['F2']=[c for c in expand([FDUA,VON,senders,SEP])]
# F3: <flotilla> von FdU Ausb
recips=flott('5')+flott('25')+flott('26')+flott('4')+['CHEF'+x for x in flott('5')]
FAM['F3']=expand([recips,['X',''],VON,FDUA,['XX']])
# F4: An alle / Alle  von <sender>  (Kiel alarm: sender 5. U-Flott; 10. Sidi; FdU Ausb)
asend=flott('5')+FDUA+['ZEHNXSIDI','ZEHNXSIDIX','FXDXUUUOST','KOMXADMXUUUBOOTE','FUNKLEITUNG']
FAM['F4']=expand([['ANALLE','ALLE','ANALLEUUUBOOTE','ALLEUUUBOOTE','ALLEBOOTE','ANALLEBOOTE'],VON,asend,['XX','X']])
# F5: <flotilla/FdU> von 5./26. flotilla etc (inter-flotilla, P1030711/PDF 1516 form)
FAM['F5']=expand([flott('26')+flott('25')+FDUA+flott('4'),['X',''],VON,flott('5'),['XX','X']])+\
          expand([flott('5'),['X',''],VON,flott('26')+flott('25')+flott('4'),['XX','X']])
if __name__=='__main__':
    fam=sys.argv[1]; off=int(sys.argv[2]) if len(sys.argv)>2 else 0
    maxlen=int(sys.argv[3]) if len(sys.argv)>3 else 99
    seen=set(); n=0; bad=0
    for c in FAM[fam]:
        c=c[:maxlen]
        if c in seen: continue
        seen.add(c)
        if coll(c,off): bad+=1; continue
        print(f'{c}@{off}'); n+=1
    print(f'# {fam}: {n} clean, {bad} collide', file=sys.stderr)
