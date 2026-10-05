#!/usr/bin/env bash
# k=1 re-run of the address-block families (replaces batch1.sh after T01)
cd /opt/enigma-p1030680/runs/ctx
# (waited here for the previous batch to finish)
[ -s out/T01_sender_fdua.rank.tsv ] && touch out/T01_sender_fdua.done
./run.sh F1_tf_von_fdua_k1 1 $PWD/F1.p.txt
./run.sh F2_fdua_von_x_k1 1 $PWD/F2.p.txt
./run.sh F3_flott_von_fdua_k0 0 $PWD/F3.p.txt
./run.sh F4_alle_k0 0 $PWD/F4.p.txt
./run.sh F5_flott_flott_k0 0 $PWD/F5.p.txt
touch out/batch1.alldone
