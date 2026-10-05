#!/usr/bin/env bash
# Remaining excl reruns at --threads 2 (user limit), sequential. Waits for A2 (started at 6 threads before the limit).
cd /opt/enigma-p1030680/runs/ctx_excl
export T=2 MAXHITS=20000000 TOPN=200
until [ -f out/A2_F3_k1_excl.done ]; do sleep 60; done
./run.sh B_F2_k1_excl 1 $PWD/F2.run.txt
./run.sh C_F5_k1_excl 1 $PWD/F5.run.txt
TOPN=5 MAXHITS=1 ./run.sh K_probe 1 $PWD/K.keep.txt --orders 123,438,785 --refl B --greek g
python3 ksel.py
./run.sh K_body_tail_k1_excl 1 $PWD/K.run.txt
touch out/batch2.alldone
