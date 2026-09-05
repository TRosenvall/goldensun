# Batch 225 — six agents, eight functions, and a trap removed at the source

Eight functions elevated, every one from agent screening and every one
byte-exact. The batch also removes a Makefile wildcard that had been quietly
mis-building this directory for three separate landings.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_885_2009760` | `0x02009760` | [ovl_30_…_a_c_c_b.c](src/overlays/rom_78603c/ovl_30_c_c_a_c_a_c_c_b.c) |
| 2 | `OvlFunc_881_200a274` | `0x0200a274` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a_c_b.c) |
| 3 | `OvlFunc_941_20084a8` | `0x020084a8` | [ovl_30_…_c_c_a_b.c](src/overlays/rom_7c5efc/ovl_30_c_a_c_c_c_a_c_c_a_b.c) |
| 4 | `OvlFunc_887_2008f90` | `0x02008f90` | [ovl_30_…_c_a_a.c](src/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a.c) |
| 5 | `OvlFunc_949_2008980` | `0x02008980` | [ovl_30_c_c_c_c_a.c](src/overlays/rom_7d4af4/ovl_30_c_c_c_c_a.c) |
| 6 | `OvlFunc_885_2008964` | `0x02008964` | [ovl_30_…_a_a_c_b.c](src/overlays/rom_78603c/ovl_30_c_c_a_c_a_a_c_b.c) |
| 7 | `OvlFunc_966_2008218` | `0x02008218` | [ovl_30_c_c_c_a_a_b.c](src/overlays/rom_7f148c/ovl_30_c_c_c_a_a_b.c) |
| 8 | `OvlFunc_921_20099e8` | `0x020099e8` | [ovl_30_…_c_a_c.c](src/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c_a_c.c) |

## A wildcard that was wrong three times in four

`Makefile`'s `rom_78603c/ovl_30_c_c_a_c_a%` applied `-O1` and appeared as **30
identical duplicate blocks**. It matched **four** translation units, and
**three of them had each already needed an explicit `-O2` override** added
after `-O1` broke them — on three separate occasions, the most recent earlier
in this same batch. `OvlFunc_885_2008964`'s split product would have been the
fourth.

Every split ever landed under that prefix wanted `-O2`. Only
`ovl_30_c_c_a_c_a_b` genuinely wants `-O1`. So the 30 blocks are gone, replaced
by one explicit rule naming that single member, and new split children now fall
to the tree default. `make compare` is the proof nothing else relied on it.

**When a prefix trap fires for the third time, stop overriding and count what
the pattern actually matches.** The check is two commands: list the `.c` files
sharing the prefix, then grep for existing explicit overrides of them. Had I
run it after the fourth instance last batch, this batch's two would not have
happened.

## The uniform fill: confirmed three times, bounded once

Batch 224 found that sched2 produces different *emitted* orders from one
spelling, so pinned fills should be written uniformly rather than transcribed.
This batch tested it hard:

- **`OvlFunc_881_200a274`** — fourteen sites emit non-ascending fills, and
  writing every fill uniformly is *byte-identical* to transcribing each. A
  second shape too: pool loads, not just `mov`/`lsl` pairs.
- **`OvlFunc_921_20099e8`** — 57 sites, at least six distinct emitted orders,
  uniform sufficed at all of them.
- **`OvlFunc_966_2008218`** — the counter-example. Uniform worked at 26 of 27;
  one site genuinely needs the descending fill, and with the other thirty
  pinned it was the *only* residue.

So the rule now carries a procedure: **write every fill uniform, measure the
residue, transcribe only the sites that survive.** Cheaper than transcribing
everything, and it leaves a file whose fills mean something.

## The prologue rule is about content, not width

`OvlFunc_921_20099e8` pushes `{r5, r6, r7, lr}` — by width alone, the
named-local case. Its three saved registers hold a base pointer and the two bit
masks of a `&= 0xfe` / `|= 1` pair. Every script constant is rebuilt at every
use, so it is a **57-pin function**, and plain C spills r8–r11 into r5–r7 at
entry for 466 differing encodings.

List what each saved register actually holds before choosing the cure.

## The cprop hazard, scoped

Batch 223 found that a held message base needs `register int m __asm__("r5")`
because gcse's cprop destroys a plain `int`. Three shapes in this one batch fix
its scope:

- `add r0, r5, #6` — m **re-read** and still live: cprop has a constant to
  propagate, the pin is required.
- `add r5, #3` — m **redefined in place**: nothing to substitute, a plain
  `int` survives, and the pin is measurably **inert**.
- a re-read **inside the arms of an `if`/`else`**: needs the pin, because cprop
  propagates into *successor* blocks, where same-block re-reads do not.

`OvlFunc_949_2008980` corrects the record I landed two batches ago, which
stated the requirement unconditionally.

## Other findings

**Two bitfield sites can want opposite things, four lines apart.** On
`OvlFunc_887_2008f90` the `&= 0xfe` site needs no local while the `|= 1` site
needs a narrow one — a commutative op puts the constant in the destination only
when it survives as a distinct QImode pseudo.

**A pointer-returning call whose result dies immediately must not be named.**
Thumb's `add reg, imm8` is destructive, so a named local forces gcc to preserve
the base and costs a `mov` per site.

**A HImode store of a constant goes through the pool even for zero.** An `int`
intermediate gives the ROM's `mov r3, #0` — 92 differing.

## Discipline

`tools/guard_generated.sh` fired on five of the eight commits and its `--fix`
path handled every one. `git add` aborted twice on a **predicted** split suffix
— once staging nothing at all, so a commit ran empty and had to be redone. The
rule is already on file and I broke it twice: **list a split's actual products
before constructing the paths.** For the later landings I listed status first
and classified each product as hand-written or generated before staging, which
is what the remaining six commits did cleanly.
