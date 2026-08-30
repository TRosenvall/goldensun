# Batch 151 — the compiler is not deterministic, and a flag group was wrong

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare`.
**That build failed**, and the failure is the most important thing in this
batch — see "The compiler is ASLR-dependent" below. After rebuilding the one
divergent object, `make compare` → `goldensun.gba: OK`, SHA1
`5c4695205413df7db52b9a184815a07783999971`, with `git status` clean across
every tracked generated `.s` — which is the proof that no other object in the
tree diverged. Every address below was read back out of the linked ELF with
`arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `OvlFunc_958_2008f44` | `02008f44` | ovl_7e636c | [ovl_cc0_c_a_c_a_c_c_a_b.c](../src/overlays/rom_7e636c/ovl_cc0_c_a_c_a_c_c_a_b.c) |
| `Func_8099d18` | `08099d18` | main ROM | [rom_97b54_c_c_b.c](../src/rom_8a000/rom_97b54_c_c_b.c) |
| `Anim_Func` | `080d6660` | main ROM | [rom_d6504_a_c_c_b.c](../src/rom_c9000/rom_d6504_a_c_c_b.c) |
| `Func_801b398` | `0801b398` | main ROM | [rom_1aeec_a_a_c_a_c_b.c](../src/rom_15000/rom_1aeec_a_a_c_a_c_b.c) |
| `OvlFunc_common2_380` | `020094d8` / `0200db6c` | common2, in ovl_7bf5a8 **and** ovl_7e7574 | [common2_c_c_c_c_a_b.c](../src/overlays/common/common2_c_c_c_c_a_b.c) |

## The compiler is ASLR-dependent, and this has been corrupting clean builds

The clean build for this report failed with two bytes wrong in the whole ROM,
inside `Func_8005b64`. `git status` named the object immediately, because the
generated `.s` files are tracked:

    rom    ldr r1, .L23+4
    ours   mov r1, r3

That reads exactly like a fake match — a `.c` committed alongside a `.s` it
does not reproduce, hidden by an incremental build. It is not. **Recompiling
the same file, with the same command, produced the committed output.**

Measured, in one container, same input, same flags:

| condition | runs | distinct outputs |
|---|---|---|
| as-is | 120 | 114 correct, **6 divergent** |
| `setarch -R` (ASLR off) | 120 | 120 correct, 0 divergent |

**gcc-2.96's optimiser depends on the process's address layout.** The divergence
is a CSE decision — whether it reuses a register that happens to already hold a
pooled constant — which is precisely the kind of choice a hash table keyed on
pointer values would make differently under a different heap layout. ~5% per
compile of this file.

Consequences, all of which change how this loop should be run:

1. **A `make compare` failure is not automatically a decompilation error.**
   Before investigating a two-byte diff, rebuild the object and re-diff. I
   spent most of this round treating a compiler flake as a source bug, probing
   four optimiser flags and three source rewrites against a file that was
   already correct.
2. **`git status` after a build is a free integrity check.** Every generated
   `.s` is tracked, so a divergent compile shows up as a modified file naming
   the exact object. Run it after every build; it costs nothing and it is the
   only cheap way to catch this.
3. **Determinism is available but needs privileges.** `setarch -R` fixes it
   completely, and requires `docker run --privileged` (unprivileged, setarch
   fails with `Operation not permitted`). Worth adding to the build invocation
   for anyone doing a release-grade verification; not wired into the Makefile
   here because it would make the standard command require `--privileged`.
4. **Past "fake match" conclusions deserve re-testing.** Any function parked or
   reverted on the strength of a single clean-build diff may have been this.

## `common2` was built without `-fcall-used-r4`

[docs/elevation.md](../docs/elevation.md) recorded that `push {r4` is a one-grep
test that a TU was not built with `-fcall-used-r4`, and asked for a sweep across
the parked set on the theory that it might explain the register-allocation
class, "the largest blocked class in the corpus".

