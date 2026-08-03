# Handoff index

Batches of matching C ready to port into `Coaltergeist/goldensun-decomp`.

Each batch is self-contained: a table of what was elevated, what it replaces,
and anything found on the way that is worth passing upstream. Read them in
order; later ones assume the tooling from earlier ones is already in.

| Batch | Functions | Status |
|---|---|---|
| [batch-01](reports/batch-01.md) | 12 | ready to port |
| [batch-02](reports/batch-02.md) | 7 | ready to port |
| [batch-03](reports/batch-03.md) | 11 | ready to port |
| [batch-04](reports/batch-04.md) | 8 | ready to port |
| [batch-05](reports/batch-05.md) | 7 | ready to port |

Every batch is verified the same way, from a clean build:

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'

## Standing items for review

Things surfaced across batches that need a decision from someone who knows the
codebase better than we do. Listed once here rather than repeated per batch.

- **`struct Actor` was 16 bytes short** and is corrected in batch 01, with
  `include/entity.h` folded into it. The five offsets that still have two
  competing readings are documented in the header itself.
- **The annotation corpus gets mechanism right and purpose wrong** often enough
  to matter -- see `docs/attribution.md`. Any annotation ported into this tree
  should be treated as a starting point, not a finding. One was corrected in
  batch 01 (`Func_80b7e7c` does not take the arguments it was documented with).
- **What are the id namespaces?** This is now the single highest-value
  question, and it is one only you can answer.

  Where the ROM pools a constant that would fit in an eight-bit `mov`
  (`ldr r0, =1`, `ldr r2, =0xf`, `ldr r3, =0x1d`), the operand was a **symbol
  reference** in the original source — gcc never pools what it can `mov`, and
  always pools a symbol address. Verified by assembling both forms.

  `message.sym` covers message ids and `file_table.sym` covers file ids.
  Neither covers a map id (`__SetDestMap`'s first argument), a text-ink value,
  or whatever `0x1d` is in the 22-function family at `ovl_314_a.s`.

  **Measured cost: 75 of the 190 functions that sit in duplicated families are
  blocked on this.** It was reported as "three functions" in batches 04 and 05,
  which was what happened to be in front of me rather than the real number.
  Naming those spaces unblocks the class outright — see `docs/elevation.md`,
  "Tell: the ROM pools a SMALL constant".

- **Narrow constant materialisation** gates 34 functions and is half solved: a
  named `int` mask reproduces the ROM's 32-bit constant, but the instruction
  ordering resists seven attempts. `src/non_matching/overlays/narrow_constant.c`
  has the detail.
