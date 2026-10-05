"""How much does a KNOWN Grund + Verfahrenkenngruppe constrain a body-level candidate?

A body-level candidate = (reflector, greek, order, stecker) + the core-offset trajectory over the
72 body letters (this is everything a body decrypt can see). For every ring setting r (26^4) we
compute the message key K(r) = E_{Grund, r, stecker}(VKG) and the body trajectory it implies; we
then count how many r reproduce a given candidate's trajectory ("indicator-consistent ring sets").
"""
import sys, itertools, numpy as np, collections
sys.path.insert(0, '/opt/enigma-p1030680/tools/indicator')
from m4ind import M4, ix, s2
N = 72
R = np.array(list(itertools.product(range(26), repeat=4)), dtype=np.int64)   # all ring settings

def traj_hash(m, win, rings):
    t = m.offsets_trajectory(win, rings, N).reshape(len(win), -1).astype(np.int64)
    h = np.zeros(len(win), dtype=np.uint64)
    for j in range(t.shape[1]):
        h = h * np.uint64(1000003) + t[:, j].astype(np.uint64)
    return h

def run(label, key, G, V, true_rings, samples=4000, seed=1):
    m = M4(*key)
    G = np.array(ix(G))
    K = m.encipher(np.broadcast_to(G, R.shape).copy(), R, ix(V))           # K(r) for every ring set
    h_ind = traj_hash(m, K, R)                                                # body trajectory implied
    counts = collections.Counter(h_ind.tolist())
    tr = np.array([ix(true_rings)])
    kt = m.encipher(G[None], tr, ix(V))
    ht = int(traj_hash(m, kt, tr)[0])
    rng = np.random.default_rng(seed)
    rw = rng.integers(0, 26, (samples, 4)); kw = rng.integers(0, 26, (samples, 4))
    hw = traj_hash(m, kw, rw)
    fp = np.array([counts.get(int(x), 0) for x in hw])
    # size of the body-equivalence class (how many ring sets r, with K = c + r, give the identical
    # body trajectory) for a few random candidates: ~26*26*(#r2 not triggering a left step)*(#right-wheel notches)
    sizes = []
    for i in range(6):
        c = (kw[i:i+1] - rw[i:i+1]) % 26
        t_all = m.offsets_trajectory((c + R) % 26, R, N).reshape(len(R), -1)
        t_0 = m.offsets_trajectory(kw[i:i+1], rw[i:i+1], N).reshape(1, -1)
        sizes.append(int(np.all(t_all == t_0, axis=1).sum()))
    print(f'{label}: true key -> {s2(kt)}; ring sets consistent with TRUE trajectory: {counts[ht]}; '
          f'random wrong candidates: mean consistent ring sets {fp.mean():.4f}, '
          f'P(pass) {np.mean(fp>0):.4f}; body-equivalence class sizes of 6 random candidates: {sizes}')

P0105 = ('B', 'C', '438', 'CH EJ NV OU TY LG SZ PK DI QB')
run('Potsdam 1 May, SEDM@DGUG', P0105, 'DGUG', 'SEDM', 'VCCH')
run('Potsdam 1 May, OEDM@MNNS', P0105, 'MNNS', 'OEDM', 'VCCH')
run('Potsdam 1 May, YMUZ@IBFK', P0105, 'IBFK', 'YMUZ', 'VCCH')
run('hypothetical order 4-3-2 (single-notch right wheel)', ('B', 'C', '432', P0105[3]), 'DGUG', 'SEDM', 'VCCH')
