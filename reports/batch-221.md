# Batch 221 — the constant-CSE batch

Five functions elevated, three parked (one of them deliberately, from a
byte-exact match), one park deepened. Clean `make clean && make compare` green,
SHA1 `5c4695205413df7db52b9a184815a07783999971`.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_881_200b84c` | `0x0200b84c` | [ovl_30_c_c_a_c_a.c](src/overlays/rom_77a7c8/ovl_30_c_c_a_c_a.c) |
| 2 | `OvlFunc_916_2008a90` | `0x02008a90` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a_a_a_c_b.c) |
| 3 | `OvlFunc_943_200a9d4` | `0x0200a9d4` | [ovl_30_…_a_c_c_b.c](src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_c_b.c) |
| 4 | `OvlFunc_954_2009214` | `0x02009214` | [ovl_30_c_c_c_c_a_b.c](src/overlays/rom_7db0c8/ovl_30_c_c_c_c_a_b.c) |
| 5 | `OvlFunc_885_20092a0` | `0x020092a0` | [ovl_30_c_c_a_c_a_c_b.c](src/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_b.c) |

## The theme: a constant used twice, and what the ROM does about it

Three of the five landed on the same blocker, arrived at independently, and the
useful part of this batch is that **the cure is not the same one twice**.

The shape: a value expensive enough to need two instructions (`mov` + `lsl`) or
a pool load, used at two or more call sites on one straight-line path. `cse_main`
commons it into a pseudo; because the uses straddle calls, the allocator must
give that pseudo a callee-saved register. Once r5–r7 are spoken for, gcc reaches
into r8–r11 and the function grows a high-register prologue and epilogue the ROM
does not have. It reads as a length difference first — 150 against 142, 198
against 184, 243 against 238 — and only then as hundreds of differing lines.

**No flag reaches it.** — *CORRECTED IN BATCH 222; the sweep behind this
sentence was invalid.* `tools/tryc.py` silently discarded bare `-f` flags
passed on the command line, so every row of the sweep was the baseline
recompiled. Re-measured with the tool fixed, on `OvlFunc_943_200a9d4`:
`-fno-gcse`, `-fno-cse-follow-jumps` and `-fno-force-mem` are genuinely inert
(150 lines, 146 differing, unchanged), but **`-fno-expensive-optimizations` is
not** — it gives 147 lines and 117 differing. What survives is the weaker
claim: no flag reaches *the ROM*, since that spelling is still five lines long
and 117 differing where the pin cure is exact. The cure shipped in this batch
and the byte-exact results are unaffected; only this reasoning was wrong.
(`-fno-rerun-cse-after-loop` *does* reach a related second-pass case — see
`OvlFunc_890_20089f4` below — so the two must not be conflated.)

Three cures, and which one applies is decided by how many uses there are:

- **`OvlFunc_943_200a9d4` — pin the argument register.** A value assigned to a
  hard call-clobbered register is dead across the next `bl`, so gcc has nothing
  to carry it in and must rematerialise. Pinning the two `SetSpeed` fills
  dropped one long-lived value, pinning four `__Func_80921c4` fills dropped the
  other with all the r8 traffic: 150 → 143 lines.
- **`OvlFunc_954_2009214` — lower the reference count instead.** Giving each
  duplicated value its own named local in the dominating block takes every
  pseudo to `REG_N_REFS == 2`, and local-alloc rematerialises at the use.
  191 differing → 14. One named local per value is enough; naming both copies
  buys nothing.
- **`OvlFunc_885_20092a0` — thirteen pins, and nothing else works.** Plain C is
  236 of 238 differing. The scaffolding was minimised by measurement, not by
  eye: 23 sites stripped one at a time, 11 inert and removed, the surviving 12
  all load-bearing under a second round.

And the inverse, in the middle of `OvlFunc_954_2009214` itself: `0x94 << 1` is
used at **four** sites and the ROM *does* keep it in r5 throughout. Naming it
costs 77 differing. Left a bare literal, CSE hoists it with the ROM's own
interleaved placement. So the rule is not "name duplicated constants" but
**name the ones gcc should not hoist and leave the ones it should** — the same
polarity as the loop-invariant rule, appearing twice in opposite directions
within one function.

## Other findings

**A cross-jump rule inverted, and it is a scheduling lever**
(`OvlFunc_916_2008a90`). The standing rule is to put a call in every arm and let
`jump.c` cross-jump it. Here the call must go *after* the if/else. The
cross-jumped tail is the same five instructions either way; what changes is the
order *inside* the arms. `sched2` breaks a priority tie on dependent count in
`rank_for_schedule` before falling through to insn order, and with the call in
the arm, argument 3's `mov r2, #0` gives one shift a third dependent. Ten
spellings sat on a 23-differing plateau varying operand order, naming, casts and
flags — all of them sharing the assumption that the call had to be duplicated.

