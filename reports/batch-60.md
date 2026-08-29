# Batch 60 — five functions, and a safety check that was nearly blind

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `HasMove` | `08078bc0` | main ROM | [rom_78b9c_a_c_b.c](../src/rom_77000/rom_78b9c_a_c_b.c) |
| `Func_808d5a4` | `0808d5a4` | main ROM | [rom_8ba38_c_c_b.c](../src/rom_8a000/rom_8ba38_c_c_b.c) |
| `OvlFunc_943_2008bb8` | `02008bb8` | ovl_7c7b9c | [ovl_30_c_a_a_c_a_c_a_a_b.c](../src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a_b.c) |
| `OvlFunc_943_2008bf0` | `02008bf0` | ovl_7c7b9c | [ovl_30_c_a_a_c_a_c_a_a_c.c](../src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a_c.c) |
| `OvlFunc_954_2008178` | `02008178` | ovl_7db0c8 | [ovl_30_c_c_a_a_a_c_b.c](../src/overlays/rom_7db0c8/ovl_30_c_c_a_a_a_c_b.c) |

## `carries_data()` checked only `.incbin`

The check that decides whether a `.s` can be deleted reported one as *"one
function and no data, convert it directly"*. It also held a `.section .rodata`
with an **`.incrom`** blob behind an exported label. Deleting it failed the link.

The corpus uses `.incrom` at **536** sites and `.incdata` at **67**. The check now
tests all three directives plus an explicit `.section .rodata`/`.data`.

**This is the second time a missed data section has broken the link.** The first
was a `.data` section with an `.incbin` — precisely the case this check was
written for. That it then missed two of the three directives is its own lesson:
*a check that fires on the exact case it was written for can still be nearly
blind to the same class.*

`Func_808d5a4` is hand split — the `.s` reduced to its data, `stage1.ld`'s
`.text` line repointed at the new `.c` while its `.rodata` line still takes the
`.s`.

## `switch` and if/else lower differently, and the reference says which

Batch 59 established that a `switch` is **required** to get gcc's balanced-tree
lowering — an if/else chain will not produce it.

This batch found the reverse. `Func_8078480` has cases 2, 3, 4, 5, 9. Written as
adjacent `case` labels, gcc recognises 2–5 as contiguous and emits a **range
test**:

```
cmp #5 / bgt / cmp #2 / bge
```

where the ROM has five individual `cmp`/`beq` pairs. Every instruction after the
third differs — **25 of 25**. An explicit if/else chain testing each value with
`==` gives the individual compares and takes it to 4.

> Individual `cmp`/`beq` pairs mean the source compared individually; a balanced
> tree means it was a `switch`.

Both park notes record their side, so neither reads as a general rule alone.

## Constructs that decided functions

- **`HasMove`**: the mask must be a named local **and** the loaded value must be
  the AND's destination. As one expression, the mask lands in a callee-saved
  register and is copied every iteration — 8 of 23. Two statements match.
- **`OvlFunc_954_2008178`**: the counter increment goes **after** the
  `__WaitFrames` call, and gcc then schedules it into the argument slot exactly
  as the ROM has it. The same swap was tried on `OvlFunc_956_20081c8` in batch 56
  and only improved the count there, because a pre-header load merge remained.
  Here nothing else is in the way — **the construct is confirmed reachable, not
  merely helpful.**
- **`Func_808d5a4`**: the second call's argument is the **value just read**, not
  the parameter. The ROM never rewrites `r1` between the `ldrsh` and the `bl`.
  Passing the parameter would be semantically identical and is not what the
  register use says.
- **The two `943` functions** need `-fno-rerun-cse-after-loop`, recognised from
  the batch-50 rule (flag id read in a guard, written in the body) and both
  verified *not* to match without it.

## Parked

- **`LoadUIBanner`** — 3 of 29, and the three are **symbol names**, not
  instructions. Its pool holds four entries all at one address; with one symbol
  gcc collapses the function from 29 to **nine** by tail-merging the identical
  arms. So **N identical pool entries means N distinct source symbols**. The
  remedy is three `.sym` definitions and is left as a maintainer's call, because
  the existing `.sym` files hold id values rather than addresses.
- **`Func_8021b80`** — the stack-arg pair has a **third shape**: one register
  walked through both slots. Batch 49 had two locals with one held; batch 52 had
  both rebuilt per arm.
- **`Func_80ad69c`** — the ROM holds the *offset* and recomputes the address;
  gcc holds the address. Naming the offset restores the length and worsens the
  count. Its `ldmia r5!, {r0}` is the tree's first single-register load with
  writeback, and that half is right.
- **`OvlFunc_932_200a9dc`** — the documented **limit** of the basic-block lever:
  the constant is used both before and inside the guard, which batch 44's third
  clause rules out.
- **`Func_8079008`** (11 of 38) and **`Func_8078480`** (4 of 25), both on
  allocation.
