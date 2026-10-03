# Pass 3 — depinning: scope, and the policy that stops it growing

Pass 3 in the owner's roadmap is depinning. This file is its scope and the
decisions that bound it.

## Scope: 617 fakematch rows, and shimcount UNDERCOUNTS them

`fakematch.txt` carries **617 rows**. Each is a landed, byte-identical file that
reaches its bytes with a construct the project classes as a shim rather than as C.

**`tools/shimcount.py` is the pin authority and it has a known blind spot.** It
counts three classes — `register ... __asm__("rN")` pins, `__asm__(".equ ...")`
symbol declarations, and empty `__asm__ volatile("")` barriers — and **misses a
fourth entirely**: a *value-producing* inline asm such as

    __asm__ __volatile__ ("mov %0, lr" : "=l" (t))

Measured: it reports **zero shims for `GetUnit`**, which ships exactly one of
those. So every pin count taken from that tool is a **lower bound**, and the real
pass-3 scope is at least 617. Fixing shimcount is the first pass-3 task, because
the scope cannot be measured until it is fixed.

## Policy, set by the owner in batch 319

> **Prefer a pin-free body. If a landing needs pins and no pin-free landing is
> readily available, do not land it — park it at its pin-free figure and leave it
> for pass 3.**

`tools/install_batch.py` enforces the mechanical half (a landing with `pins > 0`
and `fakematch: false` is a hard error) and **warns loudly on every pinned
landing**, so the decision is taken deliberately rather than by default.

This replaces the earlier working practice, which was to take pinned landings and
record the pin-free body alongside. That practice is why the seven below exist.

## Grandfathered: the seven pinned landings from batches 317–318

These predate the policy. None of them had a pin-free *landing* available — the
pin-free alternatives were parks at 2 and 7 encodings, not matches — so there was
nothing to prefer at the time.

| function | shims | pin-free alternative |
|---|---|---|
| `GetUnit` | **1, invisible to shimcount** — a transcribed `mov r3, lr` | none; the instruction is a *dead store to a pseudo*, deleted by flow1 before local-alloc, proven unreachable on six spellings including `__builtin_return_address(0)`. Arguably a **transcription**, not a pin. |
| `Func_8005ee0` | 1 register pin | none (pin-free 22, and 4 bytes short) |
| `Func_8015e8c` | 1 register pin | **yes, at 2 of 23** — preserved at `docs/repro-b317/8015e8c_pinfree.c`, residue fully diagnosed. The cheapest depin in the tree. |
| `Func_80a65e4` | 1 `"+r"` barrier | device-free body reads 7; the transposition it closes is settled on **priority**, not a tiebreak, so no source arrangement reaches it |
| `OvlFunc_933_2009874` | 2 register pins | `{q1→r1}` alone is *exactly inert*; only the pair reaches 0 |
| `OvlFunc_921_2009fa4` | 6 register pins | none recorded |
| `OvlFunc_922_200a094` | 6 register pins | none recorded (byte twin of the above — **depin one and port**) |

### Where to start

1. **Fix `shimcount.py`'s inline-asm blind spot.** Until then the scope is unknown.
2. **`Func_8015e8c`** — a measured pin-free body already exists at 2 of 23 with its
   residue named. Closest thing to free.
3. **`OvlFunc_921_2009fa4` / `OvlFunc_922_200a094`** — 12 pins between them and they
   are byte duplicates, so a depin on one ports to the other. Note from batch 318:
   the *unpinned* `int *vp` was load-bearing there (the r5 pin cost an earlier
   repro its figure), so the pins are not uniformly removable — read the ladder in
   both headers first.
4. **Reconsider `GetUnit`'s classification.** A transcribed instruction that no C
   can produce is the `r9` static-chain class, not a pin. If the project accepts
   that distinction, the row stays but the count drops.

## The law that makes depinning tractable

From `docs/elevation.md`, and it is the reason a single-pin greedy will report a
false fixpoint:

> **cse1 unifies two pseudos holding the same `CONST_INT`, and unification needs
> TWO unpinned peers — so a pin defeats it only in company.** Group pin sites by
> the value they materialise and remove each group as a unit. Measured: 20 pins
> inert singly, 65 load-bearing together; and `pinmin.py`'s `157` on
> `OvlFunc_889_2008074` is a **lower bound, not a minimum**.

---

# Batch 320: THE CHEAPEST WORK IN PASS 3 IS DELETING PINS THAT WERE NEVER LOAD-BEARING

Before looking for a mechanism to replace a pin, check whether the pin does
anything. **Thirteen of twenty-five** small pinned landings are byte-identical
with the pin simply removed.

## Method (cheap, and the baseline is already in the tree)

