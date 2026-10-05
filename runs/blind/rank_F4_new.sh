#!/usr/bin/env bash
# F4 complement: stops the original bombe dropped because the first surviving pivot hypothesis was wrong
# (--allhyp-new writes only those), streamed into the same llr2 prefilter with per-crib thresholds from the full F4 pass.
cd /opt/enigma-p1030680; O=runs/blind
[ -s $O/F4.thr ] || tools/blind/blindrank thr --ct ciphertext.txt --cribs runs/ctx/F4.p.txt --in runs/ctx/out/F4_alle_k0.tsv \
   --lang corpus/ngrams/navalblend --stat llr2 --keep 0.10 --thr-out $O/F4.thr --threads 2 && awk 'BEGIN{OFS="\t"} /^#/{print;next} {if($3<1000)$2=-1e30; print}' $O/F4.thr > $O/F4.thr.t && mv $O/F4.thr.t $O/F4.thr
args=(); while read -r l; do [[ -z "$l" ]] && continue; args+=(--crib "$l"); done < runs/ctx/F4.p.txt
tools/blind/bombe_ah --allhyp-new --ct ciphertext.txt "${args[@]}" --k 0 --gmax 0 --threads 1 --out - 2> $O/F4_new.bombe.log | \
  tools/blind/blindrank rank --ct ciphertext.txt --cribs runs/ctx/F4.p.txt --in - --lang corpus/ngrams/navalblend \
  --quad corpus/ngrams/navalblend_quadgrams.txt --stat llr2 --thr $O/F4.thr --restarts 8 --k 0 --surv-mm 2 --topn 300 --threads 1 \
  --out $O/F4_new.rank.tsv --surv $O/F4_new.surv.tsv 2> $O/F4_new.log
