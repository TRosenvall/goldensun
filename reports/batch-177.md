# Batch 177

Six functions and twenty-two parks, over nine rounds. The elevation count is
the batch's least interesting number. Its subject is that **two of this tree's
standing rejects turned out to be wrong**, between them holding 610 functions
out of every ranking, and that the second one was found by the method the first
one taught.

## Function breakdown

| # | function | address | file | screens | what it took |
|---|---|---|---|---|---|
| 1 | `OvlFunc_974_20088c4` | `0x020088c4` | [ovl_30_c_c_a_c_a_c_a_b.c](src/overlays/rom_7fcd20/ovl_30_c_c_a_c_a_c_a_b.c) | 2 | **the callees' return type** — one change, 53 call sites |
| 2 | `Func_80a1814` | `0x080a1814` | [rom_a1814_a_a_a_b.c](src/rom_a1000/rom_a1814_a_a_a_b.c) | 3 | derivation as its own statement (`n -= 0xff`) |
| 3 | `Func_800d924` | `0x0800d924` | [rom_d924_a_a_b.c](src/rom_9000/rom_d924_a_a_b.c) | **1** | nothing — r8 came out unprompted |
| 4 | `Func_800d98c` | `0x0800d98c` | [rom_d924_a_a_c.c](src/rom_9000/rom_d924_a_a_c.c) | **1** | nothing — twin of #3 |
| 5 | `GetEquippedItem` | `0x080787dc` | [rom_78414_c_c_a_c_a_b.c](src/rom_77000/rom_78414_c_c_a_c_a_b.c) | **1** | nothing — the lever came from a **park** |
| 6 | `Func_807882c` | `0x0807882c` | [rom_78414_c_c_a_c_a_c_b.c](src/rom_77000/rom_78414_c_c_a_c_a_c_b.c) | 5 | pointer-typed operand; init source order |

All six verified in the linked ELF with a `.gcc2_compiled.` symbol at the
address after a clean `make clean && make -j8 && make compare`.

Three of the six took one screen. That is not luck — it is what happens when
the candidate pool is chosen correctly, and choosing it correctly is what this
batch was actually about.

## TWO REJECTS WERE WRONG, AND THE SECOND WAS FOUND BY THE FIRST'S METHOD

**Reject one: branch-over-pool.** Every ranking skipped a function whose body
contains a literal pool, on the premise that the compiler never emits a mid-body
pool and so a mid-body pool proves hand-written asm. The premise is true — *of
old_agbcc*. This tree builds with gcc-2.96, which emits them freely. The
`poolblocked.py` docstring had the compiler name in it and nobody read it
against the rule it was supporting. **501 functions.**

**Reject two: r8–r11.** Every ranking skipped a function mentioning r8–r11
outside its push/pop, on the premise that high registers mean an allocation
fight nothing in the source reaches. But `-fcall-used-r4` takes r4 out of the
callee-saved set, so a function with four values live across a call has only r5,
r6, r7 — and the fourth goes to r8, with exactly the `mov r7, r8 / push {r7}`
prologue the reject was reading as a wall. **109 functions.**

The falsifying evidence for reject two was sitting in our own output: *our
screens have been emitting r8 and r10 for rounds.* The reject was never compared
against the thing it claimed was impossible.

