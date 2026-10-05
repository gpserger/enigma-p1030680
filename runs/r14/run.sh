#!/usr/bin/env bash
# run.sh NAME K CRIBFILE -- research/14 bombe runs; 2 threads, pinned middle ring, the human reader’s excl set
set -u
cd /opt/enigma-p1030680
EXCL=1,10,20,25,32,58,60,67,68,71
name=$1; k=$2; f=$3
O=runs/r14/out; mkdir -p $O
[ -f $O/$name.done ] && exit 0
args=(); while read -r l; do l=${l%%#*}; l=${l// /}; [[ -z "$l" ]] && continue; args+=(--crib "$l"); done < "$f"
tools/bombe/bombe --ct ciphertext.txt "${args[@]}" --k $k --gmax $k --excl $EXCL --threads 2 \
   --out $O/$name.tsv --climb corpus/ngrams/navalblend_quadgrams.txt --rank-out $O/$name.rank.tsv --topn 200 --maxhits 20000000 \
   2> $O/$name.log && touch $O/$name.done
