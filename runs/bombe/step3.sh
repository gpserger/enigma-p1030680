#!/usr/bin/env bash
# Step 3: next-best cribs, full wheel space, pinned middle ring, 8 threads, survivors ranked by hill climb + quadgrams.
set -u
cd /opt/enigma-p1030680
B=tools/bombe/bombe; O=runs/bombe/step3; T=${T:-8}; Q=corpus/ngrams/navalblend_quadgrams.txt
mkdir -p $O
run() { name=$1; shift; [ -f $O/$name.done ] && return
  nice -n 0 $B --ct ciphertext.txt "$@" --threads $T --out $O/$name.tsv --climb $Q --rank-out $O/$name.rank.tsv --topn 200 2> $O/$name.log && touch $O/$name.done; }
B41=TRAVEMUENDEBLEIBENXWEITEREBEFEHLEABWARTEN
W22=WEITEREBEFEHLEABWARTEN
# header cribs at the start of the message, no garbles
run hdr_fxdx   --crib FXDXUUUAUSBXX@0 --crib FXDXUUUXAUSBXX@0 --k 0 --gmax 0
run hdr_komx   --crib KOMXADMXUUUBOOTE@0 --k 0 --gmax 0
# header + ending combined (P1030698 template without the T.F. address), 1 unknown garble
GAP=$(printf '?%.0s' $(seq 1 37))
run hdr_end    --crib FXDXUUUAUSBXX${GAP}${W22}@0 --crib FXDXUUUXAUSBXX${GAP:1}${W22}@0 --k 1 --gmax 1
# ending WEITEREBEFEHLEABWARTEN near the end: k=0 at 40..50, k=1 at 48..50
run end_w22_k0 --crib $W22@40,41,42,43,44,45,46,47,48,49,50 --k 0 --gmax 0
run end_w22_k1 --crib $W22@48,49,50 --k 1 --gmax 1
# P1030698 body at every offset, up to 2 unknown garbles (+ forced collision garbles)
run body_b41   --crib $B41@0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 --k 2 --gmax 2
touch $O/ALL.done
