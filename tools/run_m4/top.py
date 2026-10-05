#!/usr/bin/env python3
"""Inspect the P1030680 M4 ciphertext-only run.

  top.py                 top 30 candidates over all jobs (finished: final.tsv; running: stage-A ring files)
  top.py -n 100          more
  top.py --by words      rank by naval word coverage instead of the quadgram rescore
  top.py --by combo      rank by z(rescore)+z(krah)+z(words)
  top.py --job 438       only jobs whose id contains '438'
  top.py --status        progress, throughput, ETA
  top.py --pass pass2    another pass directory (default pass1)
Columns: rescore = mean log10 quadgram prob (naval blend; higher is better; real 72-letter naval
plaintext ~ -4.8..-5.8), krah = Krah naval trigram mean (independent WWII naval corpus), words =
fraction of letters covered by known naval/telegraphic words (len>=4), job, key, plugs, text.
"""
import argparse, glob, os, re, statistics, sys, time
ROOT = '/opt/enigma-p1030680'
ap = argparse.ArgumentParser()
ap.add_argument('-n', type=int, default=30)
ap.add_argument('--by', choices=['rescore', 'krah', 'words', 'combo'], default='rescore')
ap.add_argument('--job', default='')
ap.add_argument('--status', action='store_true')
ap.add_argument('--pass', dest='pas', default='pass1')
ap.add_argument('--dir', default=None, help='explicit run dir (overrides --pass)')
a = ap.parse_args()
OUT = a.dir or f'{ROOT}/runs/m4_ctonly/{a.pas}'

def status():
    jobs = [l.split() for l in open(f'{ROOT}/runs/m4_ctonly/jobs.txt') if not l.startswith('#')]
    done = [j for j in jobs if os.path.exists(f'{OUT}/jobs/{j[1]}/DONE')]
    units_total = sum(13 if j[4][2] in '678' else 26 for j in jobs)
    walls = []; units_done = 0
    for f in glob.glob(f'{OUT}/jobs/*/ring_*.done'):
        units_done += 1
        m = re.search(r'wall ([0-9.]+)', open(f).read())
        if m: walls.append(float(m.group(1)))
    running = sorted(os.listdir(f'{OUT}/locks')) if os.path.isdir(f'{OUT}/locks') else []
    alive = 0
    if os.path.exists(f'{OUT}/pids'):
        for p in open(f'{OUT}/pids').read().split():
            alive += os.path.exists(f'/proc/{p}')
    print(f'run dir      : {OUT}')
    print(f'workers alive: {alive}   stop flag: {os.path.exists(OUT + "/stop")}')
    print(f'jobs done    : {len(done)}/{len(jobs)}   right-ring units done: {units_done}/{units_total}')
    by_tier = {}
    for j in jobs: by_tier.setdefault(j[5], [0, 0])[1] += 1
    for j in done: by_tier[j[5]][0] += 1
    print('by tier      : ' + '  '.join(f'T{t}:{d}/{n}' for t, (d, n) in sorted(by_tier.items())))
    print(f'running jobs : {" ".join(running) if running else "-"}')
    if walls:
        mw = statistics.mean(walls[-200:]); nw = max(alive, 1)
        rem = units_total - units_done
        print(f'unit wall    : mean {mw:.0f} s (last {min(len(walls),200)} units, 1 thread each)')
        print(f'ETA all jobs : {rem * mw / nw / 3600:.1f} h with {nw} workers (stage B adds ~1%)')
        t1 = [j for j in jobs if j[5] in '01' and not os.path.exists(f'{OUT}/jobs/{j[1]}/DONE')]
        rem1 = sum(13 if j[4][2] in '678' else 26 for j in t1)
        print(f'ETA tiers 0-1: {rem1 * mw / nw / 3600:.1f} h')

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
    for jd in sorted(glob.glob(f'{OUT}/jobs/*')):
        job = os.path.basename(jd)
        if a.job and a.job not in job: continue
        files = [f'{jd}/final.tsv'] if os.path.exists(f'{jd}/final.tsv') else glob.glob(f'{jd}/ring_*.tsv')
        stage = 'B' if files and files[0].endswith('final.tsv') else 'A'
        for f in files:
            try: lines = open(f).read().splitlines()
            except OSError: continue
            for l in lines:
                if l.startswith('#'): continue
                p = l.split('\t')
                if len(p) < 7: continue
                rs = float(p[0]); pt = p[6]
                if pt not in rows or rows[pt][0] < rs:
                    rows[pt] = (rs, job + stage, f'{p[2]} {p[3]} {p[4]}', p[5], pt)
    if not rows: print('no candidates yet'); return
    out = []
    for rs, job, key, plugs, pt in rows.values():
        out.append([rs, krah(pt), words_cov(pt), job, key, plugs, pt])
    if a.by == 'krah': out.sort(key=lambda r: -r[1])
    elif a.by == 'words': out.sort(key=lambda r: -r[2])
    elif a.by == 'combo':
        cols = [[r[i] for r in out] for i in range(3)]
        mu = [statistics.mean(c) for c in cols]; sd = [statistics.pstdev(c) or 1 for c in cols]
        out.sort(key=lambda r: -sum((r[i] - mu[i]) / sd[i] for i in range(3)))
    else: out.sort(key=lambda r: -r[0])
    print(f'{len(out)} distinct candidate plaintexts in {OUT}  (ranked by {a.by}; job suffix A=stage A only, B=after stage-B re-climb)')
    print(f'{"rescore":>8} {"krah":>7} {"words":>5}  {"job":<13} {"key (W R G)":<20} {"plugs":<30} plaintext')
    for r in out[:a.n]:
        print(f'{r[0]:8.4f} {r[1]:7.0f} {r[2]:5.2f}  {r[3]:<13} {r[4]:<20} {r[5]:<30} {r[6]}')

main()
