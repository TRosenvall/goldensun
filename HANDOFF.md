# Handoff index

Batches of matching C ready to port into `Coaltergeist/goldensun-decomp`.

Each batch is self-contained: a table of what was elevated, what it replaces,
and anything found on the way that is worth passing upstream. Read them in
order; later ones assume the tooling from earlier ones is already in.

| Batch | Functions | Status |
|---|---|---|
| [batch-01](reports/batch-01.md) | 12 | ready to port |
| [batch-02](reports/batch-02.md) | 7 | ready to port |

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
- **Halfword constant pooling** is the single biggest blocker to elevating more
  (`docs/elevation.md`, blocker class 1). If there is a known C shape that
  makes gcc-2.96 pool a small constant as a word when the store is to a
  `u16`, that alone unblocks several functions.
