# Batch 65 — the pool tell was never a naming decision

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.
3056 sources checked, every elevated `.c` has a tracked `.s`; 0 orphans.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_926_200a574` | `0200a574` | ovl_7b2078 | [ovl_314_c_c_a_c_c_c_c_c_a.c](../src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_c_a.c) |
| `OvlFunc_932_20081c8` | `020081c8` | ovl_7b9cb4 | [ovl_30_a_c_a_c.c](../src/overlays/rom_7b9cb4/ovl_30_a_c_a_c.c) |
| `OvlFunc_934_2008d80` | `02008d80` | ovl_7bdeb0 | [ovl_d20_c_c_a_a.c](../src/overlays/rom_7bdeb0/ovl_d20_c_c_a_a.c) |
| `OvlFunc_939_2008314` | `02008314` | ovl_7c460c | [ovl_314_a_a.c](../src/overlays/rom_7c460c/ovl_314_a_a.c) |
| `OvlFunc_939_2008350` | `02008350` | ovl_7c460c | [ovl_314_a_c_a_a_a.c](../src/overlays/rom_7c460c/ovl_314_a_c_a_a_a.c) |
| `OvlFunc_939_2008388` | `02008388` | ovl_7c460c | [ovl_314_a_c_a_a_a.c](../src/overlays/rom_7c460c/ovl_314_a_c_a_a_a.c) |
| `OvlFunc_952_2008070` | `02008070` | ovl_7d768c | [ovl_30_c_a_a_a_a.c](../src/overlays/rom_7d768c/ovl_30_c_a_a_a_a.c) |
| `OvlFunc_959_2008a34` | `02008a34` | ovl_7e7574 | [ovl_9dc_a_c_a_a_a.c](../src/overlays/rom_7e7574/ovl_9dc_a_c_a_a_a.c) |

**Seven of the eight matched on the first screen.** One was an unpark.

## The finding: 209 functions were never waiting on anyone

Batch 63 measured the pool tell at 271 unelevated functions and concluded they
were *"blocked on naming... a maintainer's call"*. **The second half was wrong.**

The four `.sym` files already define **611 symbols**. Cross-referencing:

| | count |
|---|---|
| Unelevated functions with the pool tell | 270 |
| **Every value already has a symbol** | **209** |
| Some values have symbols | 13 |
| No values have symbols | 48 |

209 functions were not blocked on a decision. They were blocked on nobody having
looked in `area.sym`. `OvlFunc_952_2008070`'s own park said *"it is a maintainer's
call, since inventing a name is exactly what this effort has deliberately
deferred"* — and `_AREA_8b` had been defined the whole time. Using it took that
function from 7 of 32 to exact.

## The technique, which was also already here

Documented at the top of `area.sym` and used by earlier elevations:

```c
extern int _AREA_3c;
if (v == (int)(&_AREA_3c)) ...      /* compare against its ADDRESS */
```

gcc-2.96 **always** pools a symbol's address and **never** pools a constant it
can build with an eight-bit `mov`. That asymmetry is the whole content of the
pool tell. An absolute symbol definition emits no bytes, so the link is
byte-identical and `make compare` proves it.

`OvlFunc_926_200a574` is the clean demonstration: it compares against `_AREA_3c`
(pooled) **and** against a literal `3` (`cmp r3, #0x3`) in the same function. The
tell really does distinguish symbols from literals rather than being an artifact
of the disassembly.

## A correctness problem caught before it shipped

The first ready-list picked symbols **by value alone**. That named the
`OvlFunc_939_*` functions `_FILE_68` and `_FILE_9f`.

**95 small values are defined in more than one namespace** — `0x68` is both
`_AREA_68` and `_FILE_68`. Both emit the same word, so a value-only lookup
produces a byte-correct ROM that **asserts a false thing about the code**. That
is precisely what `docs/names.md`'s `Basis` column exists to prevent: laundering
a guess into a fact.

The rule, read off existing elevated code rather than assumed:

| namespace | identified by |
|---|---|
| `_FILE_xx` | passed to `GetFile()` |
| `_MSG_xx` | passed to the message calls (`Func_801776c`, `Func_8017658`) |
| `_AREA_xx` | **compared against** the halfword at `gState+0x1C0` |

`tools/sym_candidates.py` implements this and **deliberately classifies only the
area case**, because reading `gState+0x1C0` is a distinct instruction signature
that cannot be confused with anything else. `file` and `msg` candidates are
reported as UNCLASSIFIED with their call targets, for identification by hand.
Guessing from the value is called out in the docstring as the thing not to do.

51 area candidates remain.

## Reading beats porting, twice

`OvlFunc_939_2008388` shares its shape and **both** area ids with its two
siblings, and the instruction streams are otherwise identical — but it returns
its two pointers **the other way round**. Porting one onto the other would have
silently swapped them. Caught by reading all three.

## A source shape that hangs gcc-2.96

`docs/repro/` holds a reproducer. `/opt/gcc296/xgcc -O2 -mthumb ...` spins
indefinitely on it; killed at 25s, 40s and 60s.

It is **not** the naming — replacing the symbol with a literal still hangs — and
four reductions failed to trigger it, so the reduction was abandoned
deliberately and the full file kept.

**The operational lesson is worth more than the bug.** A compiler hang looks
exactly like a slow container. Two piled up behind `docker run` before it was
recognised, and the first diagnosis — *"Docker has stalled"* — was wrong. If a
screen takes minutes, check `docker ps` for a long-running container and
time-bound the compiler directly.

The affected function, `OvlFunc_910_200809c`, is one of the 51 remaining area
candidates. The other 50 are unaffected.
