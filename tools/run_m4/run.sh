#!/usr/bin/env bash
# Start (or resume) the background run: WORKERS worker loops under nohup.
set -u
source "$(dirname "$0")/config.sh"
mkdir -p "$OUT"
[ -f "$JOBS" ] || python3 "$(dirname "$0")/make_jobs.py" "$JOBS"
if [ -f "$OUT/pids" ] && kill -0 $(cat "$OUT/pids") 2>/dev/null; then echo "already running (see $OUT/pids)"; exit 1; fi
rm -rf "$OUT/locks" "$OUT/stop"; mkdir -p "$OUT/locks"
: > "$OUT/pids"
for i in $(seq 1 "$WORKERS"); do
  nohup nice -n 19 ionice -c 3 "$(dirname "$0")/worker.sh" "$i" > /dev/null 2>&1 &
  echo $! >> "$OUT/pids"
done
{ echo "started $(date -Is) workers=$WORKERS"; set | grep -E "^(CT|PASS|OUT|JOBS|NGDIR|LANGNAME|RESCORE|R|R_BY_TIER|MIDRING|TOPN_UNIT|STAGEB_K|STAGEB_R|SEEDBASE)="; } > "$OUT/run_info.txt"
echo "started $WORKERS workers; output in $OUT"
