# Batch 88 — clustering by shape, and a rename that ate four files

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `OvlFunc_914_2008abc` | `02008abc` | [ovl_30_c_c_c_c_a_a.c](../src/overlays/rom_7a1ff0/ovl_30_c_c_c_c_a_a.c) |
| `OvlFunc_915_2008c8c` | `02008c8c` | [ovl_30_c_c_c_a_c.c](../src/overlays/rom_7a2bf0/ovl_30_c_c_c_a_c.c) |
| `OvlFunc_916_2008e64` | `02008e64` | [ovl_30_c_c_c_a_c_c.c](../src/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_c.c) |
| `OvlFunc_917_2009768` | `02009768` | [ovl_30_c_c_c_c_a_c_a.c](../src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_c_a.c) |
| `OvlFunc_945_20089b4` | `020089b4` | [ovl_30_c_c_a_a_c_a_a_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_a_b.c) |
| `OvlFunc_945_2008e14` | `02008e14` | [ovl_30_c_c_a_a_c_a_c_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_b.c) |
| `OvlFunc_945_2008ee0` | `02008ee0` | [ovl_30_c_c_a_a_c_a_c_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_b.c) |
| `OvlFunc_945_2008fac` | `02008fac` | [ovl_30_c_c_a_a_c_a_c_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_b.c) |
| `OvlFunc_945_2009078` | `02009078` | [ovl_30_c_c_a_a_c_a_c_b.c](../src/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_b.c) |

Nine — above the usual five to eight, because both families arrived whole and
splitting them across batches would have been worse.

## `tools/find_shape.py`

Batch 87 found three functions by hand-writing a regex over a solved function's
instruction stream with its constants as capture groups. This makes that a tool.
Give it a function you have already matched and it returns every function left
in `asm/` with the identical instruction sequence, plus the constants that
differ, ready to fill into the C you already wrote.

    every immediate      #0x2c, #7        -> a capture group
    every pool value     =0x122c, =gFoo   -> a capture group
    every branch target  bl __Foo, b .L1  -> a capture group
    every local label    .L1782:          -> a capture group

Everything else — mnemonics, register names, **addressing offsets**, the order
and count of instructions — has to match exactly. `[r5, #0x50]` and
`[r5, #0x4c]` are different struct members and therefore different C, so
addressing offsets are deliberately not wildcarded.

**`--clusters` is the mode that matters.** It groups every remaining function by
shape and ranks by payoff, which finds families where *nothing* is solved yet:

|  n | insn | payoff | representative |
|---|---|---|---|
| 18 | 176 | 2992 | `OvlFunc_883_20080c4` |
| 17 | 144 | 2304 | `OvlFunc_883_200834c` |
| 17 | 142 | 2272 | `OvlFunc_883_20088c0` |
| 7 | 233 | 1398 | `OvlFunc_common0_10c` |

77 clusters, **143 functions reachable by solving one member each**. For
comparison `find_twins.py` — which requires byte identity — tops out at a
thirteen-member group, and that group is **seventeen** by shape.

### Two bugs found by validating it against a known pair

Both would have made the tool quietly useless rather than obviously broken:

- the branch-target rule matched `bl\ (\S+)` where the assembly uses a **tab**,
  so it never fired and any function containing a branch failed to match;
- `FUNC_START` was compiled without `re.M`, so `findall` only ever matched at
  offset zero and every file reported one function or none.

Together they made it print `0 function(s) with this shape` for four families
that have members. Caught by taking a pair I knew were siblings and checking
them line by line — every line *did* match, so the failure was in the search,
not in the pattern. Worth remembering as a debugging move: when a search returns
nothing, test the predicate on a case you know should pass.

## Nine functions, one lever between them

Both families screened clean on the first attempt except the seed of the first,
which needed one thing.

The palette-fade driver walks entries 0..0xdf and passes each colour through the
scaler elevated in batch 79. Written `i += 0x10000;` and tested on `i`, gcc
increments in place and the function comes out **one instruction short**:

    rom    add r3, r6, r2 / ... / mov r6, r3 / cmp r3, r2 / bls
    ours   add r6, r2     / ... / cmp r6, r3 / bls

Assigning through a named `n` and testing `n` gives the ROM's three-operand add
and the copy back. **Testing `i` after assigning through `n` is not enough** —
the intermediate has to be what the comparison reads.

The shop-visit family needed nothing at all; its four tail arms are written out
in full and gcc cross-jumps three of them into a shared tail exactly as the ROM
does.

## A `.s` that has been split before has taken the obvious names

`split_asm.py`'s BASENAME WARNING says not to name the `.c` after the `.s`. That
is necessary and not sufficient. A file split in an earlier batch already has
`<base>_b.s` beside it, holding the **generated assembly of an elevated C
file** — and my cut helper hard-codes `_a`/`_b`/`_c`. It overwrote four of them
at once.

The symptom is misleading in a specific way: an `undefined reference` to a
function that plainly exists in `src/`.

    ovl_30_c_c_c_c_a_b.c:(.text+0x32): undefined reference to `OvlFunc_914_2008b24'

Nothing warns. `asmfacts.py --orphans` passes, because the linker script stays
perfectly consistent — it is the *contents* of a piece that were destroyed, not
a reference to it. Recovered with `git restore`; the helper now takes the first
free suffixes after looking at both `asm/` and `src/`, and the rule is written
up beside the basename warning in `elevation.md`.

## Also this batch

`tools/find_shape.py` turned an existing park from two functions into five.
`src/non_matching/ovl_7ec19c/200816c.c` — the sanctum attendants, blocked on a
literal pool the ROM splits in two — has three more members than it knew about.
One of the new ones was wired in to check rather than assumed: it screens OK,
its instruction stream is exact, and `make compare` came back 45,250 bytes out.
Reverted, and the park now records that the blocker belongs to the family, so
whatever fixes one fixes five.

## Where the project stands

3,064 elevated, 2,613 still assembly, 196 parked. The 143 functions reachable
through shape clusters are the clearest remaining seam.
