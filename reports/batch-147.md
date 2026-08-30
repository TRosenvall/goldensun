# Batch 147 — the lever was a missing declaration, and it had been written down

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every
address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `OvlFunc_956_200a4d0` | `0200a4d0` | ovl_7e0928 | [ovl_30_c_c_c_c_c_b.c](../src/overlays/rom_7e0928/ovl_30_c_c_c_c_c_b.c) |
| `OvlFunc_955_20092f0` | `020092f0` | ovl_7ddb88 | [ovl_30_c_c_c_c_b.c](../src/overlays/rom_7ddb88/ovl_30_c_c_c_c_b.c) |
| `OvlFunc_927_2009150` | `02009150` | ovl_7b4558 | [ovl_30_c_c_a_c_a_c_b.c](../src/overlays/rom_7b4558/ovl_30_c_c_a_c_a_c_b.c) |
| `OvlFunc_927_2009244` | `02009244` | ovl_7b4558 | [ovl_30_c_c_a_c_a_c_c_a.c](../src/overlays/rom_7b4558/ovl_30_c_c_a_c_a_c_c_a.c) |
| `OvlFunc_927_2009328` | `02009328` | ovl_7b4558 | [ovl_30_c_c_a_c_a_c_c_b.c](../src/overlays/rom_7b4558/ovl_30_c_c_a_c_a_c_c_b.c) |
| `OvlFunc_952_20085a4` | `020085a4` | ovl_7d768c | [ovl_30_c_a_a_c_c_c_b.c](../src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_b.c) |
| `OvlFunc_971_20091bc` | `020091bc` | ovl_7fb4a8 | [ovl_30_c_c_b.c](../src/overlays/rom_7fb4a8/ovl_30_c_c_b.c) |
| `OvlFunc_971_2009228` | `02009228` | ovl_7fb4a8 | [ovl_30_c_c_c_b.c](../src/overlays/rom_7fb4a8/ovl_30_c_c_c_b.c) |

Three parks deleted: `src/non_matching/overlays/20085a4.c` and
`src/non_matching/overlays/20091bc.c`, both of which had stood for several
batches, and `src/non_matching/ovl_7ddb88/20092f0.c`, which I wrote myself
earlier in this same round and was wrong about.

## The finding

**`mov r0` at the END of a call's argument setup comes from calling the function
through C's implicit `int f()` — no declaration at all.**

That sentence was already in the tree. `OvlFunc_954_20095e0`
(`src/overlays/rom_7db0c8/ovl_30_c_c_c_c_b.c`) records it in its header, in
capitals, with the three callees it applies to and an instruction not to add
prototypes for them. It was written as a fact about that one function. It is a
fact about the compiler, and generalising it closed four functions in one round.

The residue it fixes looks like this, and 69 park notes describe some version
of it:

```
rom   mov r1, r5 / mov r2, #6 / mov r0, r6 / bl OvlFunc_common1_5e4
ours  mov r0, r6 / mov r1, r5 / mov r2, #6 / bl OvlFunc_common1_5e4
```

`OvlFunc_956_200a4d0` was 96 of 96 lines with **three** differing, all of them
that one permutation. Deleting `extern void OvlFunc_common1_5e4(int, int, int);`
made it exact.

**An empty parameter list is not the same lever.** `extern void f();` was
screened on three separate functions here and moved nothing on any of them. The
declaration has to be absent, not merely unprototyped.

### The fix is not always at the call that looks wrong

`OvlFunc_952_20085a4` is the case that matters for applying this. Its park note
named the blocker precisely: `__ActorMessage(0xe, 0)` appears in both arms of a
branch, the ROM emits r0-first in one arm and r1-first in the other, we emitted
r0-first in both. Dropping `__ActorMessage`'s prototype does nothing. Dropping
the prototype of **`__Func_8092c40`, the call immediately before the branch**,
puts both arms right. 3 differing → 1.

So when an argument order is wrong, sweep the neighbouring calls too. On
`200a4d0` and `20092f0` the residue and the fix were at the same call; here they
are one call apart, and no amount of rewriting the call that showed the residue
would have found it.

### What made this a lever and not a lucky guess

`tools/protolever.py`, and a greedy hill-climb built on it: score the candidate,
then try deleting each `extern void` declaration on its own, keep the single
best improvement, and repeat until nothing helps. Run over the 86 saved
candidates still on disk under `scratch/`, it is a few hundred compiles and it
found `20085a4` — whose park note had named the wrong call — without anyone
reading the assembly.

Deleting **all** prototypes at once is usually worse, not better: of the 62
saved candidates with a reference, deleting every declaration made **37 worse,
21 unchanged, and 4 better** — and six of the 37 stopped compiling, because a
callee whose result is used changes type when its declaration goes. The lever is
per-call-site, and the sweeper only touches `extern void` declarations for that
reason.

### The declaration is a PER-CALL-SITE choice, not a per-callee one

`OvlFunc_971_20091bc` is where the lever finishes generalising. It calls
`__CloseUIBox(h, 1)` twice with identical arguments, and the ROM emits
`mov r0, r5 / mov r1, #1` at both. gcc, given one `extern int __CloseUIBox(int,
int);`, emits the ROM's order at the second site and the reverse at the first.
Its park called that the blocker and was right that neither the interleave lever
nor the no-prototype lever reaches it: **both sites see the same declaration, so
any change to it moves both together, and only one of them is wrong.**

What differs between the two sites is that the first call's result is discarded
and the second is returned. So give the first site its own declaration:

