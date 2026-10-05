"""Reproduce Girard's Grund results and the P1030680 trial decrypt, cross-checked with py-enigma."""
import sys, numpy as np
sys.path.insert(0, '/opt/enigma-p1030680/tools/indicator')
from m4ind import M4, ix, s2
from enigma.machine import EnigmaMachine
PLUGS_0105 = 'CH EJ NV OU TY LG SZ PK DI QB'
PLUGS_3004 = 'AE BF CM DQ HU JN LX PR SZ VW'
CT = open('/opt/enigma-p1030680/ciphertext.txt').read().strip()

def pye(refl, greek, order, rings, plugs, start, text):
    r = dict(zip('12345678', ['I','II','III','IV','V','VI','VII','VIII']))
    m = EnigmaMachine.from_key_sheet(rotors=[{'B':'Beta','C':'Gamma'}[greek]] + [r[c] for c in order],
        ring_settings=' '.join(rings), reflector=refl + '-Thin', plugboard_settings=plugs)
    m.set_display(start); return m.process_text(text)

cases = [  # (label, key, rings, Grund, Verfahrenkenngruppe incl. filler, expected message key)
 ('P1030680 1st try (Girard)', ('B','C','438',PLUGS_0105), 'VCCH', 'MNNS', 'OEDM', 'ELKC'),
 ('P1030680 2nd try (Girard)', ('B','C','438',PLUGS_0105), 'VCCH', 'DGUG', 'SEDM', 'PUYY'),
 ('P1030690 (Hoerenberg)',     ('B','C','438',PLUGS_0105), 'VCCH', 'IBFK', 'YMUZ', 'ODFF'),
 ('P1030660 30 Apr',           ('C','B','568',PLUGS_3004), 'EPEL', 'VGWP', 'AAYL', 'MVZQ'),
 ('P1030666 30 Apr',           ('C','B','568',PLUGS_3004), 'EPEL', 'MZAU', 'JCAP', 'TUYB'),
]
for label, (refl, greek, order, plugs), rings, G, V, exp in cases:
    m = M4(refl, greek, order, plugs)
    ours = s2(m.encipher(np.array(ix(G)), np.array(ix(rings)), ix(V)))
    ref = pye(refl, greek, order, rings, plugs, G, V)
    alt_rings = {'VCCH': 'AACU', 'EPEL': 'AAEL'}[rings]   # Hoerenberg's body-equivalent "normalised" rings
    alt = pye(refl, greek, order, alt_rings, plugs, G, V)
    print(f'{label:28s} {V}@{G} rings {rings}: ours={ours} py-enigma={ref} expected={exp} '
          f'{"OK" if ours == ref == exp else "FAIL"} | with rings {alt_rings}: {alt}')
print('Operator trial decrypt PUYY, first 6 body letters:', pye('B','C','438','VCCH',PLUGS_0105,'PUYY',CT[:6]), '(form row 3-4: IPZA YK)')
print('Operator trial decrypt ELKC, first 6 body letters:', pye('B','C','438','VCCH',PLUGS_0105,'ELKC',CT[:6]))
