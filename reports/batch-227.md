# Batch 227 — large functions, and the pool tells you which answer is right

Six functions elevated, all from agent screening, all byte-exact. The
well-templated pool has thinned toward large functions and this batch reflects
that: 218 to 806 instructions, one of them needing 46 pins.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_909_200a1bc` | `0x0200a1bc` | [ovl_30_…_c_c_b.c](src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_c_c_c_c_b.c) |
| 2 | `OvlFunc_896_2009450` | `0x02009450` | [ovl_314_…_a_c_b.c](src/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_b.c) |
| 3 | `OvlFunc_930_20081ec` | `0x020081ec` | [ovl_30_…_a_a_c.c](src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_a_a_c.c) |
| 4 | `OvlFunc_938_2009494` | `0x02009494` | [ovl_30_…_c_c_b.c](src/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c_c_b.c) |
| 5 | `OvlFunc_891_2008150` | `0x02008150` | [ovl_30_c_c_a_a_c_b.c](src/overlays/rom_78c76c/ovl_30_c_c_a_a_c_b.c) |
| 6 | `OvlFunc_952_20097e8` | `0x020097e8` | [ovl_30_…_c_c_b.c](src/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_b.c) |

## The pool's order settles what the register allocation cannot

`OvlFunc_909_200a1bc` is the **counterweight to `_CONST_a1`**, which I added to
`const.sym` last batch on the strength of the same surface tell — a small value
pooled where an eight-bit `mov` would do. This one is a genuine literal, and the
proof is the pool's *order*.

Its four byte stores read from a mid-function pool whose words are
`[0, 0x2410000, 0x2960000, 0xfffc0000]`. The zero sorts **first** — ahead of a
constant whose own load is forty-five instructions *earlier*.
`add_minipool_forward_ref` keeps the pool sorted by `max_address`, so an entry
can only sort ahead of an earlier reference when **its own reference is narrow**:
gcc routes the QImode zero through an HImode temp with a 64-byte pool range,
against the ROM's displacement of 60. That same narrowness dumps the pool
mid-body with the ROM's `b` over it.

`(int)&_CONST_0` reproduces the register *and* the placement and still fails —
it sorts the zero third and moves all eleven words to the end, four bytes short.

So the two functions together give a usable discriminator: when a pooled small
value looks like a symbol, **read the pool's order, not only its contents.**

## A partial reorder is a priority tie, not a barrier problem

`do { } while (0)` *ends* a scheduling region, which is the wrong tool when the
ROM shows a **partial** reorder — some instructions moved, others not.

On `OvlFunc_952_20097e8` the ROM emits a `strh` before its `ldr` while still
hoisting the address load above both. With two dependent stores the load
outranks the `strh`; with one they tie and the lower LUID wins. The cure is to
wrap **only the second store**. Every whole-region spelling — a bare barrier
between the stores, one around the first, one around both — costs 696 differing.

If the reorder is partial, change *what ties*, not where the region ends.

## Other findings

**One scratch-register pin can settle two distant clusters.** On
`OvlFunc_891_2008150` the closing store block was twelve differing because
`REG_ALLOC_ORDER` is `{3, 2, 1, 0, …}` — gcc takes r3 for the offset and r2 for
the address, the ROM the other way. Binding the pointer to r3 closed it *and*
fixed two index registers forty instructions earlier. Six spellings of that
earlier cluster had measured inert when attacked on its own.

**A homogeneous residue is one lever, not N problems.** On
`OvlFunc_896_2009450` the uniform fill left thirteen differing lines across six
sites, every one the same fault. One reading fixed all six. Check whether a
residue shares a shape before bisecting it.

**A sibling's declaration of a shared callee is not automatically right.** The
same function calls an intra-overlay routine that a sibling file declares as
returning `int`; here `void` matches and `int` costs 16 differing, because
whether the call writes r0 truncates a dependent list and flips a scheduling tie.

**Sometimes every pin is load-bearing.** `OvlFunc_930_20081ec` needed 46 pins
and *none* is removable — every one tried individually under objcmp. The usual
result is a third to a half inert, so this is the far end of the range rather
than a failed sweep.

**`__Func_8092c40` is now a tell by name.** Four functions running, it is the
lone site wanting the descending fill while everything else takes the uniform
one — and on two of them it was the entire residue.

**No constant-CSE at 806 instructions.** `OvlFunc_952_20097e8` matched the ROM's
length from the very first plain-C draft; all 674 differing lines were argument
ordering. Worth checking the length before reaching for the class.

## Discipline

`tools/guard_generated.sh` fired on two of the six commits. One landing failed
to compile because I extracted the candidate's body by slicing from its first
`extern`, which dropped a `struct` definition sitting above it — the same
mistake that once sliced an `#include` off a file. **Copy from a known line
number, never by pattern**; a line that looks like the start of the code is not
the start of the code.

A screening note claimed `OvlFunc_932_2009398` could not land without its two
file siblings because it calls one of them. `.thumb_func_start` expands to
`.global \sym`, so every function a hand-written `.s` defines is already
exported; the split worked and `make compare` after it confirmed so. Worth
checking rather than accepting — it would have blocked three functions.
