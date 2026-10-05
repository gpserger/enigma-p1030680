#!/usr/bin/env bash
# calib_run.sh CASES.tsv OUTDIR [NEXTRA=1] [THREADS=1] [CASEIDX...]
# For each calibration case: bombe (k=0, pinned) over the true job + NEXTRA random other wheel orders
# (same reflector/Greek), then blindrank stats with the truth flag and a climb of the true stop(s).
set -u
ROOT=/opt/enigma-p1030680; cd $ROOT
CASES=$1; O=$2; NX=${3:-1}; T=${4:-1}; shift 4 || true
mkdir -p $O
i=0
tail -n +2 "$CASES" | while IFS=$'\t' read -r id fold refl greek wheels rings pos plugs crib loops ptext ctext; do
  if [ $# -gt 0 ] && ! printf '%s\n' "$@" | grep -qx "$i"; then i=$((i+1)); continue; fi
  n=$O/c$i; case $crib in *@*) CS=$crib;; *) CS=$crib@0;; esac; [ -f $n.stats.tsv ] && { i=$((i+1)); continue; }
  others=$(python3 -c "
import random; r=random.Random($i*7+1); s=set()
while len(s)<$NX:
  w=r.sample('12345678',3); w=''.join(w)
  if w!='$wheels': s.add(w)
print(','.join(sorted(s)))")
  /usr/bin/time -f "%e s" ${BOMBE:-tools/bombe/bombe} ${BARGS:-} --ct $ctext --crib $CS --k 0 --gmax 0 --orders $wheels,$others --refl $refl --greek $greek \
     --threads $T --out $n.bombe.tsv 2> $n.bombe.log
  case $fold in A|B) LP=tools/ctonly2/ngrams/fold$fold;; *) LP=tools/ctonly2/ngrams/$fold;; esac   # e.g. navalloo for P1030698
  tools/blind/blindrank stats --ct $ctext --crib $CS --in $n.bombe.tsv --lang $LP \
     --quad ${LP}_quadgrams.txt --truth-plugs "$plugs" --truth-pt $ptext --climb-true 4 --threads $T > $n.stats.tsv.tmp 2>> $n.bombe.log \
     && mv $n.stats.tsv.tmp $n.stats.tsv
  echo "$i $id $wheels+$others crib=$crib stops=$(($(wc -l < $n.bombe.tsv)-1)) $(tail -1 $n.bombe.log)"
  rm -f $n.bombe.tsv
  i=$((i+1))
done
