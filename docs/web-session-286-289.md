# Web session, 2026-09-27: batches 286-289 — a handoff for the local agent

This is a complete account of one Claude Code on the web session that ran four batches
(286, 287, 288, 289) on `trosenvall/elevate`, written so the local agent can pick up without
reading the transcript. It supplements, and does not replace, the per-batch reports
(`reports/batch-286.md` .. `reports/batch-289.md`), the four HANDOFF rows, and the new
sections of `docs/elevation.md`. Where those say more on one point, they are cited.

## 1. Headline numbers

| | start of session (after batch 285) | end (after batch 289) |
|---|---|---|
| functions still in assembly | 1,152 | **1,030** |
| of which **unattempted** | ~517 | **369** |
| of which **parked** | 545 | **571** |
| not C at all (76 hand-asm + 14 ARM) | 90 | 90 |
| functions from C | 4,558 | **4,698** |
| park files | 554 | 571 |
| fakematch.txt rows | 521 | 530 |
| parkcheck OK | 69 | 101 |

**122 functions landed in four batches**, every one gated on its own `make` + `make compare`
and each batch closed by a clean `make clean && make -j8 && make compare` →
`goldensun.gba: OK`. Every commit was pushed to `origin trosenvall/elevate`. 47 commits.

| batch | agents × targets | landed | from parks | new parks | whole-file | notes |
|---|---|---|---|---|---|---|
| 286 | 5 on parks, then 11 × 5 fresh | **58** | 14 | 13 | 23 files | mixed parks + fresh; 61-100 band emptied |
| 287 | 10 × 5, unattempted only | **34** | 2 (bonus) | 18 | 18 files | 104-149 insns; 64% hit rate |
| 288 | 5 × 5, unattempted only | **18** | 0 | 8 | many | overlays + rom_8a000 |
| 289 | 3 × 4, unattempted only | **12** | 1 (bonus) | 2 | several | 10 of 12 targets |

**How to read the census.** `python3 tools/census.py` is the authority. Two traps:
- It counts a function as *parked* if ANY park file names it. A new park that mentions its
  parent or a `.s` neighbour moves that function from "available" to "parked" without it ever
  being attempted (seen twice: Func_80b9724, DrawText). The user tracks **unattempted** and
  **parked** separately; report both after every batch.
- A `.s` that spells its macro `.thumb_Func_start` (capital F; e.g. the old Func_a1f74) is
  missed by case-sensitive greps. Use `grep -i` when diffing function lists.

## 2. How the session ran (reusable orchestration)

The user asked for **N agents at M functions each, with the head agent alone running anything
that could race and alone pushing.** That division held with zero tree conflicts across 34
agents. The mechanics:

1. **The head picks targets** with `python3 tools/census.py --list 1 300` (smallest
   unattempted first), resolves each to its `.s` with `tools/showfunc.py NAME | head -1`, and
   **groups each agent's targets by bank / `.s` family**. Grouping mattered more than count:
   single-bank agents cost ~23-43k tokens per landing, mixed-bank ones 88-140k.
2. **Each agent gets a scratch dir and a common brief** (full text in §8). Agents may only
   compile and screen (`tryc.py`, `objcmp.py`, gcc `-da` dumps) inside their scratch dir. They
   never run make, never touch `src/`/`asm/`/linker scripts/`tools/`, never run git, and write
   park drafts as `<Name>.park.c` in scratch.
3. **The head lands every result**, one agent report at a time:
   - whole file (`.s` holds only that function): write `src/<same path>.c`, `objcmp --whole`,
     `git rm` the `.s`, build, compare;
   - split: `tools/split_s.py OLD.s NAME`, build + compare (**must be byte-neutral**), write
     `src/<...>_b.c`, `rm` the `_b.s`, build, compare;
   - after any split: `tools/repoint_parks.py OLD.s` (new, see §4) and hand-fix any park it
     lists as UNCHANGED;
   - `tools/asmfacts.py --orphans` and `--asm-pairs`; stage with explicit paths; commit;
     push.
4. **Park drafts** go to `src/non_matching/<bank>/<addr>.c`; replace `<this file>` in their
   Verify recipe; run `tools/parkcheck.py` on them before commit.

**Agent count vs functions per agent** (the user asked; this is the answer that was given
and measured): fixed start-up cost per agent is ~30-50k tokens, so 4-6 related functions per
agent is cheapest per landing; above ~8 the growing context makes later targets expensive.
More agents mostly changes wall-clock, not total cost; the head's serial landing is the
bottleneck at 15.

## 3. Tree changes that are not functions

