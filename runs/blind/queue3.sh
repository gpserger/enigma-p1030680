#!/usr/bin/env bash
cd /opt/enigma-p1030680
until [ -f runs/blind/queue2.done ]; do sleep 60; done
cd tools/ctonly2; O=../../runs/blind/s2
PS=$(seq -s, 0 4 60); PD=$(seq -s, 1 4 61)
[ -s $O/e2e_pairedS_base.txt ] || ./ct2dn validate testset/paired_sd.tsv ngrams --R 40 --stride 3 --threads 2 --wrong 0 --cases $PS > $O/e2e_pairedS_base.txt
[ -s $O/e2e_pairedD_r2.txt ]   || ./ct2dn validate testset/paired_sd.tsv ngrams --R 40 --stride 3 --RDN 72 --threads 2 --wrong 0 --cases $PD > $O/e2e_pairedD_r2.txt
[ -s $O/e2e_synthdn_r2.txt ]   || ./ct2dn validate testset/synth_dn.tsv ngrams --R 40 --stride 3 --RDN 72 --threads 2 --wrong 0 --cases $(seq -s, 0 12),14 > $O/e2e_synthdn_r2.txt
cd ../..; touch runs/blind/queue3.done
