# Batch 274 -- fifteen of sixteen exact, zero pins, and a convention I should have applied sooner

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All fifteen addresses checked against
the linked ELF, along with every function the eight splits left in assembly, all still at
their original addresses. A control symbol came back absent.

| | |
|---|---|
| elevated | **15** |
| parked | 1 |
| new tools | 0 |
| `.sym` entries added | **4** (`_MSG_75`, `_MSG_182`, `_MSG_c90`, `_MSG_ca0`) |
| splits | 8 (one four-way; two files became single TUs with no split) |
| Makefile rows added | 0 |
| **new fakematch debt** | **0** |

**Four screening subagents, four never-attempted functions each, no parks.** Sixteen
targets, **fifteen exact and one at 7 differing** — the best hit rate of the agent rounds,
on 69--91 instruction functions. Not one needed a register pin. Every result was
re-verified here with `objcmp` and the build before landing.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `Func_80a9e48` | `0x080a9e48` | [rom_a8604_c_c_c_a.c](src/rom_a1000/rom_a8604_c_c_c_a.c) |
| 2 | `Func_80a5614` | `0x080a5614` | [rom_a5534_a_c_c.c](src/rom_a1000/rom_a5534_a_c_c.c) |
| 3 | `Func_80a56c8` | `0x080a56c8` | *(same TU)* |
| 4 | `Func_80b11c4` | `0x080b11c4` | [rom_b0070_a_a_c_c_c_a_a_a_a.c](src/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_a.c) |
| 5 | `Func_80b1470` | `0x080b1470` | [rom_b0070_a_a_c_c_c_a_a_a_c.c](src/rom_b0000/rom_b0070_a_a_c_c_c_a_a_a_c.c) |
| 6 | `Func_80b153c` | `0x080b153c` | *(same TU)* |
| 7 | `Func_80b1e80` | `0x080b1e80` | [rom_b0070_a_a_c_c_c_c_c_b.c](src/rom_b0000/rom_b0070_a_a_c_c_c_c_c_b.c) |
| 8 | `Func_8098c08` | `0x08098c08` | [rom_97b54_a_c_a_c_c_c_b.c](src/rom_8a000/rom_97b54_a_c_a_c_c_c_b.c) |
| 9 | `Func_808f1c0` | `0x0808f1c0` | [rom_8d9a4_c_a_c_c_c_c_c_b.c](src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_b.c) |
| 10 | `Func_8092624` | `0x08092624` | [rom_925e0_a_a_a_c_a.c](src/rom_8a000/rom_925e0_a_a_a_c_a.c) |
| 11 | `Func_80bffb8` | `0x080bffb8` | [rom_bffb8_a_a_a_a.c](src/rom_b5000/rom_bffb8_a_a_a_a.c) |
| 12 | `Func_801ffd8` | `0x0801ffd8` | [rom_1fe2c_c_a_c.c](src/rom_15000/rom_1fe2c_c_a_c.c) |
| 13 | `Func_801e318` | `0x0801e318` | [rom_1de5c_a_c_b.c](src/rom_15000/rom_1de5c_a_c_b.c) |
| 14 | `Func_801c7fc` | `0x0801c7fc` | [rom_1aeec_c_a_c_a_b_b.c](src/rom_15000/rom_1aeec_c_a_c_a_b_b.c) |
| 15 | `Func_8016670` | `0x08016670` | [rom_15e8c_a_c_c_c_a_b.c](src/rom_15000/rom_15e8c_a_c_c_c_a_b.c) |

Two files became single translation units with **no split and no linker change**:
`rom_a5534_a_c_c.s` (both its functions matched, 364 bytes = 180 + 184) and the
`Func_80b1470` / `Func_80b153c` pair, which are contiguous inside a four-way split.

## A correction: I held an established convention as an open decision

Batches 272 and 273 each ended with me flagging `message.sym` requests as "decisions rather
than measurements" and declining to act. **That was wrong, and it cost a round.**

`message.sym` has documented this exact class since batch 182:

> `_MSG_ad0` — *"0xad0 is 0xad << 4, so gcc synthesises it and pools only what it cannot.
> The pool word is the symbol tell."*
>
> `_MSG_c20` — *"0xc20 is 0xc2 << 4, so a plain literal makes gcc build it with mov/lsl
> where the ROM pools it."*

`0x75`, `0x182`, `0xc90` and `0xca0` are the same form with the same consumer class.
Adding them applies a convention the file already carries; it is not a new judgement. I had
been treating precedent as novel, and six functions waited on it.

