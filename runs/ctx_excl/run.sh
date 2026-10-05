#!/usr/bin/env bash
# run.sh NAME K CRIBFILE [extra bombe args] -- like runs/ctx/run.sh but with the human reader’s excluded positions
set -u
cd /opt/enigma-p1030680
EXCL=${EXCL:-1,10,20,25,32,58,60,67,68,71}
name=$1; k=$2; f=$3; shift 3
O=runs/ctx_excl/out; mkdir -p $O
[ -f $O/$name.done ] && exit 0
args=(); while read -r l; do [[ -z "$l" || "$l" == \#* ]] && continue; args+=(--crib "$l"); done < "$f"
tools/bombe/bombe --ct ${CT:-ciphertext.txt} "${args[@]}" --k $k --gmax $k --excl $EXCL --threads ${T:-6} "$@" \
   --out $O/$name.tsv --climb corpus/ngrams/navalblend_quadgrams.txt --rank-out $O/$name.rank.tsv --topn ${TOPN:-100} --maxhits ${MAXHITS:-2000000} \
   2> $O/$name.log && touch $O/$name.done
