# Pass 2.5 — put the tree back together

**Made official by the owner on 2026-10-07.** It runs **after pass 2 closes** and
**before pass 3 starts**, and it is almost entirely gated on pass 2 finishing —
see "Why it cannot run in parallel" below.

## Why it exists

Pass 2 lands functions **one at a time**, which means cutting upstream's objects
into pieces. The tree now holds **5,203 pieces** against upstream's **521 `.s`
files** — a mean of **24.4 pieces per module**. That was the right call for
landing and it is the wrong end state.

The split itself was free: a **pure subdivision**, zero crossings, ROM
byte-identical. Its one cost was **1,543 local labels promoted to `.global`**.

Pass 2.5 reverses it, because a module is simultaneously:

- **the unit upstream ships** — `stage1.ld` lists one `.o` per object;
- **the unit pass 4 renames to** — 4,462 split-named files have to become real
  module names, and `rom_79460_c_c_c_c_a_c_c_c_c_a_a_b.c` only means anything
  beside its siblings;
- **the unit a `.global` can be demoted in** — a promoted label is only
  demotable once *every* piece that references it sits in one TU, which is
  exactly what the merge creates;
- **a natural push increment**, if finished modules are going upstream.

## The work

| item | scale |
|---|---|
| merge landed pieces back into module files | 5,203 → **521** |
| demote the promoted `.global` labels | **1,543** |
| resolve artifact→function (the directory calls this "the first task of pass 3") | 785 files |
| build the **pin-group** index — pass 3's real unit, not the file | 14,517 pins |
| clear bookkeeping debt first | 145 bodyless parks, 122 with no live recipe, 2 unbooked shims |

**Do the `.global` demotion during the merge, not after.** Both operations need
the same precondition and the same verification, and doing them separately means
traversing the tree twice.

## Is the merge mechanical, or a third matching pass?

**This was the open question when pass 2.5 was proposed, and the first
measurement says mechanical.**

`tools/mergetest.py` is the instrument. Its first case, run by hand in batch
334: `ovl_d58_a.c` + `ovl_d58_b.c`, two landed pieces **adjacent in
`overlays/rom_7ac2d8/overlay.ld`**. Compiled separately they hold 67 and 15
encodings; compiled as **one TU** the result is **82 encodings, byte-identical
to the concatenation.**

A mechanism predicts that, which is why one case was worth believing:
gcc-2.96's minipool is **per-function** (every input to
`add_minipool_forward_ref` is), and cse, gcse, local-alloc and global-alloc are
per-function too. There is no obvious pass whose output depends on what *else*
is in the translation unit.

### It also retires a worry that would have blocked the plan

Batch 285 recorded `OvlFunc_932_200b738` as matching alone and reading **43
differing** "once pasted beside its sibling". Given this result, that was **a
statement having moved during the paste**, not a property of merging. Do not
cite it as evidence against pass 2.5.

### The nine-module sweep, and it reframes the risk

`mergetest.py` was then run on **every one of the 9 fully-landed multi-piece
modules**. The result is better than the single case suggested:

| module | pieces | result |
|---|---|---|
| `rom_c0/rom_3650` | 2 | **byte-neutral**, 489 encodings |
| `rom_77000/rom_79338` | **4** | **byte-neutral**, 138 encodings |
| `rom_c0/rom_3d04` | 3 | **byte-neutral**, 132 encodings |
| `rom_9000/rom_ca2c` | 2 | **byte-neutral**, 29 encodings |
| `ovl_7ac2d8/ovl_22c4` | 10 | fails to COMPILE — `__StopTask` |
| `ovl_7d0e88/ovl_2580_a` | 6 | fails to COMPILE — `__MapActor_GetActor` |
| `rom_9000/rom_e220_a` | 8 | fails to COMPILE — `AttrAccessor` |
| `rom_c0/rom_2e00` | 3 | fails to COMPILE — `gKeyPress` |
| `rom_c9000/rom_dbb24` | 3 | fails to COMPILE — `iwram_3001eec` |

> **NOT ONE FAILURE IS A CODEGEN DIFFERENCE.** Every one is a duplicate or
> conflicting **declaration**: each piece carries its own `extern` block for the
> symbols it uses, and merging collides them. Four modules merge byte-clean,
> including one at **four pieces** and one at **489 encodings**.

So the earlier worry — that pass 2.5 might be a third *matching* pass — is
answered. **It is a declaration-hygiene pass.** The merge does not move code;
it surfaces latent declaration debt that separate TUs have been hiding.

