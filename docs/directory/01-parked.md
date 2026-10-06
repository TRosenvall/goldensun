# Parked functions

**721 park files**, 568 carrying a parseable figure, 145 bodyless (prose/triage/class parks), 122 with no live verify recipe.

A figure is the park's own claim. `tools/parkcheck.py` is what verifies it.

## Distribution

| differing encodings | parks |
|---|---|
| 0 | 1 |
| 1–2 | 13 |
| 3–5 | 13 |
| 6–10 | 18 |
| 11–20 | 48 |
| 21–50 | 125 |
| 51–100 | 78 |
| 101–300 | 163 |
| 301+ | 109 |

Median **90**.

## By figure

> **TWO THIRDS OF THESE FIGURES MEASURE PHASE, NOT DISTANCE.** A park's figure
> is a POSITIONAL count over the encoding stream, so when the two sides' 16-bit
> instruction counts differ the streams are compared out of phase and the figure
> counts the offset. A full `parkcheck` sweep in batch 331 found **388 of 571**
> verified figures in that state, **133 of them one instruction apart in
> length** — including one claiming 918 at a gap of one.
>
> **Do not rank by this column alone.** See
> [reports/park-figure-audit.md](../../reports/park-figure-audit.md) for the
> flagged list and a shortlist of the cheapest to re-price; `parkcheck` prints
> `*** INFLATED` per park, and `tools/aligncmp.py` gives a real number. Batch
> 331 worked four parks that turned out to be in this shape and **three landed
> outright.**

**92 of these parks carry register pins.** A pinned body's figure is not
comparable with a pin-free one: owner standard 3 prefers a pin-free body, so a
park at 2 with ten pins is further from a pass-2 landing than a park at 20 with
none. **Rank by (figure, pins).** Pin counts are `shimcount`'s.

