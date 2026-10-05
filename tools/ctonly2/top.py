#!/usr/bin/env python3
"""Inspect the ctonly2 ciphertext-only M4 run (tools/ctonly2, research/12_ctonly2.md).

  top.py                 top 30 candidates over all finished units (ranked by quadgram rescore)
  top.py -n 100          more
  top.py --by words|krah|combo   alternative rankings (naval word coverage / Krah naval trigrams / z-sum)
  top.py --job 438       only jobs whose id contains '438'
  top.py --status        progress by tier, throughput, ETA
  top.py --pass pass2    another pass directory (default pass1)
Columns: rescore = mean log10 quadgram prob (navalblend); real 72-letter naval plaintext ~ -4.2..-5.3,
per-unit junk after refinement ~ -5.6..-6.0 (see research/12). krah = Krah naval trigram mean,
words = fraction of letters covered by naval/telegraphic words.
"""
import argparse, glob, os, re, statistics, sys, time
ROOT = '/opt/enigma-p1030680'
sys.path.insert(0, f'{ROOT}/tools/run_m4')
ap = argparse.ArgumentParser()
ap.add_argument('-n', type=int, default=30)
ap.add_argument('--by', choices=['rescore', 'krah', 'words', 'combo'], default='rescore')
ap.add_argument('--job', default='')
ap.add_argument('--status', action='store_true')
ap.add_argument('--pass', dest='pas', default='pass1')
ap.add_argument('--dir', default=None)
a = ap.parse_args()
OUT = a.dir or f'{ROOT}/runs/ctonly2/{a.pas}'

def cfg():
    info = {}
    if os.path.exists(f'{OUT}/run_info.txt'):
        for l in open(f'{OUT}/run_info.txt'):
            if '=' in l: k, v = l.strip().split('=', 1); info[k] = v
    return info

