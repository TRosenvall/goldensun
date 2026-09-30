# Batch 302 — one landing, a two-instruction park, and a census bug that hid a family with working templates

Seven agents, 28 targets, all unattempted and all 465+ instructions — the easy work is gone.
Gate green at `5c4695205413df7db52b9a184815a07783999971` at every commit.

**1 landed, 20 newly parked, 7 left at recon with no figure invented.**
State: **4,765 of 5,710 elevated (83.4%)** — 113 available, 741 parked, 76 hand-written asm,
14 ARM, 1 unmatchable.

Reconciliation, exact: remaining 946 → 945 (**1 landed**), parked 721 → 741 (**+20**), available
134 → 113 (**21 left the pool**), and 1 + 20 = 21. Those before-figures are re-measured under
the census fix below; the figures batch 301 published for the same tree were 111 available /
744 parked, and the difference is the bug, not new work.

## Landed

| function | encodings | shims |
|---|---|---|
| `OvlFunc_936_2009930` | 578 (1340 bytes, 86 relocations) | 0 — pin-free, one `CSE_CFLAGS` row |

At plain `-O2` it was **17 of 578 with size and count already exact**; with
`-fno-rerun-cse-after-loop` it is byte-identical. Flag id `0x200` is used three times — a
`__GetFlag` in one arm, then `__SetFlag` and `__ClearFlag` in the `else` arm with the first
dominating the second — which is the documented two-part constant-CSE precondition. `-fno-gcse`
is byte-identical to the default (29 either way), so only the CSE flag reaches it, exactly as
the rule predicts.

## Two instructions from byte-exact

`OvlFunc_881_200b9fc` parks at **2 of 579**, with **size exact (1300), count exact (579) and
relocations identical in both sequence and offsets**, pin-free. The entire residue is one
**adjacent transposition** at ordinal 24/25 — `adds r1,r7,r2` against `ldr r0,[pc,#264]`, the
second `__DecompressLZ` argument fill — so it is a sched1 ready-list tie between two independent
insns feeding one call, not a class blocker. `-fno-schedule-insns2` is worse, ruling out sched2.
That makes it the closest park in the tree after `Anim_Froth`'s 10 of 529.

`OvlFunc_945_200c8e8` is next at **6 of 696, 99.1% aligned**, size and count exact, no
insert/delete anywhere, pin-free.

## The census bug: an overlay address is not unique

The batch reconciled to **22** functions leaving the available pool against **21** accounted
for. Batch 301 fixed stem matching by requiring a *name* part to land on a word boundary, and
deliberately left an *address* part matching as a bare suffix because `Func_80cd52c` embeds
`d52c` after `80`. That carve-out is correct for the main ROM and wrong for the overlays:

**every overlay loads at the same base address**, so `OvlFunc_890_2008488` and
`OvlFunc_917_2008488` are different functions sharing the address `2008488`, and both end with
`_2008488`. No anchoring rule separates them. Installing one park for the first marked the
second parked — 1,122 instructions nobody had attempted.

The fix is a **bank gate**: a park in a known bank may only claim functions in that same bank,
where the bank is the hex id in the directory name (`ovl_78b2ac` and `rom_78b2ac` name the same
bank). A park with no bank in its path — `ovl_common`, a top-level class park — spans banks by
design and is not gated.

**It recovered 24 functions and over-corrected on none** — not one was named in any park. The
controls all still match: `Func_80cd52c`, `OvlFunc_common1_4cc`, `_fac`, `_78`, every case the
docstring names as real.

**The recovered work is unusually cheap.** Thirteen of the 24 are the same duplicated routine —
`OvlFunc_*_20088c0` and `_2008ba4`, 132 instructions each, appearing in a dozen overlays — and
**eight of that family are already elevated in `src/overlays/`**, so the remaining twelve have
working templates in the tree. The 101–200 band went from 0 available to 20. The largest single
recovery is `OvlFunc_925_2009af0` at 1,876 instructions.