### Tools
- **`tools/objcmp.py`**
  - private `mkdtemp` directory per run -- the fixed `/tmp/objcmp` let concurrent agents read
    each other's `ref.o`/`cand.o` and report each other's scores;
  - **`--whole`**: compares a whole translation unit function by function (reference assembled
    as built, candidate with the Makefile's trailing `.text / .align 2, 0`, per-function
    breakdown by objdump symbol, section-tail check). This is the harness batch 285 asked for.
    `--func` on a multi-function candidate is meaningless (it compares the whole candidate
    object against one function) -- use `--whole`.
- **`tools/parkcheck.py`**: accepts a `\`-continued `--func` in a Verify recipe (it used to
  lose the `--func` and fail on multi-function `.s` files). Also note its claim regex wants
  `N encodings of M` near the top; agents' headers often phrase counts differently and read as
  NO CLAIM -- add a line `NON-MATCHING: N encodings of M differ (objcmp).`
- **`tools/repoint_parks.py`** (new) and **`tools/land_header.py`** (new) -- see §4.

### Symbols (all named by value, each completes its function, nm-verified to resolve once)
- `file_table.sym`: `_FILE_1a = 0x1a` (a `__GetFile` id; completes OvlFunc_880_2008054 and
  OvlFunc_879_2008454).
- `message.sym`: `_MSG_af7 = 0x0af7` (`m`, `m + 1` beside the admitted `_MSG_182`).
- `const.sym`: `_CONST_b = 0xb` (a pooled plain-int call argument; `_CONST_2`/`_CONST_0` are
  the same shape).
- **Withheld** because they do not complete their function: `_MSG_ad4`, `_MSG_b2c`
  (Func_80a524c), `_MSG_1d` (Func_8021488).

### Linker / asm
- `_TBL_L16 = .L16;` in the three overlay.ld files that link common (rom_7db0c8, rom_7ddb88,
  rom_7e0928), beside `_TBL_L10..L13`.
- 17 `.global .L*` exports placed **immediately before their labels** (not in the preamble --
  split_s.py refuses a preamble holding more than includes), plus `.global .L68c` / `.L6a0` on
  `.lcomm` lines and `.global .L9` in `common1_c_c_b.s`. None emits bytes.
- `rom_f6008_c.s` renamed `rom_f6008_c_c.s` before splitting it, because its `_b` suffix was
  already an elevated neighbour's generated build input (split_s.py refuses that collision).
- Two `.s` files with a `.data` tail were converted as TWO-function objects: split_s cut code
  from data, then the two code pieces were merged back into one `.c` (delete the second piece's
  `.text`/`.data` lines from overlay.ld). rom_793768 `ovl_314_c_c_c_c_c_a.c`, rom_7eaf28
  `ovl_314_c_c_c_c_a.c`.
- **Nested function**: `src/rom_15000/rom_15e8c_a_c_a_a_b.c` now holds Func_8016018 with
  Func_8015fb8 nested inside it (the old r9-binding transcription is gone; one stage1.ld line
  removed). Symbol `Func_8015fb8.0` is local.

### fakematch.txt (+9 rows, 521 → 530)
OvlFunc_965_2009158, OvlFunc_950_2008898, OvlFunc_957_2008a54, OvlFunc_927_2008d90,
Func_80e46f0, StartSnow, OvlFunc_923_2008fd8, Field_Ply, OvlFunc_935_2008754 -- all register
pins or empty `asm volatile("")` barriers. `do { } while (0)` barriers were NOT booked (the
tree treats them as a real construct; 52+ landed files use them).

### Parks retired
The four stale ones batch 285 listed (Func_80f6038, Func_80f4100, Func_80a22f4,
OvlFunc_881_2009888), `src/non_matching/preheader_load_merge.c` (the class is solved), and
every park whose function landed.

## 4. Mistakes made, and the guard for each

1. **A commit ran after its `git add` failed** (a pathspec named a directory the park
   deletion had emptied; git refused the whole list). The commit held only the deletion.
   Fixed forward with a follow-up commit. **Guard: chain `git add ... && ... && git commit`.**
2. **A shell glob missed deleted files** (`asm/foo_a*.s` only matches files on disk), leaving
   two pre-split `.s` tracked. **Guard: use git pathspecs in quotes, `git add -A 'asm/..._a*'`,
   which match deletions.**
3. **Running totals given to the user were high by two** in batch 286 (60 vs 58): a merge
   double-counted, and git records a park-to-src move as a RENAME, which a `grep -v
   non_matching` filter drops. **Guard: reconcile against census and a thumb_func_start diff
   between the batch's first and last commits before reporting.**
4. **Agent park headers inverted counts** ("101 of 104", "19 of 87" -- the MATCHING counts).
   **Guard: parkcheck every park before commit.**
5. **`split_s.py` output piped to `head`** died with BrokenPipe after splitting but before
   finishing; it happened to complete. **Guard: redirect to a file, then read it.**
6. `asmfacts --orphans` flags `X.o(.data) -- .s deleted, section still wanted` after a
   whole-file conversion whose old `.s` had no `.data`. That is a false positive (ld
   contributes nothing); compare is the authority. Confirm with
   `git show HEAD:asm/.../X.s | grep section`.

## 5. Mechanisms found (the reusable part)

Each is written up with its evidence in the batch report named; the five biggest are also in
`docs/elevation.md` under "Batch 286: five mechanisms read out of the compiler".

**Scheduling**
- **The pre-header load merge is sched2 + cross-jumping** (286): sched2 fills the load-use
  stall with the counter's `mov #0`, leaving the load last before the loop's `b`; jump2's
  find_cross_jump sinks it. `do { } while (0);` between load and counter init stops it. Class
  file retired; all three members landed.