**Ran it. The answer is no.** 16 functions in the remaining corpus push r4, and
**zero of them are parked**. Every coin-flip park is a function whose ROM leaves
r4 alone, so the flag cannot be what is wrong with any of them. Recorded in the
doc in place so it is not run a third time.

The sweep paid off elsewhere. The 16 cluster hard: eight are `common2`, the rest
m4a/sound. `common2` was already known to be a non-interwork TU; it was also
built without `-fcall-used-r4`, so its eight r4-pushing functions were
unreachable from C **by construction**. `COMMON2_CFLAGS` now substitutes
`-fcall-saved-r4`.

That flip was verified rather than assumed, and the verification is the part
worth copying: **all nine existing `common2_c*.c` compile to byte-identical `.s`
under both flags.** The flip is a no-op for the matched corpus and only opens
the r4 functions. Compiling every file a rule already covers under both settings
and diffing costs seconds, and it converts a flag-group assumption — the weakest
category of claim in this Makefile — into a fact.

## `OvlFunc_common2_380`: two source shapes

With the flag right, the function needed two things, both generalisable.

**Contiguous stack locals passed to the same callee may be one struct.** Naming
pointers to both stack objects gets everything right except a single store:

    rom    mov r3, sp / str r0, [r3, #0] / str r1, [r3, #4]
    ours   mov r3, sp / str r0, [sp, #0] / str r1, [r3, #4]

gcc folds the **offset-0** store back to `sp` while keeping the pointer for the
offset-4 store. No amount of pointer naming fixes it, because at offset 0 the
pointer and `sp` are the same value and the sp-relative encoding is no longer.
The fix is to stop having two locals: the 8-byte input block and the 20-byte
output record are contiguous and exactly fill the frame, so they are one struct,
and `&s.in` is then a genuine subobject address. This narrows the stack-vector
pointer rule in the doc, which previously said naming the pointer was the lever.
**Tell: `sub sp, #N` equals the sum of the two objects with no padding.**

**A `goto` for block order.** The ROM branches `ble` to the scaling block and
falls through to the saturating one; written structurally the two come out
swapped. The scaling block is ten instructions, well clear of the
two-instruction duplication boundary the doc warns about.

A by-value 8-byte struct parameter was also tried, on the theory that paired
r0/r1 stores are a spill signature. Wrong here: 50 of 52 differing.

## `OvlFunc_common2_41c`: a clean specimen of the coin flip

The existing park said "the C shape is probably wrong, not the flags", at 22 of
28 against a 27-line ROM, and told the next reader to test the r4 hypothesis on
`common2_380` instead of there. Both halves were right, in opposite directions.

The function is a software 64-bit logical right shift. It now sits at **27 lines
against 27, same block order, same branch senses, one instruction shape for
one.** What got it there:

- A union of `unsigned long long` and two u32 halves, **written back after the
  join**, not inside either arm. Inside the arms, gcc threads each arm straight
  to the epilogue and the join disappears (24 lines). Building the result
  arithmetically as `((u64)hi << 32) | lo` costs five instructions — two
  `mov #0` and two `orr` — where the ROM moves the pair.
- A `goto` putting the `count >= 32` arm first.

What is left is 18 lines and **every one is a register rename**: the ROM keeps
the count in r6 and the pair in r4/r5, spending three pushes to do it, while
gcc, in a leaf function with no calls, correctly prefers the low scratch
registers. That is the documented coin flip, and it is a cleaner specimen than
any of the four existing parks because nothing else differs at all — no length
difference, no ordering difference, nothing to argue about.

Also recorded: `v >> n` for a variable `n` on a 64-bit value emits a call to
`__lshrdi3`. Any ROM function doing the shift inline is doing the word
arithmetic by hand, and the source must too.

## Housekeeping

`src/non_matching/ovl_common/common2_380.c` deleted — the function is elevated
now, and a park left behind after a match is worse than no park. Worth a general
check: the park corpus is not currently audited against the matched corpus, and
this one survived because the elevation came from a doc lead rather than from
`tools/pickable.py`, which is where the park filter lives.
