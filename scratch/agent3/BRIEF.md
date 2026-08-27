# Brief: elevating Golden Sun functions from assembly to C

You are working in a MATCHING DECOMPILATION. C you write must compile, with
gcc-2.96, to the byte-identical instruction stream the original ROM has.

Your job this run is ANALYSIS AND SCREENING ONLY. You produce C bodies that
screen OK. Someone else wires them into the build.

## HARD CONSTRAINTS -- breaking these breaks other people's work

The tree is shared with three other agents and with the coordinator, who is the
ONLY one allowed to change it. You must NOT:

  * run `make` (any target, any flags) -- builds write .o/.elf across the tree
    and two at once corrupt each other
  * run `tools/split_s.py`
  * create, modify or delete ANYTHING under `asm/`, `src/`, `overlays/`,
    the `Makefile`, or any `.ld` or `.sym` file
  * run any `git` command that writes (`add`, `commit`, `checkout`, `rm`, ...)
  * write anywhere except your own `scratch/agent3/` directory

Reading any file in the tree is fine and expected. `git log`/`git show` for
reading history is fine.

If you think the tree needs to change, SAY SO IN YOUR REPORT. Do not do it.

## Read first

`docs/elevation.md` -- the method, the blocker classes, and the working
discipline. It is long; read "The loop" and "Working discipline" at the top in
full, then GREP IT for whatever shape you hit. It is the accumulated result of
110 batches and it is why this is tractable at all.

## The loop, per function

1. Read the assembly:
       python3 tools/showfunc.py <FunctionName>

2. Write your candidate C to `scratch/agent3/<name>.c`. Declare every callee
   yourself with `extern`; do not include project headers unless you need a
   type that already exists (`include/` is readable).

3. Screen it:
       docker run --rm -v "$PWD:/work" -w /work goldensun-build \
           python3 tools/tryc.py scratch/agent3/<name>.c --ref <path/to/.s> --full

   `--quiet` for just the verdict, `--full` for the instruction-by-instruction
   diff. A screen costs 2-4 seconds -- iterate freely, that is the cheap part.

   Some functions need a per-file compiler flag. ALWAYS screen both ways:
       ... tools/tryc.py ... --cflags "-fno-rerun-cse-after-loop"
   and say which one you used in your report.

4. Iterate until `OK`, or until you have a specific, measured reason it will
   not close.

## The levers that pay off most often

Full detail is in docs/elevation.md; this is the index.

* **The basic-block lever.** A constant split around another argument, or a
  pool load issued too early, means the value must be assigned to a named local
  in a block that DOMINATES the call and is not the call's own block -- one
  local per site. Needs a branch to exist; straight-line functions cannot use it.
* **Rebuilt vs carried.** If the ROM rebuilds a value at each use: a local in a
  dominating block, one per site. If the ROM carries it in a register across
  calls: a local adjacent to its first use, shared -- but ONLY if gcc would
  otherwise rebuild it. Screen the plain-literal spelling first.
* **The return-type lever.** If the only difference is the ORDER of argument
  moves, declare the callee `int` (r0 emitted last) or `void` (r0 first).
* **CSE_CFLAGS.** A value read then written with calls between, held in a
  callee-saved register the ROM does not use, usually means the file wants
  `-fno-rerun-cse-after-loop`. Try the flag before contorting the C.
* **Jump tables.** Slot i is case (base + i). The case bodies come out in
  SOURCE order, which is routinely not numeric -- read it off the block layout.
* **Pointer arithmetic.** `add r3, r6, r2` (three operands) means one
  expression `p = base + off;`. `add r3, r2` (destructive) means a walk.
* **Shifts.** `lsl r1, #16` (destructive) means `d <<= 16;` as its own
  statement. `lsl r2, r1, #16` means `d << 16` inline.
* **HImode constants.** Storing a literal through a `short *` gives a pool load.
  The ROM's `mov` means the right-hand side is int-typed AND assigned in a
  dominating block.

## Report back

For EACH function on your worklist, in your final message:

  * the function name and the ref `.s` path
  * the final screen result (`OK`, or `N differing of M`)
  * for OK: the FULL TEXT of the C file, and whether it needs `--cflags`
  * for not-OK: the blocker in two lines, and the list of what you measured
  * the scratch path of the file

Then, separately: anything you learned that is not already in
`docs/elevation.md`. A new lever, or a measured negative, is worth as much as a
function.

Do not claim a function matched unless `tryc.py` printed `OK` for it. Say
plainly which ones did not.
