# Batch 45 — five functions, and a blocker that was not one

Verified from a clean build: `make clean && make compare` → `goldensun.gba: OK`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`
and sits at exactly the address its name claims.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_888_200b270` | `0200b270` | ovl_7892c8 | [ovl_30_c_c_a_a_a_c_c_b.c](../src/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_b.c) |
| `OvlFunc_920_200846c` | `0200846c` | ovl_7a6ae4 | [ovl_30_c_c_a_a_a_a.c](../src/overlays/rom_7a6ae4/ovl_30_c_c_a_a_a_a.c) |
| `OvlFunc_930_2008ff0` | `02008ff0` | ovl_7b7f1c | [ovl_30_c_c_a_c_c_c_c_c_b.c](../src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_c_b.c) |
| `OvlFunc_930_2009028` | `02009028` | ovl_7b7f1c | [ovl_30_c_c_a_c_c_c_c_c_c_b.c](../src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_c_c_b.c) |
| `OvlFunc_939_2008468` | `02008468` | ovl_7c460c | [ovl_314_a_c_a_a_c_a_b.c](../src/overlays/rom_7c460c/ovl_314_a_c_a_a_c_a_b.c) |

## The Makefile was faking a blocker

This is the batch's main result and it is a correction to how this tree
diagnoses failures.

`OvlFunc_930_2008ff0` and `OvlFunc_930_2009028` **byte-match at -O2**. The build
was compiling them at **-O1**, because of this rule:

```make
asm/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c%.o: src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c%.c
	$(GCC296_CC) $(O1_CFLAGS) ...
```

It was written for `ovl_30_c_c_a_c_c_c_c_b.c`. It also captured
`ovl_30_c_c_a_c_c_c_c_c_*` — a **different `.s`**, further down the same split
chain.

**The split chain is not a translation-unit boundary.** Splitting carves one
overlay's assembly into `_a`/`_b`/`_c` pieces *by position*, and an overlay holds
many original translation units. Two pieces sharing a name prefix therefore say
nothing about sharing a compiler invocation. A rule anchored on a prefix spreads
one TU's flag choice onto code that never belonged to it.

The rule is now an explicit target plus a `_b_%` pattern for that stem's own
future children — which is what the pattern was for in the first place.

### Why it was expensive, and what should have caught it

At -O1 the diff is **four lines of argument fill order** — `r0` emitted first
where the ROM emits it last. That is *precisely* the shape of the fill-order
blocker the declaration lever addresses. So the work went where the catalogue
pointed:

- All four declaration combinations were swept (declare/withhold × two callees),
  then the third callee too. No change.
- Four hypotheses about the C were tested **and refuted by experiment**:

  | Hypothesis | Test | Result |
  |---|---|---|
  | the constant `5` shared across both calls | second call's `r0` → 7 | still wrong |
  | first call's `r0` being 0 in the matched twin | first call's `r0` → 0 | still wrong |
  | a constant repeated inside one argument list | made them distinct | still wrong |
  | constant sharing between the two calls | call1 made identical to the twin's | still wrong |

The last test is the one that broke it open. With `call1` byte-identical to
`OvlFunc_888_200b270`'s — a function that **matches** — the same C still came out
different. Identical C cannot compile two ways within one TU, so the difference
had to be *outside* the C.

**The lesson worth carrying:** a blocker diagnosis should be treated as suspect
when a structurally identical twin already matches. That is a much stronger
signal than any single diff, and it was available from the first screen.

`tryc.py` had been printing `(built with: O1)` the entire time. It was correct
about the Makefile; the Makefile was wrong. The line was read as a fact about
the file rather than as a claim to check.

### The tooling change

`tools/tryc.py` now prints a hint **on a mismatch only**, when the non-default
flag set came from a `%` pattern rule rather than an explicit target:

```
     ?? flags ['O1'] came from a WILDCARD rule (…ovl_30_a_c_a_a%.c).
        That rule may belong to a neighbouring TU that only shares a name prefix.
        Re-screen with the default flags before believing this diff:  --cflags "-O2"
```

On a match the flags are right by construction, so it stays quiet there.