> ### CORRECTION (batch 336): that conclusion is right in spirit and too
> ### optimistic in fact, for a measured majority of the clashes.
>
> The nine-module sweep was representative — `tools/declclash.py` puts the
> blocking rate at **146 of 247 multi-piece modules (59%)**, against the sweep's
> 5 of 9 — but it understated what a clash *is*. Two corrections:
>
> **1. The failures are not "duplicate" declarations.** C permits a repeated
> *identical* `extern`. Every one of these is a **conflicting** declaration:
> two pieces of one module declaring the same symbol at **different types**,
> each type chosen because it made that piece's own function match.
>
> **2. For 163 of the 276 clashes (59%) one side is a POINTER and another is
> not** — and reconciling those is not an edit to a declaration. It is picking
> the true type and then **rewriting the other pieces' expressions so they still
> emit the same instructions through it**, because `p[i]` scales by the pointee
> and `&x` on a value is not the word stored at `x`. Every such piece must be
> re-verified. That is bounded, evidence-driven matching work, but it is
> matching work, and pass 2.5's estimate should carry it.
>
> What makes it tractable is that **the types are facts, not preferences, and the
> ROM answers them.** Every hand-written access to `iwram_3001ebc` reads:
>
>     ldr r5, =iwram_3001ebc     <- the symbol's ADDRESS
>     ldr r3, [r5]               <- the WORD STORED THERE, then offsets
>     mov r2, #0xe0                 0xe0 / 0xe4 / 0xec off it
>
> so it **holds a pointer**, and `extern unsigned char iwram_3001ebc;` used as
> `&iwram_3001ebc` is the wrong shape however well it matched in isolation.
> One `grep` per symbol settles it. We do not need another project's opinion
> about these types — we have the ROM, and `asm/` is a labelled corpus.
>
> **And the debt is concentrated, which is the good news.** 276 clashes live in
> **52 distinct symbols**, and the distribution is steep:
>
> | rank | symbol | blocks | cumulative |
> |---|---|---|---|
> | 1 | `iwram_3001ebc` | 73 modules | 26% |
> | 2 | `gState` | 54 modules | 46% |
> | 3 | `iwram_3001e70` | 27 modules | 55% |
> | 4 | `iwram_3001e40` | 19 modules | 62% |
> | … | (through rank 11) | | **81%** |
>
> **Eleven symbols carry 81% of the blocking.** So the shape of the work is not
> 276 problems; it is ~11 symbol-typing decisions taken once, tree-wide, put in
> headers, followed by re-verification of the pieces that disagreed.

### A SECOND BLOCKER CLASS: REPEATED LOCAL TYPEDEFS, AND gcc-2.96 REJECTS EVEN AN IDENTICAL ONE

`gState`'s 215 `GlobalState` declarations do not point at a header. Each of
those files carries its own copy of

    typedef struct { unsigned char _bytes[704]; } GlobalState;

and gcc-2.96 refuses the repeat outright -- **verified, not assumed**:

    $ xgcc -B/opt/gcc296/ -O2 -c td.c
    td.c:2: conflicting types for `GlobalState'
    td.c:1: previous declaration of `GlobalState'

C89 has no typedef-redefinition rule, so **two pieces that AGREE still will not
merge.** This class did not appear in the nine-module sweep's failure names
(`__StopTask`, `__MapActor_GetActor`, `AttrAccessor`, `gKeyPress`,
`iwram_3001eec` -- all symbols), so it is additional to them.

The good news is that it is the cheap half of pass 2.5. Landed sources carry
only **33 distinct local typedef names**, and just **11 modules** have a name
with two genuinely *different* bodies:

| name | local copies |
|---|---|
| `GlobalState` | 215 |
| `u8` | 18 |
| `State` | 15 |
| `Blk`, `u16` | 12 each |
| `Unit` | 10 |

Hoisting those 33 into headers carries **no codegen risk** when the body is
unchanged -- a typedef is not a declaration of storage -- so it is a pure
unblocking step, gateable before any symbol typing is decided.

### Devices must not live in declarations

`gKeyPress` is declared seven ways across 30 landed files, and one of them
explains the whole class. `src/rom_c0/rom_2e00_b.c` has:

    extern vu32 gKeyPress;
    ...
    (void) gKeyPress;

That `vu32` is not a claim about the data — it is a **device**, there to stop the
dead read being optimised away. It works perfectly in its own TU and it is
unmergeable, because the two other pieces in the module store to the same symbol
through `vs32` and `s32`.

