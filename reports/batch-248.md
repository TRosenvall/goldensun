# Batch 248 — three whole files, and a pin that lied about converging

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_932_2009770` | `0x02009770` | [ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.c) |
| 2 | `OvlFunc_932_2009838` | `0x02009838` | [ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.c) |
| 3 | `OvlFunc_932_2009d0c` | `0x02009d0c` | [ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.c](src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.c) |
| 4 | `OvlFunc_968_200a6f8` | `0x0200a6f8` | [ovl_30_c_c_a_a_c_a_a.c](src/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_a.c) |
| 5 | `OvlFunc_968_200a90c` | `0x0200a90c` | [ovl_30_c_c_a_a_c_a_a.c](src/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_a.c) |
| 6 | `OvlFunc_968_200ab14` | `0x0200ab14` | [ovl_30_c_c_a_a_c_a_a.c](src/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_a.c) |

## Parked

| function | address | blocker | floor |
|---|---|---|---|
| `OvlFunc_968_200a47c` | `0x0200a47c` | halfword store poisoning a later narrow constant across a call | **11 of 273**, exact size, count and relocations |
| `OvlFunc_968_2009d48` | `0x02009d48` | interference-graph colouring: which of `p`/`r` takes the high register | **2 encodings** once the spill is forced |

Both parks are diagnoses with reproducible floors, not abandonments. Neither
says "allocation looks different".

**Two files landed WHOLE** — one `.c`, no split, no linker edit, no flag rule.
Counting batch 247's, that is three whole-file landings in a row, and it is now
the cheapest shape on the board by a wide margin. One of the six
(`200a90c`) screened **exact on the first try** from its finished twin.

## THE PIN THAT LIED ABOUT CONVERGING

The finding worth the batch. The recorded pin hazard is a pin whose live range
crosses a call being **deleted**, with the tell *"one instruction SHORT and an
argument register never written"*. This is a different failure, and **neither
tell fires.**

`register unsigned char *p __asm__("r7")` on a pointer live across the whole
function was honoured at its definition (`add r7, r3, r1`) — and the register
was then handed to a **second pseudo**. gcc emitted `add r7, sp, #0x10` for
`&s` mid-function, so every later `*(int *)(p + 8)` read the stack struct
instead of the actor.

| | |
|---|---|
| length | 226 against the ROM's 226 — **exact** |
| differing | 123 → **78**, i.e. it *improved* |

It reads as convergence. It is a miscompilation.

> **After any pin on a callee-saved register, read the generated `.s` for a
> SECOND definition of that register before believing the number.**

The shipped file carries no r7 pin.

## STATEMENT ORDER AND THE PIN SET ARE ONE SEARCH, NOT TWO

On `2009838` (1236 bytes, 489 encodings) the 120-permutation store-order sweep
was run twice, against different pin sets:

| base | natural order | best permutation |
|---|---|---|
| full 59-pin set | 84 | 79, and it puts the ROM's r6/r8 tenants in the **wrong registers** |
| minimal 30-pin set | **2** | — |

The full-set winner falls to 23 under the minimal set. So a sweep run **before**
the strip selects a wrong source order, that order **survives the strip**, and
the function then reads as a structural blocker.

> **Order: pin, strip, then permute.**

## EVICTION PINS AS A SET, TWICE MORE, AND A NEW BOUND

| function | pin A alone | pin B alone | both |
|---|---|---|---|
| `2009770` | 53 | — | **0** (two `__Func_8012330` sites) |
| `200a6f8` | `u`→r6: 117 (worse) | `v`→r2: 8 (inert) | **6** |

And a bound that was not previously stated:

> **A pin set is minimal only with respect to the BASE it was minimised on.**

On `200ab14` the `e4`/`f4` pins were worth **0** on the locals-only base and
**19** on the inline-derived base. Same pins, same function, different base.

## A FOURTH LOOP SHAPE

Not in the recorded taxonomy. On `2009d0c`:

| shape | differing |
|---|---|
| `while (c) { …; if (x) break; }` | 67, and four instructions **short** |
| `goto` | 75 |
| `if (c) do { … if (x) break; } while (c);` | **exact** |

The duplicated guard plus bottom test is a real, separate shape.

## THE POOLED-ZERO-IS-A-SYMBOL TELL HAS A HIGH-REGISTER EXCEPTION

The recorded entry reads `ldr r2, =0` feeding byte stores as proof of a symbol,
because a literal *"emits `mov r2, #0`"*. Both cutscenes in `rom_7b9cb4` emit
`ldr r3, =0x0 / mov r10, r3` from a plain `p->f5b = 0;` and match byte-for-byte:
**`mov r8-r11, #imm` is not a Thumb encoding**, so gcc reaches a high register
through the pool.

The file carries its own control — `2009770` stores the same field from the same
literal into a **low** register and gets `mov r3, #0` with no pool.

> Discriminator: is the pool load immediately followed by `mov r8-r11, rLOW`?

## THE HALFWORD-STORE POISON (park 1)

A genuine floor, and not the recorded halfword-pooling entry — that one is about
constants `mov #imm8` cannot build, and **this constant is zero**. gcc commons a
later `g554()[0x55] = 0` with an earlier `b->f64 = 0` *halfword* store and
rematerialises it in HImode from a mid-function pool: +8 bytes, +3 encodings,
where the ROM has `mov r3, #0`.

| condition | pools? |
|---|---|
| halfword store **before** the call | yes |
| halfword store after the call | no |
| halfword **load** rather than store | no |
| later constant a different value | no |
| halfword store of a **non-constant** | **yes** |

That last row is the decisive one: **it is the HImode `strh` instruction that
poisons the later constant, not an HImode literal.** The tell is structural and
invisible to any search for pooled halfword values. `int` locals, `unsigned
char` locals, casts, store reordering, bitfields and separate pointer chains all
still pool.

## `for`+`break` DOES NOT ALWAYS ROTATE

The recorded entry says the `for` shape rotates with no `goto` spelling, and
that a hand-written `goto` is worse. Both can hold while the same source shape
goes both ways: a landed neighbour's loop rotates, and a textually equivalent
one in `200a47c` does not — it emits the body **inline** between the test and
the latch.

Caching the pointer, shrinking the body, the outer bound and the surrounding
branches are all **inert**. Writing the body as an out-of-line block reached by
`goto`, placed where the ROM puts it, took 174 → 155 and is worth 44 on removal.

> The tell is not the loop keyword, it is **where the body sits**.

The `duplicate_loop_exit_test` mechanism is recorded as an **untested
hypothesis**, deliberately.

## A LANDING INSTRUCTION THAT WOULD HAVE SILENTLY BROKEN THE ROM

One screening agent's report said to rewrite `overlays/rom_7b9cb4/overlay.ld:51`
from the `asm/` path to a `src/` path. **It was not followed.** The build rule
is `asm/%.o: src/%.c`, so the object only ever exists under `asm/`; a `.ld` line
naming `src/<TU>.o` matches nothing, and an unmatched entry is **silently
ignored rather than an error**. All three functions would have vanished from the
overlay behind a green-looking build.

Three files in the tree already carry exactly that defect — see the
"TEN ELEVATED `.c` FILES CONTRIBUTE NOTHING TO THE ROM" section in
`docs/elevation.md`, written the same day. Agent briefs now carry an explicit
prohibition.

## Gate

`make clean && make -j8 && make compare` green, `goldensun.gba: OK`. Every
address above checked against the linked ELF with `tools/checkaddr.py`. Pairing
check at 4,592 sources; `--unlinked` unchanged at 10, so no landing here added
debt.
