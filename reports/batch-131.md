# Batch 131 — the selector becomes a tool, and predicts the flag group

Verified on a clean `make clean && make compare` — `goldensun.gba: OK` — with
every address read out of the linked ELF.

## Elevated (2219 → 2214)

| function | address | notes |
|---|---|---|
| `OvlFunc_903_2008348` | 0x02008348 | `CSE_CFLAGS` + named stack pairs |
| `OvlFunc_948_20098e0` | 0x020098e0 | first screen, default flags |
| `OvlFunc_890_2008054` | 0x02008054 | `CSE_CFLAGS` |
| `OvlFunc_891_200a244` | 0x0200a244 | `CSE_CFLAGS` |
| `OvlFunc_891_200a2f4` | 0x0200a2f4 | sibling of the above |

## `tools/pool.py`

The candidate query had been rebuilt inline **nine times** across these batches,
twice with a bug that made it report zero — each of those cost a round. It is
now one tool with the corrections applied, and its columns decide the approach
before any C is written:

- **`br == 0`** — neither naming lever can work, only the flag group is left.
- **`flag2`** — one id feeds both a `GetFlag` and a `Set`/`ClearFlag`, so screen
  with `--no-rerun-cse` from the start. It predicted the flag group correctly
  for all three functions elevated in the final round.
- **`site`** — guarded interleave sites, the ones the lever can reach.

**Park exclusion is by function NAME, not filename.** Park files are named for
the low address, and overlay functions from different overlays share it — every
overlay loads at 0x02000000. `OvlFunc_971_200808c` was being hidden from the
candidate pool by a park written for `OvlFunc_881_200808c`. The names are parsed
out of the park headers.

## Name a negated constant as `-1`

For the interleave lever, constants are named at the top so gcc rematerialises
them at their uses. A negated one must be the negative literal:

    c1 = -1;              /* rematerialised as mov+neg at the use: exact */
    c1 = 1; c1 = -c1;     /* a COMPUTED value: held in r5, prologue grows */

On `OvlFunc_891_200a244` the two-step form cost `push {r5, r14}` against the
ROM's `push {r14}` and **66 of 67 differing**; the literal was exact. gcc will
rematerialise a constant but will keep a computed value alive. Inside a guarded
block the two-step form is fine and several elevated functions use it — it is
only wrong in the dominating block, where being rematerialised is the point.

## The derived-constant machinery has no steering, in either direction

Three parks now sit on one fact from three angles:

- `OvlFunc_952_20085a4` — the ROM derives from a live base and gcc will not.
  Fixed by making the base a **symbol** (batch 130).
- `OvlFunc_943_2008950` — gcc derives (`add r2, #0x60`) where the ROM builds
  both offsets fresh. Naming them separately in the dominating block does
  nothing; neither does `CSE_CFLAGS`, so it is the main `-O2` CSE.
- `Func_80a22f4` — the ROM derives ONE of three register-pinned DMA operands and
  reloads the other two; asking for the derivation fires it on all three.

It fires on whatever is in a register and cannot be requested or suppressed per
operand. The symbol lever is the only steering available and it only pushes one
way.

## Parked

- `OvlFunc_943_2008950` — 35 of 66, above.
- `OvlFunc_924_20090c0` — 22 of 56. The guard cascade, three separately-named
  copies of `0x80 << 9`, and a derived store all reproduce; what remains is
  base/offset register roles in the address chain.
- `OvlFunc_939_20095bc` — 19 of 62, and the reason generalises: **a function
  with no conditional branch has only the flag group**, because both naming
  levers need a dominating block to rematerialise from.

## Still owed

- 12 not-yet-elevated `.s` TUs inside Makefile wildcards.
- ~3,300 lines of duplicate Makefile rule blocks.
- 287 parks, deliberately deferred. Three have been recovered by levers found
  after they were written, so the eventual sweep is worth more than the count
  suggests.
