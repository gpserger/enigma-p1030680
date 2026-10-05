#!/usr/bin/env bash
# Configuration for the ctonly2 ciphertext-only M4 run (sourced by run.sh / stop.sh). Override via env.
ROOT=/opt/enigma-p1030680
CT2=${CT2:-$ROOT/tools/ctonly2/ct2}
CT=${CT:-$ROOT/ciphertext.txt}
PASS=${PASS:-pass1}
OUT=${OUT:-$ROOT/runs/ctonly2/$PASS}
JOBS=${JOBS:-$ROOT/runs/m4_ctonly/jobs.txt}          # same priority order as research/07 (tiers 0..4)
LANG_PREFIX=${LANG_PREFIX:-$ROOT/corpus/ngrams/navalblend}   # climb tables (fused all-order + IC)
RESCORE=${RESCORE:-$ROOT/corpus/ngrams/navalblend_quadgrams.txt}  # ranking statistic
THREADS=${THREADS:-8}           # change live: echo N > $OUT/threads  (read at every unit start)
R_TIER=${R_TIER:-"40 40 20 20 20"}   # climbs per rotor setting, tiers 0..4
STRIDE=${STRIDE:-3}             # right-ring phase sampling stride (stage A)
TOPK=${TOPK:-20}                # stage-A survivors per unit
REFINE_R=${REFINE_R:-8}         # stage-B seeded climbs per neighbouring phase / middle class
SEED=${SEED:-1}
