# Batch 1 — 12 functions elevated to matching C

For porting into `Coaltergeist/goldensun-decomp`.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
with `make compare` green, built with gcc-2.96 from `camelot-gcc` under
`tools/Dockerfile`. Every function was also screened per-function with
`tools/tryc.py` before landing.

**Two of these were parked in `src/non_matching/` upstream** and are now
matching: `ActorCmd_SetScript` and `Func_80ad5f4`. Their stale attempts are
deleted in this branch.

## The functions

Names are the ones already in the tree — nothing was renamed. The last column
is our annotation's reading of what the function does, which is *inferred* and
should be treated as a suggestion, not a finding.

| Function | Address | New source | Replaces | Our reading |
|---|---|---|---|---|
| `ActorCmd_SetScript` | `0x0800ca2c` | `src/rom_9000/rom_ca2c_a.c` | whole file | script-VM: rebase script on cursor |
| `ActorCmd_CallNative` | `0x0800d6a4` | `src/rom_9000/rom_d654_a_c_a_a_a_a.c` | whole file | script-VM: call native predicate |
| `ActorCmd_GotoIfNZ` | `0x0800d780` | `src/rom_9000/rom_d654_a_c_a_a_c.c` | whole file | script-VM: jump if condition set |
| `ActorCmd_GotoIfZ` | `0x0800d7b4` | `src/rom_9000/rom_d654_a_c_a_a_c.c` | whole file | script-VM: jump if condition clear |
| `ActorAttrOp_width` | `0x0800e334` | `src/rom_9000/rom_e220_a_c.c` | whole file | attribute opcode: collision radius |
| `CanEquipItem` | `0x0807842c` | `src/rom_77000/rom_78414_a_c.c` | whole file | class-mask equipment test |
| `Func_808ed4c` | `0x0808ed4c` | `src/rom_8a000/rom_8d9a4_c_a_c_c_c_a.c` | whole file | read slot record under player |
| `Func_8091c1c` | `0x08091c1c` | `src/rom_8a000/rom_91584_c_a_c_c_c_b.c` | **split** | give item to party member |
| `MapActor_SetIdle` | `0x080920a0` | `src/rom_8a000/rom_91584_c_c_a_c_a_c_c_c_c_a_c_a.c` | whole file | park a field actor |
| `Func_8092848` | `0x08092848` | `src/rom_8a000/rom_925e0_a_a_c_b.c` | **split** | turn two actors to face each other |
| `Func_80ad5f4` | `0x080ad5f4` | `src/rom_a1000/rom_ad274_c_a_b.c` | **split** | store one actor-scale slot |
| `Func_80b7e7c` | `0x080b7e7c` | `src/rom_b5000/rom_b7410_a_c_c_c.c` | whole file | battle teardown: free sprites |

"whole file" means the `.s` held only these functions and was deleted outright.
"split" means `tools/split_s.py` cut the target out into `_b` and the linker
script was rewritten — those three carry sibling `.s` files that must come
across with them:

| Split target | Siblings created |
|---|---|
| `Func_8091c1c` | `asm/rom_8a000/rom_91584_c_a_c_c_c_a.s` |
| `Func_8092848` | `asm/rom_8a000/rom_925e0_a_a_c_c.s` |
| `Func_80ad5f4` | `asm/rom_a1000/rom_ad274_c_a_a.s`, `asm/rom_a1000/rom_ad274_c_a_c.s` |

`stage1.ld` changes accordingly — three single `.o(.text)` lines become
seven, in order.

## Header changes

One field named in `include/entity.h`:

    - /* 0x20 */ u8 unk_20[0x02];
    + /* 0x20 */ u16 width;

`ActorAttrOp_width` establishes it. Note the asymmetry it exposes: the stored
radius is read **unsigned** and the operand is narrowed to a **signed**
halfword before comparison, so a negative operand can never equal a stored
radius. That is in the ROM, not an artifact.

## Two corrections worth passing on

**`include/actor.h`'s `struct Actor` is 16 bytes short.** `void *sprite` sits
at `0x40`; it belongs at `0x50`. The fields after it are named for offsets they
do not land on — `__unk5A` is at `0x4A`, `__unk5C` at `0x4C`, `update` at
`0x5C`.

Confirmed against three of the tree's own matched files
(`src/overlays/rom_7f2f14/ovl_30_a_c_c_b.c`,
`src/overlays/rom_7c5efc/ovl_30_c_a_c_c_a_c_b.c`,
`src/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_b.c`), which bypass the header and
declare local structs putting `update` at `0x6C`, `unk55` at `0x55`, `unk59` at
`0x59` and `unk63` at `0x63` — all agreeing with `include/entity.h` and none
with `actor.h`.

Nothing is broken today: only one matched file touches the affected range and
it uses its own local struct. But the header will mislead the next person to
trust it.

**One of our own annotations was wrong.** `Func_80b7e7c` was documented as
taking a combatant id and a position. It takes no arguments. Corrected in
place; flagged here because the annotation corpus has this failure mode
generally (see `docs/attribution.md`).

## Also in this branch

- **`docs/elevation.md`** — the working method, the codegen facts that decided
  matches, and a taxonomy of the five compiler behaviours that block the rest.
  This is the part most likely to be useful beyond these twelve functions.
- **`tools/tryc.py`** — per-function screen; proves a candidate before it is
  wired into the build.
- **`tools/split_s.py`** — the `.s` splitter, byte-neutral by construction.
- **`tools/showfunc.py`**, **`tools/elevation_candidates.py`** — target
  selection.
- **Nine near-misses** parked in `src/non_matching/`, each recording its
  blocker class and what was already tried, so retries do not repeat work:
  `rom_15e8c`, `rom_783a4` (ModifyHP/ModifyPP), `rom_78480`, `rom_78a34`
  (BreakItem), `rom_8d5a4`, `rom_91254`, `rom_9a44c`, `rom_c548`
  (Func_800c548/570), `rom_d710` (ActorCmd_Loop).

## A build note (ours, not yours)

Worth stating plainly because an earlier draft of this file got it backwards:
**this is a collision in our checkout, not a bug in your tree.**

`AGBCC_DIR ?= tools/agbcc` is correct. What broke our clean build is that our
own earlier work committed *macOS* agbcc binaries to exactly that path, back
when this project was still building with agbcc. In a Linux container they
fail with `Syntax error: "(" unexpected` -- the shell trying to execute a
Mach-O file -- and it stayed hidden because `src/lib/m4a/` and
`src/lib/agb_flash/` change rarely, so their objects survived every
incremental build.

Fixed on our side with `ENV AGBCC_DIR=/opt/agbcc` in our `tools/Dockerfile`.
Nothing needed upstream. Mentioned only so that if the batch is tested in a
container, the stale `tools/agbcc/` binaries are known not to be part of it.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'

Per-function, without touching the build:

    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        python3 tools/tryc.py src/rom_9000/rom_ca2c_a.c
