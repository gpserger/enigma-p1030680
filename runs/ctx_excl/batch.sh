#!/usr/bin/env bash
# Sequential excl reruns, k=1, pinned middle ring. 4 threads, 6 if 1-min load < 10 at launch.
cd /opt/enigma-p1030680/runs/ctx_excl
export MAXHITS=20000000 TOPN=200
thr() { l=$(cut -d' ' -f1 /proc/loadavg); awk -v l=$l 'BEGIN{print (l<10)?6:4}'; }
T=$(thr) ./run.sh A1_F1F4_k1_excl 1 $PWD/A1_F1F4.txt
T=$(thr) ./run.sh A2_F3_k1_excl 1 $PWD/F3.run.txt
T=$(thr) ./run.sh B_F2_k1_excl 1 $PWD/F2.run.txt
touch out/batch.alldone
