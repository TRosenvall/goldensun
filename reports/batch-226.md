# Batch 226 — three rules of mine, corrected by measurement

Seven functions elevated, six from agent screening. The batch's value is less
in the count than in what it did to the rulebook: **two entries I wrote in
earlier batches turned out to be stated too strongly**, and both were corrected
by functions that measured the opposite way.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_959_2009e94` | `0x02009e94` | [ovl_9dc_…_c_a_b.c](src/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_a_b.c) |
| 2 | `OvlFunc_950_200813c` | `0x0200813c` | [ovl_30_…_a_a_b.c](src/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_a_a_b.c) |
| 3 | `OvlFunc_882_200adec` | `0x0200adec` | [ovl_30_…_c_a_b.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_c_a_b.c) |
| 4 | `OvlFunc_910_20085dc` | `0x020085dc` | [ovl_30_…_a_b_c.c](src/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c_a_b_c.c) |
| 5 | `OvlFunc_952_2008674` | `0x02008674` | [ovl_30_c_a_a_c_c_c_c_b.c](src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_b.c) |
| 6 | `OvlFunc_926_200a7ec` | `0x0200a7ec` | [ovl_314_…_a_a_c_b.c](src/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_a_a_c_b.c) |
| 7 | `OvlFunc_932_2009398` | `0x02009398` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_a_c_b.c) |

## The loop rule was drawn from one function and does not generalise

Batch 224 recorded: *a ROM loop with the test at the top is not a `while`; use
`do { if (!c) break; … } while (1);`*. Two functions here point opposite ways.

**`OvlFunc_910_20085dc`** has the test at the **bottom** — `b .Ltest / body /
.Ltest: test / bne body`. That is `expand_end_loop`'s own rotation, so a plain
`while` is exactly right and my do/while(1) cure costs 22 differing.

**`OvlFunc_926_200a7ec`** has the top-test shape my rule was about — and
do/while(1) costs **111 differing** there. `while` and `for (;;)` + break fail
too, byte-for-byte identically. Only `label: if (c == 1) { …; goto label; }`
matches, because it is not a natural loop and `expand_end_loop` never sees it.

So there are three cases, the ROM's shape is visible by eye, and the top-test
shape admits **two** cures whose choice is function-specific. Measure both.

## The relocation check has a blind spot

Batch 225 recorded: *a symbol hypothesis is settled by relocations, not by line
count* — after a `(int)&_MSG_…` spelling scored one differing line and was
refuted by objcmp. That rule is right in the direction it was found, and
`OvlFunc_959_2009e94` shows where it inverts.

**The reference `.s` is a disassembly.** A symbol in the original was resolved
to its value before the `.s` was written, so the assembled reference cannot
carry a relocation for it *either way*. A correct symbol spelling therefore
shows as a phantom relocation the reference "lacks" — which reads exactly like
the refutation case.

That function pools `0xa1` where an eight-bit `mov` would do, with the
counterexample four instructions away (`mov r0, #0x62`). Ten literal spellings
all emit the `mov`; the halfword exception was checked first and does not apply.
`_CONST_a1` went into `const.sym` and **`make compare` passed**, which is the
authority for that class.

An extra relocation in the candidate refutes nothing on its own. What refutes a
symbol is a relocation pointing somewhere the ROM does not.

## What a pin is *for*, not where the blocks are

`OvlFunc_950_200813c` has **no branch at all** — one basic block end to end —
so the recorded "adjacent sites in one block both need pinning" boundary ought
to apply everywhere. It does not: the first-use rule still holds for two
constants while both sites of a third are load-bearing.

The distinction is what each pin is *doing*. A second pin that only destroys a
CSE the first already destroyed is inert; one that buys **argument ordering** is
not, because no earlier pin can supply that. And the polarity is not fixed —
on `OvlFunc_952_2008674` one value has the documented order while another is
reversed, its two sites straddling an `if`/`else` join.

## Other findings

**Scaffolding that measures inert must not ship.** `OvlFunc_952_2008674`
carried four named locals through its pin sweep; each was byte-identical when
removed, individually *and* combined. Leaving them costs nothing at the build,
which is exactly why it is easy to skip — but a local that measures inert is a
false claim about what the ROM required.

**A `_MSG_` symbol's tell is a length difference, and only for shifted bytes.**
Four message ids sit in `OvlFunc_926_200a7ec` and only one needs the symbol:
`0x17e0` is `0xbf << 5`, so gcc synthesises `mov`+`lsl` where the ROM has one
pooled load. The other three are not shifted-byte-representable and pool
unaided. The question is not "is this an id" but "can gcc build it with
mov+lsl".

**`int` versus `unsigned char` for bit locals.** On `OvlFunc_932_2009398`
widening two bit locals from `unsigned char` to `int` went from 24 differing to
2 — it fixed the allocation *and* three of four tied destinations at once. This
cuts against the recorded narrow-local rule for a commutative `orr`; both are
real, and which applies depends on whether one site or several share the local.

**Both cprop shapes on one variable.** `OvlFunc_959_2009e94` holds a message
base in r5 and uses it seven times — five live re-reads, then a destructive
`add r5, #6`. The recorded rule treats those as alternatives; here they are
consecutive uses of one value, needing the register pin *and* `m += 6;`.

**A same-file callee never blocks a split.** `.thumb_func_start` expands to
`.global \sym`, so a screening note that all three functions in a `.s` had to
land together was mistaken — `split_s.py` peels one out and the call becomes an
ordinary cross-object `bl`.

## Discipline

`tools/guard_generated.sh` fired on three of the seven commits. `git add`
aborted once more on a predicted split suffix — the third time this session —
after which every remaining landing listed `git status` and classified each
product as hand-written or generated before staging. That check costs one
command and has never been wrong; predicting the suffix has now failed three
times.

Five agents were lost mid-round to a session limit and relaunched at reset; all
five had barely started, so nothing was lost but wall-clock.

**And one self-inflicted scare worth recording.** A clean rebuild appeared to
fail with undefined references in an unrelated overlay's `imports.o`. The cause
was not the tree: I had started a second `make clean && make` in the same
working directory while the first was still running, believing it had died with
the machine. Two clean cycles racing over one object tree produce exactly that
signature — a link against half-deleted intermediates.

Two things follow. **Never start a second build in the same tree without
confirming the first is gone**; a background job that stops reporting is not
necessarily a job that stopped. And my rebuild wrapper sent `make -j8`'s output
to `/dev/null`, so the first real error was invisible and only surfaced later as
a confusing link failure — the same "silence hides the defect" mistake the
`tryc` flag bug taught in batch 222. Run it with the output visible when
diagnosing.
