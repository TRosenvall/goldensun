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
- Compiler-generated `.s` files appear in `asm/` beside every `src/**/*.c` and
  must **never** be committed. `make clean` removes them; `git add -A asm` will
  sweep them up, so stage asm paths explicitly.