Relaxing it to a reported column left 122 candidates where the full reject left
twelve. Three of the first four tried matched (#2, #3, #4 above), and in all
three the r8 came out **with no lever at all**.

> A heuristic that was never wrong is not the same as a certainty. Both of these
> started as "avoid a fight you cannot win," which is good triage advice, and
> hardened into "this cannot be done," which was false. **When a reject has
> never been retested, the cheapest thing in this project is to retest it** — it
> costs one screen and it returned 610 functions.

`r12` and `r14` holding a value remain real walls; the reject is kept for those
two and only those.

## THE CALLEE'S RETURN TYPE FIXES EVERY CALL SITE AT ONCE

`OvlFunc_974_20088c4` is a 232-line debug-setup script: 53 calls to two
functions, each with three constant arguments. It screened at the ROM's **exact
length with 159 of 232 lines differing** — every call, the same way, the ROM
setting r0 last and gcc setting it first.

Declaring the two callees `int` instead of `void` — nothing else changed, and
neither result used — matched all 53.

gcc reserves r0 for a value-returning call and evaluates that argument last; for
a `void` call r0 is just another argument register and goes first. The notebook
already had the *presence* of a prototype as a lever. The **return type is a
separate and stronger knob**, and it is the same fact as the epilogue tell read
from the other side: r0 is reserved whenever a return value exists, from the
declared type alone.

> When every call in a function has its arguments in the wrong order, read the
> return types before anything else. It is one edit and it is total.

## A PARK IS A RECORD OF WHAT WORKED, NOT ONLY OF WHAT FAILED

`GetEquippedItem` and `Func_807882c` are two of three near-identical scans over
a unit's 15-slot item array. Both matched on the pointer-typed-operand lever —
give the walking *offset* the pointer type and hold the base as a plain `int`,
and the ROM's `ldrh r3, [r5, r7]` base/index roles come out right.

Neither has an elevated sibling that demonstrates it. The lever was written down
in [`8078588.c`](src/non_matching/rom_77000/8078588.c) — a **park**, in the same
address range, which found the register-offset form, used it, and then failed on
an unrelated allocation residue.

`family_siblings.py` ranks by how much of a family is already *elevated*, on the
sound theory that a matched sibling shows which symbol to spell and which of
several equivalent forms to use. It was blind to the other half of the family's
record.

> A park that fails on residue B is often the only record of working lever A.
> Read the parks nearest a candidate's address, not only its solved siblings.

The tool now prints a `parks near` column beside its `read:` lines.

## TWO NEW LEVERS, BOTH SMALL, BOTH CHEAP

**Two plain local inits are emitted in source order.** When two locals are
initialised to constants in one basic block with no dependence between them, gcc
emits the two `mov`s in the order written. Swapping `off = 0xd8; i = 0;` to
`i = 0; off = 0xd8;` took `Func_807882c` from 4 differing of 35 to 2.

This is *not* the argument-block rule, which says the opposite: inside a call's
argument list gcc emits every pooled load before any `mov`, whatever the source
order. Same words, opposite answers, different scopes — worth keeping straight.

**A byte-sized zero can be served by a register whose low byte is already zero.**
`Func_809a3c4` holds `0xc0 << 10` for two word stores, then does
`*(char *)(p + 0x5a) = 0`. gcc reused that register for the byte store and
emitted nothing, leaving the body **one line short** of the ROM — which reads as
a missing statement and is not. A named local restored the separate `mov` and
took it to exact length.

> Read a one-line-short body at a `strb`/`strh` as a possible subword reuse
> before hunting for a missing statement. Word stores cannot do this; byte and
> halfword stores can.

## THE LEVER IS THE STATEMENT BOUNDARY, NOT THE NAME

`Func_80a1814` derives `0xff` off a live `0xfe`. Measured:

```
r[0xf] = 0xfe;   t[0xf] = 0xfe - 0xff;          43 of 44, 16 differing
n = 0xfe;  r[0xf] = n;  t[0xf] = n - 0xff;      44 of 44,  1 differing
n = 0xfe;  r[0xf] = n;  n -= 0xff;  t[0xf] = n; MATCHES
```

`n - 0xff` inside the store is folded and materialised with a `mov`. `n -= 0xff`
as its own statement survives as a `sub` on the register the previous store just
used. Naming the value gets you to one differing; **giving the derivation its
own statement is what closes it.**

## THE BASIC-BLOCK LEVER IS BOUNDED BY THE REGISTER FILE

Batch 176 broke the arg-interleave wall by assigning four constants in a
dominating block. `OvlFunc_932_200a6c0` needs eight, and gcc spills all of them:
136 lines / 28 differing became **144 / 142**.

The lever's cost is a live range, and a `push {lr}` function has nowhere to put
more than two or three. Apply it to the constants the diff names, never to every
constant in the function.

## Parks

Twenty-two. The recurring classes, unchanged and unbroken:

| class | specimens this batch | status |
|---|---|---|
| duplicate-constant CSE | several | 13 flags ruled out; cse.c local CSE, no flag reaches it |
| stack-argument materialisation | `OvlFunc_932_200a6c0` and others | the ROM builds both values before storing either; the one site where the two values *coincide* matches |
| allocation-order rotation | `8020150`, `80b6a60`, `8078870` | the high register comes out right; which of the rest gets it does not |
| copy-into-a-register / three-operand shift | `20092a4` | second specimen of a recorded negative — the named-intermediate lever needs both values simultaneously live |
| loop-invariant constant not hoisted | `809a3c4` | gcc prices `mov`+`lsl` below a pool load for `0x2000`; the ROM priced it the other way |

`OvlFunc_917_20092f4` is worth reading for what came *free*: two jump tables
from two plain switches, and every cross-jumped tail including two
fall-throughs — **provided the cases are ordered by their label addresses in the
ROM rather than numerically.** It parks at 230 of 230 with four differing, two
identical calls in one switch arm with their argument `mov`s transposed, while
the identical call in a *different arm of the same switch* matches. The ROM
emits both orders itself; no source property distinguishes them.

## State

1,923 functions remain in `asm/`. The two reject corrections moved 610 of them
from invisible to rankable, which is the batch's durable result — the six
elevations are a sample of that pool, not the point of it.
