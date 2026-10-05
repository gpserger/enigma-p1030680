#!/usr/bin/env bash
# Configuration for the P1030680 M4 ciphertext-only run. Sourced by run.sh / worker.sh / job_run.sh.
# Override any variable from the environment (e.g. CT=... OUT=... for validation runs).
ROOT=/opt/enigma-p1030680
ENIGMA=${ENIGMA:-$ROOT/tools/enigma/enigma}            # trognes/enigma + --topn/--exhaust-letters patch
CT=${CT:-$ROOT/ciphertext.txt}
PASS=${PASS:-pass1}
OUT=${OUT:-$ROOT/runs/m4_ctonly/$PASS}
JOBS=${JOBS:-$ROOT/runs/m4_ctonly/jobs.txt}
WORKERS=${WORKERS:-15}
# climb model: trognes fused score (quad/tri/bi/mono + IC) in the naval-1945 blend language
NGDIR=${NGDIR:-$ROOT/corpus/ngrams}
LANGNAME=${LANGNAME:-navalblend}
# independent-ish rescore used to keep the per-unit top-N (quadgram mean log10 prob, no IC)
RESCORE=${RESCORE:-$ROOT/corpus/ngrams/navalblend_quadgrams.txt}
R_BY_TIER=${R_BY_TIER:-"20 20 10 5 5"}  # kicked restarts per rotor key (stage A) for job tiers 0..4
R=${R:-}                  # if set, overrides R_BY_TIER for every job
MIDRING=${MIDRING:-A}     # pass1: middle ring pinned (O&W shortcut); later passes: other classes
TOPN_UNIT=${TOPN_UNIT:-40}
STAGEB_K=${STAGEB_K:-1000} # per job: re-climb this many best keys ...
STAGEB_R=${STAGEB_R:-100} # ... with this many restarts + --polish
SEEDBASE=${SEEDBASE:-1}
THREADS=${THREADS:-1}     # trognes threads per unit (1 in production: WORKERS parallel jobs)