def status():
    info = cfg()
    jobsf = info.get('JOBS', f'{ROOT}/runs/m4_ctonly/jobs.txt')
    jobs = [l.split() for l in open(jobsf) if not l.startswith('#')]
    stride = int(info.get('STRIDE', 3)); rt = [int(x) for x in info.get('R_TIER', '40 40 20 20 20').split()]
    def nph(w): return -(-(13 if w[2] in '678' else 26) // stride)
    done = set(os.path.basename(f)[:-4] for f in glob.glob(f'{OUT}/units/*.tsv'))
    log = []
    if os.path.exists(f'{OUT}/run.log'):
        for l in open(f'{OUT}/run.log'):
            if l.startswith('#'): continue
            m = re.match(r'(\d+) (\S+) r(\d+) tier=(\d) R=(\d+) wall=([\d.]+) thr=(\d+) settings=(\d+) evals=(\d+)', l)
            if m: log.append(m.groups())
    pid = open(f'{OUT}/pid').read().strip() if os.path.exists(f'{OUT}/pid') else None
    alive = pid and os.path.exists(f'/proc/{pid}')
    thr = open(f'{OUT}/threads').read().strip() if os.path.exists(f'{OUT}/threads') else '?'
    print(f'run dir      : {OUT}')
    print(f'process      : {"alive pid " + pid if alive else "NOT running"}   threads: {thr}   stop flag: {os.path.exists(OUT + "/stop")}')
    tiers = {}
    rem_work = {}  # remaining setting-climbs per tier
    for j in jobs:
        t = int(j[5]); tiers.setdefault(t, [0, 0]); R = rt[min(t, len(rt) - 1)]
        for r in range(26):
            tiers[t][1] += 1
            if f'{j[1]}_r{r:02d}' in done: tiers[t][0] += 1
            else: rem_work[t] = rem_work.get(t, 0) + 17576 * nph(j[4]) * R
    print(f'units done   : {len(done)}/{26 * len(jobs)}  (unit = job x right-wheel core)')
    print('by tier      : ' + '  '.join(f'T{t}:{d}/{n}' for t, (d, n) in sorted(tiers.items())))
    jd = {}
    for f in done: jd.setdefault(f.rsplit('_r', 1)[0], 0); jd[f.rsplit('_r', 1)[0]] += 1
    print(f'jobs done    : {sum(1 for v in jd.values() if v == 26)}/{len(jobs)}')
    if log:
        last = log[-200:]
        wall = sum(float(x[5]) for x in last); sc = sum(int(x[7]) * int(x[4]) for x in last)
        rate = sc / wall if wall else 0   # setting-climbs per second (wall, at current threads)
        span = int(log[-1][0]) - int(last[0][0]) + float(last[0][5])
        print(f'throughput   : {rate:,.0f} setting-climbs/s over last {len(last)} units '
              f'({sum(int(x[8]) for x in last) / wall / 1e6:.0f} M plugboard evals/s)')
        print(f'last unit    : {log[-1][1]} r{log[-1][2]} at {time.strftime("%Y-%m-%d %H:%M", time.localtime(int(log[-1][0])))}, wall {float(log[-1][5]):.0f}s')
        if rate:
            cum = 0
            for t in sorted(rem_work):
                cum += rem_work[t] / rate
                print(f'ETA tier {t}   : +{rem_work[t] / rate / 3600:7.1f} h  (cumulative {cum / 3600:7.1f} h = {cum / 86400:5.1f} d at current rate)')

WORDS = """EINS ZWO ZWEI DREI VIER FUNF FUENF SECHS SIEBEN SIBEN ACHT NEUN NULL ZEHN ELF ZWOELF HUNDERT
TAUSEND UUUU VVVV TTTT FFFF KRKR BOOT BOOTE UBOOT FLOTTILLE FLOTT GRUPPE BEFEHL BEFEHLE BEFOHLEN
MELDUNG MELDEN MELDET LIEGT LIEGEN BLEIBEN BLEIBT WEITERE WEITER ABWARTEN ERWARTE ERWARTEN STEHT
HEUTE MORGEN ABEND NACHT DATUM STANDORT POSITION QUADRAT KURS FAHRT MARSCH MARSCHFAHRT GELEIT
GEANKERT ANKER HAFEN KIEL TRAVEMUENDE LUEBECK FEHMARN GRUEN SCHWARZ WEISS NORD SUED NORDEN SUEDEN
OSTEN WESTEN KOMMANDANT CHEF ADMIRAL FUEHRER AUSBILDUNG SOFORT DRINGEND FUNK FUNKSPRUCH SCHALTUNG
TREIBSTOFF BRENNSTOFF DIESEL MUNITION TORPEDO GEGNER FEIND FLUGZEUG FLIEGER ANGRIFF ALARM SCHIFF
DAMPFER ZERSTOERER GELEITZUG EINLAUFEN AUSLAUFEN AUSGELAUFEN EINGELAUFEN AUFTRAG VERBAND KAPITAEN
LEUTNANT NORWEGEN DAENEMARK KATTEGAT OSTSEE SKAGERRAK HORTEN KOPENHAGEN FREDERIKSHAVN SWINEMUENDE
GOTENHAFEN HELA PILLAU BORNHOLM NICHT KEIN KEINE ALLE ALLES WENN DANN NOCH SCHON DURCH UEBER UNTER
EINE EINEM EINEN EINER HABEN WIRD WERDEN SIND WURDE MELDUNGEN VERLEGEN VERLEGT SPERRGEBIET MINEN
REEDE BOJE ZUSATZ FUER NACH MITTAG UHRZEIT SEKUNDE MINUTE STUNDE STUNDEN TAGE WOCHE FRUEH
KOMMANDO OBERKOMMANDO MARINE KRIEGSMARINE SEEKRIEGSLEITUNG BEFEHLSHABER FLOTTE SICHERUNG
VORPOSTEN SCHNELLBOOT MINENSUCHER GELEITBOOT TENDER WERFT WERFTLIEGEZEIT KLAR EINSATZ
GESCHWADER DIVISION ZUGEHOERIGE ANSCHLIESSEND ABGANG ANKUNFT EINTREFFEN EINGETROFFEN
""".split()
WORDS = sorted(set(w for w in WORDS if len(w) >= 4), key=len, reverse=True)
def words_cov(t):
    cov = [False] * len(t)
    for w in WORDS:
        for m in re.finditer(w, t):
            for i in range(m.start(), m.end()): cov[i] = True
    return sum(cov) / max(len(t), 1)
KRAH = {}
def krah(t):
    if not KRAH:
        for l in open(f'{ROOT}/tools/enigma-suite/dict/00trigr.cur'):
            g, v = l.split(); KRAH[g.upper()] = int(v)
    return sum(KRAH.get(t[i:i+3], 0) for i in range(len(t) - 2)) / max(len(t) - 2, 1)

def main():
    if a.status: status(); return
    rows = {}
    for f in glob.glob(f'{OUT}/units/*.tsv'):
        unit = os.path.basename(f)[:-4]
        if a.job and a.job not in unit: continue
        for l in open(f):
            if l.startswith('#'): continue
            p = l.rstrip('\n').split('\t')
            if len(p) < 5: continue
            rs = float(p[0]); pt = p[4]
            if pt not in rows or rows[pt][0] < rs: rows[pt] = (rs, unit, p[2], p[3], pt)
    if not rows: print('no finished units yet'); return
    out = [[rs, krah(pt), words_cov(pt), unit, key, plugs, pt] for rs, unit, key, plugs, pt in rows.values()]
    if a.by == 'krah': out.sort(key=lambda r: -r[1])
    elif a.by == 'words': out.sort(key=lambda r: -r[2])
    elif a.by == 'combo':
        cols = [[r[i] for r in out] for i in range(3)]
        mu = [statistics.mean(c) for c in cols]; sd = [statistics.pstdev(c) or 1 for c in cols]
        out.sort(key=lambda r: -sum((r[i] - mu[i]) / sd[i] for i in range(3)))
    else: out.sort(key=lambda r: -r[0])
    print(f'{len(out)} distinct candidate plaintexts in {OUT} (ranked by {a.by})')
    print(f'{"rescore":>8} {"krah":>7} {"words":>5}  {"unit":<17} {"key (refl greek wheels  pos GLMR  rings)":<24} {"plugs":<30} plaintext')
    for r in out[:a.n]:
        print(f'{r[0]:8.4f} {r[1]:7.0f} {r[2]:5.2f}  {r[3]:<17} {r[4]:<24} {r[5]:<30} {r[6]}')
main()
