# Batch 114 — the ROM tells you which branch is the `if` body

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of its overlay's linked ELF.

| Function | Address | Overlay |
|---|---|---|
| `OvlFunc_917_200952c` | `0200952c` | ovl_7a4370 |
| `OvlFunc_959_200981c` | `0200981c` | ovl_7e7574 |
| `OvlFunc_959_2009918` | `02009918` | ovl_7e7574 |
| `OvlFunc_959_2009980` | `02009980` | ovl_7e7574 |
| `OvlFunc_959_20099e8` | `020099e8` | ovl_7e7574 |
| `OvlFunc_959_2009a44` | `02009a44` | ovl_7e7574 |

**6 elevated, 2434 remaining.** All by the coordinator; a second agent round is
still in flight.

## The finding: block layout tells you the polarity

Four of the six turned on the same reading, and it is cheap enough to check
every time.

**A conditional branch that jumps FORWARD past a block means that block is the
fallthrough — and therefore the `if` body.** Get it backwards and you do not get
one instruction out, you get the whole tail rearranged:

| function | wrong way round | right way round |
|---|---|---|
| `OvlFunc_917_200952c` | 42 of 55 | exact |
| `OvlFunc_959_2009980` | 10 of 56 | exact |
| `OvlFunc_959_2009918` | 3 of 54 | exact |

`200952c` reads `if (n > 0x77) { cleanup; return; }` naturally, and that is
wrong: the ROM's `bgt` skips forward over the arc code, so the arc is the body
and the cleanup is the `else`.

On a two-clause condition it means spelling the test as the **failing** case:
`if (dx > 7 || dz > 5) return 0; return 1;` rather than the passing one.

On a single flag it is subtler — **which value the ROM presets tells you.**
`mov r0, #1 / cmp / ble / mov r0, #0` presets TRUE and overwrites on failure,
which is `if (sum <= 4) return 1; return 0;`. Introducing a result variable to
force it costs an instruction.

## Signed division, spelled as division

`if (x < 0) x += 0xfffff;` followed by `asr #20` is **not** something to write
out. It is `x / 0x100000` on an `int`, and gcc generates the bias-and-shift
itself. That sequence appears four times in a 54-instruction function; writing
the bias by hand would add four branches.

Likewise the signed range: `az - bz >= -6 && az - bz <= 6` compiles to the ROM's
single `add r3, #6 / cmp r3, #0xc / bhi`. Two signed compares give two branches.

And where the ROM spells a test oddly, spell it that way: `ax - 1 < bx &&
ax + 1 > bx` is `ax == bx`, and only the long form reproduces.

## A family found by an idiom rather than a prologue

`tools/prologue_families.py` clusters on the first N instructions. These six
share no prologue — they share an *idiom*. Grepping every `.s` for functions
under 90 instructions using the `=0xfffff` division bias twice or more returns
**15 functions**, of which five are now C.

That is a third search axis alongside the prologue grep and the whole-function
shape tools: **grep for a distinctive idiom, not a position.**

## Two parks, both recording a lever's boundary

`OvlFunc_957_2008b30` (11 of 54) is the **HImode rule with nowhere to stand**. A
short store sits before any branch, so an int local at the top is carried in a
callee-saved register and one beside the store gets the pool load back. That is
the same no-boundary wall as the nine catalogued under constant-CSE, reached
from the HImode side — so the missing construct would unpark more than nine.

`OvlFunc_965_200a5c8` (17 of 55) records a boundary on the **stack-arg-pair
lever**: it fixes which register each value lands in and whether it is shared,
but not **where the stores are issued**. The ROM fills all four register
arguments first and stores last; gcc stores first. Four spellings measured.
