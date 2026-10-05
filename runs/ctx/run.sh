#!/usr/bin/env bash
# run.sh NAME K CRIBFILE [extra bombe args]   -- pinned midring unless extra args override; climb-ranked
set -u
cd /opt/enigma-p1030680
name=$1; k=$2; f=$3; shift 3
O=runs/ctx/out; mkdir -p $O
[ -f $O/$name.done ] && exit 0
args=(); while read -r l; do [[ -z "$l" || "$l" == \#* ]] && continue; args+=(--crib "$l"); done < "$f"
tools/bombe/bombe --ct ciphertext.txt "${args[@]}" --k $k --gmax $k --threads ${T:-8} "$@" \
   --out $O/$name.tsv --climb corpus/ngrams/navalblend_quadgrams.txt --rank-out $O/$name.rank.tsv --topn 100 --maxhits 2000000 \
   2> $O/$name.log && touch $O/$name.done
