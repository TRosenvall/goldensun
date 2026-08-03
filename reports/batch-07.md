# Batch 07 — 5 functions elevated, and two blocker classes retired

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–06 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a **clean** build — the first one in this project that completed without
manual recovery, see *Two build fixes* below. Every address read back from the
linked overlay ELFs.

## The functions

| Function | Address | New source |
|---|---|---|
| `OvlFunc_901_2008754` | `0x02008754` | `src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_c.c` |
| `OvlFunc_932_200b428` | `0x0200b428` | `src/overlays/rom_7b9cb4/ovl_30_a_c_c_c_a_c.c` |
| `OvlFunc_929_2008524` | `0x02008524` | `src/overlays/rom_7b7790/ovl_314_c_c_c_c_c_a.c` |
| `OvlFunc_916_2008054` | `0x02008054` | `src/overlays/rom_7a37f0/ovl_30_c_c_a.c` |
| `OvlFunc_932_2008388` | `0x02008388` | `src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_b.c` |

The first four are **whole-file** conversions — each `.s` held only its one
function, so there is no split to carry and no linker-script change. Only the
last came from a split, and its sibling `.s` files must travel with it.

Two files also come with a new linker fragment, `unknown_id.sym`, which has to
be `INCLUDE`d from `stage1.ld`.

## The two findings, which matter more than the five functions

Both of these were written up in `docs/elevation.md` as *blockers* — one for
nine failed formulations, one for twelve rounds. Both turned out to be one
line of C.

### 1. A missing prototype changes the generated code

An implicitly declared function returns `int`. gcc-2.96 therefore treats r0 as
holding a live return value across the call, and defers writing r0 when
setting up the **next** call's arguments:

    no prototype    mov r2,#0x20 / mov r3,#0x20 / mov r1,#0x40 / mov r0,#0
    prototyped      mov r2,#0x20 / mov r3,#0x20 / mov r0,#0    / mov r1,#0x40
    rom             mov r2,#0x20 / mov r3,#0x20 / mov r0,#0    / mov r1,#0x40

This had been recorded as the `arg-fill-order` blocker, and it is not a
scheduling problem at all. Both orders occur throughout the ROM, and both
occur *inside a single function*: `OvlFunc_929_2008524` fills r0 first for
`__Func_8093040` and `__ActorMessage` and last for `__Func_809280c`. That is
gcc reporting which callees the original translation unit had declarations
for.

**Prototyping only the mismatching call is not enough** — the deferral comes
from the *preceding* call's return type, so the `void` returns matter as much
as the parameter lists.

The reverse direction is equally usable: where the ROM fills r0 last, the
original had no declaration.

### 2. The pool tell needs the id *named*, not *identified*

This is the one that was blocking the most — **103 of 395 overlay
candidates** — and the one I have been asking you to ask Coaltergeist about.
**You can stop asking.** The question is still interesting, but it is no
longer in the way.

The reasoning that parked it was: the ROM pools a constant that would fit in
an eight-bit `mov`, so the operand was a symbol reference; `message.sym`
covers message ids and `file_table.sym` covers file ids; neither covers
whatever `0x4d` is; and inventing a plausible name for a shared linker
fragment is a bad trade, because a wrong name propagates where a missing one
just waits.

The last step is where it went wrong. **Matching does not need the id's
meaning — only that the operand be a symbol.** Naming it by value asserts
nothing that could later turn out to be false:

    /* unknown_id.sym */
    _ID_4d = 0x4d;

    extern int _ID_4d;
    __Func_8091f90((int) (&_ID_4d), 0x63);   /* -> ldr r0, =0x4d */

An absolute symbol definition in a linker script emits no bytes, so the link
is byte-identical to the literal. And `message.sym` has been doing exactly
this from the beginning — its own comment reads *"named by value; pending
semantic names."* The move was available the whole time.

Unidentified ids go in `unknown_id.sym`, deliberately **not** folded into
`message.sym` or `file_table.sym`. Those two namespaces are identified, and
putting an unknown id in one of them asserts something not known to be true —
which is the actual bad trade, and it is avoidable.

`OvlFunc_932_2008388` is the first out. Two siblings sit behind it in the same
`.s` (`0x4f` at `0x020083b4`, `0x51` at `0x020083e0`), mechanical once split.

The ids `0x4d`, `0x4f`, `0x51` are spaced by **two** across three consecutive
functions, which is a hint about the namespace worth keeping — whatever
indexes it appears to have paired entries.

## The candidate pool nearly doubled

`tools/elevation_candidates.py --clean` was still discarding every function
showing either shape. Reclassified:

| shape | count | what it now needs |
|---|---|---|
| `pool-tell` | 103 | define the operand by value in `unknown_id.sym` |
| `arg-fill-order` | 138 | declare every callee, with its `void` return type |

Clean overlay candidates go from **830 to 1069**. Both are still detected and
still printed, because each needs a specific extra step and the tag says
which.

Both had been *documented* before they were *filtered*, which is the same
failure the filter was written to prevent: a check that runs beats a note that
is read.

## Two build fixes, both predating our work

Neither is caused by anything in these batches; both are reachable from a
fresh clone of your tree, and both only show up in a **clean** build.

**1. Three dependencies name pre-split files.** The three common overlays
`incbin` from a specific overlay's `orig.bin`, and the dependency was declared
against `common0.o` / `common1_c.o` / `common2.o`. Those have since been
split, so the `.incbin` now lives in a descendant like
`common2_c_c_c_c_c_c_c_c_c_c.s` while the dependency points at a name nothing
builds:

    common2_c_c_c_c_c_c_c_c_c_c.s:112:
        Error: file not found: overlays/rom_7bf5a8/orig.bin

Fixed by wildcarding over the split descendants, matching the loop directly
above it that already does this per overlay directory.

**2. `as -MD` writes an unusable dependency.** A gcc-generated `.s` carries
`.file "dummy.c"`, and the assembler records that name as a dependency exactly
as written — with no directory component:

    src/rom_b0000/dummy.o: dummy.c src/rom_b0000/dummy.s

The next build needing that `.o` then dies with `No rule to make target
'dummy.c'`. Fixed by dropping directory-less `.c` entries from the generated
dependency file; the `.s` beside them is the real dependency.

Until these, every "clean build" in this project involved deleting every `.d`
by hand and retrying. That is worth knowing if you have hit the same thing.

## Still open, and still only answerable by you

- **What are the id namespaces?** No longer blocking, but `_ID_4d` is a
  placeholder and a real name is better than a correct one.
- **Five ambiguous offsets in `actor.h`** (batch 03) are documented rather
  than guessed.

## Reproducing the verification

    docker build -t goldensun-build -f tools/Dockerfile .
    docker run --rm -v "$PWD:/work" -w /work goldensun-build \
        sh -c 'make clean && make compare'
