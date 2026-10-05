#!/usr/bin/env bash
cd /opt/enigma-p1030680/runs/r14
./run.sh R1_p179_full_k3 3 $PWD/B_p179_full.txt
./run.sh R3_p179_long_k1 1 $PWD/C_long.txt
./run.sh R2_p179_core_k0 0 $PWD/A_core.txt
touch out/batch.alldone
