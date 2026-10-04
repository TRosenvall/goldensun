# What pokefirered did instead of pinning

A comparison against `pokefirered` (pret's Pokémon FireRed decompilation, 3,805
commits back to 2017), run to answer: how many matching artifacts does a mature,
human-written decompilation carry, and what did they write where we write a shim?

## The headline: they have essentially none

| | pokefirered | goldensun |
|---|---|---|
| matching `.c` files | 282 | 4,426 |
| approx functions | ~12,000 | ~3,300+ |
| **`register ... asm("rN")` pins** | **0** | **2,650** |
| `asm` statements | **21** | 4,601 |
| files mentioning `volatile` | 6 | 340 |

**Zero register pins across roughly twelve thousand functions.** And all 21 of
their `asm` statements are genuine hardware, not matching aids:

    asm("swi 0x2A");          BIOS call            (m4a.c)
    asm(".hword 0xEFFF");     AGB debug-print magic (isagbprn.c, x3)
    asm("svc 2");             HALT                 (script.c)
    asm("mov r2, pc");        read PC for multiboot (multiboot.c)

So the pin technique is **not necessary** to match a commercial GBA game. That is
the single most important fact in this report.

## CORRECTION: THE COMPILER IS NOT A STRUCTURAL DIFFERENCE.  I WAS WRONG.

The first version of this report listed "different compiler, and theirs is the
real one" as one of two structural reasons for the pin gap, and called gcc-2.96
"a *reconstruction* of what Camelot used".  **That is false, and it mattered,
because it pointed the conclusion the wrong way.**

gcc-2.96 is not a guess.  It is **patched gcc-2.96, arm-elf, Debian 2000-07-31
dev snapshot** -- the branch between FSF gcc-2.95 and gcc-3.0 -- identified
exactly, vendored in `camelot-gcc`, and it **reproduces the full Golden Sun ROM
byte-identically** (SHA1 `5c4695205413df7db52b9a184815a07783999971`).  4,426
functions in this tree match through it.  See `docs/attribution.md`:
FutureFractal identified the compiler generation, Tarpman documented the codegen
fingerprints in 2021, Karathan published the working flag set, and this project
had independently narrowed it to "GCC-family, later than agbcc's 2.9" before
finding that work.

A wrong compiler does not match 4,426 functions byte-exactly.  It produces
systematically different code everywhere.  **Both projects have the real
compiler.**

### Which makes the remaining explanation weaker, not stronger

The only structural difference left is **translation-unit size** -- their ~43
functions per file against our mostly one.  And on inspection that does not
explain register pins either: **gcc-2.96 does no cross-function optimisation at
`-O2`**, so compiling a function alone gives the same register allocation as
compiling it among neighbours.  What TU composition *does* change is the literal
pool, and data and symbol ordering -- which is why `Func_80b09fc` is enrolled
unmatchable as a "tu-pool" artifact, and why pool-order work keeps appearing in
this tree.  Pool placement is not what a register pin fixes.

> **So the honest conclusion is the opposite of what I first wrote: with the
> compiler identified and validated, and TU size not reaching register
> allocation, our 2,650 pins are almost entirely SOURCE-SHAPE ARTIFACTS.** That
> makes the four patterns below more promising, not less -- they are aimed at
> exactly the thing that is actually wrong.

## The gold: their `[LEAK-INFORMED] fix ... fakematch` commits

pokefirered had fakematches too. For some functions they later obtained the
original source and **replaced the shim with what the human actually wrote**. Those
diffs are the closest thing available to ground truth on "what would a human have
written", and all four follow one theme.

### 1. Split a compound condition — this replaced a REGISTER PIN

`sub_8113AE8`, commit `b280105f5`:

```diff
-#ifndef NONMATCHING
-    register const u16 *r0 asm("r0") = a0;
-#else
     const u16 *r0 = a0;
-#endif
-    if (r0 == NULL || r0[1] > sQuestLogCursor)
+    if (a0 == NULL) // checks must be separate to match
+        return FALSE;
+    if (r0[1] > sQuestLogCursor)
         return FALSE;
```

The original author wrote **two sequential `if`s**, not one `||`. Short-circuit
codegen for `a || b` differs from two separate early-outs, and the pin had been
papering over exactly that difference. Their own comment — *"checks must be
separate to match"* — is the lever stated outright.

### 2. Narrow the pointer type and put `volatile` AT THE USE SITE

`sub_812E768`, commit `0f1acd595`, replaced `asm("":::"r4")`:

```diff
-    const u8 *pixelsSrc;
-    u16 *pixelsDst;
+    u8 *pixelsSrc;
+    u8 *pixelsDst;
...
-            #ifndef NONMATCHING
-                asm("":::"r4");
-            #endif
-            if ((uintptr_t)pixelsDst & 0x1)
+            if ((u32)pixelsDst & 0x1)
-                pixelsDst = (void *)pixelsDst - 1;
+                pixelsDst--;
-                    toOrr = *pixelsDst & 0x0fff;
+                    toOrr = *(vu16 *)pixelsDst & 0x0fff;
-                        *pixelsDst = toOrr | ((*pixelsSrc & 0xf0) << 8);
+                        toOrr |= ((*pixelsSrc & 0xf0) << 8);
```

