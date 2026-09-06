# Batch 239 — the functions nobody had ever looked at

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_895_2008258` | `0x02008258` | [ovl_30_c_c_a_a_c.c](src/overlays/rom_78dee8/ovl_30_c_c_a_a_c.c) |
| 2 | `OvlFunc_895_2008f8c` | `0x02008f8c` | [ovl_30_c_c_c_a_c_c_a_c.c](src/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a_c.c) |
| 3 | `OvlFunc_924_20095e0` | `0x020095e0` | [ovl_f84_a_c_c_c_c_c_b.c](src/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c_b.c) |
| 4 | `Func_80df90c` | `0x080df90c` | [rom_de974_…_c_b.c](src/rom_c9000/rom_de974_c_c_c_c_c_c_c_c_c_c_b.c) |
| 5 | `Func_80b82c4` | `0x080b82c4` | [rom_b8228_a_a_b.c](src/rom_b5000/rom_b8228_a_a_b.c) |
| 6 | `SetBattleActorKnockback` | `0x080b8228` | [rom_b8228_a_a_a.c](src/rom_b5000/rom_b8228_a_a_a.c) |
| 7 | `OvlFunc_924_200a030` | `0x0200a030` | [ovl_1db4_a_b.c](src/overlays/rom_7ac2d8/ovl_1db4_a_b.c) |
| 8 | `OvlFunc_891_2008c8c` | `0x02008c8c` | [ovl_30_c_c_a_a_c_c_c.c](src/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c_c.c) |
| 9 | `OvlFunc_891_2008eb0` | `0x02008eb0` | [ovl_30_c_c_a_a_c_c_c.c](src/overlays/rom_78c76c/ovl_30_c_c_a_a_c_c_c.c) |

Nine is one over the 5–8 gate. It is one batch rather than two because a
5/4 split would put the second report under the gate, and because entries 6
and 9 were not targets at all — they arrived inside files opened for
something else, and separating them from their file-mates would misdescribe
how the work happened.

Entry 6 is the only NAMED symbol here, so its address was read from the
linked ELF rather than from its own name. That distinction is the whole
reason `checkaddr` exists: an address-named symbol carries its own answer
and "checking" it is a tautology that always passes.

## Parked

| function | address | file | blocker |
|---|---|---|---|
| `OvlFunc_924_2009db4` | `0x02009db4` | [2009db4.c](src/non_matching/ovl_7ac2d8/2009db4.c) | reload-register round robin — identical instruction multisets |

## TWO OF THESE HAD NEVER BEEN RANKED AS CANDIDATES

Entries 1 and 2 sit in files that a bare-substring test for `.gcc2_compiled.`
excluded from `templated.py` until the previous batch fixed it. Nobody had
ever looked at either. Both matched on the first round of work, and both
land whole — one function per `.s`, no split, no linker edit, no flag group.

That is the difference between a tool bug that costs accuracy and one that
costs work. The count was wrong by 126 functions, which was embarrassing;
the ranking was wrong by 126 functions, which was expensive.

## A SOLVED FUNCTION THAT WAS NEVER LANDED

Entry 3 was finished by batch 236. Its body and about forty screened
variants were sitting in `scratch_elev/b236/f20095e0/`, and batch 236
shipped five other functions without it: no `src/` file, no report row, no
`docs/elevation.md` line, and its own new finding uncashed.

Nothing in the tree recorded it as done, so it looked unattempted to every
later round and `templated.py` kept offering it as a 1.00 candidate.

**A solved candidate sitting in `scratch_elev/` is indistinguishable from an
abandoned one.** The batch report is what makes it real. Worth a sweep of
the other scratch directories for the same thing.

## THE RESIDUE'S RELOCATION LINE SORTS THE TWO PIN JOBS, WITH NO MIDDLE

Measured on entry 2, then confirmed independently on entry 8.

Dropping each required pin one at a time partitions them perfectly. An
ORDERING pin gives a small encoding count with SIZE **and** RELOCATIONS both
silent. A genuine CSE loss gives 23–600 encodings with RELOCATIONS **always**
differing.

SIZE is not the discriminator — it is silent on 16 of the 38 genuine CSE
losses in entry 2. The mechanism is simple once seen: a lost rematerialisation
moves every following `bl`, so every relocation offset moves, while a pure
argument transposition moves no byte offset at all.

This turns "read the residue's shape" into a line `objcmp` already prints.

## NOMINATE PIN CANDIDATES BY CALL-SITE FAMILY, NOT ONLY BY REPEATED CONSTANT

The recorded nomination rule is a CSE rule, so it systematically misses the
SINGLETON member of a uniform call-site family — which needs the same pin,
for ordering rather than for rematerialisation.

In entry 2 one callee is called 31 times with the same argument shape. Thirty
sites are nominated by the recorded rule; the one whose shifted byte is used
only once is not. Adding "same callee **and** same argument shape" nominates
exactly three extras and makes the set exact **with no residue read at all** —
and it minimises back to the same 48 pins the residue-driven route found.

## THE DESTRUCTIVE-ADD RULE IS A LOW-REGISTER RULE

Recorded: where the ROM's last use is a destructive `add rN, #k`, write
`m += k`, because gcc emits the destructive form only when the variable is
dead after it.

It does not reach r8–r11. Thumb-1 has no `add r8, #1`, so the destructive
form costs `mov` + `add` — **exactly what the fold costs**. The tie the
low-register case wins is a dead heat, and gcc takes the fold. The bare
register pin was tried first, per the recorded order, and is inert; only an
`"+r"` barrier closes it.

