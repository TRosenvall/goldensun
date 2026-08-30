# Batch 144 — naming a value: which register class it asks for

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every
address below was read back out of the linked overlay ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_954_20081a8` | `020081a8` | rom_7db0c8 | [ovl_30_c_c_a_a_a_c_c.c](../src/overlays/rom_7db0c8/ovl_30_c_c_a_a_a_c_c.c) |
| `OvlFunc_948_2009edc` | `02009edc` | rom_7d30e0 | [ovl_30_c_c_c_c_c_c_c_c_a.c](../src/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_a.c) |
| `OvlFunc_927_2008cd0` | `02008cd0` | rom_7b4558 | [ovl_30_a_c_a_a.c](../src/overlays/rom_7b4558/ovl_30_a_c_a_a.c) |
| `OvlFunc_955_20080c0` | `020080c0` | rom_7ddb88 | [ovl_30_c_c_a_c_c.c](../src/overlays/rom_7ddb88/ovl_30_c_c_a_c_c.c) |
| `OvlFunc_964_2008f4c` | `02008f4c` | rom_7ed0a0 | [ovl_30_a_a_c_c_a_a_a.c](../src/overlays/rom_7ed0a0/ovl_30_a_a_c_c_a_a_a.c) |
| `OvlFunc_926_200a5b8` | `0200a5b8` | rom_7b2078 | [ovl_314_c_c_a_c_c_c_c_c_b.c](../src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_c_b.c) |

Two functions were parked, at **9 of 54** and **three instructions short**.

Three of the six matched on the **first screen**, which is the practical result
of this batch: the levers are now good enough to apply before screening rather
than discover afterwards.

## The naming lever, resolved: it is about register CLASS

Batches 142 and 143 recorded that naming an expression can be the whole fix,
catastrophic, or inert, and found one condition that separates cases (naming
blocks reassociation only when gcc cannot see the definition). This batch found
the other, and it is simpler than it looked.

**What a name does is ask for a register. The question is which class.**

*Name stack arguments.* A value passed on the stack, given a local, gets
materialised into its own low register and then stored — which is what the ROM
does. `OvlFunc_954_20081a8` needed this three times over, and sharpened it:
they must be named **per call site**. One shared pair across three
six-argument calls scores **20 differing**; a separate pair per call scores
**2**, because sharing makes gcc keep one pair alive across the whole function
and reload it.

*Do not name zeros.* `OvlFunc_926_200a5b8` passes zero as two of eight
arguments. Named in a local it gets promoted to a **callee-saved** register —
gcc pushes r8 and holds it, six instructions more than the ROM, which
rematerialises `mov r3, #0` at each use. Removing the local and passing bare
literals matched exactly.

*Name a mask that would otherwise be narrowed.* From batch 143's
`OvlFunc_947_200a1ac`: `p[9] = (p[9] & -13) | 8;` lets gcc truncate the mask to
`0xf3` because the result is stored into a byte. An `int mask = -13;` first
stops that. It is the mask, not the value being masked — an `int` intermediate
for the loaded byte is worse.

The three cases stop contradicting each other once you ask what the name is
requesting. A stack argument wants a low register it was going to need anyway.
A zero wants nothing, and asking gets it a callee-saved register it does not
deserve. A mask wants its full width preserved through an operation whose
destination would otherwise justify narrowing it.

## Both arms end the same way: write it twice

`OvlFunc_948_2009edc` ends both arms of a branch by writing the same field of
the same actor. The ROM duplicates the CALL in each arm and shares only the
`add r0, #0x23 / strb` tail. The tidy C —

```c
if (...) { ...; p = __MapActor_GetActor(0xb); }
else     { ...; p = __MapActor_GetActor(0xb); v = 0; }
p[0x23] = v;
```

— costs an instruction: a named variable spanning the merge point forces gcc to
copy the merged value out of the register the arms left it in. Writing the whole
store inside both arms and letting gcc tail-merge gives the ROM exactly. 71
lines to 70.

gcc's tail merging is reliable and does not need arranging. `Func_80974d8`
confirms it from the other side: writing its shared store in both branches or
once through a temporary produces byte-identical output. The rule is narrow —
do not introduce a **named** variable that outlives the join.

## Applying findings up front is what made this batch cheap

`OvlFunc_927_2008cd0` matched on the first screen because batch 143's
else-return finding was applied before writing rather than after failing. The
ROM keeps `mov r0, #0` in its own block after the branch, which is the signature
of `if (cond) { ...; return 1; } else { return 0; }` rather than an early
`if (!cond) return 0;`.

`OvlFunc_955_20080c0` matched on the first screen from three known levers at
once: per-call-site stack arguments, a named local for the constant the ROM
holds across all three calls, and naming the shifted value passed to
`__SetFlagByte` so its `asr` lands before the constant's `lsl`.

## Read the idiom, still

`OvlFunc_964_2008f4c`: `lsl r3, r0, #3 / sub r3, r0` is a multiply by seven, and
the following `lsr` is unsigned, giving `((Random() * 7 >> 16) - 3) << 16`.

`OvlFunc_926_200a5b8`: a longer chain — `x*3`, then `*17`, then `*257` — is
gcc's own shift-and-add synthesis of a multiply by **13107**. Writing
`* 13107` reproduces all six instructions with no help. Do not transcribe the
chain.

`OvlFunc_948_2009ac8`: `if (x < 0) x += 0xfffff` before an `asr #20` is a signed
division by `0x100000`; the constant never appears in the source.

## A sweep that found nothing, recorded so it is not repeated

Batch 143's `volatile` sweep paid off, so the mask-narrowing lever got the same
treatment. It found nothing. Only two parks carry an inline negative or
complement mask, and both apply it in **int** context rather than to a byte
destination — naming the mask there makes one of them worse, 22 differing to 30.
The lever is specific to a mask whose result is stored back into a narrower
type; it is not a general "name your masks" rule.

## Parked this batch

| Function | Standing | Blocker |
|---|---|---|
| `OvlFunc_947_200a1ac` | 9 of 54 | rematerialised constant, register choice |
| `OvlFunc_948_2009ac8` | 3 short | a zero gcc will not keep callee-saved |

`OvlFunc_948_2009ac8`'s three missing instructions are exactly `push {r6}` /
`mov r6, #0` / `pop {r6}`. The ROM sets a zero before a four-way branch and
holds it across three calls to use it twenty instructions later; gcc
rematerialises at the point of use, which is strictly cheaper. Naming the zero
is necessary and not sufficient, and lengthening its live range to the top of
the function changed nothing.

**This is the same allocator behaviour that DELIVERED `OvlFunc_926_200a5b8`.**
gcc will not spend a callee-saved register on a value it can rebuild in one
instruction. There it blocks a match; here it produced one. `Func_80b0070` sits
on the same wall, and the three parks now cross-reference each other.