Four things at once, and each is a transferable lever:
- the pointer is declared **`u8 *`**, the narrow type, not `u16 *`;
- the wide access is a **cast at the dereference**: `*(vu16 *)pixelsDst`;
- `pixelsDst--` instead of `(void *)pixelsDst - 1` — natural once the type is right;
- **accumulate into a local and store once**, instead of storing in each arm.

> **`volatile` AS A CAST AT THE ACCESS, NOT ON THE DECLARATION.** This matters
> directly to us. `src/overlays/rom_7f21b8/ovl_30_a.c` records that `volatile` on
> the local "produces the RIGHT ORDERING ... not usable because it also forces a
> stack slot", and concludes that what is wanted is a *register-level volatile* —
> which is why it ships an `__asm__ volatile` barrier. `*(vu16 *)p` is a volatile
> **access** without a volatile **object**, so it creates no stack slot. That
> is a candidate cure for a whole class of our barriers and it has not been tried.

### 3. One expression with a pointer difference, not hand-unrolled arithmetic

`battle_interface`, commit `ca7134316`:

```diff
-    xPos = (u32) ConvertIntToDecimalStringN(text + 2, lvl, ...);
-    // Alright, that part was unmatchable. It's basically doing:
-    // xPos = 5 * (3 - (u32)(&text[2]));
-    xPos--;  xPos--;  xPos -= ((u32)(text));
-    var1 = (3 - xPos);  xPos = 4 * var1;  xPos += var1;
+    objVram = ConvertIntToDecimalStringN(text + 2, lvl, ...);
+    xPos = 5 * (3 - (objVram - (text + 2)));
```

The artifact was gcc's own strength-reduced output (`4*v + v` for `5*v`)
transcribed back into the source. The human wrote one expression, used **pointer
difference** instead of casting to `u32`, and **reused a variable needed later**
(`objVram`) to hold the intermediate.

### 4. Declare the extern with its real type — the arithmetic then writes itself

`CreateShedinja`, commit `e80a68327`:

```diff
+extern struct Evolution gEvolutionTable[][EVOS_PER_MON];
...
-        const struct Evolution *evos;
-        const struct Evolution *evos2;
-        // can't match it otherwise, ehh
-        evos2 = gEvolutionTable[0];
-        evos = evos2 + EVOS_PER_MON * preEvoSpecies;
```

The two helper pointers existed **only because the declaration was wrong.** Give
the extern its inner dimension and the index arithmetic gcc wants falls out of
normal subscripting.

## THE UNIFYING THEME

> **In all four cases the artifact appeared where a TYPE or a DECLARATION was
> wrong, and the fix was to correct the type — never to add a shim.** The shim was
> compensating for address arithmetic, a width, or a short-circuit that the
> compiler would have produced on its own from a correctly-typed source.

That reframes our 2,650 pins. A pin is evidence about a *declaration*, not only
about an allocator.

## The opportunity here, measured

Of goldensun's **533 pinned files**:

| signature | files | the pokefirered pattern it matches |
|---|---|---|
| raw-offset arithmetic `*(T *)(p + N)` | **272 (51%)** | 4 — wrong/absent struct type |
| `extern T name[];` with no dimensions | **198 (37%)** | 4 — missing array dimension |
| a pointer cast to `(u32)`/`(int)` | 106 (20%) | 3 — should be pointer difference |
| a compound `if (A \|\| B)` | 72 (14%) | 1 — split into two `if`s |

Those overlap, but the shape is clear: **half of our pinned files are doing
address arithmetic by hand**, which is pattern 4's exact signature.

## Tested here, and it transfers

`src/overlays/rom_77dd1c/ovl_30_c_c_c_a_a_c_b.c` carries three shims. Measured
against its own tracked `.s`:

| variant | differing lines |
|---|---|
| **pin removed** | **0 — inert** |
| `rq` barrier removed | 4 |
| `f` barrier removed | 8 |
| all three removed | 10 |

So that is a **14th free depin**, and it was missed by batch 320's sweep only
because that sweep used an arbitrary 22-line cutoff and this file is 27. **The
free-pin population is larger than 13; re-run the sweep without the cutoff.**

## Recommended order of work

1. **Re-run the depin sweep with no line limit**, across all 617 fakematch rows.
   Free pins cost nothing to remove and there are demonstrably more than found.
2. **Pattern 4 first, on the 198 dimensionless `extern` arrays.** It is the
   largest bucket, the cheapest to try, and it improves readability whether or not
   it removes a shim — a correctly-typed extern is better code regardless.
3. **Try `*(volatile T *)p` against the barrier class.** Specifically on the
   files whose headers already say "volatile gives the right ordering but forces a
   stack slot" — that reasoning stops applying to a volatile *cast*.
4. **Pattern 1 on the 72 compound conditions**, with the caveat that it only
   applies where the branches are early-outs.
5. Keep pattern 3 for review passes — it is about readability more than matching.

## A caution on what this comparison does NOT show

It does not show that zero pins is reachable here, and it does not show that
every pin is removable.  What it shows is narrower and still strong: a mature
human-written decompilation of a commercial GBA game carries **none** of this
construct; one in four of our small pinned files is **inert on removal**; and
four specific constructs the original authors used are where we reached for a
shim instead.

It also does not show that our *flags* are right for every object.  Nine objects
already carry per-file flag groups (`GCSE_CFLAGS`, `CSE_CFLAGS`) because the
original build did not use one flag set everywhere.  That is ordinary, both
projects do it, and it is a per-file discovery problem rather than a doubt about
the compiler.
