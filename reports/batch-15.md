# Batch 15 — 29 functions, a family correction, and the id namespace named

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–14 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked ELFs.

## Read this first: batches 12 and 13 have wrong counts

Batch 12 says the four-way `GetEntrances` family finished at **24/24**. Batch 13
says two-way finished at **18/18** and three-way at **9/9**. **All three are
wrong.**

My sweep matched only functions returning local `.L` tables from *every* arm.
Some return a **named global** instead — same body, same C — so they were
invisible to it. The criterion was an artefact of whichever member I read
first, and I never questioned it because the sweep kept finding members and
they kept matching.

Re-running it unconstrained found **14 more members**. Nine are elevated here;
the rest need splits.

The asymmetry is the transferable part: a sweep's false *positives* die at the
screen within minutes — one of mine produced nine bogus hits and `tryc.py`
caught them immediately. False *negatives* are silent, and this one ran for six
batches while I reported completion each time.

## The functions

| `OvlFunc_883_200806c` | `0x0200806c` | `src/overlays/rom_780898/ovl_30_a_a_a_c_b.c` |
| `OvlFunc_898_200906c` | `0x0200906c` | `src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_b.c` |
| `OvlFunc_901_20084d8` | `0x020084d8` | `src/overlays/rom_797990/ovl_314_c_c_a_a_a_c.c` |
| `OvlFunc_905_200806c` | `0x0200806c` | `src/overlays/rom_799abc/ovl_30_a_a_a_c_a_b.c` |
| `OvlFunc_907_200811c` | `0x0200811c` | `src/overlays/rom_79b154/ovl_30_c_a_a_c_a_a_a.c` |
| `OvlFunc_913_200806c` | `0x0200806c` | `src/overlays/rom_7a04ac/ovl_30_a_a_a_c_a_b.c` |
| `OvlFunc_914_200806c` | `0x0200806c` | `src/overlays/rom_7a1ff0/ovl_30_a_a_c_a_b.c` |
| `OvlFunc_915_200806c` | `0x0200806c` | `src/overlays/rom_7a2bf0/ovl_30_a_a_a_c_b.c` |
| `OvlFunc_923_2008350` | `0x02008350` | `src/overlays/rom_7aa430/ovl_314_a_c_a_b.c` |
| `OvlFunc_924_2008350` | `0x02008350` | `src/overlays/rom_7ac2d8/ovl_314_a_c_a_b.c` |
| `OvlFunc_926_200834c` | `0x0200834c` | `src/overlays/rom_7b2078/ovl_314_a_c_c.c` |
| `OvlFunc_927_200806c` | `0x0200806c` | `src/overlays/rom_7b4558/ovl_30_a_a_c_a_b.c` |
| `OvlFunc_934_2008350` | `0x02008350` | `src/overlays/rom_7bdeb0/ovl_314_a_a_a_c_b.c` |
| `OvlFunc_935_20080e0` | `0x020080e0` | `src/overlays/rom_7bf5a8/ovl_30_c_a_c.c` |
| `OvlFunc_938_2008030` | `0x02008030` | `src/overlays/rom_7c37ac/ovl_30_a.c` |
| `OvlFunc_946_200806c` | `0x0200806c` | `src/overlays/rom_7ced6c/ovl_30_a_a_a_c_b.c` |
| `OvlFunc_947_2008350` | `0x02008350` | `src/overlays/rom_7d0e88/ovl_314_a_c_a_b.c` |
| `OvlFunc_948_200806c` | `0x0200806c` | `src/overlays/rom_7d30e0/ovl_30_a_a_a_c_a_b.c` |
| `OvlFunc_948_20089f0` | `0x020089f0` | `src/overlays/rom_7d30e0/ovl_30_a_c.c` |
| `OvlFunc_948_2008a50` | `0x02008a50` | `src/overlays/rom_7d30e0/ovl_30_c_a_c_a.c` |
| `OvlFunc_948_2008ee0` | `0x02008ee0` | `src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_c_c.c` |
| `OvlFunc_957_200806c` | `0x0200806c` | `src/overlays/rom_7e3e08/ovl_30_a_a_a_c_a_b.c` |
| `OvlFunc_958_2008350` | `0x02008350` | `src/overlays/rom_7e636c/ovl_314_c_a_b.c` |
| `OvlFunc_959_200806c` | `0x0200806c` | `src/overlays/rom_7e7574/ovl_30_c_a_b.c` |
| `OvlFunc_960_200834c` | `0x0200834c` | `src/overlays/rom_7eaf28/ovl_314_a_c.c` |
| `OvlFunc_964_200806c` | `0x0200806c` | `src/overlays/rom_7ed0a0/ovl_30_a_a_a_c_a_b.c` |
| `OvlFunc_965_200806c` | `0x0200806c` | `src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_a_b.c` |
| `OvlFunc_967_200804c` | `0x0200804c` | `src/overlays/rom_7f21b8/ovl_30_c_a.c` |
| `OvlFunc_971_2009050` | `0x02009050` | `src/overlays/rom_7fb4a8/ovl_30_c_a_a_a_b.c` |

