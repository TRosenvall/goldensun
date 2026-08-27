# Batch 97 — the r2/r3 exchange turns out to be reachable, and the lever is a type

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_801ebd8` | `0801ebd8` | main ROM | [rom_1de5c_c_c_c_a_a_c_a.c](../src/rom_15000/rom_1de5c_c_c_c_a_a_c_a.c) |
| `Func_8025180` | `08025180` | main ROM | [rom_23178_a_a_a_a_a_b.c](../src/rom_15000/rom_23178_a_a_a_a_a_b.c) |
| `Func_8079d1c` | `08079d1c` | main ROM | [rom_79460_c_c_c_c_a_c_c_a_c_b.c](../src/rom_77000/rom_79460_c_c_c_c_a_c_c_a_c_b.c) |
| `OvlFunc_946_200985c` | `0200985c` | ovl_7ced6c | [ovl_30_c_c_c_c_c_a_a_c_b.c](../src/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_a_a_c_b.c) |
| `OvlFunc_946_20098b0` | `020098b0` | ovl_7ced6c | same file |

One park added, three updated.

## The r2/r3 exchange is not always unreachable

Batch 96 named this class: four parked functions differing from the ROM by
nothing but which of r2 and r3 holds which value, having absorbed a dozen
spellings between them. This round found two functions in the same class that
**do** yield, and the levers are worth more than the functions.

**A named constant of the field's own type.** `OvlFunc_946_200985c` has

```
	ldrb	r2, [r1] / mov r3, #2 / orr r3, r2 / strb r3, [r1]
```

with the **constant** in the destination, not the loaded byte. `*q = 2 | *q`,
`*q |= 2`, `*q = *q | 2` and a named `int two` all put the loaded byte there.
This closes it:

```c
unsigned char two = 2;
*q = two | *q;
```

`int two` fails and `unsigned char two` works, on the same statement. It is the
**width** of the named constant that is the lever, not the naming — which is a
sharper thing than the tree had before.

**Statement order decides a register pair.** The same function loads two
coordinates and subtracts 1 from one of them. Writing the two assignments in the
order the ROM loads them took it from 9 differing of 39 to 2. Declaration order
does nothing.

## Two pointer chains, and why gcc collapses them

`OvlFunc_923_2009bc8` (parked, with its twin `OvlFunc_924_200d158`) went from 26
differing of 39 to 7 on two more orderings:

* **Both pointers must be computed before the first store.** The function writes
  bytes at +0x55, +0x22 and +0x23. Writing the first as a struct field and the
  rest through a walked pointer gives `add r2, #0x55 / strb / sub r2, #0x33 /
  strb` — one register walked *backwards*, because gcc notices 0x55 − 0x33 =
  0x22. The ROM keeps two independent chains. Naming two pointer locals is not
  enough; both have to be **live across the first store**.
* **The store order then decides which chain gets which register.** With both
  computed up front, storing +0x55 first gives the ROM's assignment; storing
  +0x22 first swaps them.

## Where the type lever does not reach

Tried on the other two members of the class and recorded as negatives:

* `2009458.c` — identical instruction shape, `unsigned char m = 0xf7` gives 4 of
  36 against the park's existing 3; dropping the intermediate gives 5.
* `200ab58.c` — `unsigned short` for the named constant changes nothing.

The cases that yielded both **store in the same statement as the mask**; the
ones that did not compute into a variable that crosses a join. That is the
distinction to test next, and it is now written into `docs/elevation.md`
alongside the lever.

## The return-constant hoist has two causes, not one

Batch 96 defeated the hoist on `OvlFunc_common0_18` by turning
`if (n == 0) return 0;` into the positive form. The obvious next move was the
`Func_80bf37c` family — one park standing for **six** functions, blocked on what
looked like the same thing.

It does not transfer. Four restructurings were measured and all are worse than
the park's existing 7 differing:

| form | differing |
|---|---|
| return 1 as the fall-through, call test nested | 18 |
| the same with the loaded byte in its own local | 18 |
| loaded byte `unsigned char`, working copy `int` | 20 |
| return 0 as the fall-through, two locals | 26 |

The likely distinction, now recorded in the park: `common0_18` has a single early
return against a long body, while `80bf37c` has **four** exits, two returning
each value, so gcc has a real choice about which constant to preload. The
park says not to spend another round on exit shapes there.

That function also still has one unexplained instruction — `ldrb r2, [r5] /
mov r3, r2`, a redundant register copy with r2 dead immediately after. Two-local
spellings do not produce it.

## Operational

`type-foo.sh` had a leftover edit: the delay argument was captured into an unused
variable and overridden by a hard-coded `0.5`, so `./type-foo.sh 100` silently
waited thirty seconds. The documented interface works again, with the default
left at half a minute so nothing about the loop's behaviour changes.
