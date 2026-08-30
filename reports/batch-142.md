# Batch 142 — naming a value is a lever that points three different directions

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects (documented in [batch-61](batch-61.md)) → `make -j8 &&
make compare` → `goldensun.gba: OK`, SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address below was read back
out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `OvlFunc_common1_e10` | `0200a6a8` | ovl_7db0c8 (+2 more) | [common1_a_a_a_a_c_c_a_a_b.c](../src/overlays/common/common1_a_a_a_a_c_c_a_a_b.c) |
| `Func_80b0840` | `080b0840` | main ROM | [rom_b0070_a_a_c_a_c_c_c_c.c](../src/rom_b0000/rom_b0070_a_a_c_a_c_c_c_c.c) |
| `Func_808b25c` | `0808b25c` | main ROM | [rom_8ace0_a_a_c_c_a.c](../src/rom_8a000/rom_8ace0_a_a_c_c_a.c) |
| `Func_8079e9c` | `08079e9c` | main ROM | [rom_79460_c_c_c_c_a_c_c_a_c_c_c.c](../src/rom_77000/rom_79460_c_c_c_c_a_c_c_a_c_c_c.c) |
| `Func_80122c8` | `080122c8` | main ROM | [rom_1219c_a_c_a.c](../src/rom_9000/rom_1219c_a_c_a.c) |
| `GetNumDjinn` | `0807a5bc` | main ROM | [rom_79460_c_c_c_c_c_a.c](../src/rom_77000/rom_79460_c_c_c_c_c_a.c) |

`OvlFunc_common1_e10` is in shared `common1` code linked into three overlays,
at `0200a6a8`, `0200a940` and `0200b3d8`. All three were checked.

Six functions were parked. Four of them sit at **1, 2, 2 and 7 instructions**
of 48, 51, 51 and 49, which makes them leads rather than dead ends.

## The headline: naming a value helps, hurts, or does nothing, and the three cases look identical

Three functions in this batch hinged on whether an expression got its own local
variable. The outcomes were opposite, and nothing in the assembly says in
advance which case you are in.

**Naming it was the whole fix.** `GetNumDjinn`'s non-default branch reads
`u[(0x8c << 1) + which]`. The ROM computes `which + 0x118` into a register and
does `ldrb r3, [r2, r3]` with the unit pointer as base. Written inline, gcc
reassociates to `(u + which) + 0x118` and finishes with a plain
immediate-offset load — 8 of 55 differing. Assigning the index to `int k` first
and subscripting `u[k]` matches exactly. Note that source operand order does
**not** control this: `u[which + (0x8c << 1)]` produces byte-identical output to
`u[(0x8c << 1) + which]`. Only the name moves it.

**Naming it was catastrophic.** `CheckEquipmentCritBoost` is at 2 differing of
51 on a prologue ordering question — the ROM emits `mov r8, r2` before
`mov r1, #0xe` and we emit them the other way. The obvious fix is to give the
`0x200` mask a birth point before the loop counter by naming it. That takes it
from **2 differing to 47**: naming the mask earns it a callee-saved register of
its own, pulls r10 into the frame and moves the argument to r8. The mask has to
stay an unnamed loop invariant that gcc hoists by itself.

**Naming it changed nothing.** `Actor_SetAnimAndSpeed` is at 2 differing of 51
because the first `Sprite_SetAnim` call sets up its arguments in the other
order. Naming the animation index in a local before the switch produced
byte-identical output — the parameter already had a register, so there was
nothing for gcc to reorder.

The distinction that seems to hold: naming helps when it stops a
**reassociation** gcc would otherwise perform, and hurts when it promotes a
value gcc was content to rematerialise. It does nothing when the value already
lives in a register. That is a hypothesis from three data points, not a rule.

## Duplicated-looking code is often deliberately asymmetric

`Func_8079e9c` has two search loops that scan three bytes each. Writing them
the same way — which is what a tidy transcription does — cannot match. The ROM
increments the pointer **after** the compare in the enemy branch and **before**
it in the class branch: `if (*p == needle) ... p++;` against
`if (*p++ == needle)`. That single instruction's placement was the entire
difference between 8 differing and a match. Both branches then tail-merge onto
a shared `mov r0, #1`, which gcc does unprompted.

`Actor_SetAnimAndSpeed` shows the same thing at a larger scale, and its park
records it because it may be a general fact about this ROM. Across its four
call sites the argument emission order tracks the **callee**, not the
arguments:

```
case 1  SetAnim       mov r1, r8  / ldr r0, [r5, #0x50]   <- r1 first
case 1  SetAnimSpeed  ldr r0, [r5, #0x50] / mov r1, r10   <- r0 first
case 2  SetAnim       mov r1, r8  / mov r0, r5            <- r1 first
case 2  SetAnimSpeed  mov r0, r5  / mov r1, r10           <- r0 first
```

We reproduce three of the four and differ only where argument 0 is a memory
load rather than a register, which gcc hoists ahead of the register move.

## Read the idiom, not the instructions

`Func_80122c8` matched on the first screen, and would have gone straight to the
park if transcribed literally. Its coordinate conversion is

```
if (x < 0) x += 0x1fffff;
x >>= 21;
```

which is not a hand-rolled rounding fixup — it is gcc's standard expansion of a
**signed division by a power of two**. The source is `v[0] / 0x200000` and the
constant `0x1fffff` never appears in it. Spelling it as a shift produces
different code.