- **A callee's return type orders moves** (286; confirmed 287-289 several times): a `void`
  call does not set r0 in RTL, so a later write to r0 depends on the argument copy and changes
  its dependent count in rank_for_schedule. **Declaring StartTask/`__StartTask` `int` closed
  at least six functions**, including the Field_Cloak park.
- **Alias sets decide sched2 ties**: a union cast on a bitfield store's BASE (store_bit_field
  takes the base's alias set, expr.c:5008); a char-typed store as an ordering pin; a union on
  ONE load when -fno-strict-aliasing fixes all but one pair.
- `asm volatile("")` cuts a two-sided priority wall (fakematch); cse_insn's copy swap
  (cse.c:5972) skips notes, so a do-while does NOT stop it.

**Allocation**
- **"Three local-alloc quantities give dst = r3" is a block_alloc bug** (286, gdb): exactly
  three quantities are hand-sorted with qty numbers instead of qty_order slots; the lever is
  the count.
- **Global-alloc EXACT priority ties break on pseudo number**, the one place declaration
  order matters (287). Read `.17.lreg`; if two allocnos tie exactly, declaration order decides.
- **Reload registers rotate** (allocate_reload_reg, reload1.c:5003, `last_spill_reg`),
  skipping live pseudos; REG_ALLOC_ORDER is {3,2,1,0,...}, so a ROM with every reload in r0
  had r1-r3 live. Reorder statements so a live value holds the unwanted register.
- A dead REG_UNUSED QImode zero (left by a `|=` on a u8 member) steals an address register --
  the whole 882/883/884/887/897 overlay family's residue.
- A named constant's birth one insn later raises its local-alloc priority (read under gdb in
  find_free_reg: qty array at symbol `qty`, 40-byte stride).
- `sel = 0;` written early can rotate a whole allocation; copying a derived value into its
  own local lets an argument die in its register.

**Loops (loop.c / jump.c)**
- **duplicate_loop_exit_test copies exit blocks of <= 20 insns, and which jump pass does it
  decides strength reduction** (287).
- move_movables hoists a constant store address only if its load's lifetime clears
  loop.c:1803; `w = expr; *(u16 *)K = w;` keeps the ROM's per-iteration `ldr`.
- One loop reversed and its twin not = check_dbra_loop's giv_count == 0 (loop.c:7896); a
  count-down loop whose counter is unused is written counting UP.
- combine_movables (loop.c:1448) lets a single-set constant stand in for later HImode copies:
  `r = 0x1f; r &= c;` differs from `r = c & 0x1f`.
- A ROM loop that hoists nothing was a goto loop; only real C loops get invariant hoisting.
- A loop-entry `b` to the bottom test can be two test copies merged once their reload
  registers agree.

**cse / gcse / fold / combine**
- **GCSE hashes (zero_extend:SI (mem:QI)) apart from (mem:QI)** -- `x | x` closed a
  1-of-273 park.
- **A dead store inside a conditionally skipped block kills cse's constant reuse**
  (invalidate_skipped_block) -- a source route for a function exact only under a CSE flag
  (289).
- Fold puts constants outermost (`r + K + x` → `(r + x) + K`; `(r+K)*C` distributes).
- gcc rewrites a bound that is not an ARM immediate (`> 0x2fff` → `>= 0x3000`) and folds `||`
  into a range test; a local per limit keeps the ROM's shape.
