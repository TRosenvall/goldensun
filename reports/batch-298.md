# Batch 298 — seven functions, and a lever that beat a 450-spelling sweep

Seven agents, 35 unattempted targets, gate green at
`5c4695205413df7db52b9a184815a07783999971` throughout.

**7 landed, 19 newly parked, 5 existing parks improved or repointed.**
State: **4,749 of 5,710 elevated (83.2%)** — 182 available, 688 parked, 76
hand-written asm, 14 ARM, 1 unmatchable.

Reconciliation, from git and exact: 7 landed + 19 newly parked = **26 functions left
the available pool**, and parked rose by exactly 19. One `size.sym` row and four
`fakematch.txt` rows added.

This batch was **unattempted functions only**, at the owner's direction — no closing
briefs on existing parks. That changes the expected yield: batch 295's closing briefs
landed 11 of 14, where 7 of 35 here is a 20% rate on functions nothing was known
about. The targets ran 172–256 instructions, and **13 of them were under 200**, which
is the band that has historically produced landings. After this batch that band is
nearly exhausted: what remains available is mostly 400+ instructions.

## Landed

| function | encodings | shims |
|---|---|---|
| `Func_8017e88` | 205 | 0 |
| `GameInit` | 231 | 0 |
| `Task_Snow` | 227 | 0 |
| `Task_Earthquake` | 250 | 0 |
| `Func_809537c` | 244 | 0 |
| `DrawText` | 265 | 0 |
| `OvlFunc_933_20092fc` | 193 | 2 pins |
| `OvlFunc_920_2008538` | 248 | 4 pins |
| `Func_8093c00` | 246 | 3 pins |
| `OvlFunc_891_20096dc` | 257 | 33 pins |

(Ten rows, seven of them new in this batch's commits — `Func_8017e88`, `GameInit` and
`Task_Snow` landed in the frontier work immediately preceding it and are listed for
continuity.)

## The result that transfers furthest

**Distribute the shift by hand when the ROM does not fold it.**
`((win->x + (u16)(win->w - 2)) << 3) + 4` has two separate failure modes: a `u16`
local zero-extends and costs four instructions, and written inline `fold`'s
`associate:` merges `0xfffe << 3 | 4` into a single pooled `0x7fff4`. Written
distributed —

```c
(win->x << 3) + ((u16)(win->w - 2) << 3) + 4
```

— `fold` has nothing left to associate and `combine` folds the two shifts back
itself. On `DrawText` that moved the aligned figure from 83.4% to **99.2%**.

**Then it transferred.** `src/non_matching/rom_15000/8018efc.c` had been parked on
exactly this blocker after a **450-spelling sweep**, every spelling of which left the
constant inside the shift. Dropping the two distributed expressions in, with nothing
else changed, took it from **86 of 119 — not even a distance, 123 instructions against
119 — to 17 of 119 with size and count both matching.** That is the largest single
improvement to an existing park this session, and it came from a function in a
different bank.

## New blocker classes

**A callee-saved register the ROM zeroes and never reads.** Two of five targets in one
brief. `OvlFunc_925_200835c`'s reference mentions `r6` exactly three times — `push`,
`mov r6, #0`, `pop` — written once, never read; `OvlFunc_928_2009148` is the same with
`r8`. gcc-2.96 deletes the def in **all eight source shapes measured**, including both
hard-register-declaration forms. The cost is not one instruction: the dead `mov` owns
the push/pop mask, which shifts the literal pool by 4 bytes and turns nine
pc-relative loads into differences. Screen by grepping each callee-saved register —
three hits means this class, and the function cannot close without solving it.

**A bitfield store carries the enclosing STRUCT's alias set.** So an `int` global in
the value expression keeps a preceding `strb` dead-but-alive. Retyping that global
`unsigned int` → `unsigned long` — same width, but a type the struct does not contain,
where `unsigned int` canonicalises to `int` which it does — breaks the conflict and the
dead store dies. A third alias-set escape, and **not a pin**.

**gcc-2.96 never chains plain `CONST_INT`s.** Proved with a five-call probe in one
block: all five literals pool separately, because cse's `related_value` relates
`SYMBOL_REF`+offset only. So a ROM that derives several nearby constants from one held
value **identifies a named base in the original** — it is not a spelling problem. On
`Func_80a8914` the named base also revealed that a six-site r9/r10/r11 residue was
downstream collateral, vanishing once the base stayed live.

## Levers

- **Splitting a global-pointer load from its offset add is a scheduling lever**, and
  the position of the *other* prologue chain decides it. `m = g; slot = other;
  o = *(T **)(m + K);` took `OvlFunc_933_20092fc` from 17 encodings to zero, and all
  five other orderings of those three statements fail.
- **gcc reaches a stored constant from the address offset already in a register**
  (`add r3, #0x2c` for `0x258` beside offset `0x22c`). Recognising it took one park
  from 55 differing to 8; failing to control its *direction* is another park's entire
  residue.
