# Batch 156 — five elevated, and a boundary that reorganised the backlog

Clean `make clean && make compare` green from scratch. All five addresses
checked against the linked ELF.

| function | address | file |
|---|---|---|
| `Func_80b9a70` | `0x080b9a70` | `src/rom_b5000/rom_b8228_c_a_c_c_c_b.c` |
| `UpdateRespawnMap` | `0x0808b1d8` | `src/rom_8a000/rom_8ace0_a_a_c_a_c_b.c` |
| `Func_801fda8` | `0x0801fda8` | `src/rom_15000/rom_1de5c_c_c_c_c_c_b.c` |
| `Func_80907b0` | `0x080907b0` | `src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_a_c_b.c` |
| `Func_8092980` | `0x08092980` | `src/rom_8a000/rom_92950_a_a_c_a_a_b.c` |

## The finding: LIVE LOCALS versus ARGUMENT TEMPORARIES

The batch opened by producing the split-constant interleave on demand for the
first time. The ROM starts a two-instruction constant build, does one
instruction of unrelated work, then finishes the build. The lever is to split
the build across two source statements and put the independent work between:

    flag = 0x80;        instead of      flag = 0x80 << 1;
    i = 0;                              i = 0;
    flag <<= 1;

That landed `Func_80b9a70`, which had sat at 5 differing with the shape twice.

**Then it failed everywhere else, and the reason is the useful part.** The
lever only reaches constants that OUTLIVE their construction. gcc
rematerialises argument temporaries during argument fill and discards whatever
statement structure the source imposed. Measured on `ovl_7cb2c0/200dca4`
(residue unchanged character for character) and on `ovl_780898/2008fec`, where
four spellings — baseline, each call site's arguments named, both named — came
back BYTE IDENTICAL.

This re-explains a whole park class. Those parks concluded the missing
ingredient was a basic-block boundary, so straight-line cutscene scripts were
unreachable. The real discriminator is lifetime; a branch would not have saved
them.

It also became PREDICTIVE. On `OvlFunc_953_2008648` the residue was predicted
at 6 before any C was written — three interleave sites at two lines each — and
the first screen returned 6 of 43, length exact. `tools/pickable.py` now prints
a `>=2N differ` floor per candidate.

## The candidate class the filter was rejecting

`pickable.py` rejects anything with fewer than 5 calls. That rule was written
for 8-to-20 instruction bodies with no register pressure, but the SIZE rule
(prefer 40-120) already covers those. The call rule was additionally excluding
**51 loop-carrying functions in the 40-120 band** — and that is exactly where
the levers work, because a loop body is full of live locals while a cutscene
script is full of argument temporaries.

Four of the five elevations came out of that class. `UpdateRespawnMap` (51
instructions, ZERO calls) matched on the first screen.

**The screening signal, from eight worked functions:** count the values live
across the whole body. At about three, gcc's allocation is forced and matches.
At four or more, the ROM typically spends one more callee-saved register than
gcc needs — and these functions have no calls, so gcc is right and no spelling
argues it out. Four of eight matched; the four that stalled (`Func_80f4100`,
`Func_8029274`, `Func_80c0228`, `DecodeMetatileset`) all stalled on the same
whole-function register rotation. Use the ROM's `push` list as the proxy and
read register usage BEFORE writing C.

## A reading rule that became predictive in one round

Where the ROM COPIES a register before modifying it, the original had TWO live
names; where it modifies in place, one.

Derived from `Func_80c0228` (`mov r2, r3 / add r2, #0xd` against our in-place
`add`), where splitting one expression into two named locals bought 4 lines but
did not finish. Applied to `Func_8092980` on the next round it closed the
function outright: the ROM's `ldrb r3,[r3] / cmp / beq / mov r1, r3` said the
count and the loop counter were two names, and 23 differing went to zero on
that single edit.

## Two tooling faults, both self-inflicted

**A corpus scan returned a false zero, the second time from this cause.** A
scan for the interleave over `asm/**/*.s` reported 0 sites. The `.s` files
separate mnemonic from operands with a TAB, so a regex with a literal space
matches nothing. Corrected to `\s+`: **3846 sites across 555 functions**.

It was caught only because the zero contradicted a site read on screen minutes
earlier. Two consequences worth carrying: `tryc.py` renders BOTH columns
through a disassembler, so patterns lifted from its output exist in no file;
and gcc's own generated output uses the same tab separator — a claim written
into the docs that generated output was immune was wrong and was corrected by
one compile. `ovl_7c460c/2008c74` rests on such a zero and is now flagged.

**tryc's count cannot be read at face value when the pool is inside the
function.** `Func_801fda8` screened as 6 DIFFERING and was byte-identical: gcc
emitted two labels at one address and the streams misaligned across the tail.
The artifact does not only inflate the count — it puts the divergence at the
very END, where it reads like a real epilogue problem. A residue that is only
labels and only in the tail is the signature.

## Parks

Eight, each with its measured negatives: `ovl_7fb4a8/20087b0` (20 of 67, all
control flow exact — the REG_IME ordering class, reachable only as a fakematch
and deliberately not taken), `ovl_780898/2008fec` (9 of 91), `ovl_7d95dc/2008648`
(6 of 43, predicted), `rom_f4000/80f4100` (39 of 54, worth two functions —
`Func_80f6038` is instruction-identical), `rom_9000/80110e0`, `rom_15000/8029274`
(6 of 47), `rom_b5000/80c0228`, `rom_9000/800f9f4`, `rom_9000/80119cc`.

Notable negative from `DecodeMetatileset`: the ROM RELOADS one register for its
source pointer, which reads as one variable reassigned rather than two typed
views — modelling it that way made it WORSE. Matching the ROM's register reuse
is not the same as matching its register allocation.

## Process

On `Func_8029274` four edits were derived from one diff and applied together:
12 differing became 25, reading as "all four wrong". Isolated, three were
correct and one was catastrophic. **Change one thing at a time.** Its corollary
is a lever: the bad edit existed only to obtain the ROM's signed `bge`; casting
only the COMPARISON — `while ((int)p >= (int)buf)` — buys the same branch and
leaves the body as pointer dereferences.
