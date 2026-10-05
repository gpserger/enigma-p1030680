#!/usr/bin/env bash
# sequential bombe runs for research/13 (4 threads, pinned middle ring)
cd /opt/enigma-p1030680
B=tools/bombe/bombe; CT=ciphertext.txt; O=runs/ultra13; Q=corpus/ngrams/navalblend_quadgrams.txt
run(){ name=$1; k=$2; f=$3; shift 3; [ -f $O/$name.done ] && return
  args=(); while read -r l; do [[ -z "$l" || "$l" == \#* ]] && continue; args+=(--crib "$l"); done < "$f"
  $B --ct $CT "${args[@]}" --k $k --gmax $((k+3)) --threads 4 "$@" --out $O/$name.tsv \
     --climb $Q --rank-out $O/$name.rank.tsv --topn 100 --maxhits 2000000 2> $O/$name.log && touch $O/$name.done; }
run R1_p082_full_off0_k2 2 $O/full_off0.txt
run R2_p082_frags_k0 0 $O/frags.txt
