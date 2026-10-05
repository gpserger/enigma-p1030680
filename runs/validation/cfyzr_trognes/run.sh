#!/usr/bin/env bash
# CFYZR (72-letter Army) validation: correct wheel order B 5-3-1, full 26^3 starts x 26 right rings
cd /opt/enigma-p1030680
export ENIGMA_DATA=tools/enigma/ngrams
O=runs/validation/cfyzr_trognes
for R in 1 5 20 40; do
  /usr/bin/time -f "wall=%e s" tools/enigma/enigma -c -J --score m4f10 --polish -R $R -f -l wehrmacht -u B -w 531 -r AA. -g ... -T 16 -e 7 \
    --topn 200 --topn-file $O/top_R$R.tsv --rescore tools/enigma/ngrams/wehrmacht_quadgrams.txt < runs/validation/cfyzr.txt > $O/best_R$R.txt 2> $O/log_R$R.txt
done
