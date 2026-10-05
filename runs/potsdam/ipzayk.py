from enigma.machine import EnigmaMachine
import itertools
ct=open('ciphertext.txt').read().strip()
keys={'30apr':('C-Thin','Beta V VI VIII',['AAEL','EPEL'],'AE BF CM DQ HU JN LX PR SZ VW'),
      '01may':('B-Thin','Gamma IV III VIII',['AACU','VCCH'],'CH EJ NV OU TY LG SZ PK DI QB'),
      '02may':('C-Thin','Gamma V II VIII',['AAFB'],'CP DG EJ FI KT LZ MS NO QU RW')}
for day,(refl,rot,rings,plugs) in keys.items():
  for rg in rings:
   for g,refl2 in [(rot,refl)]:
    for mk in ['ELKC','PUYY']:
      m=EnigmaMachine.from_key_sheet(rotors=g,reflector=refl2,ring_settings=' '.join(rg),plugboard_settings=plugs)
      m.set_display(mk); print(day,refl2,g,rg,mk,m.process_text(ct)[:20])
