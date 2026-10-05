# Run outputs

Each directory holds the scripts, crib lists, logs and ranked results of one line of
attack. Raw bombe stop lists and calibration statistics larger than 200 kB were left out;
the sweep's state archive and top lists in `m4_ctonly/` are the only larger files kept.
`OMITTED_LARGE_FILES.tsv` lists every omitted file with its size in bytes, and each can
be regenerated with the script beside where it used to be.

| Directory | Attack | Note |
|---|---|---|
| `validation/` | Ciphertext-only pipeline checked on two real 72-letter messages | `research/07` section 2 |
| `potsdam/` | The three known Potsdam daily keys tried with every message key | `research/07` section 3 |
| `bombe/` | P1030698 re-broadcast test and the first short cribs | `research/08` |
| `ctx/` | Context-derived crib families (`gen.py`, `cc.py` collision test, `prune.py`, batches, `out/`) | `research/11` sections 3 to 6 |
| `ultra13/` | German reconstructions of DEFE 3/577 decrypts | `research/13` sections 5 and 6 |
| `r14/` | The same for DEFE 3/578 | `research/14` sections 3 and 4 |
| `ctx_excl/` | The address-block families, some body/tail phrases and the U 2356 fragments again with the ten doubtful letters excluded and the fixed bombe | `research/11` section 7 |
| `blind/` | Calibration of the stop prefilter, complete rankings of the flooded families, double-notch experiments | `research/15` |
| `m4_ctonly/` | The ciphertext-only sweep | below |

## The ciphertext-only sweep (`m4_ctonly/`)

| File | Content |
|---|---|
| `jobs.txt` | The 1,344 jobs in priority order: reflector, Greek wheel, wheel order, tier. Tier 0 is the three wheel orders seen in 1945 Potsdam keys, tier 1 the other orders with VIII on the right, tier 2 VIII elsewhere, tier 3 orders with VI or VII, tier 4 the rest. |
| `run_info.txt` | The configuration: 20, 20, 10, 5, 5 restarts per key for tiers 0 to 4, middle ring pinned, naval blend scoring. |
| `final_status.txt` | Where it stopped: 424 jobs done, tiers 0 and 1 complete, tier 2 at 256 of 336. |
| `top3000_by_rescore.txt`, `top500_by_combo.txt` | The best candidate decrypts with their keys. None reads as German. |
| `pass1_state.tar.zst` | The unit completion markers of every job started; for the 424 finished jobs the merged candidate list (`final.tsv`); and for the 32 jobs that were in progress, the candidate lists of their 273 finished units (`ring_*.tsv`), so that resuming loses nothing. Per-unit lists of finished jobs were not kept. |

To resume the sweep where it stopped:

```bash
mkdir -p runs/m4_ctonly/pass1 && zstd -dc runs/m4_ctonly/pass1_state.tar.zst | tar -C runs/m4_ctonly/pass1 -xf -
# build the patched trognes/enigma first (tools/README.md), then:
WORKERS=8 tools/run_m4/run.sh          # skips every finished unit and job
python3 tools/run_m4/top.py --status
```

The runner skips any unit with a `ring_*.done` marker and builds a job's second stage from the unit
lists on disk. The 32 jobs that were in progress when the sweep stopped therefore come with the lists
of their 273 finished units, and resume correctly.

Read `research/07` section 6 before interpreting the result. A finished job without a hit
does not exclude its wheel order: the measured chance of finding a 72-letter message in
its correct job is about 20 to 35% at 20 restarts and about 10% at 5. Re-running finished
jobs with a different `SEEDBASE` and more restarts is as legitimate as running new ones.
The sweep ran partly on a 16-thread desktop and partly on a rented 32-vCPU machine. One
right-ring unit of 456,976 keys took mostly 35 to 65 minutes (median 52) on one thread at 20
restarts, and about 31 minutes at 10.
