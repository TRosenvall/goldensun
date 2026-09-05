# Batch 234 — the scan finds a shape, and shape is not diagnosis

Eight functions elevated and two parked. The unifying result is negative and about
our own tooling: of nine functions aimed at the interleave bucket this round and
last, **eight had the silhouette and a different cause**.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `Func_808b868` | `0x0808b868` | [rom_8b674_c_a_a.c](src/rom_8a000/rom_8b674_c_a_a.c) |
| 2 | `Func_808b8e8` | `0x0808b8e8` | [rom_8b674_c_a_a.c](src/rom_8a000/rom_8b674_c_a_a.c) |
| 3 | `Func_80ad274` | `0x080ad274` | [rom_ad274_a.c](src/rom_a1000/rom_ad274_a.c) |
| 4 | `Debug_WarpMenu` | `0x08028f98` | [rom_23178_a_c_b.c](src/rom_15000/rom_23178_a_c_b.c) |
| 5 | `Func_801f77c` | `0x0801f77c` | [rom_1de5c_…_a_c_a_b.c](src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_b.c) |
| 6 | `Func_808d394` | `0x0808d394` | [rom_8ba38_a_c_b.c](src/rom_8a000/rom_8ba38_a_c_b.c) |
| 7 | `Func_80c0be4` | `0x080c0be4` | [rom_bffb8_a_c_c_a.c](src/rom_b5000/rom_bffb8_a_c_c_a.c) |
| 8 | `Func_80c0cec` | `0x080c0cec` | [rom_bffb8_a_c_c_a.c](src/rom_b5000/rom_bffb8_a_c_c_a.c) |

## Parked

| function | address | file | blocker |
|---|---|---|---|
| `OvlFunc_888_200a7d4` | `0x0200a7d4` | [200a7d4.c](src/non_matching/ovl_7892c8/200a7d4.c) | two identical `add rN, sp, #K` in one block |
| `UpdateScreenEdge_H` | `0x0800ff54` | [800ff54.c](src/non_matching/rom_9000/800ff54.c) | register-role rotation |

## Shape is not diagnosis

I have been aiming agents at the interleave bucket and describing the hit rate
as though the classifier identified the defect. It does not. What each target
actually turned out to be:

| function | flagged as | actually was |
|---|---|---|
| `Debug_WarpMenu` | interleave | one zero in the wrong register |
| `Func_801f77c` | interleave | loop preheader / induction form |
| `Func_808d394` | interleave, no memory ops | a constant built in a loop; all memory ops |
| `Func_80c0be4` | interleave | a constant commoned across two call sites |
| `OvlFunc_888_200a7d4` | interleave | both sites correct from a bare call |
| `UpdateScreenEdge_H` | interleave | symbol pools hoisted out of a loop |

The scan is a **triage filter**, not a classifier. It is still worth running —
the hit rate is real and these are functions nobody would otherwise have picked
— but the first move on any hit must be to diagnose, not to apply the class
cure. Every prompt now says so.

## A published "unreachable" is refuted

`elevation.md` states twice, naming a function, that `ldrb` + `lsl #24` before
`cmp #0` cannot be reached from C, generalising to any narrowing shift before a
zero test. **It is reachable.** Isolated A/B in one file, same flags, same
semantics:

- walking **pointer** (`p += 0x40; p[0]`) → `ldrsb`
- walking **index** (`k += 0x40; p[k]`) → `ldrb` + `lsl #24` + the interleaved
  `add` the park blamed on the scheduler

The ten recorded probes varied operand type and comparison form exhaustively;
**none varied the induction form**. The sibling park is corrected in place and
its residue drops from 8 differing to 3.

This is the third such wall this session, and they share a shape: an entry
telling the reader to stop looking, whose probes exhausted the obvious dimension
while missing the one that mattered. **An entry that closes a class deserves
more scepticism than one that opens a lever**, because nobody re-measures what
the notebook calls finished.

## A recorded tell is a flag tell first

The doc reads an `ldrsh` + `ldrh` pair on the same halfword as proof the source
wrote a guarded `do`/`while` rather than a `while`. `Func_808d394` has that pair
and both spellings are **byte-identical** — what restores it is `-fno-gcse`.
Read it as a flag tell first, guard tell second.

## Two blockers, both corpus-verified

`OvlFunc_888_200a7d4` parks on a **new blocker class**: gcc-2.96 cannot emit two
identical `add rN, sp, #K` in one basic block. Counted over the tree:

