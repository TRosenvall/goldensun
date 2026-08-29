# Batch 46 — seven functions, five of them a family

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`
and sits at exactly the address its name claims.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_923_2008ed0` | `02008ed0` | ovl_7aa430 | [ovl_e90_c_c_a_a_c_b.c](../src/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_b.c) |
| `OvlFunc_946_2009b14` | `02009b14` | ovl_7ced6c | […_c_c_c_a_b.c](../src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_b.c) |
| `OvlFunc_946_2009b68` | `02009b68` | ovl_7ced6c | […_c_c_c_c_a.c](../src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a.c) |
| `OvlFunc_964_2008dc8` | `02008dc8` | ovl_7ed0a0 | [ovl_30_a_a_c_a_b.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_c_a_b.c) |
| `OvlFunc_964_2008df4` | `02008df4` | ovl_7ed0a0 | [ovl_30_a_a_c_a_c_b.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_c_a_c_b.c) |
| `OvlFunc_964_2009348` | `02009348` | ovl_7ed0a0 | [ovl_30_a_c_a_a_c_a.c](../src/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_a.c) |
| `OvlFunc_964_20093b4` | `020093b4` | ovl_7ed0a0 | [ovl_30_a_c_a_c_a.c](../src/overlays/rom_7ed0a0/ovl_30_a_c_a_c_a.c) |

Two of the seven were **unparked without changing a line of C**. Five are a
family that fell out once one of those two was elevated.

## Two parks that were never blocked

Batch 45 found that a Makefile `%` rule can describe a neighbouring translation
unit and silently compile a file at the wrong `-O`. The obvious follow-up was to
ask which *existing* parks had been screened under wildcard-sourced flags.

Two of 105 qualified. **Both were mis-parked**, and both match on exactly the C
already sitting in their park notes:

| Function | Parked as | Under wildcard | At -O2 |
|---|---|---|---|
| `OvlFunc_923_2008ed0` | argument fill order, 6 of 41 | `ovl_e90_c_c_a_a%` | matches |
| `OvlFunc_964_2009348` | scheduling, 6 of 18 | `ovl_30_a_c_a_a%` | matches |

`ovl_e90_c_c_a_a%` captured children of a neighbouring `.s`. `ovl_30_a_c_a_a%`
captured **the other half of its own `.s`'s split** — the two halves of one
positional carve turning out to be two different original TUs, which is the
clearest possible demonstration that the split chain is not a TU boundary.

Both rules are now explicit targets plus a `_%` child pattern. The files that
legitimately need `-O1` keep it, and a clean build is green before and after.

### A documented rule has been withdrawn

`docs/elevation.md` carried a section titled *"The declaration lever is an -O2
behaviour"*. **It is wrong and has been withdrawn.**

Its evidence was a natural control: `OvlFunc_923_2008ed0` (believed -O1) and
`OvlFunc_924_2008f84` (-O2) are the same forty-one instructions, and the
identical C matched at -O2 and failed at -O1. Every observation was accurate.
The inference was not, because **`OvlFunc_923_2008ed0` was never an -O1
translation unit**. There is no -O1 counterexample to the declaration lever, and
none was ever observed.

**The control was pointing at the Makefile and was read backwards.** Two
functions that match on identical C are telling you they are the same TU shape.
When their recorded flags differ, that is evidence about the *build*, not
evidence that the flags explain the difference.

That is the same signal missed in batch 45 — where `OvlFunc_888_200b270` matched
on C structurally identical to a function that would not — so it is now written
into the doc as the thing to check rather than as a fact about one pair.

Both park notes had also warned *"this TU is built at -O1, a screen at -O2 would
be meaningless"*. The -O2 screen was right about the code and was overruled by a
rule that did not belong to the TU. A warning written as prose does not survive
contact with a tool that says otherwise — which is why the check now lives in
`tools/tryc.py` and not only in a comment.

## The position-triple family

With `OvlFunc_964_2009348` elevated, its C became a template and five siblings
fell out in a single round, all screening OK on the first attempt.

The shape: read slot 0's `x`/`y`/`z`, offset **one** of the three, and pass the
triple by address.

```c
struct Actor3 *actor = __MapActor_GetActor(0);
int pos[3];

pos[0] = actor->x + 0xffe00000;
pos[1] = actor->y;
pos[2] = actor->z;
OvlFunc_964_2008cd0(pos);
```

Two details are read off the ROM rather than chosen:

- **The offset is always an addition, never a subtraction.** The ROM loads the
  constant and adds it, including when it is negative — `+ 0xffe00000`, not
  `- 0x200000`.
- **How the constant is spelled follows how the ROM builds it.** `mov r1, #0x80
  / lsl r1, #14` means write `0x80 << 14`; `ldr r2, =0xffe00000` means write the
  literal. Both spellings appear in adjacent files in the same overlay.

This is what a family is worth: one function's worth of thinking, five
functions' worth of output. It is also an argument for revisiting parks — a
single wrong diagnosis had been holding up not one function but six.

## `pop {r1}` in a function that looks void names a RETURN VALUE

The two `rom_7ced6c` members end `pop {r1} / bx r1` where the three `rom_7ed0a0`
members end `pop {r0} / bx r0`. Written as `void`, the two are **2 of 17**,
differing in exactly that pair and nothing else — a diff small enough to read as
noise.

gcc-2.96 uses `r0` as the Thumb epilogue's scratch register whenever it can. It
reaches for `r1` only when **`r0` is still live across the epilogue**, which
happens when the value there is the function's own return value. So the ROM is
saying the function returns whatever its last call left in `r0`:

```c
return OvlFunc_946_2009a44(actor, pos);
```

This is not recoverable from the body: a tail call whose result is discarded and
one whose result is returned have identical bodies. **Check the epilogue
register before writing `void`.**

The same two instructions settled those functions' argument lists. The ROM never
rewrites `r0` before the `bl`, so the pointer returned by the earlier
`__MapActor_GetActor` is still there and is the *first* argument, with the array
in `r1` — the callee takes the actor **and** the triple, where the three siblings
take only the triple.

Added to `docs/elevation.md`.

## Process note

Deleting a hand-written `.s` and then building regenerates a gcc-produced file
at the same path, so git reports it as **modified** rather than deleted. Three
files hit this across the last two rounds. Every new or changed `.s` is now
checked for the `@ Generated by gcc` marker before staging, with `git rm
--cached` used on the regenerated ones so only hand-written assembly is
committed. This is the trap `docs/elevation.md` already warns about, in a new
guise: it previously bit measurements, and now it bites `git add`.