Two corollaries from the same function. The barrier's POSITION is part of the
lever: outside the pinned fill it acts as a fill-order anchor in its own right
and reverses the pair, so it must sit immediately before the read of the
barriered value. And its non-local cost runs further than recorded — the doc
has three instructions upstream, this measured **eight, across a whole `bl`**.

## A POSITIVE MULTIPLIER ALSO SELECTS THE SHIFT CHAIN

The only recorded note is that a NEGATIVE multiplier selects it. Probed in
isolation under production flags:

    return 0x50 * d;              lsl / add / lsl
    int k = 0x50; return k * d;   mov r3,#80 / mul r0,r0,r3
    two uses of k                 the 80 held in a PUSHED callee-saved reg

gcc-2.96 does not constant-propagate a named local into a MULT before expand,
so the local survives as a pseudo, the `mul` pattern is used instead of
`synth_mult`'s chain, and CSE then commons the constant across both sites.

So **`mov rN, #K` into a callee-saved register followed by two `mul`s reports
a named local in the source**, not a CSE curiosity. Worth 68 of 87 encodings
on entry 4 and 86 on entry 5.

## A BITFIELD IS A SECOND ROUTE OUT OF BLOCKER 1b, AT HALFWORD WIDTH

The recorded mask-width rule is stated for a BYTE container. It holds at
halfword container width too, and there the tell is DOUBLED: hand-masking
gives a narrowed `mov`/`lsl` for the clear-mask **and** an `ldrh` for the
value-mask pooled at halfword width, against the ROM's two full `ldr`s.

A 9-bit bitfield fixes both at once, because `store_bit_field` builds both
masks at int width, so neither is ever a HImode `const_int` and the
constraint ordering that IS blocker 1b never applies. 140 of 145 differing
to 2.

1b's recorded escape is "make the value SImode". The bitfield is a second
route to that, and the 1b section does not list it.

## THE `goto`-LOOP LEVER, HARMFUL AND NECESSARY IN ONE OVERLAY

In entry 8 the lever is actively harmful: 202 of 242 differing and **sixteen
bytes short**. A park in the same overlay NEEDS it.

The discriminator is already recorded — look for a rebuilt invariant in the
loop BODY. The park rebuilds its stack pair there; this ROM hoists it into
each preheader, so `goto` throws the hoist away. Two specimens either side of
one recorded rule, in one overlay, is about as clean as that rule will ever
be demonstrated.

Entry 7 is neither: its INNER loop wants `goto` and its outer does not.
Leaving the outer a real `do`/`while` keeps its LICM, so two constants still
arrive in high registers unaided — you hand-name only what LICM will not
lift, rather than the lever's blanket corollary. Both loops rewritten is 152
differing; only the inner is exact.

## A SHIFT-ADD CHAIN MUST NOT HAVE A NAMED TARGET

Assigning the product to a variable hands `expand_mult` a target pseudo, so
accumulator and result are two pseudos and the last add goes three-operand.
Written INLINE in the argument list there is no target, the accumulator IS
the result, and that is what lets a high register be the second operand of a
two-operand `add` — the ROM exactly.

## The park

`2009db4` reaches 21 differing with IDENTICAL INSTRUCTION MULTISETS — 301
lines against 301, 291 encodings against 291. Only placement differs.

Read out of `.18.greg`: the seventh argument's store needs a scratch for a
constant and takes r3; `allocate_reload_reg` resumes its round robin at
`last_spill_reg + 1`, so the eighth argument's reload takes r2 where the ROM
has r3. For the ROM to reuse r3, its constant cannot have been a reload at
all — it must have been a pseudo with a hard register, and no spelling
reaches that: naming it is const-propagated back to a literal, and r5–r11
are all committed. Six flags swept, all inert or worse.

## Method notes

**Two functions arrived by reading the rest of a file already open.** Entries
6 and 9 were never targets; they were the other function in a `.s` opened for
something else. Three such functions across two rounds, each completing a
file — the cheapest landing shape there is. Once a `.s` is open the marginal
cost of reading the rest of it is close to zero.

**A dispatch error, recorded because it cost real work.** Six agents were
launched and one was refused; the refusal was misread, so the wrong brief was
relaunched. Two agents then worked the same two functions independently while
entry 8's target went unscreened until a later relaunch.

The waste bought one accidental result worth keeping: the two agents reached
DIFFERENT source for the same two functions — one naming the multiplier `k`,
the other naming it `s` and assigning it after both lookups — and **both are
byte-identical to the ROM**. Byte-identity does not single out one spelling,
which is the standing reason a matching decompilation cannot claim to recover
the original source.

**A suggestion declined.** One agent recommended staging the regenerated
`asm/**.s` beside each new `.c`. `CLAUDE.md` forbids committing
compiler-generated `.s` from `asm/`, and every landing this session is
consistent with that rule rather than with the ~3700 such files the tree
already carries from earlier history.

**The stray `sed` is documentary damage only.** The `.gcc2_compiled.` strings
that caused the previous batch's tool bug appear in 107 hand-written `.s`
files, 150 times, and **zero times outside an `@` comment** — no instruction
or symbol was touched, consistent with `compare` passing. What was lost is
150 callee names in prose. Not recoverable without the originals, and the
corruption marker is better left visible than guessed over.
