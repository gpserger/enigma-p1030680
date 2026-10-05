#!/usr/bin/env bash
# Complement reruns with the all-hypotheses bombe (research/15 §1.3) for every family run with tools/bombe/bombe,
# weakest menus first; then the U 2356 fragments fresh with --allhyp. Sequential, 2 threads.
cd /opt/enigma-p1030680/runs/ctx_excl
./run_ah.sh A1_F1F4_k1_excl_ahnew 1 $PWD/A1_F1F4.txt --allhyp-new
./run_ah.sh B_F2_k1_excl_ahnew 1 $PWD/F2.run.txt --allhyp-new
./run_ah.sh A2_F3_k1_excl_ahnew 1 $PWD/F3.run.txt --allhyp-new
./run_ah.sh C_F5_k1_excl_ahnew 1 $PWD/F5.run.txt --allhyp-new
./run_ah.sh K_body_tail_k1_excl_ahnew 1 $PWD/K.run.txt --allhyp-new
./run_ah.sh D_dropped_k0_excl_ahnew 0 $PWD/D.run.txt --allhyp-new
if [ ! -f out/E_probe.done ]; then
  cd /opt/enigma-p1030680; args=(); while read -r l; do args+=(--crib "$l"); done < runs/ctx_excl/E.k0cand.txt
  tools/blind/bombe_ah --allhyp --ct ciphertext.txt "${args[@]}" --k 0 --gmax 0 --excl 1,10,20,25,32,58,60,67,68,71 --threads 2 \
     --orders 123,438,785 --refl B --greek g --out /dev/null 2> runs/ctx_excl/out/E_probe.log && touch runs/ctx_excl/out/E_probe.done
  cd runs/ctx_excl
fi
python3 esel.py
[ -s E.run.txt ] && ./run_ah.sh E_u2356_k0_excl_ah 0 $PWD/E.run.txt --allhyp
touch out/batch5.alldone
