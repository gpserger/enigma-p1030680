#!/usr/bin/env bash
# Rank ALL 87,019,261 stops of F4 (runs/ctx/out/F4_alle_k0.tsv) with the calibrated prefilter, 2 threads.
cd /opt/enigma-p1030680
STAT=${STAT:-llr2}; KEEP=${KEEP:-0.10}; R=${R:-8}
tools/blind/blindrank rank --ct ciphertext.txt --cribs runs/ctx/F4.p.txt --in runs/ctx/out/F4_alle_k0.tsv \
  --lang corpus/ngrams/navalblend --quad corpus/ngrams/navalblend_quadgrams.txt --stat $STAT --keep $KEEP \
  --restarts $R --k 0 --surv-mm 2 --topn 300 --threads 2 \
  --out runs/blind/F4_full.rank.tsv --surv runs/blind/F4_full.surv.tsv 2> runs/blind/F4_full.log
