# Batch 108 — find the family, then the family is cheap

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of its overlay's linked ELF with
`arm-none-eabi-nm`.

| Function | Address | Overlay | File |
|---|---|---|---|
| `OvlFunc_937_2008240` | `02008240` | ovl_7c3044 | [ovl_30_c_c_c_c_c_c_a.c](../src/overlays/rom_7c3044/ovl_30_c_c_c_c_c_c_a.c) |
| `OvlFunc_936_2008464` | `02008464` | ovl_7c097c | [ovl_30_c_c_c_a_a_c_a_a_b.c](../src/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_a_b.c) |
| `OvlFunc_949_2008644` | `02008644` | ovl_7d4af4 | [ovl_30_c_c_a_c_c_c_c_c_c_c_b.c](../src/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_c_b.c) |
| `OvlFunc_939_2008b6c` | `02008b6c` | ovl_7c460c | [ovl_314_a_c_c_a_c_a_b_b.c](../src/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_b_b.c) |
| `OvlFunc_917_2009218` | `02009218` | ovl_7a4370 | [ovl_30_c_c_c_c_a_a_a.c](../src/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_a.c) |
| `OvlFunc_911_200a608` | `0200a608` | ovl_79e5c0 | [ovl_30_c_a_c.c](../src/overlays/rom_79e5c0/ovl_30_c_a_c.c) |
| `OvlFunc_913_200a7c8` | `0200a7c8` | ovl_7a04ac | [ovl_30_c_c_c_a_c.c](../src/overlays/rom_7a04ac/ovl_30_c_c_c_a_c.c) |

**2487 functions remain in assembly, 229 parked.**

## The method that made this batch: search for the OPENING, not the shape

Two of the seven were solved the usual way — read the assembly, apply levers,
iterate. **The other five came from grepping every `.s` for the first six
instructions of a function that had just been solved.**

```python
pat = re.compile(r"\.thumb_func_start (\S+)\n\tpush\t\{r5, r6, r7, lr\}\n"
                 r"\tldr\tr3, =iwram_3001ebc\n\tldr\tr7, \[r3\]\n"
                 r"\tbl\t__CutsceneStart\n\tmov\tr5, #8\n\tmov\tr6, #0\n")
```

Six hits. Four are now C, and each cost a constant substitution plus whatever
its own tail does differently. The same trick on `OvlFunc_913_200a7c8`'s opening
returned three functions and all three are now C.

`tools/find_twins.py` and `tools/find_families.py` exist and look for *shape*
similarity across whole functions. A literal prefix match is cruder and it found
things they had not surfaced, because these functions are only identical for
their first six instructions and diverge freely afterwards. **When a function
matches, spend two minutes grepping for its prologue before moving on.**

The two families:

* **Falling debris** (3 functions): mask `iwram_3001e40`, spawn an actor, clear
  one sprite bit, set another, send it down. Needs CSE_CFLAGS *then* the
  basic-block lever — the batch-106 order.
* **Map exit** (4 of 6 functions): clear the "busy" byte on actors 8..0x41 in a
  loop, look the entrance up in an eight-byte-per-entry table, run its script,
  walk the player out.

## Two levers the map-exit family needs

**The loop induction variable has to be hoisted out of the `for` init.**

```c
z = 0;  for (i = 8; i <= 0x41; i++)     /* mov r6,#0 then mov r5,#8 */
i = 8;  z = 0;  for (; i <= 0x41; i++)  /* mov r5,#8 then mov r6,#0 -- the ROM */
```

Two instructions, and there is nothing clever about it: gcc emits the `for`'s
initialiser where the loop begins, so a statement written before the `for` comes
out first. Writing the init as its own statement puts it back in source order.

**The entry's two halfword fields must be NAMED; the script pointer must not.**

```
rom    add r3, r4, #4 / ldrh r1, [r0, r3] / add r3, r0 / ldrh r2, [r3, #2] / ldr r0, [r0, r4]
ours   ldr r0, [r2, r3] / add r3, #0x4 / ldrh r1, [r2, r3] / ...
```

`ea = t[n].a; eb = t[n].b; f(t[n].script, ea, eb);` matches. Naming the **table
base** instead — a `struct Entry *t` assigned before the loop, which is the
basic-block lever's usual shape — is 71 lines against 67 and 70 differing, much
worse, because it makes the pointer live across the loop.

That is the batch-107 carried-vs-rebuilt rule choosing *carried*: the two
halfwords are consumed immediately by one call, so they want adjacency. The
table base is not a value the ROM carries at all.

## A commit message that got ahead of the work

`OvlFunc_911_200a608` screened OK and I wrote it into a commit message as
elevated — but I had only screened it, not split the `.s` and written the `.c`.
The build stayed green because nothing had changed. It is corrected in the
following commit, and the correction is in the log rather than amended away.

The process gap is specific: **screening and wiring are two steps and only the
second one moves the tree.** `make compare` cannot catch the difference, because
a function that has not been wired is still assembly and still builds. What does
catch it is `grep -rh "^\.thumb_func_start" asm/ | wc -l` before and after.

## Parks

**`OvlFunc_931_2008d08`** — 7 of 34, and all seven are the same instruction with
r2 and r3 swapped. Seventh member of the r2/r3 exchange class and the cleanest
minimal case of it so far: 34 instructions, one block of interest, three stores.
Four spellings measured.

**`CutsceneStart`** — 52 of 64, and the instruction *forms* are all right. Four
things were solved getting there and all four generalise:

* **The ROM's three-operand `add` means a named pointer**, at eight sites.
  `*(short *)(p + off) = z;` with `off` a variable gives the register-offset
  `strh r5, [r6, r3]` — one instruction where the ROM has two.
  `s = (short *)(p + off); *s = z;` gives the ROM's pair.
* **`0xffff` needs an `unsigned short *`.** Three consecutive halfword stores
  write 0xffff, -1, -1. Through a `short *` the first converts to -1 and gcc
  merges all three into one pool load of `0xffffffff`; the ROM has
  `ldr r3, =0xffff` for the first and `mov r3, #1 / neg r3, r3` for the others.
* The two -1s are **rebuilt**, not carried.
* **`gState` is an absolute symbol** (`wram.sym`: `gState = 0x02000240`), so a
  reference can canonicalise as `=gState` or `=0x2000240` depending on whether
  gcc emits a relocation or the folded value, and `tryc.py` counts that as a
  differing line. Four spellings of the read were tried and all four are the
  same 52 — worth knowing before chasing it.

The residue is gcc deriving three nearby field offsets from each other
(`add r1, #0x10`) where the ROM rebuilds each with its own `mov`/`lsl`. Same
constant-reuse family as the 1000+ instruction park.

**`OvlFunc_898_2008e0c`** — a negative recorded rather than a park changed. The
carried-vs-rebuilt rule predicts that levering the zero should put `mov r1, #0`
after `mov r0, #0x13`, which is the ROM's order. It does not: same 2 of 41. The
park's existing bound — a fill-order mismatch on the first call after a join with
*differing predecessors* — is stronger than the placement rule rather than a
special case of it.
