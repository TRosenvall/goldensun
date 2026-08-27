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
  * write anywhere except your own `scratch/agentN/` directory

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

2. Write your candidate C to `scratch/agentN/<name>.c`. Declare every callee
   yourself with `extern`; do not include project headers unless you need a
   type that already exists (`include/` is readable).

3. Screen it:
       docker run --rm -v "$PWD:/work" -w /work goldensun-build \
           python3 tools/tryc.py scratch/agentN/<name>.c --ref <path/to/.s> --full

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

## Added for this round

Your worklist has been PRE-FILTERED with `tools/blocked_cse.py` to exclude the
one shape nothing reaches: a constant needing a pool load or two instructions to
build, used twice with one use dominating the other and no label between them.
See "Pool-constant CSE: the complete rule" in docs/elevation.md. If you hit that
shape anyway, say so and move on rather than sweeping flags at it -- six
CSE-related flags have already been measured against it.

Two things from the last round worth having:

* A DIRTY screen whose FIRST differing line is a label definition may be a false
  negative -- a label emits no bytes, and one extra label shifts every later
  position. Batch 112 unparked a function that had been sitting at "25 differing
  of 50" and was byte-perfect.
* If `tryc.py` prints `built with: O1` (or any per-file group), CHECK IT. Two of
  five such warnings last round were wrong for the file, and the check is one
  screen with `--cflags "-O2"`.

## New in round 3 — read these before you start

These came out of round 2 and are all in `docs/elevation.md` now. They are the
highest-yield things added since the last brief.

1. **`.pool_aligned` inside a loop makes `tryc.py` lie.** Four round-2 functions
   screened DIRTY at 25-48 differing lines and were byte-for-byte identical.
   The ROM's `b .LN / pool / .LN:` collapses to one label where gcc emits two,
   and everything after shifts one position. If your ref has a `.pool_aligned`
   inside a loop body and your diff looks like a uniform one-line shift, run
   `scratch/agent3/bytecheck.sh <your.c> <ref.s> <FuncName>` BEFORE changing a
   single spelling. Equal `.text` size + equal byte sequence means you are done;
   report it as byte-identical rather than as OK, and say you used bytecheck.

2. **`-fno-rerun-cse-after-loop` costs matches in loops.** Five round-2
   functions were OK on default flags and broken (23-47 differing) under it.
   Screen both ways, always, but never assume the flag is neutral.

3. **Count the `mov r0, #0` sites** in a boolean-returning function. One zero
   site = `if (A && B && C) return 1; return 0;`. Two, with one hoisted above
   the first `cmp`, = the first test is a separate early return:
   `if (!A) return 0; if (B && C) return 1; return 0;`.

4. **Three one-screen levers.** Swap the DECLARATIONS with the statements
   untouched (13 → exact once). Copy the FIRST parameter into a local to swap
   the two entry `mov`s. Use `while (1) { …; if (exit) break; i++; }` instead of
   `for (i = 0; ; i++)` when the ROM strength-reduces a table walk and you do not.

5. **Store inside each arm.** `if (c) {small} else {big}` with one store after
   the join lets gcc speculate the cheap arm above the compare (45 of 46).
   Put the store in both arms and let cross-jumping merge them (11 of 46).

6. **Read a field twice** — guard on the field, body on a local — to get the
   ROM's redundant-looking `mov`. 23 → 3 differing on one function.

7. **`ldr rN, =0` is NOT a symbol tell.** gcc-2.96 really does pool a literal 0.
   Do not go looking for a `_CONST_0`.

8. **`push {r4` in your ref means the file is not built with `-fcall-used-r4`.**
   0 of 2134 generated `.s` files have it. If every differing line is an r4/r5
   rename and the instruction count is right, screen
   `--cflags "-fcall-saved-r4"` and report that the file needs a per-file rule.

9. **Resolve the ref path yourself.** Worklist paths go stale when a `.s` is
   split. `python3 tools/showfunc.py <name>` gives the current path and also
   catches "already elevated" (tryc refuses a generated `.s`). Step 0, every time.

