# Verified parked frontier

646 parks with a parkcheck-verified figure. Parks with no recipe are ABSENT and are a work item, not a target.


## Blocker buckets (a park can appear in several)

- **frame** — 385
- **reload** — 299
- **cse-gcse** — 295
- **alloc-order** — 209
- **symbol-table** — 141
- **unclassified** — 91
- **pool-order** — 75
- **tu-shape** — 52
- **sched2-tie** — 37
- **alias-set** — 19
- **arg-interleave** — 18

## Ranked, figure <= 20

| figure | of | park | blocker(s) |
|---:|---:|---|---|
| **0** | 81 | `rom_b5000/80b9554.c` | — |
| **1** | 59 | `rom_a1000/80a9a5c.c` | symbol-table |
| **1** | 66 | `rom_b0000/80b110c.c` | symbol-table |
| **2** | 43 | `overlays/200db90.c` | cse-gcse |
| **2** | 74 | `ovl_7b9cb4/20082cc.c` | — |
| **2** ⚑flag | 75 | `ovl_7e0928/2008ba4.c` | sched2-tie, pool-order, cse-gcse, symbol-table |
| **2** | 271 | `ovl_7f2f14/200c2bc.c` | reload, cse-gcse, frame, symbol-table |
| **2** | 26 | `rom_8a000/809802c.c` | reload, frame |
| **2** | 191 | `rom_8a000/809abb4.c` | reload, cse-gcse |
| **2** | 52 | `rom_8a000/942e0.c` | — |
| **2** | ? | `rom_a1000/80a96d8.c` | sched2-tie, reload, cse-gcse, alias-set, frame, arg-interleave |
| **2** | 101 | `rom_a1000/80ab21c.c` | sched2-tie, alloc-order, reload |
| **2** | 36 | `rom_c9000/80df9d0.c` | — |
| **2** | 18 | `rom_c9000/dfa18_Tackle.c` | reload, alias-set, frame, symbol-table |
| **3** | 38 | `ovl_7b4558/2009818.c` | frame |
| **3** | 214 | `ovl_7e7574/200d0e4.c` | sched2-tie, alloc-order, reload, cse-gcse, frame, arg-interleave |
| **3** | 129 | `rom_8a000/8095938.c` | reload, symbol-table |
| **3** | 56 | `rom_8a000/8095fcc.c` | reload |
| **3** | 104 | `rom_c9000/cd260_a.c` | alloc-order, cse-gcse, frame |
| **4** | ? | `ovl_787e04/2008578.c` | alloc-order, reload, cse-gcse |
| **4** | 85 | `ovl_78ef88/200c260.c` | — |
| **4** | 207 | `ovl_7a4370/20092f4.c` | cse-gcse |
| **4** | 141 | `ovl_7a5214/2009424.c` | — |
| **4** | 59 | `ovl_7d30e0/2009308.c` | alloc-order |
| **4** | 85 | `rom_15000/80286a0.c` | sched2-tie, cse-gcse, symbol-table |
| **4** | 103 | `rom_77000/8078144.c` | symbol-table |
| **4** | 36 | `rom_9000/8011e88.c` | alloc-order |
| **4** | 30 | `rom_a1000/80ad5b4.c` | pool-order, frame, symbol-table |
| **4** | 19 | `rom_a1000/rom_ab1f4.c` | frame |
| **4** | 119 | `rom_b5000/80b6d30.c` | cse-gcse |
| **4** | 39 | `rom_f0000/80f0254.c` | arg-interleave |
| **5** | 177 | `ovl_7aa430/2009a3c.c` | reload, cse-gcse |
| **5** | 177 | `ovl_7ac2d8/200cfcc.c` | alloc-order, reload, cse-gcse |
| **5** | 86 | `rom_77000/807808c.c` | sched2-tie, cse-gcse |
| **5** | 40 | `rom_77000/8078870.c` | reload, cse-gcse, frame |
| **5** | 39 | `rom_c9000/rom_e3a3c.c` | alloc-order |
| **6** | 72 | `ovl_77a7c8/200b95c.c` | frame |
| **6** | 26 | `ovl_7ac2d8/200a648.c` | symbol-table |
| **6** | 26 | `ovl_7ac2d8/200adcc.c` | cse-gcse, frame |
| **6** | 110 | `ovl_7b4558/200a1b0.c` | cse-gcse, arg-interleave |
| **6** | 44 | `ovl_7f6e64/200b600.c` | reload |
| **6** | 34 | `ovl_common/common1_15b8.c` | frame |
| **6** | 40 | `rom_15000/8029274.c` | reload, frame |
| **6** | 133 | `rom_15000/DisplayMenuArrowCursor.c` | sched2-tie, reload, cse-gcse, alias-set |
| **6** | 167 | `rom_a1000/80a8f40.c` | reload, symbol-table |
| **6** | 37 | `rom_b5000/80c0130.c` | cse-gcse |
| **7** | 40 | `ovl_7aa430/2009bc8.c` | alloc-order |
| **7** | 40 | `ovl_7ac2d8/200d158.c` | — |
| **7** | 221 | `ovl_7b8cb0/2008904.c` | reload, cse-gcse |
| **7** | 33 | `rom_15000/801f730.c` | — |
| **7** | 88 | `rom_77000/807a0f4.c` | alloc-order, reload, cse-gcse |
| **7** | 122 | `rom_8a000/8096810.c` | alloc-order, pool-order, cse-gcse, frame |
| **7** | 48 | `rom_8a000/8096b88.c` | — |
| **7** | 44 | `rom_8a000/8099070.c` | — |
| **7** | 40 | `rom_9000/8011a84.c` | frame |
| **8** | 80 | `ovl_77dd1c/20090a4.c` | frame |
| **8** | ? | `ovl_787e04/200968c.c` | sched2-tie, alloc-order, reload, cse-gcse, alias-set |
| **8** | 537 | `ovl_78ef88/2009d04.c` | sched2-tie, pool-order, cse-gcse, frame |
| **8** | ? | `ovl_791794/200aeb0.c` | sched2-tie, alloc-order, reload, cse-gcse, alias-set |
| **8** | 41 | `ovl_7a8c8c/2008ed8.c` | alloc-order |
| **8** | 17 | `rom_15000/801c154.c` | pool-order, frame |
| **8** | 17 | `rom_15000/rom_1c154.c` | pool-order, frame |
| **8** | 29 | `rom_8a000/rom_91254.c` | cse-gcse |
| **8** | 57 | `rom_a1000/80a1a40.c` | reload, pool-order |
| **8** | 29 | `rom_f2000/80f3858.c` | alloc-order, cse-gcse |
| **9** | 133 | `ovl_78b2ac/2008ef8.c` | cse-gcse, frame, symbol-table |
| **9** | 72 | `ovl_7c7b9c/200985c.c` | — |
| **9** | 57 | `ovl_7d0e88/200a1ac.c` | alloc-order |
| **9** | 44 | `ovl_7db0c8/200842c.c` | alloc-order |
| **9** | 36 | `ovl_7e0928/2008ad4.c` | — |
| **9** | 175 | `ovl_7fa4ec/2008da4.c` | frame |
| **9** | 27 | `rom_15000/8019908.c` | frame |
| **9** | 120 | `rom_77000/8077f70.c` | sched2-tie, reload, symbol-table |
| **10** | 27 | `ovl_7b9cb4/20086a0.c` | — |
| **10** | 30 | `rom_b5000/rom_b8530.c` | — |
| **10** | 33 | `rom_f6000/80f7f30.c` | — |
| **11** | 23 | `ovl_7b7f1c/2009060.c` | alloc-order |
| **11** | 146 | `rom_8a000/8096ddc.c` | sched2-tie, alloc-order, reload, cse-gcse, alias-set |
| **11** | 60 | `rom_a1000/80a8578.c` | alloc-order |
| **11** | 12 | `rom_f9000/rom_f9a18.c` | — |
| **12** | 59 | `overlays/200a750.c` | symbol-table |
| **12** | 136 | `ovl_784360/200a440.c` | sched2-tie, alias-set, tu-shape |
| **12** | 109 | `ovl_799abc/2008a68.c` | cse-gcse |
| **12** | 371 | `ovl_7aa430/200a030.c` | sched2-tie, alloc-order, reload, cse-gcse, arg-interleave |
| **12** | 371 | `ovl_7ac2d8/200d5c0.c` | sched2-tie, alloc-order, reload, cse-gcse, frame |
| **12** | 34 | `rom_77000/8077348.c` | reload |
| **12** | 44 | `rom_9000/rom_bfa4.c` | reload, frame |
| **13** | 55 | `overlays/2008d54.c` | — |
| **13** | 32 | `ovl_7795e8/2008384.c` | symbol-table |
| **13** | 91 | `ovl_7ac2d8/20096c4.c` | alloc-order, reload |
| **13** | 262 | `ovl_7b0400/2009af0.c` | — |
| **13** | 72 | `ovl_7ced6c/200add0.c` | frame |
| **13** | 262 | `ovl_7f2f14/2009af0.c` | — |
| **13** | 47 | `rom_8a000/80979a4.c` | alloc-order, cse-gcse |
| **14** | 53 | `ovl_791794/200ac1c.c` | — |
| **14** | ? | `ovl_7db0c8/2008a3c.c` | sched2-tie, reload, frame, symbol-table |
| **14** | 36 | `rom_15000/801fd34.c` | frame |
| **14** | 24 | `rom_15000/80216b4.c` | — |
| **14** | 92 | `rom_15000/8021cb8.c` | cse-gcse |
| **14** | 51 | `rom_77000/807a2e4.c` | frame |
| **14** | 195 | `rom_8a000/808b674.c` | sched2-tie, alloc-order, reload, pool-order, frame, tu-shape |
| **14** | 101 | `rom_a1000/80a6794.c` | alloc-order |
| **14** | 59 | `rom_b0000/80b0070.c` | symbol-table |
| **14** | 59 | `rom_b5000/80b6a60.c` | — |
| **14** | 142 | `rom_c9000/AnimEnd.c` | frame |
| **14** | 87 | `rom_f0000/LoadGS1CreditsBG.c` | cse-gcse, symbol-table |
| **15** | 26 | `rom_77000/8079bf8.c` | — |
| **15** | 24 | `rom_b5000/80bf574.c` | reload |
| **15** | 707 | `rom_c9000/Anim_CriticalHit.c` | alloc-order, reload, cse-gcse, frame, tu-shape |
| **16** | 119 | `ovl_7ed0a0/20090c4.c` | reload, cse-gcse |
| **16** | 140 | `ovl_common/common1_1354.c` | alloc-order, frame |
| **16** | 48 | `rom_8a000/80936a0.c` | — |
| **16** | 29 | `rom_a1000/80a1090.c` | alloc-order |
| **16** | 29 | `rom_b5000/80be02c.c` | alloc-order |
| **16** | 738 | `rom_c9000/Anim_Djinni.c` | alloc-order, reload, cse-gcse, frame, symbol-table |
| **17** | 160 | `ovl_78ef88/200a27c.c` | alloc-order, reload, cse-gcse, frame |
| **17** | 92 | `ovl_7b8cb0/20086f0.c` | — |
| **17** | 38 | `ovl_7e7574/200a06c.c` | alloc-order, cse-gcse, frame |
| **17** | 134 | `rom_15000/801a7f4.c` | alloc-order, cse-gcse, alias-set, frame |
| **17** | 69 | `rom_15000/801c34c.c` | frame |
| **17** | 163 | `rom_15000/8029094.c` | alloc-order, cse-gcse, frame, tu-shape |
| **17** | 37 | `rom_8a000/8092b08.c` | — |
| **17** | 50 | `rom_9000/801219c.c` | — |
| **17** | 25 | `rom_a1000/80ad69c.c` | alloc-order, cse-gcse |
| **17** | 22 | `rom_b5000/80c1054.c` | — |
| **17** | 16 | `rom_f9000/80f9a30.c` | frame |
| **18** | 48 | `overlays/2008640.c` | frame |
| **18** | 31 | `ovl_7d4af4/20086e8.c` | frame |
| **18** | 44 | `ovl_7e3e08/2008f10.c` | frame |
| **18** | 46 | `rom_15000/80173f4.c` | frame |
| **18** | 20 | `rom_15000/80270ac.c` | reload, frame, tu-shape |
| **18** | 50 | `rom_8a000/808fe38.c` | — |
| **18** | 26 | `rom_8a000/8091c44.c` | frame |
| **18** | 63 | `rom_8a000/8094154.c` | frame |
| **18** | 26 | `rom_8a000/rom_91c44.c` | — |
| **18** | 411 | `rom_a1000/80a112c.c` | alloc-order, reload, cse-gcse, frame |
| **18** | 25 | `rom_b5000/rom_bd7a4.c` | — |
| **19** | 24 | `ovl_7cb2c0/20080fc.c` | — |
| **19** | 184 | `ovl_7e7574/200cda0.c` | reload, cse-gcse, symbol-table |
| **19** | 32 | `ovl_7ef4f4/200a660.c` | — |
| **19** | 32 | `rom_15000/801b9a8.c` | cse-gcse, frame |
| **19** | 117 | `rom_8a000/Task_Thunder.c` | — |
| **19** | 26 | `rom_9000/8011d60.c` | — |
| **19** | 40 | `rom_9000/8011ddc.c` | — |
| **19** | 45 | `rom_b5000/80b8000.c` | — |
| **20** | 73 | `rom_15000/8028ef0.c` | alloc-order, reload |
| **20** | 44 | `rom_15000/rom_175c0.c` | reload, frame |
| **20** | 26 | `rom_77000/8078550.c` | alloc-order |
| **20** | 30 | `rom_77000/807a458.c` | — |
| **20** | 43 | `rom_8a000/8091eb0.c` | cse-gcse |
| **20** | 75 | `rom_8a000/8094380.c` | frame |
| **20** | 28 | `rom_9000/800fa8c.c` | alloc-order, frame |
| **20** | 26 | `rom_9000/HeightTile_4.c` | — |
| **20** | 158 | `rom_b5000/80c1afc.c` | alloc-order, symbol-table, tu-shape |
| **20** | 76 | `rom_f6000/80f6148.c` | alloc-order, cse-gcse, frame, symbol-table |