| corpus | files with the pattern |
|---|---|
| ROM disassembly | **37** |
| gcc-generated (3753 total) | **0** |

Same signature as the recorded `add rHIGH, rN` blocker. Three escapes were
chased into the compiler source and closed: block-scoped slot reuse returns the
same slot rtx; gcc-2.96 spills rather than rematerialising a frame address; and
jump2 cross-jumping refuses to merge a tail ending in a call. Seventeen flags
swept, all inert.

Its park also records a **near miss that is wrong for the right-looking
reason** — a discarded struct-returning call leaks a stack slot per site and
reproduces the ROM's uncommoned address computations exactly, reaching 4
differing, but the callees take that register as a real object pointer.

`UpdateScreenEdge_H` reached **48 differing with the size exact** — pool words in
the ROM's order, all relocations at the ROM's offsets — with the whole residue
downstream of one four-way register rotation. Its park records that a hill climb
over all 9! preheader orderings bottoms out at 48, and that `-Os` matches the
line count by coincidence only.

## Mechanisms worth keeping

**The hoist count is arithmetic on the loop's RTL size.** `move_movables`
decrements its threshold by 3 after each move, so with equal savings the tests
run 30, 27, 24… against a fixed insn count. The cure for one-too-many is *one
more loop-varying RTL insn* — and three independent spellings of that same +1
produced byte-identical objects, which is what makes it a measurement rather
than a story. The recorded threshold of 15 is the has-call value; **without a
call it is 30**.

**A call-saved register says which variable, not "name it".** gcc-2.96 has no
live-range splitting, so one C variable is one pseudo. `Debug_WarpMenu`'s whole
residue was a zero in r3 where the ROM has r7; the cure is a redundant
initialiser on the *variable that owns it*. Four spellings of the zero are
byte-identical; a register pin measures 18 differing.

**Where a lever applies, predicted rather than tried.** `Func_808d394` uses the
recorded "read the field twice" trick in one loop arm only, because
`cse_around_loop` fires only when the insn before the loop-end note is the
back-jump with a single-use label. The other arm ends with a conditional branch
plus an unconditional jump, so that pass is skipped — and giving it the second
read measures *worse*.

## Two method notes

**Rank on which lines differ, not how many.** A false minimum in `Func_801f77c`
scored 7 against the correct order's 11 while emitting stores in the ROM's
reverse order — it could never reach zero.

**A differing-line count can understate a size defect.** The same function's
pool-dump difference read as "3 differing lines" but was two bytes short, having
lost the ROM's alignment pad.

## A blind spot in the authority tool

`objcmp` reported `Func_80c0be4` and `Func_80c0cec` as failing on relocations.
They are **byte-identical**: against the unmodified reference it prints *only*
`XX RELOCATIONS differ`, with SIZE and ENCODINGS silent. The difference is that
gcc emits `bl _call_via_fp` where the ROM's `.s` writes `bl _call_via_r11` — and
objcmp compares relocations **by symbol name**. Verified rather than accepted:

```
arm-none-eabi-nm src/lib/call_via.o
0000002c T _call_via_fp
0000002c T _call_via_r11
```

One symbol, one address, two names. objcmp is *the* authority in this workflow,
so a false negative there is worse than one anywhere else — it can send someone
to park a function that already matches. **Anything previously screened against
a `_call_via_*` target deserves a second look.**

## One site, two opposite cures, inside a matched pair

At the same call, `Func_80c0be4` wants a bare fill (the pinned form costs 20)
while `Func_80c0cec` wants the pinned form (bare costs 2). A hoisted constant is
worth 13 on the first and measures **inert** on the second.

That is "re-measure, never transplant" appearing *within* a near-identical pair
rather than across a family — about as tight a demonstration as the rule gets.

The screening side also caught itself: a file-scope register reservation looked
like the lever at 12 against 22, but was masking an unfixed defect elsewhere.
Against the finished source both spellings measure zero, so it was stripped.
"A pin can be a symptom of a different defect", applied unprompted.

## A manual split, done in two steps on purpose

The last two are a contiguous tail pair, which `split_s.py` cannot cut in one
go. Truncating the `.s` and dropping in the `.c` together would fuse the layout
change with the decompilation into one untestable step — so the pair was moved
into its own `.s` first and *that* proved green (byte-neutral redistribution)
before the `.c` was swapped in. After that point a failure could only be the
decompilation.

