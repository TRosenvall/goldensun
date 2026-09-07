# Batch 250 — a pure triplet, a park settled by reading, and a floor that is real

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_943_200bf30` | `0x0200bf30` | [ovl_30_c_c_c_a.c](src/overlays/rom_7c7b9c/ovl_30_c_c_c_a.c) |
| 2 | `OvlFunc_943_200c218` | `0x0200c218` | [ovl_30_c_c_c_a.c](src/overlays/rom_7c7b9c/ovl_30_c_c_c_a.c) |
| 3 | `Func_809a484` | `0x0809a484` | [rom_9a44c_a_a_a_c.c](src/rom_8a000/rom_9a44c_a_a_a_c.c) |
| 4 | `OvlFunc_968_2008118` | `0x02008118` | [ovl_30_a_a_a_c_a_c_c.c](src/overlays/rom_7f2f14/ovl_30_a_a_a_c_a_c_c.c) |
| 5 | `OvlFunc_965_2008ae8` | `0x02008ae8` | [ovl_30_a_a_a_c_c_c_c_c_c.c](src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c_c_c.c) |
| 6 | `OvlFunc_933_20094b0` | `0x020094b0` | [ovl_4e4_c_c_b.c](src/overlays/rom_7bc690/ovl_4e4_c_c_b.c) |

## Parks retired

| function | address | was blocked on | broken by |
|---|---|---|---|
| `OvlFunc_943_200c218` | `0x0200c218` | "a straight-line function can never rematerialise a constant" | a call-clobbered pin, on the first screen |

## Matched but NOT landed

| function | address | why |
|---|---|---|
| `OvlFunc_933_2009874` | `0x02009874` | 48 bytes, 20 encodings, 4 relocations, verified over three runs. Fourth of four in its `.s`, and the two matched functions are **non-adjacent**, so banking it needs a five-way cut where the tool does three. Deferred rather than hand-cut at the end of a long round. Candidate is in scratch; its park stays in place until it lands. |

## Park updated, and the floor is now believed

| function | address | floor |
|---|---|---|
| `OvlFunc_968_200c2bc` | `0x0200c2bc` | **2 of 271** — and it is a *floor*, not a spelling |

Its two file-mates `OvlFunc_968_200c048` and `OvlFunc_968_200c520` are both
**matched and verified** but deliberately unlanded: `c2bc` sits in the middle of
the `.s`, so the file lands whole or not at all.

## THE TRIPLET WAS PURER THAN THE SIGNATURE PREDICTED

Entries 3–5 were selected on a twin signature — identical size, identical
symbol count, identical `hi`/`hiv`, same neighbour. A normalised diff of the
three references shows **zero differing instructions.** They differ only in
callee spelling, the `.L` table label, and the callback stored to `f6c`.

Once the first was exact, **both others matched on the first `objcmp` run by
name substitution alone.** The "pin set does not transfer between twins" hazard
from batch 249 never fired, because there was no pin set — no register pin
appears in any of the three.

The lever chain is the batch-249 mechanism-size order, one lever per class:

| class | change | result |
|---|---|---|
| — | baseline: one `_CreateActor` call, id chosen by `if`/`else` | 59 |
| **control flow** | two call sites, one per arm | **14** |
| **allocation** | narrow named temp for the second `sel` write | **8** |
| **alias** | that temp read through `*(unsigned char *)p` | **exact** |

The control-flow lever was diagnosed from a *duplicated* `mov r2, r6` in both
arms: cross-jumping merged the common suffix and stopped there, because the arms
set r0/r2 in opposite orders. Its side effect is the point — each arm gives
x/y/z a second reference, lifting them past `flags` in global-alloc priority,
which produces the ROM's r8/r10 tenants and its spill choice. Read from the
`.18.greg` dispositions, not guessed.

## ALIAS SET 0 IS A SCHEDULING DEMOTER FOR A LOAD

The recorded `rank_for_schedule` entry runs one way: give two **stores** distinct
alias sets to remove an output dependence so both reach class 3. This is the
mirror, on a **load**, in the opposite direction.

The last remaining instruction was a `mov r3, r8` the ROM puts *before* a
`ldrb r2, [r7]` and we put after:

- read as a **struct member**, the load takes the tag's alias set, is independent
  of the preceding store, reaches class 3 and wins the slot on LUID;
- read as **`*(unsigned char *)p`** it is alias set 0, carries an
  anti-dependence on that store, drops to class 2 and **loses outright** to the
  independent class-3 register move.

8 → exact. A bitfield assignment can never supply this from source order,
because `expand_assignment` always emits the RHS before the destination read.

**Standing hazard, recorded in each of the three file headers:**
`-fno-strict-aliasing` is 2 differing on these TUs. Strict aliasing is
load-bearing here, so they must never fall under an `ALIAS_CFLAGS` rule,
including by directory wildcard.

## A PARK SETTLED BY READING THE COMPILER, AND WRONG FOR YEARS

`src/non_matching/ovl_7c7b9c/200c218.c` concluded — from `local-alloc.c` — that a
straight-line function can never rematerialise a constant, and therefore that
pinning was the only way through.

**The reading is correct and incomplete.** r0–r3 are call-clobbered, so a pin
there forces the rebuild with no branch involved. It fell to a pin on the first
screen.

> Worth a sweep of every park whose reasoning is *"unreachable in straight-line
> code"*. A park justified by reading the compiler is not thereby verified.

## `hiv=1` MISLED AGAIN, IN THE SAME WAY

Third function this week selected as low-pressure that needed eviction work
anyway. `hiv` counts the **ROM's** high registers, so one means gcc has one
*more* candidate to shed. Here it commoned `0x200` into r5 across the
`__GetFlag`/`__SetFlag` pair; a single r0 pin at the **first** site killed it,
while the `__SetFlag` site takes a plain literal.

## A DIVISION IS NOT INTERCHANGEABLE WITH ITS SHIFT

And the direction is the surprising one. Writing `v[0] >> 16` in place of
`v[0] / 0x10000` is **135 of 158 and twenty bytes short** — the ROM wants the
division's rounding correction, and the shift silently drops it.

## `200c2bc` IS A FLOOR, AND I RECORDED THE MECHANISM WRONG TWICE

Worth stating plainly, because the pattern matters more than either error:

| attempt | claim | verdict |
|---|---|---|
| 1 | `base`/`z` are a GIV and a hoisted invariant | refuted |
| 2 | gcse's `insert_insn_end_bb` appends always-last | **refuted** |

The second was published in the batch-249 report and written into the park file,
so anyone reading either would have chased it. `pre_edge_insert` calls
`insert_insn_end_bb` **only** for `EDGE_ABNORMAL`; everything else goes through
`insert_insn_on_edge`, and `commit_one_edge_insertion` then chooses among three
placements. "gcse always appends last" is not a rule that exists.

**The real floor is `loop.c`'s.** `move_movables` inserts every hoisted movable
with `emit_insn_before (pat, loop_start)` in all eight branches. `loop_start` is
the `NOTE_INSN_LOOP_BEG`, and `expand_start_loop` emits that note and the loop's
top label back to back, so **no C statement can land between them.** A hoisted
invariant is always after every source-level preheader statement — and `acc = 0`
is one.

Both escapes measured shut: `strength_reduce` refuses the giv as cheap (`not
worth while, 0 vs 65`), and `check_dbra_loop` re-emits only the *comparison*
biv while the ROM compares `i`. Thirty further variants moved nothing.

> **The method that settles these: read the `-da` dumps.** Grep an insn number
> through `.03.cse` → `.07.gcse` → `.08.loop` → `.18.greg` → `.23.sched2`. It
> takes minutes; guessing from pass names cost two batches.

## LANDINGS: TWO SPLITS, ONE OF WHICH COULD NOT BE DELETED AT ALL

`rom_7c7b9c` held both its functions **and** `.data` (33 blobs) plus `.bss` (12
`.lcomm`). `split_s.py` cannot cut it — no function boundary separates the data.
Hand split, three gates:

1. `.global .L51d8` added **alone** and gated — a `.global` emits no bytes
2. split into `_a.s` (functions) / `_b.s` (`.data` + `.bss`), `overlay.ld`
   64/71/77 remapped, **every line keeping its `asm/` path** — gated with the
   functions still in assembly
3. the `.c` placed, gated again

**`.L51d8` had to be exported.** The function does `ldr r0, =.L51d8`; the label
lives in the data half and was in no `.global` block. It worked only because
reference and definition shared one object. The tell is visible at the object
level *before* the split: the reference emits `R_ARM_ABS32 .data`
(section-relative, local label) where ours emits `R_ARM_ABS32 .L51d8`
(external). Both resolve identically after linking — benign — but that is
exactly the reference that dies unexported.

## PROCESS: `git rm` FAILED A SECOND TIME, THE SAME WAY

On a file with local modifications, and again the commands after it still ran,
leaving the original `.s` on disk beside both split halves. Harmless only
because no `.ld` referenced it any more — **checked explicitly rather than
assumed**, which mattered: the basename `ovl_30_c_c_c.o` appears in sixteen
other overlays, so a loose grep "finds" references belonging to entirely
different files.

## Gate

`make clean && make -j8 && make compare` green, `goldensun.gba: OK`. Every
address checked against the linked ELF with `tools/checkaddr.py`. Pairing 4,606
sources; `--orphans` 0; `--unlinked` unchanged at 10, so no landing here added
debt.