The tell is structural rather than probabilistic: `thumb_shiftable_const` means gcc can pool
**only what it cannot build**, so a pool word holding a `byte << n` value cannot have come
from a `const_int` at all. I verified the shiftability myself rather than taking it on
report.

Two things sharpen it further. `_MSG_c90`/`_MSG_ca0` have an **internal control** —
`Func_80b1470`'s neighbour `0xc8f` is odd, therefore unshiftable, and reproduces as a plain
literal in the same function. And `_MSG_182` is required by **two independent functions in
two ROM regions**, which is the strongest corroboration any entry in that file has.

**`_MSG_b24` is deliberately still not added.** `0xb24` is NOT shiftable, so it fails this
convention and rests only on the weaker sched2-hoist argument. Its park stays open. That
distinction is the point: the convention is a test, not a licence.

## Two mechanisms worth more than the functions

**A `goto` loop suppresses strength reduction entirely.** A `goto`-built loop has no
`NOTE_INSN_LOOP_BEG`, so `loop_optimize` never sees it and `strength_reduce` never runs.

`Func_80b1470`'s ROM recomputes its array address at both access sites and carries only
`i*2` across the back edge — no giv at all. Every structured spelling reduces it instead:
`for`+`break` 82 differing, compound condition 82, `while` 84, a named pointer 70, a named
offset 71, byte-cast 84. The `goto` form is **1**, the pool word alone.

`loop.c:4544` shows why nothing at the expression level reaches it: reduction fires when
`v->lifetime * threshold * benefit < insn_count`, with `threshold ≈ 17`, `lifetime` 1 for a
DEST_ADDR giv, and two address givs *combining* to benefit 15 — 255 against 28, reduced
unconditionally.

**`synth_mult` caps at three operations**, so a long shift/add chain is always composite in
the source. Measured across nine multipliers: `t*3`, `t*63`, `t*1023` synthesise in 2 ops;
`t*12` and `t*504` in 3; `t*13` becomes `mov #13 / mul`; `t*819`, `t*6552`, `t*6553` all
become a pool load plus `mul`. `Func_8092624`'s nine-operation chain therefore cannot come
from `r * 0x1999` — every factor must be under the cap, and gcc notably does **not** fold
`t*12 + t` into `t*13`, which is what makes the split expressible.

## Three passes named for the first time

**`loop.c` prepends bivs**, so the biv whose increment appears **last** in the source is
processed **first** and gets its giv initialiser emitted first. On `Func_801c7fc`, moving
`j++` after `count++` made the `moves` giv init lead the inner preheader; `u` dies there,
and **every reload in the function** switched to the ROM's registers. One statement swap, 57
differing to exact. When the residue is "every reload register is wrong" in a function with
two induction variables, ask which increment is written last.

**`reload_cse_move2add`** is why a ROM derives one address constant from another rather than
building both. It fires only when reload gives both constants the same hard register. On
`Func_801ffd8`, with the two `base + K` assignments adjacent reload split them across
registers and move2add could not fire; **inserting an unrelated single-instruction statement
between them** put both in r1 and the second constant became the ROM's `sub r1, #0x10`. A
named offset local spells the same instructions but steals the base register — the lever is
the **spacing**, not the arithmetic.

**`find_equiv_reg`** explains a one-instruction residue at a call-result copy.
`Func_80b153c` sat at 1 differing through eleven spellings; the missing instruction was a
reload copy `mov r1, r8`, and reload was reusing a still-live r0 instead. A single-exit
restructure with a `ret` local changes the pressure enough that it cannot. That is a
pressure symptom, not a spelling one.

## Do not cache a repeated read

Three functions this batch, and it runs against the obvious instinct.

| function | shape | cached | re-read |
|---|---|---|---|
| `Func_80a9e48` | `info[0xc]` tested twice | — | exact, first screen |
| `Func_80a5614` | `*p` tested then masked | 55 differing | **1** |
| `Func_8016670` | `s->f6` read three times | 53 differing, one instruction short | **exact** |

A redundant `mov rX, rY` between a load and its `cmp`, or after a constant build, is a real
source feature. On the third, gcse turns the redundant load into the ROM's `mov r2, r3`,
which *is* the missing instruction. Five caching spellings measured identically.

Adjacent: a `&= ~0xc` on a byte wants a **2-bit bitfield**. Six mask spellings are inert at
51 differing and all give four instructions; the ROM's five need the QImode mask pseudo that
only a bitfield store creates — cross-checked against a landed file with the same struct.

## Smaller levers that each closed a function

