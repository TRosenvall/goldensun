# Batch 04 — 8 functions elevated to matching C

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–03 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a **clean** build. Every address read back from the overlay ELFs and every
path confirmed to contain the function named.

All eight are overlay code. That is not a coincidence — see "where the
candidates are" below.

## The functions

| Function | Address | New source | Replaces |
|---|---|---|---|
| `OvlFunc_912_2008030` | `0x02008030` | `src/overlays/rom_7a0010/ovl_30_a_a.c` | whole file |
| `OvlFunc_934_2009378` | `0x02009378` | `src/overlays/rom_7bdeb0/ovl_1300_c_b.c` | **split** |
| `OvlFunc_936_20095e0` | `0x020095e0` | `src/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_b.c` | **split** |
| `OvlFunc_948_2008ec8` | `0x02008ec8` | `src/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_c_b.c` | **split** |
| `OvlFunc_970_20083c0` | `0x020083c0` | `src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_b.c` | **split** |
| `OvlFunc_970_20083dc` | `0x020083dc` | `src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_b.c` | **split** |
| `OvlFunc_970_20083f8` | `0x020083f8` | `src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_b.c` | **split** |
| `OvlFunc_970_2008414` | `0x02008414` | `src/overlays/rom_7fa4ec/ovl_30_c_c_c_a_c_c_c_b.c` | **split** |

The sibling `.s` files from each split must travel with them or the batch will
not link. The `rom_7fa4ec` chain is four sequential splits, so it leaves
`ovl_30_c_c_c_a_a.s` and `..._c_c_c_c.s` alongside the four `.c` files.

## Two techniques from this batch

**Hold a destination pointer across a call to get a callee-saved register.**
The four `OvlFunc_970` stubs record a slot's `pos.y` into a per-slot global.
Written the obvious way — fetch the actor, then assign to the global — gcc
keeps the destination address in a call-clobbered register and every one comes
out an instruction short. Taking the pointer *before* the call makes it live
across it, which is what forces the callee-saved register the ROM uses:

    int *dest = &Data_180c;                    /* before the call */
    Actor *actor = __MapActor_GetActor(0);
    *dest = actor->pos.y;

**Reach file-local data with an asm label.** `.L180c`, `.L1810` and `.L1818`
are already `.global` in a sibling `.s`, so `extern int Data_180c
__asm__(".L180c");` reaches them with no change to any assembly. Where the
label is *not* already exported, adding `.global` emits no bytes — that is what
batch 03 did for `.La0128`.

## Where the candidates are

Worth stating because it should shape where the next session looks.

Single-function overlay files of 5–14 instructions are **exhausted** — the
search returns exactly one, and it is blocked. But the same search applied to
functions *inside* multi-function files returns twenty in the overlays, of
which five were sampled and four matched.

The same search on the **main ROM** returns only ten, and most are
already-failed attempts. The main ROM has far less of this shape than the
overlays do. Anyone budgeting time should expect the overlay corpus to carry
the next several batches.

## One thing the screen caught that is worth repeating

I paired slots with destinations in address order and got two of the four
wrong — the third stub uses slot 3 and the fourth uses slot 2. The screen
reported exactly those two as differing at instruction 1 while the other two
passed.

That is the failure mode to want: a wrong assumption surfacing as a specific,
localised difference rather than a plausible near-match. It is also why
`tools/tryc.py` earns its keep despite having had six false-negative classes
and one false-positive class fixed in it over this session.

## Still open, and worth a decision from someone who knows the engine

Three functions are **one instruction** from matching, blocked on the same
thing: the ROM pools a small constant where gcc uses a `mov`. That is a
reliable tell that the operand was a *symbol reference* in the original, not a
literal — gcc never pools what it can `mov`, and always pools a symbol address.
Verified by assembling both forms; gas does not fold `ldr r0, =1`.

- `OvlFunc_971_2009050` — `ldr r0, =1`, the first argument of `__SetDestMap`.
  Plausibly a map id.
- `SetTextColor` — `ldr r2, =0xf`, a text-ink mask.

`message.sym` covers message ids and `file_table.sym` covers file ids; neither
is obviously right for either. **What are those id namespaces?** Adding a
plausible-looking name to a shared linker fragment to save one instruction is
a bad trade, so both are parked rather than guessed at.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
