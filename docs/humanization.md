# Humanization: the pass-3 and pass-4 playbook

How to get from *byte-matching C* to *C a human might plausibly have written*,
without losing the match. Written up in batch 320 from a worked example and a
comparison against `pokefirered`. Spans two passes of the owner's roadmap:

- **pass 3 — depinning**: remove shims (`reports/pass3-depin.md` is the worklist)
- **pass 4 — naming**: adopt Coaltergeist's names

...but the work is one activity, because the same evidence drives both.

## THE GOAL, AND THE THING THAT IS NOT THE GOAL

A byte-matching decompilation **is not the original source**. It is *a* source
that produces the same bytes under the identified toolchain. Where the original's
shape has not been found, the compiler's *strategy* ends up written into the
source instead — and every such place is an artifact.

> **The goal is to remove artifacts, not to remove matches.** Every edit in this
> pass must be measured byte-neutral before it ships. An edit that improves
> readability and breaks the match is not a trade-off; it is a regression.

### The measurement, and it is cheap

**The tracked generated `.s` beside a landed `.c` is the regression baseline** —
one rebuild rather than a full `make compare`. Compile the variant with the
production flags, normalise away directives and banners, diff the instruction
streams. **Zero differing lines or it does not ship.**

    /opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork \
      -mcpu=arm7tdmi -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 \
      -Iinclude -S -o /tmp/x.s <variant>.c

A worked harness is in `scratch_elev/human/run.sh`; `tools/crossfire.py` does the
same scoring against a reference when the function is still parked.

## THE ARTIFACT TAXONOMY

Four kinds, in increasing order of how hard they are to remove.

### 1. Free — a shim that does nothing

**Measured: 14 of 25 small pinned files are byte-identical with the pin simply
removed.** A pin naming the register a value would occupy anyway creates no
reload (batch 316 measured 14 such pins at plain call sites, all exactly inert),
and nobody applied that result backwards to the pins already in the tree.

**Do this first.** It costs nothing and carries no risk.

> Batch 320's sweep used an arbitrary 22-line cutoff and missed at least one
> result that way. **Re-run it with no line limit over all 617 fakematch rows.**

### 2. A number that is really the compiler's strategy

The ROM's `mov #0x81 / lsl #1` is gcc materialising **0x102**, because Thumb-1
`mov` takes an 8-bit immediate. Writing `0x81 << 1` in the source is transcribing
gcc's *output* as if it were intent.

The tree does this inconsistently, which is the proof it is an artifact. One
constant, across the landed call sites of a single function:

| spelling | sites | value |
|---|---|---|
| `0x81 << 1` | 82 | 0x102 |
| `0x80 << 1` | 14 | 0x100 |
| `0x101` | 12 | 0x101 |
| `0x102` | **11** | 0x102 |
| `0x100` | 2 | 0x100 |

**0x102 written as a shift 82 times and as the number 11 times**, chosen by
whichever made the local codegen match. A human does not spell one constant two
ways in one subsystem for reasons invisible in the source. One landed file even
carries `(0x81) << 1` with redundant parentheses — a search's footprint.

**The cure**: name the real value and derive the halved one from it, so the
magic number disappears and the idiom is labelled:

```c
mode = (EMOTE_LAYER_FIXED | EMOTE_KIND_SCRIPTED) >> 1;   /* = 0x102 >> 1 */
...
mode <<= 1;
```

**Verified byte-identical.** And note `0x81 << 1` does not even decompose
meaningfully: the real structure is `0x100 | 2`, and the shift *straddles that
boundary* (`0x81` is `0x80|1`). Nobody expressing "fixed layer, scripted kind"
arrives at `0x81 << 1`.

### 3. A wrong TYPE or DECLARATION — the big one

**This is the class with the most evidence behind it and the most instances.**
See `reports/pokefirered-patterns.md`: in all four of pokefirered's
`[LEAK-INFORMED] fix ... fakematch` commits — where they obtained the original
source and replaced a shim with what the human wrote — **the artifact was a wrong
type or declaration, and the fix corrected the type rather than adding a shim.**

> **So a pin is evidence about a DECLARATION, not only about an allocator.**

The four patterns, each transferable:

1. **Split a compound condition** — `sub_8113AE8` dropped
   `register const u16 *r0 asm("r0")` by writing two sequential `if`s instead of
   one `||`. Their comment states it: *"checks must be separate to match"*.
