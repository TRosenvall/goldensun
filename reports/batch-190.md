# Batch 190

Five elevated, nine parked. Two of the five are fakematches and one needs a
flag group.

**The result that matters is a change in how targets are chosen.** Two rounds
inside this batch went **0-for-6** picking by size, smallest first. That is not
a run of bad luck — it is the heuristic being spent, and the fix took the next
three rounds to 5-for-8.

## Function breakdown

| # | function | address | file | what it took |
|---|---|---|---|---|
| 1 | `OvlFunc_959_200c704` | `0x0200c704` | [ovl_9dc_…_a_c_b.c](src/overlays/rom_7e7574/ovl_9dc_c_c_a_a_c_b.c) | **fakematch**; `do{}while(0)` used deliberately |
| 2 | `OvlFunc_891_2009b44` | `0x02009b44` | [ovl_30_c_c_c_c_a.c](src/overlays/rom_78c76c/ovl_30_c_c_c_c_a.c) | **fakematch**; a call between uses does *not* force a rebuild |
| 3 | `OvlFunc_883_200d950` | `0x0200d950` | [ovl_30_…_a_a_c.c](src/overlays/rom_780898/ovl_30_c_c_c_c_c_a_a_c.c) | **`CSE_CFLAGS`** — guard/set shape |
| 4 | `OvlFunc_927_2009c34` | `0x02009c34` | [ovl_30_…_a_a_b.c](src/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_a_a_b.c) | a hoisted constant is not a named local |
| 5 | `OvlFunc_926_2008db4` | `0x02008db4` | [ovl_314_…_a_a_b.c](src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a_b.c) | assignment order decides an adjacent swap |

Parked: [`200805c`](src/non_matching/overlays/200805c.c) (6 of 56),
[`200b668`](src/non_matching/overlays/200b668.c) **and its twin `200b5ac`** (5 of
83 and 5 of 86), [`200a750`](src/non_matching/overlays/200a750.c),
[`200a718`](src/non_matching/overlays/200a718.c),
[`8094154`](src/non_matching/rom_8a000/8094154.c),
[`8010560`](src/non_matching/rom_9000/8010560.c),
[`807a7a0`](src/non_matching/rom_77000/807a7a0.c),
[`80e7338`](src/non_matching/rom_c9000/80e7338.c) and its twin,
[`80a1a40`](src/non_matching/rom_a1000/80a1a40.c),
[`80b280c`](src/non_matching/rom_b0000/80b280c.c).

Gated on a clean `make clean && make compare`, every address verified by
`tools/checkaddr.py` against `goldensun.elf` and the per-overlay `overlay.elf`.

## PICK TARGETS BY TEMPLATE, NOT BY SIZE

Targets had been chosen smallest-first, on the theory that small converges. With
~3,520 functions matched that theory is exhausted: **the size-ordered head of
the list is now uniformly structural** — allocation-order disagreements and
scheduling residues — because everything that falls to a spelling is gone. Two
rounds of 0-for-3 each, with every attempt plateauing across five to nine
spellings, is what that looks like from the inside.

The heuristic that *has* worked, seven rounds running, is **callee-set
identity**: `neighbour.py` finds a solved file sharing the target's callees and
globals, and that file hands over the struct layout, argument order and idiom.
But it was only ever run **after** a target was chosen.

`tools/templated.py` inverts it. It scores every remaining function by its best
available neighbour and ranks on that:

    score = |shared callees and globals| / |the target's own|

**1.00 means every callee and global the function touches already appears
together in one solved file.** 924 candidates have some neighbour; 14 score 1.00.

Every one of the five elevations this batch came from that list at 1.00. The
neighbours supplied, for free: a complete extern block (twice), an
eight-argument helper signature with the struct its last argument points at, and
a `struct` already declaring the exact field the target touches.

**Size is reported but deliberately not ranked on.** A 90-instruction function
with a perfect template beats a 50-instruction one with none — that is the
premise, and if it stops holding the tool should be struck rather than tuned.

## Four things that look alike and are not

The round's levers all turned out to be discriminations between shapes that
present identically. That is worth stating as a pattern: **at this stage of the
corpus the useful work is mostly telling apart two causes with one symptom.**

### A hoisted constant vs. a named local

A constant living in a callee-saved register across several calls is the
recorded signature of *"one load kept across a call is a named local"*. It is
also what gcc does on its own with a literal it sees reused. Guessing wrong on
`OvlFunc_927_2009c34` cost **80 differing of 92**, where plain literals gave
**8 of 90**.

**The tell is where the first use goes:**

    ROM      mov r3, #1 / str r3, [sp] / ... / mov r8, r3
    named    mov r3, #1 / mov r8, r3   / ... / mov r3, r8 / str r3, [sp]

The ROM stores the literal *straight to its destination* and copies it into a
high register afterwards. A named local is materialised into its register first
and every use, the first included, is fed from there.