- An `int` carrier keeps a zero-extension combine would drop; a pooled halfword zero is
  REG_EQUIV (block-local `{ int z = 0; ... }` after the address fixes it).
- A libcall with a constant divisor means the divisor was a variable at expand.

**Source shapes**
- **gcc nested functions are in the ROM**: r9 saved, read and never written, and every caller
  doing `add rN, sp, #K / mov r9, rN` before its `bl` is a static chain. Write the function
  nested in its caller in one TU; objcmp cannot score it, make compare can.
- A member array keeps base and offset in separate registers (`p->a[i + 0x32]` -- the queue
  spelling that closed Func_80b7548).
- u16 struct-field stores feed later byte stores via store_bit_field's (reg:HI) constants.
- BLKmode struct members share one SImode zero across strb/strh; a walk over deliberately
  BLKmode structs reproduces the move2add `add #0x55 / strb / add #0xf / strh` chain.
- `return 0;` in each arm stops two identical call tails merging.
- `__attribute__((const))` on explicit soft-float calls reproduces libcall allocation.
- `pop {r1}` in the epilogue means the function returns `int` (no return statement needed).
- `s8` in `include/gba/types.h` is plain `char`, which is UNSIGNED here; use `signed char`.

## 6. Waiting on the user (decisions not taken)

- **Per-file flag rows.** Three functions are exact only under a flag; docs/elevation.md
  admits a flag row only when spellings "provably cannot differ", so none was added:
  - ColorCycleVFXPalette -- exact at `-fno-gcse`; parked
    (`src/non_matching/rom_c9000/ColorCycleVFXPalette.c`; its `.s` also needs a split);
  - OvlFunc_959_2009528 -- exact at `-fno-rerun-cse-after-loop`; parked
    (`src/non_matching/ovl_7e7574/2009528.c`). **Try batch 289's dead-store lever first.**
  - Field_Ply -- exact at `-fno-gcse` without its pin; LANDED with the pin (fakematch).
- Batch 285's carried items (OvlFunc_930_20091b0's three decisions, etc.) are untouched.

## 7. Open work, cheapest first

1. **Task_Debug_SpriteTest** is exact as C (`src/non_matching/rom_9000/Task_Debug_SpriteTest.c`)
   -- it needs a hand-ordered `.rodata` split of `rom_1219c_c_c.s` so `.L1353c` links
   immediately before gcc's 8-byte initializer.
2. **Apply the BLKmode-walk move2add lever to OvlFunc_883_200dd68** -- measured on a scratch
   copy at 109 → 12 (right length); not yet applied. The same family (882_200c41c,
   884_200a440, 887_200968c, 897_200aeb0) all sit at 12 on one wall: a dead QImode zero in r3.
3. **Near parks**: Func_80b9dc4 (1 of 108), Func_808ce74 and Func_809b450 (2), Func_80b6d30
   and Func_80a524c-minus-symbols (4), common1_1ecc (3), common1_1078 (7), Func_80a77a4 (8).
