# Batch reports

One file per batch: `batch-NN.md`, numbered in the order they were finished.

Each one is written to be handed to someone else without further explanation,
so they repeat whatever context they need rather than referring back.

## What a batch report contains

- **The table.** Function name (as it already exists in the tree -- nothing is
  renamed), ROM address, the new `.c`, and what it replaces. Where a
  multi-function `.s` was split, the sibling `.s` files that must travel with
  it are listed too, because the batch does not link without them.
- **Header or shared-file changes**, with the evidence for each.
- **Anything found that is worth passing upstream** -- bugs, wrong
  annotations, layout problems -- kept separate from the functions themselves
  so it can be acted on independently.
- **Near-misses**, with their blocker class from `docs/elevation.md` and what
  was already tried, so a retry does not repeat the work.

## Rules the reports follow

- **Every claim is checked before it is written.** Addresses come from the
  linked ELF, not from memory; paths are confirmed to exist and to contain the
  function named. Batch 01 initially claimed a build bug was upstream's when
  git showed it was ours -- that is the failure mode these checks exist for.
- **A batch is only reported after `make clean && make compare` is green**, not
  after an incremental build. Stale objects have produced a green checksum in
  this project before.
- **Uncertainty is stated as uncertainty.** Where two readings of a field or a
  function are both live, both go in.
