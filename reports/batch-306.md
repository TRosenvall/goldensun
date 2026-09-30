# Batch 306 — three parks, and the reuse lever gets its complement

Three agents, one function each — the three smallest unattempted functions in the tree. A lean
round by design. Gate green at `5c4695205413df7db52b9a184815a07783999971` throughout.

**0 landed, 3 parked.** All three were genuine reconstructions from recon notes, not closing
briefs, and none was close. State: **4,785 of 5,710 (83.8%)** — 89 available, 746 parked.

| function | insns | figure | aligned | shims |
|---|---|---|---|---|
| `UpdateSpriteAnim` | 544 | 589 of 674 (saturated) | 297 (44.1%) | 0 |
| `ActorCmd_Player_World` | 554 | 313 of 578 (saturated) | 500 (**86.5%**) | 3 pins (landing route) |
| `Func_80aa768` | 559 | 527 of 589 (saturated) | 346 (58.7%) | 0 |

All three figures saturate, so the aligned column is the ranking view. `Func_80aa768`'s park
records the progression 574 → 566 → **474/502** → 527 and explains why **the lowest figure is
the worst candidate** — which is the discipline working as intended rather than a regression.

## The reuse lever gets its complement

Batch 304's register-inheritance lever merges two variables so a later range inherits an earlier
range's register. `Func_80aa768` needed the **opposite** — and the two are not in conflict.

A temporary declared at **function level** shares one pseudo whose `live_length` is the **sum of
all its disjoint ranges**. Across a 15-arm switch that inflated the allocno set enough that the
switch subject won a callee-saved **high** register, costing a `mov rlo, r8` before each of
fifteen `cmp n,#K`, a third high-register save, and a return-path rotation. **Brace-scoping the
temporaries per `case`** dropped the subject to **r4 with caller-saves** — the ROM's frame
exactly — and the prologue then agreed instruction for instruction. Worth **566 → 474**.

Why r4: with `-fcall-used-r4` in production flags no call-saved low register can be free, so
`find_reg` reaches r4 only via its `accept_call_clobbered` retry, gated on
`CALLER_SAVE_PROFITABLE(n_refs, calls_crossed)`. Shortening the competitor's live length is what
makes that retry fire.

**The reconciliation:** reuse **raises** an allocno's priority; region-splitting **lowers a
competitor's**, by shrinking the `live_length` term `allocno_compare` divides by. Both act on the
same formula from opposite ends. The question is never "merge or split" in the abstract — it is
which allocno is winning a register it should not, and whether to promote yours or demote the
rival. The older one-variable-per-region versus counters-unify pair is the same choice seen on
loops instead of switch arms.

## The named-offset lever has a second mechanism

Recorded as a fix for gcc folding `SYMBOL_REF + CONST_INT` into one pool word. It paid on
`Func_80aa768` where **there is no symbol at all** — the base is a loaded pointer, so nothing
could pool. Inline, `p[0x208 + idx]` folds to `(p + 0x208) + idx*2`; hoisted into its own
statement it yields the ROM's **register+register** `ldrh r2,[r7,r3]`. Parenthesising is inert,
because fold's `associate:` normalises it back.

So the lever is two levers: **pool-word folding** when the base is a symbol, **addressing-mode
selection** when it is a pointer. Combined with brief A's bound — the derived address needs **more
than one use**, established on both sides inside one function — it now has two mechanisms and one
precondition, and the mechanism decides which residues it can reach. Fourth and fifth
confirmations this week.

## Two sets prevent a hoist

`UpdateSpriteAnim` inverts the usual loop-invariant reading. The ROM has **no** hoisted base for
its insertion-sort scan because it computes `add r5, sp, #0x30` **twice in two non-dominating
arms** — two sets, so `scan_loop` refuses it as invariant. Our candidate writes it once, CSE
commons it to **one set**, and one set is exactly what makes it *movable*: loop.c hoists it,
spills it to sp+0, adds a third giv. That is the whole +7 and the +4-byte frame.

**When the ROM recomputes something you would naturally write once, the duplication may be
load-bearing** — it is how the ROM denies loop.c a movable. Reaching it needs a source shape whose
value genuinely has two sets in non-dominating arms, not an expression written twice for CSE to
undo. Not sched2 (the extra insns are *defs*, not a reordering) and not `allocno_compare` (the
quantity **count** differs, and a spelling can only move `n_refs`, `live_length` and declaration
order — it cannot delete a quantity).

## Two new uses of the one-pseudo fact

- **A literal equal to a variable's known value is not interchangeable with it.** Writing `1`
  where the ROM stores the variable holding 1 let `jump.c` cross-jump two arms the ROM keeps
  distinct — the tails matched only because the literal erased the difference. Worth 105 → 94.
  gcc's cross-jumping compares *emitted tails*, so anything making two tails textually equal
  invites a merge the ROM does not have.
