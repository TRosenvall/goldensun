# Batch 312 — rung 8, two agents disagreeing usefully, and a frame grep that hid 548 bytes

Three agents, nine targets, chosen by measurement rather than by eye. Gate green at
`5c4695205413df7db52b9a184815a07783999971` at every commit.

**0 landed, 9 parks installed, 3 tools fixed, 1 owed item cleared.**
State: **4,788 of 5,710 (83.9%)** — 28 available, 803 parked.

No landings. The batch's results are a new way an exact figure lies, a disagreement between two
agents that turned out to be two different experiments, and three defects in my own instruments —
one of which had been hiding a 548-byte stack frame.

## Parks installed

| function | figure | state |
|---|---|---|
| `OvlFunc_883_20095dc` | 1885 of 2061 | **constant set identical entry for entry** — all 49 pooled values and symbols |
| `OvlFunc_882_200b1ac` | 907 of 1071 | insns 1028 vs 1026, pool 41 vs 45, 65.7% aligned |
| `OvlFunc_951_2008e5c` | 918 of 971 | insns 946 vs 947, pool 23 vs 24, 54.4% |
| `OvlFunc_899_200b6f8` | 1100 of 1486 | **relocations 394 vs 394 in identical symbol order**, 94.4% aligned |
| `OvlFunc_957_20093f8` | 1934 of 2364 | 94.0%, saturated |
| `OvlFunc_883_200b4c8` | **960 of 1027** | pre-existing park, **first figure it has ever carried** |
| `20083cc`, `2009410`, `20088ec`, `2008d3c` | — | triage, no candidate, no figure claimed |

`899`'s 394/394 relocations in identical order means all 387 calls, 43 callees and 19 symbol
references are correct *and* correctly placed; what remains is −4 instructions and −1 pool word.

## Rung 8: an exact instruction COUNT can be a coincidence between OPCODES

The batch's strongest finding, and it defeats a discipline this project already had. Rung 7 said
compare the instruction count and the pool-word count separately rather than their sum. Still right,
and **not enough.** Under `CSE_CFLAGS`, `20095dc`'s instruction count is **1996 against 1996 —
exact** — with this histogram:

    mov +28   lsl −22   neg −4   ldr −3   sub +1      →  0

**Fifty misplaced instructions summing to zero.** Every earlier rung was a cancellation between two
things already being counted; this is a cancellation **between opcodes**, below the resolution of any
total. The instrument is the per-opcode histogram — one `grep | sort | uniq -c` per side.

Confirmed on a second function in the same overlay: `200b4c8`'s production **size is exact at 2792
bytes both sides** with **26 extra `mov` cancelling 29 missing `ldr`/`lsl`**, and its park had read
that +0 as near-success for several batches. Corrected in place.

The contrast that makes the instrument concrete: `20095dc`'s blanket-pin candidate is **two
instructions short** — a worse total — with `str −1, ldr −1` and **every other opcode exact**. The
worse total is the better candidate by a wide margin, and only the histogram says so.

### And separated-axis exactness is *still* not sufficient

A `2008e5c` candidate had the instruction count **and** the pool-word count **each separately exact**
and was structurally wrong: its `struct Vec` assignment compiled to `ldmia`/`stmia` where the
reference has three `ldr`/`str` pairs, and the two block-moves plus four instructions of pointer
setup happened to total exactly 947. **The loop body either matches or it does not, and that is
checkable by reading it, with no figure at all.** Figures rank candidates; they do not certify
structure.

## Two agents, opposite conclusions on pinning, and both are right

Briefs A and B measured the pin pass concurrently and reported opposite results. The reconciliation
is the finding, because propagating either would have been wrong.

- **Brief A** (two call-scripts, work density 2.4–2.8%): a **selective** pass beat a blanket one on
  both functions using **half the pins** — same 92.8% aligned at 159 pins versus 310, and fewer edits.
- **Brief B** (one dense function): selective pins measured **negative**, and *every partial set was
  worse than both endpoints*. Conclusion: **all-or-nothing.**

