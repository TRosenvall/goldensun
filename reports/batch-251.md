# Batch 251 — the metric read backwards from the zero end, four more times

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_968_2009f60` | `0x02009f60` | [ovl_30_c_a_c_c_c_c_c_a.c](src/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_c_a.c) |
| 2 | `OvlFunc_955_2008310` | `0x02008310` | [ovl_30_c_c_c_a_c_a_a.c](src/overlays/rom_7ddb88/ovl_30_c_c_c_a_c_a_a.c) |
| 3 | `OvlFunc_943_200ba0c` | `0x0200ba0c` | [ovl_30_c_c_a.c](src/overlays/rom_7c7b9c/ovl_30_c_c_a.c) |
| 4 | `OvlFunc_884_2009084` | `0x02009084` | [ovl_30_c_a_c_c_c_a_b.c](src/overlays/rom_784360/ovl_30_c_a_c_c_c_a_b.c) |
| 5 | `OvlFunc_882_200a180` | `0x0200a180` | [ovl_30_…_c_c_c_a.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_a.c) |
| 6 | `OvlFunc_882_200a8a4` | `0x0200a8a4` | [ovl_30_…_c_c_c_a.c](src/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_a.c) |

Four of the six landed **whole** — no split, no linker edit. Entry 5+6 is a
2,984-byte two-function TU, the largest single landing in some time, and it was
selected on `hi=0 hiv=0` at 680 instructions.

## Still held back, deliberately

| function | state | why |
|---|---|---|
| `OvlFunc_968_200c048` | matched, verified | `200c2bc` sits in the **middle** of their `.s`; the file lands whole or not at all |
| `OvlFunc_968_200c520` | matched, verified | same |
| `OvlFunc_933_2009874` | matched, verified | fourth of four, and the two matched functions are **non-adjacent** — a five-way cut where the tool does three |

## `hiv` READS BACKWARDS, AND THE ZERO END IS THE WORST CASE

Two of this batch were selected *because* `hi=0 hiv=0` — the ROM touches no high
register at all. Both still needed eviction work, and `200ba0c` needed **18
pins**: unaided gcc commons five constants into r8–r11, spends four extra
callee-saved registers, and comes out **32 bytes long**.

> `hiv` counts the **ROM's** high registers. Zero therefore means gcc has the
> **most** to shed, not the least.

That is the fourth and fifth function this week where the metric pointed the
wrong way, and the first two at zero. The heuristic is not merely weak at the
low end — it is **anti-correlated** there, and should be read that way when
picking targets.

## DROPPING PINS WAS THE BIGGEST LEVER

On the 1,828-byte function the path was not monotone in pin count:

| step | differing | size | relocations |
|---|---|---|---|
| plain, no pins | 663 | 1872 vs 1828 | differ |
| 58 CSE-nominated pins | 536 | 1832 | differ |
| + flag id pinned at both sites | 507 | 1836 | differ |
| + one pointer local per block shape | 635 | **exact** | differ |
| **− the 3 script-symbol pins** | **34** | exact | **silent** |
| + one r5 pointer pin | 22 | exact | silent |
| + 8 ordering pins | 2 | exact | silent |
| + `struct Actor *` cast on four bit sites | **0** | exact | silent |

**A script symbol is a link-time constant, so pinning it evicts nothing** and
only constrains order. The relocation line marks the transition exactly: it
stays differing through every CSE-class error and goes silent the moment the
last one is fixed. That recorded partition held again across 82 further
single-pin drops — 2–3 with relocations silent, 14+ with them differing, and
**nothing in between**.

## A PIN'S CORRECT WIDTH CAN BE ZERO, AND A CALL CLASS SHARES IT

The recorded rule is *"a pin can be worse than no pin, and width decides."* The
addition: **zero is an available width**, and the sites needing it are a
**callee class**, not scattered.

Three `OvlFunc_884_200a2e0(slot, K << n, delay)` sites emit
`mov r1 / mov r2 / lsl r1 / mov r0` — **r0 seeded last**, which an r0 pin cannot
express. Each costs 2, and the three together were the entire 6-differing
residue of an otherwise-correct all-pinned form. Diagnosing one named the other
two without sweeping.

| step | differing |
|---|---|
| all 42 pinned, plain `int c1` | 26 |
| all 42 pinned, `c1` pinned to r6 | 6 |
| all 42 **except** the three `200a2e0` sites | **0** |

## A TIE THAT DOES NOT INDICT

The ROM holds one constant in r6 and another in r8; gcc chose the reverse.
Pinning **either**, or **both**, all measure 0; permuting declaration order
measures 26, i.e. nothing.

This matters against a recorded reading that treats an *"any two of the three
work"* tie as the profile of a lever that is really a **symptom**. Here there is
no residue under any spelling, so that profile does not always indict.

## THE `&=` HALF OF THE `orr` PAIR NEEDS NOTHING — AND NOW THERE IS A REASON

The batch-240 banner records *aggregate member reference versus dereferenced
pointer* for `->f5a |= 1`; this is a third confirmation. What was **not**
recorded is why the sibling `&= 0xfe` is fine unaided.

A four-function probe plus the `-da` dumps: the Thumb `andsi3` expander
`force_reg`s operand 2 into a fresh `reg:SI`, while `iorsi3` leaves a
`subreg:SI` of a `reg:QI` — and only the plain-REG form gets its destination tie
fixed up by `.15.regmove`.

> **Cheap predictor:** if the ROM's sibling `and` came out right on its own and
> the `orr` did not, reach for the aggregate-member spelling, not a mask local.

## CHECK THE PUSH MASK BEFORE THE BODY

Entries 1 and 2 were both **exact on the first draft**, with genuinely empty
worse/inert tables — no sweep ever ran. Entry 1 re-used **seven levers verbatim**
from a file landed hours earlier; entry 2 is its neighbour's body plus three
appended calls, **cross-overlay**, zero levers.

The pre-check that predicted both is already recorded at `docs/elevation.md:7493`
— what separates twins whose levers transfer from twins whose levers *invert* is
the **register budget, not the shape**. Applied here:

| | push mask |
|---|---|
| entry 1 | byte-for-byte its twin's; callee-saved set identical (frames differ only where one spills what the other keeps in `fp`) |
| entry 2 | its neighbour's, down to `sub sp, #8` |

Every lever transferred in both. The same rule explains the *failures* — the
parked `GetMercuryDjinni` pair, genuine twins whose levers inverted. So:
**count what the ROM pushes before reading the body.** It costs nothing.

## Other measurements worth keeping

- **One HImode literal was worth 216.** Written bare,
  `*(unsigned short *)(a+6) = 0xc0 << 8` emits `ldrh r3, .L3`, and that halfword
  pool word forces a **mid-function pool with a branch over it**. Naming it took
  the all-pinned form from 216 to exact.
- **Tempting moves that measured worse:** a struct on *every* actor field 642;
  `unsigned char bit = 1` 674; naming the byte-write pointer 264; the exact ROM
  token-stream fill everywhere 260.
- **One local per value, again:** merging two constants into one local is 168
  and twelve bytes short; recycling one local for two halfword stores is 15.

## Gate

`make clean && make -j8 && make compare` green, `goldensun.gba: OK`. Every
address checked against the linked ELF. Pairing 4,611 sources; `--orphans` 0;
`--unlinked` unchanged at 10, so no landing here added debt.
