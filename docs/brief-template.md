# Brief template

The durable half of an agent brief. Per-batch material (targets, their verified
figures, their park paths, what each park claims) goes on top of this.

## Required reading, in order

1. `docs/elevation.md` — the method. Long, and the point. **grep it** before
   writing anything up as a new finding.
2. `reports/frontier.md` — the verified frontier, ranked. A park absent from it
   has no recipe and has never been measured.
3. `HANDOFF.md` last rows + the most recent `reports/batch-NN.md`.

## The standing facts

- **Every inherited diagnosis is a hypothesis.** Across pass two a park's stated
  blocker has been wrong roughly forty times out of forty-two. Its *observations*
  are usually sound; its *verdict* is the least reliable thing in the file.
- **A park with a recipe is trustworthy; one without is a coin flip.** All 646
  parkcheck-verified parks agree with their headers. Every wrong figure ever found
  here was in a park nothing had checked — including one claiming 78 of 85 that
  measures **4 of 85**.
- **Cross the lists.** One-at-a-time testing is the measured top obstacle, in six
  shapes. Use `tools/crossfire.py`: it imports the authority, flags `COUNT` when
  the instruction count differs (a positional figure on a different-length stream
  measures *misalignment*), flags `MEM` when per-opcode memory totals diverge from
  the reference (a false improvement is a *wrong program*), and reports an
  unapplied edit loudly. Rows that **tie the base** are candidate prerequisites,
  not dead ends.
- **`tools/objcmp.py` is THE AUTHORITY. Never fork it.** A forked copy drifted 32
  lines in batch 316.
- **`asm/` is a labelled corpus: every generated `.s` is byte-matching by
  construction, so each instruction window is tagged with the C construct that
  produced it.** When a reference window will not reproduce, grep the generated
  half for 4-8 of its instructions and read the `.c` beside the hit. Batch 332
  landed two of three targets that way after spelling-level search had failed on
  both. Count hits in the generated half against the hand-written half for a
  reachability bound; partition by `shimcount` for a pin-free bound.
- **If a brief's piece-membership block is EMPTY, say so and re-derive it.** In
  batch 343 all twenty blocks shipped empty: the coordinator added a filter to
  strip `piece.py`'s new duplicate-park warning, and because that warning prints
  FIRST the filter cut the entire output. Brief A caught it; the others worked
  without the data. A `piece.py` block that is empty is a coordinator bug, never a
  statement that the piece holds nothing.
- **`parkcheck` verifies at PRODUCTION flags, so a flag-dependent regression is
  invisible to it.** Batch 343 brief A found an installed park body that had
  silently regressed to one instruction LONG under the very flag its own proof is
  written against, while reading identically to the correct body at production
  flags. If a park's argument depends on a flag group, re-measure under that flag
  before trusting its figure.
- **DO NOT TRUST A BRIEF'S PROSE DESCRIPTION OF A TOOL — read the tool.** Every
  tool in `tools/` carries its current behaviour, its known defects and its
  measured hit rate in its own docstring, and that is the authority. A brief is
  written by copying the previous batch's brief forward, so a tool description in
  one goes stale the moment the tool changes.
  **This has cost three briefs real work.** `autosweep.py`'s anchor axis has swept
  every statement boundary at ANY brace depth since batch 338, and inside
  `static inline` helpers the subject calls since 341 — but the shared brief still
  said "every top-level statement boundary" through batch 343. Three briefs read
  that, believed it, and reported it back as a tool defect; one hand-rolled four
  nested anchor placements it did not need, on a function where **35 of 42
  boundaries are at depth ≥ 2 and the deepest is 5**. Measured, not argued.
- **THE TWO CHEAPEST AXES MAY ALREADY BE CLOSED — check the brief.**
  `tools/autosweep.py` runs the judgment-free half of a park from bash at zero
  token cost: the **flag axis** (the unchanged body under each per-file flag
  group) and the **anchor axis** (`do { } while (0);` at every top-level
  statement boundary). When a brief says a park has been autoswept, those are
  recorded negatives — **do not re-screen them**, and spend your round on the
  axes a script cannot reach: reading the disassembly, forming a hypothesis, and
  naming the pass from the dumps.
- **`-da` writes compiler dumps to the COMPILER'S working directory**, not beside
  the output file. Batch 340 brief D's first run left 21 dump files in the repo
  root. Pass `cwd` in any helper that shells out to the compiler, and check the
  root before you finish.
- **Two environment facts that have each cost an agent a detour.** The
  container's gcc driver is **`/opt/gcc296/xgcc`** — there is no `bin/`
  subdirectory, so `/opt/gcc296/bin/gcc` does not exist (verified: the directory
  holds exactly `cc1`, `cpp`, `tradcpp`, `xgcc`). And **`-fsched-verbose=6`
  writes into the `.23.sched2` dump file, not to stderr** (batch 336 brief A) —
  if you are looking for it on the terminal you will conclude the flag did
  nothing.
