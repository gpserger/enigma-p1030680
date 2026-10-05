#!/usr/bin/env bash
# k=0 fallback (with excl) for every crib dropped at k=1, --threads 2, after batch2.
cd /opt/enigma-p1030680/runs/ctx_excl
export T=2 MAXHITS=20000000 TOPN=200
until [ -f out/batch2.alldone ]; do sleep 60; done
TOPN=5 MAXHITS=1 ./run.sh D_probe 0 $PWD/D.k0cand.txt --orders 123,438,785 --refl B --greek g
python3 dsel.py
./run.sh D_dropped_k0_excl 0 $PWD/D.run.txt
touch out/batch3.alldone
