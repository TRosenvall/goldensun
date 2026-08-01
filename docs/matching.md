# Matching C against the ROM

## Is agbcc the right compiler?

Yes. It is GCC `2.9-arm-000512`, the reconstruction of the Nintendo AGB SDK
compiler, and it reproduces this ROM's code generation once it is invoked
correctly.

The evidence is `Func_b684`, and now `Func_488c`, `Func_11ce0` and `Func_ea54`
alongside it. With the build fixed, agbcc emits **exactly the
ROM's 52 bytes**, the same instruction selection, the same `mov ip, r0`, the
same `ldmia r4!, {r2}`, and the same `pop {r0}; bx r0` interworking epilogue.
A wrong compiler does not land on the size and idioms of a function it has never
seen.

`old_agbcc` and `agbcc_arm` are also installed. `old_agbcc` is what pokeemerald
uses for `libc.o` and `m4a.o` (the SDK library files). It is worth trying via
`--sweep` on any function that resists, but plain `agbcc` is the right default.

## Three things the build was getting wrong

**1. `-mthumb-interwork` was never passed.** `GBA_CPPFLAGS` and `GBA_CFLAGS`
were declared in the Makefile but the `%.o: %.c` rule ignored them and passed a
bare `-O`. Without the flag agbcc emits `pop {..., pc}`, which does not switch
modes on ARMv4T; the ROM uses `pop {r0}; bx r0`. **No function could ever have
matched.**

**2. `-fcall-used-r4` is required.** Golden Sun's compiler treated `r4` as
call-clobbered. Measured across the whole disassembly:

| register | functions using it without ever saving it |
|---|---|
| r4 | **727 of 2202 (33.0%)** |
| r5 | 1 |
| r6 | 0 |
| r7 | 0 |

agbcc defaults to r4 being callee-saved, so its prologue is `push {r4, lr}`
where the ROM has `push {lr}` — a mismatch in about a third of all functions.
Reproduce the measurement by walking `.thumb_func_start` blocks and comparing
the union of `push`/`pop` register lists against the registers actually used.

**3. `-O`, not `-O2`.** Within a single function the ROM re-materialises the
same address three separate times from `ip` rather than computing it once. `-O2`
common-subexpression-eliminates those into one value. This is a working default
derived from one function — re-check with `--sweep` as more are converted.

## What actually makes a function match

Eight functions now build from C. The pattern across them is consistent enough
to be worth writing down, because it turns most near-misses into matches.

**Write idiomatic C first.** Struct member access and raw offset arithmetic
compile to the same instruction, so there is no matching reason to write
`*(void **)((u8 *)e + 0x6C) = hook`. Use the structs in `include/`; add fields
to them as they are established. A function that matches but reads like
decompiler output is only half done.

**Then pin registers, and expect r3.** Every near-miss so far has been register
allocation, and almost always the same one: **agbcc picks r1 where the original
picked r3.** `register int v asm("r3")` fixes it. Func_d8f4 went from 2/5 to a
match on that alone; so did Func_ca44 and Func_ca58.

**When the original reuses a register, assign the same pinned variable twice.**
This is the idiom that unlocked Func_488c and Func_d7e8:

```c
register int v asm("r3");
v = (int)L13240;          /* ldr r3, =L13240   */
e->script = (void *)v;    /* str r3, [r0]      */
v = 0;                    /* mov r3, #0        */
e->scriptCursor = v;      /* strh r3, [r0, #4] */
```

Written as two separate variables agbcc allocates two registers and misses.

**Statement order controls instruction order.** Func_488c needed both literal
loads emitted before the dependent load, which is just a matter of writing the
two assignments in that order. Where reordering is not enough it is the
scheduler, not the model -- see Func_48a0 below.

**Splitting a function out may need a label promoted.** Func_d7e8 references
`.L13240`, a file-local rodata label. Moving the function to C put it in another
object, so the label was promoted with a `.global` alias at the same address --
the original `.L13240` is kept so nothing else has to change. Watch for this
whenever the extracted function touches a `.L` symbol, and remember to move the
`.rodata` line in `stage1.ld` if the section ends up in a different file.

## Converting in bulk

79 functions now build from C, and most of them came from one family. Two things
made that possible.

