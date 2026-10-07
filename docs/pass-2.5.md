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
