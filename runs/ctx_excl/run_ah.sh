#!/usr/bin/env bash
# run_ah.sh NAME K CRIBFILE MODE [extra bombe args]
#  MODE = --allhyp-new (complement of an earlier tools/bombe/bombe run: only the stops the first-hypothesis
#         bombe dropped) or --allhyp (fresh run). tools/blind/bombe_ah, 2 threads, raw stops to one temp file,
#         then every stop is climbed by tools/blind/blindrank (keep 1.0, R=8, 2 threads), then the raw file is deleted.
set -u
cd /opt/enigma-p1030680
EXCL=1,10,20,25,32,58,60,67,68,71
name=$1; k=$2; f=$3; mode=$4; shift 4
O=runs/ctx_excl/out
[ -f $O/$name.done ] && exit 0
args=(); while read -r l; do [[ -z "$l" || "$l" == \#* ]] && continue; args+=(--crib "$l"); done < "$f"
tools/blind/bombe_ah $mode --ct ${CT:-ciphertext.txt} "${args[@]}" --k $k --gmax $k --excl $EXCL --threads 2 "$@" \
  --out $O/$name.tsv 2> $O/$name.bombe.log || exit 1
tools/blind/blindrank rank --ct ${CT:-ciphertext.txt} --cribs $f --excl $EXCL --in $O/$name.tsv --lang corpus/ngrams/navalblend \
  --quad corpus/ngrams/navalblend_quadgrams.txt --stat llr2 --keep 1.0 --restarts 8 --k $k --surv-mm 2 --topn 300 --threads 2 \
  --out $O/$name.rank.tsv --surv $O/$name.surv.tsv 2> $O/$name.log || exit 1
rm -f $O/$name.tsv; touch $O/$name.done
