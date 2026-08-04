# Batch 22 — 5 functions, four of them from the parked set, all on types

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–21 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted. Every address
below read back from the linked ELFs.

## Read this if you have anything parked that calls through `_call_via_rN`

There are 223 functions left in `asm/` that do. Before this batch, every file in
the tree that mentioned `_call_via_r3` was parked. Four of the five functions
here came out of that set, and none of them needed a permuter.

**`bl _call_via_rN` is not a linker veneer to work around. It is what gcc emits
for a call through a function pointer.** A direct call is one instruction
shorter, so the screen shows *ours* one line short with the `ldr r3, =Sym`
missing — which reads like a dropped instruction rather than a wrong call form.
Route the call through a local of function-pointer type:

    typedef void (*CopyFn)(volatile u16 *dst, void *src, s32 len);
    CopyFn copy;  ...  copy = Func_8001af8;  copy(pal, data, 0x80);

**gcc-2.96 does not constant-propagate that back into a direct call at -O2.** A
modern compiler would, which is presumably why the shape had gone untried — it
looks like it cannot survive optimisation, and it does.

### Then two type choices decide the rest

Once the call form is right, these functions are decided entirely by types, and
both rules are things the tree already half-knew:

**1. The pointer's return type is the declaration.** An indirect call has no
prototype at the call site, so the pointer's type is the only declaration there
is, and its return type drives the same lever as a direct callee's:

    void (*fp)(int, int)   ->  gcc fills r0 FIRST
    int  (*fp)(int, int)   ->  gcc fills r0 LAST    (the ROM's order)

**2. `pop {rN}` for N ≠ 0 means the enclosing function returns non-void** —
whether or not anything is returned. This rule was already in
`docs/elevation.md`, and one of the park notes cited it correctly; it simply had
not been applied to the function next to it.

### What the park notes had said instead

This is the part worth your attention, because the notes were confident and
wrong in a consistent direction — they read a *type* problem as a *scheduling*
problem, because the diff sat next to a load.

| function | filed as | actually |
|---|---|---|
| `Func_801671c` | "class 5, SCHEDULING. Nine instructions, nine right, one in the wrong place" | pointer return type |
| `Func_80b63b0` | "logic faithful, does NOT byte-match (endgame permuter seed)" | pointer return type + non-void return |

`Func_801671c`'s note recorded three attempts, all of them about where the
pointer was *stored* — "via a typedef'd local, via a plain local, with the
destination in its own local". None was about its type. `Func_80b63b0`'s note
described both of its diffs accurately and proposed permuting both.

## Functions

| function | address | file | note |
|---|---|---|---|
| `OvlFunc_945_200d068` | `0x0200d068` | `src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a_b.c` | **unparked**, declaration lever |
| `LoadVFXFile` | `0x080e0524` | `src/rom_c9000/rom_e0524.c` | first indirect call matched |
| `Func_80b63b0` | `0x080b63b0` | `src/rom_b5000/rom_b5a0c_c_c_a_a_a_b.c` | **unparked** |
| `Func_801671c` | `0x0801671c` | `src/rom_15000/rom_15e8c_a_c_c_c_b.c` | **unparked** |
| `Func_8016738` | `0x08016738` | `src/rom_15000/rom_15e8c_a_c_c_c_c_b.c` | twin of the above |

`LoadVFXFile` was a whole-file conversion with no split; the annotation on the
original `.s` is preserved in the new file's header.

## The declaration levers are now exhausted on the parked set

Three functions came out of the park in the preceding two batches on levers the
notes never tried, which is enough of a pattern to stop doing by hand.
`tools/sweep_decls.py` screens every parked file with each declaration dropped,
and with each implicitly called function declared. The second direction needs no
parameter signature — `extern void Foo();` fixes only the return type, and the
return type is the whole of the first lever.

**263 variants, no matches.** The easy declaration wins are gone; the four here
came from the pointer-type variant, which that sweep does not generate.

A tool whose useful answer is "nothing" is indistinguishable from a broken one,
so it carries `--selftest`: it strips the one declaration known to decide a
match and checks the sweep puts it back. That same check established that the
`extern void Foo();` form is equivalent to a full prototype — without it,
direction two would have been silently inert and the zero meaningless.

While wiring that up, 17 park headers turned out to carry their source reference
in an older `-- asm/...` form that neither this sweep nor `rescreen_park.py`
could parse. A quarter of the parked set had been invisible to both. Normalised;
the re-run still came back clean.

## Counts

230 functions elevated in total. 3,069 hand-written functions remain in `asm/`
of 5,714. 83 parked.
