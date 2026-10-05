#!/usr/bin/env bash
# stop.sh      : graceful - finish the current unit (typically a few minutes), then exit
# stop.sh now  : kill immediately (the unit in flight restarts from scratch on resume)
source "$(dirname "$0")/config.sh"
touch "$OUT/stop"
if [ "${1:-}" = now ] && [ -f "$OUT/pid" ]; then kill "$(cat "$OUT/pid")" 2>/dev/null && echo "killed pid $(cat "$OUT/pid")"; else echo "graceful stop requested"; fi