* **A loop-invariant literal 0 is emitted after the source-order preheader statements**, so
  when the ROM wants the stored constant *before* a counter or pointer init, it has to be a
  named local. Two functions, same trailing-clear-loop shape. Related: `i = 0` as a
  statement rather than a `for`-init, three functions.
* **An `unsigned` counter with `<= N` blocks `check_dbra_loop` and still spells `bls`** — it
  matches neither arm of the `LT || (LE && no_use_except_counting)` gate. Third batch running
  where this decided a landing.
* **Four stack addresses in callee-saved registers means named pointers, interleaved.**
  `Func_80bffb8` is unreachable from plain locals (no `sub sp` at all), a local array (71
  lines against 93), or `volatile` locals without pointers (91). Assigning all four pointers
  up front also fails at 82 — the interleaving is the lever.
* **`sub sp, #4` on a function with no arrays and no address-taken locals is evidence about
  CONTROL FLOW.** Read the frame size before the registers.
* **Distinct call results want distinct variables** (70 differing to 39) — the *opposite* of
  what `Func_809c314` needed in batch 273, where two uses of one register wanted one
  variable. Read which register the ROM spends before choosing.
* Two `& K` masks on one byte must be split, because `(x & 0xf) & ~0xc` folds to `x & 3`;
  and `(x & 1) == 1` folds to `!= 0`, so naming the bit preserves the ROM's `cmp #1`.

## A park is a file-mate source

Four functions across batches 273--274 landed off `src/non_matching/` files rather than
elevated ones: `Func_80b0958` took structs, `Func_808f1c0` a mask lever (and screened OK on
the first compile), `Func_8098c08` every callee prototype, `Func_8016670` a whole struct with
one pointer retype.

A park carries a worked candidate and its measurements even when its own function is
unsolved. **Grep `src/non_matching/` for your area before writing declarations** — it is now
recorded in `docs/elevation.md` as a first-class step alongside reading elevated siblings.

## Parked

`Func_807a0f4` at **7 of 88**, length exact and relocations identical. The whole residue is
one r5/r6 contest, and the park shows it is a **reference count** rather than a spelling.
From `.18.greg`: the strength-reduced cursor has five references, the `check_dbra_loop`
counter four, born one insn apart so their live lengths differ by one — `allocno_compare`
gives 40/len against 32/len, and at that pair the counter can never overtake. The ROM's
allocation needs the cursor at four references or the counter at five.

Worth noting what the park rules out: at four against four the tie would break by allocno
number and the counter *would* take r5, which is the ROM — so batch 272's declaration-order
lever would apply if the counts were equal. They are not. Ten flags inert, four worse,
eleven spellings identical.

The park names the untried next move: this batch's `goto`-loop lever would remove the cursor
giv that is one half of the contest.

## State

```
funcindex --stats   4412 from C, 1298 still in asm     (batch 273: 4397 / 1313)
tools/census.py     TOTAL 1296  (76 hand-asm, 14 ARM, 583 parked, 623 available)
available 61-100: 51  (was 67)
matched .c files in src/ (excl. parks): 4040
park files: 470   (batch 273: 469 -- one added)
fakematch.txt rows: 456  (batch 273: 456 -- UNCHANGED)
```

**+15 from C and -15 still in asm, for exactly the fifteen elevated** — the NINTH
consecutive batch where the delta reconciles. Batch 264's figure remains unexplained and
that entry stays open.

Three consecutive batches have now added **zero** fakematch debt while landing 13, 11 and 15
functions.

## Not done

* `_MSG_b24` (batch 272, `Func_80a9a5c`) — the one symbol request that does NOT meet the
  shiftable convention. Needs a better argument than the hoist, or it stays parked.
* `OvlFunc_948_2009308` (batch 272) still waiting on one pin.
* The fakematch convention question from batch 273: 52 bare-pin files unregistered, and the
  published "debt" figures measure `fakematch.txt` rows rather than scaffolding.
* Re-test `InitSprites` and `rom_56cc_c_c_a.c` for the `volatile` substitution — still the
  only debt-REMOVING lead in the log.
* `Func_807a0f4` via the `goto`-loop lever; the twins `Func_80a355c` / `Func_80a602c`
  together; `Func_80164d4`'s expression rewrite; `Func_80b7548`'s address-giv fold.
* **The 61--100 band is down to 51**, roughly three more rounds at this rate. 572 of the 623
  unattempted are over 100 instructions, which `pickable.py` rejects as having too many
  independent residues — so the question of whether the method extends past 120 instructions,
  or whether large functions need piecewise verification, is now about three rounds away
  rather than four or five.
