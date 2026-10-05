#!/usr/bin/env bash
cd /opt/enigma-p1030680/runs/ctx
./run.sh T01_sender_fdua 0 $PWD/T01.txt
./run.sh F1_tf_von_fdua 0 $PWD/F1.p.txt
./run.sh F2_fdua_von_x 0 $PWD/F2.p.txt
./run.sh F3_flott_von_fdua 0 $PWD/F3.p.txt
./run.sh F4_alle 0 $PWD/F4.p.txt
./run.sh F5_flott_flott 0 $PWD/F5.p.txt
touch out/batch1.alldone
