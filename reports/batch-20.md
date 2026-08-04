# Batch 20 — 5 functions, and a screen that called a wrong function right

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–19 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean build, unassisted. Every address read back from the linked ELFs.

## Read this if you screen candidates with your own tooling

`tools/tryc.py` reported a **clean match on a function whose C was wrong**.
This is the first time in twenty batches that the screen has failed in that
direction, and it is the failure mode worth knowing about.

It normalised every label to `L<n>` in first-appearance order and then dropped
the definitions, on the reasoning — written in its own docstring — that *"their
position is implied by branch order"*. It is not.

`OvlFunc_931_2008360` compared equal on all 35 instructions, in order, and then
differed from the ROM by exactly one byte: a `beq` whose offset was `0x02` in
the ROM and `0x06` in ours. Same mnemonic, same normalised target, different
encoded distance, because the label sat two instructions further along. Both
streams had `beq L3` at the same index and, with the definitions dropped, there
was nothing left to disagree about.

**And the C was genuinely wrong.** Flag `0x909` guards only the extra
`__MessageID`; the `__ActorMessage` after it runs either way. I had put both
inside the guard — the game would skip a line of dialogue. That is a semantic
error, and the screen existed precisely to catch it.

Fixed: label definitions that something branches to are kept in the compared
stream, so a target's *position* is part of the comparison. Definitions nothing
references are still dropped, since gcc leaves those behind after pool
resolution and the disassembly does not.

### The rule, which is worth more than the fix

**When you add a normalisation to a screen, ask which direction its failure
runs in.**

Every other class found in this tool — eight now — reports a *correct* function
as wrong and costs a round. This one reported a *wrong* function as right. Those
are not the same kind of bug, and the normalisations that cause the second kind
are the ones that look most obviously safe. "The label position is implied by
branch order" reads as a true statement about assembly. It isn't.

### What was and was not at risk

Nothing shipped wrong. Every elevated function passed `make clean && make
compare`, which is byte-exact — the build caught what the screen missed, which
is the gate working as designed rather than luck. All 85 parked functions were
re-screened with the fixed tool and no verdict changed, which is also expected:
the bug could only turn a mismatch into a match, and a parked function is
already a mismatch.

## The functions

| `OvlFunc_900_2008094` | `0x02008094` | `src/overlays/rom_797740/ovl_30_c_c_a_a_c_a.c` |
| `OvlFunc_931_2008360` | `0x02008360` | `src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_a_b.c` |
| `ActorCmd_SetAttr` | `0x0800e9a0` | `src/rom_9000/rom_e220_c_c_b.c` |
| `ActorCmd_IncAttr` | `0x0800e9dc` | `src/rom_9000/rom_e220_c_c_c_b.c` |
| `ActorCmd_CmpAttr` | `0x0800ea18` | `src/rom_9000/rom_e220_c_c_c_c.c` |

`OvlFunc_900_2008094` is the one worth reading for method: sixteen calls in
forty-four instructions, matched first attempt. **Call-dense functions are
easier than small ones**, because each call pins the schedule around it. The
ranker has said so in its docstring since batch 01 and I had drifted into
taking its lowest-scoring entries instead, which favour small — and small is
where the one-instruction residues live. Three flat rounds ended when I stopped.

It also needs both halves of the declaration rule at once: `__ActorMessage` and
`__Func_809259c` declared so their `r0` is filled first, `__Func_809280c`,
`__Func_80925cc` and `__Func_8092848` left undeclared so theirs is filled last.

## A new blocker class, and what the number is not

To be precise about the direction: **the ROM's double load is the target, not
the defect.** What blocks is that gcc will not produce it.

    rom    ldr r0, =0x303 / bl __GetFlag  ...  ldr r0, =0x303 / bl __SetFlag
    ours   ldr r5, =0x303 / mov r0, r5 / bl __GetFlag  ...  mov r0, r5 / bl __SetFlag

gcc sees one value used twice, hoists it into a callee-saved register, and must
then `push {r5, lr}` where the ROM pushes `{lr}`. Two extra instructions and a
different prologue.

**839 hand-written functions exhibit the shape.** That is a count of functions
where the ROM loads one pooled constant more than once — *not* a count of
blocked functions, and it should not be read as one. Whether any given one is
blocked depends on whether gcc would CSE in that context, and it demonstrably
does not always: gcc reloads rather than CSEs in 68 functions of its own honest
output. But in **every one of those** the repeated
value is a global's address that then gets *dereferenced*, so the reload is
forced by the call possibly having changed memory, not by anything the source
did.

A hypothesis that failed, recorded so it is not retried: that the operand was a
symbol, by the same argument as the pool tell. It is not — gcc CSEs a symbol
address just as readily when the address is only passed and never dereferenced.
A `flag.sym` was written and wired into `stage1.ld` before that was tested, and
deleted rather than left asserting a namespace on an inference that did not
survive.

## Two guards added, both from mistakes in this batch

- **`tools/asmfacts.py --orphans`** reports linker-script `.o` references with
  neither a `.s` nor a `.c` behind them. A commit that deletes source can pass
  `make -j8 && make compare` on a stale object; only `make clean` catches it.
  Three overlays were committed in that state.
- **`carries_data()`** distinguishes "one function in the file" from "nothing
  else in the file". It had a false positive of its own — it required a newline
  straight after the function name, and a trailing `@ 0x0800ea18` comment made
  a clean file read as data-carrying. A false positive there blocks a
  conversion that is fine and looks like a fact about the ROM.

## Still open, and still only answerable by you

- **Five ambiguous offsets in `actor.h`** (batch 03), documented rather than
  guessed.
- **`narrow_constant`**, 34 functions, down to one peephole.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
