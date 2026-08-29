# Batch 81 — the thirteen-member family, and a screen that called matches failures

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_923_2008cc0` | `02008cc0` | [ovl_314_a_c_c_c_c_b.c](../src/overlays/rom_7aa430/ovl_314_a_c_c_c_c_b.c) |
| `OvlFunc_924_2008cd0` | `02008cd0` | [ovl_314_c_b.c](../src/overlays/rom_7ac2d8/ovl_314_c_b.c) |
| `OvlFunc_898_20091b0` | `020091b0` | [ovl_314_c_c_c_a_c_c_a_b.c](../src/overlays/rom_793768/ovl_314_c_c_c_a_c_c_a_b.c) |
| `OvlFunc_901_2008970` | `02008970` | [ovl_314_c_c_a_a_c_c_a_c_c_a_b.c](../src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a_b.c) |
| `Func_80c0df4` | `080c0df4` | [rom_bffb8_a_c_c_c.c](../src/rom_b5000/rom_bffb8_a_c_c_c.c) |
| `OvlFunc_923_2008d58` | `02008d58` | [ovl_314_c_b.c](../src/overlays/rom_7aa430/ovl_314_c_b.c) |

Six elevated, six parked, and one defect in `tryc.py` that had been telling the
truth backwards since batch 75.

## The screen was calling clean matches failures

Since batch 75 every **clean** screen has printed its `OK` line and then fallen
straight into the mismatch report:

    OK  Func_x  (21 lines)
    XX  Func_x  (rom 21 lines, ours 21, first diff at 0, 0 differ)

and returned exit status 1. The `continue` that ended the `OK` branch was
displaced when the inline-pool warning was added to it — **the commit that
closed one hole opened another**, and the symptom sat in batch 79's own sweep
output, quoted in that report, without my registering it.

`0 differ` is the tell that it is this bug and not a real mismatch, and that is
now written beside the `continue` so it cannot be lost silently again. Anything
reading the exit status, or grepping for `^  XX`, has been told that matches
failed.

Also fixed: **`lr` is normalised to `r14`.** The comment there said sp/lr/pc
were left alone because "both sides already spell those the same way". That is
true inside a push/pop list and false everywhere else — the ROM writes
`mov r14, r1` where gcc writes `mov lr, r3`.

Both changes were swept across all 134 parked files with a live reference, old
verdict against new.

## The thirteen-member family: 144 against 144, one load out of place

`OvlFunc_883_200834c` has topped `find_twins.py` by payoff since the tool
existed — **thirteen byte-identical copies at 139 instructions each**. First
attempt, and it is parked, but not far off.

What is right is nearly everything. Both streams are 144 lines. The stack frame
is the same size and every spill slot lands on the same offset. r8–r11 hold the
same values. The `ldmia r1!, {r3}` cursor over the model-id table and the
`add r0, #0x10` walk over the footprint rectangles both fall out of plain array
indexing. Every branch matches and the epilogue is identical.

**What is wrong is one load.**

    rom    ... asr r7, r1, #4                       <- tx finished
           mov r1, r9 / ldr r1, [r1, #0x10]         <- THEN the player's z
    ours   ... mov r4, r9 / ldr r4, [r4, #0x10]     <- z, five slots early
           ... asr r7, r3, #4                       <- tx finished after it

gcc hoists the `pl->z` read above the whole `tx` computation. Because it does, z
is live across that block and needs its own register, so the two `mov rLow, r9`
copies Thumb requires for a high-register base go to **different** low registers
where the ROM reuses r1 — and every register name in the window shifts with it.
Nothing writes memory between the two reads, so the scheduler is free to move it
and there is no aliasing barrier to invoke.

Eight source shapes and seventeen flag settings are tabulated in the park.
**Thirteen of the flags leave it exactly where it is**, which is itself the
finding: this is not a pass that can be switched off, it is the scheduler's
ordinary ready-list choice between two independent instructions.

## What actually moved functions this batch

Three of the six came from **naming the right thing**, and the three cases are
different enough to be worth listing together:

| function | what had to be named | why |
|---|---|---|
| `Func_80c0df4` | the four coordinate **loads** | gcc otherwise loads the x pair, adds and halves it, then loads the z pair — 12 of 30 differing. The ROM loads all four first. Naming only the two SUMS is not enough: 10 of 30. |
| `OvlFunc_901_2008864` (parked) | the stored **zero** | `*p = 0` on a `u16` pools the zero as a HImode constant. The ROM has `mov r5, #0` with r5 pushed — the signature of a pseudo created before the calls. `int z = 0;` at the top reproduces it, push included. |
| `Func_8020a60` (parked) | the loaded **value** | `*p = (*p & ~0x1000) | flip` narrows the whole expression to HImode, pooling the mask as `0xefff` where the ROM has `0xffffefff`; the extra width pushed the mask and the attribute into high registers needing a `mov` apiece inside the loop. An `unsigned int` intermediate keeps it SImode and the loop collapses from six instructions to the ROM's four. |

And one that is the opposite — **not** naming something:

`OvlFunc_898_2008d78`'s `a = GetActor(0xf); a->f64 |= 2;` gives
`mov r1, r0 / add r1, #0x64`, because gcc keeps the actor pointer alive.
`p = &GetActor(0xf)->f64; *p |= 2;` gives the ROM's single in-place
`add r0, #0x64`.

## Two readings that are about not inventing things

`OvlFunc_923_2008cc0`'s six-instruction shift-and-add chain —

    lsl r4, r0, #1 / add r4, r0        u * 3
    lsl r3, r4, #4 / add r4, r3        ... * 17
    lsl r3, r4, #8 / add r4, r3        ... * 257

— has no multiply in sight and is **one `u * 0x3333`** in the source.
Transcribed as shifts by hand it would not have matched. `0x3333` is a quarter
of the `0xcccc` that goes into the descriptor twice, which is the tell that both
are the same magic number.

`OvlFunc_923_2008d58`'s `0xc000` is **the same constant in both places** — the
ROM builds it once for a compare and leaves it in r1 as the next call's second
argument. A different literal in either place costs two instructions.

## The parks

| function(s) | state | blocker |
|---|---|---|
| `OvlFunc_883_200834c` ×13 | 144/144, 28 differ | one hoisted load; 8 shapes, 17 flags |
| `Func_8020a60`, `Func_80a2268` | 72/72, 7 differ | gcc coalesces `x + b->w` with `x1` and uses two-operand `add`; 10 spellings |
| `OvlFunc_916_2008098`, `OvlFunc_947_2008cc0` | 88 vs 84 | gcc hoists five address constants out of the inner loop where the ROM re-loads three; `-fno-loop-optimize` does not exist in this cc1 |
| `OvlFunc_898_2008d78`, `OvlFunc_901_2008864` | 32/32 and 24/24, 2 differ each | commutative-operand canonicalisation — the ROM makes the CONSTANT the `orr` destination; 9 spellings, 5 flags, and it is **not** the scheduler |

The last one is the closest thing left to a free function: two lines, in two
functions, from one decision in the expander.
