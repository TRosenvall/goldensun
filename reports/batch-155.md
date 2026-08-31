# Batch 155 — the first functions out of the pool class, and three corrections

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, green on
the first attempt, `git status` clean, `0 orphaned linker references`. All ten
addresses read back from the linked ELFs.

| Function | Address | ROM / overlay |
|---|---|---|
| `Func_8092a1c` | `08092a1c` | main ROM |
| `Func_8097a7c` | `08097a7c` | main ROM |
| `Func_8097adc` | `08097adc` | main ROM |
| `Func_80b6e30` | `080b6e30` | main ROM |
| `Func_80c1084` | `080c1084` | main ROM |
| `OvlFunc_962_200806c` | `0200806c` | ovl_7ec19c |
| `OvlFunc_950_2008500` | `02008500` | ovl_7d5838 |
| `OvlFunc_950_20085a8` | `020085a8` | ovl_7d5838 |
| `OvlFunc_919_200815c` | `0200815c` | ovl_7a67d8 |
| `OvlFunc_919_2008200` | `02008200` | ovl_7a67d8 |

## Seven out of the pool class, and it behaved exactly as predicted

These are the first functions elevated from the 516-function branch-over-pool
class since batch 154 settled it. **The pool shape caused trouble on none of
them.** gcc emits the `b` over the pool at the ROM's position unaided, and in
two cases the ROM's pool word order came out right with no special handling at
all.

Where the pool *did* matter, it was as a readout rather than a wall — and the
mechanism is now nameable:

**OPERAND MODE CONTROLS POOL PLACEMENT, via `pool_range`.** A constant written
as a bare literal into a halfword store is an HImode pool entry with a 64-byte
range, which forces an early dump and makes the mid-body pool appear. The same
constant routed through an `int` local is SImode with a 1020-byte range, and the
whole pool moves past the epilogue. `Func_8097a7c` needed the literal;
`OvlFunc_950_20085a8` is the same lever seen from the other side — every
instruction matched with `.text` at `0xc0` against the ROM's `0xc4`, and
widening the facing test to HImode reproduced the ROM's pool split and its two
alignment pads exactly.

**A four-byte `.text` difference with every instruction identical is a
pool-placement difference.** That is now a recognisable signature.

Reading the pool order off the hand-written `.s` *before* compiling is a cheap
pre-compile test: `Func_8097adc`'s ROM pulls four constants out as explicit
`.word`s ahead of `.pool`, which is gcc's HImode group, and that told us the
literals go straight into the stores. 26 differing to zero on the first attempt.

## The message_base_register park class did not exist

Three functions matched. The class parked several on the ROM holding a message
id base in a callee-saved register, listed `int base = 0xNNN` at the top of the
function, at the arm, and several literal forms as all having failed, and
concluded the allocator "decides it is not worth it".

The allocator decided nothing. `(int)(&_MSG_xxx)` makes gcc spend the register
every time, and the park never tried it. Three parks deleted as resolved — one
of them already stale, its function elevated commits earlier.

## Levers that each retired a park's stated conclusion

- **Deleting a local won `Func_80c1084`.** The park kept a second pointer beside
  the base, which forced the base into the wrong register. It had tried the
  right offset-clobber form *while keeping* the extra local, which is why it
  never fired.
- **`volatile` refuses a pointer fold** that `Func_8097a7c`'s park called
  unrefusable ("nothing in the source keeps a pointer live past its last use").
- **The offset must be declared inside the loop body** on `Func_80b6e30`.
  `move_movables` inserts a hoisted invariant immediately before `loop_start`,
  so a hoist is always *last* in the preheader — confirmed across five
  orderings. The ROM's `mov r5, #4` sits after the hoist, so it comes from
  `strength_reduce`, which runs later; declaring the offset in the body makes it
  a giv and lands its init on the right side.
- **`Func_8092a1c` needed no work at all** — its parked C was already correct
  and had been written off on the retired ceiling claim.
- **A residue can be downstream of a spelling elsewhere.** `OvlFunc_919_2008200`
  had 18 differing lines that looked like a register swap in its *head*; they
  were entirely the *tail's* pointer locals, and fixing those corrected the head
  as a side effect.

## THREE CORRECTIONS, all to this project's own tooling and claims

**1. `tryc.py` cannot be read at face value on branch-over-pool functions.** gcc
emits two labels at the same address where the ROM has one; the streams
misalign by a token and everything after reads as differing. `Func_80b6e30`
screens as **13 differing and is byte-identical**. `Func_8092a1c` screens as 6
and is byte-identical. Two independent screens found this and it was confirmed
by assembling both sides. **On anything with a mid-body pool: `tryc` for
iteration, `make compare` for truth, nothing in between.**

**2. An ad-hoc byte-comparison script written to work around (1) was itself
wrong**, and is deleted. It extracted one function into a standalone `.s` and
assembled it outside its file's context, which changes pool and alignment
behaviour; it called three correct functions "different". `make compare` is
green with all of them. A check that disagrees with the authority is worse than
no check, because it invites backing out correct work.

**3. `scratch/` was committed into the repo by mistake** in two commits that
used `git add -A ... scratch`, sweeping 388 working files into history.
`.gitignore` already ignored `scratch_elev/` for exactly this reason. Both
`scratch/` and `toDelete/` are ignored now, untracked with `--cached` so nothing
left disk. It surfaced because a screening pass noticed the working tree did not
match and flagged it instead of committing over it.

Batch 154's claim that "three of four interleave flags were spurious" was also
corrected there: measured, the `pickable.py` fix changes nothing on the current
candidate list.
