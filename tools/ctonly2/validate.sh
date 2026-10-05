#!/usr/bin/env bash
# Validation for tools/ctonly2 (research/12_ctonly2.md).
#  1. engine self-test: py-enigma-generated test cases decrypt exactly (incl. left-wheel steps)
#  2. end-to-end: for each case, the unit (true job x true right core) is searched with the key hidden;
#     reports rank of the true plaintext in the unit, wall time, and N random wrong-order units of the
#     same ciphertext (false-positive / junk-ceiling check).  CFYZR (M3, navalblend tables) and
#     P1030698 (M4, leave-one-out tables) are appended automatically; synthetic cases use held-out
#     fold tables (ngrams/foldA|foldB_*).
#  3. (optional, TRUTHCORES=1) detection probability at the true cores over 8 seeds.
# usage: validate.sh [R=40] [CASES=0,1,...] [WRONG=1] [THREADS=8]
set -eu
cd "$(dirname "$0")"
R=${R:-40}; CASES=${CASES:-0,1,2,3,4,5,6,7,8,9,32,33}; WRONG=${WRONG:-1}; THREADS=${THREADS:-8}
[ -x ct2 ] || make
g++ -O2 -march=native -std=c++17 -o /tmp/ct2_test_engine src/test_engine.cc && /tmp/ct2_test_engine | tail -1
./ct2 validate testset/synth32.tsv ngrams --R "$R" --threads "$THREADS" --stride 3 --wrong "$WRONG" --cases "$CASES"
if [ "${TRUTHCORES:-0}" = 1 ]; then ./ct2 validate testset/synth32.tsv ngrams --R "$R" --threads "$THREADS" --trials 8; fi
