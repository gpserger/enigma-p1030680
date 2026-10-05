#!/usr/bin/env bash
# Stop the run. Default: graceful (workers finish their current job, then exit).
# stop.sh now : kill immediately (in-flight right-ring units restart from scratch on resume;
#               finished units keep their .done markers).
source "$(dirname "$0")/config.sh"
touch "$OUT/stop"
if [ "${1:-}" = now ]; then
  [ -f "$OUT/pids" ] && kill $(cat "$OUT/pids") 2>/dev/null
  pkill -f -- "--topn-file $OUT/" 2>/dev/null
  rm -rf "$OUT/locks"; echo "killed"
else
  echo "graceful stop requested: workers exit after their current job (use 'stop.sh now' to kill)"
fi