`docs/elevation.md` records that **the tracked generated `.s` beside a landed `.c`
is a regression baseline** — one rebuild rather than a full `make compare`. So for
each fakematch row whose file is under ~22 non-comment lines and carries one or two
register pins: strip each `register T x __asm__("rN")` to a plain `T x`, recompile
with the production flags, and diff the normalised instruction stream against that
file's own tracked `.s`. Zero differing lines means the pin was inert.

Harness: `scratch_elev/depin/sweep.py`.

## Result: 13 free depins

| pins | file |
|---|---|
| 1 | `src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_b.c` |
| 1 | `src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_b.c` |
| 1 | `src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_c_b.c` |
| 1 | `src/overlays/rom_7a7298/ovl_30_c_c_c_c_a_a.c` |
| 1 | `src/overlays/rom_7f21b8/ovl_30_a.c` | **done, batch 320** |
| 1 | `src/overlays/rom_7fc720/ovl_30_c_a_c_a_a.c` |
| 1 | `src/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_b.c` |
| 2 | `src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_b.c` |
| 2 | `src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_b.c` |
| 2 | `src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_b.c` |
| 2 | `src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_b.c` |
| 2 | `src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_c_c_c_c_b.c` |
| 2 | `src/overlays/rom_7db0c8/ovl_30_c_c_a_a_a_c_a.c` |

Two more are nearly free at 2 differing lines (`rom_793768/ovl_314_c_c_c_a_c_a_a_c_a_c.c`,
`rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_c_c.c`).

**Why they exist:** batch 316 measured that **all 14 pins added at plain call sites
are exactly inert**, because pinning a value to the register it already occupies
creates no reload. Nobody applied that result *backwards* to the pins already in
the tree. These thirteen are that result, collected.

The sweep covered only files under 22 lines with ≤2 pins — **25 of 617 rows**.
Running it across the whole table is the obvious next step and needs no new idea.

## A CAUTION THE FIRST DEPIN EXPOSED

`src/overlays/rom_7f21b8/ovl_30_a.c` carried a pin **and** an
`__asm__ volatile ("" : : "r" (rq))` barrier. Removing the pin is byte-identical;
removing both costs 2. So the depin is real but **partial** — the file is still a
fakematch.

And after the depin, **`shimcount.py` reports nothing for it.** The surviving
barrier is *value-consuming*, not the empty `__asm__ volatile("")` form the tool
recognises, so a file with a live shim now reads as clean.

> **Depinning can make `shimcount.py` report a FALSE CLEAN.** That is the second
> blind spot found in it (the first being the value-producing
> `__asm__("mov %0, lr" : "=l"(t))` class, invisible on `GetUnit`). Fixing
> `shimcount.py` is pass 3's first task, and it is now a correctness issue and not
> only a scoping one: without the fix, progress through this list will *look* like
> it is removing shims that are still there.

## A depin that honestly failed, with the evidence upgraded in place

`src/overlays/rom_7ef4f4/ovl_30_a_c_c_a_c.c` — five lines, two pins — **does not
depin.** Its header claimed the `r0`/`r1` build order "is not reachable from plain
C here". That claim now rests on **8 source spellings and 16 flags**, all reading
the same 8 differing lines, and the mechanism is named:

The ROM materialises `-1` **three times** — `mov #1` then `neg`, because Thumb
`mov` takes only an 8-bit immediate:

    mov r0,#1 / mov r1,#1 / mov r2,#1 / mov r3,#0 / neg r0,r0 / neg r1,r1 / neg r2,r2

Unpinned, cse1 unifies all three pseudos into one materialisation plus two copies:

    mov r2,#1 / neg r2,r2 / mov r3,#0 / mov r0,r2 / mov r1,r2

Pinning **two** of the three defeats the unification, because — per the
interacting-pins law — unification needs **two unpinned peers**. Ruled out: plain
literals, three named locals, declaration-initialised locals, `~0`, mixed widths,
nested scopes, an `int v[3]` aggregate (worse, 14), mixed `0-1`/`~0`/`-1`; and
`-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-cse-skip-blocks`,
`-fno-rerun-cse-after-loop`, `-fno-expensive-optimizations`,
`-fno-schedule-insns2`, `-fno-regmove`, `-fno-force-mem`, `-fno-strength-reduce`,
`-fno-peephole`, `-fno-peephole2`, `-fno-defer-pop`, `-fno-caller-saves`,
`-fno-function-cse` (all 8) and `-fno-omit-frame-pointer` (13).

> **A park claim that survives re-measurement is worth upgrading IN PLACE, not
> merely confirming.** The next reader should inherit the sweep rather than repeat
> it — which is the whole reason this project writes figures into headers.
