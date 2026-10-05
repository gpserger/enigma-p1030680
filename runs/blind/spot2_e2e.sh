#!/usr/bin/env bash
# Blind spot 2 end-to-end: full unit (true job x true right core, key hidden, all 26^3 cores), rank 1 in unit.
cd /opt/enigma-p1030680/tools/ctonly2; O=../../runs/blind/s2
PD=$(seq -s, 1 4 61)
run() { n=$1; shift; [ -s $O/$n.txt ] || ./ct2dn validate "$@" --threads 2 --wrong 0 > $O/$n.txt; }
run e2e_synthdn_base  testset/synth_dn.tsv ngrams --R 40 --stride 3 --cases $(seq -s, 0 12),14
run e2e_synthdn_s1eq  testset/synth_dn.tsv ngrams --R 40 --stride 3 --strideDN 1 --RDN 15 --cases $(seq -s, 0 12),14
run e2e_pairedD_base  testset/paired_sd.tsv ngrams --R 40 --stride 3 --cases $PD
run e2e_pairedD_s1eq  testset/paired_sd.tsv ngrams --R 40 --stride 3 --strideDN 1 --RDN 15 --cases $PD
touch $O/e2e.done
