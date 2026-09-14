# Batch 263 -- five save-byte accessors, solo, in one pass

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. All five addresses checked against
the linked ELF — and all five are **named symbols**, the case `checkaddr.py`
exists for.

| | |
|---|---|
| elevated | **5** |
| whole-file conversions | 1 (five functions) |
| splits | 0 |
| build-input changes | 0 |
| fakematch debt added | 5 functions |

Solo round, no agents. This is the follow-on batch 262 pointed at: the same family
as `GetFlag`/`SetFlag`/`ClearFlag`, parked on the same `combine_regs` blocker.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `GetFlagByte` | `0x080793b8` | [rom_79338_c_a.c](src/rom_77000/rom_79338_c_a.c) |
| 2 | `SetFlagByte` | `0x080793c8` | [rom_79338_c_a.c](src/rom_77000/rom_79338_c_a.c) |
| 3 | `IncFlagByte` | `0x080793d8` | [rom_79338_c_a.c](src/rom_77000/rom_79338_c_a.c) |
| 4 | `DecFlagByte` | `0x080793f8` | [rom_79338_c_a.c](src/rom_77000/rom_79338_c_a.c) |
| 5 | `GetFlagNybble` | `0x08079418` | [rom_79338_c_a.c](src/rom_77000/rom_79338_c_a.c) |

Whole object: 124 bytes, 57 encodings, 5 relocations identical, measured 3×.

## The transfer paid immediately

The index-extract idiom from `rom_79338_a.c` — barrier after the shift to kill the
first `combine_regs` tie, pinned index to kill the second — was transferred
verbatim. **`GetFlagByte` and `SetFlagByte` were exact on the first screen with
nothing but that transfer.** The mechanism is recorded in `rom_79338_a.c` and is
not repeated here.

## Two results, both about NOT reaching for a variable

**A copy in the ROM can be a CSE artifact, not a source variable.**
`IncFlagByte`'s ROM loads into r2, copies to r3, compares the copy, then adds from
r2. That reads unmistakably like two named locals. It is not — every two-name
spelling has the copy coalesced away. Reading the location **three times and naming
nothing** is exact:

```c
if (p[i] <= 0xfe)
    p[i] = p[i] + 1;
return p[i];
```

| spelling | differing |
|---|---|
| **no locals at all** | **EXACT** |
| `unsigned char v` | 7 |
| `unsigned int v, w` | 7 |
| `v` plus a named result | 13, and two instructions long |
| early-return form | 10 |
| assigning back through the index | 7 |

`DecFlagByte` is the same shape and closed on the same spelling with no further
work.

> A redundant-looking `mov` between a load and its use is evidence of ONE source
> expression read more than once, not of two variables. Try naming **less** before
> naming more.

The comparison must also be unsigned — the ROM's `bhi` against a signed `bgt` is
one of the seven.

**One pin load-bearing and a second pin harmful, in the same function.**
`GetFlagNybble` needed its base pointer pinned: unpinned, gcc hoists the pool load
ahead of the index computation and puts it in the wrong register (8 differing);
pinned, 2. The remaining two were the first two instructions swapped — a cheap
`mov r1, #4` hoisted above the `lsl`.

| | differing |
|---|---|
| base pointer pinned + barrier after the shift | **EXACT** |
| …plus the shift variable pinned | **10 — worse than doing nothing** |

The lever for the second defect was barrier **placement**: moving the existing
barrier to sit *between* the shift and the mask constant.

> The recorded "try the bare pin first" rule is about reaching for a pin before a
> barrier. It does not mean a second pin is the next thing to try once one pin is
> already in place — and barrier placement is a separate lever from presence.

## Method note

`objcmp --func` filters only the **reference** and compiles the whole candidate, so
the first screen of a five-function file returned a relocation dump rather than a
verdict. Splitting into five single-function candidates is what made the round
readable. This is the trap already recorded in `docs/elevation.md`; it cost one
command here rather than a round, because the failure was loud.

The round then went: read the generated `.s` against the ROM side by side, and
sweep spellings. Guessing from encoding indices moved nothing on two attempts;
reading the two listings found both causes immediately.

## State

| | |
|---|---|
| matched | **4,328** (76.5% of the 5,655 elevatable) |
| remaining, hand-written thumb | 1,327 |
| &nbsp;&nbsp;parked | 442 |
| &nbsp;&nbsp;UNATTEMPTED | 885 |

Pool went **1,332 → 1,327**, exactly -5.
