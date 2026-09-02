# Batch 180

Ten functions. **Nine were parked**, and every one of those parks had reached a
"not reachable from source" conclusion. One of them was mine, written two
batches ago, and its central claim was wrong.

This was also the first batch screened by parallel subagents, at the user's
request. Nine of the ten were screened that way; every match was re-verified
here by `make compare`, which remains the only authority.

## Function breakdown

| # | function | address | file | previously | what it took |
|---|---|---|---|---|---|
| 1 | `OvlFunc_969_200d9f0` | `0x0200d9f0` | [ovl_314_c_c_b.c](src/overlays/rom_7f6e64/ovl_314_c_c_b.c) | **parked** | remove a name — first screen |
| 2 | `OvlFunc_882_2008030` | `0x02008030` | [ovl_30_a_a.c](src/overlays/rom_77dd1c/ovl_30_a_a.c) | **parked** | remove a name; signed `short` field |
| 3 | `Func_801ce90` | `0x0801ce90` | [rom_1ca1c_a_c_b.c](src/rom_15000/rom_1ca1c_a_c_b.c) | **parked** | remove a name — rotation was a symptom |
| 4 | `Func_801cee0` | `0x0801cee0` | [rom_1ca1c_a_c_c.c](src/rom_15000/rom_1ca1c_a_c_c.c) | **parked (mine)** | which side of the `if` carries the exit |
| 5 | `Func_8092504` | `0x08092504` | [rom_91584_…a_c_c.c](src/rom_8a000/rom_91584_c_c_a_c_c_c_c_c_a_c_c.c) | **parked** | address-holding pointer; `volatile` on both |
| 6 | `Func_80aaf58` | `0x080aaf58` | [rom_aa538_c_c_a_c_b.c](src/rom_a1000/rom_aa538_c_c_a_c_b.c) | — | own return type; two induction variables |
| 7 | `Func_80ae714` | `0x080ae714` | [rom_ad274_c_c_b.c](src/rom_a1000/rom_ad274_c_c_b.c) | — | don't cache the loop bound |
| 8 | `GiveDjinni` | `0x0807a1b4` | [rom_79460_…a_c_a_b.c](src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a_b.c) | **parked** | remove a name |
| 9 | `Func_80b2928` | `0x080b2928` | [rom_b0070_c_c_a_c_b.c](src/rom_b0000/rom_b0070_c_c_a_c_b.c) | — | three unnamed re-reads |
| 10 | `OvlFunc_943_20088e0` | `0x020088e0` | [ovl_30_a_c_c.c](src/overlays/rom_7c7b9c/ovl_30_a_c_c.c) | **parked** | a narrower TYPE on the compared copy |

All ten verified in the linked ELF with a `.gcc2_compiled.` symbol at the
address after a clean `make clean && make -j8 && make compare`.

## A PARK OF MINE WAS WRONG, AND HOW IT WAS WRONG IS THE LESSON

`Func_801cee0`'s park — which I wrote in batch 179 — recorded that the branch
polarity in two of three switch arms was *"source-inert across the plain
`break`, an explicit `goto inc`, and an explicit `goto out`"*, and concluded the
4-differing form was structurally unreachable.

All three of those spellings keep the **return** as the `if`'s then-body. That
is the thing that decides the polarity.

```
    if (*p > N) return; break;
        bls Lafter / b Lret / Lafter: b Ljoin   ->  bls Ljoin / b Lret

    if (*p <= N) break; return;
        bhi Lafter / b Ljoin / Lafter: b Lret   ->  bhi Lret / b Ljoin   <- the ROM
```

gcc expands `if (C) X;` as invert-C-and-jump-past, then threads the resulting
one-jump block. **Which side of the `if` carries the exit picks the condition.**
Swapping it costs nothing else — registers, block order, the CSEd double read
and the cross-jumped tail all stay put.

I had varied the *form* of the exit three ways and never varied *which arm it
was in*. Three negative results that all share a hidden constant are one
negative result.

