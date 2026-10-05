#!/usr/bin/env bash
# Re-rank the stored D (k=0) stops with blindrank so every mm<=2 survivor is listed per crib
# (the old top-200 list was filled entirely by the 11-letter ALLEVVVTTTFFF@0 crib). After batch5, 2 threads.
cd /opt/enigma-p1030680
until [ -f runs/ctx_excl/out/batch5.alldone ]; do sleep 60; done
O=runs/ctx_excl/out
zcat $O/D_dropped_k0_excl.tsv.gz | tools/blind/blindrank rank --ct ciphertext.txt --cribs runs/ctx_excl/D.run.txt --excl 1,10,20,25,32,58,60,67,68,71 \
  --in - --lang corpus/ngrams/navalblend --quad corpus/ngrams/navalblend_quadgrams.txt --stat llr2 --keep 1.0 --restarts 8 --k 0 --surv-mm 2 \
  --topn 300 --threads 2 --out $O/D_rerank.rank.tsv --surv $O/D_rerank.surv.tsv 2> $O/D_rerank.log && touch $O/D_rerank.done
