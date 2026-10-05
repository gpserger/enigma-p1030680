#!/usr/bin/env bash
cd /opt/enigma-p1030680
until [ -f runs/blind/queue1.done ]; do sleep 30; done
runs/blind/spot2_e2e.sh
runs/blind/rank_sender.sh
runs/blind/rank_F4_new.sh
touch runs/blind/queue2.done