2. **Narrow the pointer type, take the wide access as a cast at the dereference**
   — `sub_812E768` dropped `asm("":::"r4")` with `u8 *` instead of `u16 *`,
   `*(vu16 *)p` at the use, `p--` instead of `(void *)p - 1`, and
   accumulate-into-a-local-then-store-once.

   **REFINEMENT, measured in batch 322 (worth 16 -> 7 on one function): THE CAST
   MUST BE ON A STORE THROUGH A NAMED POINTER.** `*(volatile unsigned char *)q = w`
   stops combine substituting into the insn, because combine will not substitute
   into an insn holding a volatile MEM. The *inline* form
   `*(volatile unsigned char *)(p + k) = w` measures **no change at all**, because
   expand folds the address and combine never sees two insns to merge. So the
   lever needs the pointer to exist as its own pseudo first — which is the same
   precondition as the volatile-cast bound already recorded here (a cast needs a
   pointer that already exists), arriving from the other direction.
3. **One expression with a pointer difference**, not hand-unrolled arithmetic —
   `battle_interface` replaced a transcription of gcc's own strength reduction
   (`4*v + v` for `5*v`) with `xPos = 5 * (3 - (objVram - (text + 2)))`.
4. **Declare the extern with its real type** — `CreateShedinja` deleted two
   helper pointers and a *"can't match it otherwise, ehh"* comment by declaring
   `extern struct Evolution gEvolutionTable[][EVOS_PER_MON]`, inner dimension
   included, after which normal subscripting produces the arithmetic gcc wants.

**Measured opportunity in this tree**, of 533 pinned files:

| signature | files | matching pattern |
|---|---|---|
| raw-offset arithmetic `*(T *)(p + N)` | **272 (51%)** | 4 |
| `extern T name[];` with no dimensions | **198 (37%)** | 4 |
| a pointer cast to `(u32)`/`(int)` | 106 (20%) | 3 |
| a compound `if (A \|\| B)` | 72 (14%) | 1 |

Half our pinned files do address arithmetic by hand. That is pattern 4's exact
signature, and pattern 4 improves the code whether or not it removes a shim — a
correctly-typed extern is better regardless.

### 4. Genuinely unreachable — the real ceiling

Some constructs have no C spelling under this toolchain. They must be kept, and
the right response is to **label them precisely** so no one re-litigates.

The worked example is the barrier class. `src/overlays/rom_7f21b8/ovl_30_a.c`
needs the slot materialised at a specific point to get the ROM's argument
interleave, and:

| attempt | result |
|---|---|
| `__asm__ volatile ("" : : "r" (slot))` | **0 — ships** |
| `volatile int slot` | 6 — forces a stack slot |
| `(void)*(volatile int *)&slot` | 7 — worse |
| `static __inline__` wrapper taking slot as a parameter | 2 |
| declaration reorder / comma expression / extern volatile | 2 |
| *(~27 formulations across three batches)* | — |

> **A VOLATILE CAST NEEDS A POINTER THAT ALREADY EXISTS.** pokefirered's
> `*(vu16 *)p` worked because the function already had a pointer to cast.
> Manufacturing one with `&local` defeats itself: taking the address forces the
> value to memory, which is the very thing `volatile` on the declaration does
> wrong. **So pattern 2 applies to pointer-walking code and not to
> argument-setup ordering.**

## NAMING (pass 4), AND ITS LIMITS

Names must come from evidence in *this* tree, never invention:

- **The callee's real definition.** `MapActor_Surprise(int slot, int mode)` is
  defined in a landed file; that gave both parameter names and the `int` types.
- **The callee's body.** Reading it gave the flag decomposition: `mode & 3` is
  the emote kind, `mode & 0x100` chooses fixed-vs-inherited layer bits. All four
  combinations are in use across the ROM, so it is a designed flag set.
- **The house style**, which is well attested across ~400 named functions:
  `MapActor_WaitAnim`, `ActorCmd_Wander`, `RaisePartyLevels`, `CheckSpecialExits`.

And two hard constraints:

> **Do NOT invent a `*.sym` entry for a value the ROM does not pool.**
> `const.sym` exists only for constants the ROM pools, because "gcc-2.96 never
> pools a constant it can build with an eight-bit `mov`". An 8-bit-movable index
> like `0xe` is therefore not covered and naming it in the table would be a guess
> dressed as evidence. Put it in a header `#define` instead, flagged as unnamed.
>
> **Do NOT read a Golden Sun decompilation's `src/`** (CLAUDE.md). Other games'
> decompilations are fine — that is how the patterns above were found. Pass 4's
> function names should come from Coaltergeist's published table, not from our
> inference; a placeholder in house style is fine until then.

