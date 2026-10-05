#!/usr/bin/env bash
cd /opt/enigma-p1030680/runs/ctx
until [ -f out/batch1.alldone ]; do sleep 30; done
./run.sh K_body_tail 0 $PWD/K2run.txt
touch out/batch2.alldone