> **A device in a DECLARATION is module-scoped debt. The same device written at
> the USE SITE is statement-scoped and merges for free.**
>
>     extern u32 gKeyPress;                      /* the real type, shareable */
>     (void) *(volatile u32 *) &gKeyPress;       /* the device, local */
>
> This is the same distinction as `docs/owner-decisions.md` ruling 4 (a qualifier
> applied to one of several reads is a device; applied to an access whose width
> is a genuine property of the data it is a lever) — and the same shape as the
> `*(vu16 *)p` pattern in pokefirered's leak-informed source, where the pointer
> is plainly typed and the one real access carries the qualifier.

**Pass-2 consequence, and it is the cheapest thing on this page:** every clash
was created by pass 2, one piece at a time, and pass 2 is still running. A guard
at install time — does this piece's `extern` for symbol S match the type the
module already uses? — costs nothing and stops clash 277. Retrofitting 276 is
work; preventing the next one is free.

### That debt is real, and worth fixing regardless

`ovl_22c4`'s failure is the instructive one: the clash is against
**`include/task.h:45` itself**, which means a piece declares `__StopTask` with a
signature that disagrees with the project's own header. That is a genuine
inconsistency sitting in the tree right now, invisible only because the two
declarations never meet in one translation unit.

**Pass 2.5 therefore owns a job nobody had scheduled**: delete each piece's
redundant local `extern`s in favour of the headers, and reconcile the ones that
actually disagree. That is work pass 4 would have wanted anyway, and the merge
is what makes it visible — and checkable, since a clash is a hard compile error
rather than a silent difference.

**Expect the failure rate to scale with piece count.** The four clean merges
have 2–4 pieces; four of the five failures have 3–10. At the tree's mean of
24.4 pieces per module, assume every merge needs a declaration pass first.

### What is NOT yet tested

The clean case was 2 pieces and 57 lines whose preamble is nothing but `extern`
declarations. In rough order of risk:

1. **a piece carrying `.data` or `.rodata`** — `ovl_d58_c` has a `.data` section
   and was deliberately excluded from the test. **Still the top risk**, since
   nothing in the nine-module sweep exercised a data section;
2. **whether a declaration-cleaned merge stays byte-neutral** — the five
   failures stopped at the compiler, so their *codegen* is untested. Fix one
   module's declarations by hand and re-run `mergetest.py` on it; that is the
   single most informative remaining measurement;
3. **many pieces** — the sweep reached 4 clean and 10 failing-to-compile; the
   mean is 24.4;
5. **pieces with different per-file Makefile flags.** This is a **hard blocker,
   not a risk**: two pieces compiled with different flags cannot be one TU at
   all, and `mergetest.py` refuses rather than returning a meaningless verdict.

## Why it cannot run in parallel with pass 2

**Only 9 multi-piece modules are fully landed today.** 246 have at least one
landed `.c`, but 237 of those still contain assembly pieces — and a module
cannot be merged while part of it is still `.s`.

So the ordering is forced, not chosen:

    pass 2    finish elevating
    pass 2.5  merge, demote, index
    pass 3    depin, by PIN GROUP
    pass 4    name and reorganise, by MODULE

The one thing that *can* be done early is **testing the merge**: run
`mergetest.py` on those 9 fully-landed modules now, and specifically on one with
a `.data` section and one with `static` helpers. If those are clean, pass 2.5 is
a scripted operation plus one gate, and the merge tool can be written while pass
2 is still running instead of at the end.

## Why pass 3's unit is the pin group, not the file

Recorded here because it decides how pass 2.5's index should be built.
From `reports/pass3-depin.md`:

> **cse1 unifies two pseudos holding the same `CONST_INT`, and unification needs
> TWO unpinned peers — so a pin defeats it only in company.**

Measured: **20 pins inert singly, 65 load-bearing together.** A single-pin
greedy sweep therefore reports a false fixpoint. Pins must be grouped by the
value they materialise and removed as a unit — and those groups **cross files**,
so the index has to be built across the merged tree rather than per file.

The encouraging counterweight: a batch-320 probe found **13 of 25** small pinned
landings are byte-identical with the pin **simply deleted** — it was never
load-bearing. If that rate holds on the long tail of 1–2-pin files, a large
share of the 785 files is nearly free. It will not hold on the file carrying
**501** pins.

## Verification

The same discipline as every other pass: **`make compare` is the gate.** A merge
that changes a byte fails, and `mergetest.py` catches it at the object level
before the build does.
