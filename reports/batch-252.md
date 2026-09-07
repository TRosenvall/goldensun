# Batch 252 — an eighteen-copy park falls to one diagnostic compile

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_968_200af30` | `0x0200af30` | [ovl_30_c_c_a_a_c_a_c.c](src/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_c.c) |
| 2 | `OvlFunc_968_20087d8` | `0x020087d8` | [ovl_30_a_a_c_a_c.c](src/overlays/rom_7f2f14/ovl_30_a_a_c_a_c.c) |
| 3 | `OvlFunc_968_20085e4` | `0x020085e4` | [ovl_30_a_a_a_c_c_c_b.c](src/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_c_b.c) |
| 4 | `OvlFunc_968_20086a0` | `0x020086a0` | [ovl_30_a_a_c_a_a.c](src/overlays/rom_7f2f14/ovl_30_a_a_c_a_a.c) |
| 5 | `OvlFunc_968_2008cc8` | `0x02008cc8` | [ovl_30_a_c_c_c_a.c](src/overlays/rom_7f2f14/ovl_30_a_c_c_c_a.c) |
| 6 | `OvlFunc_968_2008374` | `0x02008374` | [ovl_30_a_a_a_c_c_a_a_b.c](src/overlays/rom_7f2f14/ovl_30_a_a_a_c_c_a_a_b.c) |
| 7 | `OvlFunc_883_20080c4` | `0x020080c4` | [ovl_30_a_a_a_c_c_a.c](src/overlays/rom_780898/ovl_30_a_a_a_c_c_a.c) |
| 8 | `OvlFunc_905_20080c4` | `0x020080c4` | [ovl_30_a_a_a_c_a_c_a.c](src/overlays/rom_799abc/ovl_30_a_a_a_c_a_c_a.c) |
| 9 | `OvlFunc_913_20080c4` | `0x020080c4` | [ovl_30_a_a_a_c_a_c_a.c](src/overlays/rom_7a04ac/ovl_30_a_a_a_c_a_c_a.c) |
| 10 | `OvlFunc_914_20080c4` | `0x020080c4` | [ovl_30_a_a_c_a_c_a.c](src/overlays/rom_7a1ff0/ovl_30_a_a_c_a_c_a.c) |
| 11 | `OvlFunc_915_20080c4` | `0x020080c4` | [ovl_30_a_a_a_c_c_a.c](src/overlays/rom_7a2bf0/ovl_30_a_a_a_c_c_a.c) |
| 12 | `OvlFunc_923_20083a8` | `0x020083a8` | [ovl_314_a_c_a_c_a.c](src/overlays/rom_7aa430/ovl_314_a_c_a_c_a.c) |
| 13 | `OvlFunc_924_20083a8` | `0x020083a8` | [ovl_314_a_c_a_c_a.c](src/overlays/rom_7ac2d8/ovl_314_a_c_a_c_a.c) |
| 14 | `OvlFunc_927_20080c4` | `0x020080c4` | [ovl_30_a_a_c_a_c_a.c](src/overlays/rom_7b4558/ovl_30_a_a_c_a_c_a.c) |
| 15 | `OvlFunc_934_20083a8` | `0x020083a8` | [ovl_314_a_a_a_c_c_a.c](src/overlays/rom_7bdeb0/ovl_314_a_a_a_c_c_a.c) |
| 16 | `OvlFunc_946_20080c4` | `0x020080c4` | [ovl_30_a_a_a_c_c_a.c](src/overlays/rom_7ced6c/ovl_30_a_a_a_c_c_a.c) |
| 17 | `OvlFunc_947_20083a8` | `0x020083a8` | [ovl_314_a_c_a_c_a.c](src/overlays/rom_7d0e88/ovl_314_a_c_a_c_a.c) |
| 18 | `OvlFunc_948_20080c4` | `0x020080c4` | [ovl_30_a_a_a_c_a_c_a.c](src/overlays/rom_7d30e0/ovl_30_a_a_a_c_a_c_a.c) |
| 19 | `OvlFunc_957_20080c4` | `0x020080c4` | [ovl_30_a_a_a_c_a_c_a.c](src/overlays/rom_7e3e08/ovl_30_a_a_a_c_a_c_a.c) |
| 20 | `OvlFunc_958_20083a8` | `0x020083a8` | [ovl_314_c_a_c_a.c](src/overlays/rom_7e636c/ovl_314_c_a_c_a.c) |
| 21 | `OvlFunc_959_20080c4` | `0x020080c4` | [ovl_30_c_a_c_a.c](src/overlays/rom_7e7574/ovl_30_c_a_c_a.c) |
| 22 | `OvlFunc_964_20080c4` | `0x020080c4` | [ovl_30_a_a_a_c_a_c_a.c](src/overlays/rom_7ed0a0/ovl_30_a_a_a_c_a_c_a.c) |
| 23 | `OvlFunc_965_20080c4` | `0x020080c4` | [ovl_30_a_a_a_c_a_c_a.c](src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_a_c_a.c) |
| 24 | `OvlFunc_955_2008970` | `0x02008970` | [ovl_30_c_c_c_a_c_c_c_c_c_c_c_c_a.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_a_c_c_c_c_c_c_c_c_a.c) |
| 25 | `OvlFunc_955_2008160` | `0x02008160` | [ovl_30_c_c_c_a_a_a_a.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_a_a_a_a.c) |
| 26 | `OvlFunc_955_2009424` | `0x02009424` | [ovl_30_c_c_c_c_c_b.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_b.c) |
| 27 | `OvlFunc_955_2008714` | `0x02008714` | [ovl_30_c_c_c_a_c_c_c_c_c_c_c_a_a.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_a_c_c_c_c_c_c_c_a_a.c) |
| 28 | `OvlFunc_884_2008940` | `0x02008940` | [ovl_30_c_a_c_c_c_a_a.c](src/overlays/rom_784360/ovl_30_c_a_c_c_c_a_a.c) |
| 29 | `OvlFunc_884_2008bbc` | `0x02008bbc` | [ovl_30_c_a_c_c_c_a_a.c](src/overlays/rom_784360/ovl_30_c_a_c_c_c_a_a.c) |
| 30 | `OvlFunc_925_2008b24` | `0x02008b24` | [ovl_314_c_c_c_a_c_c_a_a_b.c](src/overlays/rom_7b0400/ovl_314_c_c_c_a_c_c_a_a_b.c) |

Thirty functions, the largest batch in the project. Twenty-nine landed **whole**
— no split, no linker edit, no flag rule; one needed a routine two-way split.

## Parks retired

| park | covered | was |
|---|---|---|
| `ovl_780898/20080c4.c` | **18 functions** | 7 of 176, three rounds, fifteen spellings |
| `ovl_7ddb88/2008970.c` | 1 | 25 of 28 |
| `ovl_7ddb88/2008160.c` | 1 | 17 of 107 |
| `ovl_7ddb88/2009424.c` | 1 | 67 of 108 |
| `ovl_7f2f14/200af30.c` | 1 | 21 of 37 |
| `ovl_7f2f14/20087d8.c` | 1 | 6 of 76 |

Parked total falls 679 → 652.

## THE EIGHTEEN-COPY PARK, AND WHY IT SAT

`ovl_780898/20080c4.c` covered eighteen byte-identical copies of the
block-pushing routine, one per overlay, and called itself *"the largest single
lever left in the tree."*

> **The lever is alias set 0 used as a SCHEDULING device.**

Wrapping three struct fields in one-member unions gives their stores
`MEM_ALIAS_SET 0`, which chains stores that same-base offset disambiguation
would otherwise leave independent. That lengthens the dependence path behind
`mov r1, sl` and lifts its sched2 priority above the constant build that was
jumping over it. It is a **set** — dropping any one of the three costs 3, 4 or
5 — and the right set is the fields whose **stores** are mis-ordered, not whose
values are.

**What made it findable was one diagnostic compile.** Running
`-fno-schedule-insns2` *first* collapsed the window 7 → 4, proving sched2 owned
the residue and pointing at an **alias** lever rather than another source
permutation. The park had exhausted ORDER twice and never tried ALIAS — exactly
the failure the recorded search order exists to prevent. The flag was never
shipped; it named the pass in one compile.

The same trick re-classified `2008970`, whose park said cross-jumping with *"no
known fix"*. It **is** cross-jumping — but the identical tails are *manufactured
by sched2*, which fills a load-use slot and drops an instruction to the end of a
block where another block also ends, and `jump.c` merges them. One empty
`__asm__ __volatile__("")` stops the chain: **25 → 0**.

## AND THE BOUNDARY ON THAT SAME LEVER

Landed in the same batch, which is the useful part:

> `volatile` and union-member access are both **INERT against cross-jumping**.

`jump.c`'s `rtx_renumbered_equal_p` consults neither `MEM_VOLATILE_P` nor
`MEM_ALIAS_SET`, so the batch-244 alias escape is a **gcse/cse lever only**.
Both were measured and both failed before the sched2 mechanism was found.

## A VERDICT THAT MUST BE READ PAST

`OvlFunc_925_2008b24` is the first landing where `objcmp` cannot be satisfied:

```
XX ENCODINGS differ in 1 place(s) (ref 1590, ours 1590)
   first at index 1587: ref 0000003a  ours 00000000
   ours-only relocation: 00000fc0 R_ARM_ABS32 _AREA_3a
