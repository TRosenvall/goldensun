# Batch 222 — the batch that corrected itself

Seven functions elevated, one of them **unparked**, one long-standing park
re-diagnosed, one tool bug fixed and one published finding retracted. Clean
`make clean && make compare` green, SHA1
`5c4695205413df7db52b9a184815a07783999971`.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_896_2008d5c` | `0x02008d5c` | [ovl_314_…_a_a_b.c](src/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_b.c) |
| 2 | `OvlFunc_895_2008d1c` | `0x02008d1c` | [ovl_30_…_c_c_a_b.c](src/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a_b.c) |
| 3 | `OvlFunc_941_2009448` | `0x02009448` | [ovl_30_c_c_c_c_c_a_b.c](src/overlays/rom_7c5efc/ovl_30_c_c_c_c_c_a_b.c) |
| 4 | `OvlFunc_941_2009394` | `0x02009394` | [ovl_30_c_c_c_c_a.c](src/overlays/rom_7c5efc/ovl_30_c_c_c_c_a.c) |
| 5 | `OvlFunc_955_20096d4` | `0x020096d4` | [ovl_30_c_c_c_c_c_c_b.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_b.c) |
| 6 | `OvlFunc_883_200af14` | `0x0200af14` | [ovl_30_…_a_a_b.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_b.c) |
| 7 | `OvlFunc_942_2008ba0` | `0x02008ba0` | [ovl_30_c_c_c_c_b.c](src/overlays/rom_7c6bac/ovl_30_c_c_c_c_b.c) |

## A published finding was wrong, and the tool was why

Batch 221 published, in the report, the HANDOFF row and `docs/elevation.md`,
that **no flag reaches the constant-CSE class** — `-fno-gcse`,
`-fno-cse-follow-jumps`, `-fno-expensive-optimizations`, `-fno-force-mem` and
`-fno-rerun-cse-after-loop` all "byte-identical to the default".

`tools/tryc.py` was silently discarding bare `-f` flags. Its append loop
iterates the *Makefile-derived* set, never `sys.argv`, and `check_opts` only
validates `--` options — so the flag passed validation and was never used.
**Every row of that sweep was the baseline recompiled.**

Re-measured with the tool fixed, on `OvlFunc_943_200a9d4`:

| | lines (rom 142) | differing |
|---|---|---|
| baseline | 150 | 146 |
| `-fno-gcse` | 150 | 146 — inert |
| `-fno-cse-follow-jumps` | 150 | 146 — inert |
| `-fno-force-mem` | 150 | 146 — inert |
| **`-fno-expensive-optimizations`** | **147** | **117 — NOT inert** |

Three of the four were genuinely inert, which is exactly what made the wrong
claim look corroborated. All three places are corrected to the weaker statement
that survives: **no flag reaches the ROM** — that spelling is still five lines
long and 117 differing where the pin cure is exact. The batch 221 cures and
their byte-exact results are unaffected; only the reasoning was wrong.

Bare `-f` flags are honoured now **and echoed** as `(extra flags: …)`. The echo
is the real fix: silence was the defect, because a dropped flag and an inert
flag are indistinguishable in the output. (A second caveat found later in the
batch: tryc ignores positional `-O` flags too — `-O0` on an exact candidate
still reports OK.)

Two rules follow, both now in the doc:

- **A negative result needs a positive control.** Before believing a flag
  changes nothing, run one that must — `-fno-omit-frame-pointer` widens the
  push — and check the number moves. A sweep where every row equals the
  baseline is evidence the sweep is broken, not evidence about the compiler.
- **A tool's documented usage is not its behaviour.** This usage went into
  agent briefs and into the method doc on the strength of a comment in the
  source rather than a test.

It was found by a screening agent that ran its flags through `xgcc` directly
and got a different answer than tryc gave.

## The canonical DUP-CONST specimen matches, and the filter was rejecting on it

`src/non_matching/ovl_7c5efc/2009394.c` called itself **"THE CANONICAL SMALL
SPECIMEN"** of the duplicate-constant CSE wall. It closed its search after
**thirteen flags** with *"no spelling separates two uses of one literal… This
is the class boundary, not a function-specific problem"*, and that conclusion
was promoted into `filtered.py` as a hard reject.

**The assumption all thirteen measurements shared is that the lever had to be a
flag.** None tried writing the argument into a hard register. Two `PIN3` blocks,
one `do { } while (0)` and one `PIN2` make it byte-exact.

The reject was dropping roughly 158 candidates. It is a warning column now, and
four candidates reappeared immediately. Five of this batch's seven elevations
are functions dominated by that same class.

*A park that measured N spellings has only ruled out what those N shared* was
already in the doc. Here it was paid for at the scale of a filter.

## The prologue picks the cure

Batch 221 said the use count chooses between pinning and naming. That is not
the right question — **read the ROM's prologue**:

- `push {lr}` alone: the ROM keeps nothing, so every repeat is rebuilt and
  **only pins work**. Named locals measure *worse* — 208/199 against plain C's
  207/195 on `OvlFunc_896_2008d5c`, and 275 lines against a 271-line baseline
  on `OvlFunc_942_2008ba0`, which is worse than doing nothing.
- `push {r5, lr}`: one register, usually a base pointer. Still a pin function.
- A wider push: the ROM does keep constants, so named locals may be right, and
  some values must stay bare literals so CSE hoists them.

**Pin the FIRST use.** On `OvlFunc_895_2008d1c`, `0x9999` appears at four
sites; pinning the three `SetSpeed` sites and leaving the one `__Func_80933d4`
site open still measured 242 lines and 229 differing, with the value commoned
out of the single unpinned site and the base pointer displaced out of r5.
Pinning that first site went to 241 lines and 3 differing in one step.
`OvlFunc_941_2009448` shows the converse — an earlier *pinned* use destroys the
pseudo CSE would have handed the later site, so a second pin on the same value
is often inert.

**And the reference-count cure has a hard boundary**: on
`OvlFunc_955_20096d4`, five per-site named locals are *byte-identical* to bare
literals, because CSE commons the pseudos back together before `REG_N_REFS` is
consulted. Naming cannot separate uses CSE will re-merge.

## Other findings

**"N pins" is a size, not a set.** Individually-inert pins are not jointly
removable — six inert one at a time cost 211 differing together on
`OvlFunc_942_2008ba0`; four on `OvlFunc_941_2009448` failed at 69, and every 2-
and 3-subset had to be measured. And greedy stripping from *both ends* of the
site list converges on the same count with *different sets*, because pins
sharing a value are mutually substitutable. Minimisation names a count and a
property, not a canonical answer.

**A scheduling region is ended by a loop, not by a label.** `do { } while (0)`
and `while (0) ;` bound the region; `goto B; B: ;`, `if (0) ;`, `;` and `{ }`
are all inert. So it is not label-shifting — what bounds it is the loop note
`jump.c` leaves behind — which is why it is legitimate to ship, unlike the
label-shifting hack refused in batch 219.

**A one-statement pinned fill can replace a barrier.** Writing the whole value
in one statement makes every seed `mov` sit at depth 2 while shifts and small
movs sit at depth 1, so `sched` takes the seeds first with ties broken by
argument order. It removed nine volatile-asm barriers on `OvlFunc_955_20096d4`.

**Inside a pinned fill the shift's position is source order** — qualifying the
template's "sched2 re-lands the shifts", which holds only when the `lsl` is
last. Two functions needed this, worth 28 → 0 and 25 → 9.

**A member store and a cast store are not the same store** when the offset
needs an explicit add: the cast form emits the address first, the member form
the value first, with no call involved.

## The three-batch park was mis-diagnosed

`OvlFunc_888_200b1b8` sat at 79 lines and 74 differing for three batches
blaming a register *count*. The fifth register was holding a **store address
hoisted above a call in its own right-hand side** — naming the right-hand side
first took it to the ROM's length immediately. Now 74 lines and 17 differing.

Two corrections came with it. A pin the park had recorded as *worse* is
load-bearing once the hoist is fixed, and two pins it recorded as load-bearing
are now inert and were removed: **a measurement is only valid in the shape it
was taken in**. And the pooled zero it looked like a `const.sym` symbol —
`(int)&_CONST_0` even scored *better* by tryc — is a literal: `objcmp` shows
the reference carries **no relocation** at that pool word. A symbol hypothesis
is settled by relocations, not by line count.

## Discipline

The generated-`.s` guard caught a regeneration on the unpark commit, but I had
chained it to the `git commit` in one command and it landed anyway — the same
mistake batch 221 identified. Fixed by amend; the remaining four commits ran
the guard as a genuinely separate step and were clean. **Chaining the guard to
the commit defeats the guard.**
