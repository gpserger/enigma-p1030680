#!/usr/bin/env bash
# Runs inside the CPU window with the ct-only run stopped: rehearsal on P1030698 + decisive P1030698-crib test.
set -u
cd /opt/enigma-p1030680
B=tools/bombe/bombe; O=runs/bombe; T=16
PUB=TTTFFFZWOVIERVVVFXDXUUUXAUSBXXTRAVEMUENDEBLEIBENXWEITEREBEFEHLEATWARTKNX
C72=TTTFFFZWOVIERVVVFXDXUUUXAUSBXXTRAVEMUENDEBLEIBENXWEITEREBEFEHLEABWARTENX
C71=${C72%X}
C71B=TTTFFFZWOVIERVVVFXDXUUUAUSBXXTRAVEMUENDEBLEIBENXWEITEREBEFEHLEABWARTENX
C70=${C71B%X}
# R1: P1030698 own ciphertext, full M4 space (pinned middle ring), published text (0 garbles) + corrected text (2 natural garbles)
$B --ct $O/p98.txt --crib $PUB@0 --crib $C72@0 --k 3 --threads $T --gmax 30 --out $O/R1_p98.tsv 2> $O/R1_p98.log
# R2: P1030698 with 2 artificial ciphertext garbles (pos 9, 53), published text as crib
$B --ct $O/p98_garbled2.txt --crib $PUB@0 --k 3 --threads $T --gmax 30 --out $O/R2_p98g2.tsv 2> $O/R2_p98g2.log
# D: target, all P1030698-text variants, full M4 space incl. middle-ring turnover classes, k<=3 unknown garbles
$B --ct ciphertext.txt --crib $C72@0 --crib $C71@0,1 --crib $C71B@0,1 --crib $C70@0,1,2 --k 3 --midring all --threads $T --gmax 30 \
   --out $O/D_target_p98.tsv 2> $O/D_target_p98.log
echo finished > $O/stopwindow.done
