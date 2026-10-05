#!/usr/bin/env bash
# Naval M4 validation: P1030698 (72 letters, Potsdam 1 May 1945, b Gamma IV-III-VIII rings AACU start WQYR)
# through the production job_run.sh, leave-one-out naval blend (P1030698 excluded), 16 threads.
cd /opt/enigma-p1030680
export CT=runs/validation/p1030698.txt OUT=$PWD/runs/validation/p1030698_job/R${R:-5} THREADS=16 \
       NGDIR=$PWD/tools/run_m4/ngrams LANGNAME=navalloo RESCORE=$PWD/tools/run_m4/ngrams/navalloo_quadgrams.txt R=${R:-5}
/usr/bin/time -f "total wall %e s" tools/run_m4/job_run.sh 0002_bG438 b G 438