This is the **fourth route** to the over-attribution `census.py` documents (lesson 6: a park
citing a function; lesson 7: a park's opening sentence naming a neighbour; batch 301: a name
part as bare suffix; batch 302: a non-unique address). The pattern across all four is that the
rule was generalised from the main ROM and the overlays broke it. Expect a fifth.

## A documented rule was wrong, and its correction had never landed

`docs/elevation.md`'s **"`cmp #K / bge` and `cmp #K / blt` with K>0 are UNREACHABLE"** told
readers to **exclude 33 functions across 44 sites** from worklists. It is false.

An independent scan of the **4,351 compiler-generated `.s` files** — gcc's own output, so proof
of what gcc-2.96 emits — finds **21 surviving such sites in 15 files** (23 in 16 once this
batch's landing is counted). The shape was already in the tree two dozen times.

The `combine.c` reading in that section was right but incomplete: the LT C → LE C−1 rewrite is
real, but `stmt.c`'s `emit_case_nodes` emits a switch **range test** directly as LT/GT and never
reaches `simplify_comparison` at all. Verified by reading the sources involved:

- **20 of 21 are range tests, and every one of those 15 files uses a contiguous `switch` case
  run.** This is what made `OvlFunc_936_2009930` matchable — its two sites are precisely the
  `cmp r3,#2 / bgt` + `cmp r3,#1 / blt` pair the section called a hard floor.
- **1 of 21 is a loop back-edge**, reached by a **named bound** (`n = 11; for (; i < n; i++)`),
  because combine only rewrites a constant it can see.

**Two corrections were already in the document and the original section was never struck**, so a
reader arriving there first got the excluding advice anyway. The section is now struck in place
and points at both. **The lesson is about the document: a correction that leaves the original
claim standing has not landed.**

## A tool that could destroy the tree, and did — three times

`split_s.py` deletes a tracked `.s` and rewrites a linker script. It parsed `argv[1]`/`argv[2]`
positionally with **no flag handling whatsoever**, so `--dry-run` was silently ignored and the
destructive split ran for real. **Three of this batch's seven agents** hit it, each under
instructions to touch nothing outside its scratch directory; each recovered only because it
thought to check `git status` afterwards.

Unknown options now **fail closed**, and `--dry-run` is real — it prints the writes, the removal
and the ld rewrite and mutates nothing. Both paths verified, the tree verified clean against
`HEAD` afterwards, and the dry-run independently corroborated the split shape one agent reported.

## My own briefing error

`docs/elevation.md:1473` names `Func_808bec0` in the `.call_via` structural-blocker list, and
that section ends **"Do not assign these to a survey brief."** I assigned it anyway. The agent
verified rather than trusting, found exactly one `.call_via r4` site, and correctly offered no
figure. Read the blocker lists before building a brief.

## Levers that transfer

- **A named offset local defeats `fold`'s symbol+const collapse** — worth 492 → 2 on
  `OvlFunc_881_200b9fc`, making size and count exact in one edit. The discriminator is the base:
  a **symbol** base plus a constant needs the local, a **pointer** load plus a constant does not,
  measured twelve instructions apart in the same function.
- **A same-stem oracle before writing a line.** `Anim_Hail` put `Anim_Ground`'s *first* candidate
  at 230 of 588 with size and count already exact, for one compile on 561 instructions.
- **Two walking pointers for one array** — `Anim_Volcano`'s biggest single lever, 363 → 402. With
  "one counter across five disjoint loops" (352 → 363) this gives both halves of the reuse rule in
  one candidate: the discriminator is *what the variable carries*, not whether it is reused.
- **A ROM computing a provably-constant index tells you the value is assigned on more than one
  path.** `Anim_Condemn` computes `d*7` though `d` is provably 0; `d = 0` once lets cprop fold all
  seven accesses, while assigning it inside *both* arms gives two reaching definitions — gcc-2.96's
  cprop needs a single one — worth +22 instructions.
- **Read the latch condition code**: `ble` vs `bls` separated signed from unsigned loop bounds in
  three functions where a plain C pointer compare gets it wrong.
- **Deleting a counter** can be the lever: `UI_NameEntry` 215 → 252 aligned, the low-register
  rotation rule read forwards.

## Measurement

- **The saturation warning was proved directly.** Four genuinely different compilations (different
  `.s` md5s) all reported objcmp 513/549 *and* aligncmp 271/413/99 to the digit; a one-instruction
  probe moved both. The tools are sensitive — the insensitivity was the candidate's distance.
- **objcmp can rank backwards.** `Anim_PlanetDiver`'s candidate p3 reads **148 lower** on objcmp
  (205 vs 353) and is **structurally farther** (6 insert/delete hunks vs 4). Ranked per discipline
  p5 is parked; p3 is kept and the inversion written into the header.
- **Three parks had their relocation symbol sequence exact on the first candidate** (41, 52 and 60
  symbols) — the cheapest available confirmation that the program shape is right before spending
  anything on allocation.

## New bounds on existing rules

- **The `expand_decl` corollary does not reach a pointer scalar**: `T *x[1]` read as `x[0]`
  allocates exactly like `T *x`, byte for byte. The corollary was measured on `u32`/`u8[4]`.
- **A pin is a per-function hypothesis, never a family default** — catastrophic on
  `BaseAnim_Breath` (61.7% → 35.9% aligned, measured twice), needed on `BaseAnim_Blob`. Sixth and
  seventh instances of a pin measuring worse.
- **`ldr` vs `ldrh` for a pooled register constant is invisible**: Thumb-1 has no PC-relative
  `LDRH`, so gas assembles both to the same encoding, and "fixing" `REG_BLDALPHA` to match the
  ROM's printed `ldr` cost 8 encodings.
- **Field_Force's "array holds one more pointer than it uses" lever does not generalise** — the
  extra element costs 78 aligned lines here, and `[3]`/`[4]` scoring identically says the cost is
  the declared size crossing a boundary.

## Decisions waiting on the owner

- **`OvlFunc_common1_920` is blocked, deliberately unattempted.** It needs 19 label externs and
  every one is one- or two-digit; a shape-matched probe emitted 17 of the 19 names in a single
  compile, so any candidate would be a **wrong program** that builds and links cleanly. The remedy
  renames 19 labels in the shared common1 data file several overlays link.
- **`_FILE_bf` is missing from `include/file_table.h`** (the table skips `bc` and `bf`).
  `BaseAnim_Blob` uses a local placeholder; landing needs the header entry, because a literal
  `0xbf` gives `mov` where the ROM has a pool `ldr`.
- **`UpdateSpriteAnim` is misnamed** — it is a multi-line text-label rasteriser, not an animation
  tick, on three independent confirmations. Anyone trusting the name will mis-read it, which is
  exactly the trap the census bug set.
- **Two split collisions**: `Func_809bcf8` and the existing `809c138.c` park both claim
  `rom_9bb64_c_a_a_b.c`; same shape between `Func_8020244` and `8020198.c`. Mutually exclusive —
  first to land defines the split.
- **`Anim_Bind` and `Anim_PsyphonSeal` constrain each other's `.rodata` placement** and should be
  landed in one pass. `PsyphonSeal`'s two pins are exactly inert on Bind, so they look removable
  from that park.

## Open

- **Host disk at 100%, 1.9 GB free.** Unresolved; needs ~12 GB freed and the candidates are all
  outside this project.
- **The twelve `_20088c0`/`_2008ba4` duplicates** are the cheapest work in the tree — eight
  siblings already elevated as templates.
- `Anim_PlanetDiver` (353 of 538, 85.3% aligned, relocation sequence identical row for row) is the
  likeliest of this batch's parks to close.
- Three of four `Anim_*` blockers in briefs A and B are register allocation; `Anim_Condemn` is the
  cleanest test case — a single register pair with the rest of the program known good.
