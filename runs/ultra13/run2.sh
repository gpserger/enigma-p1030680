#!/usr/bin/env bash
cd /opt/enigma-p1030680/runs/ultra13
while [ ! -f R2_p082_frags_k0.done ]; do sleep 60; done
cd /opt/enigma-p1030680
tools/bombe/bombe --ct ciphertext.txt --crib EINSZWOUHRJKIELJEINGELAUFENYFFFTTTBLEIBTBESETZT@18,20,23 --crib EINSZWOUHRKIELEINGELAUFENXFFFTTTBLEIBTBESETZT@2,20,22,25 \
  --k 1 --gmax 4 --threads 4 --out runs/ultra13/R3_tf19_k1.tsv --climb corpus/ngrams/navalblend_quadgrams.txt --rank-out runs/ultra13/R3_tf19_k1.rank.tsv --topn 100 --maxhits 2000000 \
  2> runs/ultra13/R3_tf19_k1.log && touch runs/ultra13/R3_tf19_k1.done