Seventeen of those are one function — `FindEntityAtPosition`, one identical
copy per overlay. `area.sym` gains 12 entries.

## `FindEntityAtPosition`: solved, and the diagnosis had been wrong

Parked for four rounds. I had recorded it as "40 instructions against 40, only
`r4` and `r1` exchanged — a register allocation problem", and spent seven
formulations on statement order and declaration shape.

**The exchange was a symptom.** The cause was one line up:

    e = *p++;      /* walking a pointer  -- table gets the callee-saved register */
    e = tbl[i];    /* indexing a base    -- table gets r1, as the ROM has it     */

Indexing makes the table live from the top of the function, which changes its
allocation priority; gcc then strength-reduces `tbl[i]` back into the ROM's own
`add r1, #0x34` / `ldmia r1!` with the registers the right way round.

Three more fell out, each exposed by the previous:

- the counter is initialised **before** the x coordinate is computed;
- each shifted comparison goes into **its own variable** — `a >>= 16` shifts in
  place, but the ROM's `asr r2, r3, #0x10` writes a *different* register, which
  is a new value rather than a modified one;
- **the counter is unsigned** — the ROM ends with `bls`, not `ble`.

The diff moved 2 → 7 → 15 → 35 → match. None of it was reachable from the
framing I had been carrying since the function was parked.

### Its annotation is wrong, and all 17 share it

    r0 = an {x, y, z} triple, r1 = the entity to skip (the caller itself).

`r1` is overwritten by `mov r1, r2` before it is ever read. There is no second
argument. Anyone starting on any of the seventeen will read that line first.

## The id namespace is named: `_ID_xx` → `_AREA_xx`

`unknown_id.sym` is now `area.sym`, 70 symbols. **ROM SHA1 unchanged**, which is
the point — an absolute symbol definition emits no bytes, so the names are free
to be wrong and free to be corrected.

A single signed halfword at `gState + 0x1C0` holds the current area. Sixty-one
functions read it to select per-area data: edge transitions, map entrances,
event tables, scripts. One id, many lookups.

**The evidence, and what it is not.** The real support is structural and read
out of the ROM: **121 of the 122 ids are compared in exactly one overlay**,
which is what "each area's code lives in one place" looks like.

The `MapEntrance_ARRAY_*` and `Events_TolbiSpring` names are **not** the
maintainer's — they arrived with the upstream tree and are an earlier
contributor's inference. I had been counting them as independent confirmation
in batches 13 and 14. They are not, and `area.sym` says so.

Two corrections to my own figures while doing this:

- The space is **70% dense over `0x10`–`0xbd`**, not "contiguous per-area runs"
  as batches 11 and 13 claim. Eleven of 23 overlays have gaps; two have clusters
  56 apart.
- There are **122 ids**, not the ~190 I reported. That count swept up unrelated
  comparisons inside large functions, including one against `0x9ffff` — a value
  a signed halfword can never equal.

Oddities a correct account should still explain: ids `0x00`–`0x0f` never appear;
52 values inside the range are unused; `0x6a` is compared in two overlays,
alone against the other 121.

## A seventh false-negative class in the screen

`tools/tryc.py` normalises `#145` vs `#0x91` and `.word -2080365184` vs
`0x84010000`, but **not literal-pool loads**: the ROM disassembly writes
`ldr r0, =1`, gcc writes `=0x1`.

That hid `OvlFunc_971_2009050`, whose nine instructions were otherwise exact —
and it was targeted at the worst possible class, since a pooled small constant
is the *pool tell* itself. Fixed; the park was re-screened after.

Worth knowing if you screen candidates from scratch paths, because the failure
mode is a clean-looking mismatch you then park.

## A measurement I nearly reported as a headline

I measured "the ROM builds an immediate before a pooled argument" at 2242 sites
and was drafting it as the largest blocker in the project. **It is not a blocker
at all** — gcc produces that shape readily whenever the pooled operand is the
*first* argument, because gcc fills `r0` last for an implicitly declared callee.
Eighteen sites in gcc's own honest output do exactly that.

What actually resists is narrower: the order among the **non-`r0`** arguments.
Both compilers fill `r0` last, which is precisely why the declaration lever
cannot reach it.

## Still open, and still only answerable by you

- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.
- **`narrow_constant`**, 34 functions, down to one peephole: gcc folds the mask
  to `sub r3, #0x10` because a `3` is live and `3 - 0x10 == ~0xc`.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
