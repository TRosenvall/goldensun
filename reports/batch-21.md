# Batch 21 — 7 functions, and a second declaration lever hiding behind the first

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–20 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted. Every address
below read back from the linked ELFs.

## Read this if you have parked anything on argument fill order

Batch 07 retired the `arg-fill-order` blocker class with what the docs call the
declaration lever: an implicitly declared callee returns `int`, so gcc-2.96
keeps r0 live across it and defers writing r0 when setting up the **next**
call's arguments. Prototype the callee — with its `void` return — and gcc fills
r0 first. That has decided functions in nine batches and it is all still true.

**It is not the only declaration lever, and the second one had been parked
against.** Leaving the *mismatching call itself* implicit changes the order gcc
fills **that call's own** argument registers:

    prototyped   mov r0, r8    / mov r1, #0x80 / mov r2, r5
    implicit     mov r1, #0x80 / mov r2, r5    / mov r0, r8
    rom          mov r1, #0x80 / mov r2, r5    / mov r0, r8

The two effects are indistinguishable from the symptom — in both, a missing
prototype moves r0 — and conflating them cost two functions a park each.

`LoadStatusIcon` had been parked since batch 05 with a note recording three
failed reformulations and the flat conclusion *"the order does not move"*. One
of the things never tried was deleting the declaration of the call that was
actually wrong. It matched on the first screen. `Func_8078948` had been parked
the same way, cited that note as the same class, and also matched first screen.

So when fill order is the only mismatch there are **four** things to try:

1. prototype every callee (the batch-07 lever — still the common case);
2. make the **preceding** call implicit;
3. make the **mismatching** call implicit;
4. prototype the mismatching call but not the preceding one.

**The limit, stated honestly:** this moves r0 and only r0. Where the
transposition is among the non-r0 arguments — `mov r1 / ldr r2` against
`ldr r2 / mov r1` — neither lever reaches it. `OvlFunc_882_2008398` stays parked
for exactly that reason, and had already tried both prototype directions before
any of this was understood.

## Functions

| function | address | file | note |
|---|---|---|---|
| `OvlFunc_924_2008f84` | `0x02008f84` | `src/overlays/rom_7ac2d8/ovl_f84_a_a_b.c` | map-exit cutscene, -O2 twin |
| `OvlFunc_945_200c198` | `0x0200c198` | `src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_c_b.c` | |
| `OvlFunc_957_200b518` | `0x0200b518` | `src/overlays/rom_7e3e08/ovl_30_c_c_c_a_b.c` | one-shot scene behind two flag guards |
| `OvlFunc_968_2008754` | `0x02008754` | `src/overlays/rom_7f2f14/ovl_30_a_a_c_a_b.c` | third map-exit twin |
| `LoadInventoryIcon` | `0x0801a2a4` | `src/rom_15000/rom_19ebc_a_c_c_c_a_b.c` | second lever |
| `LoadStatusIcon` | `0x0801a2ec` | `src/rom_15000/rom_19ebc_a_c_c_c_a_c_b.c` | **unparked**, second lever |
| `Func_8078948` | `0x08078948` | `src/rom_77000/rom_78414_c_c_a_c_b.c` | **unparked**, second lever |

### The map-exit cutscene family

Three of the seven are the same function in three overlays: install an update
hook on slot 0, set its speed, walk it, clear the hook, fade out, hand off.
`OvlFunc_923_2008ed0` is the fourth and is **still parked**, because its TU
builds at **-O1** and the batch-07 lever is an -O2 behaviour — its identical
twin at -O2 matches unchanged. That remains the cleanest natural control in the
tree for the lever being an optimisation-level effect, and the second lever
found here does not change it.

`OvlFunc_968_2008754` differs from the twins at both ends and needed two
existing techniques:

* the final argument is **read** from `iwram_3001ebc` rather than passed in, and
  read *early* — the load must be its own statement at the top of the body or
  gcc sinks it past the fifteen intervening calls;
* the tail builds `0x16c` at runtime, so the offset is written as separate
  statements over a typed base — the statement-form lever `GetEntrances` needed.

## Parked this batch

`OvlFunc_968_20087d8` (`src/non_matching/ovl_7f2f14/20087d8.c`), the map-entry
counterpart in the same overlay, at 70 lines against 72 on **constant-CSE**.
The ROM materialises `-1` three times; gcc builds it once and copies:

    rom    mov r0,#1 / mov r1,#1 / mov r2,#1 / neg r0 / neg r1 / neg r2
    ours   mov r2,#1 / neg r2,r2 / mov r0,r2 / mov r1,r2

Note the direction, which an earlier note in this tree had backwards: **the
ROM's redundant form is the target and gcc's optimisation is the defect.** The
ROM is not being clever; it is doing the same cheap thing three times, and gcc
is too smart to reproduce it. Screened at both -O2 and -O1 — -O1 fixes an
unrelated pointer-hoist in the same function but the CSE survives untouched, so
a per-file -O1 rule would not buy the match.

Three sub-problems there *are* solved and are recorded in the park note, the
most interesting being that gcc **will** reuse a value a compare established:
the two zero stores are spelled `*p = flag`, using the `__GetFlag` result that
the `cmp r6, #0 / bne` above proved is zero, and literal `0` costs two extra
lines. So gcc reuses aggressively in both directions; here only one of the two
is what Camelot's compiler did.

## Correction to the tooling notes in earlier batches

`docs/elevation.md` now says the build must run in the container. A round of
this batch went into "fixing" the Makefile for two failures that turned out to
be artifacts of running bare `/usr/bin/make` on macOS:

* GNU make **3.81** orders the `elf_deps` prerequisites ahead of the
  static-pattern `%.ld`, so `-T $<` hands ld an ELF object and it reports
  `ignoring invalid character '\000' in script / syntax error`;
* **BSD sed** reads `-i`'s argument as a *backup suffix*, so the script is eaten
  and the filename becomes the script — every `.d` under `asm/` then parses as
  sed's `a` command.

Both are host-only and the Makefile needed no change; the documented Docker
build was green the entire time. Flagged here in case anyone builds on macOS
outside the container and reads the same symptoms as tree corruption.

## Counts

225 functions elevated in total. 3,074 hand-written functions remain in `asm/`
of 5,714. 85 parked.
