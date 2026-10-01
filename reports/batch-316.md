# Batch 316 — six agents, thirty parked functions

**Pass two.** The user's framing for this pass: *"At this point, I'd consider all
parked functions available."* AVAILABLE was 0 after batch 313 closed the
unattempted frontier, so every target here was a park with written analysis
explaining why it had not landed.

State: **4,807 / 5,710 (84.2%)**, 903 remaining, 812 parked, 0 available.
Three landings this batch, fourteen parks improved, five blocker diagnoses
refuted and one confirmed.

## Landings

| function | from | mechanism |
|---|---|---|
| `Func_80f0678` | 6 of 165 | **two edits the park held in separate lists** — one in *rejected-because-worse*, one in *inert*, never crossed |
| `GetUnit` | 16 of 31 | block-scoped multiplier (alone **16**) + function-scope `int res` (alone **15**) + one transcribed `mov r3,lr` |
| `Func_8005ee0` | 19 of 108 | natural spelling (alone **22**, *worse*) + one register pin (alone **19**, *exactly inert*) |

Both of brief C's landings are the same shape, and it is the shape this pass
keeps finding: **the ingredients measure inert or worse individually and the
whole figure jointly.** `Func_8005ee0` is the cleanest instance yet — 22 (worse),
19 (inert), 0 (together).

## Parks improved

| park | from | to |
|---|---|---|
| `dfa18_Tackle` | 8 of 402 | **2** — size, count, frame and relocations ALL exact |
| `OvlFunc_923_2009a3c` | 181 of 177 *(saturated)* | **5 of 179** — a ported twin body |
| `OvlFunc_924_200cfcc` | 5 | 5 (held; source of the port) |
| `Func_80bd424` | 337 | **333** |
| `OvlFunc_924_200968c` | 12 | **8** |
| `OvlFunc_924_200aeb0` | 12 | **8** |
| `OvlFunc_924_2008a3c` | 17 | **14** |
| `OvlFunc_923_200a030` | 13 | **12** |
| `OvlFunc_924_200d5c0` | 13 | **12** (the owed twin port, measured) |
| `InitMapActors` | 14 **+ RELOCDIFF** | 14, **relocations clean** |
| `Func_80bd3e4` | — | confirmed **0 nested**; its "8 of 32" claim was wrong for sixteen batches |

## The five findings

### 1. `REG_N_REFS` is loop-depth weighted, so every allocation claim in the tree was mispriced

`flow.c` does `REG_N_REFS (i) += loop_depth`, with depth 1 at function level and
+1 per enclosing loop. Reproduced exactly by a control: a `for` counter at depth 1
reads 7, at depth 2 reads 11. Every park that fed `allocno_compare` a
**source-level** reference count had the wrong number — including one that read
"&tmp (3 references)" against "n-1 (2 references)", concluded *not a tie*, and was
right by **4.8×** rather than by a hair.

It is also a **lever**: moving an *existing* reference deeper into a loop raises
`n_refs` with no new reference. A scratch reference placed in loop 1 made one
allocno allocate first and moved the nested TU 66 → 61 — the first time that
allocation order had moved at all. And it closes the nested TU's `fp` wall on all
three sides, replacing an open thread this project had recorded.

### 2. An aggregate is a lever, and it retired a four-step impossibility proof

`dfa18_Tackle`'s window was closed by a chain that was sound at every step and
reading the wrong insns: a reload spill-store to an alias-set-0 slot, a load the
ROM puts before it, `true_dependence` holding, sched2 unable to hoist above a
producer — therefore the blocker is reload, one pass earlier. The edit is one
declaration:

    was   DrawFn d1;  DrawFn d0;      now   DrawFn dfs[2];

Make the two pointers **one addressable object** and the spill-store and its load
never exist. There is no pair to order and the window comes out exact. A
two-member struct is byte-identical, so the aggregate *kind* is free; the array
must keep the scalars' declaration slot (moved, it reads 9).

### 3. Duplicates need a PORT, not a shared fix

`dupfuncs.py` pairs functions and the convenient reading is "whatever lands one
lands both". Measured: `2009a3c`'s twin was **saturated at 181 of 177** with its
figure withdrawn, and porting the solved body across with three renames gives
**5 of 179** — the identical residue. A duplicate group is a transfer opportunity
*with work attached*, and until the port is done the twin's figure describes a
different body entirely. Worth 181 → 5 on a park nobody had assigned. The same
rule made `200a030`'s closure transfer to `200d5c0` — measured, same figure, same
first index — and separately **struck "fix one and the other follows" from
`8096ddc`/`200a440`, because the flag separates those two.**

### 4. A recipe is not verification

**Twenty parks' `Verify with:` recipes named gitignored scratch paths, and
fourteen more named a `.s` that no longer exists.** The second cause is this
project's own success: *landing a function by split invalidates the recipe of
every other park in the same `.s`.* `Func_80f0678` landed out of
`rom_f0254_c_c.s` this session and left its sibling `80f07f0` pointing at a file
that had ceased to exist. All 34 are fixed and re-measure to their claimed
figures.

These figures were never *wrong* — `parkcheck.py` verifies the installed body and
uses the recipe only for the reference. They were **unreproducible by a human
following the recipe**, which is a quieter defect.

