#!/usr/bin/env python3
"""Synthetic validation of tools/bombe/bombe against py-enigma.

For each case: random M4 key (random rings on all four wheels, 10 random plugs), a real 72-letter
window of naval plaintext from corpus/messages.json, encrypted with py-enigma; then `ng` random
ciphertext letters are corrupted. The bombe is run over the case's whole job (reflector x Greek x
wheel order, all 26^4 positions x right-ring phases x middle-ring classes) with the plaintext as crib
and k = 3. PASS if some reported setting decrypts the *uncorrupted* ciphertext (py-enigma, with the
reported plugboard, undetermined letters self-steckered) to the plaintext at every position whose
letters are determined. Cases are forced into the hard stepping regimes: left wheel stepping inside
the message (middle-ring turnover) and double-notch right wheel.
Usage: validate.py N [seed] [threads]
"""
import json, random, subprocess, sys, os
sys.path.insert(0, '/opt/enigma-p1030680/.venv/lib/python3.14/site-packages')
from enigma.machine import EnigmaMachine
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '../..'))
ROM = ['I', 'II', 'III', 'IV', 'V', 'VI', 'VII', 'VIII']
NOTCH = ['Q', 'E', 'V', 'J', 'Z', 'ZM', 'ZM', 'ZM']
A = lambda i: chr(65 + i % 26)

def machine(refl, greek, wh, rings, plugs):
    return EnigmaMachine.from_key_sheet(rotors=[greek] + [ROM[w] for w in wh], ring_settings=' '.join(rings),
                                        reflector=refl + '-Thin', plugboard_settings=plugs)

def enc(refl, greek, wh, rings, pos, plugs, text):
    m = machine(refl, greek, wh, rings, plugs); m.set_display(pos); return m.process_text(text)

def main():
    N = int(sys.argv[1]); rnd = random.Random(int(sys.argv[2]) if len(sys.argv) > 2 else 1)
    thr = sys.argv[3] if len(sys.argv) > 3 else '2'
    texts = [m['plaintext_clean'] for m in json.load(open(f'{ROOT}/corpus/messages.json'))
             if m.get('collection') == 'U534' and len(m.get('plaintext_clean') or '') >= 72]
    npass = 0
    for case in range(N):
        mode = ['leftstep', 'dnright', 'random'][case % 3]
        refl = rnd.choice('BC'); greek = rnd.choice(['Beta', 'Gamma'])
        while True:
            wh = rnd.sample(range(8), 3)
            if mode != 'dnright' or wh[2] >= 5: break
        rings = [A(rnd.randrange(26)) for _ in range(4)]
        pos = [A(rnd.randrange(26)) for _ in range(4)]
        if mode == 'leftstep':  # middle window 1..3 steps before its notch -> left steps within ~72 letters
            n = NOTCH[wh[1]][0]; pos[2] = A(ord(n) - 65 - rnd.randrange(0, 3))
        letters = rnd.sample([chr(65 + i) for i in range(26)], 20)
        plugs = ' '.join(letters[i] + letters[i + 1] for i in range(0, 20, 2))
        t = rnd.choice(texts); o = rnd.randrange(len(t) - 71); pt = t[o:o + 72]
        ct = enc(refl, greek, wh, ''.join(rings), ''.join(pos), plugs, pt)
        ng = rnd.randrange(4); gpos = rnd.sample(range(72), ng); cg = list(ct)
        for g in gpos: cg[g] = rnd.choice([c for c in 'ABCDEFGHIJKLMNOPQRSTUVWXYZ' if c != ct[g]])
        cg = ''.join(cg)
        order = ''.join(str(w + 1) for w in wh)
        r = subprocess.run([f'{HERE}/bombe', '--ct', cg, '--crib', pt + '@0', '--k', '3', '--orders', order,
                            '--refl', refl, '--greek', 'b' if greek == 'Beta' else 'g', '--midring', 'all',
                            '--threads', thr, '--gmax', '12'], capture_output=True, text=True)
        ok = False; best = None
        for line in r.stdout.splitlines():
            if line.startswith('#'): continue
            f = line.split('\t'); p4, r4 = f[4], f[5]; st = f[10].split(' | ')[0].strip()
            dec = enc(refl, greek, wh, r4, p4, st, ct)
            det = set(st.replace(' ', '')) | set(f[10].split('self:')[1].strip())
            bad = sum(1 for i in range(72) if dec[i] != pt[i] and ct[i] in det and pt[i] in det)
            if bad == 0: ok = True; best = (p4, r4, f[6], f[3]); break
        npass += ok
        stops = [l for l in r.stderr.splitlines() if 'histogram' in l]
        print(f"case {case} {mode:8s} {refl} {greek} {order} rings={''.join(rings)} pos={''.join(pos)} garbles={sorted(gpos)} "
              f"-> {'PASS' if ok else 'FAIL'} {best or ''} | {stops[0].split(':',1)[1].strip() if stops else ''}", flush=True)
    print(f"{npass}/{N} passed")

main()