**They selected over different things.** A selected over **call sites** — which calls get a pin — in
functions where nearly every instruction is argument fill. B selected over **constants** — which
pooled values get pinned — in a function that computes.

Brief B supplied the mechanism, which is the more general half: **allocation is a zero-sum
competition.** Removing one constant's allocno hands that register to the next in `global.c`'s
priority order, so a partial constant set is not a partial version of the full one — it is a
different allocation problem. That also explains why a mixed case resisted selective pinning last
batch: *the thing being selected over is not local to the site.*

So: **over call sites, select. Over constants, do not** — read the endpoints rather than searching
the middle. Bound, measured: `ALL except 0xa00000` is byte-identical to `ALL`, because a value at
four sites that never survives a call has no allocno to remove.

## Three defects in my own instruments

**The frame grep hid a 548-byte frame.** Thumb-1 `sub sp,#imm` caps at 508 bytes, so a larger frame
is built through a register — `ldr r5,=0xfffffddc / add sp, r5` — and the documented first grep
reports **frame 0**. It reads as *frameless*, the exact opposite of the truth. `20083cc` has a
**548-byte frame with six stack aggregates**, making it the hardest frame of its group while being
six instructions **shorter** than a sibling, and my triage ranked it easiest. Re-running the fixed
tool over all 37 unattempted functions found **two** invisible frames: that one, and **`LuckyDiceMain`
at 768 bytes with 27 aggregates**, which had been sitting in the table as frameless. Only five are
genuinely frameless.

Two further corrections to the triad: its second and third greps were **mis-assigned** (`mov rX, sp`
does not appear in `20083cc` at all; `add rX, sp, #K` is the aggregate form there), and a **fourth
check** is needed — `str rX,[sp]` with no matching load is outgoing argument space for a
five-or-more-argument call, 14 sites in one function and 10 in another, **invisible to all three greps
because offset 0 forms no address.**

**`datacheck.py` under-reported a split export set, and its recipe would not link.** It matched only
`^(\.L\w+):`, so a data symbol **defined by `.lcomm`** with no label line was absent from its label
set and a function's read of it was never reported as crossing files. It named two symbols where the
correct set is **three**. Fixed; tree-wide reach measured rather than assumed at **2 files, 4
symbols** — small, and both would have been link failures found the hard way.

**`parkcheck.py` gained a `NOBODY` verdict.** A park with no function body still produces a number:
objcmp compiles the empty translation unit and reports every one of the reference's encodings as
differing. That number is meaningless but indistinguishable from a real measurement. Found on an
honest no-figure triage park that measured 2602 regardless. A sweep found 126 bodyless triage parks
and **none** claiming a figure, so this is preventive.

## A briefing error of the opposite kind from every other on this record

Every previous one has the same shape: a doc section over-claims, I propagate it, an agent measures it
away. **This one is the inverse.** I told brief C that `-fno-rerun-cse-after-loop` was "still
unsettled" and that its loop-bearing target was "the one place left to settle it".
`docs/elevation.md` already contained a section titled **"`-fno-rerun-cse-after-loop` is not a loop
phenomenon"**, measured on a loopless function where it was the *only* one of six CSE flags that
reached it, stating plainly that the name describes *when* the pass runs, not what it acts on. The
question was settled, in the opposite direction from my premise. I also recorded brief B's "has a loop
is not the precondition" as a finding when it was a re-confirmation.

The lesson is not "grep the doc" in general — **I wrote that instruction into all three briefs and did
not follow it myself.** A premise in a brief is a claim and needs the same grep the brief demands.

The flag is now measured five ways with **no correlation to loops**: only-flag-that-reaches on two
loopless functions, byte-identical on two one-loop functions, actively worse on a two-loop function.
And on one it is byte-identical **while genuinely taking effect** — under `-da` it removes exactly two
lines from `09.cse2`, so the pass really is skipped, and **the dump file still exists either way, so a
dump-presence check cannot detect this.**

## Levers