But one park was wrong. `80bd3e4` claimed "8 of 32" while its body measured 30,
and the body is byte-identical across that whole span — so the claim was stale
from the moment batch 300 added the recipe, and `parkcheck` would have said
MISMATCH on any day anyone ran it on that file. **Nobody did.** Attaching a
recipe to an old prose figure without re-measuring is exactly how a wrong number
survives sixteen batches.

### 5. Screen memory accesses before accepting any improvement

Two more **false improvements**, both one-word edits that improved the figure and
were wrong programs: `if (count != 0)` reads 58 from 66 at *identical instruction
count* while reading the count word **once where the ROM reads it twice**. Of 30
crossed variants only **five** pass a memory-access screen and **four of those
only by cancellation**. At this distance from zero, a better figure from doing
less work than the ROM is a likely outcome of a random search, not a rare one.

## Refuted blockers

Five more, continuing the pass's dominant finding — **every landed park this
session had its blocker diagnosed wrong.**

- "local-alloc.c:1131 ties the output to the first input" → that pass is not
  involved; the pseudo is **global** in `sl`, and the mechanism is `reload.c`'s
  **`find_dummy_reload`** taking `XEXP (plus, 0)`. The cse1 half stands.
- "register allocation ORDER" on `80f6148` → it is local-alloc's
  **destination/dying-source combine**.
- "round-robin spill-register counter" → the trace says `Using reg 3` while the
  emitted copy is r2, set later in `choose_reload_regs`.
- A dead `REG_UNUSED` QImode zero blamed for taking r3 → a **non-cause**;
  `.17.lreg` shows it and the SImode zero both in r3, not conflicting.
- `GetUnit`'s "the permutation is forced by the dead `mov r3,r14`" → **both
  halves false**, and the replacement proof is much stronger because it is
  measured on a stream where that copy is the only difference (six non-asm
  spellings all byte-for-byte, `__builtin_return_address` among them).

`Func_80c1afc`'s diagnosis **reproduces figure-for-figure** and is properly
closed — the first park this pass whose analysis survived re-measurement intact.
Two agents **declined to close** parks where they had the deciding code but no
reachability proof in either direction, which is the right standard.

## New method, recorded

- **A pin can lose a function by enabling a cross-jump the ROM does not have.**
  `GetUnit` + `register int k __asm__("r3")` gets every instruction right except
  the tail and reads 17, because the pin makes both arms' final adds identical in
  early RTL and jump optimisation merges them.
- **The loop-hoist gate is a product that cascades** —
  `threshold * savings * lifetime >= insn_count`, and forcing it shrinks the loop
  until a second insn qualifies on a later pass.
- **The MULT's operand order is fixed at expand**, not by regmove.
- **A park's figure is not a distance unless its relocations are clean**, and
  `--func` does not tell you. `InitMapActors` recorded 14 and could never have
  linked.
- All 14 pins added at plain call sites are **exactly inert** — pinning a value
  to the register it already occupies creates no reload.

## Tooling

`parkcheck.py` — two more silent-unverifiability fixes **in the checker** (fifth
and sixth found in it):

- `CLAIM3`: "PARKED at 20 of 76" matched neither existing pattern, so such a park
  reported `NO CLAIM` and went unverified. Of 15 headers matching it, 11 already
  matched (no change) and **four were being skipped**. All four now report
  honestly and **three turn out to carry a different defect**: `200ab7c`
  (57 of 530) and `200c260` (78 of 85) have **no recipe at all**, and `200bfb0`
  (960 of 1027) has **no function definition**.
- `ALTVERIFY` widened: `80bd424` honestly wrote "objcmp cannot isolate a nested
  parent" and came back `UNCHECKABLE`, which reads as a defect in the park rather
  than a property of the measurement.

The drifted `objcmp.py` forks in agent scratch are deleted. Brief F measured 32
diff lines against the authority and re-measured every load-bearing figure with
`tools/objcmp.py`; all agreed, the fork having been used only for relative sweeps.

## A mistake caught before it shipped

My first pass at the recipe fix rewrote **every** `scratch_elev` path in a park
header rather than only the recipe line — 59 files. That would have repointed
prose references to *variant* files at the park's own path, attributing each
variant's figure to the park body. It is a **measurement device** by the
definition this project already carries: it changes what is measured rather than
what is compiled. Reverted and redone narrowly against the recipe line: 20 files.

## Still open

- `dfa18_Tackle` at **2 of 402** is now the closest park in the tree, with size,
  count, frame and relocations all exact and both differing encodings real
  instructions (a two-index `ldr` swap at 223/225).
- `Func_80bd424`'s remaining 9 are each named off the histogram and independently
  checkable; `Func_80bd3e4` rides in free when it lands.
- Three parks carry figures nobody can verify: `200ab7c` and `200c260` have no
  recipe, `200bfb0` has no body.
- `shimcount.py` misses the inline-asm shim class entirely and reports zero for
  both of brief C's landings.
- Owner decisions unchanged: promote `DMA3_COPY_RW` to `include/dma.h`;
  `_CONST_1f`/`_CONST_200`. `_FILE_e4` stays **withheld** — its structural
  evidence is already accepted in `file_table.sym`, but it does not *complete*
  its function, and a good structural argument is necessary without being
  sufficient.
