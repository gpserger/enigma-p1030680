#!/usr/bin/env bash
# Worker loop: claim the next unfinished job in priority order (atomic mkdir lock) and run it.
set -u
source "$(dirname "$0")/config.sh"
WID=$1
mkdir -p "$OUT/jobs" "$OUT/locks"
while read -r prio id refl gk w tier; do
  [[ $prio == \#* ]] && continue
  [ -f "$OUT/stop" ] && break
  [ -f "$OUT/jobs/$id/DONE" ] && continue
  mkdir "$OUT/locks/$id" 2>/dev/null || continue
  echo "$(date -Is) worker $WID start $id" >> "$OUT/worker_$WID.log"
  if "$(dirname "$0")/job_run.sh" "$id" "$refl" "$gk" "$w" "$tier" 2>> "$OUT/worker_$WID.log"; then
    echo "$(date -Is) worker $WID done  $id" >> "$OUT/worker_$WID.log"
  else
    echo "$(date -Is) worker $WID FAIL  $id" >> "$OUT/worker_$WID.log"
  fi
  rmdir "$OUT/locks/$id" 2>/dev/null
done < "$JOBS"
echo "$(date -Is) worker $WID exit" >> "$OUT/worker_$WID.log"
