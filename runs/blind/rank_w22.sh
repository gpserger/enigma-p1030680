#!/usr/bin/env bash
# Re-run of capped end_w22_k1 (research/08 step 3: WEITEREBEFEHLEABWARTEN@48-50, k=1, ~35.6M verified, 20M ranked) with bombe_ah --allhyp, streamed into llr2 filter + climb.
cd /opt/enigma-p1030680; O=runs/blind; CR=(--crib WEITEREBEFEHLEABWARTEN@48,49,50)
L=(--lang corpus/ngrams/navalblend --quad corpus/ngrams/navalblend_quadgrams.txt --stat llr2)
# pilot on 8 wheel orders x 4 (refl, Greek) = 32 of 1344 jobs -> per-crib thresholds for keep=0.10
if [ ! -s $O/w22.thr ]; then
  tools/blind/bombe_ah --allhyp --ct ciphertext.txt "${CR[@]}" --k 1 --gmax 1 --orders 123,245,367,418,526,684,738,857 --threads 2 --out $O/w22_pilot.tsv 2> $O/w22_pilot.log
  tools/blind/blindrank thr --ct ciphertext.txt --cribs $O/w22.cribs --in $O/w22_pilot.tsv "${L[@]}" --keep ${KEEP:-0.10} --thr-out $O/w22.thr --threads 2
  awk 'BEGIN{OFS="\t"} /^#/{print;next} {if($3<1000)$2=-1e30; print}' $O/w22.thr > $O/w22.thr.t && mv $O/w22.thr.t $O/w22.thr; rm -f $O/w22_pilot.tsv
fi
tools/blind/bombe_ah --allhyp --ct ciphertext.txt "${CR[@]}" --k 1 --gmax 1 --threads 1 --out - 2> $O/w22_full.bombe.log | \
  tools/blind/blindrank rank --ct ciphertext.txt --cribs $O/w22.cribs --in - "${L[@]}" --thr $O/w22.thr --restarts 8 --k 1 --surv-mm 3 \
  --topn 300 --threads 1 --out $O/w22_full.rank.tsv --surv $O/w22_full.surv.tsv 2> $O/w22_full.log