- **A narrowing survives only if its intermediate has more than one set.**
  `(unsigned short)heading - cur` written inline loses its zero-extension to combine; routing it
  through a variable with three sets elsewhere keeps it. That is the reuse lever used to make a
  value **un-foldable** rather than to place it — a different application with different
  preconditions.

## Pool words are a size-and-count defect, and the recorded rule was half right

Both `ActorCmd_Player_World` and `Func_80aa768` hit the same class in different modes, and
together they **partially correct** the recorded "a plain `p[off] = 0;` always pools":

- **QImode, ours missing the pool word.** The ROM's `*(unsigned char *)(spr+0x26) = 0` pools, and
  that short-range fixup dumps the pending five-word pool mid-function. We emit `mov r3,#0 / strb`,
  nothing forces the dump, and that single word accounts for the **entire 4-byte size gap and the
  1-encoding count gap**, with ~20 of 94 differences being downstream `ldr [pc,#N]` offsets whose
  relocation symbols already match.
- **HImode, ours having an extra one.** `*(u16 *)(p+0x220) = 2;` × 4 gives `ldrh r3,.L76 / strh`
  where the ROM gives `mov r3,#2 / strh` — two `.short 2` entries and two extra dumps.
  Ruled out by measurement, not assumption: gcc did **not** split the store into `(set reg 2)` +
  `(set mem reg)` (no `mov` near it; the constant arrives as a pool MEM), and pooled-site counts
  are 4 under production, `-fno-gcse`, `-fno-rerun-cse-after-loop` and `-fno-cse-follow-jumps`
  alike — so not gcse PRE, not either cse pass. `*thumb_movhi_insn` *does* carry a `mov %0,#%1`
  alternative, so the pattern is not the obstacle: **the constant never reaches it as a
  CONST_INT.**

The rule was right about the outcome and wrong about the implied mechanism. The tell is whether a
single-insn constant store survives to the pool decision, and a wrong pool word cascades into
every later pc-relative offset.

## Traps

- **`char` is unsigned in this configuration, and the wrong spelling is shorter** — three sites
  gave `ldrb [r7,#0x1c]` for the ROM's `mov r3,#0x1c / ldrsb r3,[r7,r3]`, so it scores better
  while being wrong.
- **A better program can be a wrong program.** `UpdateSpriteAnim`'s cast-expression call measures
  closer on *both* axes (1648 against 1640 bytes, 680 against 674 encodings) and emits a direct
  `bl` where the ROM has a pool word plus `_call_via_r3`. The gain was deleting a veneer the ROM
  has. Count `_call_via_rN` on both sides before believing a figure.
- **`*(unsigned char *)&flags` was wrong**: the ROM's `add r0,sp,#8 / ldrb` is a narrowing store
  through a *spilled pseudo*, not `&local` — Thumb has no `ldrb rX,[sp,#imm]`. Taking the address
  for real pins the slot and inverts the whole scalar triple.
- **`duplicate_loop_exit_test`'s signature identifies the loop's source form**: guard + bottom
  test + offset giv is a real `while`; none of that, re-reading its pointer each iteration, is a
  `goto` loop invisible to loop.c.
- **An unused aggregate gets a stack slot; an unused scalar does not.** `ActorCmd_Player_World`'s
  40-byte frame hole is *declared locals the function never uses* — three dead `vec3_t` close it
  where `int spare` came back 4 bytes short, and the twin's 0x68 frame closes with the same block.
  The `expand_decl` corollary confirmed from the frame side on two functions at once.

## Also confirmed

Second confirmation of batch 302's retraction that `cmp #K / blt` with K>0 is reachable — this
time on a **loop back-edge via a named bound**, the other of the two routes. A union alias-set-0
escape restored two loads CSE had deleted (`*(int *)` store and `*(u16 *)` load are different
alias sets at -O2, so the store did not kill the load). And `sched1` was explicitly excluded on
all three functions by `-da`, per yesterday's finding.

## Process

`ActorCmd_Player_World` carries **3 pins that are a landing route, not scaffolding** — the ROM
calls through `.call_via r4`, which gcc-2.96 cannot emit since its only indirect-call pattern is
`bl _call_via_rN`. `shimcount` warns about the missing `fakematch.txt` row and is right to; that
row is due **when it lands**, because that file maps functions to landed `src/` paths.

Brief C was stopped mid-stride at the user's request and **resumed from its own context** rather
than restarted, keeping the two mechanisms it had already found.

## Open

- `UpdateSpriteAnim`'s split shape (text/data, nine `.global` labels, two `stage1.ld` rows) is
  **carried forward unverified** — no agent can run `make`. Land and gate that split on its own
  before installing its `.c`.
- ~30 frontier parks still carry stale or missing recipes. Yesterday proved each is a **disabled
  test**, not untidiness.
- Host disk ~1.6 GB free. Unresolved.