`Func_809b5dc` (parked at 1 instruction of 48) gives the same lesson from the
other side. The ROM does `ldrh / add / strh` and then a separate
`lsl #16 / asr #16` on the value it loaded. That shift pair exists **only**
because the old value is still live after the store, which makes it the
signature of a post-increment: `v = (*b)++`. The equivalent-looking
`n = *b; *b = n + 1;` reuses one `ldrsh` and never emits the shifts.

## Two levers confirmed twice, so they are general

**An accumulator that is only ever stored as a halfword still wants to be
`int`.** `Func_808b25c` declares its accumulator `short` in the obvious
reading; that makes gcc sign-extend on every assignment
(`lsl r3, r2, #0x10 / asr r4, r3, #0x10`), which the ROM does not do. It keeps
the masked value wide and truncates at the `strh`. Same shape as the halfword
note in `const.sym`, approached from the opposite direction.

**A global's offset must be built at runtime, not folded.** Writing
`*(short *)(gState + (0xed << 1))` lets gcc fold the whole thing into one pooled
`ldr r3, =gState+0x1da`. Assigning `gState` to a local `unsigned char *` first
forces the ROM's `mov r2, #0xed / lsl r2, #1 / add r3, r2`. This closed
`Func_808b25c` and was worth three instructions on `Func_809b5dc` (44 lines to
47), so it is now in the general toolkit rather than one function's notes. The
related finish-the-offset-before-the-base rule closed `Func_808b25c`'s last two
instructions: reading the map id *before* loading the list pointer, rather than
after.

`Func_808b25c` also reproduces a genuine quirk — its accumulator is
uninitialised on the path where the first list entry matches immediately. That
is the ROM's behaviour, not a transcription slip.

## Which operand is the pointer, again

`CheckEquipmentCritBoost`'s inner load is `ldrh r3, [r5, r7]`, where r5 walks
`0xd8, 0xda, …` and r7 holds the argument. The ROM makes the **walking offset**
the base register and the argument the index. Declaring the argument as a
pointer and indexing it produces the operands the other way round and shifts
every register after it. Taking the argument as an `int` and walking
`unsigned char *p = (unsigned char *)0xd8` flips it. The C looks wrong — a
pointer literal `0xd8` — and is evidently what the compiler saw. Second clean
instance of the rule recorded in batch 141.

## A park closed on a measurement that could not have returned anything else

`tools/park_status.py` was written this cycle because the park corpus is 349
files and a quarter of everything ever parked has since been elevated — 117 of
466 — which only works if the closest parks can be found again. A hand-rolled
regex got it wrong twice in one sitting, reporting two parks at "0 differing"
that are really 4 and a length difference, and matching a blocker line in only
77 of 349 because the headers use several formats. The tool parses four
recorded diff formats, the blocker line, and whether the park carries usable C.
**25 parks are at 2 differing or fewer; 26 have no usable C anywhere.**

It immediately surfaced `rom_b0000/80b0a20.c`, closed on three claims:

* zero generated `.s` files contain a mid-function pool
* `old_agbcc` emits pools only at `.func_end`
* "no source spelling reaches" the branch over the pool

All three are false. The search looked for the `.pool` **directive**, which gcc
never writes — it writes `.word` tables at its own labels — so it could only
ever return zero. 64 matching functions carry a mid-function pool, `old_agbcc`
is not the compiler, and compiling that park's own C emits the branch-over-pool
shape exactly. Re-measured it is 28 lines against 29, first difference at line
4, where the ROM loads a pooled zero before a byte store and gcc emits it
after. That is ordering, not a ceiling, and the park now says so.

## Also settled

`OvlFunc_common1_e10` was parked on symbol capture: the ROM names its script
tables `.L2`, `.L3`, `.L11`, `.L12`, `.L13`, and gcc generates `.L3`, `.L11`
and `.L12` for its own branches in the same function, so the code compiled
clean and loaded branch targets instead of tables. The park recorded that no
workaround existed because the symbols did not appear to be `.global` — which
was wrong, and checking it is what unblocked the function. They **are**
exported; `.global .L3` lives in `common1_c_c_b.s`, the file that *defines*
them, not the file that references them. The fix is a linker-script alias per
symbol (`_TBL_L3 = .L3;`), which emits no bytes. It must go in every script
listing the referencing `.o` — three here — and missing one is a link error
rather than a silent wrong answer.

`Func_80b0840` came from settling whether adding `"r0"` to `DMA3_SET`'s clobber
list would force the reload the ROM has without breaking the 41 elevated files
and 57 call sites already using it. Measured rather than assumed: it compiles
despite r0 also being an input operand, it forces the reload, and all 96
overlays still compare with the SHA1 unchanged.

## Parked this batch

| Function | Standing | Blocker |
|---|---|---|
| `Func_809b5dc` | 47 of 48 | pooled literal `1` in a halfword compare |
| `CheckEquipmentCritBoost` | 2 of 51 | prologue emission order |
| `Actor_SetAnimAndSpeed` | 2 of 51 | argument emission order |
| `Func_8096b88` | 7 of 49 | register-role swap in a loop preheader |
| `Func_807977c` | 23 of 42 | callee-saved birth order |
| `Func_80175c0` | 29 of 43 | one too few callee-saved registers |

`Func_809b5dc`'s remaining instruction is a pooled literal `1` compared against
a halfword. Reading into a named `short` and comparing does not pool it; that
was measured. `const.sym`'s halfword exception covers a constant that meets a
halfword in **arithmetic**, and a compare against an `ldrsh` result is evidently
not covered. No `_CONST_1` was added, because that would clear the letter of
that file's bar on one function's evidence and not its intent.
