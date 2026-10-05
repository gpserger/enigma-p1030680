# Third-party patch

`trognes-enigma-topn-exhaust.patch` applies to <https://github.com/trognes/enigma> at commit
`3bbde93373a3b0e4bfeda18edebe9113b803904a` (file `enigma.cc`). That program is licensed under
GPL-3.0, and this patch is offered under the same licence.

It adds `--topn N --topn-file F --rescore QUADFILE` (a global top-N of distinct decrypts ranked by an
independent quadgram score) and an optional exhaustive first-pair filter. See `research/07` section 1.

    git clone https://github.com/trognes/enigma tools/enigma
    cd tools/enigma && git checkout 3bbde93373a3b0e4bfeda18edebe9113b803904a
    git apply ../third_party/trognes-enigma-topn-exhaust.patch && make
