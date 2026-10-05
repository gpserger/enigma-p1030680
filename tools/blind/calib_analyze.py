#!/usr/bin/env python3
"""Analyse prefilter calibration (research/15 spot 1): for each case, where do the recoverable true stops
fall in the per-stratum distribution of each cheap statistic?  usage: calib_analyze.py DIR [DIR..]"""
import sys, glob, os, collections
import numpy as np
STATS = ['llr1', 'llr2', 'pt_bi', 'pt_quad', 'pt_ic']
FGRID = [0.005, 0.01, 0.02, 0.05, 0.1, 0.15, 0.2, 0.3, 0.4, 0.5, 0.7, 1.0]
cases = []
for d in sys.argv[1:]:
    for f in sorted(glob.glob(os.path.join(d, 'c*.stats.tsv')), key=lambda x: int(os.path.basename(x)[1:].split('.')[0])):
        rows = [l.rstrip('\n').split('\t') for l in open(f) if not l.startswith('#')]
        if not rows: cases.append(dict(name=f, n=0)); continue
        A = np.array([[float(x) for x in r[:15]] for r in rows])
        cases.append(dict(name=f, n=len(rows), A=A))
col = {'npairs': 1, 'ndet': 4, 'llr1': 6, 'llr2': 7, 'pt_bi': 8, 'pt_quad': 9, 'pt_ic': 10, 'tm': 11, 'cmm': 12, 'crest': 13, 'cmatch': 14}
need = {s: {} for s in STATS}; need_flat = {s: {} for s in STATS}
nexact = nrec = 0; rows_out = []
for c in cases:
    if c['n'] == 0: rows_out.append(f"{c['name']}: no stops"); continue
    A = c['A']; exact = A[:, col['tm']] == 72; rec = (A[:, col['cmm']] == 0) & (A[:, col['cmatch']] >= 66)
    nexact += exact.any(); nrec += rec.any()
    info = f"{os.path.basename(os.path.dirname(c['name']))}/{os.path.basename(c['name'])}: stops={c['n']} exact={int(exact.sum())} recoverable={int(rec.sum())}"
    if rec.any():
        i = np.where(rec)[0]
        info += f" rec npairs={sorted(set(A[i, col['npairs']].astype(int)))} ndet={sorted(set(A[i, col['ndet']].astype(int)))}"
        for s in STATS:
            v = A[:, col[s]]; best = 1.0; bestflat = 1.0
            for j in i:
                st = A[:, col['npairs']] == A[j, col['npairs']]
                # keep fraction needed so that stop j survives: share of its stratum with a strictly higher value, + the tie block
                frac = ((v[st] > v[j]).sum() + (v[st] == v[j]).sum()) / st.sum()
                best = min(best, frac)
                bestflat = min(bestflat, ((v > v[j]).sum() + (v == v[j]).sum()) / len(v))
            need[s][c['name']] = best; need_flat[s][c['name']] = bestflat
            info += f" {s}:{best:.3f}"
    rows_out.append(info)
print('\n'.join(rows_out))
print(f"\ncases={len(cases)}  with exact true stop={nexact}  with recoverable stop (climb R=4 gives mm=0 and >=66/72 letters)={nrec}")
for lab, N in (('per-(crib,npairs) stratum', need), ('flat (no stratum)', need_flat)):
    print(f"\nretention of recoverable cases vs keep fraction [{lab}]:")
    print('stat     ' + ' '.join(f'{f:>6}' for f in FGRID))
    for s in STATS:
        v = np.array(list(N[s].values()))
        if len(v) == 0: continue
        print(f'{s:8} ' + ' '.join(f'{(v <= f + 1e-12).mean():6.2f}' for f in FGRID) + f'   (n={len(v)}, median need {np.median(v):.3f}, 95th pct need {np.quantile(v, 0.95):.3f})')