- **Lever 1 (reuse) pays at band ENTRY for POINTERS**, not just in the endgame. Merging *constant*
  ranges removes a quantity the count measures — hence the endgame precondition already recorded.
  Merging *pointer* or *counter* ranges raises a reference count and **buys a register**: on `200b1ac`
  that took size **+28 → −4**, objcmp **980 → 907**, and dropped the frame **from five slots to three**.
- **The indexed loop — deny cse1 a hoisted address.** `q = &c->v[2]` gets commoned with an argument
  **940 instructions later** and parked in r11 function-wide; that fourth long-lived quantity *is* the
  extra push. Writing `c->v[k]` makes the pointer a product of loop.c strength reduction, which runs
  after cse1 and gcse, taking a function from four high registers to **three, matching the reference's
  push list exactly.** **Attribution correction, mine:** I recorded this family as *gcse/PRE invents
  allocnos* and wrote that into the briefs, but `-fno-gcse` does not remove it here — so **run
  `-fno-gcse` before naming gcse.**
- **The int-carrier must be ADJACENT**, which resolves a contradiction *inside* the band doc: it gives
  the carrier and separately advises assigning named constants "all at the top". Carrier adjacent →
  the reference's `mov`/`lsl`; the same carrier hoisted to one set with five uses → constant
  propagation pushes it back down and the value **returns to the pool, loaded five times.** Scoped
  both: hoist a constant whose job is to **occupy a register**; keep one adjacent whose job is to
  **shape one store.**
- **Negate a live value rather than spell the negative constant** — `-(0xc0 << 10)` as a literal pools
  *both* `0xfffd0000` and `0x2ffff`; hoisting `t` and writing `-t` gives the reference's `neg r2, r4`.
  Opposite placement to the carrier, which is why "all at the top" cannot be blanket advice.

## The pooled-small-constant screen cannot name a symbol by itself

An agent first recorded that GAS collapses `ldr rX,=K` for 8-bit `K` into a `mov`, corrected a park
over it, then **assembled the code and disproved itself** — four `ldr [pc]` and four `.word`s. So those
pooled eight-bit-movable sites are real and the precondition holds. But **`force_const_mem` on a
spilled constant pseudo explains a pooled small constant as well as a relocation does**, and a second
function demonstrates that branch. The screen identifies **a site worth reading**; it does not identify
a symbol, and **no `.sym` entry may be written on the strength of it.** With brief A's finding that
**relocation parity adjudicates** — a symbol spelling that adds an entry is wrong when the count is
already exact — a symbol now needs two checks before adoption.

## Corrections to my triage, and a third population failure

- **Work density, not high-register count, orders the call-script population.** Brief A's three ran
  17 / 19 / 16 high-register mentions — **the hardest has the fewest** — while work density (the share
  of instructions that are neither call nor argument-fill) ran 3.8% / 5.2% / 11.3% and ordered them
  correctly, with hand-write sites tracking it almost linearly at 20 / 95 / 236. The band axis measures
  *wide-constant reuse*, and a function that barely computes has nothing to reuse.
- **The pure/mixed screen I put in all three briefs is wrong on both halves.** One function reloads a
  value nine times and is a *pure* rebuild, so reload count carries **no** information; raw copy count
  carries almost none, since ten of another function's sixteen copies were one pointer. The copies must
  be **partitioned**, and only the multi-instruction/pooled-constant class predicts the pin's reach.
- **Three of nine targets were branch-dense, not straight-line** (61, 76 and 79 labels), so they wanted
  the ordinary 500-instruction lever set while my brief led with straight-line mechanisms.
- **Third population failure in three batches:** the pooled-small-constant screen is **inert on all
  three** of brief B's targets — 120 distinct pooled values and the smallest is `0x101` in every one,
  so **zero candidate sites.** The mechanism is proved; the population was assumed. Rule now explicit:
  **a mechanism's proof and its population are separate claims needing separate evidence**, and the
  screen is cheaper than the brief that cites it.

## Owed item cleared: pins minimised to a fixpoint

