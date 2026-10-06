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
