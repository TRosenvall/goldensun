# Batch 253 — four parks, four wrong diagnoses

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_883_200834c` | `0x0200834c` | [ovl_30_a_a_c_a.c](src/overlays/rom_780898/ovl_30_a_a_c_a.c) |
| 2 | `OvlFunc_905_200834c` | `0x0200834c` | [ovl_30_a_a_a_c_c_a.c](src/overlays/rom_799abc/ovl_30_a_a_a_c_c_a.c) |
| 3 | `OvlFunc_913_200834c` | `0x0200834c` | [ovl_30_a_a_a_c_c_a.c](src/overlays/rom_7a04ac/ovl_30_a_a_a_c_c_a.c) |
| 4 | `OvlFunc_914_200834c` | `0x0200834c` | [ovl_30_a_a_c_c_a.c](src/overlays/rom_7a1ff0/ovl_30_a_a_c_c_a.c) |
| 5 | `OvlFunc_915_200834c` | `0x0200834c` | [ovl_30_a_a_c_a.c](src/overlays/rom_7a2bf0/ovl_30_a_a_c_a.c) |
| 6 | `OvlFunc_923_2008630` | `0x02008630` | [ovl_314_a_c_c_a.c](src/overlays/rom_7aa430/ovl_314_a_c_c_a.c) |
| 7 | `OvlFunc_924_2008630` | `0x02008630` | [ovl_314_a_c_c_a.c](src/overlays/rom_7ac2d8/ovl_314_a_c_c_a.c) |
| 8 | `OvlFunc_927_200834c` | `0x0200834c` | [ovl_30_a_a_c_c_a.c](src/overlays/rom_7b4558/ovl_30_a_a_c_c_a.c) |
| 9 | `OvlFunc_934_2008630` | `0x02008630` | [ovl_314_a_a_c_a.c](src/overlays/rom_7bdeb0/ovl_314_a_a_c_a.c) |
| 10 | `OvlFunc_946_200834c` | `0x0200834c` | [ovl_30_a_a_c_a.c](src/overlays/rom_7ced6c/ovl_30_a_a_c_a.c) |
| 11 | `OvlFunc_947_2008630` | `0x02008630` | [ovl_314_a_c_c_a.c](src/overlays/rom_7d0e88/ovl_314_a_c_c_a.c) |
| 12 | `OvlFunc_948_200834c` | `0x0200834c` | [ovl_30_a_a_a_c_c_a.c](src/overlays/rom_7d30e0/ovl_30_a_a_a_c_c_a.c) |
| 13 | `OvlFunc_957_200834c` | `0x0200834c` | [ovl_30_a_a_a_c_c_a.c](src/overlays/rom_7e3e08/ovl_30_a_a_a_c_c_a.c) |
| 14 | `OvlFunc_958_2008630` | `0x02008630` | [ovl_314_c_c_a.c](src/overlays/rom_7e636c/ovl_314_c_c_a.c) |
| 15 | `OvlFunc_959_200834c` | `0x0200834c` | [ovl_30_c_c_a.c](src/overlays/rom_7e7574/ovl_30_c_c_a.c) |
| 16 | `OvlFunc_964_200834c` | `0x0200834c` | [ovl_30_a_a_a_c_c_a.c](src/overlays/rom_7ed0a0/ovl_30_a_a_a_c_c_a.c) |
| 17 | `OvlFunc_965_200834c` | `0x0200834c` | [ovl_30_a_a_a_c_c_a.c](src/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_a.c) |
| 18 | `OvlFunc_923_20091b4` | `0x020091b4` | [ovl_1150_c_c_a.c](src/overlays/rom_7aa430/ovl_1150_c_c_a.c) |
| 19 | `OvlFunc_895_20087d0` | `0x020087d0` | [ovl_30_c_c_c_a_a.c](src/overlays/rom_78dee8/ovl_30_c_c_c_a_a.c) |

Nineteen functions, all whole-file landings — no split, no linker edit, no flag
rule anywhere. Seventeen came from a single park.

## Parks retired

| park | covered | was |
|---|---|---|
| `overlays/200834c.c` | **17 functions** | 28 differing, since batch 91 |
| `ovl_78dee8/20087d0.c` | 1 (TryPushBlock) | 6 instructions short |
| `ovl_7aa430/20091b4.c` | 1 | 5 differing |

## Parks deleted as stale

`overlays/2008b2c.c` and `ovl_7b0400/200b1c0.c` — both subjects already elevated,
verified defined-once under `src/` and absent from `asm/`.

## Park improved, not closed

`rom_15000/8021390.c`: **37 → 5 of 97**, and its central claim disproved.

## EVERY PARK THIS BATCH HAD NAMED THE WRONG MECHANISM

That is the finding, and it is now four for four:

| park | recorded blocker | what it actually was |
|---|---|---|
| `200834c` | instruction scheduling | **register allocation**, one pass earlier |
| `20087d0` | allocator uses one high register where the ROM uses three | it used **fewer** — backwards |
| `20091b4` | emission order of two pooled builds | a **register WAW**, not a memory dependence |
| `8021390` | "no source form separates them" | a **third pin device** exists |

> A park's recorded blocker is a hypothesis, not a finding. The ones that fall
> are the ones where nobody re-tested the diagnosis.

## `200834c`: THIRTEEN COPIES BECAME SEVENTEEN, BY DELETING TWO LOCALS

`find_twins.py` had ranked this family first by payoff since the tool existed,
and it had sat at 28 differing since batch 91 with thirteen `-f` flags failing to
move it.

Three things from the `-da` dumps show the pass was misidentified:

- **There is no `.sched` dump at all.** gcc-2.96/thumb runs no sched1 at `-O2` —
  which is exactly why `-fno-schedule-insns` measured inert. The flag had nothing
  to turn off.
- At `.18.greg` the order is **already the ROM's**. Nothing has been hoisted yet.
- The ROM reuses one low register where the park's C used two, and that reuse is
  an anti-dependence pinning the second load behind the first chain.

> **A load the scheduler appears to have hoisted can be a scheduler CONSEQUENCE
> of an allocator CAUSE. Diff `.18.greg`, not just the final `.s`. If the ROM
> reuses one low register where you use two, the lever is one local too many.**

Both source changes were **removals** of a named local — 28 → 5 by un-naming a
step word's high half, 5 → 0 by un-hoisting a model id. **No pin, no union, no
flag, and zero register pins across all seventeen files.** Worth noting against a
week in which nearly every lever was pin-shaped.

The family is **seventeen, not thirteen**: a normalised-stream sweep found four
more identical copies that `find_twins.py` misses because their table symbol
already carries a real name. And the park's claim that elevating "means renaming
the tables per overlay" was wrong — the asm-label idiom reaches them with no
other file touched.

## READ THE SIGN OF `-fno-schedule-insns2`, NOT JUST THE NUMBER

The same routine family gave **opposite verdicts on the same day**, and one
compile separates them.

| function | flag effect | verdict |
|---|---|---|
| the 18-copy block-push park (batch 252) | **improves** 7 → 4 | sched2 owns the residue; alias reached it |
| `20087d0`, same routine | **worsens** 44 → 63, 10 → 37 | sched2 already right; alias is the wrong axis |

On `20087d0` all **twenty** one-member-union subsets over the seven candidate
fields measured **exactly 44** — not one encoding moved. That is what "wrong
axis" looks like, and family membership did not predict it.

## A THIRD PIN DEVICE

The recorded devices are the **ordering** pin and the **eviction** pin.
`8021390` needs neither:

| spelling | differing |
|---|---|
| park's best | 37 of 97 |
| pin on the body zeros **alone** | 11 |
| a second plain local for the call argument **alone** | 37 (inert) |
| **both** | **5** |

The pin **places nothing**. It stops cprop folding the two locals back into one
pseudo, which is what lets the split survive to reload — exactly what the park
concluded was impossible.

> **Tell:** you split a value into two locals, the split measures inert, and the
> dumps show one pseudo where you wrote two. Pin one and re-measure.

Worth re-screening the class's other recorded instances, `OvlFunc_952_200be40`
and `OvlFunc_891_2008098`, and the sibling `Func_8021488`.

## `20091b4`: THE DECIDING EDGE IS A REGISTER WAW

Two pooled builds tie at priority 68, same cost, both class 3, so
`rank_for_schedule` falls to **dependent count**, 3 against 2. The extra edge is
`REG_DEP_OUTPUT` — the store's *value* register colliding with the offset load's.

Both recorded dependent-count levers move **memory** dependences. **Alias was the
right class to reach for and the wrong tool.** The count cannot be equalised
either: every spelling yielding the ROM's instruction triple sits at 5, and every
spelling killing the dependence flips gcc to a different addressing form.

The remedy was the **empty `asm` as a region splitter** — a third distinct use,
after the ordering barrier and batch 252's cross-jumping use. It does not win the
tiebreak, it bypasses it.

**A boundary this function demonstrates both halves of:** the barrier can fix a
**reorder**, never an **interleave**.

## SECOND INSTANCE OF THE RELOCATION-PLACEHOLDER SIGNATURE

`20091b4` lands with `objcmp` reporting one differing encoding and an ours-only
`R_ARM_ABS32 _AREA_35`, because the ROM's pooled word is a **linker-defined
symbol**. `make compare` is green. The literal is again the evidence the symbol is
right: `0x35` emits `mov r0, #0x35` for 80 bytes against 84.

