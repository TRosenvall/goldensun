# Park figure audit — 388 of 571 measured figures are PHASE, not distance

Generated from a full `tools/parkcheck.py` sweep (batch 331). **Regenerate by
running `parkcheck` over the whole corpus** — it now flags this itself.

## What the flag means

A park's figure is a **positional** count over the encoding stream. When the two
sides' 16-bit instruction counts differ, the streams are compared **out of
phase**: one inserted or missing instruction makes every later position differ,
and the figure counts the offset rather than the number of wrong decisions.

    388 of 571 parks with a verified figure (67%) are in this state.

The extreme case is `src/non_matching/ovl_7d6418/2008e5c.c`, which claims a
figure of **918** with lengths 878/879 — **a one-instruction gap.** That park
reads as hopeless and is one structural difference away from being measurable.

**So for these parks the figure is not a difficulty estimate.** It does not say
the code is nearly right either — the real distance is simply *unknown* until
the lengths agree. `tools/aligncmp.py` is what gives a real number.

## Length gap distribution

| length gap | parks |
|---|---|
| 1 | 133 |
| 2 | 71 |
| 3 | 38 |
| 4 | 36 |
| 5 | 24 |
| 6 | 14 |
| 7 | 9 |
| 8 | 11 |
| 9 | 9 |
| 10 | 5 |
| 11+ | 38 |

**133 parks are ONE instruction apart in length.** Another 71 are two.

## Why this is the highest-yield target list in the tree

Batch 331 worked four parks that happen to be in this shape, without knowing it
at the time. **All four moved, and three landed outright:**

| park | figure | gap | outcome |
|---|---|---|---|
| `ovl_7cb2c0/20080fc.c` | 19 | 1 | **LANDED at 0** |
| `rom_77000/8078550.c` | 20 | 2 | **LANDED at 0** |
| `rom_77000/807a458.c` | 20 | 2 | **LANDED at 0** |
| `ovl_7b9cb4/20086a0.c` | 10 | 2 | re-priced: the real content is **2 differing instructions of 24** |

Four for four is a small sample, but the mechanism is not mysterious: finding
the one missing instruction is a bounded question, and once the lengths agree
the residue is usually small. Two of the three landings came from a park whose
header had **correctly** identified it as misalignment and then pointed the
lever at the wrong statement.

## Shortlist: gap <= 2 and figure <= 40

The cheapest to re-price. Sorted by gap then figure.

