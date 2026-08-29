# Batch 96 — two blockers defeated, and a class named

Verified from a clean build: `make clean && make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`.
Every address below was read back out of the linked ELF with `arm-none-eabi-nm`.

| Function | Address | ROM / overlay | File |
|---|---|---|---|
| `Func_80b8f08` | `080b8f08` | main ROM | [rom_b8228_c_a_c_c_a_b.c](../src/rom_b5000/rom_b8228_c_a_c_c_a_b.c) |
| `OvlFunc_957_2008d90` | `02008d90` | ovl_7e3e08 | [ovl_30_c_c_a_c_c_c_c_c_c_c_a_c_b.c](../src/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_a_c_b.c) |
| `OvlFunc_948_2009f78` | `02009f78` | ovl_7d30e0 | [ovl_30_c_c_c_c_c_c_c_c_b.c](../src/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_b.c) |
| `OvlFunc_881_2008030` | `02008030` | ovl_77a7c8 | [ovl_30_a_a_a_b.c](../src/overlays/rom_77a7c8/ovl_30_a_a_a_b.c) |
| `OvlFunc_922_2009fac` | `02009fac` | ovl_7a8c8c | [ovl_30_c_c_c_c_c_c_c_a.c](../src/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_a.c) |

Two parked, and both parks carry more than the function.

## The call-in-each-arm lever closes a function

Batch 95 found this on a park: when the ROM branches over a choice of two nearby
constants and gcc insists on if-converting it, moving the **call** inside the
arms blocks the conversion, and cross-jumping then merges the tails back into
the ROM's single `bl`.

`Func_80b8f08` is the second use and the first to close. `Func_80b6b40(id > 0x7f ? 2 : 1, buf)`
gets if-converted; written as two calls in two arms it went from **29 differing
of 40 to 2**, and the last two were the multiply's operand order.

That multiply is worth a note of its own. Thumb's `mul rd, rs` is two-operand,
so one input becomes the destination — and unlike a commutative `and` or `orr`,
**the source's operand order decides which**. `Random() * n` gives the ROM,
`n * Random()` gives the reverse. Four parked functions say the same question on
a bitwise op is not reachable; multiply and mask behave differently.

## The positive test stops the return-constant hoist

`OvlFunc_common0_18` is parked, but it defeated the blocker that parked
`Func_80bf37c` in batch 89 **and five shape siblings with it**.

Written as an early return, `if (n == 0) return 0;` lets gcc hoist the
`mov r0, #0` above the test. The ROM keeps it in the else block. Turning the
test around —

```c
if (n != 0) { ... return n; }
return 0;
```

— puts it back: **30 differing of 40 down to 8 of 41**. This is the first time
that hoist has been defeated, and the whole `Func_80bf37c` family is worth
re-screening against it.

## The r2/r3 exchange is now a named class with a lead

Four parked functions differ from the ROM by nothing but which of r2 and r3
holds which value, identical instructions in identical order:

| file | shape | differing |
|---|---|---|
| `ovl_7ed0a0/2009458.c` | masked byte | 3 of 36 |
| `rom_b0000/80b2ed8.c` | pooled constant | 19 of 46 |
| `ovl_7b9cb4/200ab58.c` | walked pointer | 7 of 35 |
| `ovl_common/common0_18.c` | masked byte | 8 of 41 |

Between them they have absorbed a dozen spellings — operand order, declaration
order, named intermediates, walked versus indexed pointers, signed versus
unsigned fields — and none of it moves the pair.

**The lead:** `src/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c_c.c` has the *identical*
four-instruction masked-byte sequence and **matches**. The difference is what
follows — there the `and` result feeds an `orr` before being stored, giving it a
longer live range; in `common0_18` it is stored immediately. If the allocator is
splitting on live-range length that is testable, and it would cover at least two
of the four.

## Smaller readings

**Three pooled small constants were area symbols.** `OvlFunc_948_2009f78`
compares against `ldr r3, =0x75` where `mov r3, #0x75` encodes — gcc never pools
what an eight-bit `mov` can build, and always pools a symbol's address.
`area.sym` already defined `_AREA_75`, `_AREA_76` and `_AREA_78`.

**A signed-char read has to be held in an `int`.** `OvlFunc_957_2008d90` does
`ldrb / lsl #24 / asr #24` where a `signed char` local gives `ldrsb` in two
instructions — and costs a second load later, because the narrow type will not
stay in one register across a call. The type of the **local** decides it, not
the cast at the load; both `*(signed char *)p` and
`(signed char)*(unsigned char *)p` work once the local is `int`.

**Two wram symbols 0x64 apart share one pool entry.** `OvlFunc_922_2009fac`
loads `iwram_3001f30` and reaches `iwram_3001ecc` with `sub r3, #0x64`. Naming
both externs gives two pool loads, because gcc cannot know two unrelated externs
are related. Writing the second as an offset from the first supplies the
relationship. That is ugly C and almost certainly not what Camelot wrote — the
honest reading is one object spanning both addresses — but it is what reproduces
the bytes, and it is recorded that way in the file.

**`__divsi3 = _divsi3_RAM;` was added to `overlays/rom_77a7c8/overlay.ld`.** The
established alias for a C division inside an overlay; emits no bytes.

## Two more comments that failed their own control

Continuing the discipline from batch 95. Both of these were written from reading
the assembly and then tested:

* `OvlFunc_881_2008030` — hoisting the `*r * 9 / 10` limit into a named local
  looked load-bearing while the gState fold was still in place. Once the base is
  a local, the inline form matches on its own.
* `Func_80b8f08` — batch 95's operand-order rule says a base-first
  register-offset load means pointer arithmetic, so the final read was written
  that way. Both `buf[i]` and the pointer form give the same forty instructions
  here. The rule held where the base was an extern pointer; a **stack array** is
  not the same case.

Both sources are now the simpler spelling, with the negative recorded. That is
six such corrections in five batches, and the reason the rate has dropped is
that the check now happens before the claim is written rather than after.