**The guard of a table walk must be a separate `if`, not a `while`** (same
function). The ROM loads the same halfword twice in the preheader. A `while`
lets `jump.c`'s `duplicate_loop_exit_test` copy the latch test up and CSE serves
both from one load; a hand-written guard plus `do { } while` is generated
independently, takes the memory-form `ldrsh`, and lets PRE insert the raw load
separately. 80 differing → 27.

**Splitting a shift off its load swaps which callee-saved register each of a
coordinate pair gets** (`OvlFunc_885_20092a0`, new). Both forms emit the *same
eight instructions in the same order* — only the register numbers differ. Birth
order is identical and declaration order is inert, so this is neither the
pointer-birth-order rule nor the statement-order rule. What changes is how many
RTL insns each pseudo's definition splits into at expand, which reorders the
allocator's work list. "Everything right but two callee-saved registers swapped"
now has a third thing to try, and it costs nothing.

**The extern's element type decides whether gcc folds a symbol-plus-offset into
the pool** (`OvlFunc_886_2008368`, parked). Against `extern unsigned char
gState[]`, `*(short *)(gState + (0xe1 << 1))` folds to one pool word
`=gState+450`. Against `extern short gState[]`, `gState[0xe1]` does not fold and
emits the ROM's base-and-add. Forcing the base through a named local is
byte-identical to the subscript form, so it is the declared type doing the work.
**Coming out short with a folded `=sym+offset` where the ROM has a bare symbol
is the tell.**

**An earlier *use* of a literal serves as the interleave lever's definition**
(`OvlFunc_890_20089f4`, parked at five instructions). The documented lever asks
for a named local in a dominating block. It does not need one: a site with bare
literals comes out in the ROM's order when an earlier site already defined that
value, because CSE rewrites the later literals as a reference to the earlier
pseudo, which never gets a hard register. That is why the guarded half of that
function fell so cheaply — and why the first two calls cannot be reached at all.

## A fourth mis-scoped -O1 wildcard

`rom_78603c/ovl_30_c_c_a_c_a%` captured `OvlFunc_885_20092a0`'s split product on
prefix alone. The function is exact at -O2 and 32 encodings wrong at -O1. An
explicit non-pattern rule overrides it, as at `Makefile:289` — its two genuine
siblings under that wildcard do want -O1. Worth carrying: **`objcmp` run against
the original `.s` path reports `(built with: O1)` while the same candidate on a
scratch path gets the tree default.** One file, two answers, and the difference
decides whether it looks like a match.

## Parked

- **`Func_80b9554` — parked deliberately, from a byte-exact result.** objcmp:
  176 bytes, 81 encodings, 7 relocations identical. **It is a GCC nested
  function** — the static-chain class — and that was *proved*, not inferred:
  written as a genuine nested function inside a stand-in parent, gcc-2.96 emits
  it byte-identical to the ROM. Faking that from a non-nested function needs a
  read of a hard register nothing writes, a pointer laundered through
  `__asm__ __volatile__`, and a dependence on that asm's placement to order two
  moves — none of which means anything to a reader, and any nearby edit breaks
  it silently. The fix is a whole-file job: elevate the caller `Func_80b9724`
  and write this function and its sibling `Func_80b9604` nested inside it.
- **`OvlFunc_890_20089f4`** — five real instructions, unguarded arg-interleave.
  The full spelling table is in the park *including the flags that measure no
  change*, so the space is not re-walked.
- **`OvlFunc_886_2008368`** — 145 against 143 lines; an extra `b` to the next
  label says the first block's structure is not the ROM's.
- **`OvlFunc_888_200b1b8` (deepened)** — batch 220's one-variable lever does not
  transfer across types. Both cast directions produce byte-identical, worse
  output.

## Discipline

The generated-`.s` guard was run as a separate pre-commit step every time, and
**it caught a regeneration on the first commit of the batch** — the same trap
that required an amend in batch 221's predecessor. Running it chained to the
commit is what let it through last time; running it standalone is what caught it
this time.

One commit needed an amend for a different reason: `split_s.py`'s edit to
`overlay.ld` was left unstaged, which would have left the tree unbuildable from
clean. **List a split's products and its linker-script edit before constructing
the `git add`.**