`OvlFunc_889_2008074` landed byte-exact with **233 pins across 82 sites** and the set was never
reduced. At a fixpoint it holds **157 pins across 53 sites — 29 sites and 76 pins were not
load-bearing at all**, 35% of the sites and a third of the pins. A pin that is not load-bearing tells
the next reader the compiler needed forcing where it did not, so the un-minimised record overstated
how resistant that function is by a third.

**The oracle is a reusable finding.** For a landed function objcmp has nothing to compare against —
converting it replaced the ROM-side `.s` with gcc's own output — and `tryc.py` rightly refuses that
comparison as a tautology *for a matching question*. But the regression question is different: *does
this edit change the output from the known-good baseline?* There the tautology is the point. The
tracked generated `.s` beside every landed `.c` is therefore a **regression baseline**: one rebuild,
~15 seconds, versus a full `make compare`. That gap is what made a pass over 82 sites affordable.

The result is a **fixpoint, not a minimum** — no *single* further removal is possible, but a pair could
be jointly removable where neither is singly removable. Stated in both the file and the tool.

## Two dump-reading traps

- **gcc-2.96's `-da` dumps are not byte-comparable across runs.** Every dump prints a raw heap pointer
  in `NOTE_INSN_BLOCK_BEG`/`_END`, and from `18.greg` onward `NOTE_INSN_DELETED` prints an
  **uninitialised int**. There is a four-line noise floor, and 206-line diffs downstream of local-alloc
  were **entirely noise**.
- **Do not read a mid-pipeline dump as the output.** `09.cse2` shows a halfword test as
  `movhi + ashl 16 + ashr 16`, which reads exactly like this project's documented `ldrsh` defect and
  cost a four-variant spelling sweep. `combine` merges them afterwards and all four spellings already
  emitted the reference's `mov r2,#0 / ldrsh r3,[r5,r2]`. **There was never a defect.**

## Structural findings

- **Undefined `.L` symbols are GLOBAL VARIABLES, not labels** — found independently by both
  reconstruction agents in different overlays. One function loads six of them 23 times; they are absent
  from every `.sym` file. They are data the disassembler named like labels, reachable with
  `extern unsigned char Lxxxx[] __asm__(".Lxxxx");`, and they are a **prerequisite** to those functions
  rather than a residue in them.
- **A pool skip is still a basic-block boundary.** An analysis doc read `200b1ac` as "one loop, zero
  if/else, three branches are skips"; each of those `b` instructions **is a cse1 block boundary**. Six
  blocks, not one straight line — which is why the same `~0xc` mask is built two different ways 290
  instructions apart.
- **A normalisation trap that reads exactly like a wrong reconstruction:** `20095dc`'s constant set
  matches the reference entry for entry, but **the reference writes hex and gcc writes decimal**, so an
  un-normalised diff reports all 49 values differing on both sides. It does not report "cannot
  compare" — it reports total disagreement.
- **`-ffixed-r8..r11` gives size and count exact on `200b1ac` at the best aligned figure, and is not a
  route** — it replicates the masking signature the band doc retracted, now on a second function, so
  **the retraction stands stronger.**

## Next

- **`FieldMain`** remains the best-placed park: 3 spill slots in 965 instructions, no split needed, two
  landed callees opening with the same `galloc_ewram(0x1b, 0xccc)`.
- **`OvlFunc_969_200cbec`** at **2 of 1068** is still the closest unresolved function, and its remaining
  work is one `-fsched-verbose=8` dump read rather than a search.
- **`20095dc`**'s residue is **one spill pair** — same nine live-range classes as the reference,
  permuted between banks.
- Name the undefined `.L` data symbols in `2008d3c` and `20088ec`; both are blocked on that, not on a
  residue.

## Owed

- `2009a3c` needs re-measuring before its blocker diagnosis is trusted (saturated, figure withdrawn in 311).
- Promote `DMA3_COPY_RW` to `include/dma.h` — **owner decision**, two functions satisfy the standing condition.
- Owner decisions still open: the `.L4`/`.L5` rename in the shared common1 data file; `_CONST_1f`/`_CONST_200`.
