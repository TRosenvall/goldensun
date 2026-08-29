# Batch 101 — jump tables open up, and a tool that lied about a file

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM | File |
|---|---|---|---|
| `Func_808b2b0` | `0808b2b0` | main ROM | [rom_8ace0_a_a_c_c_b.c](../src/rom_8a000/rom_8ace0_a_a_c_c_b.c) |
| `Func_808c44c` | `0808c44c` | main ROM | [rom_8ba38_a_a_b.c](../src/rom_8a000/rom_8ba38_a_a_b.c) |
| `SetCameraTarget` | `0809335c` | main ROM | [rom_93304_a_a_a_a_a_b.c](../src/rom_8a000/rom_93304_a_a_a_a_a_b.c) |
| `Func_80aa460` | `080aa460` | main ROM | [rom_a8604_c_c_c_c_a_c.c](../src/rom_a1000/rom_a8604_c_c_c_c_a_c.c) |
| `GetWeaponSpriteID` | `080b6eb4` | main ROM | [rom_b6eb4_a.c](../src/rom_b5000/rom_b6eb4_a.c) |

Three parked. Two rounds; the first produced three and was left open.

## Jump tables are now reachable, and there are 106 of them

`GetWeaponSpriteID` is the first jump-table switch matched in the tree.
gcc-2.96 turns a dense switch into a real table — `cmp / bhi / ldr =table /
lsl / ldr / mov pc, r3` followed by N `.word`s — and getting there needed two
things, both of which then reproduced on two more functions in the same round.

**The case values have to be dense enough.** `GetWeaponSpriteID`'s table has an
entry for case 4 even though 4 is not a case; its slot holds the default label.
Cases 0, 1, 2, 3 and 5 with nothing for 4 is what makes the range dense enough
that gcc prefers a table over the decision tree.

**The work goes inside each case, not after the switch.** Written as
`case 0: tbl = A; break; ... default: return r;` with one `r = tbl[type];` after
the switch, gcc emits a separate default block and comes out five instructions
long. Written as `r = A[type];` in every case, it cross-jumps the five identical
tails into one **and** lets the default fall straight out to the shared
`return r` — which is exactly what the table's default slot points at. Same
decision seen twice: give gcc identical tails and let the default do nothing.

**The case order in the source decides the block order.** Confirmed on both
`Func_80aa460` and `Func_808b2b0`. The ROM emits case 5's body before case 3's
in one and the 0x36 body before the 0x37 one in the other; writing the cases in
the other order costs four and two positions respectively. With a jump table the
bodies come out in source order, so the ROM's layout reads the source's case
order directly — including how the cases are **grouped**, since `case 4: case 7:`
sharing one body is visibly different from four separate cases that gcc merges.

`Func_80aa460` also needed a switch **fallthrough**: the ROM has one block
falling into the next, which is what gives element 1 two calls and element 0xB
one.

**106 functions with jump tables remain in `asm/`**, the smallest at 26
instructions. That is a class worth working systematically.

## Seven pooled constants that were area symbols

`Func_808b2b0` pools every one of its seven case values — `ldr r2, =0x38` where
`mov r2, #0x38` would encode. gcc never pools what an eight-bit `mov` can build,
so they are symbols, and `area.sym` already defines all six distinct values:
`_AREA_36`, `_AREA_37`, `_AREA_38`, `_AREA_39`, `_AREA_3a`, `_AREA_3c`.

The used set being exactly those six, **skipping 0x3b**, is what makes this a
reading rather than a guess.

## A tool that lied, and how it lied

`tools/split_s.py` refuses to convert a single-function `.s` that also carries
data, because deleting the file would take the data with it and the link would
fail much later. It told me `rom_b6eb4.s` held "only GetWeaponSpriteID and no
data". It was wrong twice over:

* it counted `.incbin` but **not `.incrom`**, and this file's five tables are
  `.incrom`;
* its stranded-label test looked only for labels the function never mentions —
  but a jump table's targets are **both referenced by the function and data**,
  so all five passed.

I deleted the `.s` on the tool's word and the link died on `.Lc2a46`. That is
the second time this check has been blind; batch 78 was `.lcomm`.

It now counts every blob directive, flags any `.section` appearing after the
last `.func_end`, and flags labels *defined* after the code regardless of
whether the function mentions them. Verified against the file that fooled it.

The lesson that survives the fix is in `docs/elevation.md`: **when the tool says
a file has no data, look at the tail yourself.** The failure surfaces as an
undefined-symbol link error that reads like a bad decompilation rather than a
bad split, which is why it costs disproportionate time.

## A global that is read once still wants a local

`SetCameraTarget` reads `iwram_3001e70` indirectly and the ROM hoists that load
above the null test. Written inside the block it lands after the test and costs
five positions. Same shape as the gState-base rule, but here the base is used
**once** — so the rule is about where the ROM puts the load, not about reuse.

## Parks

* **`Func_80a51d0`** — the ROM re-reads an indirect global five times; plain C
  keeps it in one register (47 against 49), and `volatile` forces all five but
  overshoots by one. The volatile spelling is deliberately **not** kept in the
  file: it is a lie about the object and it does not match either. The honest
  form is kept and the volatile result recorded so it is not re-derived.
* **`OvlFunc_929_2008598`** — 4 of 55, the interleaved shifted-constant argument.
  Two pieces of gcc's own arithmetic in it look like source decisions and are
  not: the stored 0x209 is derived from the 0x1c0 offset already in a register,
  and the 0x1c2 gState offset is then derived from that.
* **`OvlFunc_common2_380`** — gcc addresses two stack objects through `sp` where
  the ROM copies `sp` to a register first. Naming the pointers gets the length
  right and halves the difference.
