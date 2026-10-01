# Batch 308 — the `.call_via` route confirmed, and a contradiction resolved

*Written after the fact. Batches 308 and 309 were committed function-by-function and I failed to
publish either report or HANDOFF row at the time, so the index jumped 307 → 310. The work itself was
gated and committed as it happened; this restores the record.*

Four agents, 13 targets — every remaining function in the 401–800 instruction band. Gate green
throughout.

**0 landed, 8 parked, 3 recons.** State at close: **4,785 of 5,710 (83.8%)**.

## The structural result: `.call_via` reproduces byte-exactly in a main-ROM TU

Confirmed twice, which reopened a class that had been written off:

- a **codegen probe** (`docs/probe-call_via-r3.c`) emits the macro's expansion verbatim at production
  flags — `.align 2,0 / mov r12, pc / bx r3`, plus a harmless `.code 16` costing no bytes;
- at **object level**, `mov ip, pc` is identical, **the `.align` fill halfword sits on both sides at the
  same place**, the callee's pool offset agrees exactly, and the only residue is which register holds
  the callee.

**The spelling depends on the site count:** binding the callee inside the helper — the single-site form
that matched `Func_8097a10` — measures **worse** on a three-site function (1712 → 1704 bytes). Use the
unpinned `bx %1` form for two or more sites.

**The queue this opened: 35 remaining functions, 134 sites, nine of them single-site.** Two of those
nine were parked at 14 of 371, and `Func_808bec0` had been declined in batch 302 on the retracted
claim.

## The `int`-carrier contradiction resolved

Recorded as unresolved one batch earlier. The carrier does not change whether the constant pools
(`0x1010` is unshiftable either way) and not the printed mnemonic — **it changes which of the two pool
loads is emitted first**, and therefore which register holds the destination address:

    REG_BLDALPHA = 0x1010;                 ->  ADDRESS load first
    { int b = 0x1010; REG_BLDALPHA = b; }  ->  VALUE   load first

Read the reference's order and pick. It was **predicted, then measured** on an already-installed park
(647 → 580), and applied cleanly to a sixth function on first reading in the next batch.

## Parks

| function | figure | note |
|---|---|---|
| `Anim_CriticalHit` | **19 of 707** | size and count exact, 98.6% aligned |
| `Anim_Djinni` | **26 of 738** | size and count exact, 98.1% aligned |
| `BufferString` | 325 of 745 | **size exact**, 82.8% aligned |
| `BaseAnim_ParticleCloud` | 739 of 791 | frame exact |
| `BaseAnim_Nova` | 674 of 809 | frame exact, 58-row relocation sequence exact on v1 |
| `Func_80f3078` | 805 of 839 | 6 pins of the `dma.h` class |

## Other findings

- **The missing-case lever splits in two.** `case_values_threshold()` is **5** (probed). **Case A**
  (3–4 dense nodes) is a tree *because of the threshold* — the fourth, lowest case is real but there is
  no further hidden case. **Case B** (≥5 nodes, span ≤ 10×count) is the new tell: the formula says
  *table*, so a decision tree **is itself the signal**, and only a far-out-of-range case flips it — on
  an unsigned selector, **`case -1:`**, worth **124 bytes, 27 instructions and all 34 phantom
  relocations in one edit**.
- **I over-promoted that lever and retracted it in 310**: a tree-wide screen found only one function
  carries the Case B shape.
- **Two adjacent cases must be two duplicated arms** (`group_case_nodes` merges nodes *sharing a
  code_label*), but `case 8: case 9:` **must stay stacked** — the discriminator is purely numeric.
- **`check_dbra_loop` reverses a `for` counter into a down-counter**, so a fixed-length copy the ROM
  counts up must be `do { } while`. **Three passes care about loop form and they disagree.**
- **Reproduce the ROM's number of accesses, not a tidy single read** — the converse of the
  vec-store-through-pointer lever, completing it: the access count in the reference is *source
  information*.
- **A reference `.s` carried a literal where the ROM has a symbol.** `=0xcd` against
  `_FILE_cd = 0xcd`, which made objcmp report `RELOCATIONS differ` forever. The proof is a codegen
  argument: `0xcd` is 8-bit-movable, so gcc would `mov` it and it would **never reach the pool** — a
  pool word holding it can only come from a symbol. Fixed byte-neutral; it removed one differing
  encoding.
- **An `int` temp can STOP a pool** (1712 → 1676, worse) — so that lever has no reliable precondition
  and must be measured both ways per site.
- **A landing prerequisite can be one line in a hand-written `.s`**: `BufferString` needs a
  `.func_end_emit_size` macro call, and the `orr r2,r5` in the prologue **proves** the size is the
  linker symbol rather than a literal.