**Many functions are one-line wrappers.** 104 across the ROM match the shape
`push {lr}` / set a constant / `bl` / `pop {r0}` / `bx r0` -- a dispatch-table
entry that supplies a constant to a shared implementation. They convert
mechanically:

```c
extern void Func_ca60c(void *desc, int variant);
void Func_ca57c(void *desc) { Func_ca60c(desc, 6); }
```

**`pop {r0}; bx r0` means the function returns void.** This is the single most
useful tell in the whole exercise. That epilogue pops the return address into r0,
which would destroy a return value -- so nothing is returned. Declare the callee
`int` and agbcc emits `pop {r1}; bx r1` instead and the match is lost. A function
that really does return something uses r1 for the scratch.

**Convert whole runs into one .c file.** The expensive part is file surgery, not
C. Adjacent convertible functions can share a single `.c`, which collapses a
12-function conversion into one split and one `stage1.ld` line.
`rom_c9000/src/f_1_rom_ca57c.c` holds twelve.

### Two traps when splitting a .s

Both of these produced link errors that took a while to read, so they are worth
knowing before the first attempt.

**A trailing run can delete the `.rodata` that follows it.** The last function in
a file has no next-function boundary, so a naive extent calculation runs to
end-of-file and swallows the `.section .rodata` after it. Cap the extent at the
function's own `.func_end`. This silently removed `.Lc3604` from `rom_c10e8.s`
and produced `undefined reference` at link time.

**A file-local `.L` label needs promoting when its user moves to C.** `Func_d7e8`
references `.L13240`, a rodata label in the same file. Once the function lives in
another object the reference cannot resolve. Add a `.global` alias at the same
address and keep the original label so nothing else changes:

```
	.global	L13240
L13240:
.L13240:
	.incrom 0x13240, 0x13254
```

Also remember to move the `.rodata` line in `stage1.ld` if the section ends up in
a different object than before.

**Snapshot every file a bulk edit will touch, not just the ones you expect.** An
incomplete snapshot cost a file that had to be reconstructed from the ROM bytes
(readable enough -- the BL offset gives the callee, `PC + 4 + offset` with PC at
the instruction address plus four).

## The blocker: agbcc cannot emit register-offset addressing

This is the largest obstacle to the whole effort, and it is worth knowing before
picking a function to convert.

Thumb has a register-offset addressing mode -- `ldr rD, [rB, rI]`, and the same
for `ldrb`/`ldrh`/`str`/`strb`/`strh` -- and the ROM uses it constantly. **This
agbcc build never emits it**, at any optimisation level, with either compiler.
It always materialises the address first:

```c
u8 f1(u8 *p, int i) { return p[i]; }        /* both already in registers */
```
```
        ROM idiom                 agbcc, every flag combination
        ldrb r0, [r0, r1]         add  r0, r0, r1
                                  ldrb r0, [r0]
```

That probe is the fairest possible case -- base and index arrive in argument
registers, nothing to schedule -- and it still misses. `--sweep` confirms it
across `agbcc`, `old_agbcc`, `-O` and `-O2`: every combination lands at 57% on
`Func_793b8`, a five-instruction function, and none reaches the addressing mode.

**Measured over the ROM proper: 818 of 2259 functions (36.2%) contain at least
one such instruction, 4369 instructions in total.** That is a larger share than
the `-fcall-used-r4` problem above, and unlike that one it is not a flag that can
be flipped. Any function that indexes an array is currently unreachable.

### It is a gap in the agbcc reconstruction, not hand-written assembly

That was the open question, and it is now settled. **781 of the 813 functions
that use the addressing mode -- 96.1% -- also carry agbcc's `-mthumb-interwork`
epilogue**, the `pop {rN}; bx rN` pair that only appears when that flag is on.
Hand-written assembly has no reason to adopt a compiler's interworking
convention, and certainly not 781 times.

So the original toolchain emitted `ldr rD, [rB, rI]` and this reconstruction of
it does not. **The fix is a compiler fix**, not a per-function workaround.

**Where it actually lives** (corrected after reading agbcc's sources, which sit in
`../agbcc`): the pattern is *not* missing. `GO_IF_LEGITIMATE_ADDRESS` in
`gcc/thumb.h` accepts REG+REG, but only `&& reload_completed` -- it is disabled
before reload because Thumb has 3 guaranteed reload registers where 4 are needed.
That is an upstream GCC 2.9 limitation with an explicit `???` comment, not a pret
omission. The design defers to `reload_cse_regs` / `reload_combine` to re-form the
addresses afterwards, and that is not happening. No optimisation flag reaches it,
`-fomit-frame-pointer` included.

