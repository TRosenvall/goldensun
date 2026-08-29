# Batch 123 — eight elevated, three parked, and a lever recovered from a bad park

Verified on a clean `make clean && make compare` (with the documented host-side
recovery of the five agbcc objects) — `goldensun.gba: OK` — and every address
below read out of the linked ELF, not assumed.

## Elevated (2270 → 2262)

| function | address | file |
|---|---|---|
| `DialogueBox` | 0x080187fc | `src/rom_15000/rom_17e88_c_b.c` |
| `Func_80dfddc` | 0x080dfddc | `src/rom_c9000/rom_dfa18_c_c_c_c_b.c` |
| `Debug_TestEquipAndStatus` | 0x080b0444 | `src/rom_b0000/rom_b0070_a_a_c_a_a_c_b.c` |
| `Func_8092be0` | 0x08092be0 | `src/rom_8a000/rom_92950_c_a_c_a_b.c` |
| `AddMenuBarOption` | 0x080287a8 | `src/rom_15000/rom_23178_a_a_a_a_c_c_c_b.c` |
| `OvlFunc_915_2008bf8` | 0x02008bf8 | `src/overlays/rom_7a2bf0/ovl_30_c_c_c_a_a.c` |
| `OvlFunc_909_20085f4` | 0x020085f4 | `src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_b.c` |
| `OvlFunc_911_2008800` | 0x02008800 | `src/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_c_b.c` |

Seven needed a split; `make compare` was green after the splits and **before**
any `.c` landed, which is what separates a layout mistake from a bad
decompilation.

## The main result: a park that was wrong, and the lever it was hiding

I had parked `Func_801c8a0` claiming gcc-2.96 always folds a symbol plus a
constant offset into one pool entry, so the ROM's runtime `mov r2,#0x88 /
ldr r3,=gState / lsl r2,#2 / add r3,r2` was unreachable.

A control refuted it: **34 of 531 already-matching functions in this tree
contain that exact shape.** The C that produces it does the address arithmetic
in `unsigned int` locals, one operation per statement, rather than in a single
pointer expression. `src/rom_8a000/rom_8a5f8_b.c` has been doing it since it was
written.

**Sizing over the 2262 remaining: 356 functions carry the shape** — 115 with a
dereferenced-pointer base (the easy form, no integer locals needed), 216 with a
direct symbol base (needs the idiom), 25 with both. That is ~16% of what is
left, and it was written off.

### The control lied the first time, and the reason matters

The first control run reported **0 of 531**, which would have confirmed the bad
park. The detector matched `add rD, rS` — the ROM disassembly's two-operand
shorthand — while gcc emits `add rD, rD, rS`. `add_rr` came back 0 across the
entire corpus, which is impossible for real thumb output.

This is the third time this exact mismatch has cost a conclusion (the split
shifted build; the `.call_via` write-off). The rule is now stronger than "run a
positive control": **count the sub-patterns, and treat a zero component as
evidence the regex is broken, not the compiler.** `tryc.py` has folded this form
from the start; new detectors should reuse that normalisation.

## Parked, all three with the reasoning that generalises

- **`Func_80a3ddc`** — 39 of 39 lines, instruction count exact. The ROM keeps a
  redundant copy of a loaded halfword, and that second live scratch register is
  what forces the hoisted zero out to r12 and the destination pointer into r5.
  Typing the value `int` buys the ROM's `ldrh` and loses the copy; typing it
  `short` buys the copy and the ROM's exact allocation and loses the `ldrh`.
- **`Func_801c8a0`** — 60 of 63 after the idiom above; loop 1 is now
  instruction-for-instruction identical. What remains is narrow: the ROM hoists
  the loop's invariant *constants* but not its invariant *load*. The goto lever
  is all-or-nothing and cannot express partial hoisting, and six attempts to
  defeat gcc's alias analysis on that load changed nothing.
- **`Func_8093304`** — 32 of 33. One instruction: gcc folds the join-point store
  into a reg+reg addressing mode where the ROM keeps a separate `add`.

## Rules added to docs/elevation.md

1. **`ldrh` vs `ldrsh` is decided by the DESTINATION type.** A HImode local is
   loaded with `ldrsh` plus a zero index register and tested with `lsl #16`. A
   plain `ldrh rD,[rB,#0]` only ever comes from an SImode destination. Found on
   one function, immediately decisive on the next.
2. **A rebuilt loop bound is not always the `goto` tell.** Given
   `for (i = 0; i <= 0x1bf; i++)`, gcc rewrites the test to `i < 0x1c0`, and
   0x1c0 *is* a cheap shifted build where 0x1bf is not — so it rebuilds the
   bound every iteration and rotates the loop. Check "is K+1 a cheap shifted
   build?" before reaching for the goto lever; the answer is often "this should
   have been a do/while".
3. **When a derived constant IS reachable**, refining the earlier
   `Func_80160fc` park: reachable when the base constant has already been forced
   into a register by a runtime use, so CSE has something to derive from; not
   reachable when both values are only ever folded into addresses.
4. **The named-pointer lever has a precondition** — it needs the offset to be
   mutated after the pointer is taken. Where the offset is dead after the store,
   gcc always folds into the addressing mode.

## From the agent pass, verified here

- **A byte-offset register gets reused as the stored byte when the low bytes
  agree.** `gState+0x11c` then storing 0x1c: gcc emits `strb r2,[r3]` reusing
  the offset register because `0x11c & 0xff == 0x1c`, one instruction shorter
  than the ROM. An `int` local declared immediately before the store fixes it;
  the same declaration at the top of the function does not. A new cause for the
  "ours is one shorter" signature, distinct from the HImode rule.
- **The basic-block lever works on the POOLED constants, not the misplaced
  `mov`.** On `OvlFunc_909_20085f4`, naming the r0 constant took 10 → 8; naming
  the seven pooled operands instead took 10 → 2; adding the two `mov`+`lsl`
  builds as dominating-block locals was exact.
- **Hoisting a derived pointer above a pair of calls buys the callee-saved
  register the ROM spends.** `AddMenuBarOption` was 41 of 45 and one high
  register short; moving one statement above the first call was exact next
  screen. Read the push list as the diagnostic.

## Owed

- `src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c%.c` carries an `-O1` wildcard that
  is wrong for `OvlFunc_968_2009780`'s TU (26 differing at `-O1`, 2 at `-O2`).
  Fifth instance of the wildcard-flag trap; needs an explicit `-O2` rule when
  that function is wired.
- 12 not-yet-elevated `.s` TUs still sit inside Makefile wildcards and will
  silently inherit a flag when elevated.