After batch 252's `_AREA_3a`, this is a recognisable pattern rather than a
one-off: **a pooled constant below 256 is a tell that the source referenced a
symbol.**

## TWO CORRECTIONS TO THIS ROUND'S OWN TARGETING

**The assigned target did not exist.** `arg_interleave_flat.c` — picked as a
14-function class park — was closed in **batch 200**; all fourteen members are
elevated and the file survives only as a record. Verified by sampling five, all
absent from `asm/`. The agent pivoted and closed `20091b4` instead, so nothing
was lost, but the selection was wrong.

**Stale-park counts disagree and none is authoritative.** A survey flagged 4, a
tool reports 17 on a different criterion. Checked by hand: one was a **false
positive** (subject still in `asm/`; the scan matched only elevated callees), one
is **unresolved** (its `.s` is gone, but the evidence was a grep that cannot tell
a prototype from a definition). Only **two** were deleted.

## THE FAMILY VEIN IS NOW SPENT

Re-running `find_twins.py` after this batch: **6 groups, 6 functions reachable,
largest group 2 duplicates**, and 5 of the 6 are already parked. The 18-copy and
17-copy families were the last large ones. The class-park leverage behind batches
252–253 is **not repeatable at that scale**, and the next rounds should expect
ordinary rates.

## KNOWN TOOLING BLIND SPOT

`close_parks.py` screens **76 of 517** parks; `rank_parks.py` reports **445
skipped (no reference)**. Both because a park's recorded `ref` path goes stale the
moment its `.s` is split — the failure mode `docs/elevation.md:4326` already
records. The park corpus is largely invisible to automated screening, which is
plausibly why four wrong diagnoses went unchallenged for so long.

## Still held back

`OvlFunc_968_200c048` and `OvlFunc_968_200c520` (matched and verified; `200c2bc`
sits mid-`.s` at a real floor of 2 of 271) and `OvlFunc_933_2009874` (matched;
needs a five-way cut where the tool does three). Candidates preserved, parks in
place.

`8021390` is **not** landed: closing it needs `_MSG_1b = 0x1b;` added to
`message.sym`, which is a build-input change rather than a new `.c`, and is moot
until the function actually closes.

## Gate

`make clean && make -j8 && make compare` green, `goldensun.gba: OK`. Every address
checked against the linked ELF. Pairing 4,648 sources; `--orphans` 0;
`--unlinked` unchanged at 10.
