#!/usr/bin/env bash
# Re-run of the capped sender_fxdx family (research/08 step 3b: 843M block stops, ~160M verified, 20M ranked),
# with the all-hypotheses bombe fix, streamed into the llr2 prefilter + climb (1 bombe thread + 1 rank thread).
cd /opt/enigma-p1030680; O=runs/blind; CR=(--crib VVVFXDXUUUAUSBXX@3,4,5,6,8,10,12,16,17,18,22,23,25 --crib VONFXDXUUUAUSBXX@3,4,5,6,8,10,12,16,17,18,19,20,23,25)
L=(--lang corpus/ngrams/navalblend --quad corpus/ngrams/navalblend_quadgrams.txt --stat llr2)
# pilot on 8 wheel orders x 4 (refl, Greek) = 32 of 1344 jobs -> per-crib thresholds for keep=0.10
if [ ! -s $O/sender.thr ]; then
  tools/blind/bombe_ah --allhyp --ct ciphertext.txt "${CR[@]}" --k 0 --gmax 0 --orders 123,245,367,418,526,684,738,857 --threads 2 --out $O/sender_pilot.tsv 2> $O/sender_pilot.log
  tools/blind/blindrank thr --ct ciphertext.txt --cribs $O/sender.cribs --in $O/sender_pilot.tsv "${L[@]}" --keep ${KEEP:-0.10} --thr-out $O/sender.thr --threads 2
  rm -f $O/sender_pilot.tsv
fi
tools/blind/bombe_ah --allhyp --ct ciphertext.txt "${CR[@]}" --k 0 --gmax 0 --threads 1 --out - 2> $O/sender_full.bombe.log | \
  tools/blind/blindrank rank --ct ciphertext.txt --cribs $O/sender.cribs --in - "${L[@]}" --thr $O/sender.thr --restarts 8 --k 0 --surv-mm 2 \
  --topn 300 --threads 1 --out $O/sender_full.rank.tsv --surv $O/sender_full.surv.tsv 2> $O/sender_full.log
