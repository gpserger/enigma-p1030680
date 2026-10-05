#!/bin/bash
cd /opt/enigma-p1030680
printf '%s\n' archives/DEFE-3-577_3.pdf archives/DEFE-3-577_4.pdf archives/DEFE-3-743_05.pdf archives/DEFE-3-577_5.pdf archives/DEFE-3-743_04.pdf archives/DEFE-3-577_2.pdf archives/DEFE-3-577_1.pdf archives/DEFE-3-743_03.pdf archives/DEFE-3-743_02.pdf archives/DEFE-3-743_01.pdf \
 | xargs -P 3 -I{} sh -c '.venv/bin/python tools/ocr/ocr_pages.py {} 2>&1 | grep -v -i warn >> archives/ocr/log_$(basename {} .pdf).txt'
echo ALLDONE >> archives/ocr/ALLDONE
