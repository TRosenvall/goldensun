# Batch 262 -- nine functions, including the family that defeated batch one

Gated on a clean `make clean && make -j8 && make compare`, green at the target
SHA1 `5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked ELF with `tools/checkaddr.py` — and four of the nine are **named symbols**
(`CheckEquipmentCritBoost`, `GetFlag`, `SetFlag`, `ClearFlag`), which is the case
that check exists for — the `Func_*` names carry their own answer.

| | |
|---|---|
| elevated | **9** |
| parks retired | 5 |
| whole-file conversions | 4 (three of them multi-function) |
| splits | 0 |
| build-input changes | 0 |
| fakematch debt added | 4 functions |

A six-agent round; five closed, one produced a proof.

## What landed

| | function | address | source |
|---|---|---|---|
| 1 | `OvlFunc_965_200a6fc` | `0x0200a6fc` | [ovl_30_c_a_c_a_a.c](src/overlays/rom_7ef4f4/ovl_30_c_a_c_a_a.c) |
| 2 | `OvlFunc_882_2008d5c` | `0x02008d5c` | [ovl_30_…_c_c_c_c.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a_c_c_c_c.c) |
| 3 | `Func_80170c4` | `0x080170c4` | [rom_15e8c_c_a_a_a_c_c.c](src/rom_15000/rom_15e8c_c_a_a_a_c_c.c) |
| 4 | `Func_80170f8` | `0x080170f8` | [rom_15e8c_c_a_a_a_c_c.c](src/rom_15000/rom_15e8c_c_a_a_a_c_c.c) |
| 5 | `Func_8017248` | `0x08017248` | [rom_15e8c_c_a_a_a_c_c.c](src/rom_15000/rom_15e8c_c_a_a_a_c_c.c) |
| 6 | `CheckEquipmentCritBoost` | `0x08079cbc` | [rom_79460_…_a_c_a.c](src/rom_77000/rom_79460_c_c_c_c_a_c_c_a_c_a.c) |
| 7 | `GetFlag` | `0x08079338` | [rom_79338_a.c](src/rom_77000/rom_79338_a.c) |
| 8 | `SetFlag` | `0x08079358` | [rom_79338_a.c](src/rom_77000/rom_79338_a.c) |
| 9 | `ClearFlag` | `0x08079374` | [rom_79338_a.c](src/rom_77000/rom_79338_a.c) |

## The flag accessors

`docs/elevation.md` opens by recording that the **first batch ever attempted on
this project** took five small flag accessors from this family and matched none of
them. All three functions in `rom_79338_a.s` are now exact, pool placement
included.

The blocker was never "which register the shift lands in". It is **`combine_regs`**:
`*thumb_ashlsi3` has two alternatives, so `must_match_0` stays −1 and
`block_alloc`'s tie loop does not skip operand 1 — which carries a `REG_DEAD` note,
so `combine_regs` ties the shift result into the parameter's quantity holding
`qty_phys_copy_sugg = r0`. The same tie fires again on the right shift. Two ties,
one quantity, one register.

> The cure is to break the two ties **separately and by different means**. An empty
> `asm volatile` reading the parameter *after* the shift kills the first
> `REG_DEAD`; making the byte index a **hard register** kills the second, because
> `combine_regs` returns 0 whenever the source is a hard reg.

And the prerequisite was worth more than the pins: `gFlags[i] & bit` puts the load
first and gcc hoists the address computation ahead of the mask (11 of 13). Writing
`b & bit` — **value first, mask second** — reaches 3 of 13 with no pins at all.

`rom_79338_c_a.s` holds **five more accessors parked on the identical blocker** and
should now be a mechanical re-run.

## Two recorded rules corrected

**"Naming the mask is catastrophic" (batch 142) is TYPE-specific.**

```
int mask             47 differing
short mask           47 differing
unsigned short mask  EXACT
```

The front end's bitwise shortening narrows `unsigned short & unsigned short` to a
HImode AND, which is what lets the ROM's *two* `ldrh` of one slot survive — the
guard load is `(mem:HI)`, the argument load is `(zero_extend:SI (mem:HI))`, so CSE
keeps both. With `int` the AND stays SImode, CSE folds them, and the allocator
drops a spill.

**`check_dbra_loop`'s escape only fires if the hoist happens in the SAME loop
pass.** `loop_optimize` runs twice. Here the ascending `for` *did* reverse the
loop — in pass 1 — while the movable only cleared its threshold in pass 2, so the
hoist landed behind the synthesised init and the escape silently failed.

> A movable that is "not desirable" in pass 1 and moved in pass 2 is usually **one
> luid short**, and the missing luid is an insn the previous pass deleted.

`threshold` 23, `savings` 1, `insn_count` 27: `23 < 27` fails, `46 ≥ 27` passes.
The whole park hinged on one luid.

## Mechanisms

**Only TAG IDENTITY separates alias sets.** A `COMPONENT_REF` takes the *field's*
set, so one tag carrying both bytes is worth nothing — 4 differing, same as no tag.
Two distinct tags: 2. Adding an `int` member to the shared tag is inert. With a
trap attached: **ARM's `STRUCTURE_SIZE_BOUNDARY` is 32**, so a one-`char` struct has
`sizeof` 4 and `f += 0x23` emits `adds r6, #140`. A tag adopted for its alias set
must not own the pointer arithmetic.

