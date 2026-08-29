# Batch 115 — the first parallel round that paid, and a false negative I had certified as absent

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked overlay ELF with
`arm-none-eabi-nm`.

**32 functions elevated. 2434 → 2402 remaining.** This is by a wide margin the
largest batch of the project, and it is entirely a consequence of four agents
screening in parallel against filtered worklists while the build stayed
single-threaded through me.

## What landed

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_909_2008568` | `02008568` | rom_79c738 | [ovl_30_c_c_c_c_c_c_c_b.c](../src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_b.c) |
| `OvlFunc_969_20084bc` | `020084bc` | rom_7f6e64 | [ovl_314_c_a_c_a_c_a_b.c](../src/overlays/rom_7f6e64/ovl_314_c_a_c_a_c_a_b.c) |
| `OvlFunc_925_20088cc` | `020088cc` | rom_7b0400 | [ovl_314_c_c_c_a_c_a_b.c](../src/overlays/rom_7b0400/ovl_314_c_c_c_a_c_a_b.c) |
| `OvlFunc_946_2009de0` | `02009de0` | rom_7ced6c | `ovl_30_…_a_b.c` |
| `OvlFunc_946_200a004` | `0200a004` | rom_7ced6c | `ovl_30_…_a_c_b.c` |
| `OvlFunc_946_200a450` | `0200a450` | rom_7ced6c | `ovl_30_…_a_c_c_b.c` |
| `OvlFunc_907_2008cb4` | `02008cb4` | rom_79b154 | [ovl_30_c_a_c_c_a_b.c](../src/overlays/rom_79b154/ovl_30_c_a_c_c_a_b.c) |
| `OvlFunc_907_2008f3c` | `02008f3c` | rom_79b154 | [ovl_30_c_c_b.c](../src/overlays/rom_79b154/ovl_30_c_c_b.c) |
| `OvlFunc_956_2008b30` | `02008b30` | rom_7e0928 | [ovl_30_c_c_a_c_b.c](../src/overlays/rom_7e0928/ovl_30_c_c_a_c_b.c) |
| `OvlFunc_956_20085e0` | `020085e0` | rom_7e0928 | [ovl_30_c_a_c_a_b.c](../src/overlays/rom_7e0928/ovl_30_c_a_c_a_b.c) |
| `OvlFunc_968_2008e88` | `02008e88` | rom_7f2f14 | [ovl_30_c_a_c_a_a_a.c](../src/overlays/rom_7f2f14/ovl_30_c_a_c_a_a_a.c) |
| `OvlFunc_932_200b9c8` | `0200b9c8` | rom_7b9cb4 | [ovl_30_c_c_a_b.c](../src/overlays/rom_7b9cb4/ovl_30_c_c_a_b.c) |
| `OvlFunc_959_2009880` | `02009880` | rom_7e7574 | [ovl_9dc_c_a_c_a_c_c_c_c.c](../src/overlays/rom_7e7574/ovl_9dc_c_a_c_a_c_c_c_c.c) |
| `OvlFunc_947_200901c` | `0200901c` | rom_7d0e88 | [ovl_314_c_a_a_b.c](../src/overlays/rom_7d0e88/ovl_314_c_a_a_b.c) |
| `OvlFunc_905_2008a00` | `02008a00` | rom_799abc | [ovl_30_a_a_a_c_c_c_c_b.c](../src/overlays/rom_799abc/ovl_30_a_a_a_c_c_c_c_b.c) |
| `OvlFunc_916_2008b8c` | `02008b8c` | rom_7a37f0 | [ovl_30_c_c_c_a_c_a_b.c](../src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_b.c) |
| `OvlFunc_935_2008b8c` | `02008b8c` | rom_7bf5a8 | [ovl_b8c_a.c](../src/overlays/rom_7bf5a8/ovl_b8c_a.c) |
| `OvlFunc_common0_70` | `020080a0` | common0 | [common0_a_b_b.c](../src/overlays/common/common0_a_b_b.c) |
| `OvlFunc_888_200a67c` | `0200a67c` | rom_7892c8 | `ovl_30_…_a_b.c` |
| `OvlFunc_888_200b144` | `0200b144` | rom_7892c8 | `ovl_30_…_a_c_b.c` |
| `OvlFunc_943_2008514` | `02008514` | rom_7c7b9c | [ovl_30_a_a_a_c_b.c](../src/overlays/rom_7c7b9c/ovl_30_a_a_a_c_b.c) |
| `OvlFunc_921_200954c` | `0200954c` | rom_7a7298 | `ovl_30_…_c_c_b.c` |
| `OvlFunc_924_200a844` | `0200a844` | rom_7ac2d8 | [ovl_22c4_c_c_c_c_b.c](../src/overlays/rom_7ac2d8/ovl_22c4_c_c_c_c_b.c) |
| `OvlFunc_924_200ae08` | `0200ae08` | rom_7ac2d8 | [ovl_2dcc_b.c](../src/overlays/rom_7ac2d8/ovl_2dcc_b.c) |
| `OvlFunc_880_2008d74` | `02008d74` | rom_7795e8 | [ovl_30_c_c_b.c](../src/overlays/rom_7795e8/ovl_30_c_c_b.c) |
| `OvlFunc_880_2008cfc` | `02008cfc` | rom_7795e8 | [ovl_30_c_c_a_b.c](../src/overlays/rom_7795e8/ovl_30_c_c_a_b.c) |
| `OvlFunc_881_200a768` | `0200a768` | rom_77a7c8 | `ovl_30_…_a_a_b.c` |
| `OvlFunc_883_200b45c` | `0200b45c` | rom_780898 | `ovl_30_…_c_c_b.c` |
| `OvlFunc_945_2008284` | `02008284` | rom_7cb2c0 | [ovl_30_a_c_c_a_b.c](../src/overlays/rom_7cb2c0/ovl_30_a_c_c_a_b.c) |
| `OvlFunc_945_200e3ac` | `0200e3ac` | rom_7cb2c0 | `ovl_30_…_c_c_b.c` |
| `OvlFunc_945_2009978` | `02009978` | rom_7cb2c0 | `ovl_30_…_a_c_b.c` |
| `OvlFunc_899_2008080` | `02008080` | rom_794ac0 | [ovl_30_a_c_a_a_a_a_b.c](../src/overlays/rom_794ac0/ovl_30_a_c_a_a_a_a_b.c) |

## The finding that matters most: I certified a false-negative class as absent, and it was not

Batch 113 built `tools/label_false_negatives.py`, swept all 228 parks, got zero
hits, and recorded that as a **clean negative** — evidence that the
label-cascade false negative found in batch 112 was a one-off.

Four functions this batch screen DIRTY in `tools/tryc.py` at 48, 28, 25 and 26
differing lines, and are **byte-for-byte identical** to the ROM. All four are
now elevated and `make compare` is green.

The sweep was not wrong about the parks. It was wrong about its own reach, in
two ways I should have seen:

* **It only screened parks.** A false negative that fires on a function nobody
  has parked yet is invisible to it. Three of these four had never been
  attempted; the fourth was in a worklist, not a park.
* **It only flagged diffs whose FIRST differing line is a label definition.**
  That is the batch-112 signature. These four have a different one: a
  `.pool_aligned` *inside a loop body*, where the ROM's `b .LN / pool / .LN:`
  collapses to one label and gcc emits two. The first differing line is an
  ordinary instruction; the label shift appears further down.

The corrected rule, now in the doc: **any DIRTY screen whose ref contains
`.pool_aligned` inside a loop should go to a byte-level check before a single
spelling is changed.** The check is to assemble both standalone and diff the
`objdump -d` byte column with `.text` sizes — a scratch `bytecheck.sh` doing
exactly that is at `scratch/agent3/bytecheck.sh`.

The wider lesson is about how I reported the earlier result. "Swept all 228
parks: zero hits" was true and I stated it as though it settled the question.
A negative from a detector only bounds what that detector looks at, and I had
written the detector narrowly enough that it could not have found these.

## `-fno-rerun-cse-after-loop` is not free, and loops are where it costs

The doc has said for many batches to try this flag early. Three separate agents
independently measured the counterpart:

| function | default | with the flag |
|---|---|---|
| `OvlFunc_918_20097ec` | 18 differing | **45** |
| `OvlFunc_881_200a768` | **OK** | 23 differing |
| `OvlFunc_916_2008b8c` | **OK** | broken |
| `OvlFunc_888_200b144` | **OK** | broken |
| `OvlFunc_969_20084bc` | **OK** | 35 differing |

The pattern is consistent: in a loop whose base address is a `SYMBOL_REF`, the
second CSE pass is what keeps that base in one register. Removing it makes gcc
rebuild the address at every access. **A function with a real loop is where the
flag is most likely to cost a match**, so it must be measured, never assumed
neutral, and above all never adopted as a whole-file group on the strength of
one function in that file.

## Three new levers, each cheap and each measured

**Declaration order alone, with statements untouched.** `OvlFunc_956_20085e0`
went 13 of 54 → exact on swapping `int m; int n;` to `int n; int m;`. Swapping
the *assignments* instead gave 11; splitting into four locals gave 6. The doc's
"what does NOT reach it" section implied declaration order is inert once the
values are born in separate statements. It is not. One screen, so it belongs at
the top of any same-length r2↔r3 park.

**Copying the first parameter into a local swaps the two entry `mov`s.**
`OvlFunc_883_200b45c` differed only in `mov r8, r1` / `mov r6, r0` order.
`f(int first, …) { int slot = first; … }` makes gcc emit the *second*
parameter's copy first. Naming the second parameter does nothing.

**`while (1) { …; if (exit) break; i++; }` reaches strength reduction where
`for (i = 0; ; i++)` does not.** `OvlFunc_881_200a768` walks a 12-byte-record
table; the `for` form rebuilds `i*12` at every access (29 of 52 differing), the
`while` form produces the ROM's two byte-offset induction variables and matches
exactly. The two are semantically identical — only the position of the
increment relative to the `break` differs. The *second* loop in the same
function strength-reduces correctly in `for` form, so this is not about record
size; it is about the increment being reachable only through the fall-through
edge.

## One flag group added

`OvlFunc_945_2009978` needs `-fno-schedule-insns2`: at `-O2` the post-reload
scheduler hoists `mov r0,#0x8f / lsl r0,#4` above the `gState[0x22b] = 3` store
(5 of 49). `-O1` also matches but changes more than necessary. A new
`SCHED2_CFLAGS` group and an **explicit** rule — not a wildcard — is in the
Makefile for that one object.