| fig | pins | of | function | module | park |
|---|---|---|---|---|---|
| 0 | 1 | 81 | `Func_80b9554` | rom_b5000/rom_b8228.s | [src/non_matching/rom_b5000/80b9554.c](../../src/non_matching/rom_b5000/80b9554.c) |
| 1 |  | 191 | `Field_Halt` | rom_8a000/rom_9a44c.s | [src/non_matching/rom_8a000/809abb4.c](../../src/non_matching/rom_8a000/809abb4.c) |
| 2 |  | 26 | `Field_Move` | rom_8a000/rom_97b54.s | [src/non_matching/rom_8a000/809802c.c](../../src/non_matching/rom_8a000/809802c.c) |
| 2 |  | 44 | `Func_80175c0` | rom_15000/rom_15e8c.s | [src/non_matching/rom_15000/rom_175c0.c](../../src/non_matching/rom_15000/rom_175c0.c) |
| 2 |  | 319 | `Func_80a96d8` | rom_a1000/rom_a8604.s | [src/non_matching/rom_a1000/80a96d8.c](../../src/non_matching/rom_a1000/80a96d8.c) |
| 2 |  | 101 | `Func_80ab21c` | rom_a1000/rom_aa538.s | [src/non_matching/rom_a1000/80ab21c.c](../../src/non_matching/rom_a1000/80ab21c.c) |
| 2 |  | 59 | `Func_80b6a60` | rom_b5000/rom_b5a0c.s | [src/non_matching/rom_b5000/80b6a60.c](../../src/non_matching/rom_b5000/80b6a60.c) |
| 2 |  | 39 | `Func_80f0254` | rom_f0000/rom_f0254.s | [src/non_matching/rom_f0000/80f0254.c](../../src/non_matching/rom_f0000/80f0254.c) |
| 2 |  | 29 | `Func_80f3858` | rom_f2000/rom_f2028.s | [src/non_matching/rom_f2000/80f3858.c](../../src/non_matching/rom_f2000/80f3858.c) |
| 2 |  | 36 | `HeightTile_A` | rom_9000/rom_11ce0.s | [src/non_matching/rom_9000/8011e88.c](../../src/non_matching/rom_9000/8011e88.c) |
| 2 |  | 74 | `OvlFunc_932_20082cc` | overlays/ovl_30.s | [src/non_matching/ovl_7b9cb4/20082cc.c](../../src/non_matching/ovl_7b9cb4/20082cc.c) |
| 2 |  | 57 | `OvlFunc_947_200a1ac` | overlays/ovl_1528.s | [src/non_matching/ovl_7d0e88/200a1ac.c](../../src/non_matching/ovl_7d0e88/200a1ac.c) |
| 2 |  | 271 | `OvlFunc_968_200c2bc` | overlays/ovl_30.s | [src/non_matching/ovl_7f2f14/200c2bc.c](../../src/non_matching/ovl_7f2f14/200c2bc.c) |
| 2 | 10 | 402 | `BaseAnim_Tackle` | rom_c9000/rom_dfa18.s | [src/non_matching/rom_c9000/dfa18_Tackle.c](../../src/non_matching/rom_c9000/dfa18_Tackle.c) |
| 3 |  | 69 | `Func_801c34c` | rom_15000/rom_1aeec.s | [src/non_matching/rom_15000/801c34c.c](../../src/non_matching/rom_15000/801c34c.c) |
| 3 |  | 85 | `OvlFunc_896_200c260` | overlays/ovl_314.s | [src/non_matching/ovl_78ef88/200c260.c](../../src/non_matching/ovl_78ef88/200c260.c) |
| 3 |  | 38 | `OvlFunc_927_2009818` | overlays/ovl_30.s | [src/non_matching/ovl_7b4558/2009818.c](../../src/non_matching/ovl_7b4558/2009818.c) |
| 4 |  | 19 | `Func_80ab1f4` | rom_a1000/rom_aa538.s | [src/non_matching/rom_a1000/rom_ab1f4.c](../../src/non_matching/rom_a1000/rom_ab1f4.c) |
| 4 |  | 119 | `Func_80b6d30` | rom_b5000/rom_b5a0c.s | [src/non_matching/rom_b5000/80b6d30.c](../../src/non_matching/rom_b5000/80b6d30.c) |
| 4 |  | 221 | `OvlFunc_931_2008904` | overlays/ovl_30.s | [src/non_matching/ovl_7b8cb0/2008904.c](../../src/non_matching/ovl_7b8cb0/2008904.c) |
| 4 |  | 104 | `StartRain` | rom_8a000/rom_944ec.s | [src/non_matching/rom_8a000/StartRain.c](../../src/non_matching/rom_8a000/StartRain.c) |
| 4 | 45 | 453 | `OvlFunc_887_2008578` | overlays/ovl_30.s | [src/non_matching/ovl_787e04/2008578.c](../../src/non_matching/ovl_787e04/2008578.c) |
| 5 |  | 39 | `Anim_Attack` | rom_c9000/rom_e3958.s | [src/non_matching/rom_c9000/rom_e3a3c.c](../../src/non_matching/rom_c9000/rom_e3a3c.c) |
| 5 |  | 133 | `DisplayMenuArrowCursor` | rom_15000/rom_1aeec.s | [src/non_matching/rom_15000/DisplayMenuArrowCursor.c](../../src/non_matching/rom_15000/DisplayMenuArrowCursor.c) |
| 5 |  | 40 | `Func_8078870` | rom_77000/rom_78414.s | [src/non_matching/rom_77000/8078870.c](../../src/non_matching/rom_77000/8078870.c) |
| 5 |  | 179 | `OvlFunc_923_2009a3c` | overlays/ovl_1a3c.s | [src/non_matching/ovl_7aa430/2009a3c.c](../../src/non_matching/ovl_7aa430/2009a3c.c) |
| 5 |  | 179 | `OvlFunc_924_200cfcc` | overlays/ovl_35b8.s | [src/non_matching/ovl_7ac2d8/200cfcc.c](../../src/non_matching/ovl_7ac2d8/200cfcc.c) |
| 6 |  | 167 | `Func_80a8f40` | rom_a1000/rom_a8604.s | [src/non_matching/rom_a1000/80a8f40.c](../../src/non_matching/rom_a1000/80a8f40.c) |
| 6 |  | 37 | `Func_80c0130` | rom_b5000/rom_bffb8.s | [src/non_matching/rom_b5000/80c0130.c](../../src/non_matching/rom_b5000/80c0130.c) |
| 6 |  | 110 | `OvlFunc_927_200a1b0` | overlays/ovl_30.s | [src/non_matching/ovl_7b4558/200a1b0.c](../../src/non_matching/ovl_7b4558/200a1b0.c) |
| 6 |  | 175 | `OvlFunc_970_2008da4` | overlays/ovl_30.s | [src/non_matching/ovl_7fa4ec/2008da4.c](../../src/non_matching/ovl_7fa4ec/2008da4.c) |
| 7 |  | 122 | `FieldMove_NoTarget` | rom_8a000/rom_944ec.s | [src/non_matching/rom_8a000/8096810.c](../../src/non_matching/rom_8a000/8096810.c) |
| 7 |  | 88 | `Func_807a0f4` | rom_77000/rom_79460.s | [src/non_matching/rom_77000/807a0f4.c](../../src/non_matching/rom_77000/807a0f4.c) |
| 8 |  | 44 | `Func_800bfa4` | rom_9000/rom_be70.s | [src/non_matching/rom_9000/rom_bfa4.c](../../src/non_matching/rom_9000/rom_bfa4.c) |
| 8 |  | 57 | `Func_80a1a40` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a1a40.c](../../src/non_matching/rom_a1000/80a1a40.c) |
| 8 |  | 32 | `Func_80bd424` | rom_b5000/rom_bbb0c.s | [src/non_matching/rom_b5000/80bd424.c](../../src/non_matching/rom_b5000/80bd424.c) |
| 8 |  | 41 | `OvlFunc_922_2008ed8` | overlays/ovl_30.s | [src/non_matching/ovl_7a8c8c/2008ed8.c](../../src/non_matching/ovl_7a8c8c/2008ed8.c) |
| 8 | 1 | 133 | `OvlFunc_887_200968c` | overlays/ovl_30.s | [src/non_matching/ovl_787e04/200968c.c](../../src/non_matching/ovl_787e04/200968c.c) |
| 8 | 1 | 148 | `OvlFunc_897_200aeb0` | overlays/ovl_30.s | [src/non_matching/ovl_791794/200aeb0.c](../../src/non_matching/ovl_791794/200aeb0.c) |
| 8 | 64 | 537 | `OvlFunc_896_2009d04` | overlays/ovl_314.s | [src/non_matching/ovl_78ef88/2009d04.c](../../src/non_matching/ovl_78ef88/2009d04.c) |
| 9 |  | 24 | `Func_80216b4` | rom_15000/rom_20198.s | [src/non_matching/rom_15000/80216b4.c](../../src/non_matching/rom_15000/80216b4.c) |
| 9 |  | 33 | `Func_80f7f30` | rom_f6000/rom_f6008.s | [src/non_matching/rom_f6000/80f7f30.c](../../src/non_matching/rom_f6000/80f7f30.c) |
| 9 |  | 72 | `OvlFunc_943_200985c` | overlays/ovl_30.s | [src/non_matching/ovl_7c7b9c/200985c.c](../../src/non_matching/ovl_7c7b9c/200985c.c) |
| 10 |  | 36 | `Func_801fd34` | rom_15000/rom_1de5c.s | [src/non_matching/rom_15000/801fd34.c](../../src/non_matching/rom_15000/801fd34.c) |
| 10 |  | 27 | `OvlFunc_932_20086a0` | overlays/ovl_30.s | [src/non_matching/ovl_7b9cb4/20086a0.c](../../src/non_matching/ovl_7b9cb4/20086a0.c) |
| 11 |  | 146 | `Func_8096ddc` | rom_8a000/rom_96cdc.s | [src/non_matching/rom_8a000/8096ddc.c](../../src/non_matching/rom_8a000/8096ddc.c) |
| 11 |  | 60 | `Func_80a8578` | rom_a1000/rom_a7380.s | [src/non_matching/rom_a1000/80a8578.c](../../src/non_matching/rom_a1000/80a8578.c) |
| 11 |  | 12 | `SoundMainBTM` | rom_f9000/rom_f95e0.s | [src/non_matching/rom_f9000/rom_f9a18.c](../../src/non_matching/rom_f9000/rom_f9a18.c) |
| 12 |  | 136 | `OvlFunc_884_200a440` | overlays/ovl_30.s | [src/non_matching/ovl_784360/200a440.c](../../src/non_matching/ovl_784360/200a440.c) |
| 12 |  | 59 | `OvlFunc_888_200a750` | overlays/ovl_30.s | [src/non_matching/overlays/200a750.c](../../src/non_matching/overlays/200a750.c) |
| 12 |  | 109 | `OvlFunc_905_2008a68` | overlays/ovl_30.s | [src/non_matching/ovl_799abc/2008a68.c](../../src/non_matching/ovl_799abc/2008a68.c) |
| 12 | 5 | 371 | `OvlFunc_923_200a030` | overlays/ovl_1a3c.s | [src/non_matching/ovl_7aa430/200a030.c](../../src/non_matching/ovl_7aa430/200a030.c) |
| 12 | 5 | 371 | `OvlFunc_924_200d5c0` | overlays/ovl_35b8.s | [src/non_matching/ovl_7ac2d8/200d5c0.c](../../src/non_matching/ovl_7ac2d8/200d5c0.c) |
| 13 |  | 707 | `Anim_CriticalHit` | rom_c9000/rom_e3958.s | [src/non_matching/rom_c9000/Anim_CriticalHit.c](../../src/non_matching/rom_c9000/Anim_CriticalHit.c) |
| 13 |  | 81 | `Func_801bcd4` | rom_15000/rom_1aeec.s | [src/non_matching/rom_15000/801bcd4.c](../../src/non_matching/rom_15000/801bcd4.c) |
| 13 |  | 32 | `OvlFunc_880_2008384` | overlays/ovl_30.s | [src/non_matching/ovl_7795e8/2008384.c](../../src/non_matching/ovl_7795e8/2008384.c) |
| 13 |  | 91 | `OvlFunc_924_20096c4` | overlays/ovl_f84.s | [src/non_matching/ovl_7ac2d8/20096c4.c](../../src/non_matching/ovl_7ac2d8/20096c4.c) |
| 13 |  | 72 | `OvlFunc_946_200add0` | overlays/ovl_30.s | [src/non_matching/ovl_7ced6c/200add0.c](../../src/non_matching/ovl_7ced6c/200add0.c) |
| 13 |  | 55 | `OvlFunc_959_2008d54` | overlays/ovl_9dc.s | [src/non_matching/overlays/2008d54.c](../../src/non_matching/overlays/2008d54.c) |
| 13 | 3 | 47 | `Func_80979a4` | rom_8a000/rom_97384.s | [src/non_matching/rom_8a000/80979a4.c](../../src/non_matching/rom_8a000/80979a4.c) |
| 13 | 15 | 262 | `OvlFunc_968_2009af0` | overlays/ovl_30.s | [src/non_matching/ovl_7b0400/2009af0.c](../../src/non_matching/ovl_7b0400/2009af0.c) |
| 13 | 15 | 262 | `OvlFunc_968_2009af0` | overlays/ovl_30.s | [src/non_matching/ovl_7f2f14/2009af0.c](../../src/non_matching/ovl_7f2f14/2009af0.c) |
| 14 |  | 142 | `AnimEnd` | rom_c9000/rom_cd508.s | [src/non_matching/rom_c9000/AnimEnd.c](../../src/non_matching/rom_c9000/AnimEnd.c) |
| 14 |  | 102 | `Func_80a6794` | rom_a1000/rom_a5534.s | [src/non_matching/rom_a1000/80a6794.c](../../src/non_matching/rom_a1000/80a6794.c) |
| 14 |  | 59 | `Func_80b0070` | rom_b0000/rom_b0070.s | [src/non_matching/rom_b0000/80b0070.c](../../src/non_matching/rom_b0000/80b0070.c) |
| 14 |  | 87 | `LoadGS1CreditsBG` | rom_f0000/rom_f0254.s | [src/non_matching/rom_f0000/LoadGS1CreditsBG.c](../../src/non_matching/rom_f0000/LoadGS1CreditsBG.c) |
| 14 |  | 53 | `OvlFunc_897_200ac1c` | overlays/ovl_30.s | [src/non_matching/ovl_791794/200ac1c.c](../../src/non_matching/ovl_791794/200ac1c.c) |
| 14 |  | 367 | `OvlFunc_954_2008a3c` | overlays/ovl_30.s | [src/non_matching/ovl_7db0c8/2008a3c.c](../../src/non_matching/ovl_7db0c8/2008a3c.c) |
| 14 | 1 | 195 | `InitMapActors` | rom_8a000/rom_8b674.s | [src/non_matching/rom_8a000/808b674.c](../../src/non_matching/rom_8a000/808b674.c) |
| 14 | 4 | 92 | `Func_8021cb8` | rom_15000/rom_20198.s | [src/non_matching/rom_15000/8021cb8.c](../../src/non_matching/rom_15000/8021cb8.c) |
| 16 |  | 738 | `Anim_Djinni` | rom_c9000/rom_dd2ac.s | [src/non_matching/rom_c9000/Anim_Djinni.c](../../src/non_matching/rom_c9000/Anim_Djinni.c) |
| 16 |  | 63 | `Func_8094154` | rom_8a000/rom_93304.s | [src/non_matching/rom_8a000/8094154.c](../../src/non_matching/rom_8a000/8094154.c) |
| 16 |  | 119 | `OvlFunc_964_20090c4` | overlays/ovl_30.s | [src/non_matching/ovl_7ed0a0/20090c4.c](../../src/non_matching/ovl_7ed0a0/20090c4.c) |
| 16 |  | 140 | `OvlFunc_common1_1354` | overlays/common1.s | [src/non_matching/ovl_common/common1_1354.c](../../src/non_matching/ovl_common/common1_1354.c) |
| 17 |  | 163 | `Debug_WarpMenu_UI` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/8029094.c](../../src/non_matching/rom_15000/8029094.c) |
| 17 |  | 23 | `Func_8078aa0` | rom_77000/rom_78a8c.s | [src/non_matching/rom_77000/8078aa0.c](../../src/non_matching/rom_77000/8078aa0.c) |
| 17 |  | 92 | `OvlFunc_931_20086f0` | overlays/ovl_30.s | [src/non_matching/ovl_7b8cb0/20086f0.c](../../src/non_matching/ovl_7b8cb0/20086f0.c) |
| 17 |  | 38 | `OvlFunc_959_200a06c` | overlays/ovl_9dc.s | [src/non_matching/ovl_7e7574/200a06c.c](../../src/non_matching/ovl_7e7574/200a06c.c) |
| 17 |  | 16 | `RealClearChain` | rom_f9000/rom_f95e0.s | [src/non_matching/rom_f9000/80f9a30.c](../../src/non_matching/rom_f9000/80f9a30.c) |
| 17 | 15 | 160 | `OvlFunc_896_200a27c` | overlays/ovl_314.s | [src/non_matching/ovl_78ef88/200a27c.c](../../src/non_matching/ovl_78ef88/200a27c.c) |
| 18 |  | 20 | `Func_80270ac` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/80270ac.c](../../src/non_matching/rom_15000/80270ac.c) |
| 18 |  | 411 | `Func_80a112c` | rom_a1000/rom_a1050.s | [src/non_matching/rom_a1000/80a112c.c](../../src/non_matching/rom_a1000/80a112c.c) |
| 18 |  | 48 | `OvlFunc_901_2008640` | overlays/ovl_314.s | [src/non_matching/overlays/2008640.c](../../src/non_matching/overlays/2008640.c) |
| 18 |  | 31 | `OvlFunc_949_20086e8` | overlays/ovl_30.s | [src/non_matching/ovl_7d4af4/20086e8.c](../../src/non_matching/ovl_7d4af4/20086e8.c) |
| 18 |  | 44 | `OvlFunc_957_2008f10` | overlays/ovl_30.s | [src/non_matching/ovl_7e3e08/2008f10.c](../../src/non_matching/ovl_7e3e08/2008f10.c) |
| 19 |  | 32 | `OvlFunc_965_200a660` | overlays/ovl_30.s | [src/non_matching/ovl_7ef4f4/200a660.c](../../src/non_matching/ovl_7ef4f4/200a660.c) |
| 19 | 9 | 184 | `OvlFunc_959_200cda0` | overlays/ovl_9dc.s | [src/non_matching/ovl_7e7574/200cda0.c](../../src/non_matching/ovl_7e7574/200cda0.c) |
| 20 |  | 73 | `Func_8028ef0` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/8028ef0.c](../../src/non_matching/rom_15000/8028ef0.c) |
| 20 |  | 43 | `Func_8091eb0` | rom_8a000/rom_91584.s | [src/non_matching/rom_8a000/8091eb0.c](../../src/non_matching/rom_8a000/8091eb0.c) |
| 20 |  | 158 | `Func_80c1afc` | rom_b5000/rom_c1a34.s | [src/non_matching/rom_b5000/80c1afc.c](../../src/non_matching/rom_b5000/80c1afc.c) |
| 20 |  | 76 | `Func_80f6148` | rom_f6000/rom_f6008.s | [src/non_matching/rom_f6000/80f6148.c](../../src/non_matching/rom_f6000/80f6148.c) |
| 20 |  | 75 | `Player_ExitStairs` | rom_8a000/rom_93304.s | [src/non_matching/rom_8a000/8094380.c](../../src/non_matching/rom_8a000/8094380.c) |
| 20 | 1 | 381 | `Anim_Venus` | rom_c9000/rom_e0564.s | [src/non_matching/rom_c9000/Anim_Venus.c](../../src/non_matching/rom_c9000/Anim_Venus.c) |
| 21 |  | 33 | `AddPartyMember` | rom_77000/rom_79460.s | [src/non_matching/rom_77000/807961c.c](../../src/non_matching/rom_77000/807961c.c) |
| 21 |  | 291 | `OvlFunc_924_2009db4` | overlays/ovl_1db4.s | [src/non_matching/ovl_7ac2d8/2009db4.c](../../src/non_matching/ovl_7ac2d8/2009db4.c) |
| 22 |  | 151 | `Func_801908c` | rom_15000/rom_18cac.s | [src/non_matching/rom_15000/801908c.c](../../src/non_matching/rom_15000/801908c.c) |
| 22 |  | 42 | `Func_807977c` | rom_77000/rom_79460.s | [src/non_matching/rom_77000/807977c.c](../../src/non_matching/rom_77000/807977c.c) |
| 23 |  | 25 | `Func_80a7440` | rom_a1000/rom_a7380.s | [src/non_matching/rom_a1000/80a7440.c](../../src/non_matching/rom_a1000/80a7440.c) |
| 23 |  | 32 | `Func_80b2720` | rom_b0000/rom_b0070.s | [src/non_matching/rom_b0000/80b2720.c](../../src/non_matching/rom_b0000/80b2720.c) |
| 23 |  | 30 | `Func_80c1014` | rom_b5000/rom_bffb8.s | [src/non_matching/rom_b5000/80c1014.c](../../src/non_matching/rom_b5000/80c1014.c) |
| 23 |  | 29 | `OvlFunc_941_2008094` | overlays/ovl_30.s | [src/non_matching/ovl_7c5efc/2008094.c](../../src/non_matching/ovl_7c5efc/2008094.c) |
| 23 | 1 | 79 | `OvlFunc_888_200b1b8` | overlays/ovl_30.s | [src/non_matching/ovl_7892c8/200b1b8.c](../../src/non_matching/ovl_7892c8/200b1b8.c) |
| 24 |  | 44 | `Func_8006088` | rom_c0/rom_5cf8.s | [src/non_matching/rom_c0/8006088.c](../../src/non_matching/rom_c0/8006088.c) |
| 24 |  | 25 | `Func_8006384` | rom_c0/rom_5cf8.s | [src/non_matching/rom_c0/8006384.c](../../src/non_matching/rom_c0/8006384.c) |
| 24 |  | 202 | `NintendoLogo` | rom_f2000/rom_f2028.s | [src/non_matching/rom_f2000/NintendoLogo.c](../../src/non_matching/rom_f2000/NintendoLogo.c) |
| 24 |  | 123 | `OvlFunc_901_2008f30` | overlays/ovl_314.s | [src/non_matching/ovl_797990/2008f30.c](../../src/non_matching/ovl_797990/2008f30.c) |
| 24 |  | 32 | `OvlFunc_973_200871c` | overlays/ovl_30.s | [src/non_matching/ovl_7fc720/200871c.c](../../src/non_matching/ovl_7fc720/200871c.c) |
| 24 | 6 | 379 | `OvlFunc_935_2008ca0` | overlays/ovl_b8c.s | [src/non_matching/ovl_7bf5a8/2008ca0.c](../../src/non_matching/ovl_7bf5a8/2008ca0.c) |
| 25 |  | 64 | `Func_808f498` | rom_8a000/rom_8d9a4.s | [src/non_matching/rom_8a000/808f498.c](../../src/non_matching/rom_8a000/808f498.c) |
| 25 |  | 27 | `Func_8092b54` | rom_8a000/rom_92950.s | [src/non_matching/rom_8a000/rom_92b54.c](../../src/non_matching/rom_8a000/rom_92b54.c) |
| 25 |  | 30 | `Func_80a3d9c` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a3d9c.c](../../src/non_matching/rom_a1000/80a3d9c.c) |
| 25 |  | 73 | `OvlFunc_887_2008e34` | overlays/ovl_30.s | [src/non_matching/overlays/2008e34.c](../../src/non_matching/overlays/2008e34.c) |
| 25 |  | 79 | `OvlFunc_891_2008098` | overlays/ovl_30.s | [src/non_matching/ovl_78c76c/2008098.c](../../src/non_matching/ovl_78c76c/2008098.c) |
| 25 |  | 37 | `OvlFunc_common1_588` | overlays/common1.s | [src/non_matching/ovl_common/common1_588.c](../../src/non_matching/ovl_common/common1_588.c) |
| 26 |  | 472 | `Anim_Whirlwind` | rom_c9000/rom_d2d98.s | [src/non_matching/rom_c9000/Anim_Whirlwind.c](../../src/non_matching/rom_c9000/Anim_Whirlwind.c) |
| 26 |  | 186 | `Func_80a3ef0` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a3ef0.c](../../src/non_matching/rom_a1000/80a3ef0.c) |
| 26 |  | 43 | `Func_80ae9f0` | rom_a1000/rom_ae88c.s | [src/non_matching/rom_a1000/80ae9f0.c](../../src/non_matching/rom_a1000/80ae9f0.c) |
| 26 |  | 34 | `Func_80b0a20` | rom_b0000/rom_b0070.s | [src/non_matching/rom_b0000/80b0a20.c](../../src/non_matching/rom_b0000/80b0a20.c) |
| 26 |  | 48 | `Func_80b2ed8` | rom_b0000/rom_b0070.s | [src/non_matching/rom_b0000/80b2ed8.c](../../src/non_matching/rom_b0000/80b2ed8.c) |
| 26 |  | 34 | `Func_80b90ac` | rom_b5000/rom_b8228.s | [src/non_matching/rom_b5000/80b90ac.c](../../src/non_matching/rom_b5000/80b90ac.c) |
| 26 |  | 26 | `GetWeaponType` | rom_b5000/rom_b6e7c.s | [src/non_matching/rom_b5000/80b6e7c.c](../../src/non_matching/rom_b5000/80b6e7c.c) |
| 26 |  | 62 | `OvlFunc_926_2008388` | overlays/ovl_314.s | [src/non_matching/ovl_7b2078/2008388.c](../../src/non_matching/ovl_7b2078/2008388.c) |
| 26 |  | 71 | `OvlFunc_971_20087b0` | overlays/ovl_30.s | [src/non_matching/ovl_7fb4a8/20087b0.c](../../src/non_matching/ovl_7fb4a8/20087b0.c) |
| 27 |  | 33 | `Func_80063bc` | rom_c0/rom_5cf8.s | [src/non_matching/rom_c0/80063bc.c](../../src/non_matching/rom_c0/80063bc.c) |
| 27 |  | 37 | `Func_8011164` | rom_9000/rom_108e4.s | [src/non_matching/rom_9000/8011164.c](../../src/non_matching/rom_9000/8011164.c) |
| 27 |  | 58 | `Func_80a3354` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a3354.c](../../src/non_matching/rom_a1000/80a3354.c) |
| 28 |  | 44 | `Func_8079664` | rom_77000/rom_79460.s | [src/non_matching/rom_77000/8079664.c](../../src/non_matching/rom_77000/8079664.c) |
| 28 |  | 238 | `Func_80b1bd0` | rom_b0000/rom_b0070.s | [src/non_matching/rom_b0000/80b1bd0.c](../../src/non_matching/rom_b0000/80b1bd0.c) |
| 28 |  | 135 | `OvlFunc_932_200a6c0` | overlays/ovl_30.s | [src/non_matching/ovl_7b9cb4/200a6c0.c](../../src/non_matching/ovl_7b9cb4/200a6c0.c) |
| 29 |  | 30 | `Func_8005868` | rom_c0/rom_56cc.s | [src/non_matching/rom_c0/rom_5868.c](../../src/non_matching/rom_c0/rom_5868.c) |
| 29 |  | 38 | `Func_801e3c8` | rom_15000/rom_1de5c.s | [src/non_matching/rom_15000/801e3c8.c](../../src/non_matching/rom_15000/801e3c8.c) |
| 29 |  | 39 | `Func_8079008` | rom_77000/rom_79008.s | [src/non_matching/rom_77000/rom_79008.c](../../src/non_matching/rom_77000/rom_79008.c) |
| 29 |  | 124 | `Func_8093af8` | rom_8a000/rom_93304.s | [src/non_matching/rom_8a000/8093af8.c](../../src/non_matching/rom_8a000/8093af8.c) |
| 29 |  | 30 | `Func_80e3908` | rom_c9000/rom_e28f4.s | [src/non_matching/rom_c9000/80e3908.c](../../src/non_matching/rom_c9000/80e3908.c) |
| 29 |  | 48 | `OvlFunc_959_200a69c` | overlays/ovl_9dc.s | [src/non_matching/ovl_7e7574/200a69c.c](../../src/non_matching/ovl_7e7574/200a69c.c) |
| 29 | 11 | 270 | `OvlFunc_952_200c0b4` | overlays/ovl_30.s | [src/non_matching/ovl_7d768c/200c0b4.c](../../src/non_matching/ovl_7d768c/200c0b4.c) |
| 30 |  | 140 | `DisplayMenuArrowCursor2` | rom_15000/rom_1aeec.s | [src/non_matching/rom_15000/DisplayMenuArrowCursor2.c](../../src/non_matching/rom_15000/DisplayMenuArrowCursor2.c) |
| 30 |  | 45 | `Func_8011fd8` | rom_9000/rom_11ce0.s | [src/non_matching/rom_9000/8011fd8.c](../../src/non_matching/rom_9000/8011fd8.c) |
| 30 |  | 74 | `Func_80165d8` | rom_15000/rom_15e8c.s | [src/non_matching/rom_15000/80165d8.c](../../src/non_matching/rom_15000/80165d8.c) |
| 30 |  | 32 | `Func_80bd3e4` | rom_b5000/rom_bbb0c.s | [src/non_matching/rom_b5000/80bd3e4.c](../../src/non_matching/rom_b5000/80bd3e4.c) |
| 30 |  | 61 | `Func_80c0f98` | rom_b5000/rom_bffb8.s | [src/non_matching/rom_b5000/80c0f98.c](../../src/non_matching/rom_b5000/80c0f98.c) |
| 30 |  | 72 | `Func_80d67dc` | rom_c9000/rom_d6504.s | [src/non_matching/rom_c9000/80d67dc.c](../../src/non_matching/rom_c9000/80d67dc.c) |
| 30 |  | 32 | `Func_80f7df0` | rom_f6000/rom_f6008.s | [src/non_matching/rom_f6000/80f7df0.c](../../src/non_matching/rom_f6000/80f7df0.c) |
| 30 |  | 31 | `OvlFunc_881_200a7dc` | overlays/ovl_30.s | [src/non_matching/ovl_77a7c8/200a7dc.c](../../src/non_matching/ovl_77a7c8/200a7dc.c) |
| 30 |  | 43 | `OvlFunc_909_200809c` | overlays/ovl_30.s | [src/non_matching/ovl_79c738/200809c.c](../../src/non_matching/ovl_79c738/200809c.c) |
| 30 |  | 35 | `YesNoMenu2` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/rom_28e54.c](../../src/non_matching/rom_15000/rom_28e54.c) |
| 31 |  | 34 | `Func_8006408` | rom_c0/rom_5cf8.s | [src/non_matching/rom_c0/8006408.c](../../src/non_matching/rom_c0/8006408.c) |
| 31 |  | 49 | `Func_80a2144` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a2144.c](../../src/non_matching/rom_a1000/80a2144.c) |
| 31 |  | 197 | `InitEnemyUnit` | rom_77000/rom_79460.s | [src/non_matching/rom_77000/8079460.c](../../src/non_matching/rom_77000/8079460.c) |
| 31 |  | 46 | `OvlFunc_888_200a6f0` | overlays/ovl_30.s | [src/non_matching/ovl_7892c8/200a6f0.c](../../src/non_matching/ovl_7892c8/200a6f0.c) |
| 32 |  | 127 | `Func_801179c` | rom_9000/rom_11568.s | [src/non_matching/rom_9000/801179c.c](../../src/non_matching/rom_9000/801179c.c) |
| 33 |  | 57 | `Func_80bac6c` | rom_b5000/rom_b9b30.s | [src/non_matching/rom_b5000/80bac6c.c](../../src/non_matching/rom_b5000/80bac6c.c) |
| 33 |  | 37 | `Func_80c00d8` | rom_b5000/rom_bffb8.s | [src/non_matching/rom_b5000/rom_c00d8.c](../../src/non_matching/rom_b5000/rom_c00d8.c) |
| 33 |  | 201 | `OvlFunc_890_20089f4` | overlays/ovl_30.s | [src/non_matching/ovl_78b2ac/20089f4.c](../../src/non_matching/ovl_78b2ac/20089f4.c) |
| 33 |  | 153 | `WaitTextPrompt` | rom_b5000/rom_bb588.s | [src/non_matching/rom_b5000/WaitTextPrompt.c](../../src/non_matching/rom_b5000/WaitTextPrompt.c) |
| 34 |  | 41 | `OvlFunc_945_2008058` | overlays/ovl_30.s | [src/non_matching/ovl_7cb2c0/2008058.c](../../src/non_matching/ovl_7cb2c0/2008058.c) |
| 35 |  | 77 | `Func_800eaf8` | rom_9000/rom_ea54.s | [src/non_matching/rom_9000/800eaf8.c](../../src/non_matching/rom_9000/800eaf8.c) |
| 35 |  | 53 | `OvlFunc_881_2009c08` | overlays/ovl_30.s | [src/non_matching/ovl_77a7c8/2009c08.c](../../src/non_matching/ovl_77a7c8/2009c08.c) |
| 36 |  | 235 | `ActorCmd_Camera` | rom_9000/rom_d924.s | [src/non_matching/rom_9000/800daf0.c](../../src/non_matching/rom_9000/800daf0.c) |
| 36 |  | 91 | `Func_80292c4` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/80292c4.c](../../src/non_matching/rom_15000/80292c4.c) |
| 36 |  | 47 | `Func_80a40ac` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a40ac.c](../../src/non_matching/rom_a1000/80a40ac.c) |
| 36 |  | 53 | `Func_80c0228` | rom_b5000/rom_bffb8.s | [src/non_matching/rom_b5000/80c0228.c](../../src/non_matching/rom_b5000/80c0228.c) |
| 36 |  | 85 | `OvlFunc_952_2008264` | overlays/ovl_30.s | [src/non_matching/ovl_7d768c/2008264.c](../../src/non_matching/ovl_7d768c/2008264.c) |
| 36 |  | 41 | `OvlFunc_957_2008bc8` | overlays/ovl_30.s | [src/non_matching/ovl_7e3e08/2008bc8.c](../../src/non_matching/ovl_7e3e08/2008bc8.c) |
| 37 |  | 426 | `Anim_Mars` | rom_c9000/rom_e0564.s | [src/non_matching/rom_c9000/Anim_Mars.c](../../src/non_matching/rom_c9000/Anim_Mars.c) |
| 37 |  | 105 | `Func_80b9470` | rom_b5000/rom_b8228.s | [src/non_matching/rom_b5000/80b9470.c](../../src/non_matching/rom_b5000/80b9470.c) |
| 37 |  | 89 | `OvlFunc_947_20099f0` | overlays/ovl_1528.s | [src/non_matching/ovl_7d0e88/20099f0.c](../../src/non_matching/ovl_7d0e88/20099f0.c) |
| 38 |  | 185 | `Debug_FaceTest` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/802977c.c](../../src/non_matching/rom_15000/802977c.c) |
| 38 |  | 288 | `Func_807905c` | rom_77000/rom_79008.s | [src/non_matching/rom_77000/807905c.c](../../src/non_matching/rom_77000/807905c.c) |
| 38 |  | 43 | `Func_808bc44` | rom_8a000/rom_8ba38.s | [src/non_matching/rom_8a000/808bc44.c](../../src/non_matching/rom_8a000/808bc44.c) |
| 38 |  | 74 | `Func_80930bc` | rom_8a000/rom_92950.s | [src/non_matching/rom_8a000/80930bc.c](../../src/non_matching/rom_8a000/80930bc.c) |
| 38 |  | 39 | `Func_80ae99c` | rom_a1000/rom_ae88c.s | [src/non_matching/rom_a1000/80ae99c.c](../../src/non_matching/rom_a1000/80ae99c.c) |
| 38 |  | 56 | `Func_80d66cc` | rom_c9000/rom_d6504.s | [src/non_matching/rom_c9000/80d66cc.c](../../src/non_matching/rom_c9000/80d66cc.c) |
| 38 |  | 48 | `Func_80e727c` | rom_c9000/rom_e6638.s | [src/non_matching/rom_c9000/80e727c.c](../../src/non_matching/rom_c9000/80e727c.c) |
| 38 |  | 45 | `Func_80f0614` | rom_f0000/rom_f0254.s | [src/non_matching/rom_f0000/80f0614.c](../../src/non_matching/rom_f0000/80f0614.c) |
| 38 | 4 | 92 | `OvlFunc_923_2009df8` | overlays/ovl_1a3c.s | [src/non_matching/ovl_7aa430/2009df8.c](../../src/non_matching/ovl_7aa430/2009df8.c) |
| 39 |  | 367 | `Anim_Jupiter` | rom_c9000/rom_dfa18.s | [src/non_matching/rom_c9000/Anim_Jupiter.c](../../src/non_matching/rom_c9000/Anim_Jupiter.c) |
| 39 |  | 43 | `CanRemoveItem` | rom_77000/rom_78414.s | [src/non_matching/rom_77000/8078980.c](../../src/non_matching/rom_77000/8078980.c) |
| 39 |  | 64 | `CreateSpriteLayer` | rom_9000/rom_b798.s | [src/non_matching/rom_9000/800bbc0.c](../../src/non_matching/rom_9000/800bbc0.c) |
| 39 |  | 62 | `Func_80788c4` | rom_77000/rom_78414.s | [src/non_matching/rom_77000/80788c4.c](../../src/non_matching/rom_77000/80788c4.c) |
| 39 |  | 44 | `Func_8093054` | rom_8a000/rom_92950.s | [src/non_matching/rom_8a000/8093054.c](../../src/non_matching/rom_8a000/8093054.c) |
| 39 |  | 40 | `Func_80e38b8` | rom_c9000/rom_e28f4.s | [src/non_matching/rom_c9000/80e38b8.c](../../src/non_matching/rom_c9000/80e38b8.c) |
| 40 |  | 367 | `ActorCmd_Unk9` | rom_9000/rom_d924.s | [src/non_matching/rom_9000/800df04.c](../../src/non_matching/rom_9000/800df04.c) |
| 40 |  | 53 | `Func_8021d88` | rom_15000/rom_20198.s | [src/non_matching/rom_15000/8021d88.c](../../src/non_matching/rom_15000/8021d88.c) |
| 40 | 2 | 153 | `OvlFunc_923_2009cb4` | overlays/ovl_1a3c.s | [src/non_matching/ovl_7aa430/2009cb4.c](../../src/non_matching/ovl_7aa430/2009cb4.c) |
| 40 | 2 | 153 | `OvlFunc_924_200d244` | overlays/ovl_35b8.s | [src/non_matching/ovl_7ac2d8/200d244.c](../../src/non_matching/ovl_7ac2d8/200d244.c) |
| 41 |  | 55 | `OvlFunc_882_200bc48` | overlays/ovl_30.s | [src/non_matching/ovl_77dd1c/200bc48.c](../../src/non_matching/ovl_77dd1c/200bc48.c) |
| 41 |  | 89 | `OvlFunc_947_2009938` | overlays/ovl_1528.s | [src/non_matching/ovl_7d0e88/2009938.c](../../src/non_matching/ovl_7d0e88/2009938.c) |
| 42 | 1 | 302 | `OvlFunc_954_2008540` | overlays/ovl_30.s | [src/non_matching/ovl_7db0c8/2008540.c](../../src/non_matching/ovl_7db0c8/2008540.c) |
| 42 | 15 | 107 | `OvlFunc_932_20086dc` | overlays/ovl_30.s | [src/non_matching/ovl_7b9cb4/20086dc.c](../../src/non_matching/ovl_7b9cb4/20086dc.c) |
| 43 |  | 45 | `Func_8012388` | rom_9000/rom_1219c.s | [src/non_matching/rom_9000/8012388.c](../../src/non_matching/rom_9000/8012388.c) |
| 43 |  | 53 | `Func_80a51d0` | rom_a1000/rom_a4f08.s | [src/non_matching/rom_a1000/80a51d0.c](../../src/non_matching/rom_a1000/80a51d0.c) |
| 43 |  | 62 | `OvlFunc_957_2008b30` | overlays/ovl_30.s | [src/non_matching/ovl_7e3e08/2008b30.c](../../src/non_matching/ovl_7e3e08/2008b30.c) |
| 44 |  | 61 | `OvlFunc_924_20090c0` | overlays/ovl_f84.s | [src/non_matching/ovl_7ac2d8/20090c0.c](../../src/non_matching/ovl_7ac2d8/20090c0.c) |
| 44 |  | 65 | `OvlFunc_948_2009ac8` | overlays/ovl_30.s | [src/non_matching/ovl_7d30e0/2009ac8.c](../../src/non_matching/ovl_7d30e0/2009ac8.c) |
| 44 |  | 44 | `UnpackTilemap` | rom_9000/rom_f9cc.s | [src/non_matching/rom_9000/800fac8.c](../../src/non_matching/rom_9000/800fac8.c) |
| 45 |  | 64 | `Func_80a3c08` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a3c08.c](../../src/non_matching/rom_a1000/80a3c08.c) |
| 45 |  | 205 | `Func_80b7738` | rom_b5000/rom_b7410.s | [src/non_matching/rom_b5000/80b7738.c](../../src/non_matching/rom_b5000/80b7738.c) |
| 45 |  | 272 | `OvlFunc_899_200c8c8` | overlays/ovl_30.s | [src/non_matching/ovl_794ac0/200c8c8.c](../../src/non_matching/ovl_794ac0/200c8c8.c) |
| 45 | 1 | 50 | `Func_80e73a0` | rom_c9000/rom_e7320.s | [src/non_matching/rom_c9000/80e73a0.c](../../src/non_matching/rom_c9000/80e73a0.c) |
| 46 |  | 493 | `BaseAnim_Blast` | rom_c9000/rom_db6c8.s | [src/non_matching/rom_c9000/80db6e0.c](../../src/non_matching/rom_c9000/80db6e0.c) |
| 46 |  | 41 | `OvlFunc_970_20092ac` | overlays/ovl_30.s | [src/non_matching/ovl_7fa4ec/20092ac.c](../../src/non_matching/ovl_7fa4ec/20092ac.c) |
| 47 |  | 63 | `Func_80167e0` | rom_15000/rom_15e8c.s | [src/non_matching/rom_15000/80167e0.c](../../src/non_matching/rom_15000/80167e0.c) |
| 47 |  | 133 | `Func_80a68ec` | rom_a1000/rom_a5534.s | [src/non_matching/rom_a1000/80a68ec.c](../../src/non_matching/rom_a1000/80a68ec.c) |
| 47 |  | 209 | `Func_80c0a24` | rom_b5000/rom_bffb8.s | [src/non_matching/rom_b5000/80c0a24.c](../../src/non_matching/rom_b5000/80c0a24.c) |
| 47 |  | 144 | `OvlFunc_882_200c41c` | overlays/ovl_30.s | [src/non_matching/ovl_77dd1c/200c41c.c](../../src/non_matching/ovl_77dd1c/200c41c.c) |
| 47 |  | 51 | `OvlFunc_948_2009838` | overlays/ovl_30.s | [src/non_matching/rom_7d30e0/2009838.c](../../src/non_matching/rom_7d30e0/2009838.c) |
| 48 |  | 73 | `DecodeMetatileset` | rom_9000/rom_f9cc.s | [src/non_matching/rom_9000/800f9f4.c](../../src/non_matching/rom_9000/800f9f4.c) |
| 48 |  | 55 | `Func_8010560` | rom_9000/rom_10424.s | [src/non_matching/rom_9000/8010560.c](../../src/non_matching/rom_9000/8010560.c) |
| 48 |  | 56 | `Func_8011b54` | rom_9000/rom_11568.s | [src/non_matching/rom_9000/8011b54.c](../../src/non_matching/rom_9000/8011b54.c) |
| 48 |  | 168 | `Func_8093e28` | rom_8a000/rom_93304.s | [src/non_matching/rom_8a000/8093e28.c](../../src/non_matching/rom_8a000/8093e28.c) |
| 48 |  | 569 | `Func_80a9f10` | rom_a1000/rom_a8604.s | [src/non_matching/rom_a1000/80a9f10.c](../../src/non_matching/rom_a1000/80a9f10.c) |
| 48 |  | 157 | `OvlFunc_969_200da28` | overlays/ovl_314.s | [src/non_matching/ovl_7f6e64/200da28.c](../../src/non_matching/ovl_7f6e64/200da28.c) |
| 48 |  | 82 | `UpdateScreenEdge_H` | rom_9000/rom_f9cc.s | [src/non_matching/rom_9000/800ff54.c](../../src/non_matching/rom_9000/800ff54.c) |
| 49 |  | 60 | `Func_8003e58` | rom_c0/rom_3e58.s | [src/non_matching/rom_c0/8003e58.c](../../src/non_matching/rom_c0/8003e58.c) |
| 49 |  | 155 | `Func_80b5f0c` | rom_b5000/rom_b5a0c.s | [src/non_matching/rom_b5000/80b5f0c.c](../../src/non_matching/rom_b5000/80b5f0c.c) |
| 49 |  | 195 | `StartMenu_Main` | rom_15000/rom_1ca1c.s | [src/non_matching/rom_15000/801ca1c_StartMenu_Main.c](../../src/non_matching/rom_15000/801ca1c_StartMenu_Main.c) |
| 50 |  | 69 | `Func_80a355c` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a355c.c](../../src/non_matching/rom_a1000/80a355c.c) |
| 51 |  | 60 | `Func_807a7a0` | rom_77000/rom_79460.s | [src/non_matching/rom_77000/807a7a0.c](../../src/non_matching/rom_77000/807a7a0.c) |
| 51 |  | 60 | `Func_80a8b10` | rom_a1000/rom_a8604.s | [src/non_matching/rom_a1000/80a8b10.c](../../src/non_matching/rom_a1000/80a8b10.c) |
| 51 |  | 60 | `GetPortrait` | rom_15000/rom_19d2c.s | [src/non_matching/rom_15000/8019d2c.c](../../src/non_matching/rom_15000/8019d2c.c) |
| 52 |  | 63 | `Func_80110e0` | rom_9000/rom_108e4.s | [src/non_matching/rom_9000/80110e0.c](../../src/non_matching/rom_9000/80110e0.c) |
| 53 |  | 59 | `Func_8077cb8` | rom_77000/rom_77320.s | [src/non_matching/rom_77000/8077cb8.c](../../src/non_matching/rom_77000/8077cb8.c) |
| 53 |  | 110 | `Func_808a5f8` | rom_8a000/rom_8a5f8.s | [src/non_matching/rom_8a000/808a5f8.c](../../src/non_matching/rom_8a000/808a5f8.c) |
| 53 |  | 65 | `Func_80c9048` | rom_c9000/rom_c9048.s | [src/non_matching/rom_c9000/80c9048.c](../../src/non_matching/rom_c9000/80c9048.c) |
| 53 |  | 53 | `OvlFunc_882_2009498` | overlays/ovl_30.s | [src/non_matching/ovl_77dd1c/2009498.c](../../src/non_matching/ovl_77dd1c/2009498.c) |
| 53 | 14 | 100 | `OvlFunc_947_200a384` | overlays/ovl_1528.s | [src/non_matching/ovl_7d0e88/200a384.c](../../src/non_matching/ovl_7d0e88/200a384.c) |
| 55 |  | 97 | `Func_80114a0` | rom_9000/rom_108e4.s | [src/non_matching/rom_9000/80114a0.c](../../src/non_matching/rom_9000/80114a0.c) |
| 55 |  | 87 | `Func_801b424` | rom_15000/rom_1aeec.s | [src/non_matching/rom_15000/801b424.c](../../src/non_matching/rom_15000/801b424.c) |
| 55 |  | 63 | `OvlFunc_959_200a718` | overlays/ovl_9dc.s | [src/non_matching/overlays/200a718.c](../../src/non_matching/overlays/200a718.c) |
| 56 |  | 54 | `OvlFunc_903_200843c` | overlays/ovl_314.s | [src/non_matching/overlays/ovl903_200843c.c](../../src/non_matching/overlays/ovl903_200843c.c) |
| 57 |  | 530 | `?` | — | [src/non_matching/ovl_7c7b9c/200ab7c.c](../../src/non_matching/ovl_7c7b9c/200ab7c.c) |
| 57 |  | 61 | `OvlFunc_946_2008e00` | overlays/ovl_30.s | [src/non_matching/overlays/2008e00.c](../../src/non_matching/overlays/2008e00.c) |
| 57 | 7 | 93 | `OvlFunc_935_20089c0` | overlays/ovl_2e0.s | [src/non_matching/ovl_7bf5a8/20089c0.c](../../src/non_matching/ovl_7bf5a8/20089c0.c) |
| 58 |  | 68 | `GiveItemTo` | rom_77000/rom_78414.s | [src/non_matching/rom_77000/8078588.c](../../src/non_matching/rom_77000/8078588.c) |
| 58 | 5 | 233 | `Anim_Curse` | rom_c9000/rom_d5258.s | [src/non_matching/rom_c9000/d5c48_Curse.c](../../src/non_matching/rom_c9000/d5c48_Curse.c) |
| 59 |  | 60 | `Func_8011f54` | rom_9000/rom_11ce0.s | [src/non_matching/rom_9000/8011f54.c](../../src/non_matching/rom_9000/8011f54.c) |
| 59 |  | 114 | `Func_80b5864` | rom_b5000/rom_b5368.s | [src/non_matching/rom_b5000/80b5864.c](../../src/non_matching/rom_b5000/80b5864.c) |
| 59 | 2 | 136 | `Func_80123f4` | rom_9000/rom_1219c.s | [src/non_matching/rom_9000/80123f4.c](../../src/non_matching/rom_9000/80123f4.c) |
| 60 |  | 69 | `CutsceneStart` | rom_8a000/rom_91584.s | [src/non_matching/rom_8a000/916b0.c](../../src/non_matching/rom_8a000/916b0.c) |
| 60 |  | 87 | `Func_808d828` | rom_8a000/rom_8d5dc.s | [src/non_matching/rom_8a000/808d828.c](../../src/non_matching/rom_8a000/808d828.c) |
| 60 |  | 147 | `OvlFunc_886_2008368` | overlays/ovl_30.s | [src/non_matching/ovl_786f0c/2008368.c](../../src/non_matching/ovl_786f0c/2008368.c) |
| 60 |  | 309 | `ScreenTransitionIn` | rom_8a000/rom_8d9a4.s | [src/non_matching/rom_8a000/808fefc.c](../../src/non_matching/rom_8a000/808fefc.c) |
| 62 |  | 61 | `GetLocationName` | rom_8a000/rom_8ace0.s | [src/non_matching/rom_8a000/808b158.c](../../src/non_matching/rom_8a000/808b158.c) |
| 63 |  | 76 | `Func_80a9aec` | rom_a1000/rom_a8604.s | [src/non_matching/rom_a1000/80a9aec.c](../../src/non_matching/rom_a1000/80a9aec.c) |
| 63 | 14 | 134 | `OvlFunc_958_2008df0` | overlays/ovl_cc0.s | [src/non_matching/ovl_7e636c/2008df0.c](../../src/non_matching/ovl_7e636c/2008df0.c) |
| 64 |  | 266 | `Func_808e23c` | rom_8a000/rom_8d9a4.s | [src/non_matching/rom_8a000/808e23c.c](../../src/non_matching/rom_8a000/808e23c.c) |
| 64 |  | 92 | `OvlFunc_960_2008b24` | overlays/ovl_314.s | [src/non_matching/ovl_7eaf28/2008b24.c](../../src/non_matching/ovl_7eaf28/2008b24.c) |
| 65 |  | 106 | `OvlFunc_883_200d64c` | overlays/ovl_30.s | [src/non_matching/ovl_780898/200d64c.c](../../src/non_matching/ovl_780898/200d64c.c) |
| 65 |  | 64 | `OvlFunc_924_2009c9c` | overlays/ovl_f84.s | [src/non_matching/ovl_7ac2d8/2009c9c.c](../../src/non_matching/ovl_7ac2d8/2009c9c.c) |
| 66 |  | 79 | `Func_80c0eec` | rom_b5000/rom_bffb8.s | [src/non_matching/rom_b5000/80c0eec.c](../../src/non_matching/rom_b5000/80c0eec.c) |
| 66 |  | 77 | `Sprite_SetAnim` | rom_9000/rom_b798.s | [src/non_matching/rom_9000/800ba30.c](../../src/non_matching/rom_9000/800ba30.c) |
| 67 |  | 123 | `Func_80a1ac0` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a1ac0.c](../../src/non_matching/rom_a1000/80a1ac0.c) |
| 67 |  | 83 | `Task_BlitAnim_BG1Wide` | rom_c9000/rom_cd260.s | [src/non_matching/rom_c9000/80cd358.c](../../src/non_matching/rom_c9000/80cd358.c) |
| 68 |  | 72 | `Func_80164d4` | rom_15000/rom_15e8c.s | [src/non_matching/rom_15000/80164d4.c](../../src/non_matching/rom_15000/80164d4.c) |
| 68 |  | 87 | `Func_807a3a8` | rom_77000/rom_79460.s | [src/non_matching/rom_77000/807a3a8.c](../../src/non_matching/rom_77000/807a3a8.c) |
| 70 |  | 86 | `OvlFunc_947_2008cc0` | overlays/ovl_314.s | [src/non_matching/overlays/2008cc0.c](../../src/non_matching/overlays/2008cc0.c) |
| 71 |  | 316 | `AnimStart` | rom_c9000/rom_cd508.s | [src/non_matching/rom_c9000/80cd594.c](../../src/non_matching/rom_c9000/80cd594.c) |
| 72 |  | 76 | `Func_80a602c` | rom_a1000/rom_a5534.s | [src/non_matching/rom_a1000/80a602c.c](../../src/non_matching/rom_a1000/80a602c.c) |
| 72 |  | 75 | `OvlFunc_956_2008ba4` | overlays/ovl_30.s | [src/non_matching/ovl_7e0928/2008ba4.c](../../src/non_matching/ovl_7e0928/2008ba4.c) |
| 73 |  | 91 | `Func_80113e4` | rom_9000/rom_108e4.s | [src/non_matching/rom_9000/80113e4.c](../../src/non_matching/rom_9000/80113e4.c) |
| 73 |  | 121 | `Func_80ad40c` | rom_a1000/rom_ad274.s | [src/non_matching/rom_a1000/80ad40c.c](../../src/non_matching/rom_a1000/80ad40c.c) |
| 73 |  | 66 | `UpdateScreenEdge_V` | rom_9000/rom_f9cc.s | [src/non_matching/rom_9000/800fec8.c](../../src/non_matching/rom_9000/800fec8.c) |
| 77 |  | 87 | `Func_8005fcc` | rom_c0/rom_5cf8.s | [src/non_matching/rom_c0/8005fcc.c](../../src/non_matching/rom_c0/8005fcc.c) |
| 77 |  | 87 | `Func_8022768` | rom_15000/rom_21dfc.s | [src/non_matching/rom_15000/8022768.c](../../src/non_matching/rom_15000/8022768.c) |
| 78 |  | 115 | `Func_8011bf4` | rom_9000/rom_11568.s | [src/non_matching/rom_9000/8011bf4.c](../../src/non_matching/rom_9000/8011bf4.c) |
| 78 |  | 86 | `OvlFunc_916_2008098` | overlays/ovl_30.s | [src/non_matching/overlays/2008098.c](../../src/non_matching/overlays/2008098.c) |
| 78 |  | 89 | `TestCollision` | rom_9000/rom_11ce0.s | [src/non_matching/rom_9000/80120dc.c](../../src/non_matching/rom_9000/80120dc.c) |
| 79 |  | 145 | `Func_8096fb0` | rom_8a000/rom_96cdc.s | [src/non_matching/rom_8a000/8096fb0.c](../../src/non_matching/rom_8a000/8096fb0.c) |
| 79 | 1 | 94 | `Func_80b5a0c` | rom_b5000/rom_b5a0c.s | [src/non_matching/rom_b5000/80b5a0c.c](../../src/non_matching/rom_b5000/80b5a0c.c) |
| 80 |  | 103 | `OvlFunc_969_200b6d0` | overlays/ovl_314.s | [src/non_matching/overlays/200b6d0.c](../../src/non_matching/overlays/200b6d0.c) |
| 81 |  | 102 | `Func_8021390` | rom_15000/rom_20198.s | [src/non_matching/rom_15000/8021390.c](../../src/non_matching/rom_15000/8021390.c) |
| 81 |  | 145 | `OvlFunc_956_200804c` | overlays/ovl_30.s | [src/non_matching/ovl_7e0928/200804c.c](../../src/non_matching/ovl_7e0928/200804c.c) |
| 83 |  | 303 | `AnimStart2` | rom_c9000/rom_cd508.s | [src/non_matching/rom_c9000/80cd86c.c](../../src/non_matching/rom_c9000/80cd86c.c) |
| 84 |  | 197 | `DrawMsgGlyph` | rom_15000/rom_178b0.s | [src/non_matching/rom_15000/80178b0.c](../../src/non_matching/rom_15000/80178b0.c) |
| 84 |  | 109 | `Func_800615c` | rom_c0/rom_5cf8.s | [src/non_matching/rom_c0/800615c.c](../../src/non_matching/rom_c0/800615c.c) |
| 84 |  | 108 | `Func_801bd98` | rom_15000/rom_1aeec.s | [src/non_matching/rom_15000/801bd98.c](../../src/non_matching/rom_15000/801bd98.c) |
| 84 |  | 96 | `Func_808b090` | rom_8a000/rom_8ace0.s | [src/non_matching/rom_8a000/808b090.c](../../src/non_matching/rom_8a000/808b090.c) |
| 86 |  | 86 | `ColorCycleVFXPalette` | rom_c9000/rom_d2d98.s | [src/non_matching/rom_c9000/ColorCycleVFXPalette.c](../../src/non_matching/rom_c9000/ColorCycleVFXPalette.c) |
| 86 |  | 179 | `CreateActor` | rom_9000/rom_c004.s | [src/non_matching/rom_9000/CreateActor.c](../../src/non_matching/rom_9000/CreateActor.c) |
| 86 |  | 86 | `Func_80119cc` | rom_9000/rom_11568.s | [src/non_matching/rom_9000/80119cc.c](../../src/non_matching/rom_9000/80119cc.c) |
| 86 |  | 106 | `OvlFunc_912_20081c4` | overlays/ovl_30.s | [src/non_matching/ovl_7a0010/20081c4.c](../../src/non_matching/ovl_7a0010/20081c4.c) |
| 86 |  | 246 | `Task_08097644` | rom_8a000/rom_97384.s | [src/non_matching/rom_8a000/8097644.c](../../src/non_matching/rom_8a000/8097644.c) |
| 88 |  | 106 | `OvlFunc_926_2008e94` | overlays/ovl_314.s | [src/non_matching/ovl_7b2078/2008e94.c](../../src/non_matching/ovl_7b2078/2008e94.c) |
| 90 |  | 136 | `Func_8026e80` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/8026e80.c](../../src/non_matching/rom_15000/8026e80.c) |
| 92 | 14 | 374 | `Func_801c49c` | rom_15000/rom_1aeec.s | [src/non_matching/rom_15000/801c49c.c](../../src/non_matching/rom_15000/801c49c.c) |
| 93 |  | 122 | `OvlFunc_952_200be40` | overlays/ovl_30.s | [src/non_matching/ovl_7d768c/200be40.c](../../src/non_matching/ovl_7d768c/200be40.c) |
| 93 | 8 | 267 | `Anim_Cast` | rom_b5000/rom_c10e8.s | [src/non_matching/rom_b5000/80c1470_Anim_Cast.c](../../src/non_matching/rom_b5000/80c1470_Anim_Cast.c) |
| 94 |  | 192 | `Func_800c880` | rom_9000/rom_c880.s | [src/non_matching/rom_9000/800c880.c](../../src/non_matching/rom_9000/800c880.c) |
| 94 |  | 110 | `Func_801fe2c` | rom_15000/rom_1fe2c.s | [src/non_matching/rom_15000/801fe2c.c](../../src/non_matching/rom_15000/801fe2c.c) |
| 94 |  | 242 | `Func_80b1260` | rom_b0000/rom_b0070.s | [src/non_matching/rom_b0000/80b1260.c](../../src/non_matching/rom_b0000/80b1260.c) |
| 97 |  | 220 | `BaseAnim_FullScreenSlash` | rom_c9000/rom_ece7c.s | [src/non_matching/rom_c9000/80ecef4.c](../../src/non_matching/rom_c9000/80ecef4.c) |
| 97 |  | 119 | `Func_80bb7c0` | rom_b5000/rom_bb588.s | [src/non_matching/rom_b5000/80bb7c0.c](../../src/non_matching/rom_b5000/80bb7c0.c) |
| 97 |  | 114 | `OvlFunc_949_200807c` | overlays/ovl_30.s | [src/non_matching/ovl_7d4af4/200807c.c](../../src/non_matching/ovl_7d4af4/200807c.c) |
| 98 |  | 151 | `OvlFunc_917_2009070` | overlays/ovl_30.s | [src/non_matching/ovl_7a4370/2009070.c](../../src/non_matching/ovl_7a4370/2009070.c) |
| 98 |  | 106 | `PreloadSpriteGFX` | rom_9000/rom_b074.s | [src/non_matching/rom_9000/800b6b8.c](../../src/non_matching/rom_9000/800b6b8.c) |
| 101 |  | 112 | `Func_8010ff0` | rom_9000/rom_108e4.s | [src/non_matching/rom_9000/8010ff0.c](../../src/non_matching/rom_9000/8010ff0.c) |
| 101 |  | 132 | `Func_8021228` | rom_15000/rom_20198.s | [src/non_matching/rom_15000/8021228.c](../../src/non_matching/rom_15000/8021228.c) |
| 102 |  | 130 | `Func_808bb2c` | rom_8a000/rom_8ba38.s | [src/non_matching/rom_8a000/808bb2c.c](../../src/non_matching/rom_8a000/808bb2c.c) |
| 102 |  | 283 | `OvlFunc_946_2008f70` | overlays/ovl_30.s | [src/non_matching/ovl_7ced6c/2008f70.c](../../src/non_matching/ovl_7ced6c/2008f70.c) |
| 104 |  | 148 | `OvlFunc_959_200cf60` | overlays/ovl_9dc.s | [src/non_matching/ovl_7e7574/200cf60.c](../../src/non_matching/ovl_7e7574/200cf60.c) |
| 105 |  | 252 | `DrawInventoryIcon` | rom_15000/rom_19ebc.s | [src/non_matching/rom_15000/801a088.c](../../src/non_matching/rom_15000/801a088.c) |
| 105 |  | 135 | `Func_8019bfc` | rom_15000/rom_1908c.s | [src/non_matching/rom_15000/8019bfc.c](../../src/non_matching/rom_15000/8019bfc.c) |
| 108 |  | 142 | `Func_80799b0` | rom_77000/rom_79460.s | [src/non_matching/rom_77000/80799b0.c](../../src/non_matching/rom_77000/80799b0.c) |
| 109 |  | 205 | `DrawLine` | rom_c9000/rom_cd508.s | [src/non_matching/rom_c9000/80cde90.c](../../src/non_matching/rom_c9000/80cde90.c) |
| 109 |  | 127 | `Func_8017658` | rom_15000/rom_15e8c.s | [src/non_matching/rom_15000/8017658.c](../../src/non_matching/rom_15000/8017658.c) |
| 109 |  | 133 | `OvlFunc_883_200dd68` | overlays/ovl_30.s | [src/non_matching/ovl_780898/200dd68.c](../../src/non_matching/ovl_780898/200dd68.c) |
| 109 | 3 | 107 | `OvlFunc_921_20095b4` | overlays/ovl_30.s | [src/non_matching/ovl_7a7298/20095b4.c](../../src/non_matching/ovl_7a7298/20095b4.c) |
| 111 |  | 101 | `Func_80aad10` | rom_a1000/rom_aa538.s | [src/non_matching/rom_a1000/80aad10.c](../../src/non_matching/rom_a1000/80aad10.c) |
| 111 |  | 133 | `OvlFunc_881_200c058` | overlays/ovl_30.s | [src/non_matching/ovl_77a7c8/200c058.c](../../src/non_matching/ovl_77a7c8/200c058.c) |
| 112 |  | 128 | `Func_801a66c` | rom_15000/rom_1a66c.s | [src/non_matching/rom_15000/801a66c.c](../../src/non_matching/rom_15000/801a66c.c) |
| 112 |  | 121 | `UpdateMusicSettings` | rom_f9000/rom_f9080.s | [src/non_matching/rom_f9000/rom_f91e8.c](../../src/non_matching/rom_f9000/rom_f91e8.c) |
| 112 | 2 | 126 | `OvlFunc_960_20089cc` | overlays/ovl_314.s | [src/non_matching/ovl_7eaf28/20089cc.c](../../src/non_matching/ovl_7eaf28/20089cc.c) |
| 113 |  | 151 | `OvlFunc_924_20097a8` | overlays/ovl_f84.s | [src/non_matching/ovl_7ac2d8/20097a8.c](../../src/non_matching/ovl_7ac2d8/20097a8.c) |
| 113 | 1 | 236 | `RespawnAtSanctum` | rom_8a000/rom_8a5f8.s | [src/non_matching/rom_8a000/RespawnAtSanctum.c](../../src/non_matching/rom_8a000/RespawnAtSanctum.c) |
| 113 | 3 | 120 | `Func_80108e4` | rom_9000/rom_108e4.s | [src/non_matching/rom_9000/80108e4.c](../../src/non_matching/rom_9000/80108e4.c) |
| 115 |  | 297 | `OvlFunc_956_200876c` | overlays/ovl_30.s | [src/non_matching/ovl_7e0928/200876c.c](../../src/non_matching/ovl_7e0928/200876c.c) |
| 116 |  | 131 | `Func_80b920c` | rom_b5000/rom_b8228.s | [src/non_matching/rom_b5000/80b920c.c](../../src/non_matching/rom_b5000/80b920c.c) |
| 116 | 2 | 200 | `call_via` | rom_8a000/rom_8ace0.s | [src/non_matching/rom_8a000/808ae74.c](../../src/non_matching/rom_8a000/808ae74.c) |
| 118 |  | 770 | `Anim_Unused_Fizz` | rom_c9000/rom_d5258.s | [src/non_matching/rom_c9000/Anim_Unused_Fizz.c](../../src/non_matching/rom_c9000/Anim_Unused_Fizz.c) |
| 118 |  | 150 | `Func_8010788` | rom_9000/rom_10424.s | [src/non_matching/rom_9000/8010788.c](../../src/non_matching/rom_9000/8010788.c) |
| 119 |  | 125 | `AnimTransitionIn` | rom_b5000/rom_bffb8.s | [src/non_matching/rom_b5000/80c08ec.c](../../src/non_matching/rom_b5000/80c08ec.c) |
| 120 | 12 | 318 | `OvlFunc_971_2008860` | overlays/ovl_30.s | [src/non_matching/ovl_7fb4a8/2008860.c](../../src/non_matching/ovl_7fb4a8/2008860.c) |
| 121 |  | 232 | `Func_80a93a4` | rom_a1000/rom_a8604.s | [src/non_matching/rom_a1000/80a93a4.c](../../src/non_matching/rom_a1000/80a93a4.c) |
| 122 |  | 145 | `Func_80105d4` | rom_9000/rom_10424.s | [src/non_matching/rom_9000/80105d4.c](../../src/non_matching/rom_9000/80105d4.c) |
| 124 |  | 149 | `Func_80056cc` | rom_c0/rom_56cc.s | [src/non_matching/rom_c0/80056cc.c](../../src/non_matching/rom_c0/80056cc.c) |
| 124 |  | 123 | `Func_80a5b94` | rom_a1000/rom_a5534.s | [src/non_matching/rom_a1000/80a5b94.c](../../src/non_matching/rom_a1000/80a5b94.c) |
| 124 | 1 | 142 | `Func_8021488` | rom_15000/rom_20198.s | [src/non_matching/rom_15000/8021488.c](../../src/non_matching/rom_15000/8021488.c) |
| 126 |  | 204 | `Func_80b24e4` | rom_b0000/rom_b0070.s | [src/non_matching/rom_b0000/80b24e4.c](../../src/non_matching/rom_b0000/80b24e4.c) |
| 126 |  | 146 | `OvlFunc_942_200851c` | overlays/ovl_30.s | [src/non_matching/ovl_7c6bac/200851c.c](../../src/non_matching/ovl_7c6bac/200851c.c) |
| 127 |  | 144 | `OvlFunc_887_20093e4` | overlays/ovl_30.s | [src/non_matching/ovl_787e04/20093e4.c](../../src/non_matching/ovl_787e04/20093e4.c) |
| 129 |  | 289 | `OvlFunc_951_2008ac8` | overlays/ovl_30.s | [src/non_matching/ovl_7d6418/2008ac8.c](../../src/non_matching/ovl_7d6418/2008ac8.c) |
| 130 |  | 156 | `Func_80aae14` | rom_a1000/rom_aa538.s | [src/non_matching/rom_a1000/80aae14.c](../../src/non_matching/rom_a1000/80aae14.c) |
| 132 |  | 505 | `Func_80a4924` | rom_a1000/rom_a47b4.s | [src/non_matching/rom_a1000/80a4924.c](../../src/non_matching/rom_a1000/80a4924.c) |
| 134 |  | 146 | `Debug_SoundTest` | rom_f9000/rom_f9080.s | [src/non_matching/rom_f9000/rom_f92fc.c](../../src/non_matching/rom_f9000/rom_f92fc.c) |
| 134 |  | 130 | `Func_8006240` | rom_c0/rom_5cf8.s | [src/non_matching/rom_c0/8006240.c](../../src/non_matching/rom_c0/8006240.c) |
| 134 |  | 167 | `Func_80a6614` | rom_a1000/rom_a5534.s | [src/non_matching/rom_a1000/80a6614.c](../../src/non_matching/rom_a1000/80a6614.c) |
| 134 |  | 130 | `OvlFunc_888_200a7d4` | overlays/ovl_30.s | [src/non_matching/ovl_7892c8/200a7d4.c](../../src/non_matching/ovl_7892c8/200a7d4.c) |
| 134 |  | 180 | `OvlFunc_967_200904c` | overlays/ovl_30.s | [src/non_matching/ovl_7f21b8/200904c.c](../../src/non_matching/ovl_7f21b8/200904c.c) |
| 135 |  | 150 | `CopyMapTiles` | rom_9000/rom_10424.s | [src/non_matching/rom_9000/CopyMapTiles.c](../../src/non_matching/rom_9000/CopyMapTiles.c) |
| 136 |  | 201 | `Func_8010e14` | rom_9000/rom_108e4.s | [src/non_matching/rom_9000/8010e14.c](../../src/non_matching/rom_9000/8010e14.c) |
| 136 |  | 139 | `Func_8028574` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/8028574.c](../../src/non_matching/rom_15000/8028574.c) |
| 137 |  | 135 | `OvlFunc_959_200938c` | overlays/ovl_9dc.s | [src/non_matching/ovl_7e7574/200938c.c](../../src/non_matching/ovl_7e7574/200938c.c) |
| 138 |  | 143 | `CamelotLogo` | rom_f2000/rom_f2028.s | [src/non_matching/rom_f2000/80f2d54.c](../../src/non_matching/rom_f2000/80f2d54.c) |
| 138 |  | 120 | `OvlFunc_957_2008f94` | overlays/ovl_30.s | [src/non_matching/ovl_7e3e08/2008f94.c](../../src/non_matching/ovl_7e3e08/2008f94.c) |
| 140 |  | 148 | `Func_801dd28` | rom_15000/rom_1ca1c.s | [src/non_matching/rom_15000/801dd28.c](../../src/non_matching/rom_15000/801dd28.c) |
| 140 |  | 192 | `Func_8093fa0` | rom_8a000/rom_93304.s | [src/non_matching/rom_8a000/8093fa0.c](../../src/non_matching/rom_8a000/8093fa0.c) |
| 140 |  | 184 | `Func_80b8c1c` | rom_b5000/rom_b8228.s | [src/non_matching/rom_b5000/80b8c1c.c](../../src/non_matching/rom_b5000/80b8c1c.c) |
| 141 |  | 169 | `AnimTransitionOut` | rom_c9000/rom_cc5d8.s | [src/non_matching/rom_c9000/80cd104.c](../../src/non_matching/rom_c9000/80cd104.c) |
| 141 |  | 162 | `Debug_BattleTest` | rom_b5000/rom_b5368.s | [src/non_matching/rom_b5000/80b5368_Debug_BattleTest.c](../../src/non_matching/rom_b5000/80b5368_Debug_BattleTest.c) |
| 141 |  | 193 | `Func_802106c` | rom_15000/rom_20198.s | [src/non_matching/rom_15000/802106c.c](../../src/non_matching/rom_15000/802106c.c) |
| 141 |  | 151 | `PlaySound` | rom_f9000/rom_f9080.s | [src/non_matching/rom_f9000/rom_f9080.c](../../src/non_matching/rom_f9000/rom_f9080.c) |
| 142 |  | 162 | `Func_80228e4` | rom_15000/rom_21dfc.s | [src/non_matching/rom_15000/80228e4.c](../../src/non_matching/rom_15000/80228e4.c) |
| 143 |  | 219 | `Func_8018850` | rom_15000/rom_17e88.s | [src/non_matching/rom_15000/8018850.c](../../src/non_matching/rom_15000/8018850.c) |
| 143 |  | 206 | `OvlFunc_925_200835c` | overlays/ovl_314.s | [src/non_matching/ovl_7b0400/200835c.c](../../src/non_matching/ovl_7b0400/200835c.c) |
| 143 |  | 149 | `OvlFunc_963_2008124` | overlays/ovl_30.s | [src/non_matching/ovl_7ec968/2008124.c](../../src/non_matching/ovl_7ec968/2008124.c) |
| 143 | 3 | 400 | `Anim_Drain` | rom_c9000/rom_d82b0.s | [src/non_matching/rom_c9000/d82b0_Drain.c](../../src/non_matching/rom_c9000/d82b0_Drain.c) |
| 147 |  | 291 | `Anim_Sleep` | rom_c9000/rom_d5258.s | [src/non_matching/rom_c9000/80d59b0.c](../../src/non_matching/rom_c9000/80d59b0.c) |
| 147 | 1 | 170 | `OvlFunc_959_200c9a0` | overlays/ovl_9dc.s | [src/non_matching/ovl_7e7574/200c9a0.c](../../src/non_matching/ovl_7e7574/200c9a0.c) |
| 148 |  | 138 | `OvlFunc_969_200a200` | overlays/ovl_314.s | [src/non_matching/ovl_7f6e64/200a200.c](../../src/non_matching/ovl_7f6e64/200a200.c) |
| 149 |  | 265 | `Func_8018a50` | rom_15000/rom_17e88.s | [src/non_matching/rom_15000/8018a50.c](../../src/non_matching/rom_15000/8018a50.c) |
| 150 |  | 161 | `OvlFunc_970_2008f80` | overlays/ovl_30.s | [src/non_matching/ovl_7fa4ec/2008f80.c](../../src/non_matching/ovl_7fa4ec/2008f80.c) |
| 151 |  | 163 | `OvlFunc_942_2008958` | overlays/ovl_30.s | [src/non_matching/ovl_7c6bac/2008958.c](../../src/non_matching/ovl_7c6bac/2008958.c) |
| 151 | 3 | 248 | `Func_80111b4` | rom_9000/rom_108e4.s | [src/non_matching/rom_9000/80111b4.c](../../src/non_matching/rom_9000/80111b4.c) |
| 155 |  | 274 | `Anim_MoveIntro` | rom_b5000/rom_c10e8.s | [src/non_matching/rom_b5000/Anim_MoveIntro.c](../../src/non_matching/rom_b5000/Anim_MoveIntro.c) |
| 155 |  | 174 | `Func_801f088` | rom_15000/rom_1de5c.s | [src/non_matching/rom_15000/801f088.c](../../src/non_matching/rom_15000/801f088.c) |
| 155 | 3 | 273 | `Anim_Unused_ElementOrbs` | rom_c9000/rom_dbbdc.s | [src/non_matching/rom_c9000/dbbdc_ElementOrbs.c](../../src/non_matching/rom_c9000/dbbdc_ElementOrbs.c) |
| 155 | 16 | 255 | `OvlFunc_970_2008b34` | overlays/ovl_30.s | [src/non_matching/ovl_7fa4ec/2008b34.c](../../src/non_matching/ovl_7fa4ec/2008b34.c) |
| 159 |  | 184 | `Func_80a5388` | rom_a1000/rom_a4f08.s | [src/non_matching/rom_a1000/80a5388.c](../../src/non_matching/rom_a1000/80a5388.c) |
| 161 |  | 190 | `Func_801b810` | rom_15000/rom_1aeec.s | [src/non_matching/rom_15000/801b810.c](../../src/non_matching/rom_15000/801b810.c) |
| 166 |  | 174 | `Func_80a24d0` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a24d0.c](../../src/non_matching/rom_a1000/80a24d0.c) |
| 167 |  | 211 | `Func_80a7d68` | rom_a1000/rom_a7380.s | [src/non_matching/rom_a1000/80a7d68.c](../../src/non_matching/rom_a1000/80a7d68.c) |
| 168 |  | 222 | `ActorCmd_Player_Climb` | rom_9000/rom_ebec.s | [src/non_matching/rom_9000/800f7f4.c](../../src/non_matching/rom_9000/800f7f4.c) |
| 169 |  | 187 | `Func_80b5534` | rom_b5000/rom_b5368.s | [src/non_matching/rom_b5000/80b5534.c](../../src/non_matching/rom_b5000/80b5534.c) |
| 170 |  | 185 | `PrepareSaveHeader` | rom_15000/rom_1de5c.s | [src/non_matching/rom_15000/801f818.c](../../src/non_matching/rom_15000/801f818.c) |
| 171 | 1 | 256 | `OvlFunc_971_2008580` | overlays/ovl_30.s | [src/non_matching/ovl_7fb4a8/2008580.c](../../src/non_matching/ovl_7fb4a8/2008580.c) |
| 171 | 4 | 375 | `Anim_DeathPlunge` | rom_c9000/rom_cb1a4.s | [src/non_matching/rom_c9000/cb1a4_DeathPlunge.c](../../src/non_matching/rom_c9000/cb1a4_DeathPlunge.c) |
| 172 |  | 218 | `Func_801ba68` | rom_15000/rom_1aeec.s | [src/non_matching/rom_15000/801ba68.c](../../src/non_matching/rom_15000/801ba68.c) |
| 172 |  | 476 | `Func_80a5788` | rom_a1000/rom_a5534.s | [src/non_matching/rom_a1000/80a5788.c](../../src/non_matching/rom_a1000/80a5788.c) |
| 172 |  | 239 | `Func_80a8d34` | rom_a1000/rom_a8604.s | [src/non_matching/rom_a1000/80a8d34.c](../../src/non_matching/rom_a1000/80a8d34.c) |
| 172 |  | 192 | `OvlFunc_971_2008398` | overlays/ovl_30.s | [src/non_matching/ovl_7fb4a8/2008398.c](../../src/non_matching/ovl_7fb4a8/2008398.c) |
| 173 | 4 | 348 | `Anim_Torch` | rom_c9000/rom_e6638.s | [src/non_matching/rom_c9000/80e6638.c](../../src/non_matching/rom_c9000/80e6638.c) |
| 174 |  | 454 | `Func_80bfba4` | rom_b5000/rom_bbb0c.s | [src/non_matching/rom_b5000/80bfba4.c](../../src/non_matching/rom_b5000/80bfba4.c) |
| 176 |  | 207 | `OvlFunc_968_200cbd8` | overlays/ovl_30.s | [src/non_matching/ovl_7f2f14/200cbd8.c](../../src/non_matching/ovl_7f2f14/200cbd8.c) |
| 178 |  | 243 | `Func_80a7478` | rom_a1000/rom_a7380.s | [src/non_matching/rom_a1000/80a7478.c](../../src/non_matching/rom_a1000/80a7478.c) |
| 181 |  | 239 | `OvlFunc_933_2008e2c` | overlays/ovl_4e4.s | [src/non_matching/ovl_7bc690/2008e2c.c](../../src/non_matching/ovl_7bc690/2008e2c.c) |
| 182 |  | 206 | `Func_809c138` | rom_8a000/rom_9bb64.s | [src/non_matching/rom_8a000/809c138.c](../../src/non_matching/rom_8a000/809c138.c) |
| 183 |  | 215 | `Func_80a7850` | rom_a1000/rom_a7380.s | [src/non_matching/rom_a1000/80a7850.c](../../src/non_matching/rom_a1000/80a7850.c) |
| 184 |  | 217 | `Func_80912b8` | rom_8a000/rom_8d9a4.s | [src/non_matching/rom_8a000/80912b8.c](../../src/non_matching/rom_8a000/80912b8.c) |
| 184 |  | 198 | `OvlFunc_927_200a2c0` | overlays/ovl_30.s | [src/non_matching/ovl_7b4558/200a2c0.c](../../src/non_matching/ovl_7b4558/200a2c0.c) |
| 185 |  | 186 | `Anim_Unused_ScreenMelt` | rom_c9000/rom_cfef4.s | [src/non_matching/rom_c9000/Anim_Unused_ScreenMelt.c](../../src/non_matching/rom_c9000/Anim_Unused_ScreenMelt.c) |
| 185 |  | 363 | `Func_800b388` | rom_9000/rom_b074.s | [src/non_matching/rom_9000/800b388.c](../../src/non_matching/rom_9000/800b388.c) |
| 185 |  | 219 | `Func_80a8914` | rom_a1000/rom_a8604.s | [src/non_matching/rom_a1000/80a8914.c](../../src/non_matching/rom_a1000/80a8914.c) |
| 186 |  | 209 | `Func_8079f10` | rom_77000/rom_79460.s | [src/non_matching/rom_77000/8079f10.c](../../src/non_matching/rom_77000/8079f10.c) |
| 191 |  | 307 | `Func_80a60d4` | rom_a1000/rom_a5534.s | [src/non_matching/rom_a1000/80a60d4.c](../../src/non_matching/rom_a1000/80a60d4.c) |
| 193 |  | 213 | `Debug_LoadPresetParty` | rom_b5000/rom_b5368.s | [src/non_matching/rom_b5000/Debug_LoadPresetParty.c](../../src/non_matching/rom_b5000/Debug_LoadPresetParty.c) |
| 193 |  | 203 | `Func_80aa56c` | rom_a1000/rom_aa538.s | [src/non_matching/rom_a1000/80aa56c.c](../../src/non_matching/rom_a1000/80aa56c.c) |
| 193 |  | 222 | `OvlFunc_common1_1928` | overlays/common1.s | [src/non_matching/ovl_common/common1_1928.c](../../src/non_matching/ovl_common/common1_1928.c) |
| 196 | 1 | 251 | `LoadGS1TitleGFX` | rom_f2000/rom_f2028.s | [src/non_matching/rom_f2000/80f24a0_LoadGS1TitleGFX.c](../../src/non_matching/rom_f2000/80f24a0_LoadGS1TitleGFX.c) |
| 198 |  | 239 | `Func_8010230` | rom_9000/rom_f9cc.s | [src/non_matching/rom_9000/8010230.c](../../src/non_matching/rom_9000/8010230.c) |
| 199 |  | 213 | `Func_8095c08` | rom_8a000/rom_944ec.s | [src/non_matching/rom_8a000/8095c08.c](../../src/non_matching/rom_8a000/8095c08.c) |
| 199 |  | 362 | `Func_80a38d0` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a38d0.c](../../src/non_matching/rom_a1000/80a38d0.c) |
| 199 | 5 | 303 | `Anim_Unused_SkullCloud` | rom_c9000/rom_dbbdc.s | [src/non_matching/rom_c9000/dbbdc_SkullCloud.c](../../src/non_matching/rom_c9000/dbbdc_SkullCloud.c) |
| 201 |  | 226 | `Func_80a63e4` | rom_a1000/rom_a5534.s | [src/non_matching/rom_a1000/80a63e4.c](../../src/non_matching/rom_a1000/80a63e4.c) |
| 204 |  | 336 | `Anim_UndeadSword` | rom_c9000/rom_ece7c.s | [src/non_matching/rom_c9000/80ed104.c](../../src/non_matching/rom_c9000/80ed104.c) |
| 204 | 26 | 418 | `OvlFunc_971_20092e0` | overlays/ovl_30.s | [src/non_matching/ovl_7fb4a8/20092e0.c](../../src/non_matching/ovl_7fb4a8/20092e0.c) |
| 205 |  | 303 | `Func_80ba2c0` | rom_b5000/rom_b9b30.s | [src/non_matching/rom_b5000/80ba2c0.c](../../src/non_matching/rom_b5000/80ba2c0.c) |
| 207 | 2 | 280 | `Func_800c62c` | rom_9000/rom_c004.s | [src/non_matching/rom_9000/800c62c.c](../../src/non_matching/rom_9000/800c62c.c) |
| 210 |  | 238 | `Func_8094544` | rom_8a000/rom_944ec.s | [src/non_matching/rom_8a000/8094544.c](../../src/non_matching/rom_8a000/8094544.c) |
| 212 |  | 707 | `AdvanceMsgText` | rom_15000/rom_15e8c.s | [src/non_matching/rom_15000/80168f4.c](../../src/non_matching/rom_15000/80168f4.c) |
| 212 |  | 223 | `OvlFunc_common0_10c` | overlays/common0.s | [src/non_matching/ovl_common/common0_10c.c](../../src/non_matching/ovl_common/common0_10c.c) |
| 212 | 33 | 582 | `OvlFunc_881_2009ca4` | overlays/ovl_30.s | [src/non_matching/ovl_77a7c8/2009ca4.c](../../src/non_matching/ovl_77a7c8/2009ca4.c) |
| 215 |  | 303 | `Func_80c2724` | rom_b5000/rom_c1a34.s | [src/non_matching/rom_b5000/80c2724.c](../../src/non_matching/rom_b5000/80c2724.c) |
| 215 |  | 243 | `OvlFunc_883_200aa54` | overlays/ovl_30.s | [src/non_matching/ovl_780898/200aa54.c](../../src/non_matching/ovl_780898/200aa54.c) |
| 216 |  | 303 | `Func_80a90bc` | rom_a1000/rom_a8604.s | [src/non_matching/rom_a1000/80a90bc.c](../../src/non_matching/rom_a1000/80a90bc.c) |
| 218 | 11 | 295 | `OvlFunc_969_200bbc8` | overlays/ovl_314.s | [src/non_matching/ovl_7f6e64/200bbc8.c](../../src/non_matching/ovl_7f6e64/200bbc8.c) |
| 220 |  | 239 | `Func_8012b2c` | rom_9000/rom_1219c.s | [src/non_matching/rom_9000/8012b2c.c](../../src/non_matching/rom_9000/8012b2c.c) |
| 224 |  | 251 | `GameStart` | rom_8a000/rom_8a5f8.s | [src/non_matching/rom_8a000/808a8e4.c](../../src/non_matching/rom_8a000/808a8e4.c) |
| 226 |  | 588 | `Anim_Ground` | rom_c9000/rom_e0564.s | [src/non_matching/rom_c9000/Anim_Ground.c](../../src/non_matching/rom_c9000/Anim_Ground.c) |
| 227 |  | 307 | `Func_80ab314` | rom_a1000/rom_aa538.s | [src/non_matching/rom_a1000/80ab314.c](../../src/non_matching/rom_a1000/80ab314.c) |
| 228 |  | 280 | `Func_80b88d0` | rom_b5000/rom_b8228.s | [src/non_matching/rom_b5000/80b88d0.c](../../src/non_matching/rom_b5000/80b88d0.c) |
| 230 |  | 264 | `Func_80aafb8` | rom_a1000/rom_aa538.s | [src/non_matching/rom_a1000/80aafb8.c](../../src/non_matching/rom_a1000/80aafb8.c) |
| 232 |  | 263 | `OvlFunc_common1_5e4` | overlays/common1.s | [src/non_matching/ovl_common/common1_5e4.c](../../src/non_matching/ovl_common/common1_5e4.c) |
| 236 | 8 | 537 | `OvlFunc_955_2008bd4` | overlays/ovl_30.s | [src/non_matching/ovl_7ddb88/2008bd4.c](../../src/non_matching/ovl_7ddb88/2008bd4.c) |
| 237 |  | 259 | `Field_Catch` | rom_8a000/rom_9ad70.s | [src/non_matching/rom_8a000/809ad70.c](../../src/non_matching/rom_8a000/809ad70.c) |
| 241 |  | 279 | `BaseAnim_Tentacle` | rom_c9000/rom_cc5d8.s | [src/non_matching/rom_c9000/BaseAnim_Tentacle.c](../../src/non_matching/rom_c9000/BaseAnim_Tentacle.c) |
| 241 |  | 265 | `Func_80c24f0` | rom_b5000/rom_c1a34.s | [src/non_matching/rom_b5000/80c24f0.c](../../src/non_matching/rom_b5000/80c24f0.c) |
| 243 |  | 251 | `Anim_SpiderWeb` | rom_c9000/rom_cc5d8.s | [src/non_matching/rom_c9000/rom_ccebc.c](../../src/non_matching/rom_c9000/rom_ccebc.c) |
| 243 |  | 499 | `Func_809bcf8` | rom_8a000/rom_9bb64.s | [src/non_matching/rom_8a000/809bcf8.c](../../src/non_matching/rom_8a000/809bcf8.c) |
| 245 |  | 251 | `OvlFunc_933_2009638` | overlays/ovl_4e4.s | [src/non_matching/ovl_7bc690/2009638.c](../../src/non_matching/ovl_7bc690/2009638.c) |
| 245 |  | 260 | `UpdateSprite` | rom_9000/rom_b074.s | [src/non_matching/rom_9000/800b168.c](../../src/non_matching/rom_9000/800b168.c) |
| 246 | 1 | 272 | `Func_8078bf0` | rom_77000/rom_78b9c.s | [src/non_matching/rom_77000/8078bf0.c](../../src/non_matching/rom_77000/8078bf0.c) |
| 249 |  | 432 | `Anim_Spire` | rom_c9000/rom_e0564.s | [src/non_matching/rom_c9000/Anim_Spire.c](../../src/non_matching/rom_c9000/Anim_Spire.c) |
| 250 | 8 | 468 | `Func_80c02a4` | rom_b5000/rom_bffb8.s | [src/non_matching/rom_b5000/80c02a4.c](../../src/non_matching/rom_b5000/80c02a4.c) |
| 251 |  | 274 | `Func_801be80` | rom_15000/rom_1aeec.s | [src/non_matching/rom_15000/801be80.c](../../src/non_matching/rom_15000/801be80.c) |
| 251 |  | 276 | `Func_808e9c0` | rom_8a000/rom_8d9a4.s | [src/non_matching/rom_8a000/808e9c0.c](../../src/non_matching/rom_8a000/808e9c0.c) |
| 251 |  | 281 | `LoadMapActors` | rom_8a000/rom_8ace0.s | [src/non_matching/rom_8a000/808b3ec.c](../../src/non_matching/rom_8a000/808b3ec.c) |
| 252 |  | 266 | `UpdateFieldScreen` | rom_9000/rom_f9cc.s | [src/non_matching/rom_9000/8010000.c](../../src/non_matching/rom_9000/8010000.c) |
| 256 | 3 | 306 | `OvlFunc_966_200920c` | overlays/ovl_30.s | [src/non_matching/ovl_7f148c/200920c.c](../../src/non_matching/ovl_7f148c/200920c.c) |
| 260 | 3 | 282 | `CreateBattleSpriteOverlays` | rom_b5000/rom_b7410.s | [src/non_matching/rom_b5000/CreateBattleSpriteOverlays.c](../../src/non_matching/rom_b5000/CreateBattleSpriteOverlays.c) |
| 261 |  | 358 | `Func_80a7a34` | rom_a1000/rom_a7380.s | [src/non_matching/rom_a1000/80a7a34.c](../../src/non_matching/rom_a1000/80a7a34.c) |
| 264 | 7 | 277 | `Func_80c11ec` | rom_b5000/rom_c10e8.s | [src/non_matching/rom_b5000/80c11ec.c](../../src/non_matching/rom_b5000/80c11ec.c) |
| 268 | 2 | 295 | `OvlFunc_897_20090c4` | overlays/ovl_30.s | [src/non_matching/ovl_791794/20090c4.c](../../src/non_matching/ovl_791794/20090c4.c) |
| 270 | 6 | 316 | `OvlFunc_969_200d6a0` | overlays/ovl_314.s | [src/non_matching/ovl_7f6e64/200d6a0.c](../../src/non_matching/ovl_7f6e64/200d6a0.c) |
| 271 |  | 272 | `Func_800655c` | rom_c0/rom_5cf8.s | [src/non_matching/rom_c0/800655c.c](../../src/non_matching/rom_c0/800655c.c) |
| 271 |  | 290 | `Func_80f07f0` | rom_f0000/rom_f0254.s | [src/non_matching/rom_f0000/80f07f0.c](../../src/non_matching/rom_f0000/80f07f0.c) |
| 275 |  | 344 | `Func_80a5cc0` | rom_a1000/rom_a5534.s | [src/non_matching/rom_a1000/80a5cc0.c](../../src/non_matching/rom_a1000/80a5cc0.c) |
| 276 | 169 | 1082 | `OvlFunc_884_20097c8` | overlays/ovl_30.s | [src/non_matching/ovl_784360/20097c8.c](../../src/non_matching/ovl_784360/20097c8.c) |
| 282 |  | 301 | `GetVenusDjinni` | rom_8a000/rom_944ec.s | [src/non_matching/rom_8a000/8096140.c](../../src/non_matching/rom_8a000/8096140.c) |
| 284 |  | 365 | `InitWorldMap` | rom_9000/rom_108e4.s | [src/non_matching/rom_9000/InitWorldMap.c](../../src/non_matching/rom_9000/InitWorldMap.c) |
| 286 | 10 | 464 | `BaseAnim_HauntAttack` | rom_c9000/rom_ceb30.s | [src/non_matching/rom_c9000/ceb30_HauntAttack.c](../../src/non_matching/rom_c9000/ceb30_HauntAttack.c) |
| 287 | 9 | 398 | `BaseAnim_Spore` | rom_c9000/rom_ca1e4.s | [src/non_matching/rom_c9000/ca1e4_Spore.c](../../src/non_matching/rom_c9000/ca1e4_Spore.c) |
| 290 |  | 379 | `Func_800d340` | rom_9000/rom_ca6c.s | [src/non_matching/rom_9000/800d340.c](../../src/non_matching/rom_9000/800d340.c) |
| 294 |  | 495 | `Anim_Ray` | rom_c9000/rom_d9ab8.s | [src/non_matching/rom_c9000/Anim_Ray.c](../../src/non_matching/rom_c9000/Anim_Ray.c) |
| 295 |  | 429 | `Anim_Mercury` | rom_c9000/rom_dfa18.s | [src/non_matching/rom_c9000/Anim_Mercury.c](../../src/non_matching/rom_c9000/Anim_Mercury.c) |
| 295 |  | 341 | `Func_80a8604` | rom_a1000/rom_a8604.s | [src/non_matching/rom_a1000/80a8604.c](../../src/non_matching/rom_a1000/80a8604.c) |
| 296 |  | 584 | `Anim_Ragnarok` | rom_c9000/rom_e7320.s | [src/non_matching/rom_c9000/Anim_Ragnarok.c](../../src/non_matching/rom_c9000/Anim_Ragnarok.c) |
| 299 |  | 360 | `OvlFunc_897_200b01c` | overlays/ovl_30.s | [src/non_matching/ovl_791794/200b01c.c](../../src/non_matching/ovl_791794/200b01c.c) |
| 300 | 6 | 412 | `Anim_HelmSplitter` | rom_c9000/rom_e6638.s | [src/non_matching/rom_c9000/e6638_HelmSplitter.c](../../src/non_matching/rom_c9000/e6638_HelmSplitter.c) |
| 301 |  | 397 | `Func_8092c40` | rom_8a000/rom_92950.s | [src/non_matching/rom_8a000/8092c40.c](../../src/non_matching/rom_8a000/8092c40.c) |
| 301 | 2 | 377 | `LoadMapData` | rom_9000/rom_f9cc.s | [src/non_matching/rom_9000/LoadMapData.c](../../src/non_matching/rom_9000/LoadMapData.c) |
| 308 | 1 | 384 | `Anim_DjinnSet` | rom_c9000/rom_cc5d8.s | [src/non_matching/rom_c9000/cc5d8_DjinnSet.c](../../src/non_matching/rom_c9000/cc5d8_DjinnSet.c) |
| 309 | 1 | 510 | `Func_80191cc` | rom_15000/rom_1908c.s | [src/non_matching/rom_15000/80191cc.c](../../src/non_matching/rom_15000/80191cc.c) |
| 311 |  | 347 | `Anim_Unused_SabreRain` | rom_c9000/rom_cb1a4.s | [src/non_matching/rom_c9000/rom_cb4ec.c](../../src/non_matching/rom_c9000/rom_cb4ec.c) |
| 313 | 3 | 578 | `ActorCmd_Player_World` | rom_9000/rom_ebec.s | [src/non_matching/rom_9000/800f2f8.c](../../src/non_matching/rom_9000/800f2f8.c) |
| 315 |  | 337 | `Debug_PaletteEditor` | rom_8a000/rom_8ba38.s | [src/non_matching/rom_8a000/808d0c8.c](../../src/non_matching/rom_8a000/808d0c8.c) |
| 316 | 2 | 432 | `Anim_ShiningStar` | rom_c9000/rom_cfef4.s | [src/non_matching/rom_c9000/cfef4_ShiningStar.c](../../src/non_matching/rom_c9000/cfef4_ShiningStar.c) |
| 317 |  | 430 | `Func_80b9ec0` | rom_b5000/rom_b9b30.s | [src/non_matching/rom_b5000/80b9ec0.c](../../src/non_matching/rom_b5000/80b9ec0.c) |
| 320 | 461 | 1378 | `OvlFunc_897_2008054` | overlays/ovl_30.s | [src/non_matching/ovl_791794/2008054.c](../../src/non_matching/ovl_791794/2008054.c) |
| 325 |  | 745 | `BufferString` | rom_15000/rom_17e88.s | [src/non_matching/rom_15000/BufferString.c](../../src/non_matching/rom_15000/BufferString.c) |
| 326 |  | 368 | `Field_Move_Target` | rom_8a000/rom_97b54.s | [src/non_matching/rom_8a000/8097c3c.c](../../src/non_matching/rom_8a000/8097c3c.c) |
| 330 |  | 365 | `Field_Douse` | rom_8a000/rom_97b54.s | [src/non_matching/rom_8a000/Field_Douse.c](../../src/non_matching/rom_8a000/Field_Douse.c) |
| 337 |  | 416 | `Func_801d108` | rom_15000/rom_1ca1c.s | [src/non_matching/rom_15000/801d108.c](../../src/non_matching/rom_15000/801d108.c) |
| 337 |  | 360 | `OvlFunc_896_200c49c` | overlays/ovl_314.s | [src/non_matching/ovl_78ef88/200c49c.c](../../src/non_matching/ovl_78ef88/200c49c.c) |
| 338 |  | 401 | `Func_8028194` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/8028194.c](../../src/non_matching/rom_15000/8028194.c) |
| 343 | 8 | 392 | `Anim_Haunt` | rom_c9000/rom_cd508.s | [src/non_matching/rom_c9000/cd508_Haunt.c](../../src/non_matching/rom_c9000/cd508_Haunt.c) |
| 343 | 10 | 452 | `Anim_EPowerUp` | rom_c9000/rom_cb1a4.s | [src/non_matching/rom_c9000/cb1a4_EPowerUp.c](../../src/non_matching/rom_c9000/cb1a4_EPowerUp.c) |
| 347 |  | 363 | `Func_80a414c` | rom_a1000/rom_a1814.s | [src/non_matching/rom_a1000/80a414c.c](../../src/non_matching/rom_a1000/80a414c.c) |
| 348 |  | 476 | `Anim_Thorn` | rom_c9000/rom_dd2ac.s | [src/non_matching/rom_c9000/Anim_Thorn.c](../../src/non_matching/rom_c9000/Anim_Thorn.c) |
| 351 |  | 456 | `Func_801de5c` | rom_15000/rom_1de5c.s | [src/non_matching/rom_15000/801de5c.c](../../src/non_matching/rom_15000/801de5c.c) |
| 351 |  | 393 | `OvlFunc_933_20098a4` | overlays/ovl_18a4.s | [src/non_matching/ovl_7bc690/20098a4.c](../../src/non_matching/ovl_7bc690/20098a4.c) |
| 352 |  | 371 | `Field_Force` | rom_8a000/rom_97b54.s | [src/non_matching/rom_8a000/Field_Force.c](../../src/non_matching/rom_8a000/Field_Force.c) |
| 353 |  | 538 | `Anim_PlanetDiver` | rom_c9000/rom_cd508.s | [src/non_matching/rom_c9000/Anim_PlanetDiver.c](../../src/non_matching/rom_c9000/Anim_PlanetDiver.c) |
| 363 |  | 493 | `StartTitleScreen` | rom_f2000/rom_f2028.s | [src/non_matching/rom_f2000/StartTitleScreen.c](../../src/non_matching/rom_f2000/StartTitleScreen.c) |
| 367 |  | 619 | `Anim_Ice` | rom_c9000/rom_c91dc.s | [src/non_matching/rom_c9000/Anim_Ice.c](../../src/non_matching/rom_c9000/Anim_Ice.c) |
| 368 |  | 393 | `OvlFunc_957_200909c` | overlays/ovl_30.s | [src/non_matching/ovl_7e3e08/200909c.c](../../src/non_matching/ovl_7e3e08/200909c.c) |
| 372 |  | 424 | `Func_808d9a4` | rom_8a000/rom_8d9a4.s | [src/non_matching/rom_8a000/808d9a4.c](../../src/non_matching/rom_8a000/808d9a4.c) |
| 378 |  | 493 | `Anim_Plasma` | rom_c9000/rom_d2d98.s | [src/non_matching/rom_c9000/Anim_Plasma.c](../../src/non_matching/rom_c9000/Anim_Plasma.c) |
| 380 | 2 | 541 | `BaseAnim_Growth` | rom_c9000/rom_dd2ac.s | [src/non_matching/rom_c9000/80dd2c4.c](../../src/non_matching/rom_c9000/80dd2c4.c) |
| 383 | 2 | 459 | `Anim_PsyphonSeal` | rom_c9000/rom_cfef4.s | [src/non_matching/rom_c9000/Anim_PsyphonSeal.c](../../src/non_matching/rom_c9000/Anim_PsyphonSeal.c) |
| 384 |  | 602 | `Anim_Douse` | rom_c9000/rom_c91dc.s | [src/non_matching/rom_c9000/Anim_Douse.c](../../src/non_matching/rom_c9000/Anim_Douse.c) |
| 388 |  | 425 | `OvlFunc_957_200b644` | overlays/ovl_30.s | [src/non_matching/ovl_7e3e08/200b644.c](../../src/non_matching/ovl_7e3e08/200b644.c) |
| 390 | 98 | 497 | `OvlFunc_932_2008ec0` | overlays/ovl_30.s | [src/non_matching/ovl_7b9cb4/2008ec0.c](../../src/non_matching/ovl_7b9cb4/2008ec0.c) |
| 395 |  | 461 | `Func_80ae2f4` | rom_a1000/rom_ad274.s | [src/non_matching/rom_a1000/80ae2f4.c](../../src/non_matching/rom_a1000/80ae2f4.c) |
| 404 |  | 447 | `Func_801f200` | rom_15000/rom_1de5c.s | [src/non_matching/rom_15000/801f200.c](../../src/non_matching/rom_15000/801f200.c) |
| 415 |  | 432 | `Anim_Unsummon` | rom_c9000/rom_e6638.s | [src/non_matching/rom_c9000/Anim_Unsummon.c](../../src/non_matching/rom_c9000/Anim_Unsummon.c) |
| 419 |  | 414 | `Func_80c1ffc` | rom_b5000/rom_c1a34.s | [src/non_matching/rom_b5000/80c1ffc.c](../../src/non_matching/rom_b5000/80c1ffc.c) |
| 422 |  | 440 | `Func_80b6f44` | rom_b5000/rom_b6f44.s | [src/non_matching/rom_b5000/80b6f44.c](../../src/non_matching/rom_b5000/80b6f44.c) |
| 424 | 52 | 592 | `OvlFunc_882_2008434` | overlays/ovl_30.s | [src/non_matching/ovl_77dd1c/2008434.c](../../src/non_matching/ovl_77dd1c/2008434.c) |
| 442 |  | 454 | `OvlFunc_common1_1b08` | overlays/common1.s | [src/non_matching/ovl_common/common1_1b08.c](../../src/non_matching/ovl_common/common1_1b08.c) |
| 444 |  | 500 | `Menu_Settings` | rom_15000/rom_1ca1c.s | [src/non_matching/rom_15000/Menu_Settings.c](../../src/non_matching/rom_15000/Menu_Settings.c) |
| 448 |  | 484 | `Anim_Quake` | rom_c9000/rom_d9ab8.s | [src/non_matching/rom_c9000/Anim_Quake.c](../../src/non_matching/rom_c9000/Anim_Quake.c) |
| 450 |  | 459 | `Anim_Annihilation` | rom_c9000/rom_e7320.s | [src/non_matching/rom_c9000/Anim_Annihilation.c](../../src/non_matching/rom_c9000/Anim_Annihilation.c) |
| 463 |  | 532 | `Func_80f2028` | rom_f2000/rom_f2028.s | [src/non_matching/rom_f2000/80f2028.c](../../src/non_matching/rom_f2000/80f2028.c) |
| 464 |  | 496 | `OvlFunc_880_2008de4` | overlays/ovl_30.s | [src/non_matching/ovl_7795e8/2008de4.c](../../src/non_matching/ovl_7795e8/2008de4.c) |
| 473 |  | 536 | `OvlFunc_890_2008488` | overlays/ovl_30.s | [src/non_matching/ovl_78b2ac/2008488.c](../../src/non_matching/ovl_78b2ac/2008488.c) |
| 475 |  | 509 | `Anim_AstralBlast` | rom_c9000/rom_cfef4.s | [src/non_matching/rom_c9000/Anim_AstralBlast.c](../../src/non_matching/rom_c9000/Anim_AstralBlast.c) |
| 475 |  | 514 | `Anim_Prism` | rom_c9000/rom_d2d98.s | [src/non_matching/rom_c9000/Anim_Prism.c](../../src/non_matching/rom_c9000/Anim_Prism.c) |
| 484 |  | 550 | `Anim_Bind` | rom_c9000/rom_cfef4.s | [src/non_matching/rom_c9000/Anim_Bind.c](../../src/non_matching/rom_c9000/Anim_Bind.c) |
| 488 |  | 525 | `UI_NameEntry` | rom_15000/rom_20198.s | [src/non_matching/rom_15000/8020bd8.c](../../src/non_matching/rom_15000/8020bd8.c) |
| 500 |  | 549 | `Field_Carry_Target` | rom_8a000/rom_97b54.s | [src/non_matching/rom_8a000/Field_Carry_Target.c](../../src/non_matching/rom_8a000/Field_Carry_Target.c) |
| 502 | 3 | 562 | `BaseAnim_StatDown` | rom_c9000/rom_d9ab8.s | [src/non_matching/rom_c9000/d9ab8_StatDown.c](../../src/non_matching/rom_c9000/d9ab8_StatDown.c) |
| 510 |  | 587 | `Anim_Bolt` | rom_c9000/rom_dd2ac.s | [src/non_matching/rom_c9000/Anim_Bolt.c](../../src/non_matching/rom_c9000/Anim_Bolt.c) |
| 527 |  | 589 | `Func_80aa768` | rom_a1000/rom_aa538.s | [src/non_matching/rom_a1000/80aa768.c](../../src/non_matching/rom_a1000/80aa768.c) |
| 530 |  | 550 | `Func_80b0aac` | rom_b0000/rom_b0070.s | [src/non_matching/rom_b0000/80b0aac.c](../../src/non_matching/rom_b0000/80b0aac.c) |
| 555 |  | 623 | `Anim_Condemn` | rom_c9000/rom_cfef4.s | [src/non_matching/rom_c9000/Anim_Condemn.c](../../src/non_matching/rom_c9000/Anim_Condemn.c) |
| 560 |  | 624 | `Anim_Volcano` | rom_c9000/rom_d45ec.s | [src/non_matching/rom_c9000/Anim_Volcano.c](../../src/non_matching/rom_c9000/Anim_Volcano.c) |
| 565 |  | 607 | `BaseAnim_SonicWave` | rom_c9000/rom_c91dc.s | [src/non_matching/rom_c9000/80c9ca8.c](../../src/non_matching/rom_c9000/80c9ca8.c) |
| 575 |  | 645 | `Func_801a98c` | rom_15000/rom_1a66c.s | [src/non_matching/rom_15000/801a98c.c](../../src/non_matching/rom_15000/801a98c.c) |
| 587 | 10 | 668 | `BaseAnim_Revive` | rom_c9000/rom_cf2a0.s | [src/non_matching/rom_c9000/cf2a0_Revive.c](../../src/non_matching/rom_c9000/cf2a0_Revive.c) |
| 589 |  | 674 | `UpdateSpriteAnim` | rom_9000/rom_a97c.s | [src/non_matching/rom_8a000/UpdateSpriteAnim.c](../../src/non_matching/rom_8a000/UpdateSpriteAnim.c) |
| 616 | 88 | 677 | `OvlFunc_969_200c23c` | overlays/ovl_314.s | [src/non_matching/ovl_7f6e64/200c23c.c](../../src/non_matching/ovl_7f6e64/200c23c.c) |
| 618 |  | 684 | `BattleMain` | rom_b5000/rom_b5a0c.s | [src/non_matching/rom_b5000/80b63c8.c](../../src/non_matching/rom_b5000/80b63c8.c) |
| 619 |  | 707 | `Anim_Nereid` | rom_c9000/rom_d2d98.s | [src/non_matching/rom_c9000/Anim_Nereid.c](../../src/non_matching/rom_c9000/Anim_Nereid.c) |
| 647 |  | 660 | `BaseAnim_Breath` | rom_c9000/rom_dbbdc.s | [src/non_matching/rom_c9000/dbc30_Breath.c](../../src/non_matching/rom_c9000/dbc30_Breath.c) |
| 653 |  | 688 | `BaseAnim_Attack` | rom_c9000/rom_e3958.s | [src/non_matching/rom_c9000/e3aa0_Attack.c](../../src/non_matching/rom_c9000/e3aa0_Attack.c) |
| 666 | 6 | 703 | `BaseAnim_Blob` | rom_c9000/rom_cf88c.s | [src/non_matching/rom_c9000/cf8e0_Blob.c](../../src/non_matching/rom_c9000/cf8e0_Blob.c) |
| 674 |  | 809 | `BaseAnim_Nova` | rom_c9000/rom_d45ec.s | [src/non_matching/rom_c9000/d4604_Nova.c](../../src/non_matching/rom_c9000/d4604_Nova.c) |
| 682 |  | 782 | `Anim_DragonCloud` | rom_c9000/rom_e7320.s | [src/non_matching/rom_c9000/Anim_DragonCloud.c](../../src/non_matching/rom_c9000/Anim_DragonCloud.c) |
| 696 |  | 840 | `OvlFunc_945_2009f3c` | overlays/ovl_30.s | [src/non_matching/ovl_7cb2c0/2009f3c.c](../../src/non_matching/ovl_7cb2c0/2009f3c.c) |
| 712 |  | 817 | `Func_8025200` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/8025200.c](../../src/non_matching/rom_15000/8025200.c) |
| 718 | 4 | 781 | `BuildDraw2DFuncEx` | rom_c9000/rom_ed408.s | [src/non_matching/rom_c9000/BuildDraw2DFuncEx.c](../../src/non_matching/rom_c9000/BuildDraw2DFuncEx.c) |
| 724 | 30 | 962 | `OvlFunc_969_20088b4` | overlays/ovl_314.s | [src/non_matching/ovl_7f6e64/20088b4.c](../../src/non_matching/ovl_7f6e64/20088b4.c) |
| 731 |  | 816 | `Anim_TitanBlade` | rom_c9000/rom_e7320.s | [src/non_matching/rom_c9000/e99c0_TitanBlade.c](../../src/non_matching/rom_c9000/e99c0_TitanBlade.c) |
| 736 |  | 774 | `Func_80a6ccc` | rom_a1000/rom_a5534.s | [src/non_matching/rom_a1000/80a6ccc.c](../../src/non_matching/rom_a1000/80a6ccc.c) |
| 737 |  | 838 | `Func_80bd898` | rom_b5000/rom_bbb0c.s | [src/non_matching/rom_b5000/80bd898.c](../../src/non_matching/rom_b5000/80bd898.c) |
| 739 |  | 791 | `BaseAnim_ParticleCloud` | rom_c9000/rom_d5258.s | [src/non_matching/rom_c9000/d52c8_ParticleCloud.c](../../src/non_matching/rom_c9000/d52c8_ParticleCloud.c) |
| 740 |  | 778 | `BaseAnim_RapidSlash` | rom_c9000/rom_e28f4.s | [src/non_matching/rom_c9000/e2974_RapidSlash.c](../../src/non_matching/rom_c9000/e2974_RapidSlash.c) |
| 753 |  | 835 | `OvlFunc_959_200a7b0` | overlays/ovl_9dc.s | [src/non_matching/ovl_7e7574/200a7b0.c](../../src/non_matching/ovl_7e7574/200a7b0.c) |
| 756 |  | 875 | `Anim_Ramses` | rom_c9000/rom_e7320.s | [src/non_matching/rom_c9000/Anim_Ramses.c](../../src/non_matching/rom_c9000/Anim_Ramses.c) |
| 788 |  | 815 | `Anim_Frost` | rom_c9000/rom_d9ab8.s | [src/non_matching/rom_c9000/Anim_Frost.c](../../src/non_matching/rom_c9000/Anim_Frost.c) |
| 789 |  | 830 | `Func_80bae40` | rom_b5000/rom_b9b30.s | [src/non_matching/rom_b5000/80bae40.c](../../src/non_matching/rom_b5000/80bae40.c) |
| 795 |  | 838 | `Func_802592c` | rom_15000/rom_23178.s | [src/non_matching/rom_15000/802592c.c](../../src/non_matching/rom_15000/802592c.c) |
| 795 | 3 | 833 | `ActorCmd_Player` | rom_9000/rom_ebec.s | [src/non_matching/rom_9000/800ebec.c](../../src/non_matching/rom_9000/800ebec.c) |
| 805 | 6 | 839 | `Func_80f3078` | rom_f2000/rom_f2028.s | [src/non_matching/rom_f2000/80f3078.c](../../src/non_matching/rom_f2000/80f3078.c) |
| 815 | 2 | 849 | `Func_8090a5c` | rom_8a000/rom_8d9a4.s | [src/non_matching/rom_8a000/Func_8090a5c.c](../../src/non_matching/rom_8a000/Func_8090a5c.c) |
| 819 |  | 888 | `OvlFunc_881_2008c28` | overlays/ovl_30.s | [src/non_matching/ovl_77a7c8/2008c28.c](../../src/non_matching/ovl_77a7c8/2008c28.c) |
| 824 |  | 915 | `BaseAnim_Heal` | rom_c9000/rom_d8948.s | [src/non_matching/rom_c9000/d89ac_Heal.c](../../src/non_matching/rom_c9000/d89ac_Heal.c) |
| 834 |  | 927 | `CalcStats'` | rom_77000/rom_77320.s | [src/non_matching/rom_77000/CalcStats.c](../../src/non_matching/rom_77000/CalcStats.c) |
| 844 |  | 1007 | `BaseAnim_StatUp` | rom_c9000/rom_d9194.s | [src/non_matching/rom_c9000/d91dc_StatUp.c](../../src/non_matching/rom_c9000/d91dc_StatUp.c) |
| 907 | 1 | 1071 | `OvlFunc_882_200b1ac` | overlays/ovl_30.s | [src/non_matching/ovl_77dd1c/200b1ac.c](../../src/non_matching/ovl_77dd1c/200b1ac.c) |
| 918 |  | 971 | `OvlFunc_951_2008e5c` | overlays/ovl_30.s | [src/non_matching/ovl_7d6418/2008e5c.c](../../src/non_matching/ovl_7d6418/2008e5c.c) |
| 921 |  | 990 | `Anim_Gaia` | rom_c9000/rom_e28f4.s | [src/non_matching/rom_c9000/Anim_Gaia.c](../../src/non_matching/rom_c9000/Anim_Gaia.c) |
| 923 |  | 1029 | `OvlFunc_886_2008658` | overlays/ovl_30.s | [src/non_matching/ovl_786f0c/2008658.c](../../src/non_matching/ovl_786f0c/2008658.c) |
| 960 |  | 1027 | `OvlFunc_883_200b4c8` | overlays/ovl_30.s | [src/non_matching/overlays/200b4c8.c](../../src/non_matching/overlays/200b4c8.c) |
| 960 |  | 1027 | `OvlFunc_883_200bfb0` | overlays/ovl_30.s | [src/non_matching/ovl_780898/200bfb0.c](../../src/non_matching/ovl_780898/200bfb0.c) |
| 977 |  | 1062 | `OvlFunc_964_200a59c` | overlays/ovl_30.s | [src/non_matching/ovl_7ed0a0/200a59c.c](../../src/non_matching/ovl_7ed0a0/200a59c.c) |
| 998 | 2 | 1586 | `OvlFunc_968_200b068` | overlays/ovl_30.s | [src/non_matching/ovl_7f2f14/200b068.c](../../src/non_matching/ovl_7f2f14/200b068.c) |
| 1026 |  | 1039 | `Anim_Atalanta` | rom_c9000/rom_dbbdc.s | [src/non_matching/rom_c9000/rom_dc968.c](../../src/non_matching/rom_c9000/rom_dc968.c) |
| 1042 |  | 1132 | `Anim_ScreenShatter` | rom_c9000/rom_cb1a4.s | [src/non_matching/rom_c9000/Anim_ScreenShatter.c](../../src/non_matching/rom_c9000/Anim_ScreenShatter.c) |
| 1055 |  | 1164 | `OvlFunc_917_2008488` | overlays/ovl_30.s | [src/non_matching/ovl_7a4370/2008488.c](../../src/non_matching/ovl_7a4370/2008488.c) |
| 1059 |  | 1095 | `Anim_Kirin` | rom_c9000/rom_eb754.s | [src/non_matching/rom_c9000/Anim_Kirin.c](../../src/non_matching/rom_c9000/Anim_Kirin.c) |
| 1100 | 428 | 1486 | `OvlFunc_899_200b6f8` | overlays/ovl_30.s | [src/non_matching/ovl_794ac0/200b6f8.c](../../src/non_matching/ovl_794ac0/200b6f8.c) |
| 1263 |  | 1416 | `Anim_Boreas` | rom_c9000/rom_d6970.s | [src/non_matching/rom_c9000/Anim_Boreas.c](../../src/non_matching/rom_c9000/Anim_Boreas.c) |
| 1452 |  | 1585 | `OvlFunc_936_2008590` | overlays/ovl_30.s | [src/non_matching/ovl_7c097c/2008590.c](../../src/non_matching/ovl_7c097c/2008590.c) |
| 1885 |  | 2061 | `OvlFunc_883_20095dc` | overlays/ovl_30.s | [src/non_matching/ovl_780898/20095dc.c](../../src/non_matching/ovl_780898/20095dc.c) |
| 1934 | 796 | 2364 | `OvlFunc_957_20093f8` | overlays/ovl_30.s | [src/non_matching/ovl_7e3e08/20093f8.c](../../src/non_matching/ovl_7e3e08/20093f8.c) |
| 3080 |  | 3380 | `BaseAnim_SpecialAttack` | rom_c9000/rom_e47b8.s | [src/non_matching/rom_c9000/e47b8_SpecialAttack.c](../../src/non_matching/rom_c9000/e47b8_SpecialAttack.c) |

