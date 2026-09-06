# Batch 245 — the twins that weren't cheap, and a 1.00 that wasn't the template

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_936_200b2a4` | `0x0200b2a4` | [ovl_30_c_c_c_c_a_a_b.c](src/overlays/rom_7c097c/ovl_30_c_c_c_c_a_a_b.c) |
| 2 | `OvlFunc_968_20089c8` | `0x020089c8` | [ovl_30_a_c_c_a_c_a.c](src/overlays/rom_7f2f14/ovl_30_a_c_c_a_c_a.c) |
| 3 | `OvlFunc_926_2008bf4` | `0x02008bf4` | [ovl_314_…_a_a_a_a_a.c](src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a_a.c) |
| 4 | `OvlFunc_926_2008cd4` | `0x02008cd4` | [ovl_314_…_a_a_a_a_a.c](src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_a_a_a_a_a.c) |

## Parked

| function | address | blocker |
|---|---|---|
| `GetMercuryDjinni` | `0x080965a8` | callee-saved **three**-cycle; 17 of 239 at exact length and exact relocations |
| `GetVenusDjinni` | `0x08096140` | same class; **four instructions short — our colouring is better than the ROM's** |

Four elevated is one under the usual 5–8 gate. The round was four agents by
request; publishing rather than holding, because committed-but-unreported work
is the failure this project already paid for once.

No landing here depends on a flag group.

## NEAR-TWIN DID NOT MEAN CHEAP

Both parks are elemental siblings of `GetMarsDjinni`, landed hours earlier with
its solved template in hand. I expected the cheapest work on the board. The
selection heuristic — *elemental siblings are near-twins, so solve one and the
rest follow* — has paid five times this week and did not pay here.

**Mars's levers inverted rather than transferred, twice.** Its gState tail wants
**two** locals; Mercury wants **one**, because both loads are adjacent. Its
gState offset must be built **destructively**; Venus's is non-destructive.

That is sufficient-not-necessary appearing between functions that genuinely
*are* twins — the strongest form of it seen so far, and a caution against
treating family membership as transferable mechanism rather than transferable
*shape*.

Both parks are diagnoses, not abandonments. Mercury's class discriminator fired
twice — 80 random declaration permutations all give 20 differing, and 32
`goto`/`do-while` form combinations all give 58 — and its best result already
depends on a hard register pin worth 42→17, with plain C flooring at 42. So it
is 17 short of a *match*, not 17 short of a *spelling*.

Venus is **four instructions short because we produce a strictly better
colouring than the ROM** — five chains where it spends six. The missing four are
the low-register copies the ROM must emit because its array base sits in a high
register. A pin cannot reach it: the member that must move has to move *into* a
high register, and forcing that explodes the function to 321 lines.

## A 1.00 TEMPLATE SCORE WAS NOT WHAT CARRIED ENTRIES 3–4

`templated.py` ranked them first at 1.00 — on six shared symbols and a
cross-overlay neighbour. The tool's own note says a low-symbol 1.00 is close to
coincidence, and here it was.

What carried them was the **literally adjacent same-bank sibling**, cut from the
same parent `.s`: they are `_a`, it is `_b`. Both matched on the first
candidate.

Meanwhile the 1.00 neighbour's headline lever is the most expensive thing
available here — **128 differing and 96 bytes long** on one, 139 and 112 long on
the other.

**The rule refused itself, and that is the point.** It is recorded as *countable*
rather than stylistic — count the `bl`s between the stores. Eight there; **one**
here, for eleven field accesses.

## THE ACCESSOR LEVER IS SYMMETRIC, WITH A SIZE TELL IN BOTH DIRECTIONS

Entry 2's ROM **caches** — one accessor call at the top, the pointer held across
the body. Writing it uncached is 131 differing and **36 bytes LONG**.

Entries 3–4 are the opposite polarity, where caching a repeating ROM was 117
differing and **8 bytes SHORT**.

So: uncached-against-caching is LONG, cached-against-repeating is SHORT, and the
countable test picks the side before anything is compiled. Both polarities
landed in one batch.

## A `void f(void)` PROTOTYPE REACHES BACKWARD

The recorded rule is **forward** — the deferral is caused by the *preceding*
call's return type — and a recorded table proves the parameter list is
irrelevant. Neither covers a **zero-argument** callee acting on a call that comes
*before* it.

On entry 2, three such callees each cost 2 when dropped, and never at their own
site; one breaks a site **two calls further back**.

It is the **return type**, not the missing line — `extern void f();` with an
empty list is 0, `extern int f(void);` is 2, no declaration is 2. Declared `int`,
the call becomes a `call_value` carrying an explicit SET of r0 rather than a bare
CLOBBER, so sched2 sees an output dependence it otherwise lacks.

Two riders: **a pin does not immunise a site against this** (both broken sites
are already pinned), and the same shape is inert four times in the same
function's opening block — so it is positional, not per-callee.

## THE NAMING LEVER HAS A FIRST-USE FORM, LIKE THE PIN RULE

On entry 1, only **10 of 49** named slots are load-bearing — precisely the first
site at which each variable takes each value. The other 39 revert to literals
individually **and all together** at zero cost. Five recorded entries cover
naming stack arguments; none minimises the naming.

**Shipped fully named rather than minimised**, deliberately departing from
"inert scaffolding must not ship." That rule exists because an inert *pin* is
unnatural and misleads a reader about what carries weight; a consistently named
set of stack-argument locals is ordinary C, and naming exactly ten of forty-nine
would read as arbitrary. The measurement is in the file header so the next
reader knows which ten matter.

Two corrections came with it:

- **"Stack arguments must be named PER CALL SITE" is not general.** 92 per-site
  locals across 46 calls is **byte-identical** to three shared ones — gcc commons
  them straight back. The rule turns on whether the *values* are distinct.
- **A recorded entry says the literal spelling of a repeated stack constant
  compiles the same.** Here it does not — by 469 encodings and twelve bytes
  short, because gcc hoists in straight-line code but *rematerialises inside the
  loop body*, never allocating the ROM's fifth callee-saved register.

## A CARRIED COUNTER-DERIVED VALUE IS NOT A SECOND SOURCE VARIABLE

Entries 3–4 share a `.s` and hold **both shapes of one loop**, which is what
makes it decidable:

| function | ROM shape | inline expression | hand-written counter |
|---|---|---|---|
| `2008bf4` | carried in r8 | exact | exact — tie |
| `2008cd4` | recomputed inline | exact | **91, 4 bytes LONG** |

The asymmetry, not a direction rule: a hand-written counter **always** becomes
the carried register — the `cd4` variant reproduces `bf4`'s loop
instruction-for-instruction in a function whose ROM has no such register — while
the inline expression leaves gcc free and it chooses per body.

So: **recomputed is decisive** (no named counter), **carried is undecidable**,
and inline is the only spelling reaching both shapes.

A coefficient-sign explanation was tried and **rejected** — a four-way control
builds the carried register in all four, and `-fno-strength-reduce` is
byte-identical — so no pass is named. Worth more than a mechanism that merely
fits.

## Two findings from the parks

**Where a function's loops carry no hoistable invariant and no
strength-reducible giv, `goto` and `do`/`while` are indistinguishable — including
to the register allocator.** 32 form combinations, all 58 differing. The `goto`
lever is recorded as acting through `loop.c`, and the loop-note weighting of
`REG_N_REFS` is a separate consequence one would expect to move allocation
anyway. It does not. **Read the loop bodies for an invariant before spending a
sweep on an allocator residue.**

**A pool-loaded zero feeding a `strb` is a loop-invariant-motion artefact, not a
mode tell.** Venus's ROM has a hoisted `ldr r3,=0`, which reads exactly like the
recorded halfword-literal pooling and invites writing a literal. Measured
backwards: the literal gives `mov r3,#0` and is **two instructions short**. The
pool word comes from *where* the constant was hoisted to, not from the store's
mode.

## Method notes

**The hole test cut one search to two elements.** On entry 1: 44 holes, 19
all-cheap, 6 candidates — and four of the six were single-argument pool-load flag
calls, leaving exactly two sites to try. One pin, of **width one**, was the
answer.

**An exhaustive add-one-pin pass over 51 other sites** gave 37 ties and 11 worse,
the worst at 275 differing with relocations differing — pinning there destroys a
flag-result-as-zero commoning. More evidence the nomination screen is "expensive
**and** commoned".

**Entry 2's fifth pin is one its template lists as inert** — sufficient-not-
necessary in the **add** direction, which is the rarer half.

**One agent flagged its own budget split**: it spent most of the round on Mercury
before starting Venus, so Venus's four-instruction gap is well characterised but
less exhaustively swept. Recorded so a later round knows which park is the
shallower one.
