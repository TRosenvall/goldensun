# Batch 109 — the prologue habit becomes a tool

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of its overlay's linked ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_967_200815c` | `0200815c` | ovl_7f21b8 | [ovl_30_c_c_a_c_b.c](../src/overlays/rom_7f21b8/ovl_30_c_c_a_c_b.c) |
| `OvlFunc_967_20081c8` | `020081c8` | ovl_7f21b8 | [ovl_30_c_c_a_c_c_b.c](../src/overlays/rom_7f21b8/ovl_30_c_c_a_c_c_b.c) |
| `OvlFunc_967_2008234` | `02008234` | ovl_7f21b8 | [ovl_30_c_c_a_c_c_c_b.c](../src/overlays/rom_7f21b8/ovl_30_c_c_a_c_c_c_b.c) |
| `OvlFunc_967_200829c` | `0200829c` | ovl_7f21b8 | [ovl_30_c_c_a_c_c_c_c_b.c](../src/overlays/rom_7f21b8/ovl_30_c_c_a_c_c_c_c_b.c) |
| `OvlFunc_959_2008dcc` | `02008dcc` | ovl_7e7574 | [ovl_9dc_a_c_c_a_a_c_a_b.c](../src/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_a_b.c) |
| `OvlFunc_959_2008e80` | `02008e80` | ovl_7e7574 | [ovl_9dc_a_c_c_a_a_c_c_a.c](../src/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_c_a.c) |
| `OvlFunc_959_2008f30` | `02008f30` | ovl_7e7574 | [ovl_9dc_a_c_c_a_a_c_c_c.c](../src/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_c_c.c) |

**2480 functions remain in assembly, 230 parked.** All seven came from two
families; only two functions were actually reasoned about from scratch.

## `tools/prologue_families.py`

Batch 108's best result came from taking a function that had just matched,
escaping its first six instructions and grepping every `.s`. That only finds a
family *after* solving a member. This does it for the whole tree at once.

It reads every `.thumb_func_start`, takes the first N instruction lines,
canonicalises the operands that vary within a family, and groups on the result:

```
immediates      mov r5, #8   / mov r5, #0x10   ->  #K
pool symbols    ldr r0, =.L250c / =.L1d00      ->  =S
branch targets  .L652 / .L472                  ->  L
```

**Registers are kept literal**, which is the whole design decision. What varies
within a real family is the constants; what does not vary is the register
numbering and which operand position each register sits in. Canonicalising
registers too would collide unrelated functions.

`--n 12` is the useful depth. At `--n 6` the largest "family" is 235 functions
sharing nothing but the high-register save boilerplate.

**34 families of 3+ share their first twelve instructions, covering 166
functions.** The three largest are per-overlay copies of the same routine:

| members | instructions |
|---|---|
| 18 | 172 |
| 18 | 139 |
| 17 | 132 |

Those three are 53 functions and one solved member each would clear them. They
are also 132-172 instructions with r8-r12 in use, so they are a project rather
than a round — noted here as the largest single lever left in the tree.

## The tool found something already parked, which is the useful check

One of the first families it surfaced — four 20-instruction DMA setups — turned
out to be `src/non_matching/ovl_7a1ff0/2008c0c.c`, parked since batch 55 with a
correct analysis: the ROM has two `ldr r3, =REG_DMA3SAD` (one per arm) and one
`stmia` after the join, which is a **partial tail merge** that gcc-2.96 does not
perform. One `DMA3_COPY` gives 21 lines, one per arm gives 24, the ROM has 22.

That the tool independently surfaced a family somebody had already identified by
hand is the evidence that it is finding real families rather than coincidences.

## Two families solved

**Four Lemuria attendants** (`OvlFunc_967_*`, 36-37 instructions). All four ask
the same question — is the player facing the shrine? — using the **quadrant
facing test** from batch 91:

```c
unsigned short d = (a->facing + 0x2000) & ~0x3fff;
if (d == 0xc000)
```

The mask spelled `~0x3fff` (so gcc pools `0xffffc000` in one `ldr`) and the
result `unsigned short` (so the `lsl r3, #16` appears) are both from that
write-up. The first screened OK; the other three were constant substitutions.

**Three shrine-offering handlers** (`OvlFunc_959_*`, 37-38 instructions). Four
constants need the basic-block lever, with the `if` supplying the boundary, and
two of them are the **same value passed as two arguments of one call**:

```
rom    mov r0, #0xc0 / mov r1, #0xc0 / mov r2, #0x80 / lsl r0, #10 / lsl r1, #10
ours   mov r1, #0xc0 / lsl r1, #0xa / mov r2, #0x80 / mov r0, r1
```

gcc builds `0xc0 << 10` once and copies it; the ROM builds it twice. One local
per site.

## A refinement to carried-vs-rebuilt

Batch 107's rule says a value the ROM carries in a register across calls wants a
named local adjacent to its first use. The `-1` in these three functions is
carried in r6 across three calls — and **naming it makes things worse**: 22
differing of 38, because gcc then builds it *before* `__CheckPartyItem` where
the ROM builds it after. As a plain literal in all three places, gcc carries it
into r6 by itself and materialises it at the comparison, which is where the ROM
does.

So the rule needs a precondition it did not have:

> A value the ROM carries wants naming only when gcc would otherwise **rebuild**
> it. If gcc is already carrying it, naming only moves where it is built — and
> the literal's position is usually the ROM's.

Check the unnamed screen first. That is one screen, and it is the difference
between a two-line edit and a wrong one.

## A six-function park

The tool also found six functions sharing all sixteen of their instructions, and
they are parked together at 3 differing:

```
rom    mov r1, #0xcb / mov r0, #0 / lsl r1, #1 / ldr r2, =0x2d7
ours   mov r1, #0xcb / lsl r1, #1 / ldr r2, =0x2d7 / mov r0, #0
```

Both halves of that — the `mov`/`lsl` pair split around another argument, and
the pool load issued last — are shapes the basic-block lever reaches. **The
lever needs a branch to put between the assignment and the use, and these
functions have none**: sixteen instructions, four calls, no control flow at all.

That is the limit `docs/elevation.md` states explicitly, and this is the cleanest
instance of it in the tree. Four spellings measured, plus the batch-106
argument-order table explaining why the return-type lever cannot substitute: it
moves r0 between first and last, and the ROM wants r0 in the **middle**.

Worth revisiting only if some construct is found that puts r0 in the middle
without a branch. Six functions come with it.
