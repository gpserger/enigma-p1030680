# Archive sources and how to rebuild this directory

The scans themselves are not in the repository (see `../NOTICE.md`). This file says what they are,
how to get them, and how every derived file here was made.

## The four pieces

All are Admiralty-bound teleprinter copies of Bletchley Park naval decrypts, in English, held by
The National Archives, Kew. ZTPG is the general German naval series. ZTPGU is the U-boat series.

| Piece | Former reference | Covering dates (by date of issue) | Catalogue entry |
|---|---|---|---|
| DEFE 3/577 | ZTPG 367000-367999 | 1945 Apr 29 - May 3 | <https://discovery.nationalarchives.gov.uk/details/r/C1910474> |
| DEFE 3/578 | ZTPG 368000-368999 | 1945 May 3 - 17 | <https://discovery.nationalarchives.gov.uk/details/r/C1910475> |
| DEFE 3/743 | ZTPGU 38000-38999 | 1945 Apr 9 - May 4 | <https://discovery.nationalarchives.gov.uk/details/r/C1910640> |
| DEFE 3/744 | ZTPGU 39000-39162 | 1945 May 5 - 24 | <https://discovery.nationalarchives.gov.uk/details/r/C1910641> |

Download is free with a free Discovery account: add the piece to the basket and download. Each
piece arrives as one or several PDF files. We named them as below. The checksums are of the files
we received in September 2026 and let you confirm you have the same page numbering.

| Our file name | Pages | Bytes | SHA-256 |
|---|---|---|---|
| `DEFE-3-577_1.pdf` | 195 | 41934038 | `4f9eb4bb8bf3f35f87a959888e301fe4eecc79d0ce3e7cadc7426a236d0ab3c3` |
| `DEFE-3-577_2.pdf` | 204 | 41890330 | `942f0fccdc295f4c96aaf214d5fd2d0fd4ac6dc06d14caf4913736eff789bdc7` |
| `DEFE-3-577_3.pdf` | 213 | 41992190 | `711e3e81b9266f086ca9e0b362729f0d87bb6913e7fbbc3b0921112a08cb66b7` |
| `DEFE-3-577_4.pdf` | 215 | 41983507 | `5d8a86ccb7eef45cabca64844962d527d9b31ecfcab504eb1153d47035b46909` |
| `DEFE-3-577_5.pdf` | 213 | 41499760 | `5d2436bf51a220962244729f668d9297cc94b4ec9e738d7dbd3c86164403eb8c` |
| `DEFE-3-578_01.pdf` | 243 | 149700544 | `4357671b55240b74dc50c463041669e82ec8e2c313a64fbaef040ade7f93a846` |
| `DEFE-3-578_02.pdf` | 250 | 149160001 | `879e9bae941830f432f02405270890c345d80a548121082b71fcc856c3b14622` |
| `DEFE-3-578_03.pdf` | 240 | 147706020 | `4c28924340280f31cfcffa664fe4afbb1337dd4eaccce3e7beaf2e90e47c8a0f` |
| `DEFE-3-578_04.pdf` | 238 | 146739847 | `2dabfebac26304b75407113c5dea4900000559243d3b4482b5e0d0ba724d2086` |
| `DEFE-3-743_01.pdf` | 241 | 145512297 | `1642808dddea341eb5cd762cbf7d2a2a2813fcc248516d06d13fdce0247c5172` |
| `DEFE-3-743_02.pdf` | 237 | 146828304 | `db3020418d75f8ecac4e846f4bea129a165f2b8da47df032566863ad123ba0cd` |
| `DEFE-3-743_03.pdf` | 225 | 144179563 | `850e628b6412570ba973d28917a77181f2ea774ff49c088f0fcc331473a016ca` |
| `DEFE-3-743_04.pdf` | 219 | 141127560 | `3f65b59c251d8c296edcba8a0bd40c3eebfe4e91c073fcb7854b99e379417b47` |
| `DEFE-3-743_05.pdf` | 223 | 145333984 | `bfff6b61ad9323233de1cf0adbea763676f00c76abe9eaf0bc3c81501b7fb9f6` |
| `DEFE-3-744.pdf` | 193 | 41899490 | `e1e7e501dca3f2e16fe7ad8957611102d279af3a30ad3afd87fcd4496e996215` |

Page numbers in every file here are PDF page numbers within these files, 1-based.

**The volumes are in order of issue, not of origin.** A message sent on the afternoon of 1 May 1945
can appear anywhere in the following five days of DEFE 3/578, because the new key day that began at
noon took Bletchley several days to break. Filter by date string, not by page range.

## Derived files

| File | What it is | How it was made |
|---|---|---|
| `ocr_text.tar.zst` | OCR text of all 3,349 pages, one file per page as `ocr/<volume>/<page>.txt` | `tools/ocr/ocr_pages.py`: each page rendered at 200 dpi greyscale with `pdftoppm`, read by `rapidocr-onnxruntime` 1.2.3, lines regrouped by position. Good for serial numbers, dates and keyword search. It garbles words. |
| `ocr_index.json` | Per page: serial number, T O O, and every date string the OCR found | Built from the OCR text with regular expressions. |
| `transcripts/` | 943 page transcripts: 696 pages of DEFE 3/577 (parts 3 to 5 in full and the last 55 pages of part 2, times of origin about 23 April to 2 May) and 247 pages of DEFE 3/578 and 3/744: all but two of the 95 pages carrying a 1 May date, plus neighbours and undated pages (`research/13`, `research/14`) | Vision-capable Claude models reading page images rendered at 110 to 130 dpi, with the prompt in `TRANSCRIPTION_PROMPT.md`. Mostly the small model (Haiku), with a mid-size model (Sonnet) for the pages around the morning of 1 May and for 20 repairs. At most 40 pages per task. |
| `transcripts_QA.tsv` | For each transcript: the share of the OCR text's 5-letter sequences that also occur in the transcript | `ocr_5gram_recall` near 0.7 to 0.9 is normal because the OCR itself is noisy. Values under 0.35 mean a stub, a wrong page or a nearly blank page. Use it to decide which pages to re-read. |
| `calibration/` | Eight pages transcribed independently by OCR and by three model sizes | The comparison that set the approach above. |
| `forms/` | Readings of the handwriting on the P1030680 form and its neighbours | `hand_analysis.md` explains the method. The photographs are on Hörenberg's site. |

## Transcript format

Line 1 is `META serial=... too=... toi=... freq=... from=... to=...`. Line 2 is `GIST` and one sentence.
Line 3 is blank. The page follows. `too` is the German time of origin. `toi` is GMT (German time minus
two hours) with the date. Some early transcripts put all fields on one line separated by `|`, or have
a serial such as `ADM (1)` where the model picked up the distribution marking. Search the body as well
as the META line.

These transcripts were made to find one message. They were never proofread. Where a detail matters,
go back to the scan.
