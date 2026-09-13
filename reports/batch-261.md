# Batch 261 -- eight functions, and what one flag bug had been costing

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked ELF with `tools/checkaddr.py`.

| | |
|---|---|
| elevated | **8** |
| parks retired | 6 |
| whole-file conversions | 5 (two of them text+data) |
| splits | 1 |
| build-input changes | 0 Makefile rules |
| fakematch debt added | 3 functions |

A six-agent round; every agent closed. **All eight came out of the park pile.**

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `OvlFunc_common2_304` | `0x0200945c` | [common2_c_c_c_c_a_a.c](src/overlays/common/common2_c_c_c_c_a_a.c) |
| 2 | `Func_80974d8` | `0x080974d8` | [rom_97384_c_a_a_a.c](src/rom_8a000/rom_97384_c_a_a_a.c) |
| 3 | `Func_80babdc` | `0x080babdc` | [rom_b9b30_c_a_c_b.c](src/rom_b5000/rom_b9b30_c_a_c_b.c) |
| 4 | `OvlFunc_890_2009ca8` | `0x02009ca8` | [ovl_30_c_c_a_c_b_c.c](src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_c.c) |
| 5 | `OvlFunc_890_200a108` | `0x0200a108` | [ovl_30_c_c_a_c_b_c.c](src/overlays/rom_78b2ac/ovl_30_c_c_a_c_b_c.c) |
| 6 | `OvlFunc_common2_618` | `0x02009770` | [common2_…_c_c_a.c](src/overlays/common/common2_c_c_c_c_c_c_c_c_c_c_a.c) |
| 7 | `OvlFunc_common2_41c` | `0x02009574` | [common2_c_c_c_c_c_c_c_a.c](src/overlays/common/common2_c_c_c_c_c_c_c_a.c) |
| 8 | `OvlFunc_common2_44c` | `0x020095a4` | [common2_c_c_c_c_c_c_c_a.c](src/overlays/common/common2_c_c_c_c_c_c_c_a.c) |

## What the COMMON2 flag bug had been costing

Batch 260 found that `tryc`/`objcmp` dropped `COMMON2_CFLAGS`' `-fcall-used-r4` →
`-fcall-saved-r4` substitution, so every common2 function whose ROM pushes r4 was
screened against a prologue gcc could not emit. Four parks were affected. **All
four were targeted this round and all four closed.**

The sharpest case is `OvlFunc_common2_41c`:

> It **matched on the first candidate, untouched.** It needed no work at all and
> had been sitting parked.

The other three needed real work but their recorded numbers were all wrong:
`common2_618`'s "38 of 102" was really 48 of 100, and `common2_304`'s 52 was a
register cascade rather than the flag.

## A silent compiler hazard

Measured directly under this tree's flags:

```
return 0.0;   ->  .long 0x0, 0x0            correct
return 1.0;   ->  .long 0x3ff00000, 0x0     correct
return 2.5;   ->  .long 0xafafafaf, 0x0     GARBAGE
return -1.0;  ->  .long 0xafafafaf, 0x0     GARBAGE
return 1.5f;  ->  .word 0x80000000          GARBAGE, a DIFFERENT wrong value
```

**This gcc build cannot emit a floating literal.** Only `dconst0` and `dconst1`
survive. The agent reported all cases as `0xafafafaf`; measuring here showed SFmode
fails differently, so a grep for one pattern misses the other.

Any soft-float ROM constant must be spelled as a **64-bit integer bit pattern**
with the function typed `long long`, which pools correctly. **Swept: `afafafaf`
appears in no tracked file.** That is expected — `make compare` would have caught
it — but the failure is silent at compile time.

## A pin that miscompiles is a pin that is under-dosed

Batch 257 recorded that a pin-induced miscompile is a local minimum that cannot
reach zero. `OvlFunc_890_2009ca8` shows the other half. From 5 of 456:

| | differing |
|---|---|
| pin on the first site alone | **3 — and it MISCOMPILES** |
| pin on the second site alone | 5 (inert) |
| **both pins** | **0** |

The pair does not merely score better — it **repairs** the miscompile. With the
second zero holding its own register there is no pseudo left for gcse to common
into the first's, so the wrong-value store cannot form.

> The pin that looks best by count is the one that miscompiles, and the cure is
> another pin rather than fewer.

That is the third shape this week where **one pin has nothing to be ordered
against**, after the cheap-argument pair and repeated-constant CSE. It also
disproves the *"no basic block to put cprop at"* ceiling a **fourth** time.

## Mechanisms

**A third lever for the adjacent pair: add a dependent.** Alongside "delay the
consumer" and "promote the producer" — sink a later store into **both** arms of an
`if/else` so it becomes a dependent of the insn you want to win. Cross-jumping runs
in `.25.jump2`, *after* scheduling, so it merges the duplicated tails back: **zero
instruction cost**.

**A `goto` into a block gives ROM block order without the cost.** Duplicating a
shared tail into all three arms produces the ROM's layout via cross-jumping — but
lets `load_mems` promote a field across a loop and destroys the ROM's
per-iteration `ldr/sub/str`. One copy, entered by `goto`, gives layout *and* loop.

**The hoisted-invariant floor has a third escape: stop it being a movable.** Give
the value a real statement and loop.c never hoists it.

**The `[sp,#0]` fold is cse2, and `volatile` on the object beats it.** It replaces
a bare-REG address with its frame equivalent and leaves `(plus REG 4)` alone —
which is why offset 0 folds and other offsets do not. Eight spellings inert; the
`volatile` needs no pointer local. This unblocked a residue another park had called
unreachable.

**`volatile` for loop MEM promotion goes on the DECLARATION**, not the use site — a
boundary on the recorded rule. And the alias lever cannot reach that promotion at
all: `load_mems` compares constant offsets off one known base.

**Which arm is the THEN arm decides what stays live** — worth 82 instructions. The
fall-through arm inherits whatever the compare left live; a constant rebuilt from
scratch costs two instructions that displace every block boundary after them.

**Stack-slot literals vs named locals: the discriminator is REACH, not value.**
Literals win when one commoned pair serves a long run in one block; named locals
when each guarded block builds its own. That reconciles this batch's 133-of-182
result with batch 257's opposite finding.

## Two rules corrected by measurement

**"A LUID lever that costs instructions" may be paying for something unrelated.**
A park had recorded a lever as costing "a second pseudo and two instructions". It
costs neither — it costs a HImode-literal pool, because taking a `short[2]`'s
address moves it from an SImode pseudo to the stack and `*thumb_movhi_insn` has no
immediate alternative. Diff the generated `.s` before concluding a lever failed.

**Batch 151's offset-0/`sp` rule** now has its mechanism (`purge_addressof` plus
cse2's `canon_reg`), its true cost (**two** instructions, because the sp-based
store is provably frame memory so sched2 hoists a load above it), and its cure (a
user hard register is the one thing `canon_reg` will not substitute).

## The class-vs-count pattern

Two more parks this batch had a correct count and a wrong class. That is **ten in
four batches**, against three with a wrong number.

> The recorded count is usually reliable. The recorded *class* is not — and it is
> the class that decides which levers get tried.

## State

| | |
|---|---|
| matched | **4,314** (76.3% of the 5,655 elevatable) |
| remaining, hand-written thumb | 1,341 |
| &nbsp;&nbsp;parked | 453 |
| &nbsp;&nbsp;UNATTEMPTED | 888 |
| park files | 506, resolving to 476 distinct subjects |

Pool went **1,349 → 1,341**, exactly -8.
