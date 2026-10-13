# Batch 345 — the pool holds up, and four tool defects fell out of it

**5 briefs, 20 targets, 3 landings and 7 length gaps closed**, plus **2 landings
by sweep alone** with no agent and no tokens. Park files **628 → 623**.

Drawn from the same gap pool as 344: parks within ±1 instruction whose figures
(33 to 109) are phase rather than distance.

## Landings

| function | before | counts | how |
|---|---|---|---|
| `Func_80c0eec` | 66 (phase) | — | a shared pseudo costing **zero instructions**, plus `while (1)` ≠ `for (;;)` |
| `OvlFunc_912_20081c4` | 86 (phase) | — | `s1 = 8;` keeps the constant out of `force_to_mode` |
| `OvlFunc_924_2009c9c` | 65 (phase) | — | **`CSE_CFLAGS`, park body untouched** |

### By sweep alone — no agent, no tokens

| function | before | after |
|---|---|---|
| `StartRain` | 3 of 104 | **0** |
| `OvlFunc_970_2008da4` | 5 of 175 | **0** |

One `do { } while (0);` anchor each, found by a free pass over the 45 device-free
parks whose **instruction counts already agree** at a figure ≤6. Equal length is
the precondition that makes a figure a real distance, which is why the pass was
pointed at that band and not at the lowest figures. 2 zeros and 1 improvement in
45 — the same ~1-in-20 as the batch-338 run of this route, for wall-clock only.

## Seven length gaps closed

Each gives a park its first figure that is a real distance.

| park | before | after | counts |
|---|---|---|---|
| `DrawMsgGlyph` | 84 | **33** | 176/176 insns, 436/436 bytes |
| `Func_8005fcc` | 74 | **25** | 79/79, 188/188, relocations now clean |
| `OvlFunc_956_200804c` | 81 | **23** | 132/132, 316/316, 145/145 |
| `OvlFunc_907_2008328` | 60 | **8** | 74/74, 220/220 |
| `Func_808e23c` | 64 | **28** | 215/215, 632 bytes throughout |
| `OvlFunc_946_2008f70` | 102 | **79** | |
| `OvlFunc_883_200d64c` | 65 | **55** | 100/100, 224/224 |

## A gate went RED, and the cause is worth knowing

`OvlFunc_912_20081c4` was byte-identical and its first install **broke the link**.
Its `.s` holds the function *and* a trailing `.global` data block
(`gOvl_02008318`, `MapEntrance_ARRAY_912__02008318`, `gOvl_02008498`,
`gOvl_02008658`, `.L4d8`) that **four sibling objects in the same overlay** link
against — so converting the function whole deleted symbols nothing else defined.

Reverted to green, then re-landed through `split_s.py`, which already handles
this: *"holds X plus 4 blob(s) and 1 label(s), ALL of it after the code"* —
function to `_b`, data to `_c`. Gated after the split alone, then after the
install.

**The lesson:** "converts whole" is about the *functions* in a piece.
A single-function `.s` can still be unconvertible whole if it carries exported
data, and `piece.py` counts functions, so it says CONVERTS WHOLE either way.

## A correction to `docs/elevation.md`, caught by brief C

I had written **"`volatile` is the only escape from the halfword narrowing on
Thumb"** with the citation `combine.c:5921-5938`. Wrong twice:

- **Not the only escape** — naming the operand in a local reads **0** where
  `volatile` reads 38.
- **Wrong function** — `:5921-5938` is inside `make_extraction`. It tests
  `MEM_VOLATILE_P (inner)`, which is presumably how it got attached, but it has
  nothing to do with narrowing a mask.

The real site is `force_to_mode`'s `PLUS`/`MINUS`/`MULT` case,
**`combine.c:6855-6860`**, where `mask = fuller_mask` re-masks an SImode
`(plus e -8)` to a HImode destination and `-8` becomes **65528** — fitting no
Thumb `addsi3` immediate, so it costs a pool word plus a three-operand add where
the ROM has `subs r3, #8`. Writing `s1 = 8;` keeps it out of the narrowing, and
that is what landed the function. Verified by reading the compiler source.

## FOUR CAUSES OF ONE DEAD SLICE IN `autosweep`

Brief B reported 4 anchor variants "produced NO ROW" on all four of its targets
and guessed the defect was systematic. It is real, it was **not** one defect, and
chasing it took four fixes. Measured, not assumed — 9 of 45 parks in the clean
sweep were affected, 20 dead variants.

`boundaries()` took every `;` at brace depth ≥ 1. The four things that follow a
`;` and cannot legally begin a new statement:

1. **A `for` header's own semicolons** — `for (;do { } while (0); i != 11; i++)`.
   Two dead variants per `for` loop; this was brief B's 4. *Cleared 3 of 9 parks.*
2. **`else`** — `if (c) x = 1; <HERE> else …`. Also `while` of a `do…while`.
   *Cleared 4 more.*
3. **A declaration using a TYPEDEF.** The guard enumerated built-in type keywords,
   so `vec3_t vb;`, `DrawFn *fp;`, `CopyFn cp;` passed straight through. All 12 of
   `Anim_CriticalHit`'s failures. Replaced by a **structural** test — two
   space-separated identifiers before `;`/`,`/`=` is a declaration whatever the
   first one is called.
