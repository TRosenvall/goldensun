# Batch 194

Five elevated, three of them withdrawn from `src/non_matching/`, plus one park
rewritten with a sharper diagnosis.

Two results, and they pull in opposite directions. One is a **lever boundary**:
a single blocker name turned out to be covering two different problems, only one
of which a pin can touch. The other is a **false-residue generator** that has
nothing to do with the C at all — a Makefile wildcard applying the wrong
optimisation level to translation units that merely share a name prefix.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `OvlFunc_945_200bdec` | `0x0200bdec` | [ovl_30_…_a_c_b.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_c_b.c) | **fakematch**; *previously parked* at 2 of 26 |
| 2 | `OvlFunc_945_200dca4` | `0x0200dca4` | [ovl_30_…_a_c_b.c](src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_c_a_c_b.c) | **fakematch**; *previously parked* at 2 of 43 |
| 3 | `OvlFunc_949_2008894` | `0x02008894` | [ovl_30_…_c_c_b.c](src/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_c_c_b.c) | **fakematch**; first screen, six interleaves |
| 4 | `OvlFunc_968_20094f4` | `0x020094f4` | [ovl_30_…_c_c_b.c](src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_b.c) | **fakematch**; a Makefile rule, not a residue |
| 5 | `OvlFunc_968_2009150` | `0x02009150` | [ovl_30_…_c_a_b.c](src/overlays/rom_7f2f14/ovl_30_c_a_c_a_c_a_c_c_a_b.c) | **fakematch**; *previously parked* at 14, eleven of them flags |

Gated on a clean `make clean && make compare`, every address verified against
the per-overlay `overlay.elf`.

## ONE BLOCKER NAME, TWO DIFFERENT PROBLEMS

Three parks were filed under "arg-interleave", all at 2 differing. Two fell to a
pin on the first screen. The third, `OvlFunc_898_2008e0c`, **resisted six
structurally distinct pin forms** — either register pinned alone, both pinned,
initialised or not, and a folding expression — of which four are byte-identical
to the seven declaration spellings already recorded there.

Lining the residues up shows why, and gives a discriminator that costs nothing:

    reachable    mov r1, #0xa0 / mov r2, #0x28 / mov r0, #0x8 / lsl r1, #0x7
    reachable    mov r1, #0xd0 / mov r0, #0x8  / lsl r1, #0x8
    NOT          mov r0, #0x13 / mov r1, #0x0

In the first two the stray `mov` has to land **inside another register's split
build**. That is the pin moving its *own* register's mov relative to other
instructions, which is exactly what its second knob does. In the third the two
instructions are **independent movs** with no shift to sit inside, and which of
two independent movs is emitted first is the post-allocation scheduler's
business.

**`mov` against a `lsl` of another register is reachable; `mov` against a bare
`mov` is not.** Apply that before reaching for the lever.

One near-miss on the parked one is worth keeping, because it explains the wall
rather than just reporting it. A real data dependence *does* order the pair:

    p0 = 0x13;  p1 = p0 - 0x13;      ->  mov r0, #19 / mov r1, r0

The order is now the ROM's and the second instruction is not, because a
dependence is carried in a register. The only construct found that orders two
independent movs is one that stops them being independent. That is a cleaner
reason to park than "the spellings tie".

## A WILDCARD RULE IS A FALSE-RESIDUE GENERATOR

`OvlFunc_968_20094f4` screened at 14 differing and looked like an ordinary
scheduling problem — the kind that absorbs a long sweep. It was not. The TU is
caught by `rom_7f2f14/ovl_30_c_a_c_a_c_a%`, which applies `O1_CFLAGS`: **14
differing at `-O1`, exact at `-O2`.** Two siblings in that overlay already
carried explicit `-O2` overrides for the same reason.

`tools/tryc.py` found it, not I. It printed the diff and then warned that the
flags came from a wildcard rule that "may belong to a neighbouring TU that only
shares a name prefix", naming the re-screen to run. That warning turned a
plausible 14-instruction residue into a one-line Makefile rule.

**So the sweep was run over the whole park directory.** Thirteen distinct
wildcard rules apply non-default flags, and **nine parks sit under one**. The
two acted on this batch both yielded:

- `OvlFunc_968_2009150`, parked at 14 of 81, screens at **6** with the flags the
  build actually uses. Two of its three real problems were *invisible* at `-O1`,
  so no amount of spelling work at the inherited flag could have found them.

That is now the fourth false residue and the second false park from this one
wildcard, on top of the three the Makefile already records as "PARKED on the
strength of the inherited flag". **When a park's TU sits under a wildcard rule,
re-screen at the default flags before reading the residue as evidence about the
source.** Seven of the nine remain unexamined and are the cheapest work
available.

## THE HAND-ROLLED COMPARE IS A FALSE-CLEAN GENERATOR

While diagnosing `20094f4` I compiled by hand at plain `-O2` and wrote a small
normaliser to diff the streams. It reported **zero differences** on a candidate
tryc rated at 14.

Both halves of that were wrong in opposite directions. The flags were not the
build's, and the normaliser collapsed `ldr rN, <pool>` to a single token, hiding
exactly the class of difference that matters. The two errors happened to cancel
into a confident, wrong answer.

`tryc` already handles both — `resolve_pools()` folds gcc's explicit pool into
the ROM's `=value` shorthand, and `makefile_flags()` takes the per-file flags
from the *reference* path precisely because a scratch file has no rule of its
own. **The screen is the gate. A hand-rolled comparison is how you talk yourself
into a wrong answer**, and the only reason this one was caught is that the two
methods disagreed and the tool was believed over the improvisation.

## Smaller results

**A store can be interleaved into argument setup.** In `2009150` the ROM emits
`mov r1 / lsl r1 / mov r2 / str r1, [r5, #0x28] / mov r0 / lsl r2` — the stored
value and the second argument are *the same value*, so the store sits between
the halves of the fill. Pinning the argument registers and writing the store
between the assignments reproduces it, at two separate sites.

**The constant can be the `orr` destination.** The ROM loads into `r2` and
builds the constant in `r3`. Both `*p |= 1` and `*p = 1 | *p` give the roles the
other way round and are byte-identical to each other. One pin, on the
destination, matches. Torn down afterwards: the form that first matched pinned
both registers, and removing the second changes nothing, so it is not in the
file.

**Six interleaves in three distinct shapes, one screen.** `OvlFunc_949_2008894`
needed nothing new — pins for the fills, an undeclared `__Func_8092c40` because
the ROM fills `r1` before `r0`, and the guard written `== 0` with the long arm
as the `if` body. The lever is no longer being discovered; it is being spent.

## Correction carried forward

Batch 193 reported "~315" tracked compiler-generated `.s` files. The real figure
is **3,496 of 4,616** tracked `.s` under `asm/`, against **3,545** elevated `.c`
— close to one per elevated function. That is not a handful that escaped notice,
it is the tree's standing state, and it contradicts BRANCH.md at a scale that
makes "never commit compiler-generated `.s`" a question for the maintainer
rather than a rule I can simply follow. The batch-193 report has been amended.
