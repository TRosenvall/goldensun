# A source shape that HANGS gcc-2.96

`gcc296-hang.c` makes `/opt/gcc296/xgcc -O2 -mthumb ...` spin indefinitely.
Reproduced three times; killed at 25s, 40s and 60s (`rc=124`).

    /opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
        -fno-builtin -nostdinc -ffreestanding -Iinclude -S -o /tmp/o.s \
        docs/repro/gcc296-hang.c

It is an intended elevation of `OvlFunc_910_200809c`
(`asm/overlays/rom_79dd90/ovl_30_c_c_a_a_a_a.s`).

## Why this matters operationally

A hang looks exactly like a slow container. Two of these piled up behind
`docker run` before it was recognised, and the first diagnosis -- "Docker has
stalled" -- was wrong. **If a screen takes minutes, check `docker ps` for a
long-running container and time-bound the compiler directly** rather than
assuming the toolchain is merely slow.

## What it is NOT

- **Not the `_AREA_` symbol naming.** Replacing `(int)(&_AREA_22)` with the
  literal `0x22` still hangs. The naming technique is unaffected.
- **Not the array-typed return.** Returning `0` instead of the second symbol
  still hangs.
- **Not optimisation level.** Not narrowed, but the reductions below compile at
  `-O0`, `-O1` and `-O2` alike.

## Bisection, as far as it got

These all COMPILE (rc=0), so none of them alone is the trigger:

    the guard and the gState read, no flag blocks
    the same plus ONE flag block
    two flag blocks with no enclosing guard, minimal externs
    two flag blocks inside a guard, minimal externs, int return

The trigger needs something further from the full file -- candidates not yet
eliminated are the `GlobalState` typedef, the `__asm__(".Lc7c")` label, and the
`void *` return interacting with the two-block shape. **The reduction was
abandoned deliberately** rather than spend more of a session on it; the file
here reproduces it in one command, which is enough for someone to pick up.

## Consequence for the function

`OvlFunc_910_200809c` cannot be elevated in this shape. It is one of 56
area-id functions whose symbols already exist in `area.sym`; the other 55 are
unaffected and one of them was elevated the same day.
