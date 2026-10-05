#!/bin/bash
cd /opt/enigma-p1030680
printf '%s\n' archives/DEFE-3-578_01.pdf archives/DEFE-3-578_02.pdf archives/DEFE-3-744.pdf archives/DEFE-3-578_03.pdf archives/DEFE-3-578_04.pdf \
 | xargs -P 3 -I{} sh -c '.venv/bin/python tools/ocr/ocr_pages.py {} 2>&1 | grep -v -i warn >> archives/ocr/log_$(basename {} .pdf).txt'
echo ALLDONE578 >> archives/ocr/ALLDONE578
