"""Vectorised M4 model for the Kenngruppen indicator step (Grund -> message key).

Conventions (match py-enigma / research/01 §1.5):
  window position p, ring r, core offset o = p - r (mod 26)
  forward through rotor:  out = W[(in + o) % 26] - o
  stepping before each key press: right always; middle if right window at a notch,
  or (double step) if middle window at its own notch, which also steps the left.
  Greek wheel never steps. Plugboard applied on entry and exit.
"""
import numpy as np
A = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'
ix = lambda s: [A.index(c) for c in s]
WIR = {'I':'EKMFLGDQVZNTOWYHXUSPAIBRCJ','II':'AJDKSIRUXBLHWTMCQGZNPYFVOE',
 'III':'BDFHJLCPRTXVZNYEIWGAKMUSQO','IV':'ESOVPZJAYQUIRHXLNFTGKDCMWB',
 'V':'VZBRGITYUPSDNHLXAWMJQOFECK','VI':'JPGVOUMFYQBENHZRDKASXLICTW',
 'VII':'NZJHGRCXMYSWBOUFAIVLPEKQDT','VIII':'FKQHTLXOCBJSPDZRAMEWNIUYGV',
 'Beta':'LEYJVCNIXWPBQMDRTAKZGFUHOS','Gamma':'FSOKANUERHMBTIYCWLQPZXVGJD'}
NOTCH = {'I':'Q','II':'E','III':'V','IV':'J','V':'Z','VI':'ZM','VII':'ZM','VIII':'ZM'}
REF = {'B':'ENKQAUYWJICOPBLMDXZVFTHRGS','C':'RDOBJNTKVEHMLFCWZAXGYIPSUQ'}  # thin B / thin C
ROMAN = dict(zip('12345678',['I','II','III','IV','V','VI','VII','VIII']))

class M4:
    def __init__(s, refl, greek, order, plugs):
        s.names = [{'B':'Beta','C':'Gamma'}[greek]] + [ROMAN[w] for w in order]
        s.fw = [np.array(ix(WIR[n])) for n in s.names]
        s.bw = [np.argsort(f) for f in s.fw]
        s.ref = np.array(ix(REF[refl]))
        pb = list(range(26))
        for p in plugs.split():
            a, b = ix(p); pb[a], pb[b] = b, a
        s.pb = np.array(pb)
        s.notch = [set(ix(NOTCH.get(n, ''))) for n in s.names]

    def step(s, win):
        """win: int array (...,4) window positions [greek,left,mid,right]; returns stepped copy."""
        w = win.copy()
        mid_at = np.isin(w[..., 2], list(s.notch[2]))
        right_at = np.isin(w[..., 3], list(s.notch[3]))
        w[..., 1] = np.where(mid_at, (w[..., 1] + 1) % 26, w[..., 1])
        w[..., 2] = np.where(mid_at | right_at, (w[..., 2] + 1) % 26, w[..., 2])
        w[..., 3] = (w[..., 3] + 1) % 26
        return w

    def letter(s, x, off):
        """encipher letter(s) x given core offsets off (...,4) (already stepped)."""
        x = s.pb[x]
        for k in (3, 2, 1, 0):
            x = (s.fw[k][(x + off[..., k]) % 26] - off[..., k]) % 26
        x = s.ref[x]
        for k in (0, 1, 2, 3):
            x = (s.bw[k][(x + off[..., k]) % 26] - off[..., k]) % 26
        return s.pb[x]

    def encipher(s, win, rings, text):
        """win, rings: int arrays (...,4); text: list of ints. Returns (...,len) outputs."""
        out = []
        for t in text:
            win = s.step(win)
            out.append(s.letter(np.full(win.shape[:-1], t), (win - rings) % 26))
        return np.stack(out, -1)

    def offsets_trajectory(s, win, rings, n):
        """core-offset sequence for n key presses (defines the body decryption exactly)."""
        traj = []
        for _ in range(n):
            win = s.step(win); traj.append((win - rings) % 26)
        return np.stack(traj, -2)

def s2(a): return ''.join(A[i] for i in np.ravel(a))