### A missing namespace worth building

Every `GetFieldActor` call site in the tree passes a bare slot index. A
**field-actor slot namespace** would pay off far beyond any one function. Same
shape as the existing `_AREA_*` and `_FILE_*` tables, but it is a header of
`#define`s rather than a `*.sym` file, because the indices are not pooled.

## THE WORKED EXAMPLE, END TO END

`OvlFunc_967_2008030`, all three stages verified at **0 differing lines**.

**Before** — two shims, three magic numbers, meaningless types:

```c
extern void __MapActor_Surprise(unsigned int a, unsigned int b);
int OvlFunc_967_2008030(void)
{
    unsigned int w;
    w = 0x81;
    {
        register unsigned int rq __asm__("r0") = 0xe;
        __asm__ volatile ("" : : "r" (rq));
        w <<= 1;
        __MapActor_Surprise(rq, w);
    }
    return 0;
}
```

**Depinned** — the pin was inert (class 1):

```c
        unsigned int rq = 0xe;
        __asm__ volatile ("" : : "r" (rq));
```

**Humanized** — classes 2 and 3 addressed, class 4 labelled:

```c
#include "emote.h"

int Cutscene_SurpriseActor(void)
{
    int mode;
    int slot;

    /* 0x102 needs two instructions: Thumb-1 `mov` takes an 8-bit immediate, so
     * gcc builds it as `mov #0x81` + `lsl #1`.  The ROM fills the slot argument
     * between those two, so they have to stay separate statements -- joining
     * them, or writing the constant at the call, each costs 2 encodings. */
    mode = (EMOTE_LAYER_FIXED | EMOTE_KIND_SCRIPTED) >> 1;

    slot = FIELD_ACTOR_SLOT_0E;
    FORCE_IN_REGISTER(slot);

    mode <<= 1;
    __MapActor_Surprise(slot, mode);
    return 0;
}
```

The knowledge goes in a header, where the other ~250 call sites can use it:

```c
#define EMOTE_KIND_NONE       0      /* delete the actor's existing bubble     */
#define EMOTE_KIND_ANIM       1      /* _Actor_SetAnim(bubble, 1), no script   */
#define EMOTE_KIND_SCRIPTED   2      /* _Actor_SetAnim(bubble, 2) + script     */
#define EMOTE_LAYER_FIXED  0x100     /* force the bubble's priority bits to 4; */
                                     /* clear = inherit from the target actor  */
#define FORCE_IN_REGISTER(x)  __asm__ volatile ("" : : "r" (x))
```

Deliverables live in `scratch_elev/human/`.

## ORDER OF WORK

1. **Fix `shimcount.py` first.** It has **two** blind spots — the
   value-producing `__asm__("mov %0, lr" : "=l"(t))` class (reports zero for
   `GetUnit`) and the value-*consuming* barrier (reports zero for the file
   depinned in batch 320, which still has one). **Depinning can therefore make it
   report a false clean**, so progress will *look* like it is removing shims that
   are still there. This is a correctness problem, not just a scoping one.
2. **Re-run the free-pin sweep with no line limit**, all 617 rows. Class 1.
3. **Pattern 4 on the 198 dimensionless externs.** Largest bucket, cheapest,
   improves the code either way.
4. **Pattern 1 on the 72 compound conditions**, where the branches are early-outs.
5. **Class 2 constants** — a header of named flags per subsystem. Start with the
   `MapActor_Surprise` mode set, which has ~250 call sites.
6. **Pattern 3** on the 106 pointer-to-int casts, as a readability pass.
7. **Label class 4 precisely** wherever it remains, with the formulation count.

## WHAT THIS DOES NOT CLAIM

pokefirered matches ~12,000 functions with **zero** register pins, and all 21 of
its `asm` statements are genuine hardware. We have 2,650 pins. The compiler is
**not** the explanation — gcc-2.96 is identified and reproduces the ROM
byte-identically — and TU size does not reach register allocation, because
gcc-2.96 does no cross-function optimisation at `-O2`. So most of our pins are
source-shape artifacts and most should be removable.

**But "most" is not "all", and nothing here proves zero is reachable.** Class 4
is real: the barrier above has survived ~27 formulations. The honest claim is that
one in four small pinned files is inert, half the rest show a wrong-declaration
signature, and the ceiling has not been located.