## FIVE NEW LEVERS, EACH FROM READING ONE FUNCTION PROPERLY

**A narrower TYPE on the compared copy, not a second read** (#10). Where the ROM
has `ldrb rA / mov rB, rA` with the copy *before* the `cmp`, every int-typed
spelling either lost the `mov` or put it after the branch. Declaring the
compared copy `unsigned char` while the stored value stays wide makes the
QImode/SImode pair non-coalescable; the copy lands correctly and a three-way
register rotation resolves for free. 45 differing to 1. **This is the mirror of
the read-count lever, not an instance of it** — a genuine second read here is
byte-identical to one read, because CSE folds it away.

**A second, giv-only index variable relocates induction updates past a store**
(#6). With `-fcall-used-r4` the loop counter is spilled around the call, and
sched2 cannot disambiguate a `char` store from the spill slot, so their order is
frozen at RTL order. Two indices — one surviving only in the loop compare, one
appearing only in address expressions so biv elimination deletes it and keeps
its givs — put the updates where the ROM has them.

**Assignment order of an address-holding pointer fixes register birth order**
(#5), where declaration order is inert. And `volatile` must be on the pointee as
well as the pointer, or gcc keeps the `p == sp+0` equivalence for the single
non-loop reference and the store folds to `str r3, [sp]`. No flag reached it.

**The return type is an oracle in both directions** (#9, #6). A ROM epilogue
that pops into r0 is positive evidence of a `void` function — a cheap check to
run *before* writing anything. And the same lever applies to the function's own
signature: declaring #6 `int` with no `return` statement gives the ROM's
`pop {r1} / bx r1`.

**A repeated pooled constant may be an offset RE-READ, not a value REBUILT**
(#7). The selector flagged `ldr r3, =0x219` twice as the duplicate-constant
wall. It is the loop bound re-read across a call, too large for an `ldrb`
immediate. Re-reads yield to spelling; rebuilds do not. Caching the bound costs
51 differing.

## TWO NEGATIVES WORTH AS MUCH AS THE MATCHES

**`goto fail` is a negative when gcc already cross-jumps the returns**
(`GiveDjinni`). With two plain `return -1` statements gcc merges them into the
ROM's shared block and places it *before* the success tail; an explicit `goto`
to a trailing label inverts the two. The recorded shared-exit lever is for when
our output is *missing* the shared block.

**`Func_80a3d9c` is now parked with its mechanism named** rather than guessed.
Three of its four missing instructions are a `0xf800` mask that combine folds:
because the value is single-set from an `ldrh`, `nonzero_bits <= 0xffff`, so
`simplify_shift_const` rewrites mask-then-shift into shift-then-mask and drops
the mask as redundant. If the width were unknown the rewrite would leave an AND
with no Thumb insn and no valid split, and combine would reject the whole
combination — which is exactly the ROM's four lines. Only two states are
reachable from source (31 lines with the constant hoisted, 28 with it folded)
and the ROM is neither. It will close by itself if the `Func_801f730` fold is
ever defeated. Thirty spellings and six flags measured.

## On the subagents

Each was sandboxed to its own `scratch/` directory, forbidden to run `make` or
touch anything else, and required to report `(lines, differing)` for **every**
spelling tried rather than only the winner. Those measurement tables are most of
the value above — the negatives are what identify a mechanism.

One found a real bug in `tools/tryc.py`: the helper-alias hint searched
`("divsi3", "udivsi3", "modsi3", "umodsi3")` in order, and since `"modsi3"` is a
substring of `"umodsi3"` it named the wrong alias — a line someone would have
pasted into an `overlay.ld`. Fixed, ordered longest-first, and the message now
says to check whether the alias is already present.

## State

1,896 functions remain in `asm/`. The read-count rule has now unparked eleven
functions across batches 178–180, and in six of those the park had specifically
concluded that a register difference was unreachable. **A register rotation
beside a missing or misplaced copy is a symptom often enough that it should not
be recorded as a blocker class on its own.**
