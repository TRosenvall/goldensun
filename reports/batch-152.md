# Batch 152 — a flag group, and screening for shape families

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. Green on
the first attempt this time, with `git status` clean across every tracked
generated `.s` — the free integrity check from [batch-151](batch-151.md), which
is what catches the compiler's ASLR nondeterminism. Every address below was read
back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `OvlFunc_common2_28c` | `020093e4` / `0200da78` | common2, in ovl_7bf5a8 **and** ovl_7e7574 | [common2_a_b.c](../src/overlays/common/common2_a_b.c) |
| `OvlFunc_946_200a2c8` | `0200a2c8` | ovl_7ced6c | [..._a_c_c_a_c_b.c](../src/overlays/rom_7ced6c/) |
| `OvlFunc_946_200ab80` | `0200ab80` | ovl_7ced6c | [..._a_c_c_c_c_b.c](../src/overlays/rom_7ced6c/) |
| `OvlFunc_946_200ac4c` | `0200ac4c` | ovl_7ced6c | [..._a_c_c_c_c_c_b.c](../src/overlays/rom_7ced6c/) |
| `OvlFunc_946_200ad0c` | `0200ad0c` | ovl_7ced6c | [..._a_c_c_c_c_c_c_b.c](../src/overlays/rom_7ced6c/) |

## Screen for a SHAPE FAMILY, not for one function

The method change is the headline. One round in this batch produced **no
elevation at all** — two functions worked deeply, both parked. The next round
changed approach: instead of picking the most tractable single candidate, look
down `tools/pickable.py` for several entries with the same size, call count and
interleave score in the same overlay.

Five `OvlFunc_946_*` came out that way, four of them in one split chunk. Solving
the shape once produced **four elevations**, three of them on the first screen.
That is a better return than any amount of spelling-search on one function, and
it is repeatable: the tool already prints everything needed to spot a family.

**The one thing that mattered: put the call in EVERY arm.** These dispatchers
branch to a shared tail holding `mov r2, #0` and the `bl` — and, where the
constant is negative, the `neg` as well. That reads like one call after a join,
and writing it that way is **nine instructions short**, because gcc then
materialises the common first argument once instead of per arm. It is gcc
cross-jumping identical call tails. Writing the call out in all five or six arms
took the first function from 66 differing of 80 to 13, and the siblings written
that way matched outright.

Worth setting beside the shared-call-tail parks already in the corpus: those
record gcc **failing** to cross-jump where the ROM did. This is the same
mechanism from the other side, and the source shape to try is identical — put
the call in the arms and let the compiler decide.

`OvlFunc_946_200a2c8` is the clearest specimen: three of its arms make two calls,
the first written out in full and the second sharing the family tail. Two of its
conditions are also short-circuit ORs rather than nested tests, which is what the
ROM's pairs of branches into a shared label are.

## `common2` was built without `-fcall-used-r4`, and `common2_a` is the same TU

Following [batch-151](batch-151.md)'s flag fix, the Makefile pattern is widened
from `common2_c%` to `common2_%`. `common2_a` carries both tells — the
non-interwork `pop {pc}` epilogue and a `push {r4, ...}` — so it wants exactly
those flags, and `OvlFunc_common2_28c` byte-matches under them.

Three source shapes, each measured, took it from 21 differing of 23 to zero:

- **The two 8-byte operands are SUBOBJECTS of one struct, with a pointer local
  to each.** As two separate locals both offset-0 stores fold to `[sp, #N]`
  while the offset-4 stores go through the register. This corrects batch 151's
  rule in a way that matters: it needs **two** pointers here. Pointing one
  pointer at the whole struct and using offsets 0/4/8/0xc gives the right
  addresses via the wrong instructions.
- **Pointer BIRTH ORDER decides the register names.** Assigning both pointers up
  front hands the wrong one the lower register and swaps r5/r6 through the entire
  body — 8 differing lines from one misplaced assignment.
- **The frame is laid out by DECLARATION ORDER, last-declared lowest.** Read the
  ROM's `add rN, sp, #imm` offsets, sort them, declare in the opposite order.

Also useful operationally: `-mno-thumb-interwork` cancels the flag, and a later
`-fcall-saved-r4` overrides an earlier `-fcall-used-r4`, so a `common2` function
screens correctly from any scratch path even though `tryc` matches Makefile
overrides by source path. The old `common2_254` park had concluded no meaningful
screen was possible for its file.

## What the parks established

**The register coin flip lives in the SLACK.** Four functions in one chunk share
a source shape. The three that read three actor values and spend three
callee-saved registers match byte for byte. The one that reads two and spends
two is a pure r5/r6 rename — 13 differing lines and nothing else. With three
live values gcc and the original compiler agree; with two, gcc hands the
higher-priority pseudo the first available callee-saved register and the ROM
hands it the second. Before parking a two-register rename, check whether a
sibling with more live values matches from the same source: if it does, the
shape is proven and what is left really is the flip.

**The basic-block lever has a boundary, and an inverse.** It retires
arg-interleave by assigning the constant in a different basic block, which needs
a branch to cross — a call does not create one. `OvlFunc_945_200dca4` is eleven
calls in sequence with no condition at all, so it has one basic block and the
lever has nothing to bite on; it parks at 2 of 43. The cutscene scripts in these
overlays are mostly straight-line and are exactly the shape that produces
interleaves, so this is a boundary worth knowing rather than a fact about one
function.

The inverse turned up on `Anim_UnleashIntro`: where the ROM builds a
two-instruction constant **contiguously** and gcc splits it, naming the value in
a local in the **same** basic block fixes it. Both its `StartTask` sites went
from wrong to exact that way, 6 differing to 2.

**A wrong jump table can look like a register problem.** `Anim_UnleashIntro`
switches on 0..4 where the fifth case does what `default` does. Four labelled
cases plus a default gives a comparison chain, not a table: 59 differing of 80.
Adding the redundant `case 4:` produces the table, takes it to 6, **and fixes an
r5/r6 swap.** A two-register rename is not always an allocation coin flip — it
can be downstream of a control-flow shape that is wrong. Count the ROM's
jump-table entries and write out exactly that many cases.

**`common2_618`, the double decoder, is most of the way in.** 38 of 102, with the
whole register allocation and every branch sense matching. Word 0 of a double is
the HIGH word here (ARM's mixed-endian layout, agreeing with `common2_304`); the
stack scratch is a union read at three widths and needs a pointer local, or gcc
scalarises it away and comes out 19 instructions short; and the mantissa is one
`unsigned long long`, since the ROM's shifts compute into temporaries and then
`mov` into a fixed register pair. What is left is three cases of gcc being
cleverer that pull against each other.