10. **You can screen against a `.sym` addition without touching the tree.**
    Copy the sym file into your scratch dir, add your symbol, and bind-mount it
    over the original in the same `docker run`:
    `-v "$PWD/scratch/agentN/message_plus.sym:/work/message.sym:ro"`.
    Report the needed line; do not edit the real file.

Your worklist is `scratch/agentN/worklist.json` — 16 functions, band 34-62
instructions, parks already excluded.

## Round 4 — you are working a BIGGER band

Previous rounds worked 34-62 instruction functions. Your worklist is **12
functions of 60-99 instructions**. This band has 550 unparked members and only
about 20 parks across the whole thing, so it is essentially untouched — the
levers in this brief have barely been tried here.

What changes at this size:

* **Expect more than one blocker per function.** At 40 instructions a DIRTY
  screen is usually one decision. At 80 it is often two or three independent
  ones. Fix the EARLIEST differing position first and re-screen — later
  differences frequently dissolve because they were a cascade from the first.
* **Do not judge progress by the differing COUNT alone.** A change that takes
  you 30 → 28 may be right, and one that takes 30 → 12 may be a coincidence of
  register naming. Read where the first difference moved to.
* **Loops are much more common here.** That makes item 2 above (the CSE flag
  costing matches) and the strength-reduction lever (item 4) far more likely to
  matter than they were in the small band.
* **Budget your time.** 12 functions, not 16. If one is still far off after a
  dozen screens, park it with the measurements and move on — a well-measured
  park is worth more than a half-finished match, and the coordinator can pick
  it up with fresh eyes.

Report the first-differing-line POSITION as well as the count for anything that
does not close. At this size that is the single most useful number for whoever
picks it up next.

## Round 5 — new since your last brief

Round 4 returned 29 matches from 48 functions. These are the things that changed
underneath you; all are in `docs/elevation.md` now.

1. **`tryc.py` got a fix — re-screen before believing an old measurement.** It now
   folds `mov rd, rs` = `add rd, rs, #0` for LOW registers (they are the same
   halfword, 0x1c1a). Anything previously parked at "1 differing, and it is a
   reg-to-reg move" may now be OK.
2. **The HImode-literal rule is NARROWER than the doc used to say.** Plain
   literals are right for `1…0x7fff` through a `u16 *`. Only **`0`** and values
   **≥ 0x8000** need an `int` local. And for ≥ 0x8000 you need BOTH an unsigned
   pointer and the local — through a signed `short *`, `0xb000` pools as
   `0xffffb000`.
3. **`volatile` is a reading, not a hack.** One `ldr =sym` with TWO `ldr [rN]`
   and no call between means the global is `volatile` — two textual reads in C
   are not enough, gcc CSEs them. A stack halfword stored and then loaded back
   is a `volatile` local. Prefer `volatile` over a `-fno-gcse` rule: it costs no
   per-file flag group.
4. **`-fno-schedule-insns2` is a misleading probe.** On every scheduling-shaped
   residue in round 4 it moved the first difference back to ~1 and multiplied the
   count. It is never the answer to a one-instruction scheduling difference.
5. **Name the store's DESTINATION pointer** when the ROM computes the address as
   a whole instruction before the value. In one case this dissolved a difference
   twenty positions EARLIER — the later difference was causing the earlier one.
6. **Three more one-screen levers**: `i = 0;` as its own statement is not the
   same as a `for`-init; DELETING a single-use local reaches an r0↔r4 exchange;
   deleting a loop-BOUND local moves the bound into a high register (symptom: an
   extra callee-saved push).
7. **Strict aliasing can SINK a store.** If a store lands far from where the ROM
   has it and nothing else differs, write it through a `char *` lvalue (alias set
   0, cannot move) before reaching for the scheduler.
8. **`-ffixed-r7`** is worth one screen when the ROM saves a HIGH register
   (r8–r11) and you do not, and the line-count gap is about four. Report it; the
   coordinator adds the per-file rule.
9. **Do not spend a `.sym` addition on argument ORDER.** The symbol tell governs
   hoisting across a call, not setup order within one call — measured negative.

Your worklist is 15 functions, band 60-99. That band returned 60% last round.
