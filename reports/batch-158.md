# Batch 158 — five elevated, and screening finally beat sampling

Clean `make clean && make compare` green from scratch. All five addresses
checked against the linked ELF.

| function | address | file |
|---|---|---|
| `Func_801a778` | `0x0801a778` | `src/rom_15000/rom_1a66c_a_a_b.c` |
| `Task_Cutscene` | `0x080915ec` | `src/rom_8a000/rom_91584_a_c_a_c_a.c` |
| `Func_80a3c98` | `0x080a3c98` | `src/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c_a_b.c` |
| `Func_80b6cdc` | `0x080b6cdc` | `src/rom_b5000/rom_b5a0c_c_c_c_a_b.c` |
| `Func_80c01bc` | `0x080c01bc` | `src/rom_b5000/rom_bffb8_a_a_c_b.c` |

## Three screens, applied before writing any C

The previous batch ended with four of seven rounds producing zero or one
elevation, all stalling on register assignment. This batch's change was to
predict that from the disassembly instead of discovering it after four screens.

1. **Parameter shuffling.** Count `mov r<callee-saved>, r<0-3>` in the first
   ten instructions. Every function that stalled on rotation had at least one.
2. **Unreachable copies** — `pure_copies()`, now in `tools/pickable.py`. Counts
   `mov rX, rY` where rY is never written again: a copy whose two names never
   diverge, which gcc coalesces, so no C local can produce it. Confirmed on
   four parked functions. Of 51 low-call candidates only 17 score zero.
3. **The push list.** If the ROM spends more callee-saved registers than a
   natural C version needs, addressing differences are downstream of that and
   no spelling will close them.

With all three applied the low-call pool is about 20 functions, and **four of
the five elevations matched on the first or second screen.**

`pure_copies()` went into `pickable.py` rather than a scratch script — the rule
from batch 157, after a hand-rolled filter re-offered already-parked work.

## Levers

**Name the COMPLETE offset, not a partial one.** The ROM addresses a table as
base plus a computed index with the field displacement folded INTO the index.
`s[i + 2]` gives the mirror image, and naming the partial offset changes
nothing — gcc re-associates `s + off + 4` to `(s + off) + 4` and the output is
byte-identical. Naming the whole displacement in its own local closed
`Func_80b6cdc` and `Func_80a3c98`. Operand order inside the offset expression
does not matter; completeness does.

**Its boundary: the base fold.** On `Func_80f7df0` the ROM holds the base in r4
across the body and pushes r5 to keep three complete offsets live. gcc folds
each base-plus-offset into a pointer, needs one register fewer, and does not
push r5. The fold and the register spend are ONE decision, so naming offsets
cannot buy the more expensive form. Check the push list first.

**The operand-mode rule has a third guise.** A value whose only consumer is a
narrow store gets canonicalised to whatever is cheapest for those bits. It has
now appeared as a pooled constant where the ROM has a `mov` (`Func_8011b00`),
a spurious pool load of zero (`Func_80173f4`), and as the SIGN of a pooled word
— ours `=0xffffaf80` against the ROM's `=0xaf80`, same stored halfword,
different pool word (`Func_80c01bc`). Routing the value through an `int` local
fixes all three.

**A `volatile` local reproduces a stack-resident value.** `Func_8092504`
allocates a four-byte frame for one value and re-reads it every iteration while
pushing r5, r6 and r7 — so it is not short of registers. As a plain local gcc
keeps it in a register and the function is five lines short; declared volatile
it is 8 differing at exactly 34. **Stated as an inference**: a plain local that
gcc happened to spill gives the same shape and the two cannot be distinguished
from output alone.

## An open question worth naming

`Func_801b9a8` (and its twin `Func_801b9ec`, identical but for one trailing
call) sits one line short because the ROM spends a POOL LOAD on `0x1f` where
the 8-bit immediate form exists and gcc uses it. Thumb-1 gas will not fold
`ldr rX, =imm8`, so this is a SYMBOL TELL — the original subtracted a named
value. No `.set` or `.equ` in the tree evaluates to 0x1f, and the pooled form
recurs in at least five other files, so it is a shared constant, most likely an
icon-table base. **Naming it closes two functions.** Guessing a name would be
inventing source, so it is left open on purpose.

## Skips that cost nothing

`Func_8011164` was skipped without writing C — a near-sibling of the parked
`Func_80110e0`, same inner loop, same reloaded base, and that park already
records three formulations folding to the hoisted output. The two `rom_f9000`
candidates were skipped as well: that is the m4a region with its own build
flags, where a match would not mean what it appears to.
