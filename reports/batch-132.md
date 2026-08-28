# Batch 132 — a screen that lied, and a rule of mine that was wrong

Verified on a clean `make clean && make compare` — `goldensun.gba: OK` — with
every address read out of the linked ELF and the new `_MSG_f76` symbol confirmed
at 0x0f76.

## Elevated (2214 → 2208)

| function | address | notes |
|---|---|---|
| `OvlFunc_946_200967c` | 0x0200967c | name cheap constants, not pooled ones |
| `OvlFunc_945_2008728` | 0x02008728 | `CSE_CFLAGS` + HImode literal |
| `OvlFunc_885_20080dc` | 0x020080dc | symbol base + no-prototype + explicit `-O2` |
| `OvlFunc_966_2008158` | 0x02008158 | two base variables + no-prototype |
| `OvlFunc_921_2008974` | 0x02008974 | no-prototype |
| `OvlFunc_926_2008f80` | 0x02008f80 | first screen |

## A green screen and a red build

`OvlFunc_885_20080dc` screened **exact** and its linked overlay differed in
**18 bytes** — all argument-order swaps at four call sites. The cause was the
wildcard hazard that had been sitting in the owed list: the new file is caught
by `rom_78603c/ovl_30_c_c_a_c_a%`, which applies `O1_CFLAGS`.

**`tryc.py` cannot see a wildcard.** It compiles with the flags for the file's
own path, and a file that does not exist yet has no path in the Makefile — so
the screen used `-O2` while the build used `-O1`. The screen was right about the
C and wrong about the build.

Worse, adding the explicit `-O2` rule **did not fix it**, and for a few minutes
that looked like evidence against the diagnosis. The Makefile is not a
dependency of the `.o`, so make reported the object up to date and never ran the
new recipe. Deleting `asm/<path>.o` made the same rule work immediately.
`make -n <the .o>` saying "is up to date" is the tell.

`tools/pool.py` now prints a **wildcard column** for every candidate. Control:
it flags the `O1` case that bit and clears a known-clean path; there are 361
pattern rules with non-default flags. And the standing owed item is now measured
rather than remembered — **exactly 12 of 1152** unelevated `.s` files sit under
one.

## A discriminator of mine that was wrong

Batch 130 recorded that the no-prototype lever cannot work when the callee
appears in two arms of a branch with different argument orders, because "no
single declaration choice can satisfy both".

`OvlFunc_921_2008974` is exactly that case — `mov r0,#0xc / mov r1,#0xf` in one
arm and `mov r1,#0xe / mov r0,#0xc` in the other — and dropping the single
prototype matches **both**. gcc's unprototyped ordering is not one fixed order;
it varies per call site the way the ROM's does.

The lever still fails on `OvlFunc_952_20085a4`, but not for that reason, and
what does decide it is unknown. The correct rule is: **try it, it is one
screen.** The park has been corrected.

## Name cheap constants; leave pool constants inline

The naming lever works because gcc rematerialises a named constant at each use.
That holds for `mov`, `mov`+`lsl`, `mov`+`neg` — not for one needing a pool
load, where rematerialising is expensive enough that gcc holds the value
instead.

`OvlFunc_946_200967c` has four arms each storing `0x19999` and each passing
`0xf2 << 18`. Naming all eight gave **65 of 82 differing** with three extra
pushes. Naming only the shifted builds was exact. *If the ROM reaches the value
with `ldr rN, =…`, do not name it.*

## The offset register reused as the stored value

When the ROM materialises a store address where gcc folds to reg+reg, look at
what is stored: if the ROM reuses the **offset** register as the **value**,
write the offset variable as the value.

    off -= 0x3c;
    p = (int *)(base + off);
    off = 0x18;          /* the offset variable becomes the value */
    *p = off;

Worth 49 differing to 21 on `OvlFunc_904_2008054` (still parked). Same mechanism
as the named-pointer rule, reached from the other side.

## Selection

Five parks this batch, four of them straight-line functions where both naming
levers are inapplicable by construction. `pool.py` prints `br`, and **br == 0
with any repeated constant or unguarded interleave site is a park before the
first screen**. I did not read the column on four occasions and it cost four
rounds' worth of screens.

## Still owed

- 12 `.s` TUs under non-default wildcards (now measured, and detectable).
- ~3,300 lines of duplicate Makefile rule blocks.
- 293 parks, deferred by request until the fresh pool thins.
