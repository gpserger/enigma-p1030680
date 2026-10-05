#!/usr/bin/env bash
# Coordinator fragments (U 2356 Kiel arrival), excl, --threads 2, after batch3. All fail the k=1 drop rule;
# the ones with >=2 loops at k=0 are probed and run at k=0.
cd /opt/enigma-p1030680/runs/ctx_excl
export T=2 MAXHITS=20000000 TOPN=200
until [ -f out/batch3.alldone ]; do sleep 60; done
TOPN=5 MAXHITS=1 ./run.sh E_probe 0 $PWD/E.k0cand.txt --orders 123,438,785 --refl B --greek g
python3 esel.py
[ -s E.run.txt ] && ./run.sh E_u2356_k0_excl 0 $PWD/E.run.txt
touch out/batch4.alldone