4. **A pin macro.** `{ R0; R1; q0 = …; }` where `R0` is
   `#define R0 register int q0 __asm__("r0")` — a declaration wearing a
   statement's clothes. Now read out of the file by **shape**, the same
   correction `shimcount.py` needed when macros named R0..R3 made it report 4
   pins against 96.

**The general rule, now in the tool:** a `;` is a boundary only when what follows
it can legally *be* a new statement.

Worth stating plainly: **the tool was right to report that slice as UNTESTED
rather than inert, and that report is the only reason any of this was found.**
The guard added in batch 338 paid for itself. But every "axis CLOSED" recorded in
a park header before this batch should be read as *closed except those cases*.

## `autosweep` also needed the aligned verdict

Batch 344 gave the flag axis a guard that fires when a variant's instruction
count moves. Brief A found that **firing is not sufficient** — the row still
prints its lower ndiff, and a caveat beside a misleading number reads as
progress. Lower rows now carry the **aligned residue** and cannot enter `BEST`
unless it improved. On `GiveItemTo`:

```
-fno-strength-reduce      57 of 68  <-- ALIGNED RESIDUE IS WORSE (42 -> 54)
-fno-rerun-cse-after-loop 49 of 68  <-- ALIGNED RESIDUE IS WORSE (42 -> 44)
```

**Two false leads on one park** where the brief had found one. "49 against 58"
reads like a 9-encoding gain; it is a 12-instruction regression.

## Mechanisms

- **Check the pseudo is in the greg list at all** before diagnosing a ±1 as
  priority. Two of brief D's four gaps were **local**-alloc, not `global_alloc`,
  and both parks said `global_alloc`: the pseudos are in neither `.18.greg`'s
  allocno list nor its dispositions, because one basic block means
  `local-alloc.c:1495-1497`'s `QTY_CMP_PRI` owns them.
- **N names iff a call separates the N uses.** With a call, reload rematerialises
  the `REG_EQUIV` constant per argument and naming pays; without one, naming is a
  regression — one target wanted **no** name at all (102 → 88, where
  one-name-per-site reads **263**). `cse.c:1586` is why it never transfers to a
  pooled *address*: a constant-pool MEM is `RTX_UNCHANGING_P`, so
  `invalidate_memory` cannot drop it across the `bl` (`cse.c:2260`, `:6068-6080`).
  This resolves an apparent contradiction between three of brief E's targets.
- **A shared pseudo can cost zero instructions and still be the blocker.** Four
  `gKeyRepeat` reads commoned into one pseudo, reload rematerialising the load at
  each use, so the count already matched and every count-based reading came back
  clean. The cost is the two-address tie on `*thumb_andsi3_insn`, where
  `regmove` ties the output to an operand only if it **dies** there. Invisible to
  every instrument here that counts instructions, `gapscan` included.
- **`while (1)` is not `for (;;)`** to gcc-2.96 — 15 aligned instructions of block
  placement, and the reason that park sat at 66: it had measured three loop
  shapes, all `for`/`while(cond)` forms.
- **The anchor axis is not separable.** All 31 subsets of five in-loop boundaries
  never beat aligned 8, and two anchors cost two instructions in combination
  while costing none alone. So "anchor axis closed" is a claim about *single*
  anchors only.
- **Naming a constant is a two-way lever** — it landed one target and made another
  four instructions worse. Direction is not predictable from the park.

## The recorded-negative rule, fourth batch running

Brief E found **two of its three winning levers in the park's own
recorded-negative list**. Brief C found **four** dead negatives, including
`DrawMsgGlyph`'s "separate loop pointer is worse (84→95)", which is **47 → 33**
on the gap-closed body.

A negative is a measurement of **one body** and expires silently when the body
changes. Worth recording, but it reads as "inert against the body of batch N".

## Process

- I told brief B to put a split in the manifest's `split` field. That is valid
  only for a **landing** — `install_batch.py` refused it and was right, since
  splits run for any entry carrying the field regardless of figure. The template
  now states the rule and says the tool is the authority over a launch prompt.
- A background sweep reported **complete** while still running, and a second run
  then appended to the same log, interleaving the two. Attribution was lost: the
  same park appeared twice with different bases. Both sweep landings come from a
  single clean re-run. **A completion notification is not proof the work stopped.**

## Open

- Two owner decisions from brief B: `_CONST_0 = 0x0;` in `const.sym` (evidence:
  gcc-2.96 cannot pool a literal 0 in any mode whose pool load is `ldr` —
  `arm.md:3840`, `:4625`, five spellings measured), and the
  `__divsi3 = _divsi3_RAM;` alias in the common1 linker script, which
  `OvlFunc_common1_78` needs before it can land at any figure.
- `OvlFunc_956_200804c`'s 23 is at `-fno-strict-aliasing`; its header states both
  figures loudly so `parkcheck` reading it as worse at production flags does not
  mislead anyone into reverting the body.
- Brief E: read **every landed `.c` in the park's own `asm/` directory**. Its
  highest-value move cost one `cat` — target 1's landed twin in the same piece
  carries the exact lever its park needed while that park declared the class
  closed.