See [agbcc-bug-correction.md](agbcc-bug-correction.md).

The clincher is that **`agbcc_arm`, built from the same tree, emits the ARM
equivalent without difficulty** -- `ldrb r0, [r0, r1]` from the same one-line
reproducer. It is the Thumb machine description that is missing the pattern, not
the compiler as a whole.

A full bug report, with the reproducer and the ROM statistics, is written up in
[agbcc-thumb-regoffset-bug.md](agbcc-thumb-regoffset-bug.md) ready to file
against pret/agbcc.

Note on which binary: `old_agbcc` is not an older *release* of agbcc -- it is a
second binary built from the same source tree with a different configuration.
Both are affected, so there is no version of the install to fall back on.

Until then, **pick candidates by grepping the mode out.** A function with no
`[rX, rY]` operand is worth attempting; one with several is currently
unreachable. That filter is what produced every match so far:

```sh
# clean candidates: small, no calls out, no register-offset addressing
grep -L '\[r[0-9]*, r[0-9]*\]' rom_*/src/*.s
```

### A third obstacle: constant materialisation

Five functions of the shape `REG_X = constant` cannot be matched, and the reason
is neither registers nor scheduling. The ROM loads the constant from the
literal pool where agbcc emits a `mov` immediate:

```
        ROM                             agbcc
        ldr  r2, =REG_BLDCNT            ldr  r1, .L3
        ldr  r3, .Lc0eb0  @ 0xbf        mov  r0, #0xbf
        strh r3, [r2]                   strh r0, [r1]
        bx   lr                         bx   lr
        .Lc0eb0: .word 0xbf
```

`0xBF` fits in a Thumb `mov` immediate, so agbcc uses one. Measured threshold:

| value | agbcc emits |
|---|---|
| 0x7F, 0xBF, 0xFF | `mov rN, #value` |
| 0x100 | `mov rN, #0x80` (plus a shift) |
| 0x1BF and above | `ldr rN, .LN+4` -- pool |

So the pool only appears above 255, and no optimisation level or flag changes it
(`-O0`, `-O`, `-O2`, `-O3`, `-Os`, and several others all behave the same). The
original compiler pooled a value that this one materialises inline, which is a
COST MODEL difference rather than a missing feature.

This is a much smaller problem than the addressing mode -- it affects five
functions rather than 818 -- and unlike that one it has no obvious upstream fix
to ask for. It is recorded here so the shape is recognised rather than
re-attempted: **a function whose only oddity is a pooled small constant is not
worth chasing.**

### A second, smaller obstacle: instruction scheduling

`Func_48a0` compiles to the ROM's exact five instructions in a different order:

```
        ROM                       agbcc
        ldr r3, =iwram_1e50       ldr r3, .L3
        mov r0, #0x81             ldr r3, [r3]
        ldr r3, [r3]              mov r0, #0x81
        lsl r0, #18               lsl r0, #18
        sub r0, r3                sub r0, r0, r3
```

The original interleaved the independent constant materialisation between the
two dependent r3 operations; agbcc keeps `mov`/`lsl` adjacent. Statement order
and register pins do not move it -- this is the scheduler, below the level C can
address. Unlike the addressing mode this affects only a handful of functions, but
it is worth recognising so it is not mistaken for a modelling error.

## asmdiff masks relocations, and that can hide a mismatch

`Func_488c` was reported as `*** EXACT MATCH ***` while three bytes differed.
The differing instructions were relocation sites -- `ldr r3, =iwram_1e50` and the
load through it -- which asmdiff blanks out because the linker has not yet filled
them in. The register *inside* a masked instruction is masked with it.

