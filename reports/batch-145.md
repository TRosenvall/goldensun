# Batch 145 — twenty-one functions, and a wrong conclusion corrected

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every
address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_8005b64` | `08005b64` | main ROM | [rom_56cc_a_c.c](../src/rom_c0/rom_56cc_a_c.c) |
| `InitSprite` | `0800b7c0` | main ROM | [rom_b798_c_a_a_a.c](../src/rom_9000/rom_b798_c_a_a_a.c) |
| `PrintBattleText` | `080174f8` | main ROM | [rom_15e8c_c_a_c_c_a.c](../src/rom_15000/rom_15e8c_c_a_c_c_a.c) |
| `Func_80197c4` | `080197c4` | main ROM | [rom_1908c_c_a_c_a.c](../src/rom_15000/rom_1908c_c_a_c_a.c) |
| `Func_8021a18` | `08021a18` | main ROM | [rom_20198_c_c_c_a_c_a.c](../src/rom_15000/rom_20198_c_c_c_a_c_a.c) |
| `Func_80289e8` | `080289e8` | main ROM | [rom_23178_a_a_a_c_a.c](../src/rom_15000/rom_23178_a_a_a_c_a.c) |
| `Func_8092878` | `08092878` | main ROM | [rom_925e0_a_a_c_c.c](../src/rom_8a000/rom_925e0_a_a_c_c.c) |
| `Func_8093168` | `08093168` | main ROM | [rom_92950_c_c_c_a.c](../src/rom_8a000/rom_92950_c_c_c_a.c) |
| `Func_809397c` | `0809397c` | main ROM | [rom_93304_a_c_c_a.c](../src/rom_8a000/rom_93304_a_c_c_a.c) |
| `Func_8097194` | `08097194` | main ROM | [rom_96cdc_c_c_a.c](../src/rom_8a000/rom_96cdc_c_c_a.c) |
| `Func_80978c4` | `080978c4` | main ROM | [rom_97384_c_a_c_c.c](../src/rom_8a000/rom_97384_c_a_c_c.c) |
| `Func_809b5dc` | `0809b5dc` | main ROM | [rom_9ad70_c_c_a.c](../src/rom_8a000/rom_9ad70_c_c_a.c) |
| `Func_80e3994` | `080e3994` | main ROM | [rom_e3958_c_c_c_a.c](../src/rom_c9000/rom_e3958_c_c_c_a.c) |
| `OvlFunc_898_2009674` | `02009674` | ovl_793768 | [ovl_314_c_c_c_a_c_c_c.c](../src/overlays/rom_793768/ovl_314_c_c_c_a_c_c_c.c) |
| `OvlFunc_921_2008030` | `02008030` | ovl_7a7298 | [ovl_30_a_a_a.c](../src/overlays/rom_7a7298/ovl_30_a_a_a.c) |
| `OvlFunc_934_20090e0` | `020090e0` | ovl_7bdeb0 | [ovl_d20_c_c_c_a_a.c](../src/overlays/rom_7bdeb0/ovl_d20_c_c_c_a_a.c) |
| `OvlFunc_941_2008384` | `02008384` | ovl_7c5efc | [ovl_30_c_a_c_c_c_a_c_a.c](../src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c_a.c) |
| `OvlFunc_943_200b1a8` | `0200b1a8` | ovl_7c7b9c | [ovl_30_c_a_a_c_a_c_a_c_c_a.c](../src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_c_a.c) |
| `OvlFunc_944_20080c0` | `020080c0` | ovl_7ca63c | [ovl_30_a_c_a.c](../src/overlays/rom_7ca63c/ovl_30_a_c_a.c) |
| `OvlFunc_946_2009a44` | `02009a44` | ovl_7ced6c | [ovl_30_..._a_a.c](../src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a.c) |
| `OvlFunc_964_2008cd0` | `02008cd0` | ovl_7ed0a0 | [ovl_30_a_a_c_a_a.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_c_a_a.c) |

Six functions were parked. Two carry the register allocation read straight out
of the compiler.

## The correction: `cmp rN, #K / bge` is reachable, and batch 143 said it wasn't

[Batch 143](batch-143.md) recorded this shape as unreachable from an `if`. The
evidence was a corpus count: the sequence appears **exactly once** across 3205
generated `.s` files, and that one instance is switch dispatch. The report said
so confidently and told future rounds not to spend time on it. `Func_8093168`
was parked at 4 of 57 on exactly that shape, with four spellings recorded as
failures.

**The inference was wrong**, and the way it was wrong is the part worth keeping.
A corpus count says what the tree **contains**, not what the compiler can
**emit** — and every function in the tree was written by someone who had already
concluded the shape was out of reach. The measurement was real; the conclusion
drawn from it did not follow.

A direct nine-way probe settles it. For `x < 8`:

```
if (x < 8) x = 8;                 ->  cmp r0, #7 / bgt     (the rewrite)
return x < 8 ? 8 : x;             ->  cmp r0, #8 / bge     <-- the ROM's form
int k = 8; if (x < k) x = k;      ->  cmp r0, #8 / bge     <-- the ROM's form

early return, short, !(x>=8), x<=7, char, x-8<0   ->  all cmp #7 / bgt
```

