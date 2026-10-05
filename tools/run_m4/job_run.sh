#!/usr/bin/env bash
# Run one job = (thin reflector, greek, wheel order): stage A over all right-ring units
# (26, or 13 for double-notch VI/VII/VIII on the right), then stage B re-climb of the best keys.
# usage: job_run.sh JOBID REFL GREEK WHEELS [TIER]   (resumable: finished units have .done markers)
set -u
source "$(dirname "$0")/config.sh"
ID=$1; REFL=$2; GK=$3; W=$4; TIER=${5:-0}
if [ -z "$R" ]; then set -- $R_BY_TIER; shift "$TIER"; R=$1; fi
JD=$OUT/jobs/$ID; mkdir -p "$JD"
[ -f "$JD/DONE" ] && exit 0
case ${W:2:1} in 6|7|8) RINGS="A B C D E F G H I J K L M";; *) RINGS="A B C D E F G H I J K L M N O P Q R S T U V W X Y Z";; esac
for X in $RINGS; do
  [ -f "$JD/ring_$X.done" ] && continue
  SEED=$(( SEEDBASE * 100003 + 10#${ID:0:4} * 131 + $(printf %d "'$X") ))
  T0=$(date +%s.%N)
  "$ENIGMA" -4 -c -J --score m4f10 -f -l "$LANGNAME" -d "$NGDIR" -R "$R" -e "$SEED" -T "$THREADS" \
      -u "$REFL" -w "$GK$W" -r "AA$MIDRING$X" -g "...." \
      --topn "$TOPN_UNIT" --topn-file "$JD/ring_$X.tsv" --rescore "$RESCORE" \
      < "$CT" > /dev/null 2> "$JD/ring_$X.err"
  rc=$?
  if [ $rc -eq 0 ] && [ -f "$JD/ring_$X.tsv" ]; then
    T1=$(date +%s.%N)
    { grep -E "^Analysed|^Finished" "$JD/ring_$X.err"; echo "wall $(echo "$T1 - $T0" | bc)"; echo "R $R"; } > "$JD/ring_$X.done"
    rm -f "$JD/ring_$X.err"
  else
    echo "unit $ID ring $X failed rc=$rc" >&2; exit 1
  fi
done
# ---- stage B: re-climb the best STAGEB_K distinct keys of the job with more restarts + polish
if [ ! -f "$JD/final.tsv" ]; then
  cat "$JD"/ring_*.tsv | grep -v '^#' | sort -t$'\t' -k1,1gr | awk -F'\t' '!seen[$3" "$4" "$5]++' | head -n "$STAGEB_K" > "$JD/stageB_in.tsv"
  : > "$JD/stageB_out.tsv.tmp"
  while IFS=$'\t' read -r rs sc WW RR GG PL PT; do
    "$ENIGMA" -4 -c -J --score m4f10 --polish -f -l "$LANGNAME" -d "$NGDIR" -R "$STAGEB_R" -e 7 -T 1 \
      -u "${WW:0:1}" -w "${WW:1}" -r "$RR" -g "$GG" --topn 1 --topn-file "$JD/sb.tmp" --rescore "$RESCORE" \
      < "$CT" > /dev/null 2>&1
    B=$(grep -v '^#' "$JD/sb.tmp" | head -1)
    # keep whichever of (stage A board, stage B board) rescored higher
    if [ -n "$B" ] && awk -v a="$(echo "$B" | cut -f1)" -v b="$rs" 'BEGIN{exit !(a>b)}'; then echo "$B" >> "$JD/stageB_out.tsv.tmp";
    else printf '%s\t%s\t%s\t%s\t%s\t%s\t%s\n' "$rs" "$sc" "$WW" "$RR" "$GG" "$PL" "$PT" >> "$JD/stageB_out.tsv.tmp"; fi
  done < "$JD/stageB_in.tsv"
  rm -f "$JD/sb.tmp"
  { echo -e "#rescore\tscore\tW\tR\tG\tplugs\tplaintext"; sort -t$'\t' -k1,1gr "$JD/stageB_out.tsv.tmp" | awk -F'\t' '!seen[$7]++'; } > "$JD/final.tsv"
  rm -f "$JD/stageB_out.tsv.tmp"
fi
date -Is > "$JD/DONE"
