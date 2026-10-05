#!/usr/bin/env python3
"""Decrypt a broken U-534 form with its known key, optionally with your own re-reading of the ciphertext.
Usage:  .venv/bin/python tools/decrypt_form.py P1030679                 # corpus ciphertext
        .venv/bin/python tools/decrypt_form.py P1030679 12=E 24=T       # override letters at positions (0-based)
        .venv/bin/python tools/decrypt_form.py P1030679 --ct FILE       # whole ciphertext from a file (letters only)
Prints plaintext in 4-letter groups and marks every position that differs from the corpus decrypt."""
import sys, json
from enigma.machine import EnigmaMachine
ms = json.load(open('corpus/messages.json')); ms = ms if isinstance(ms, list) else ms.get('messages', ms)
m = {x['id']: x for x in ms}[sys.argv[1]]
ct = list(m['ciphertext_clean'])
args = sys.argv[2:]
if args[:1] == ['--ct']:
    ct = [c for c in open(args[1]).read().upper() if c.isalpha()]; args = args[2:]
    assert len(ct) == len(m['ciphertext_clean']), f"expected {len(m['ciphertext_clean'])} letters, got {len(ct)}"
for a in args:
    p, L = a.split('='); ct[int(p)] = L.upper()
names = {'1':'I','2':'II','3':'III','4':'IV','5':'V','6':'VI','7':'VII','8':'VIII'}
mach = EnigmaMachine.from_key_sheet(
    rotors=' '.join([{'B':'Beta','G':'Gamma','C':'Gamma'}[m['greek']]] + [names[w] for w in m['wheels']]),
    reflector={'B':'B-Thin','C':'C-Thin'}[m['reflector']],
    ring_settings=' '.join(m['rings']), plugboard_settings=m['plugs'])
mach.set_display(m['wheel_positions'])
pt = mach.process_text(''.join(ct))
ref = m['plaintext_clean']
grp = lambda s: ' '.join(s[i:i+4] for i in range(0, len(s), 4))
print('CT :', grp(''.join(ct))); print('PT :', grp(pt))
diff = [i for i, (a, b) in enumerate(zip(pt, ref)) if a != b]
print('differs from corpus decrypt at positions:', diff if diff else 'none')
print('interpretation:', m.get('interpretation_de', ''))