The `<` to `<= K-1` rewrite is applied to an `if` statement's comparison, but
**not** to a conditional expression's, and **not** when the bound arrives through
a named local. Naming `Func_8093168`'s two bounds took it from 4 differing to 2
to a match.

Corrected in `docs/elevation.md`, in batch-143 itself, and in the HANDOFF row
that pointed at it — that row was actively telling future rounds to skip the
class.

## The scheduler-interleave wall has a source-level key

Recorded across three batches as unreachable, on the strength of three
independent spellings producing byte-identical output. It has a key, and it is
not a spelling of the arithmetic.

`OvlFunc_964_2008cd0`'s final residue was two independent chains emitted in the
wrong order. Nine source spellings were inert at 2 differing — commuted
operands, alternate literals, a named temporary, a named constant, a named mask,
split compound assignments, and the statement moved to four different points.
Four scheduler flags were inert too.

**Declaring the object as a struct and reading the halfword as a typed
`unsigned short` field** — instead of `*(unsigned short *)(e + 6)` — fixed it
outright, *even though both spellings emit the identical `ldrh r1,[r6,#6]`*. The
alias set, not the instruction, is what sched2 sees.

## A rule that was too broad: rebuilt constants and the `goto` lever

The doc records "a pool constant rebuilt inside a loop" as the selection
signature for the `goto`-loop lever. It has a **second cause**: reload
rematerialising under register pressure instead of spilling, which looks
identical in the output.

`Func_8021a18` has two such constants and both classic signatures. Written with
`goto` loops it scored **80 lines and 71 differing**; written with plain nested
`for` loops it scored **76/76 and 4 differing on the first screen**, because the
`goto` form defeats the strength reduction that produces the ROM's induction
variables. Count the live values first — near the twelve-register ceiling, a
rebuilt constant is reload, not the source.

The `goto` lever does have a second, cheaper-to-spot signature: it defeats
`check_dbra_loop` reversal. `Func_80197c4` had no hoisted invariants at all, yet
gcc reversed all three of its counters into countdowns. A ROM loop counting **up**
with `add / cmp #N / bne`, where the counter's only use is the exit test, is
itself a `goto` tell.

## Levers that paid, briefly

**`ldrb / lsl #24 / cmp` is a `volatile signed char` read.** Twelve spellings
probed: plain `signed char` and a `signed char` bitfield give `ldrsb`; plain
`char` is unsigned in this toolchain; `*p << 24` and `unsigned char` fold the
shift away. Only `volatile signed char` keeps the `lsl #24` and drops the
`asr #24`.

**Two textually separate `return -1;` statements always merge.** So a ROM that
materialises the same constant at two exits used ONE variable, not two returns —
and it has to be assigned inside the loop that kills it.

**An unused local array reproduces a bare `sub sp, #N`.** gcc-2.96 does not
eliminate the frame slot for an unreferenced array; probed five ways.

**A struct member and a pointer subscript emit a byte STORE's address and value
in opposite orders.** Closed `InitSprite` after three scheduler flags failed.
MEASURED not to extend to loads — the same substitution on `Func_808e0b0`'s read
is byte-identical.

**Naming is not a boolean.** On `OvlFunc_943_200b1a8` the pooled `0xffff` had to
be named *and* the assignment had to sit between two specific statements: in
place, a match; one statement earlier, 6 differing; one later or unnamed, 58.

**Declaration order and assignment order are both levers and are not
interchangeable.** On `OvlFunc_898_2009674`, swapping declarations closed it;
swapping assignments went to 78 differing.

**Split one variable into two with disjoint live ranges** to flip a register-role
swap that resists ordering levers — 17 differing to 2 on `Func_80289e8`.

**A stack scratch area wants a `struct`, not a `char[]`.** Two instructions per
field on `Func_8005b64`.

## Parked this batch

| Function | Standing | Blocker |
|---|---|---|
| `Func_80930bc` | 80 of 81 | allocation priority; also reads two uninitialised locals — a genuine ROM bug, reproduced |
| `OvlFunc_905_2008a68` | 12 of 110 | two independent chains scheduled in the other order |
| `OvlFunc_883_200d64c` | 62 of 112 | parameter one register too high; allocation read via `-dg` |
| `Func_801c34c` | 10 of 64 | sched2 hoisting pool loads in a straight-line prologue |
| `Func_8003e58` | 2 short | ROM carries one address in two registers |
| `OvlFunc_896_200c260` | 6 of 85 | reload temp register choice plus sched2 |

Two of these carry the compiler's own allocation dump. `OvlFunc_883_200d64c`'s
`-dg` output gives the priority order directly, and `global.c`'s
`allocno_compare` formula explains why our order differs: the parameter's five
references spread over the whole function genuinely lose to two pointers with
three references over forty instructions. Raising it needs references or a
shorter live range, and neither is available without changing the semantics.

`OvlFunc_896_200c260` has a clean diagnosis worth reusing: `-fno-schedule-insns2`
reproduces the ROM's instruction **order** exactly and leaves only a temp
register wrong, which identifies two visible symptoms as one cause. That flag is
not usable in the build, but it separates "wrong order" from "wrong register"
cheaply.
