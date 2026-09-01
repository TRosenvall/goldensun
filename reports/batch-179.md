# Batch 179

Five functions and four parks. Every one of the five was found by the selector
batch 178 produced, and the batch's result is that **the selector cuts both
ways** — which the first version of the rule did not say.

## Function breakdown

| # | function | address | file | screens | what it took |
|---|---|---|---|---|---|
| 1 | `Func_80b06ec` | `0x080b06ec` | [rom_b0070_a_a_c_a_c_c_c_a_b.c](src/rom_b0000/rom_b0070_a_a_c_a_c_c_c_a_b.c) | **1** | remove the name — four CSEd reads |
| 2 | `Func_80bf54c` | `0x080bf54c` | [rom_bbb0c_a_c_c_a_b.c](src/rom_b5000/rom_bbb0c_a_c_c_a_b.c) | **1** | remove the name — the twelfth TickStatusCounter |
| 3 | `Func_801ce48` | `0x0801ce48` | [rom_1ca1c_a_a_c_c_b.c](src/rom_15000/rom_1ca1c_a_a_c_c_b.c) | 2 | double read, then the halfword-store pooling fix |
| 4 | `Func_808ddb8` | `0x0808ddb8` | [rom_8d9a4_a_a_b.c](src/rom_8a000/rom_8d9a4_a_a_b.c) | 2 | **add** a name — one read used twice |
| 5 | `Func_8092ba8` | `0x08092ba8` | [rom_92950_c_a_c_a_a.c](src/rom_8a000/rom_92950_c_a_c_a_a.c) | 4 | named offset; sentinel hoisted above the base |

All five verified in the linked ELF with a `.gcc2_compiled.` symbol at the
address after a clean `make clean && make -j8 && make compare`.

## THE READ-COUNT LEVER RUNS IN BOTH DIRECTIONS

Batch 178 said: a body that is one instruction short, with a `mov rA, rB` in the
ROM's version of the missing line, is a CSEd reload — **remove a name.** That is
right and it landed two more functions here on the first screen.

`Func_808ddb8` is the same rule with the sign flipped. It walks a `-1`
terminated table of key/value pairs, and the ROM loads each entry ONCE and uses
it twice — `ldrsh r3, [r2, r4]` for the sentinel test, then `cmp r0, r3` for the
key. Written with two reads through the pointer, gcc cannot prove the second
names the same object and **reloads it through a recomputed address**:

```
    ours   sub r3, r2, #0x2 / mov r4, #0x0 / ldrsh r3, [r3, r4]    29 lines
    rom    (nothing — r3 still holds it)                           26 lines
```

Reading the entry once into a named local and testing that local twice matched
on the first screen.

> **The diagnostic is which way the extra instruction points.** An extra COPY in
> the ROM means the source read twice and we named it — remove the name. An
> extra LOAD in OURS means the source read once and we did not — add one.

Batch 178's "naming a value is not free" stands; this is its other half. Naming
buys a statement boundary and costs a repeated read, so the question is never
"should this be named" but "how many times does the ROM touch memory".

## A NAMED ZERO CAN CSE WITH A CALL ARGUMENT

`Func_80a7440` is parked on the collision between the two halves.

Its store `*(u16 *)(s + 0x174) = 0` gets the zero POOLED — `ldr r3, =0x0` where
the ROM has `mov r3, #0x0` — which is the recorded halfword-store pooling rule,
whose recorded fix is to name the value. That fix works on `Func_801ce48` in
this same batch, taking it from one differing line to a match.

Here it backfires. Two lines later the function calls `Func_80a77a4(0)`, and
once the stored zero has a name gcc CSEs the two zeroes into one register and
drops two instructions — 21 lines against the ROM's 23. Two separate names do
not help; gcc CSEs on the value, not the spelling.

> Before naming a stored constant, look for the same constant in a nearby
> argument list. The lever and the CSE are the same mechanism seen from two
> sides.

## THE SENTINEL'S LIVE RANGE DECIDES WHETHER IT GETS r0

`Func_8092ba8` returns `-1` or a looked-up value. Written with `v = -1` beside
its use, gcc keeps the result in r0 for the whole body and returns it directly —
one instruction SHORT of the ROM's `mov r0, r1` at the join. Moving that one
assignment to the top of the function, above the base-pointer load, overlaps the
two live ranges so they cannot share r0, and the copy reappears. That was the
match.

Renaming the variable and declaring it first does **not** do it: 24 lines and 6
differing, unchanged. It is the assignment point that matters, not the
declaration — the same distinction batch 176 drew for two constants in different
callee-saved registers.

## Parks

Four. Two are new:

| function | best | class |
|---|---|---|
| `Func_80a7440` | 23/23, **16** | named zero CSEs with a call argument |
| `MapActor_WaitAnim` | 26 vs 29, 19 | loop guard hoisted — ours is three short |

`MapActor_WaitAnim` is the mirror of batch 177's branch-over-pool correction:
there a `b` to the next label was ours and not the ROM's, here it is the ROM's
and not ours. The ROM tests the frame counter before the body, reaching the test
with a branch over the increment; gcc rotates the loop so the guard sits at the
bottom and the branch disappears. Three spellings tried, all 26 lines.

## State

1,906 functions remain in `asm/`. `tools/cse_reload.py` still lists 89 parked
functions carrying the load-then-copy shape, and the two-directional reading of
it means the ones where OUR version is long are now candidates too.
