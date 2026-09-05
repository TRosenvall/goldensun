# Batch 232 — a whole file, a wildcard that only trapped the future, and a claim I got wrong

Seven functions, five of them a single file replaced end to end. The batch also
contains a correction I made against myself, which is the part I would keep if I
could keep only one thing.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_965_2009238` | `0x02009238` | [ovl_30_…_a_a_b.c](src/overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_a_a_b.c) |
| 2 | `OvlFunc_926_2008518` | `0x02008518` | [ovl_314_…_c_b.c](src/overlays/rom_7b2078/ovl_314_c_c_a_a_c_c_b.c) |
| 3 | `OvlFunc_891_2009c14` | `0x02009c14` | [ovl_30_…_a_a.c](src/overlays/rom_78c76c/ovl_30_c_c_c_c_c_a_a.c) |
| 4 | `OvlFunc_891_2009d14` | `0x02009d14` | [ovl_30_…_a_a.c](src/overlays/rom_78c76c/ovl_30_c_c_c_c_c_a_a.c) |
| 5 | `OvlFunc_891_2009e10` | `0x02009e10` | [ovl_30_…_a_a.c](src/overlays/rom_78c76c/ovl_30_c_c_c_c_c_a_a.c) |
| 6 | `OvlFunc_891_2009f0c` | `0x02009f0c` | [ovl_30_…_a_a.c](src/overlays/rom_78c76c/ovl_30_c_c_c_c_c_a_a.c) |
| 7 | `OvlFunc_891_2009ff4` | `0x02009ff4` | [ovl_30_…_a_a.c](src/overlays/rom_78c76c/ovl_30_c_c_c_c_c_a_a.c) |

## A whole file, which is the cheapest landing there is

Rows 3–7 are one `.s` replaced by one `.c`: **no split, no linker edit**. One
`.ld` line names the object and the `.s` carries no data, so the default
`asm/%.o: src/%.c` rule puts the object exactly where the script expects it.

Four of the five fell to the reopened interleave class with **no pins, no flags
and no barriers** — the uniform whole-value ascending fill closed them on the
first screen. That is the clearest evidence yet that the recorded straight-line
boundary was on the dominating-block lever and never on the shape itself.

Worth aiming at deliberately: when a `.s` holds a handful of same-family
functions, solving all of them beats splitting one out, and the linker script is
never touched.

## The claim I got wrong

A screening note reported that this batch refuted `elevation.md`'s entry
*"Constant CSE inside ONE basic block: closed"* — which says, of a repeated
constant ≥ 256 in one straight-line block, *"there is no spelling. Stop sweeping
and park it."* I repeated that framing into a commit message **before reading the
entry**.

It is wrong and the entry is right. The entry scopes itself explicitly — *"Only
same value, 256 or more, one straight-line block is closed"* — and carves out, in
its own text, that repeats separated by a control-flow boundary are reachable.
The cure that shipped assigns two locals **above a dominating guard**, which puts
them in a different basic block. That is the entry's own exemption, not a
counterexample to it.

I caught it only because I had just told the user I would verify it, having got
exactly this wrong earlier in the day with the twins tool. **Scepticism toward an
entry that closes a class is right; but scepticism means reading it, and its
scope line answered the question in one sentence.**

What survives is narrower and still worth having: the dominating-block lever
works here on **two copies of one value**, where it is recorded as ordering two
*different* ones. And the closure argument is about *spellings* — every C
spelling folds to the same `const_int` before cse runs — so it says nothing about
register pins, which constrain allocation rather than respell a value. Whether
pins in a genuinely single block refute it is **still open** and was not settled
here.

## A wildcard that only ever trapped the future

`rom_7ef4f4/ovl_30_a_c_c_c_c_c%` appeared **thirty times** with an identical
`-O1` recipe. It captures six TUs — and four already carried explicit rules
overriding it, so only two ever took `-O1`, and those two still do. Its entire
live effect was on files that did not exist yet.

`OvlFunc_965_2009238` was the first to reach it: **286 differing at `-O1`,
byte-identical at `-O2`**. It would have screened green and built red, with
nothing in the source to explain the difference.

The screening note proposed a fourth explicit override in that one directory. I
narrowed the pattern at source instead — thirty copies replaced by two explicit
rules for the only stems that took `-O1`. Verified three ways rather than
assumed:

- `make compare` green afterwards: the six existing TUs are unchanged.
- A hypothetical new stem member resolves to the tree default, while the genuine
  `-O1` file still resolves to `-O1`.
- The function now verifies **OK against its original `asm/` path**, where it
  mismatched before.

That third check is why the two changes shipped in one commit, and it carries
past this function: **a mismatch measured against a path under a too-broad
pattern rule is a tooling artifact, not evidence about the source.** This is the
second such wildcard narrowed in a day, which suggests whatever generated them
emitted duplicates per overlay group rather than once.

## The pin sweep has a direction

On `OvlFunc_926_2008518`, drops unlock **in call order**, and the payoff is at
the end of the chain:

```
3/3/3 pins, SetSpeed on the hand-placed-zero special case
  → site 4 drops q0
  → site 4 drops q2, split build bare
  → the hand-placed zero now measures INERT, plain uniform fill matches
  → SetSpeed drops q2
  → fixpoint at 3/2/1
```

The special-case *spelling* looked load-bearing at every earlier step while it
was really absorbing scheduling pressure from an over-pinned site twenty
instructions later. A sweep that halts at "this drop regresses" ships that
special case plus three inert pins and reports success.

**Sweep front to back, and after any successful drop re-open every later site
including its spelling**, not just its pin list. Recorded already: a pin can be a
symptom of a different defect — where that defect was an aliasing one. Not
recorded: the other defect can be another pin of the same class.

It also bounds the refinement from last batch. *"Pin the single-instruction
arguments, leave the split build bare"* is **fully inert** at this function's
first site — 41 differing, no better than no pins — because there the split build
*is* the shared constant. Where those collide, the shared-constant rule wins.

And the four interleave sites were not the defect at all: plain literals already
had the entire eight-argument tail exact, and the whole residue was one constant
CSE'd into a callee-saved register at the function top. The class got it into
reach; a single first-use pin was the fix.

## Naming only the stack pair merges the address chains

Where a six-argument call's **register** arguments also live past the
`[rN,#imm]` limit, both groups need a chain. With only the pair named, gcc emits
one pointer that walks to it and then `sub r3, #0x14` back — five wrong
instructions per site. Naming all six **values** gives two chains born back to
back, as the ROM has them: 27 → 9 → 4.

The cure is naming the values, not introducing pointers: a record pointer
measures 39, an `int *` per chain 57. Boundary: at sites where the register
arguments reach through plain `[r7,#imm]`, the pair alone is right.
