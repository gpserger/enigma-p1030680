#!/usr/bin/env bash
# Keep at most one raw stop file: once a run is .done (ranked), delete probe .tsv, gzip family .tsv.
cd /opt/enigma-p1030680/runs/ctx_excl/out
while :; do
  for d in *.done; do n=${d%.done}; [ -f $n.tsv ] || continue
    case $n in *_probe) rm -f $n.tsv;; *) gzip -f $n.tsv;; esac; done
  [ -f batch4.alldone ] && exit 0; sleep 60
done