- **If this brief mentions a PIECE, verify its membership with
  `tools/piece.py` before trusting anything the brief says about it.** The
  coordinator has mis-stated piece membership four times, by grouping the
  *shortlist* parks that share a reference `.s` and then describing the piece as
  if those were its only members; the real pieces held three and seven
  functions. And **ignore any brief sentence of the form "neither ships
  alone"** — it is always false. `tools/split_s.py` cuts before/target/after, so
  ANY member can be cut out alone and no member is hostage to another's figure.
  Three briefs across batches 335-336 had to reject that premise. Write one
  candidate per function unless you have measured a reason not to.
- **When a park names another function, read that function's LANDED SOURCE
  first.** Twin, module-mate, piece-mate, "same construct as" — it costs one
  `cat`, and seven times now the answer has been sitting in it while the park
  that named it went unread.
- **The compiler's source is on disk — read it, do not recall it.**
  `/opt/camelot-gcc/gcc-2.96/gcc` in the container,
  `~/gs_project/camelot-gcc/gcc-2.96/gcc` on the host: the exact patched
  gcc-2.96 that builds this ROM. **Quote the file and line in every mechanism
  claim**, so the next agent can check it in one command. Batch 329 found a
  mechanism written into a park header from memory that was wrong in the costly
  direction — it called the dependent-count rung "the only reachable rung" when
  `rank_for_schedule` has an earlier rung at `haifa-sched.c:4069-4096` — and
  that error had been steering two parks' probes.
- **Check `--whole`, not just `--func`.** A figure with dirty relocations is not a
  distance; three parks were found in that state.

## Required deliverables

In `scratch_elev/<batch>/<letter>/`:

- `p<N>_candidate.c` per target, with a full header: the figure, a **working
  `Verify with:` recipe naming the INSTALLED path**, the split shape
  (`datacheck.py` output and `split_s.py --dry-run`), the pin count, and what the
  residue IS.
- **`MANIFEST.json`** — one entry per target, schema in `tools/install_batch.py`'s
  docstring. This is how the batch gets installed; a target without an entry does
  not get installed.

  **THE `split` FIELD IS FOR LANDINGS ONLY.** `install_batch.py` refuses a `split`
  on an entry whose figure is not 0, and it is right to: splits run for any entry
  carrying the field regardless of figure, so a split for a park that is not
  landing is tree churn plus promoted `.global` labels for no match. If your park
  *would* need a split when it eventually lands, put the shape in `notes` and in
  the park header, and set `"split": null`.

  This has now been mis-stated to a brief in a coordinator launch prompt (345
  brief B), so if a prompt tells you otherwise, **the tool is the authority** —
  its refusal message says exactly what to do instead.
- `FINDINGS.md` — per target: the figure **you measured** (never inherited), what
  the park claimed and whether it survived, the mechanism, **the crossed pairs and
  triples you tried**, and the measured inert/worse list with figures.

## Pin policy

**Prefer a pin-free body.** If a landing needs pins and no pin-free landing is
readily available, **do not report it as a landing** — report it as a park at its
pin-free figure, with the pinned figure recorded beside it, and leave it for pass
3 (`reports/pass3-depin.md`). Give **both figures** whenever they differ.

## Rules of engagement

- Work **only** inside your own scratch directory. Do not run `make`, do not touch
  git, do not write elsewhere in the tree. The coordinator installs.
- Do not read another decompilation's `src/`.
- Never hand-edit a generated `.s` in `asm/` instead of fixing the `.c`.
- **Checkpoint findings to `FINDINGS.md` as you go.** Agents have been lost to API
  timeouts three times; the ones that checkpointed lost only their current target.
- **Declining to close is a real result.** A refuted diagnosis is not a closure; it
  is an open park with a better map. Say which parts of a diagnosis you reproduced
  and which you refuted — both are findings.
- A **device** (a fictitious symbol, a never-read union member, an `.equ`) is
  permitted as an *instrument* and forbidden as a *result*. If you use one to see
  past a blocker, label it, keep its number as a figure **about that blocker**, and
  ship the device-free body.

## What the coordinator must NOT put in a brief

**A bound, unless the mechanism that closes it can be stated.** Batch 318 had two
propagated bounds refuted, one phrased as an instruction not to look, on a question
where all three of that brief's targets then landed by doing the forbidden thing.
A lever wrongly recorded inert costs one agent one round; a **bound** wrongly
recorded closed costs every future agent the whole class, silently.

State bounds with their evidence attached: *"three MEMs measured alias set 0 in
`.19.flow2`"* survives being wrong; *"the MEMs are in alias set 0"* is a claim the
next reader builds on — and that load was alias set 22.