```

Size and instruction count already agree. Ours holds a **relocation
placeholder** where the ROM's object holds the resolved value, because
`_AREA_3a = 0x3a;` is defined in `area.sym`, which `stage1.ld:17` INCLUDEs.
`make compare` — the only authority that sees past the object level — is green.

**The literal is worse, and that is the evidence the symbol is right.** Spelling
it `0x3a` gives 5 differing, one instruction and four bytes short, because the
ROM pools a value below 256 — which a literal never would.

> **A pooled small constant is a tell that the source referenced a symbol.**
> Object-level identity is sufficient but not necessary; where a link-time
> symbol is involved, the compare gate decides.

## SMALLER RESULTS WORTH KEEPING

- **A stack argument can need a REGISTER pin.** r3/r2 for arguments 5 and 6 of
  six-argument calls, worth 17 → 8. The recorded lever is about *naming* stack
  values; this is the placement half, and it applies only where the fifth and
  sixth arguments differ.
- **Class-drop is the cheapest first minimisation.** 26 builds removed 219 of
  373 pins; 16 of 26 callee classes need none, split by whether the callee's
  constants are shifted or pooled rather than by argument count.
- **Dropping a pin was the last lever twice more** — once removing a
  `SetBehavior` pin for the final encoding, once because a link-time constant
  evicts nothing when pinned.
- **`register T x __asm__("rN")` on a callee-saved local overrides the allocator
  outright** — 17 → 4 on one line. This bounds batch 205's *"the pin is inert
  against all four register-placement parks"*, which was about r0–r2 **argument**
  pins.
- **An argument pin assigned FIRST wins the scheduler's `INSN_LUID` tiebreak**,
  because `precompute_register_parameters` copies shiftable-const arguments into
  pseudos before the r0 copy is emitted.
- **Where a dominating branch exists, naming buys the same effect for free** —
  one function matched with no pins at all, while its pinned variant sticks at
  38 of 207. Look for the branch before reaching for the pin.
- **Two halfwords short with one fewer instruction is a `use_related_value`
  site** — the ROM builds `0x4013` from a live `0x4000` with `adds r5, #19`.
