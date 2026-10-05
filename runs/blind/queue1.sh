#!/usr/bin/env bash
cd /opt/enigma-p1030680
while pgrep -x blindrank >/dev/null; do sleep 30; done
runs/blind/spot2_short.sh
BOMBE=tools/blind/bombe_ah BARGS=--allhyp tools/blind/calib_run.sh runs/blind/calib_w.tsv runs/blind/cal_w_ah 1 2 > runs/blind/cal_w_ah.log 2>&1
touch runs/blind/queue1.done