- **A loop whose last block before the back-edge is a conditional body, with the
  exit's tail after the back-edge, is a `goto` in the source.** `for(;;)`+`break` gets
  rotated by loop.c and additionally lets gcc constant-propagate the return variable
  away — the tell is a frame one word small.
- **Three indirect calls want three function-pointer locals.** One shared local emits
  `bl _call_via_fp`, and re-assigning it does not help because gcc CSEs it.
- **The veneer register in `bl _call_via_rN` is a free readout of the allocation.** A
  pointer local assigned across a call gets a callee-saved register; wrong veneer
  number means wrong live range, and the fix is where the local is assigned.
- **The tell for a mask is the complement's FORM, not its width** — one function wants
  `~0xc` as a bitfield (`mov #0xd / neg`) twenty lines from a `0xfe` that must not be,
  and both fit imm8.

## Corrections to the method, including three of mine

- **The one-member union alias escape reaches SCHEDULING only, not LICM.** I
  documented it this session from `common1_1078`; five union forms on `Func_801b664`
  were all byte-identical to no union at all. Only `volatile` reaches
  `loop_invariant_p`, and there it was worth 145 encodings.
- **The negative-offset lever is bounded by the symbol's USE COUNT.** `(&sym)[-N]`
  folds the addend at -O2 with a single use; it reproduces the ROM only at two or more.
  With one use the address needs a named pointer local first.
- **My Bresenham write-up had it backwards**, and the agent that reported it corrected
  itself: axis-named is **17**, role-named is **28**. Role-naming is the right
  *mechanism* and the larger residue; axis-naming wins 11 encodings by accident,
  because gcc's fixed x→r0/y→r1 choice suits one arm. The negative to cite is the
  four-pseudo attempt at 69.
- **Three references' `@` prose was wrong about its own function** in one brief — a
  `.s` line count passed off as an instruction count, a four-way dispatch described as
  one behaviour, and an allocator described as a font fetch. Two others checked out.

## Measurement warnings added this batch

- **A matching instruction count does not rank two candidates.** On
  `OvlFunc_971_2008580`, pinning makes the count match exactly (256 == 256, a true
  distance) and it is far worse — 221 differing at 47.7% aligned, against the shipped
  variant's 262 encodings at 66.0%.
- **An embedded union at an odd halfword offset breaks the layout and reads as a
  win.** `STRUCTURE_SIZE_BOUNDARY` is 32, so a one-`short` union pads to 4 and
  4-aligns; at `0x396` it shifted every later field and scored **154 of 199 with the
  size matching**, against the correct layout's 156. Only the pool constants gave it
  away.
- **`RELOCATIONS differ` is two findings.** Same symbols in the same order with
  different offsets is a size shift and carries no information; symbols **reordered**
  means block layout — that is how `OvlFunc_971_2008398`'s real blocker was found.
- **cse does not see through an empty `"+r"` barrier on a known constant — it deletes
  the insn** (present in `.00.rtl`, gone by `.03.cse`).
- **Do not hand-count a reference containing `.call_via`.** A hand ledger repeated
  `tryc.py`'s old bug and read 228 instructions instead of 236, making a candidate look
  5 short when it was 3 over.

## Decisions

- **`-ffixed-r7` on `rom_f2028_c_a.o`: NO.** A park requests the test; the answer is
  negative. That ROM's prologue saves r7 *to reach* r8, and under the flag the source
  saves three callee-saved values where the ROM saves four, coming out 180 instructions
  against 202. Both functions build from that object.
- **`_SIZE_80155d0 = 0x318` admitted**, arithmetic re-derived independently, satisfying
  criterion 1 in its strong form (`0x318 = 0xc6 << 2`, so gcc builds it and the ROM
  pools it) plus the DMA-word criterion. It completes `DrawText`.
- **`_FILE_18 = 0x18` withheld** — the pool-load signature is there and it would fill
  the hole beside `_FILE_19`, but it completes no function.
- Landing `Func_80a8914` will need a **`GCSE_CFLAGS` row**; under `-fno-gcse` its pool
  collapses to the ROM's exact eight words and the residue drops from twelve sites to
  three. Recorded as diagnostic, since a path-keyed flag row does nothing until the
  `.c` exists.

## Open

- **`Func_801b664` at 2 of 200, pin-free** — the closest thing in the tree to done.
  The last two are one adjacent transposition, and the mechanism is self-limiting: the
  boundary that buys the other 5 truncates the dependency chain that would win them.
- **`Func_80f62b8` is the cheapest instrument in the corpus** for HANDOFF's open
  `REG_ALLOC_ORDER` question — one clean r0/r1 exchange, no confounding shape
  difference, and a falsifiable prediction recorded in the park.
- Three targets unattempted, with their reference facts recorded: `Func_80a7478`,
  `Func_80aafb8`, `BaseAnim_Tentacle` (the last needs a text/data split exporting five
  labels).
- **30 frontier parks still have stale or missing `Verify with:` recipes.** All were
  measured in batch 297a and two repaired; note `parkcheck`'s `header_of()` reads only
  the **first** comment block, so a recipe placed after a prepended note is invisible.
- The available pool is 182, but only ~15 remain under 400 instructions.
