# Batch 74 — a new flag group, and a size symbol that measures more than a function

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, and every
overlay `cmp` clean. Every address below was read back out of the linked ELF.
0 orphaned linker references.

| Function | Address | File |
|---|---|---|
| `Func_8017a64` | `08017a64` | [rom_178b0_a.c](../src/rom_15000/rom_178b0_a.c) |
| `OvlFunc_944_2008030` | `02008030` | [ovl_30_a_a_a.c](../src/overlays/rom_7ca63c/ovl_30_a_a_a.c) |
| `Func_80f037c` | `080f037c` | [rom_f0254_a_b.c](../src/rom_f0000/rom_f0254_a_b.c) |
| `Func_80c90e4` | `080c90e4` | [rom_c9048_a_b.c](../src/rom_c9000/rom_c9048_a_b.c) |
| `Func_800d304` | `0800d304` | [rom_ca6c_c_b.c](../src/rom_9000/rom_ca6c_c_b.c) |

## The blob is bigger than the function

`Func_800d304` copies an ARM routine into RAM and runs it. Its first instruction
is `ldr r5, =0x4e8` where gcc synthesises `0x9d << 3` in two — the pool tell, so
the operand was a **symbol**. The convention was already in the tree:
`rom_52f4.c` uses `_DECOMPRESS_LZ_SIZE`, emitted by a `.func_end_emit_size`
macro in the assembly.

It was parked last round anyway, because applying that macro to `Func_800a494`
gives **0x4c4**, not 0x4e8, and would have produced a build that does not match.

**The thirty-six missing bytes are veneers and a pool.** `Func_800a494` does
`bl Func_800a958`, and `Func_800a958`/`a960`/`a968` are 8-byte
`ldr r4, =X / bx r4` thunks sitting immediately after it, followed by the
12-byte pool those three read from:

```
0x4c4  body
0x018  three veneers it calls through
0x00c  their pool
-----
0x4e8  exactly what the function copies
```

So the blob is the routine **plus everything it needs to run from RAM**, and the
size has to be measured from the end of that pool rather than from the
function's own `.func_end`. The ELF confirms `_FUNC_800A494_SIZE = 0x4e8`.

### That opened a family

Searching for the same wrapper shape — `bl Func_8004938` followed by a DMA3
`stmia` — found three more functions with a pooled small size, every one a tell.
Two more symbols are now defined with the same macro: `_FUNC_8001DC8_SIZE`
(0xe0) and `_FUNC_800A37C_SIZE` (0x9c), both exactly the function extent with no
veneers to account for. The macro was already used three times in one of those
very files, for the `_BLITFADE_*_SIZE` symbols, so nothing had to be invented.

`Func_8003e10` and `UnpackTilemap` still park, but on smaller, separate
questions — their size symbols are settled and in the build.

All three edits emit no bytes: absolute symbol definitions, the same class as
the `.global` lines `HANDOFF.md` records.

## `-fno-gcse` — a new flag group, and its bounds

`Func_80f037c` fills a buffer in four runs. At `-O2` gcc **sinks** the step
constant's pool load past the loop label, so it is re-loaded on all 240
iterations:

```
rom    ldr r4, =0x20002 / mov r3, #0xef / L1: sub r3, #1 / ...
ours   mov r3, #0xef / L1: ldr r4, =0x20002 / sub r3, #1 / ...
```

Three source placements are byte-identical, and of five flags probed only
`-fno-gcse` moves it. `GCSE_CFLAGS` is now a group alongside `O1_CFLAGS`,
`CSE_CFLAGS`, `COMMON2_CFLAGS` and `ALIAS_CFLAGS`.

**Swept before adopting**, same discipline as the alias flag: `-fno-gcse`
improves **six** parked functions — one from 116 differing lines to 19, two from
18 to 6 — and matches **none** outright. So it is a real mechanism with more to
give, and it is not a general key.

**And it does not generalise to a shared address.** `UnpackTilemap` passes
`gBuffer` to two calls; the ROM loads the address twice, gcc loads it once into
r10 and pays for a high-register save. `-fno-gcse` does *not* stop that. Worth
recording next to the win, because the two symptoms look alike.

## Two levers doing second jobs

**An offset in the TYPE lets gcc decide per access.** `Func_80c90e4` reads four
fields, and the ROM derives exactly **one** offset from another — `add r1, #4`
gets 0x7794 from the 0x7790 it is holding — while the other two are loaded
fresh. Struct members reproduce that unaided.

That is worth reading next to `Func_80173ac` in batch 73, where struct members
**stopped** a derivation chain. The lever is not "members prevent derivation";
it is that members let gcc decide per access, and gcc's decision matched the ROM
in both cases. Writing offsets as arithmetic is what forces a single chain.

**Guard inversion, stated as a rule of thumb.** `Func_8017a64` was 12 of 35 on
block order alone: written as the obvious `else if` chain gcc keeps the cheap arm
inline and branches away to the expensive one; the ROM does the opposite.
Whichever arm the ROM lets **fall through** is the arm the source's `if` was
true for.

## `OvlFunc_944_2008030` joins the alias group

An `int` store followed by a pointer load out of the same object, and strict
aliasing lets the scheduler hoist the load above the store — the same mechanism
as the seven TUs in batches 69–70.

Worth noting what did **not** work: reading the pointer earlier in the source,
the lever that fixed `OvlFunc_957_200b610` in batch 71, gives 9 differing lines
here rather than 2. **Statement order moves a register birth; it does not stop a
scheduler that has been told the two accesses cannot conflict.**

## I skipped my own tool and paid for it

`split_asm.py` checks three things before a cut. I cut `Func_8017a64` without
running it and hit two of them in succession — a `.rodata` section stranded in
the deleted `.s`, then a `.L` label crossing the new boundary. Both fixed, but
that is precisely the failure the tool was written for after batch 61, and
running it costs one command.

## Parks

| Function | Blocker |
|---|---|
| `Func_80c0e38` / `Func_80c0e70` | literal pool PLACEMENT — 18 of 19, and the one missing instruction is a `b` that exists only to jump over the ROM's mid-function pool |
| `Func_800d304`'s siblings — `Func_8003e10` | setup-instruction placement, 2 of 26 |
| `UnpackTilemap` | a symbol address CSEd across two call sites |
| `Func_80e3908` | gcc one instruction AHEAD — the ROM copies a pointer out of r0 to free r0, and nothing in C asks for that |
| `Func_80b0a20` | the width of a pooled zero |
| `Func_809b0dc` | the same, at 1 of 29 |

The fade pair is the cleanest example of pool placement in the corpus: seven
setup instructions exact, the whole body exact, one branch that is pure
placement artifact. Getting there needed three things worth keeping — the
`0x2044` through an `int` local (stored straight to a `vu16` gcc pools it as a
halfword), both register addresses in pointer locals born in the ROM's order,
and the step operands the right way round.

`Func_80b0a20` and `Func_809b0dc` share a symptom that looks like the pool tell
and is not: `ldr rN, =0x0` where `mov rN, #0` would do. **gcc pools the zero
too**, as a halfword; only the width differs. Substituting a word-width symbol
does force the ROM's `ldr` — the mechanism is real — but makes both functions
worse, so it is recorded as a width tell rather than a naming lead.
