# Batch 121 — working by family, and two levers that bracket their target

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. All 18
symbols read back out of `goldensun.elf` / the linked overlay ELFs.

**18 elevated, 12 parked. 2314 → 2296 remaining.**

## The selection axis changed, and it is the main result

Picking candidates one at a time had stopped paying — two rounds in a row of
allocation floors. `tools/twin_families.py` groups the **remaining** functions
by identical opcode stream, so one solved `.c` becomes a template and the rest
are a search-and-replace on constants. **46 families of 2+ in the 20–120 band,
covering 96 functions.**

Results this batch:

* a **four-member DMA family** solved in one round — including a member that had
  been parked for several batches at "21 lines against 22, missing one
  `ldr r3, =REG_DMA3SAD`", whose answer was sitting in its three siblings;
* a **two-member family** where the sibling matched on the first screen after
  six constants were substituted.

Two cautions, both learned the hard way and both in the tool's docstring:

* It groups unelevated functions **against each other**. The first version
  compared them against already-elevated ones and returned zero, because
  generated `.s` files use gcc's own `.thumb_func` rather than the repo's
  `.thumb_func_start` macro — the scan saw no elevated bodies at all. Checked
  the positive control before believing the zero.
* **A family can be uniformly blocked as easily as uniformly solvable.** Two
  families this batch: one worth four functions, one worth zero.

## The DMA finding, which contradicts what was recorded last batch

Batch 120 recorded that a volatile `asm` is *not* cross-jumped, so a DMA call in
both arms of an `if` gives two expansions and the fix is to select the differing
argument into a local. A four-member family says the opposite, and the
contradiction is the answer:

    if (flag) DMA3_COPY(A, d, size); else DMA3_COPY(B, d, size);

gcc merges the two asm bodies into **one** `stmia` while keeping the per-arm
operand setup — including a duplicated `ldr r3, =REG_DMA3SAD`. Selecting the
source first gives 21 lines against the ROM's 22.

Both readings are real; the ROM's line count separates them. **Two DMA base
loads with one `stmia` means the call is in both arms.**

## Two levers that bracket their target without hitting it

**The QImode zero test.** The ROM checks a decremented byte with a single
`lsl r3, #0x18` then `cmp`. A narrow *variable* is normalised on register entry
— `lsl` + `lsr`, one instruction too many. A narrow *cast* used only in a
comparison is folded away entirely — one too few. Six spellings measured; the
ROM sits between them. That distinction is the same mechanism behind the
ORR-destination lever, where the narrow local surviving is exactly what is
wanted.

**The ORR destination itself is sharper than recorded.** `int m = 2;` is
byte-identical to the plain literal — gcc folds it. `unsigned char m = 2;`
matches. So the `int` spelling is not a weaker version of the lever, it is *no
lever at all*, and a null result from it says nothing about the technique.

## The constant-CSE rule: a boundary is not sufficient

`OvlFunc_932_2008b3c` uses a flag id once before an `if` and once in each arm —
three uses in three different basic blocks, so the boundary the rule asks for is
plainly present. gcc hoists it into a callee-saved register anyway and pays a
push. Of the CSE-family flags only `-fno-rerun-cse-after-loop` undoes it;
`-fno-gcse` is byte-identical to the default.

The mirror image, in the same batch: a three-member family calling
`__Func_80933f8(-1, -1, -1, 0)` is parked because none of the three has any
boundary, while `OvlFunc_923_2009208` — same callee, same three arguments, same
three-separate-locals spelling — **matches**, because it has an early
`if (GetFlag(...)) return;` between the assignments and the uses.

## Other things worth keeping

**Build a constant AFTER the call if the ROM does.** A constant written before a
call and used after it is live across it and cannot live in r4 (call-clobbered
under this tree's `-fcall-used-r4`); gcc reaches for a third callee-saved
register and the whole function renames. 24 differing → 12.

**Naming one level too many costs a register**, and its inverse: **two constants
in different registers are simultaneously live**, so a value may need to be born
*earlier* than its first use suggests. Both show up in the push list first.

**The perturb test earns its keep.** `OvlFunc_901_20089f8` reads as one blocker
at 16 differing. Perturbing the repeated constant so gcc cannot common it gives
51 lines against 51 with 5 differing — revealing a **second** blocker four
instructions from the end. Without it the park would have recorded the CSE as
the whole story.

**Splitting a single-function `.s` by hand: remap every section.** Three files
this batch carried `.data` *and* `.bss` besides the function. Remapping only
`(.data)` links past the first error and fails on `.bss` with a much less
obvious message. Grep the linker script for every line naming the object.