One trap in building it, worth recording because it nearly shipped: **the
generic `src/%.c` rule is a wildcard too.** Recording every pattern match made
the hint fire on every mismatch, which is worse than not having it. The pattern
is now recorded only when it actually contributes a non-default flag. Verified
against three files — one O1-via-wildcard (hints), one O1 via the new explicit
rule (silent, correctly — that one is deliberate), one on default flags (silent).

### An audit of the other wildcard rules

All 13 wildcard compile rules were checked for the same over-reach. Three span
more than one parent:

- `overlays/common/common2_c%` — **deliberate and verified.** All 14 functions
  of that TU return `pop {pc}`, unique in the corpus; the Makefile documents it.
- `rom_7ed0a0/ovl_30_a_c_a_a%` and `rom_7ed0a0/ovl_30_c_c_c_a%` — each captures
  a second file from a different `.s`. **Both currently build green**, so nothing
  is mis-compiled today, and they were left alone rather than churned. The
  exposure is future: a new split under either stem inherits -O1 silently. That
  is now exactly what the tryc hint catches.

## The other three

### A join-store whose two arms build the pointer in different orders

`OvlFunc_939_2008468`. Two arms reach a single shared `strb` — the join shape
`OvlFunc_964_2008fe8` introduced in batch 44 — but the arms build the pointer
differently. The taken arm does `add r0, #0x23` and then loads through it; the
other does `mov r3, #1` **first** and adds afterwards.

Writing both arms as `p = ... + 0x23;` is the obvious reading and puts the add
before the 1 in the second arm, swapping two positions. The second arm has to
advance the pointer *after* computing the value.

This is the **second time in two batches** that the fix was to make two arms of
one `if` *less* symmetric than they read. The instinct is to factor the common
subexpression out of both arms; the ROM did not.

Its first `__MapActor_GetActor` is used inline inside the comparison — the
named-variable lever from batch 44.

### The pool tell, applied straight

`OvlFunc_920_200846c` dispatches to one of three per-area setup functions. The
ROM does `ldr r3, =0x31` where `cmp r2, #0x31` would do, three times, so those
operands were **symbols** — and `area.sym` already defined all three.

It reuses the 24-member GetEntrances family's address arithmetic verbatim,
including `off = 0` as a *variable* rather than a literal: Thumb `ldrsh` has no
immediate-offset form, so the zero has to live in a register.

Its `.s` held only this function and no data, so no split was needed.

### Two stack-arg pairs sharing two locals

`OvlFunc_888_200b270` and the two `930` twins are the same shape: two calls that
each pass four register arguments and two stack arguments, with the **same two
locals reused for both pairs** — the ROM builds the second pair into the same
registers, and separate locals cost two positions. In each, the second callee is
deliberately left undeclared because its `r0` is written last (the subtractive
side of the declaration lever).

`tryc.py` warned that `200b270`'s reference file keeps a literal pool inside a
function. That pool belongs to a sibling in the same `.s`; this function has no
pool loads at all, every constant being an eight-bit `mov`. Confirmed by
`make compare`.

## Parked

`OvlFunc_965_200a6fc` at **2 of 29 lines**, same length — an absolute-difference
range check whose *inner* test branches the opposite way from the ROM:

```
rom    cmp r2, r3 / bge <out> / b <call>
ours   cmp r2, r3 / blt <call> / b <out>
```

Getting the **outer** arms right was itself a fix — the first attempt had them
swapped and stood at 9 lines. The ROM's fall-through is the non-negative arm,
which is the branch-polarity lever working exactly as documented. It just does
not reach the inner test.

Three formulations were tried, all giving the identical 2-line diff: plain
if/else; an explicit `goto call;` ending the positive arm; and a fully flat goto
chain with a single shared `out:` label and **no `return` anywhere**, on the
theory that gcc gives return-blocks special placement. gcc-2.96 normalises the
pair in its jump optimiser after the source shape is gone, so no spelling of the
*condition* reaches it.

This is the first function parked purely on inner-test polarity, so there is no
family yet. If a second appears, the two together would say whether the trigger
is the shared epilogue label or the arm being empty apart from its return.