4. **Nested-function jobs**: Func_8022a7c inside Func_8022b44 (RunItemScreen, ~715 lines);
   Func_80b9604 + Func_80b9554 inside Func_80b9724 (its park has the measurements; supersedes
   80b9554.c's r9 binding); rom_23178_a_a_a_a_c_a_c_b.c and the Func_80270d8 / 80e73a0 /
   80be18c parks likely share the shape.
5. **Reopen with known levers**: StartRain and StartEarthquake with StartSnow's r1-zero
   DMA3_CLEAR pin and BLD do-while; GetVenusDjinni now sits alone in its `.s`.
6. **Where the unattempted work is**: 93 at 101-200 instructions, 121 at 201-400, 85 at
   401-800, 69 above 800. rom_8a000 has been the richest bank (26 of its last 29 targets);
   rom_15000's menu/UI code and rom_9000 the hardest.

## 8. The agent brief, verbatim (last used for batch 289)

Paths inside it point at this session's scratch dir; substitute your own. Each agent also got
a `targets.txt` in its scratch dir, one `Name|instructions|asm/path.s` per line.

```
# Common brief -- goldensun batch 287 (read fully before starting)

Repo: /home/user/goldensun, a matching decompilation of Golden Sun (GBA). You write C that
patched gcc-2.96 (/opt/gcc296/xgcc) compiles byte-identically to the ROM's disassembly.
The compiler is gcc-2.96, NOT agbcc.

Read first: /home/user/goldensun/CLAUDE.md, then reports/batch-288.md, reports/batch-287.md and reports/batch-286.md (the newest
levers -- read both fully, they are short), then grep docs/elevation.md (24k lines -- grep, do not read
whole) for the constructs you meet. reports/batch-285.md has the levers before that (loop-shape rules: goto vs break; alias set 0
as a sched barrier; `do{}while(0)` plants total scheduling barriers; move2add; local-alloc
quantity counts; priority readout from -da .17.lreg; declaration-order permutation is INERT
when locals are all register-resident -- don't lead with it).

## Tools (run from /home/user/goldensun; everything is native, no docker)
- `python3 tools/showfunc.py NAME` -- print a function's ROM asm.
- `python3 tools/tryc.py CAND.c --ref asm/<file>.s` -- text-level diff screen.
- `python3 tools/objcmp.py CAND.c asm/<file>.s --func NAME` -- encoding-level authority
  short of the real build. "OK"/no XX lines = exact. The candidate must hold ONLY that
  function for --func to be meaningful.
- `python3 tools/objcmp.py CAND.c REF.s --whole` -- compares a whole TU function by
  function (use it when your candidate holds several functions of one .s).
- gcc dumps: `/opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi
  -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -Iinclude -S -da CAND.c` (do it in
  your scratch dir). Compiler source is under /opt (grep for haifa-sched.c etc. if present).
- Solved corpus: src/**/*.c (landed, byte-exact) and their generated asm/**/*.s. Grep it for
  the callee set / shape before inventing constructs -- families recur across overlays.
- Never read another decompilation project's source.

## HARD RULES
- Work ONLY in your scratch dir (given below). Do NOT run make, do NOT edit anything under
  src/ (other than the park file(s) named in YOUR brief), asm/, include/, linker scripts,
  tools/, docs/. Do NOT git add/commit/push/restore/checkout. Leave files you did not create
  alone (do not delete stray files anywhere).
- Other agents run concurrently in the same tree; that's why the above matters.
- A whole-file conversion must be verified function-by-function FROM THE COMBINED FILE
  (objcmp compiles the whole candidate but filters one function).

## Deliverable (your final message)
For each target: status EXACT / IMPROVED / NO CHANGE, the objcmp line, and the absolute
path of the candidate .c in your scratch dir. For EXACT ones: the full list of extern
declarations/structs the file needs (it must compile standalone), and whether the ROM .s
holds other functions or data (so I know whether a split is needed). For non-exact ones:
update the park file header IN PLACE (src/non_matching/...) with the new best source, an
accurate "N encodings of M" count as objcmp reports it, a `Verify with:` recipe, and what
you tried that was inert -- only if you improved it or learned something concrete. Report
any reusable finding (a lever with its mechanism) in 3-6 sentences. Be honest about
negatives; don't claim a mechanism you didn't check.

## Addendum: your targets
Your 5 targets are in targets.txt in your scratch dir (name|instructions|ROM .s). They are
FRESH -- never elevated, never parked -- so write the C from scratch. Before writing, grep
src/ for landed functions calling the same callees, and check docs/structs.md for existing
structs. Look at how landed neighbours in the same bank declare externs.
Budget your effort across all five; don't sink everything into one. For a target you cannot
close, write the best candidate to <scratch>/<Name>.park.c with a header in the park style
(see any src/non_matching/*.c): "N encodings of M" exactly as objcmp reports, a Verify with:
recipe, the blocker you identified, what was inert. Do NOT create files under src/.
Several targets share a .s with others (possibly another agent's) -- that's fine; I split.

These targets are ~100-170 instructions. For overlay (OvlFunc_*) targets, grep src/overlays for
landed functions calling the same __-prefixed callees: cutscene families recur across overlays. Families recur across banks: batch 286
landed 44 fresh functions in these same banks, so grep src/ for landed siblings (same
callees, same struct offsets) before writing -- copying a landed neighbour's idiom was the
single biggest win last batch. Useful fast checks from batch 286: `pop {r1}` = returns int;
a member array keeps base and offset in separate registers; a callee declared int vs void
can reorder argument moves; `do { } while (0)` is a zero-byte scheduling barrier.
```

## 9. Web-session environment notes

- No Docker: build natively with
  `make GCC296_DIR=/opt/gcc296 AGBCC_DIR=/opt/agbcc -j8 && make GCC296_DIR=/opt/gcc296 AGBCC_DIR=/opt/agbcc compare`.
  An incremental build is about a minute; a clean one a little more.
- `baserom.gba` comes from `TRosenvall/my-roms` via add_repo, symlinked, SHA1 checked.
- `git restore src/lib` after builds (clean every time here, but the rule stands).
- A generated `.s` beside every elevated `.c` is tracked; `tools/asmfacts.py --asm-pairs`
  was run before every commit.
