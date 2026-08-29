# Batch 51 — eleven functions from a searchable blocker shape

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`
and sits at exactly the address its name claims.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_895_200856c` | `0200856c` | ovl_78dee8 | [ovl_30_c_c_a_c_b.c](../src/overlays/rom_78dee8/ovl_30_c_c_a_c_b.c) |
| `OvlFunc_895_20085ac` | `020085ac` | ovl_78dee8 | *(same object)* |
| `OvlFunc_895_20085ec` | `020085ec` | ovl_78dee8 | *(same object)* |
| `OvlFunc_895_2008634` | `02008634` | ovl_78dee8 | *(same object)* |
| `OvlFunc_895_200867c` | `0200867c` | ovl_78dee8 | *(same object)* |
| `OvlFunc_895_20086c4` | `020086c4` | ovl_78dee8 | *(same object)* |
| `OvlFunc_895_200870c` | `0200870c` | ovl_78dee8 | *(same object)* |
| `OvlFunc_895_2008754` | `02008754` | ovl_78dee8 | *(same object)* |
| `OvlFunc_935_2008368` | `02008368` | ovl_7bf5a8 | [ovl_2e0_a_c_a.c](../src/overlays/rom_7bf5a8/ovl_2e0_a_c_a.c) |
| `OvlFunc_935_20083e0` | `020083e0` | ovl_7bf5a8 | [ovl_2e0_a_c_c.c](../src/overlays/rom_7bf5a8/ovl_2e0_a_c_c.c) |
| `OvlFunc_941_2008460` | `02008460` | ovl_7c5efc | [ovl_30_c_a_c_c_c_a_c_b.c](../src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c_b.c) |

**Eleven for eleven.** All were found by *searching for a blocker*, not by
ranking candidates.

## Batch 50 stated the rule; this batch searched for it

Batch 50 named the recognition rule for constant-CSE:

> **A flag id read in a guard and written in the body.**

That is mechanically detectable — a pooled constant loaded into `r0` before two
or more `__GetFlag`/`__SetFlag`/`__ClearFlag` calls in one function. The sweep
found **19 unelevated functions** carrying it, eight of them in a single `.s`.
Eleven are elevated here; eight remain queued.

This is worth separating from the shape-group work in batch 50. `shape_groups.py`
ranks by *opcode signature* and its largest group turned out to be uniformly
blocked. Searching for a **known blocker with a known fix** is the opposite
trade: every hit is expected to convert, and eleven of eleven did.

Each was also screened **without** the flag to confirm it is genuinely needed —
18 against 16, 19 against 17, 25 against 23 — so no rule was added speculatively.

## Eight functions, one object, one Makefile rule

The eight `ovl_78dee8` members are contiguous in the `.s`, so they go into a
**single `.o` under a single rule** rather than eight of each.

That is a deliberate trade against the tree's usual one-function-per-file habit.
With fourteen `-fno-rerun-cse-after-loop` rules already present and the standing
item in `HANDOFF.md` questioning whether the flag is a per-file choice at all,
adding one rule instead of eight matters more than the slightly larger file.

`tools/split_s.py` cuts at **one** function, so this needed the redundant tail
object removed from the linker script by hand once the `.c` covered its
contents. **First time a split has been collapsed rather than extended.**

### What varies across the eight is not uniform

Two differences that a family template would have got wrong, both read off the
individual listings:

- **Six of the eight end with a call** to `OvlFunc_895_20097c0(0)`; the first two
  branch straight to the epilogue.
- **`OvlFunc_895_20085ac` inverts the flag pairing.** Everywhere else the first
  tile value sets the *second* flag; there it sets the first.

The flag ids that are multiples of four are written as shifts (`0xc0 << 2`)
because the ROM builds them with `mov`/`lsl`; the rest are pooled and written as
literals. That is just what fits an eight-bit immediate.

## The count is now the argument

`HANDOFF.md`'s standing item on this flag has carried a hedge since batch 25 —
evidence "thin", possibly *"gcc-2.96 runs a pass the original compiler did not,
in which case these two rules should be dropped"*. **It was written when there
were two files.**

There are now **fifteen rules** covering **twenty-two functions**, with eight
more candidates queued behind them. That is no longer comfortably read as *"the
original build used this flag on these particular files"*.

If it is a compiler difference, the right fix is compiler-level and every one of
those rules should come out together. That is a maintainer's call rather than
ours — but every batch that adds a rule makes it more pressing, so the standing
item now states it as the open question it has become instead of a footnote
about two files.

## `tryc.py` now rejects unknown options

`--func <name>` was passed to `tryc.py` for **many rounds**. It is not an option
and never was: every function in the `.c` is compared against the reference by
name, so there is nothing to select.

It was harmless only because scratch files hold one function. This batch's
eight-function file exposed it — the tool reported **one** result and printed
**the same name eight times**, which reads exactly like eight passes.

A flag that silently does nothing is worse than one that errors, because it
reads like a filter that is working. That is the same failure mode already
recorded inside `pick_candidates.py`, where a mis-ordered test made
`has_arg_interleave` reject nothing and look like it worked.

`tryc.py` now exits with the list of known options when given anything else. The
real behaviour — *every function in the file is checked* — is what made the
eight-member screen trustworthy once the phantom flag was gone.