The tool says so ("match is modulo relocations; confirm with a full `make
compare`"), and that warning should be taken literally: for any function whose
body is mostly symbol loads, asmdiff can only tell you the sizes and the
scheduling agree. **A green asmdiff on such a function means "try it", not
"done".** The three register mismatches only surfaced from a byte diff of the
built ROM against `baserom.gba`, which is the check worth running:

```sh
gmake clean && gmake compare-rom
python3 -c "
a=open('baserom.gba','rb').read(); b=open('goldensun.gba','rb').read()
d=[i for i in range(min(len(a),len(b))) if a[i]!=b[i]]
print(len(d),'bytes differ, first at', hex(d[0]) if d else '-')"
```

## Two Makefile footguns

- **A failed C compile used to leave a valid-looking object behind.** The
  `%.o: %.c` pipeline runs `as` on whatever agbcc emitted before erroring, so a
  broken source produced a small but linkable `.o`. `.DELETE_ON_ERROR:` now
  removes it.
- **If `foo.s` and `foo.c` both exist, the `.s` rule silently wins.** The `.s`
  pattern rule appears first in the Makefile. Delete the `.s` to switch a
  function over to C — and expect the checksum to fail until it matches.

A related trap that is *not* a Makefile bug: an `.o` newer than its `.c` is
never rebuilt. For a long time `gmake compare` was passing while linking
assembly-derived objects, so the green checksum said nothing about the C. When
in doubt, `gmake clean` before trusting a match.

## Using tools/asmdiff.py

Reads `GBA_CPPFLAGS` / `GBA_CFLAGS` straight out of the Makefile, so the differ
can never disagree with the build.

```sh
# compare against the ROM directly (offset = the address in the function name)
tools/asmdiff.py Func_b684 rom_9000/src/f9_4_rom_b684.c \
    --rom-offset 0xb684 --rom-size 52

# or against an existing disassembly (assembled first, so it compares machine
# code rather than source text)
tools/asmdiff.py Func_b684 rom_9000/src/f9_4_rom_b684.c \
    --ref rom_9000/src/f9_3_rom_b388.s

# try every compiler and flag combination, ranked by how close each lands
tools/asmdiff.py Func_b684 rom_9000/src/f9_4_rom_b684.c \
    --rom-offset 0xb684 --rom-size 52 --sweep

# override flags for one run
tools/asmdiff.py Func_b684 src.c --rom-offset 0xb684 --rom-size 52 \
    --cflags="-mthumb-interwork -O -fhex-asm -fcall-used-r4"
```

Exit status is 0 only on an exact match, so it can gate a script.

Output is side by side, ROM on the left. Branch targets are shown as signed
instruction deltas (`->+12`) rather than addresses, so inserting one instruction
does not make every later branch look different.

## Worked example: Func_b684, assembly -> matching C

The first function fully elevated. `rom_9000/src/f9_4_rom_b684.s` is gone; the
`.c` is the only source and the ROM still matches. The path there, in order:

| step | aligned |
|---|---|
| starting point (`u8` param, dereferenced table pointer, no interwork) | 48 bytes, wrong size |
| `-mthumb-interwork` | 52 bytes, correct epilogue |
| `int` param, take address instead of dereferencing | 10/28 |
| split the loop counter into its own variable | 14/28 |
| `-fcall-used-r4` | correct prologue, `push {lr}` |
| reuse the count variable as the loop temp | 18/26 |
| pin `obj`/`i`/`n`/`e`, materialise the two addresses by hand | **26/26 exact** |

The lesson worth carrying: **structure first, pins last.** Splitting the counter
and reusing the temp were source-level changes that each moved several
instructions. The `register asm()` pins only settled the final naming once the
instruction sequence was already right. Reaching for pins early hides the real
mismatch.

## Reading a diff

The remaining differences on `Func_b684` are all register allocation — the
instruction sequence is right. Typical causes, in the order worth trying:

- **A value copied into a high register you did not ask for** usually means
  something else clobbered the register it was in. Look for an address
  computation that got hoisted above the loop.
- **A missing or extra `mov rN, ip`** means the compiler kept a value live where
  the ROM re-derived it (or the reverse). Restructuring the source — early
  return vs. nested `if`, one loop variable vs. two — moves this more reliably
  than flags do.
- **Wholesale register renaming** with identical structure is the last mile.
  `register X asm("rN")` pins a variable; it is a legitimate matching tool, not
  a hack.

For `Func_b684` specifically, splitting the loop counter into its own variable
(`n` loaded, `i` decremented) took it from 38% to 46% aligned, and
`-fcall-used-r4` fixed the prologue exactly.

## Functions that call other functions

An unlinked object holds 0 where a call target will go; the ROM holds the
resolved offset. Comparing those bytes directly reports a difference at every
call site even when the code is correct.

`asmdiff.py` reads the object's relocation table (`objdump -r`) and blanks four
bytes at each relocated offset on both sides before comparing. Verified on
`Func_b168`, whose known-good assembly carries four `R_ARM_THM_CALL`
relocations: **11 differing bytes without masking, 0 with**.

The report names the masked symbols, so you can see what was excused:

```
4 relocation site(s) masked: Func_3d28, Func_3dec, Func_aa0c
26/26 instructions aligned   *** EXACT MATCH ***
match is modulo relocations; confirm with a full `make compare`
```

An exact match here means "identical apart from operands the linker fills in".
That is the correct standard for an unlinked object, but confirm it for real by
deleting the `.s` and running `make compare`. Use `--no-reloc` to compare raw
bytes.

Offsets come from the candidate object, so if the instruction layout has
diverged they will not line up — in that case the diff already shows the real
problem and the masking is irrelevant.

## Using decomp-permuter

Once the instruction sequence is right but the registers are wrong, the
remaining search is mechanical. decomp-permuter automates it: it mutates the C
(reordering temporaries, swapping equivalent idioms, changing types) and scores
each variant against the target object.

```sh
tools/permute_setup.sh Func_b074 rom_9000/src/f9_1_rom_b074.c \
                                 rom_9000/src/f9_1_rom_b074.s

python3 decomp-permuter/permuter.py nonmatching/Func_b074
```

`permute_setup.sh` writes `nonmatching/<Func>/` containing `compile.sh` (the
project's own pipeline, with flags read from the Makefile so it cannot compile
differently from the build), `base.c` (preprocessed, other functions stripped)
and `target.o`.

`target.o` is built from the reference `.s`, not from raw ROM bytes, on purpose:
GAS marks `.incbin` content as data, so an object built that way will not
disassemble as Thumb and the permuter cannot score against it.

Lower scores are better; 0 is a match. Dependencies: `pycparser`, `attrs`,
`toml`, `Levenshtein`, `pynacl`.

## Upstream tools worth adding

`tools/asmdiff.py` covers the inner loop. Two established tools go further and
are worth pulling in when the easy functions are done:

- **asm-differ** (`simonlindholm/asm-differ`) — richer diffing, live re-run on
  file change. `asmdiff.py` covers the same ground for this project and reads
  the Makefile's flags, so this is a convenience rather than a gap.

decomp-permuter is already set up; see the section above.

## Data layouts

`include/` now carries the structures the annotations established. They are
worth reading before any further conversion work, because most remaining
functions are mostly field access.

| header | covers |
|---|---|
| `entity.h` | the 0x70-byte overworld entity |
| `scene.h` | the scene/dialogue block behind `iwram_1ebc` |
| `map.h` | map state, the cell array, map-object and region records |
| `save.h` | party and save state in `ewram_240`, and the save-bit idioms |
| `combatant.h` | the 0x14C-byte combatant record |
| `m4a.h` | the sound driver's track and tone layouts |

Two conventions differ and mixing them up produces code that assembles and then
reads garbage:

- `iwram_1ebc` and `iwram_1e70` are **pointers** to their blocks. Every access
  loads the pointer first, then adds an offset.
- `ewram_240` **is** the block. Offsets apply directly, with no load.

`scene.h` and `map.h` deliberately define offset constants rather than structs.
Their total sizes are not established -- the scene block is read as far as
+0x236 with nothing bounding it, and the map state is described as 0x194 bytes
in one place while the world-map path reads +0x338 -- so declaring a struct
would mean inventing both a size and every hole between the known fields.

Where a struct *is* declared it carries a `sizeof` assertion, and
`make check-layouts` compiles them. Nothing links that file; it exists so a
wrong offset fails the build instead of silently producing wrong code. Confirm
the check can fail before trusting it -- break an assertion deliberately and
watch agbcc reject it.

### Unresolved conflicts, recorded rather than guessed

- `Entity +0x30/+0x34` are read as max speed / acceleration by the movement
  code and as a scale pair by the draw loop. Named for movement because far
  more call sites use it that way.
- `Entity +0x40` holds a move target, but the draw loop reads a priority byte
  at `+0x42`, which overlaps it. One reading is wrong.
- `Entity +0x0A/+0x12` are not fields at all -- they alias the high halfwords
  of the 16.16 x and z, and the ROM reads them both ways.