- **Minimisation from both ends can agree on cardinality and disagree on
  membership.** Where both directions reach the *identical* set, that is a
  property of the function rather than of the sweep.
- **A conditional arm is not a special region** — six of seventeen residual
  diffs were diamond-arm statements the harness could not reach, not a class.

## THE PARKS WERE THE USEFUL INPUT

Three of four functions in one overlay were parks, and **each had named its
blocker correctly**. The landed sibling offered as a template was not what
carried them.

> A park that names its blocker is worth more than a neighbour that shares its
> callees. It is also worth re-reading parks whose reasoning is sound but
> *incomplete* — one closed here had concluded from `local-alloc.c` that a
> straight-line function can never rematerialise a constant, which is true and
> misses that r0–r3 are call-clobbered.

`src/non_matching/overlays/constant_reuse.c` should be revised: an eleven-flag
sweep found nothing there because the answer was never a flag.
`src/non_matching/ovl_7aa430/20091b4.c` is worth re-running — all four of its
recorded rungs tie at 5 on an identical shape where one barrier is exact.

## Still held back

`OvlFunc_968_200c048` and `OvlFunc_968_200c520` (both matched and verified;
`200c2bc` sits in the middle of their `.s` at a real floor of 2 of 271), and
`OvlFunc_933_2009874` (matched; needs a five-way cut where the tool does three).
Candidates preserved, parks left in place.

## Gate

`make clean && make -j8 && make compare` green, `goldensun.gba: OK`. Every
address checked against the linked ELF. Pairing 4,634 sources; `--orphans` 0;
`--unlinked` unchanged at 10.