## No parseable figure (153)

Prose, triage and class parks, plus any whose claim phrasing `parkcheck` cannot read.

| function | park |
|---|---|
| `?` | [src/non_matching/overlays/200807c.c](../../src/non_matching/overlays/200807c.c) |
| `?` | [src/non_matching/overlays/20082b4.c](../../src/non_matching/overlays/20082b4.c) |
| `?` | [src/non_matching/overlays/20082f0.c](../../src/non_matching/overlays/20082f0.c) |
| `?` | [src/non_matching/overlays/2008308.c](../../src/non_matching/overlays/2008308.c) |
| `?` | [src/non_matching/overlays/20083d0.c](../../src/non_matching/overlays/20083d0.c) |
| `?` | [src/non_matching/overlays/200858c.c](../../src/non_matching/overlays/200858c.c) |
| `?` | [src/non_matching/overlays/2008950.c](../../src/non_matching/overlays/2008950.c) |
| `?` | [src/non_matching/overlays/2008dc8.c](../../src/non_matching/overlays/2008dc8.c) |
| `?` | [src/non_matching/overlays/2008f8c.c](../../src/non_matching/overlays/2008f8c.c) |
| `?` | [src/non_matching/overlays/2008fc8.c](../../src/non_matching/overlays/2008fc8.c) |
| `?` | [src/non_matching/overlays/2009090.c](../../src/non_matching/overlays/2009090.c) |
| `?` | [src/non_matching/overlays/20090c0.c](../../src/non_matching/overlays/20090c0.c) |
| `?` | [src/non_matching/overlays/20091c4.c](../../src/non_matching/overlays/20091c4.c) |
| `?` | [src/non_matching/overlays/20097a4.c](../../src/non_matching/overlays/20097a4.c) |
| `?` | [src/non_matching/overlays/2009818.c](../../src/non_matching/overlays/2009818.c) |
| `OvlFunc_932_200b668` | [src/non_matching/overlays/200b668.c](../../src/non_matching/overlays/200b668.c) |
| `?` | [src/non_matching/overlays/200b6ac.c](../../src/non_matching/overlays/200b6ac.c) |
| `?` | [src/non_matching/overlays/200bd40.c](../../src/non_matching/overlays/200bd40.c) |
| `?` | [src/non_matching/overlays/200c78c.c](../../src/non_matching/overlays/200c78c.c) |
| `?` | [src/non_matching/overlays/common1_78.c](../../src/non_matching/overlays/common1_78.c) |
| `OvlFunc_880_20083cc` | [src/non_matching/ovl_7795e8/20083cc.c](../../src/non_matching/ovl_7795e8/20083cc.c) |
| `?` | [src/non_matching/ovl_77a7c8/20081c4.c](../../src/non_matching/ovl_77a7c8/20081c4.c) |
| `?` | [src/non_matching/ovl_77a7c8/2009a98.c](../../src/non_matching/ovl_77a7c8/2009a98.c) |
| `?` | [src/non_matching/ovl_77a7c8/200a4a8.c](../../src/non_matching/ovl_77a7c8/200a4a8.c) |
| `?` | [src/non_matching/ovl_77a7c8/200b57c.c](../../src/non_matching/ovl_77a7c8/200b57c.c) |
| `?` | [src/non_matching/ovl_780898/2009490.c](../../src/non_matching/ovl_780898/2009490.c) |
| `?` | [src/non_matching/ovl_78603c/200950c.c](../../src/non_matching/ovl_78603c/200950c.c) |
| `?` | [src/non_matching/ovl_7892c8/200874c.c](../../src/non_matching/ovl_7892c8/200874c.c) |
| `OvlFunc_888_200888c` | [src/non_matching/ovl_7892c8/200888c.c](../../src/non_matching/ovl_7892c8/200888c.c) |
| `?` | [src/non_matching/ovl_78b2ac/200a614.c](../../src/non_matching/ovl_78b2ac/200a614.c) |
| `?` | [src/non_matching/ovl_78dee8/2008154.c](../../src/non_matching/ovl_78dee8/2008154.c) |
| `OvlFunc_896_200a7f8` | [src/non_matching/ovl_78ef88/200a7f8.c](../../src/non_matching/ovl_78ef88/200a7f8.c) |
| `?` | [src/non_matching/ovl_791794/2009410.c](../../src/non_matching/ovl_791794/2009410.c) |
| `?` | [src/non_matching/ovl_791794/200a84c.c](../../src/non_matching/ovl_791794/200a84c.c) |
| `?` | [src/non_matching/ovl_793768/2009754.c](../../src/non_matching/ovl_793768/2009754.c) |
| `?` | [src/non_matching/ovl_794ac0/200c754.c](../../src/non_matching/ovl_794ac0/200c754.c) |
| `OvlFunc_898_2009090` | [src/non_matching/ovl_797990/2008c1c.c](../../src/non_matching/ovl_797990/2008c1c.c) |
| `?` | [src/non_matching/ovl_79b154/2008328.c](../../src/non_matching/ovl_79b154/2008328.c) |
| `?` | [src/non_matching/ovl_79e5c0/2008694.c](../../src/non_matching/ovl_79e5c0/2008694.c) |
| `?` | [src/non_matching/ovl_79e5c0/20088ec.c](../../src/non_matching/ovl_79e5c0/20088ec.c) |
| `OvlFunc_913_2008d3c` | [src/non_matching/ovl_7a04ac/2008d3c.c](../../src/non_matching/ovl_7a04ac/2008d3c.c) |
| `?` | [src/non_matching/ovl_7a2bf0/20089f8.c](../../src/non_matching/ovl_7a2bf0/20089f8.c) |
| `?` | [src/non_matching/ovl_7a5214/20097ec.c](../../src/non_matching/ovl_7a5214/20097ec.c) |
| `?` | [src/non_matching/ovl_7a6ae4/20087f8.c](../../src/non_matching/ovl_7a6ae4/20087f8.c) |
| `OvlFunc_924_200bd20` | [src/non_matching/ovl_7ac2d8/200bd20.c](../../src/non_matching/ovl_7ac2d8/200bd20.c) |
| `?` | [src/non_matching/ovl_7b2078/2008afc.c](../../src/non_matching/ovl_7b2078/2008afc.c) |
| `?` | [src/non_matching/ovl_7b4558/2009d04.c](../../src/non_matching/ovl_7b4558/2009d04.c) |
| `?` | [src/non_matching/ovl_7b8cb0/2008b2c.c](../../src/non_matching/ovl_7b8cb0/2008b2c.c) |
| `?` | [src/non_matching/ovl_7b9cb4/200abe0.c](../../src/non_matching/ovl_7b9cb4/200abe0.c) |
| `?` | [src/non_matching/ovl_7bf5a8/2008170.c](../../src/non_matching/ovl_7bf5a8/2008170.c) |
| `?` | [src/non_matching/ovl_7c097c/20096bc.c](../../src/non_matching/ovl_7c097c/20096bc.c) |
| `?` | [src/non_matching/ovl_7c7b9c/20090a0.c](../../src/non_matching/ovl_7c7b9c/20090a0.c) |
| `?` | [src/non_matching/ovl_7c7b9c/20091c8.c](../../src/non_matching/ovl_7c7b9c/20091c8.c) |
| `?` | [src/non_matching/ovl_7c7b9c/2009684.c](../../src/non_matching/ovl_7c7b9c/2009684.c) |
| `?` | [src/non_matching/ovl_7c7b9c/2009920.c](../../src/non_matching/ovl_7c7b9c/2009920.c) |
| `?` | [src/non_matching/ovl_7c7b9c/200ac84.c](../../src/non_matching/ovl_7c7b9c/200ac84.c) |
| `?` | [src/non_matching/ovl_7cb2c0/200b51c.c](../../src/non_matching/ovl_7cb2c0/200b51c.c) |
| `?` | [src/non_matching/ovl_7cb2c0/200bd10.c](../../src/non_matching/ovl_7cb2c0/200bd10.c) |
| `?` | [src/non_matching/ovl_7cb2c0/200be34.c](../../src/non_matching/ovl_7cb2c0/200be34.c) |
| `?` | [src/non_matching/ovl_7cb2c0/200beec.c](../../src/non_matching/ovl_7cb2c0/200beec.c) |
| `?` | [src/non_matching/ovl_7cb2c0/200d0e4.c](../../src/non_matching/ovl_7cb2c0/200d0e4.c) |
| `?` | [src/non_matching/ovl_7ced6c/2009c84.c](../../src/non_matching/ovl_7ced6c/2009c84.c) |
| `?` | [src/non_matching/ovl_7ced6c/2009d2c.c](../../src/non_matching/ovl_7ced6c/2009d2c.c) |
| `?` | [src/non_matching/ovl_7ced6c/200a16c.c](../../src/non_matching/ovl_7ced6c/200a16c.c) |
| `?` | [src/non_matching/ovl_7ced6c/200a3c4.c](../../src/non_matching/ovl_7ced6c/200a3c4.c) |
| `?` | [src/non_matching/ovl_7d0e88/2008f58.c](../../src/non_matching/ovl_7d0e88/2008f58.c) |
| `OvlFunc_948_200938c` | [src/non_matching/ovl_7d30e0/200938c.c](../../src/non_matching/ovl_7d30e0/200938c.c) |
| `?` | [src/non_matching/ovl_7d5838/20080c0.c](../../src/non_matching/ovl_7d5838/20080c0.c) |
| `?` | [src/non_matching/ovl_7d6418/2008880.c](../../src/non_matching/ovl_7d6418/2008880.c) |
| `?` | [src/non_matching/ovl_7d6418/20096a8.c](../../src/non_matching/ovl_7d6418/20096a8.c) |
| `?` | [src/non_matching/ovl_7db0c8/200804c.c](../../src/non_matching/ovl_7db0c8/200804c.c) |
| `OvlFunc_959_200b054` | [src/non_matching/ovl_7e7574/200b054.c](../../src/non_matching/ovl_7e7574/200b054.c) |
| `?` | [src/non_matching/ovl_7e7574/200c794.c](../../src/non_matching/ovl_7e7574/200c794.c) |
| `?` | [src/non_matching/ovl_7eaf28/2008d24.c](../../src/non_matching/ovl_7eaf28/2008d24.c) |
| `OvlFunc_965_200a8a0` | [src/non_matching/ovl_7ef4f4/200a8a0.c](../../src/non_matching/ovl_7ef4f4/200a8a0.c) |
| `?` | [src/non_matching/ovl_7f2f14/2009d48.c](../../src/non_matching/ovl_7f2f14/2009d48.c) |
| `?` | [src/non_matching/ovl_7f2f14/200a47c.c](../../src/non_matching/ovl_7f2f14/200a47c.c) |
| `OvlFunc_969_20092c8` | [src/non_matching/ovl_7f6e64/20092c8.c](../../src/non_matching/ovl_7f6e64/20092c8.c) |
| `OvlFunc_969_200a360` | [src/non_matching/ovl_7f6e64/200a360.c](../../src/non_matching/ovl_7f6e64/200a360.c) |
| `?` | [src/non_matching/ovl_7fb4a8/2008e10.c](../../src/non_matching/ovl_7fb4a8/2008e10.c) |
| `?` | [src/non_matching/ovl_7fcd20/200829c.c](../../src/non_matching/ovl_7fcd20/200829c.c) |
| `?` | [src/non_matching/ovl_common/15b8.c](../../src/non_matching/ovl_common/15b8.c) |
| `?` | [src/non_matching/ovl_common/4cc.c](../../src/non_matching/ovl_common/4cc.c) |
| `?` | [src/non_matching/ovl_common/common1_2060.c](../../src/non_matching/ovl_common/common1_2060.c) |
| `OvlFunc_common1_920` | [src/non_matching/ovl_common/common1_920.c](../../src/non_matching/ovl_common/common1_920.c) |
| `?` | [src/non_matching/ovl_common/fac.c](../../src/non_matching/ovl_common/fac.c) |
| `?` | [src/non_matching/rom_15000/80160fc.c](../../src/non_matching/rom_15000/80160fc.c) |
| `?` | [src/non_matching/rom_15000/801bc34.c](../../src/non_matching/rom_15000/801bc34.c) |
| `?` | [src/non_matching/rom_15000/801c8a0.c](../../src/non_matching/rom_15000/801c8a0.c) |
| `?` | [src/non_matching/rom_15000/801cc50.c](../../src/non_matching/rom_15000/801cc50.c) |
| `?` | [src/non_matching/rom_15000/8020198.c](../../src/non_matching/rom_15000/8020198.c) |
| `?` | [src/non_matching/rom_15000/8020244.c](../../src/non_matching/rom_15000/8020244.c) |
| `Func_8022b44` | [src/non_matching/rom_15000/8022a7c.c](../../src/non_matching/rom_15000/8022a7c.c) |
| `?` | [src/non_matching/rom_15000/8028df4.c](../../src/non_matching/rom_15000/8028df4.c) |
| `Func_8023178` | [src/non_matching/rom_15000/Func_8023178.c](../../src/non_matching/rom_15000/Func_8023178.c) |
| `?` | [src/non_matching/rom_15000/Func_8023e70.c](../../src/non_matching/rom_15000/Func_8023e70.c) |
| `?` | [src/non_matching/rom_15000/Func_8024934.c](../../src/non_matching/rom_15000/Func_8024934.c) |
| `Func_8026080` | [src/non_matching/rom_15000/Func_8026080.c](../../src/non_matching/rom_15000/Func_8026080.c) |
| `Func_8027114` | [src/non_matching/rom_15000/Func_8027114.c](../../src/non_matching/rom_15000/Func_8027114.c) |
| `?` | [src/non_matching/rom_15000/MenuBar.c](../../src/non_matching/rom_15000/MenuBar.c) |
| `?` | [src/non_matching/rom_7d30e0/200949c.c](../../src/non_matching/rom_7d30e0/200949c.c) |
| `?` | [src/non_matching/rom_7d30e0/2009df8.c](../../src/non_matching/rom_7d30e0/2009df8.c) |
| `?` | [src/non_matching/rom_8a000/808bec0.c](../../src/non_matching/rom_8a000/808bec0.c) |
| `?` | [src/non_matching/rom_8a000/80916b0.c](../../src/non_matching/rom_8a000/80916b0.c) |
| `?` | [src/non_matching/rom_8a000/80919d8.c](../../src/non_matching/rom_8a000/80919d8.c) |
| `?` | [src/non_matching/rom_8a000/8093304.c](../../src/non_matching/rom_8a000/8093304.c) |
| `?` | [src/non_matching/rom_8a000/8094428.c](../../src/non_matching/rom_8a000/8094428.c) |
| `?` | [src/non_matching/rom_8a000/809509c.c](../../src/non_matching/rom_8a000/809509c.c) |
| `?` | [src/non_matching/rom_8a000/95290.c](../../src/non_matching/rom_8a000/95290.c) |
| `?` | [src/non_matching/rom_8a000/FieldMain.c](../../src/non_matching/rom_8a000/FieldMain.c) |
| `Task_ScreenWindowTransition` | [src/non_matching/rom_8a000/Task_ScreenWindowTransition.c](../../src/non_matching/rom_8a000/Task_ScreenWindowTransition.c) |
| `?` | [src/non_matching/rom_9000/80f9f4.c](../../src/non_matching/rom_9000/80f9f4.c) |
| `Task_Debug_SpriteTest` | [src/non_matching/rom_9000/Task_Debug_SpriteTest.c](../../src/non_matching/rom_9000/Task_Debug_SpriteTest.c) |
| `UpdateActors` | [src/non_matching/rom_9000/UpdateActors.c](../../src/non_matching/rom_9000/UpdateActors.c) |
| `?` | [src/non_matching/rom_a1000/80a33d4.c](../../src/non_matching/rom_a1000/80a33d4.c) |
| `?` | [src/non_matching/rom_a1000/80a3ddc.c](../../src/non_matching/rom_a1000/80a3ddc.c) |
| `?` | [src/non_matching/rom_a1000/80a5fe0.c](../../src/non_matching/rom_a1000/80a5fe0.c) |
| `?` | [src/non_matching/rom_a1000/80a8088.c](../../src/non_matching/rom_a1000/80a8088.c) |
| `?` | [src/non_matching/rom_a1000/80ad6d4.c](../../src/non_matching/rom_a1000/80ad6d4.c) |
| `Func_80a2680` | [src/non_matching/rom_a1000/Func_80a2680.c](../../src/non_matching/rom_a1000/Func_80a2680.c) |
| `Func_80acab8` | [src/non_matching/rom_a1000/Func_80acab8.c](../../src/non_matching/rom_a1000/Func_80acab8.c) |
| `?` | [src/non_matching/rom_b5000/80b84c0.c](../../src/non_matching/rom_b5000/80b84c0.c) |
| `?` | [src/non_matching/rom_b5000/80b8574.c](../../src/non_matching/rom_b5000/80b8574.c) |
| `?` | [src/non_matching/rom_b5000/80b8824.c](../../src/non_matching/rom_b5000/80b8824.c) |
| `Func_80b9724` | [src/non_matching/rom_b5000/80b9604.c](../../src/non_matching/rom_b5000/80b9604.c) |
| `?` | [src/non_matching/rom_b5000/80be18c.c](../../src/non_matching/rom_b5000/80be18c.c) |
| `Func_80bbb0c` | [src/non_matching/rom_b5000/Func_80bbb0c.c](../../src/non_matching/rom_b5000/Func_80bbb0c.c) |
| `Func_80be378` | [src/non_matching/rom_b5000/Func_80be378.c](../../src/non_matching/rom_b5000/Func_80be378.c) |
| `?` | [src/non_matching/rom_c0/1b70_cos.c](../../src/non_matching/rom_c0/1b70_cos.c) |
| `?` | [src/non_matching/rom_c0/1b70_sin.c](../../src/non_matching/rom_c0/1b70_sin.c) |
| `?` | [src/non_matching/rom_c0/2dd8.c](../../src/non_matching/rom_c0/2dd8.c) |
| `?` | [src/non_matching/rom_c0/2df0.c](../../src/non_matching/rom_c0/2df0.c) |
| `free` | [src/non_matching/rom_c0/rom_2df0.c](../../src/non_matching/rom_c0/rom_2df0.c) |
| `?` | [src/non_matching/rom_c9000/80cd52c.c](../../src/non_matching/rom_c9000/80cd52c.c) |
| `?` | [src/non_matching/rom_c9000/80e3a3c.c](../../src/non_matching/rom_c9000/80e3a3c.c) |
| `Func_80e7338` | [src/non_matching/rom_c9000/80e7338.c](../../src/non_matching/rom_c9000/80e7338.c) |
| `Anim_Cybele` | [src/non_matching/rom_c9000/Anim_Cybele.c](../../src/non_matching/rom_c9000/Anim_Cybele.c) |
| `Anim_Judgment` | [src/non_matching/rom_c9000/Anim_Judgment.c](../../src/non_matching/rom_c9000/Anim_Judgment.c) |
| `Anim_Neptune` | [src/non_matching/rom_c9000/Anim_Neptune.c](../../src/non_matching/rom_c9000/Anim_Neptune.c) |
| `Anim_Procne` | [src/non_matching/rom_c9000/Anim_Procne.c](../../src/non_matching/rom_c9000/Anim_Procne.c) |
| `Anim_Thor` | [src/non_matching/rom_c9000/Anim_Thor.c](../../src/non_matching/rom_c9000/Anim_Thor.c) |
| `BaseAnim_Bite_Sting` | [src/non_matching/rom_c9000/BaseAnim_Bite_Sting.c](../../src/non_matching/rom_c9000/BaseAnim_Bite_Sting.c) |
| `BaseAnim_Meteor` | [src/non_matching/rom_c9000/BaseAnim_Meteor.c](../../src/non_matching/rom_c9000/BaseAnim_Meteor.c) |
| `BaseAnim_ParticleSpray` | [src/non_matching/rom_c9000/BaseAnim_ParticleSpray.c](../../src/non_matching/rom_c9000/BaseAnim_ParticleSpray.c) |
| `BaseAnim_Tiamat` | [src/non_matching/rom_c9000/BaseAnim_Tiamat.c](../../src/non_matching/rom_c9000/BaseAnim_Tiamat.c) |
| `LuckyDiceMain` | [src/non_matching/rom_f4000/LuckyDiceMain.c](../../src/non_matching/rom_f4000/LuckyDiceMain.c) |
| `Func_80f6440` | [src/non_matching/rom_f6000/Func_80f6440.c](../../src/non_matching/rom_f6000/Func_80f6440.c) |
| `Func_80f7f78` | [src/non_matching/rom_f6000/Func_80f7f78.c](../../src/non_matching/rom_f6000/Func_80f7f78.c) |
| `LuckyWheelsMain` | [src/non_matching/rom_f6000/LuckyWheelsMain.c](../../src/non_matching/rom_f6000/LuckyWheelsMain.c) |
| `ply_pend` | [src/non_matching/rom_f9000/rom_f9afc.c](../../src/non_matching/rom_f9000/rom_f9afc.c) |
| `ply_prio` | [src/non_matching/rom_f9000/rom_f9b40.c](../../src/non_matching/rom_f9000/rom_f9b40.c) |
| `ply_lfodl` | [src/non_matching/rom_f9000/rom_f9bf4.c](../../src/non_matching/rom_f9000/rom_f9bf4.c) |
| `?` | [src/non_matching/tiny_reg_order.c](../../src/non_matching/tiny_reg_order.c) |