**The class test decides before the dependent-count tie-break.** Two insns tied on
priority; one anti-dependent (class 2), one memory-dependent at cost 2 (class 1).
The higher class won *before* dependent count, so the constant took the slot
despite the load having five dependents to its two.

**"Right shape, two hard registers exchanged" is an allocno-priority tie** — the
levers are reference count and live length, not control flow. Deleting an
intermediate local and reassigning the *parameter* lengthened one live range 14→16
and shortened the other 12→11: same reference counts, opposite verdict.

**A ternary is one instruction shorter than a diamond.** `*p = c ? A : B;` hoists
the then-arm's pool load above the `cmp`; an `if/else` with the identical store in
both arms is merged back by cross-jumping at zero cost. 105 → 7.

**Branch polarity is the THEN ARM, not the condition.** All three parked attempts
wrote the inner then arm as the `return`; writing the *call* there flips the
fall-through. The condition, constants and instruction count were already correct.

**Operand order in a commutative `add` is set by the C front end.**
`build_binary_op` rewrites `int + pointer`, so that residue is unreachable while
the expression is pointer arithmetic.

**A literal stored to a `vu16` never reaches the shiftable-constant split**, so a
ROM building a *halfword* store's value with `mov`+`lsl` is telling you the value
came from an `int` local. Worth 45 → 3.

## The one that did not close, and why that is useful

`OvlFunc_957_2008a54` parks at **3 of 48 with a proof**, not another unbeaten tie.

> A store whose operands die in it and which has no in-block dependent has
> `INSN_PRIORITY` **1** — the floor — because `add_branch_dependences` links it to
> the block-ending branch with `REG_DEP_ANTI` and `arm_adjust_cost` returns 0 for
> anti-dependences.

That makes a two-sided contradiction: the ROM needs `p(shift) > p(store)` to win an
earlier contest and `p(store) > p(shift)` to win a later one. And each device is
unreachable **by construction** — the alias device needs the store to be the
*producer* of the new edge (here the conflicting read precedes it), there is no
anti-edge to promote, and the block ends at the first compare so any added
dependent is a real instruction.

> Before spending a round on an adjacent pair, check that the device you intend has
> somewhere to attach.

Its park file had been **comment-only** — the 3-differing source was lost. It now
ships with a body, so the next attempt starts from the residue.

## State

| | |
|---|---|
| matched | **4,323** (76.4% of the 5,655 elevatable) |
| remaining, hand-written thumb | 1,332 |
| &nbsp;&nbsp;parked | 446 |
| &nbsp;&nbsp;UNATTEMPTED | 886 |
| park files | 501, resolving to 470 distinct subjects |

Pool went **1,341 → 1,332**, exactly -9.

## Park-record accuracy

Two more wrong records this batch: `GetFlag`'s "2 differing" was really 11 of 13 (2
was only ever true under `--no-sched2`), and `2008a54`'s park had no body at all.
One park had a **correct class that was still useless** — "a copy elided at a
shared exit" described the symptom exactly while pointing at the wrong lever
family, since the defect was one register exchange counted eight times.

Running total: **eleven** parks with a correct count and a wrong class, against
four with a wrong number.
