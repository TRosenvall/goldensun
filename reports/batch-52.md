# Batch 52 — seven functions, and the worklist emptied

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`
and sits at exactly the address its name claims.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_884_2008634` | `02008634` | ovl_784360 | [ovl_30_c_a_a_a_c_c_a_c_c_b.c](../src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_c_b.c) |
| `OvlFunc_933_2008498` | `02008498` | ovl_7bc690 | [ovl_314_c_c_a_b.c](../src/overlays/rom_7bc690/ovl_314_c_c_a_b.c) |
| `OvlFunc_948_2009070` | `02009070` | ovl_7d30e0 | [ovl_30_c_a_c_c_a_a_c_a_a_b.c](../src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_c_a_a_b.c) |
| `OvlFunc_949_20085dc` | `020085dc` | ovl_7d4af4 | [ovl_30_c_c_a_c_c_c_c_c_c_b.c](../src/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_b.c) |
| `OvlFunc_952_200bf84` | `0200bf84` | ovl_7d768c | [ovl_30_c_a_c_a_b.c](../src/overlays/rom_7d768c/ovl_30_c_a_c_a_b.c) |
| `OvlFunc_955_200805c` | `0200805c` | ovl_7ddb88 | [ovl_30_c_c_a_c_a.c](../src/overlays/rom_7ddb88/ovl_30_c_c_a_c_a.c) |
| `OvlFunc_967_20084b0` | `020084b0` | ovl_7f21b8 | [ovl_30_c_c_c_c_b.c](../src/overlays/rom_7f21b8/ovl_30_c_c_c_c_b.c) |

**Seven for seven.** The 19-function constant-CSE worklist from batch 51 is now
**exhausted**: 18 elevated across two batches, one already parked.

## Two false positives, both predicted before screening

Five of the seven needed `-fno-rerun-cse-after-loop`. **Two did not**, and they
are the more interesting result.

| | Shape | Outcome |
|---|---|---|
| `OvlFunc_967_20084b0` | `0x9a7` loaded for two `__GetFlag` calls | on opposite arms of an area test — matches at plain `-O2` |
| `OvlFunc_955_200805c` | `0x335` loaded twice | `__SetFlag` in one arm, `__ClearFlag` in the other — matches at plain `-O2` |

In both, the two uses **can never be live at the same time**, so gcc has nothing
to hoist. The flag changes nothing — `955_200805c` is 6 of 36 either way — and no
Makefile rule was added for either.

This is exactly the caveat `tools/pick_candidates.py` has carried since batch 26,
argued from the mechanism. It is now confirmed twice on live functions, and the
note says what to do about it:

> **The search finds a shape. Whether the two uses can be live at once is a
> separate question the reader still has to answer.**

**Eighteen rules would have been twenty without that check** — which matters
precisely because the standing item now questions whether any of them belong.

## `955_200805c` inverts batch 49's advice

Its real defect was the stack-arg pair, and the fix runs the **opposite** way to
what batch 49 established.

Batch 49 said to hoist a shared stack-arg value to a dominating block. That is
right when the ROM builds it **once**. Here both arms pass the same pair
`(0x23, 0x4d)` and the ROM builds it **fresh in each arm**, so the locals have to
be assigned inside each arm:

| Spelling | Result |
|---|---|
| assigned in each arm | 36 lines, **exact** |
| hoisted above the `if` | 35 lines, 26 differ — gcc materialises once and carries it across the branch |
| bare literals | 36 lines, 6 differ — one register reused for both slots instead of two |

The duplication looks redundant in the C and is what the ROM says. Added to
`docs/elevation.md` under the stack-arg-pair lever, with the rule stated as:
**count the materialisations in the reference before deciding where the
assignment goes.**

## Fourth instance of the asymmetric-arms pattern

`OvlFunc_952_200bf84` duplicates its `__ActorMessage` call in **both** arms rather
than joining after the `if`. The ROM emits it twice, once per arm.

That is now the fourth time in this tree that the fix was to make two arms of one
construct *less* symmetric than they read — after the join-store in batch 45, the
`__GetFlag`-and-discard in batch 47, and the pointer-build order in batch 45. It
has become frequent enough to be worth checking by default rather than
discovering each time.

## Housekeeping

- Three `.global` lines added to `ovl_7f21b8/ovl_30_c_c_c_c.s` for the script
  tables `967_20084b0` returns — **eleventh through thirteenth** in this tree, all
  verified byte-neutral before the split.
- `_AREA_b4` added to `area.sym` by value, placed in the **sorted block** rather
  than appended to the tail where later additions have been accumulating.
- `-fno-rerun-cse-after-loop` now covers **twenty rules / twenty-seven
  functions**. See the standing item in `HANDOFF.md`; the count is the argument.
