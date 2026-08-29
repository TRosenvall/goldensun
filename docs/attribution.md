# Attribution

What this project learned from elsewhere, and where the line is drawn.

## The line

Three different things get called "copying" and only one of them is a problem.

**Tools** are for using. `pret/agbcc` and `camelot-gcc` are GPL'd compiler
distributions published for reuse. Building with them is no different from
building with GCC.

**Facts** are not ownable. "Camelot used gcc-2.96" and "the flag set includes
`-fcall-used-r4`" are facts about a 2001 ROM, independently discoverable by
anyone with a disassembler and patience. Using them is research. Saying where we
learned them is courtesy, and it is done below.

**Expression** is theirs. Another project's decompiled C, its prose, its
annotations -- that is work product, and copying it would be plagiarism.

## Working rule

**Superseded 2026-08-03.** Coaltergeist has confirmed that this project may
read and take from `goldensun-decomp` freely, and everything produced here goes
back to that repository. The restriction below no longer applies, and their
`src/` is now read directly.

This is recorded rather than quietly deleted, because the history before this
date was produced under the old rule and the distinction still matters to
anyone reading it: work committed before 2026-08-03 was derived from
disassembly without reference to their C; work after it may not be.

The rule as it stood:

> **We do not read another decomp's `src/` while writing our own C.** That is
> the line that actually matters, and it is the one we hold.

It cost real time — three rounds went into working around a question that one
look at their `src/` settled in minutes — and it was still the right default to
hold until the permission was explicit rather than assumed.

Everything else -- their README, their toolchain, their published findings -- is
fair to read and is credited here.

### On convergence

Matching decompilation is tightly constrained. For many functions there is
essentially one C formulation that produces the required bytes, so two people
working independently often arrive at nearly the same source. That is
convergence, not copying, and it will happen here.

It is indistinguishable from copying by inspection alone, which is why the rule
above is about *process*: our git history shows each function derived from its
disassembly, and the `src/` of other projects is not consulted.

## What we learned from others

### Coaltergeist/goldensun-decomp and camelot-gcc

An independent Golden Sun decompilation, considerably further along than this
one, and the `camelot-gcc` toolchain repo that accompanies it.

We learned from its public documentation:

- **The compiler.** Patched gcc-2.96, arm-elf, Debian 2000-07-31 dev snapshot --
  the branch between FSF gcc-2.95 and gcc-3.0. We had narrowed this to
  "GCC-family, later than agbcc's 2.9" independently; they had identified it
  exactly.
- **The flag set**, and that `-ffixed-r7` is unnecessary under gcc-2.96 because
  it avoids r7 naturally.
- **The codegen fingerprint list**, which explains two obstacles we had
  documented as open: small-constant literal-pool preference (an `unsigned
  short` halfword target pools natively) and MULT-by-non-power-of-two lowering
  (a gcc-2.96 cost-model behaviour).

`camelot-gcc` is used as our toolchain. It vendors FSF GCC under GPL v2 and its
README offers it for other Camelot GBA decomps.

**We have not read their `src/`.** Their matched C is their work.

Both projects descend from the same disassembly, `gsret/goldensun`.

### pret

`pret/agbcc` was this project's compiler before the gcc-2.96 identification, and
its `thumb.h` is where the Thumb register-offset limitation was traced. The
maintainer's response to our bug report -- that agbcc does not cover every
compiler real games used -- was correct and redirected the investigation.

A comment on the pret Discord, that Camelot likely compiled their own gcc-2.9
rather than using the SDK branch, is what turned that from a dead end into a
line of enquiry.

`decomp.wiki` supplied the precedent that vendor-forked GCC is the norm rather
than the exception -- KMC on N64, PSY-Q on PSX, SN Systems on PS2 -- and pointed
at `asm-differ` and `decomp-permuter-agbcc`.

### Camelot GBA compiler research

Credited in camelot-gcc's README and repeated here because the findings are
load-bearing for this project: **FutureFractal** identified the GS1 compiler as
stock GCC 3.0-era; **Tarpman** documented the codegen fingerprints in 2021;
**Karathan** published the working flag set.

### SAT-R/sa2

The stock m4a ("Sappy") audio engine is shared across GBA titles, and the sa2
project's reverse-engineering of it is the reference for that subsystem.

## What is ours

- The annotations: prose on all 5,642 functions, including all 3,540 overlay
  functions.
- The overlay analysis: the six-slot export contract, the pushable-log puzzle,
  the save-bit idioms, the counter facing arcs, the per-map story-state
  structure.
- The data layouts in `include/`.
- The docs: matching technique, menus, sound, overlays, this file.

The annotation layer in particular has no counterpart in the projects above.

## A finding about our own annotations

Porting the annotation layer onto this tree surfaced a quality problem in it
that is worth stating plainly.

Where both projects had named the same function, we disagreed at **506 of 515
addresses**. Most of that is naming convention — our `FadePaletteToHalf` is
their `BlitFade_Div2_ROM`, and we mean the same thing. But narrowing to
addresses where both names carry a recognisable domain, **35 of 100 conflicted
outright**, and spot-checking showed a consistent pattern:

**our prose gets the mechanism right and the purpose wrong.**

Two examples, both verified against the disassembly:

- `0x543c` — we wrote "allocate a scratch, DMA3-copy a decoder template into
  it, call it there, release it." The mechanism is exactly right; it is a
  ROM-to-IWRAM trampoline. We called it decompression. Their `BlitFade_Add`,
  and the `BlitFade_Add_ROM` twin at `0x1fb8`, show it is a blit.
- `0x6910` — our prose describes sound channels and instrument loading. The
  disassembly is `REG_WAITCNT` masking, a call to `ReadFlashId`, and a u16
  truncation. It is `IdentifyFlash`. We were simply wrong.

This is what comes of characterising functions by shape rather than tracing
what they do — which is exactly how much of the annotation corpus was produced,
and it is the cost of the coverage. `docs/overlays.md` already warns that a
call trace says what a function reaches for, not what it means; this is the
same limitation showing up in the hand-written prose too.

### What follows from it

**Where this tree has already named a function, its name wins and our prose is
not added.** Their names read like recovered SDK symbols (`BlitFade_Div2_ROM`,
`DecompressLZ1_ROM`, `cam4aSoundMain`) against our inferences from reading
code. `tools/port_c_annotations.py` skips those 578 functions.

**Where nothing has been named, our prose is the only description available**
and is placed. That is the 3,197 functions in `asm/` and 79 in `src/`. It
should be read as a starting point for verification, not as established fact.

Anyone acting on a ported annotation should check it against the disassembly
before relying on it.
