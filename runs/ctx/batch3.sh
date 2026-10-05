#!/usr/bin/env bash
cd /opt/enigma-p1030680/runs/ctx
until [ -f out/batch2.alldone ]; do sleep 30; done
./run.sh R_fulllength_k3 3 $PWD/R.txt
touch out/batch3.alldone