```c
extern int  __CloseUIBox(int h, int n);
extern void CloseBoxV(int h, int n) __asm__("__CloseUIBox");
```

The first call goes through `CloseBoxV`, the second keeps the `int` prototype
and is returned. Both orders come out right and the function matches exactly,
as does its twin `OvlFunc_971_2009228` — same body, two message ids apart.

Screened and rejected, all still 2 differing: storing the first call's result,
an alias with an empty parameter list instead of a void return, and the alias
applied to the returned site instead. Deleting `__CloseUIBox`'s declaration
outright also matches — but only because that makes *both* sites unprototyped,
which is the same fix by accident and would not survive a third call site.

The rule to carry forward: **when one site of a repeated call is right and
another is wrong, the answer is two declarations, not one different one.** The
`__asm__("name")` alias costs nothing and emits nothing.

## I parked a function this round and was wrong within the hour

`OvlFunc_955_20092f0` sat at 123 of 123 lines, 15 differing, the instruction
multiset exact, every difference the same one-slot displacement of `mov r0` at
six call sites. Seven spellings of those calls left the count **exactly**
unchanged: the constants hoisted, the constants inline, a shared zero variable,
per-call-site locals, the decrement folded into the argument, an empty parameter
list. I read that invariance as proof the residue lived below the source, wrote
a park note saying so, and moved on.

Invariance under changes to the call site is evidence about **where** the lever
is. It is not evidence that there is none. The park note is now the header of
the elevated file, kept as a correction.

Two levers were needed at once, and they pull in opposite directions:

  * **`mov r0` INTO THE MIDDLE** of another argument's `mov`/`lsl` pair — name
    that constant in a block that **dominates** the call, so gcc
    rematerialises it at the use. Took 18 differing to 15.
  * **`mov r0` AT THE END** — delete the prototype. 15 → exact.

`OvlFunc_954_20095e0` needed both as well and says so. Read which one each site
wants before reaching for either.

### The limit of the first lever, measured

The dominating-block naming only works when there **is** a dominating block. On
two straight-line functions — `OvlFunc_967_2008308` and `OvlFunc_911_20082b4`,
both wanting `mov r0` in the middle — naming the constant at the top of the
function instead lengthens a live range across a call, gcc allocates a
callee-saved register rather than rematerialising, and the function grows a
push/pop pair. `2008308` goes 60 differing → 78; `20082b4` goes 33 lines → 39.
Both remain parked and both notes now say so.

## Two tools were quietly lying

**`tools/pickable.py` was re-serving parked functions.** It ranks on the
assembly, so a function tried and parked in an earlier round scores exactly as
well as a fresh one and floats straight back to the top. Two of this round's
first three picks were parks written in earlier batches — I re-derived
`OvlFunc_967_2008308` from scratch before finding its note. It now drops any
address already under `src/non_matching`: 102 candidates → 64 real ones.

**`tools/tryc.py` took the union of every matching Makefile rule's flags.** make
gives an explicit target rule precedence over every pattern rule; tryc did not
model that. All three `OvlFunc_927` functions have split-derived names that fall
inside an `ovl_30_c_c_a_c_a%` **-O1** wildcard belonging to an unrelated stem in
the same directory, and at -O1 the first of them is 98 lines with 40 differing.
Each got an explicit -O2 rule — and tryc kept reporting the -O1 diff afterwards,
because it was still unioning in the wildcard. Its own `?? flags came from a
WILDCARD rule` hint was right both times. **Believe that hint before believing
the diff.** With the precedence fixed, all three matched on the first screen.

## Corrections to existing park notes

**`OvlFunc_955_2009424`: the fourth register is `-1`, not the gState base.** The
park blamed naming the `gState` base for forcing a fourth callee-saved register
and a `mov r7, r8 / push {r7}`. With the base inlined the push is still there.
The real occupant is `-1`: the function calls `__Func_80933f8` twice, once with
a single `-1` and once as `__Func_80933f8(-1, -1, -1, 0)`, and gcc
common-subexpressions four uses of a two-instruction constant into one pseudo
that is live across `__Func_80933d4`. That puts it in the
`src/non_matching/overlays/constant_reuse.c` class, where every function in the
tree containing the `-1` triple is parked and none is elevated. The prototype
lever still helped it (78 differing → 74) and should be kept in any retry.

Naming the base **does** fix the address fold — `ldr r3, =gState / mov r2, #0xe1
/ lsl r2, #1 / add r3, r2` instead of a folded `ldr r3, =gState+450`. It only
looks expensive because the `-1` pseudo has already spent the budget.

**`OvlFunc_967_2008308`: 60 differing is not sixty problems.** The ROM emits
`b L0 / L0:` to jump over an early literal pool and we put the pool at the end,
which shifts every later line. The whole function is right except the
`__MapActor_Emote` interleave and the pool dump point.

## `_MSG_2352` added to message.sym

`20085a4`'s park had already worked out that the message base has to be a
**symbol** — the ROM holds `0x2352` in r5 for the whole cutscene and derives the
other two ids with `add r0, r5, #2` and `add r0, r5, #3`, and written as a plain
`int` gcc pools all three independently and never spends the register, because
rematerialising a pool constant is cheaper than a push/pop pair. What was
missing is that `_MSG_2352` was never added to `message.sym`, so every screen
since had been run with the symbol unresolved and the note's own "really 2, not
3" was never cashed. One line, no bytes emitted.

## Numbers

3267 elevated / 2150 remaining / 380 parked.
