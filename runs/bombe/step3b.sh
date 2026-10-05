#!/usr/bin/env bash
# Step 3b: FdU Ausb as sender after VVV/VON (P1030698 layout) at collision-free offsets 3..25, k=0, pinned, 8 threads.
cd /opt/enigma-p1030680; O=runs/bombe/step3
tools/bombe/bombe --ct ciphertext.txt --crib VVVFXDXUUUAUSBXX@3,4,5,6,8,10,12,16,17,18,22,23,25 \
  --crib VONFXDXUUUAUSBXX@3,4,5,6,8,10,12,16,17,18,19,20,23,25 --k 0 --gmax 0 --threads 8 \
  --out $O/sender_fxdx.tsv --climb corpus/ngrams/navalblend_quadgrams.txt --rank-out $O/sender_fxdx.rank2.tsv --topn 200 \
  2> $O/sender_fxdx.log && touch $O/sender_fxdx.done
