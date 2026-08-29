# Batch 141 — a 521-function "toolchain ceiling" was not one

Verified on a clean `make clean && make compare` — `goldensun.gba` SHA1
`5c4695205413df7db52b9a184815a07783999971`, all 96 overlays comparing — with
every address below read out of the linked ELFs.

**remaining 2205 · elevated 3212 · parked 346**

## Elevated (5) — all from the branch-over-pool class

| function | address | ELF | what it took |
|---|---|---|---|
| `OvlFunc_881_200b8fc` | `0200b8fc` | rom_77a7c8 | plain literals; nothing special |
| `OvlFunc_common1_1490` | `0200ad28` | common1 | naming a shift against a pool load |
| `Func_801e858` | `0801e858` | goldensun | first screen, no lever |
| `UIDrawText` | `0801e940` | goldensun | `unsigned` parameters |
| `Func_8020b14` | `08020b14` | goldensun | finishing the offset before the base |

## The class was written off on a rationale that failed three times

`tools/poolblocked.py` justified 521 functions as unreachable with "old_agbcc
emits a function's constant pool at `.func_end` and never in the middle".

1. **old_agbcc is not the compiler.** `/opt/gcc296/xgcc` builds essentially
   everything; old_agbcc builds five m4a and agb_flash objects.
2. **gcc296 does emit mid-function pools** — 64 already-matching functions have
   one. It never writes `.pool`, it writes `.word` tables at its own labels,
   which is why searching for the directive found nothing and read as proof.
3. **The functions themselves match.** Five now do.

The measurement was wrong in both directions before it was right. A looser test
— "any data mid-body" — gives 562, and is also wrong: 85 matching functions have
mid-body `.word` blocks and every one is a switch **jump table**. Pools and jump
tables are both data in the middle and mean opposite things.

**What the class actually holds**, measured: of 521, 198 also match the
precompute shape, 43 also match const-remat, 3 are ARM. That leaves **277 where
the pool is the only recorded blocker** — and they are mostly large: 176 over
120 instructions, only 7 at 35 or under. The small end is now exhausted. The
reachable part is real but expensive per function.

## Levers found, and their preconditions

**Pool ENTRY ORDER is reachable.** An `int` intermediate moves a constant out of
gcc's HImode-literal group into general reference order:

```c
*q = 0x3f3f;          ->  3f42 c04 3f3f 4000050 3001ecc 534 536 52a
v = 0x3f3f; *q = v;   ->  3f42 c04 4000050 3001ecc 534 3f3f 536 52a  = ROM
```

**A halfword literal is pooled; naming it defeats that.** The reverse of the
above, and the pair is easy to confuse. `*(short *)a = 0;` pools the zero;
`int z = 0; *(short *)a = z;` gives `mov`. Both directions are now documented
together.

**The offset pair.** *Clobber* the offset after taking the address to force an
explicit address computation; *finish* the offset before the base to get
register-offset addressing. Which the ROM wants is visible in the store —
`[rA, #0]` against `[rB, rO]` — before any screen is run.

**Which operand is the pointer decides the addressing base.** `[r5, r7]` versus
`[r7, r5]` is chosen by C pointer type, not by the order of the addition, which
gcc normalises. Declaring the offset as `char *` and the base as `int` puts them
in the ROM's roles.

**Naming a shifted value works against a pool load, not against a cheap `mov`.**
Against another expensive operand it is worth one screen; against a cheap `mov`
it is the argument-precompute wall, settled by eleven probes.

## Two corrections to earlier conclusions

**A "dead register" was a type error.** `UIDrawText` has `mov r8, r3` before a
call with r8 never read — the shape HANDOFF describes as unreachable prologue
bookkeeping, and it nearly went to a park. With `unsigned` parameters it matches
at 45 of 45 and gcc emits that instruction itself. Check the `lsr`/`asr`
signedness tell before diagnosing a dead register.

**A recorded lead was false.** `ovl_78c76c/20095d4.c` said gcc emits the ROM's
argument interleave from `f3(0xe, 0x102, 0x204)`. Compiling exactly that gives
gcc's usual form. Eleven source forms were probed — argument count, prototype
presence, signedness, locals, stack arguments — and every one is identical. That
class is a genuine compiler difference and the note is corrected.

## Parked (6)

`OvlFunc_919_2008200` (18/36, register-role swap, instruction sequence
identical), `Func_8097a7c` + `Func_8097adc` (12/38, a final increment gcc folds
into a store immediate), `Func_80b6e30` (15/34, label and pool structure at the
loop tail), `Func_8092b08` (17/38, register-role swap), `OvlFunc_881_200a7dc`
(32/33, duplicated base pointer).

`OvlFunc_881_200a7dc` carries the precondition that was missing from the
"carry the values the ROM carries" lever: it works only when the values are
actually **distinct**. `Func_80a1bdc` was elevated by adding a second pointer
because its two pointers hold different values; here they are always equal and
no source form yields two registers.
