#!/usr/bin/env bash
# Blind spot 2, cheap experiments at the true cores (2 threads). Outputs in runs/blind/s2/
cd /opt/enigma-p1030680/tools/ctonly2; O=../../runs/blind/s2; mkdir -p $O
B=./ct2dn; TS=testset/paired_sd.tsv; C=$(seq -s, 0 63)
# 1. per-phase single-climb success vs phase error d (80 climbs per phase), plain and masked objective
[ -s $O/phasescan_m0.tsv ] || $B phasescan $TS ngrams --R 20 --trials 4 --threads 2 --cases $C > $O/phasescan_m0.tsv
[ -s $O/phasescan_m1.tsv ] || $B phasescan $TS ngrams --R 20 --trials 4 --threads 2 --cases $C --mask 1 > $O/phasescan_m1.tsv
# 2. detection at the true cores (stage A + B, 8 seeds): baseline vs fixes at equal stage-A cost
run() { n=$1; shift; [ -s $O/$n.txt ] || $B validate "$@" --threads 2 --trials 8 > $O/$n.txt; }
for S in $TS testset/synth_dn.tsv; do t=$(basename $S .tsv)
  run tc_${t}_base   $S ngrams --R 40 --stride 3
  run tc_${t}_mask   $S ngrams --R 40 --stride 3 --mask 2
  run tc_${t}_s1eq   $S ngrams --R 40 --stride 3 --strideDN 1 --RDN 15
  run tc_${t}_r2     $S ngrams --R 40 --stride 3 --RDN 72
  run tc_${t}_maskr2 $S ngrams --R 40 --stride 3 --mask 2 --RDN 72
done
touch $O/short.done
