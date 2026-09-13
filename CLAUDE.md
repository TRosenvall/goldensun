# goldensun — agent notes

A matching decompilation of Golden Sun (GBA). Work means writing original C from
this project's own disassembly so that gcc-2.96 emits byte-identical output
against `baserom.gba`.

## Read these first, in this order

1. **[BRANCH.md](BRANCH.md)** — the working branch, and why the session's start
   -of-conversation git snapshot cannot be trusted to name it. Check this before
   any git operation.
2. **[docs/elevation.md](docs/elevation.md)** — the method, the levers, the
   blocker classes and the working discipline. It is long and it is the point;
   grep it before writing anything up as a new finding.
3. **[HANDOFF.md](HANDOFF.md)** — the batch index. The last row is the current
   state of play.

## Build and verify

    docker run --rm -v "$PWD:/work" -w /work goldensun-build sh -c \
      'make AGBCC_DIR=/opt/agbcc -j8 && make AGBCC_DIR=/opt/agbcc compare'

`AGBCC_DIR=/opt/agbcc` is required in-container: the checked-in `tools/agbcc` is
a Mach-O binary and will not run there. Target SHA1:
`5c4695205413df7db52b9a184815a07783999971`.

Screen candidates with `tools/tryc.py` before touching the build. A commit is
gated on the build and compare both passing.

## Never

- push, or switch branches (see BRANCH.md)
- read another decompilation's `src/`
- hand-edit a generated `.s` in `asm/` instead of fixing the `.c` beside it
- write AppleScript files

Note on the third: this line used to read *"commit compiler-generated `.s` files
from `asm/`"*, which contradicted
[docs/elevation.md](docs/elevation.md) — see "The generated `.s` beside the `.c`
IS tracked — commit it". The tree carries **3,914** tracked `.s` files bearing
gcc's own banner, and `.gitignore` deliberately does not cover `.s`. Converting a
function deletes its hand-written `.s` and the next build writes a generated one
to the same path, so git reports it as *modified*; that is expected and the new
file belongs in the commit. Following the old wording through batches 257–259 left
17 elevated files without their generated `.s`, which is how the contradiction was
found.