| figure | ref/ours | gap | park |
|---|---|---|---|
| 12 | 51/50 | 1 | [src/non_matching/overlays/200a750.c](../src/non_matching/overlays/200a750.c) |
| 13 | 28/29 | 1 | [src/non_matching/ovl_7795e8/2008384.c](../src/non_matching/ovl_7795e8/2008384.c) |
| 17 | 27/28 | 1 | [src/non_matching/ovl_7e7574/200a06c.c](../src/non_matching/ovl_7e7574/200a06c.c) |
| 18 | 29/28 | 1 | [src/non_matching/ovl_7d4af4/20086e8.c](../src/non_matching/ovl_7d4af4/20086e8.c) |
| 18 | 41/42 | 1 | [src/non_matching/ovl_7e3e08/2008f10.c](../src/non_matching/ovl_7e3e08/2008f10.c) |
| 19 | 24/23 | 1 | [src/non_matching/ovl_7cb2c0/20080fc.c](../src/non_matching/ovl_7cb2c0/20080fc.c) **(landed in 331)** |
| 20 | 35/36 | 1 | [src/non_matching/rom_8a000/8091eb0.c](../src/non_matching/rom_8a000/8091eb0.c) |
| 23 | 21/20 | 1 | [src/non_matching/rom_a1000/80a7440.c](../src/non_matching/rom_a1000/80a7440.c) |
| 23 | 26/25 | 1 | [src/non_matching/ovl_7c5efc/2008094.c](../src/non_matching/ovl_7c5efc/2008094.c) |
| 23 | 65/64 | 1 | [src/non_matching/ovl_7892c8/200b1b8.c](../src/non_matching/ovl_7892c8/200b1b8.c) |
| 24 | 25/24 | 1 | [src/non_matching/ovl_7fc720/200871c.c](../src/non_matching/ovl_7fc720/200871c.c) |
| 24 | 39/38 | 1 | [src/non_matching/rom_c0/8006088.c](../src/non_matching/rom_c0/8006088.c) |
| 25 | 65/64 | 1 | [src/non_matching/ovl_78c76c/2008098.c](../src/non_matching/ovl_78c76c/2008098.c) |
| 26 | 28/27 | 1 | [src/non_matching/rom_b0000/80b0a20.c](../src/non_matching/rom_b0000/80b0a20.c) |
| 26 | 29/28 | 1 | [src/non_matching/rom_b5000/80b90ac.c](../src/non_matching/rom_b5000/80b90ac.c) |
| 26 | 37/36 | 1 | [src/non_matching/rom_b0000/80b2ed8.c](../src/non_matching/rom_b0000/80b2ed8.c) |
| 26 | 39/40 | 1 | [src/non_matching/rom_a1000/80ae9f0.c](../src/non_matching/rom_a1000/80ae9f0.c) |
| 26 | 53/54 | 1 | [src/non_matching/ovl_7b2078/2008388.c](../src/non_matching/ovl_7b2078/2008388.c) |
| 26 | 53/54 | 1 | [src/non_matching/ovl_7fb4a8/20087b0.c](../src/non_matching/ovl_7fb4a8/20087b0.c) |
| 27 | 27/28 | 1 | [src/non_matching/rom_c0/80063bc.c](../src/non_matching/rom_c0/80063bc.c) |
| 28 | 206/205 | 1 | [src/non_matching/rom_b0000/80b1bd0.c](../src/non_matching/rom_b0000/80b1bd0.c) |
| 29 | 29/28 | 1 | [src/non_matching/rom_c9000/80e3908.c](../src/non_matching/rom_c9000/80e3908.c) |
| 29 | 33/34 | 1 | [src/non_matching/ovl_7e7574/200a69c.c](../src/non_matching/ovl_7e7574/200a69c.c) |
| 29 | 35/34 | 1 | [src/non_matching/rom_77000/rom_79008.c](../src/non_matching/rom_77000/rom_79008.c) |
| 29 | 36/35 | 1 | [src/non_matching/rom_15000/801e3c8.c](../src/non_matching/rom_15000/801e3c8.c) |
| 29 | 116/115 | 1 | [src/non_matching/rom_8a000/8093af8.c](../src/non_matching/rom_8a000/8093af8.c) |
| 30 | 57/56 | 1 | [src/non_matching/rom_c9000/80d67dc.c](../src/non_matching/rom_c9000/80d67dc.c) |
| 31 | 44/43 | 1 | [src/non_matching/rom_a1000/80a2144.c](../src/non_matching/rom_a1000/80a2144.c) |
| 33 | 30/29 | 1 | [src/non_matching/rom_b5000/rom_c00d8.c](../src/non_matching/rom_b5000/rom_c00d8.c) |
| 33 | 51/50 | 1 | [src/non_matching/rom_b5000/80bac6c.c](../src/non_matching/rom_b5000/80bac6c.c) |
| 36 | 50/49 | 1 | [src/non_matching/rom_b5000/80c0228.c](../src/non_matching/rom_b5000/80c0228.c) |
| 37 | 86/85 | 1 | [src/non_matching/ovl_7d0e88/20099f0.c](../src/non_matching/ovl_7d0e88/20099f0.c) |
| 37 | 95/96 | 1 | [src/non_matching/rom_b5000/80b9470.c](../src/non_matching/rom_b5000/80b9470.c) |
| 38 | 35/34 | 1 | [src/non_matching/rom_a1000/80ae99c.c](../src/non_matching/rom_a1000/80ae99c.c) |
| 38 | 40/39 | 1 | [src/non_matching/rom_f0000/80f0614.c](../src/non_matching/rom_f0000/80f0614.c) |
| 38 | 44/43 | 1 | [src/non_matching/rom_c9000/80e727c.c](../src/non_matching/rom_c9000/80e727c.c) |
| 38 | 79/78 | 1 | [src/non_matching/ovl_7aa430/2009df8.c](../src/non_matching/ovl_7aa430/2009df8.c) |
| 39 | 40/39 | 1 | [src/non_matching/rom_77000/8078980.c](../src/non_matching/rom_77000/8078980.c) |
| 39 | 40/39 | 1 | [src/non_matching/rom_c9000/80e38b8.c](../src/non_matching/rom_c9000/80e38b8.c) |
| 39 | 57/56 | 1 | [src/non_matching/rom_77000/80788c4.c](../src/non_matching/rom_77000/80788c4.c) |
| 40 | 47/48 | 1 | [src/non_matching/rom_15000/8021d88.c](../src/non_matching/rom_15000/8021d88.c) |
| 10 | 24/22 | 2 | [src/non_matching/ovl_7b9cb4/20086a0.c](../src/non_matching/ovl_7b9cb4/20086a0.c) **(landed in 331)** |
| 17 | 16/18 | 2 | [src/non_matching/rom_f9000/80f9a30.c](../src/non_matching/rom_f9000/80f9a30.c) |
| 20 | 23/21 | 2 | [src/non_matching/rom_77000/8078550.c](../src/non_matching/rom_77000/8078550.c) **(landed in 331)** |
| 20 | 27/25 | 2 | [src/non_matching/rom_77000/807a458.c](../src/non_matching/rom_77000/807a458.c) **(landed in 331)** |
| 21 | 30/28 | 2 | [src/non_matching/rom_77000/807961c.c](../src/non_matching/rom_77000/807961c.c) |
| 23 | 28/26 | 2 | [src/non_matching/rom_b5000/80c1014.c](../src/non_matching/rom_b5000/80c1014.c) |
| 25 | 23/25 | 2 | [src/non_matching/rom_8a000/rom_92b54.c](../src/non_matching/rom_8a000/rom_92b54.c) |
| 25 | 27/25 | 2 | [src/non_matching/ovl_common/common1_588.c](../src/non_matching/ovl_common/common1_588.c) |
| 27 | 34/32 | 2 | [src/non_matching/rom_9000/8011164.c](../src/non_matching/rom_9000/8011164.c) |
| 27 | 51/49 | 2 | [src/non_matching/rom_a1000/80a3354.c](../src/non_matching/rom_a1000/80a3354.c) |
| 29 | 25/23 | 2 | [src/non_matching/rom_c0/rom_5868.c](../src/non_matching/rom_c0/rom_5868.c) |
| 30 | 27/25 | 2 | [src/non_matching/rom_15000/rom_28e54.c](../src/non_matching/rom_15000/rom_28e54.c) |
| 30 | 29/27 | 2 | [src/non_matching/rom_f6000/80f7df0.c](../src/non_matching/rom_f6000/80f7df0.c) |
| 30 | 30/28 | 2 | [src/non_matching/ovl_77a7c8/200a7dc.c](../src/non_matching/ovl_77a7c8/200a7dc.c) |
| 30 | 36/34 | 2 | [src/non_matching/ovl_79c738/200809c.c](../src/non_matching/ovl_79c738/200809c.c) |
| 31 | 27/29 | 2 | [src/non_matching/rom_c0/8006408.c](../src/non_matching/rom_c0/8006408.c) |
| 31 | 43/41 | 2 | [src/non_matching/ovl_7892c8/200a6f0.c](../src/non_matching/ovl_7892c8/200a6f0.c) |
| 36 | 44/42 | 2 | [src/non_matching/rom_a1000/80a40ac.c](../src/non_matching/rom_a1000/80a40ac.c) |
| 38 | 264/266 | 2 | [src/non_matching/rom_77000/807905c.c](../src/non_matching/rom_77000/807905c.c) |

## Caveat on the counts themselves

`objcmp`'s "instruction count" is **16-bit encodings only**. A Thumb `bl` is a
32-bit encoding and is therefore counted in the *other* column, alongside true
literal-pool words — so both numbers understate a function's real length by its
call count. The columns are labelled accordingly as of batch 331; read them
together and subtract the calls when you need a true length. A difference
between the two sides' 16-bit counts is still a genuine signal, because each
side excludes its own calls.
