#!/usr/bin/env python3
"""dec.py REFL GREEK WHEELS POS RINGS 'AB CD ..'  -> py-enigma decrypt of P1030680 (partial plugboard allowed)"""
import sys
from enigma.machine import EnigmaMachine
R={'B':'B-Thin','C':'C-Thin','B-thin':'B-Thin','C-thin':'C-Thin'}
G={'b':'Beta','g':'Gamma','Beta':'Beta','Gamma':'Gamma'}
W={'1':'I','2':'II','3':'III','4':'IV','5':'V','6':'VI','7':'VII','8':'VIII'}
refl,greek,wh,pos,rings,plugs=sys.argv[1:7]
ct=open('/opt/enigma-p1030680/ciphertext.txt').read().strip()
m=EnigmaMachine.from_key_sheet(rotors=' '.join([G[greek]]+[W[c] for c in wh]),ring_settings=' '.join(rings),
    reflector=R[refl],plugboard_settings=plugs.split('|')[0].strip())
m.set_display(pos); print(m.process_text(ct))
