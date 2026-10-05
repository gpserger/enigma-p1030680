#!/usr/bin/env python3
"""p082 reconstructions: generate abridged German variants, count letters, collision-screen.
Output: variants of length 69..72 (72 minus <=3 filler) that are collision-free at offset 0."""
import itertools, sys
CT=open('/opt/enigma-p1030680/ciphertext.txt').read().strip()
def coll(c,o=0): return [o+i for i,x in enumerate(c) if x!='?' and o+i<len(CT) and CT[o+i]==x]

# --- building blocks (corpus spellings) ---
PRIO   = ['', 'SSD']
ADDR_FLOTT = ['ZWOFUENFXUNDZWOSECHSXUUUFLOTTXX','ZWOFUNFXZWOSECHSXUUUFLOTTXX','ZWOFUENFUNDZWOSECHSUUUFLOTTXX',
              'ZWOFUNFXUNDZWOSECHSXUUUFLOTTXX','ZWOFUENFXYZWOSECHSXUUUFLOTTXX']
ADDR_DIRECT = ['JOTTOWUENSCHEJYFANGBOOTEXX','OTTOWUENSCHEYFANGBOOTEXX','JOTTOWUENSCHEJXX','OTTOWUENSCHEXX']
SIGN   = ['', 'FXDXUUUAUSB', 'VVVFXDXUUUAUSB', 'XFXDXUUUAUSB','XXFXDXUUUAUSB']
VERB_A = ['SICHERSTELLENUNDMELDENDASS','SICHERSTELLENUNDBESTAETIGENDASS','SICHERSTELLENUNDMELDEN','SICHERSTELLEN']
SUBJ   = ['JOTTOWUENSCHEJUNDBEFOHLENEFANGBOOTE','OTTOWUENSCHEUNDBEFOHLENEFANGBOOTE','JOTTOWUENSCHEJUNDFANGBOOTE',
          'OTTOWUENSCHEUNDFANGBOOTE','JOTTOWUENSCHEJUNDTTTFFFBOOTE']
OBJ    = ['NORWEGENKARTENANBORDHABEN','NORWEGENKARTENANBORD','KARTENNORWEGENANBORD','KARTENAUSRUESTUNGNORWEGENANBORD',
          'STELLKARTENNORWEGENANBORD']
# direct-to-ship short bodies
DBODY  = ['NORWEGENKARTENANBORDNEHMENXMELDEN','NORWEGENKARTENANBORDNEHMENXBESTAETIGEN','KARTENAUSRUESTUNGNORWEGENANBORDNEHMEN',
          'NORWEGENKARTENSICHERSTELLENXBESTAETIGUNGERBETEN','SOFORTNORWEGENKARTENANBORDNEHMENXMELDEN',
          'NORWEGENKARTENANBORDXMELDEN','STELLKARTENAUSRUESTUNGNORWEGENANBORDNEHMEN']
out=set()
def add(s,fam):
    if 66<=len(s)<=72: out.add((s,fam))
for p,a,v,s,o,g in itertools.product(PRIO,ADDR_FLOTT,VERB_A,SUBJ,OBJ,SIGN):
    body = v+s+o if v.endswith('DASS') else s+o+'X'+v if v!='SICHERSTELLEN' else v+s+o
    add(p+a+body+('X' if g and not g.startswith(('X','V')) else '')+g,'FLOTT')
    # verb-last telegraphese: subj obj SICHERSTELLEN X MELDEN
    add(p+a+s+o.replace('HABEN','')+'SICHERSTELLENXMELDEN'+g,'FLOTT-vlast')
for p,a,b,g in itertools.product(PRIO,ADDR_DIRECT,DBODY,SIGN):
    add(p+a+b+g,'DIRECT')
    add(p+a+'VVVFXDXUUUAUSBXX'+b,'DIRECT-vvvhead')
res=[]
for s,fam in sorted(out):
    for o in range(0,72-len(s)+1):
        if not coll(s,o): res.append((s,o,fam))
print('generated',len(out),'len-ok variants; collision-free (variant,offset) pairs:',len(res),file=sys.stderr)
for s,o,fam in res: print(f'{len(s)}\t{o}\t{fam}\t{s}')
