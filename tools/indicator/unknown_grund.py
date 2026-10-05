"""If the Grund is unknown (free 26^4), how much does 'exists G: E_G(SEDM) = K' constrain K?"""
import sys, itertools, numpy as np
sys.path.insert(0, '/opt/enigma-p1030680/tools/indicator')
from m4ind import M4, ix
G = np.array(list(itertools.product(range(26), repeat=4)))
for key, rings in [(('B','C','438','CH EJ NV OU TY LG SZ PK DI QB'), 'VCCH'), (('C','B','568','AE BF CM DQ HU JN LX PR SZ VW'), 'EPEL')]:
    m = M4(*key)
    for V in ('SEDM', 'OEDM'):
        K = m.encipher(G, np.broadcast_to(np.array(ix(rings)), G.shape).copy(), ix(V))
        code = ((K[:, 0] * 26 + K[:, 1]) * 26 + K[:, 2]) * 26 + K[:, 3]
        u = len(np.unique(code))
        print(f'{key[2]} {V}: {u} of {26**4} message keys reachable from some Grund ({u/26**4:.3f}); '
              f'information = {-np.log2(u/26**4):.2f} bits')
