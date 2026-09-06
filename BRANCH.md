# Working branch

    trosenvall/elevate

All elevation work — every `src/**/*.c`, every park in `src/non_matching/`,
every `reports/batch-NN.md` and the `HANDOFF.md` index — is committed here.

## Why this file exists

An agent session's start-of-conversation git snapshot can be **stale**. It is
captured once and never refreshed, so after a long session (or a resumed one) it
may still name whatever branch was checked out when the transcript began. This
tree has a `trosenvall/decompExploration` branch that has not moved since
2026-08-29 and that is what the stale snapshot tends to show.

The snapshot is not authoritative. This file and `git rev-parse` are:

    git rev-parse --abbrev-ref HEAD

## Standing rules

- **Never push.** Origin is a personal fork kept for preservation.
- **Never switch branches.** If `HEAD` is not `trosenvall/elevate`, stop and ask
  rather than checking out.
- `git checkout` is denied by project policy — use `git restore`.
- `git restore src/lib` before staging; the build dirties `.i` files there.
- A generated `.s` appears in `asm/` beside every `src/**/*.c`, and it **is
  tracked** — staging it with the `.c` is the tree's convention, inherited from
  upstream with 2,535 such files. Converting a function deletes its hand-written
  `.s` and the next build writes a generated one to the same path, so git reports
  it as *modified*; that is correct, not compiler output leaking in. Verify with
  `python3 tools/asmfacts.py --asm-pairs`, which fails if an elevated `.c` lacks
  its sibling `.s`. See "The generated `.s` beside the `.c` IS tracked" in
  docs/elevation.md — this was got backwards twice.
- `git add -A asm` still sweeps up unrelated build output, so stage asm paths
  explicitly.
