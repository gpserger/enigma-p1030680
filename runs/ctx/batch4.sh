#!/usr/bin/env bash
cd /opt/enigma-p1030680/runs/ctx
until [ -f out/batch3.alldone ]; do sleep 30; done
./run.sh F6_boat_tf_openers 0 $PWD/F6.txt
touch out/batch4.alldone
