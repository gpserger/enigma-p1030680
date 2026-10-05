#!/usr/bin/env bash
# Start (or resume) the ctonly2 background run. Finished units (OUT/units/*.tsv) are skipped.
set -u
source "$(dirname "$0")/config.sh"
mkdir -p "$OUT/units"
if [ -f "$OUT/pid" ] && kill -0 "$(cat "$OUT/pid")" 2>/dev/null; then echo "already running (pid $(cat "$OUT/pid"))"; exit 1; fi
rm -f "$OUT/stop"
[ -f "$OUT/threads" ] || echo "$THREADS" > "$OUT/threads"
nohup nice -n 5 "$CT2" run --ct "$CT" --lang "$LANG_PREFIX" --rescore "$RESCORE" --jobs "$JOBS" --out "$OUT" \
  --threads "$(cat "$OUT/threads")" --R-tier "$R_TIER" --stride "$STRIDE" --topk "$TOPK" --refineR "$REFINE_R" --seed "$SEED" \
  > "$OUT/stdout.log" 2>&1 &
echo $! > "$OUT/pid"
{ echo "started $(date -Is) pid $(cat "$OUT/pid")"; for v in CT2 CT PASS OUT JOBS LANG_PREFIX RESCORE R_TIER STRIDE TOPK REFINE_R SEED; do echo "$v=${!v}"; done; } >> "$OUT/run_info.txt"
echo "started pid $(cat "$OUT/pid") with $(cat "$OUT/threads") threads; output in $OUT"
