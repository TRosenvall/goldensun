# Batch 154 — two blocker classes retired, eleven functions out

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, green on
the first attempt, `0 orphaned linker references`. All eleven addresses read
back from the linked ELFs.

| Function | Address | Overlay |
|---|---|---|
| `OvlFunc_907_2008fa0` | `02008fa0` | ovl_79b154 |
| `OvlFunc_939_20085f0` | `020085f0` | ovl_7c460c |
| `OvlFunc_913_2008c68` | `02008c68` | ovl_7a04ac |
| `OvlFunc_951_20088f8` | `020088f8` | ovl_7d6418 |
| `OvlFunc_893_2008054` | `02008054` | ovl_78dd40 |
| `OvlFunc_894_2008054` | `02008054` | ovl_78de18 |
| `OvlFunc_929_2008598` | `02008598` | ovl_7b7790 |
| `OvlFunc_900_20081e4` | `020081e4` | ovl_797740 |
| `OvlFunc_904_2008054` | `02008054` | ovl_799998 |
| `OvlFunc_937_200833c` | `0200833c` | ovl_7c3044 |
| `OvlFunc_926_200c1ec` | `0200c1ec` | ovl_7b2078 |

## SETTLED: the branch-over-pool shape is not a blocker

Batch 153 retired the *rationale* for this class — it rested on old_agbcc's pool
behaviour, and old_agbcc is not the compiler here. This batch answers what was
left: whether gcc would actually place the pools correctly.

**It does, unaided.** The decisive observation is `AnimEnd`: its ROM has two
branch-over-pool sites; while the diff stood at 64 differing **both were absent**
from our output, and once the instruction count converged **both appeared
spontaneously and matched**. On `StartSnow` the ROM's `b` over its pool is
present in our stream from the first screen.

The pool branch is a consequence of **code length**, not of a source construct.
If a reference has one and your output does not, you are the wrong length for
some other reason. **516 functions should be screened normally.**

`StartSnow` is the concrete cost of the old belief: skipped entirely last batch
without a line of C written, and now at 13 of 85.

**A park written earlier in this same batch was wrong about this** and is
corrected in place. On `Func_801c154` I concluded from one function that pool
contents reproduce while placement does not, and hypothesised the class splits
by size with the large members reachable. Both halves are wrong. The superseded
reasoning is kept in the park — it was a fair inference from one function and
someone will form it again.

## FILE STRUCTURE is a blocker class, and it is now mechanical

Thirty-one single-function `.s` files carry their data as a `.section .data` of
`.incbin` blobs after `.func_end`. Those functions were unreachable for a reason
unrelated to codegen: convert the file wholesale and the data goes with it.

**`tools/split_s.py` now handles it.** It used to refuse every single-function
file carrying data; it now tests whether any data sits *ahead of* the function,
and if none does, splits code to `_b` and data to `_c` through the ordinary
path. Two things made the clean case safe and both predated the change: the cut
at `.func_end` is unambiguous, and **the linker scripts already listed `(.text)`
and `(.data)` on separate lines**. The machinery existed and was unreachable
behind a blanket refusal.

Six of this batch's eleven came out of that class. `OvlFunc_907_2008fa0` was
parked one batch ago as "matches byte-exact but cannot be installed".

## Levers, each measured

- **A struct pointer is a register-allocation lever, not a readability choice.**
  On 893/894 the offset-arithmetic form pinned the base in the wrong register
  through *thirteen* spellings; the struct form was exact immediately. The old
  park concluded "the construct is right and the allocator disagreed", which
  kept the search inside pointer arithmetic — where the answer was not.
- **The declaration lever has a third form:** `void` versus `int` **return** on
  the mismatching callee. 929 needed none of the four documented variants.
- **The message base is a SYMBOL, not a literal.** `(int)(&_MSG_e23)` makes gcc
  spend a callee-saved register where `int base = 0xe23` is constant-propagated.
  This is a key to the whole `message_base_register.c` park class, which lists
  the literal forms as failed and never tried the symbol.
- **A narrow local flips an `orr` destination** where an int local never does —
  which is what stops gcc cross-jumping two arms the ROM keeps separate.
- **`*q++ = v;` emits a single-register `stmia`.** An existing park claimed this
  "appears NOWHERE in the generated corpus, so there is no matching C to copy
  the idiom from". False, verified by probe; worth 13 lines on `StartSnow`.
- **Thumb-1 gas does not fold `ldr rX, =imm8` into `mov`**, so a pooled small
  constant is always a genuine symbol tell, never a disassembly artifact.

## A likely new class: ldrh/ldrsh CSE

`OvlFunc_912_20081c4` (10 of 105) and `OvlFunc_901_2008f30` (19 of 123) share a
residue. The ROM reads the same halfword twice, once unsigned and once signed;
gcc emits only the sign-extended load and derives the other value from it. **The
merge is legal** — the low sixteen bits are identical — which is why no source
spelling prevents it. A third instance is named in the 912 park. Worth a
`find_shape.py` sweep before another round is spent on one function.

## Operational

`tools/pickable.py`'s interleave count has a false positive: its regex binds the
back-reference to `r0`, so an argument set up for one call followed by a shifted
constant for the next reads as an interleave site. Fixed by requiring the two
registers to differ.

**CORRECTION, added after measurement.** This report first said "three of one
agent's four flags were that". That does not hold up. Measured across the tree
the guard removes 202 of 4616 raw matches (4.4%), but on the CURRENT candidate
list it changes **nothing** -- all six candidates keep their counts. Across the
whole filter-eligible population four functions change and two drop to zero. The
"three of four" figure came from an agent's impression rather than a count, and
I repeated it without checking. The fix is still right; its impact is smaller
and narrower than stated.

A second over-count was found while verifying and is NOT fixed: the gap in that
regex is unbounded and crosses `bl` boundaries, so a match can pair a `mov` from
one call with a `lsl` from another twenty-five instructions later. It can both
merge two real sites into one and manufacture one from none. Bounding the gap,
or restarting it at each `bl` or label, is the follow-up.

The unguarded-interleave boundary now has seven confirmed instances. It is cheap
enough to recognise up front that no spellings should be spent on it.