### A call between two uses vs. an actual rebuild

The recorded false-positive list says a call between two sites excuses the
rebuild, because calls clobber the argument registers. That was the largest of
the three classes. **It only holds when the constant lives in a call-clobbered
register.** On `OvlFunc_891_2009b44` gcc parked both constants in *callee-saved*
r5 and r6 precisely so they would survive the intervening `bl`.

**The marker is a WIDER PROLOGUE, not a `mov`** — `push {r5, r6, r7, lr}` plus
an r11 save against the ROM's `push {r5, r6, lr}`. It paid for itself
immediately: `OvlFunc_883_200d950` showed the same signature, and going straight
for a hoisted constant took it from 85 differing to 6 in one step, where three
spellings tried before diagnosis had all made it worse.

### An adjacent register swap vs. the allocator

Two functions ended with a two-register swap and nothing else, and they are
different classes. The discriminator is **how far apart the two values are
initialised**:

| | `OvlFunc_926_2008db4` | `OvlFunc_932_200b668` |
|---|---|---|
| the two `mov`s | adjacent | far apart, across a call |
| assignment order | **0 differing** | untried — no adjacent pair to order |
| declaration order | inert (6) | inert (8) |
| outcome | elevated | parked at 5 |

Declaration order is inert in both, because local-alloc orders by priority
rather than pseudo number. But where the two initialisations sit adjacently in
one basic block their order is still visible, and the source has a vote.
**Sweep assignment order first; if the two `mov`s are not neighbours in the ROM,
do not bother.**

### Scaffolding that is load-bearing vs. scaffolding that is habit

`OvlFunc_959_200c704` landed with three pin blocks, a barrier and a two-step
constant. Each was then deleted from the *finished* file:

| removed | differing |
|---|---|
| the pin on the repeated constant | 32 |
| the `do{}while(0)` barrier | 4 |
| the pin on an unrelated call | 2 |

All load-bearing, and the sizes say what each is *for*. On
`OvlFunc_891_2009b44` the same teardown found the opposite: a third pin on a
call's `r0` argument measured **exactly** what omitting it did, so it is not in
the landed file.

**Building a fakematch up tells you what helped; tearing it down tells you what
is still needed.** Those are different questions, and only the second belongs in
the file. This also bounds the *anchor every argument* rule — anchor the
arguments that participate in the interleave you are fixing.

## `do { } while (0)` used on purpose

The barrier finding was recorded as an observation in batch 189; this is the
first time it was reached for deliberately. The shape: a pool load the ROM emits
**after** a call, which gcc hoists **above** it because the target register is
callee-saved and survives.

    do { msg = 0x2411; } while (0);

emits no instruction, splits the scheduling region, stops the hoist. 4 → 2.
**When the ROM materialises a constant after a call and you cannot keep it
there, the construct that costs nothing is the loop note.**

## Two clean `REG_ALLOC_ORDER` probes

`OvlFunc_932_200b668` sits at **5 of 83** with the instruction count exact and
every operation right; the residue is purely an r8↔r10 rotation. Its twin
`OvlFunc_932_200b5ac` — the same routine rotating the other way — was written to
the same shape and landed at **5 of 86 with the identical rotation at four
sites**.

Two independently written functions reaching an identical residue by an
identical mechanism is not a spelling accident. Together with
`OvlFunc_919_200805c` (6 of 56, also purely one register) these are the cleanest
probes the corpus has for the standing `REG_ALLOC_ORDER` hypothesis: if it is
ever tested, all three should go to zero and nothing else can move.

## Recorded negatives

- **The signedness tell can be inert.** `Func_807a7a0` ends its loop with a
  signed `ble` where we emit `bls`, but declaring the counter `int` rather than
  `unsigned int` changes **nothing** — gcc knows the value is a small
  non-negative and picks the unsigned form regardless of the declared type.
- **Copying a neighbour's declaration is not automatically right.** The
  neighbour declares `__vec3_translate` with unsigned parameters; adopting that
  made no difference at all to `OvlFunc_932_200b668`.
- **gcc folds `ldrsh` + zero-extend into `ldrh` and will not be talked out of
  it** — and it folded two of four identical field loads in one loop, so it is a
  per-load decision made after CSE with no source handle.
- **The ROM advances a pointer rather than indexing.** On `Func_8094154`,
  `out[0]`/`out[1]` gave 52 differing, `*out++` gave 48, and an explicit
  `q = out; out = out + 1;` gave 19.

## A tool was overwritten and restored

`tools/pickable.py` already existed and is imported by `filtered.py`. A new tool
was written to that path with `cat >`, destroying it and breaking the candidate
filter. It was caught from the resulting circular-import error, restored with
`git restore`, the tree confirmed to match HEAD exactly, and the new tool
rewritten as `templated.py` after checking the name was free. Nothing was lost.
**Check before writing.**
