# Naming evidence

**Generated — do not hand-edit.** Regenerate with:

    python3 tools/name_evidence.py --doc > docs/names.md

Renaming is deliberately deferred until elevation is finished. Function names
do not survive into the ROM — `objcopy -O binary` drops the symbol table — so a
bulk rename is byte-neutral and `make compare` proves it. Nothing is gained by
paying for it early, and names chosen with a whole overlay in view beat names
chosen one function at a time.

What is **not** free to defer is the evidence, which is what this table holds.
Working out what a function does happens during elevation; without capturing
it, the rename pass would re-read all of it.

## Read the `Basis` column before trusting a name

`docs/attribution.md` records that the inherited annotation corpus gets
mechanism right and **purpose wrong** often enough to matter — `Func_80b7e7c`
does not take the arguments it was documented with, corrected in batch 01. A
name from an annotation and a name from reading the code are not equally
trustworthy, so each row carries where it came from:

| Basis | Meaning | Rename without re-checking? |
|---|---|---|
| `read` | we described the behaviour ourselves while elevating it | yes |
| `named` | the annotation proposes an actual identifier | yes — checkable against the body in seconds |
| `call-trace` | the annotation says outright it is a call trace, not a description | no — it claims nothing about purpose |
| `annotation` | inherited prose asserting purpose | **no — verify first** |
| `none` | nothing beyond the address | no |

Recording only the name would launder a guess into a fact.

| Function | Address | Proposed | Basis | Evidence |
|---|---|---|---|---|
| `OvlFunc_883_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the _c part stays as assembly and keeps its slot in FindEntityAtPosition. Scans entity slots 8..0x41 -- the map-object |
| `OvlFunc_883_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the _a part stays as assembly and keeps its slot in FillMapRectCollisionByte. Writes one byte into every cell of a rec |
| `OvlFunc_905_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_905_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_912_2008030` | `0x02008030` | `ResetRecordArray` | named | Overlay 912: initialise a fifteen-entry slot table. Whole-file conversion of asm/overlays/rom_7a0010/ovl_30_a_a.s. |
| `OvlFunc_913_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_913_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_914_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_914_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_915_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_915_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_923_2008350` | `0x02008350` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_923_2008528` | `0x02008528` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_924_2008350` | `0x02008350` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_924_2008528` | `0x02008528` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_927_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_927_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_929_2008524` | `0x02008524` | `TalkStaged` | named | TalkStaged. Slot 9 delivers a line, turns to face slot 0x0a for sixty frames, turns back to slot 0 for twenty, then delivers a closing line. |
| `OvlFunc_934_2008350` | `0x02008350` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_934_2008528` | `0x02008528` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_946_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_946_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_947_2008350` | `0x02008350` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_947_2008528` | `0x02008528` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_948_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_948_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_957_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_957_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_958_2008350` | `0x02008350` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_958_2008528` | `0x02008528` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_959_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_959_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_964_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_964_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_965_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_965_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_968_20084f4` | `0x020084f4` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `ActorAttrOp_width` | `` | — | read | Actor attribute opcode: collision radius. Whole-file conversion of asm/rom_9000/rom_e220_a_c.s -- one function, so the ROM layout is preserv |
| `ActorCmd_CallNative` | `` | — | read | Actor script VM: the opcode that calls a native predicate. Whole-file conversion of asm/rom_9000/rom_d654_a_c_a_a_a_a.s -- one function, so  |
| `ActorCmd_GotoIfNZ` | `` | — | read | Actor script VM: the two conditional-jump opcodes. Whole-file conversion of asm/rom_9000/rom_d654_a_c_a_a_c.s -- it holds both of these and  |
| `ActorCmd_GotoIfZ` | `` | — | read | Actor script VM: the two conditional-jump opcodes. Whole-file conversion of asm/rom_9000/rom_d654_a_c_a_a_c.s -- it holds both of these and  |
| `ActorCmd_SetScript` | `` | — | read | Actor script VM: the opcode that makes the script jump to a new base. Whole-file conversion of asm/rom_9000/rom_ca2c_a.s -- one function, so |
| `Camera_SetTarget` | `` | — | read | Behaviour: install the 0x135F0 script, optionally retuning the movement. Whole-file conversion of asm/rom_9000/rom_c004_c_a_a_c_a_c_c_c_c_c. |
| `CanEquipItem` | `` | — | read | Equipment: may this unit's class use this item? Whole-file conversion of asm/rom_77000/rom_78414_a_c.s -- one function, so the ROM layout is |
| `Func_801c21c` | `0x0801c21c` | — | read | UI panels: release a panel's OBJ tiles. Split out of asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_a_a.s; the neighbours are ROM layout is unchanged |
| `Func_808ed4c` | `0x0808ed4c` | — | read | Map interaction: read the slot record for whatever the player is facing. Whole-file conversion of asm/rom_8a000/rom_8d9a4_c_a_c_c_c_a.s -- o |
| `Func_8091c1c` | `0x08091c1c` | — | read | Cutscene layer: hand an item to a party member. Split out of asm/rom_8a000/rom_91584_c_a_c_c_c.s; the preceding functions stay in asm/rom_8a |
| `Func_8091ff0` | `0x08091ff0` | — | read | Cutscene layer: start a looping sound. Whole-file conversion of asm/rom_8a000/rom_91584_c_c_a_c_a_c_c_a_a_c.s -- one function, so the ROM la |
| `Func_80925e0` | `0x080925e0` | — | read | Particles: move one along a decaying ballistic arc. Split out of asm/rom_8a000/rom_925e0_a_a_a.s; the _a and _c parts stay as assembly and a |
| `Func_8092848` | `0x08092848` | — | read | Cutscene layer: turn two field actors to face each other. Split out of asm/rom_8a000/rom_925e0_a_a_c.s, which also holds Func_8092878; the r |
| `Func_8097a54` | `0x08097a54` | — | read | Movement: restart the idle script once an actor has stopped moving. Split out of asm/rom_8a000/rom_97384_c_c.s. The neighbours are the _a an |
| `Func_809ad70` | `0x0809ad70` | — | read | Idle flicker: nudge a resting actor's palette at random. Split out of asm/rom_8a000/rom_9ad70_a_a_a.s; the rest stays in ..._a_a_a_c.s, list |
| `Func_80a23c0` | `0x080a23c0` | — | read | Menu: draw the party's coin total. Split out of asm/rom_a1000/rom_a1814_c_a_a_c_a_c.s, which holds eleven functions; the _a and _c parts sta |
| `Func_80a735c` | `0x080a735c` | — | read | Menu: is this item selectable? Split out of asm/rom_a1000/rom_a5534_c_c_c.s. The preceding functions stay in ..._c_c_c_a.s; the trailing .ro |
| `Func_80ad5f4` | `0x080ad5f4` | — | read | Field actor scale table: store one slot's value. Split out of asm/rom_a1000/rom_ad274_c_a.s, which holds six functions; the neighbours are a |
| `Func_80b7e7c` | `0x080b7e7c` | — | read | Battle teardown: release every combatant's sprite. Whole-file conversion of asm/rom_b5000/rom_b7410_a_c_c_c.s -- one function, so the ROM la |
| `InitSpriteLayer` | `` | — | read | Sprites: bind a part to its resource. Split out of asm/rom_9000/rom_b798_c_a_a.s; the _a and _c parts stay as assembly and are listed around |
| `LoadMoveIcon` | `` | — | read | Menu icons: load the move icon for a move. Split out of asm/rom_15000/rom_19ebc_a_c_c.s, which holds twelve functions; the neighbouring _a/_ |
| `LoadOldMoveIcon` | `` | — | read | Menu icons: load the item icon for a move. Split out of asm/rom_15000/rom_19ebc_a_c_c.s, which holds twelve functions; the neighbouring _a/_ |
| `MapActor_SetIdle` | `` | — | read | Cutscene layer: park a field actor where it stands. Whole-file conversion of ROM layout is preserved without splitting the translation unit. |
| `OvlFunc_881_200bfb4` | `0x0200bfb4` | — | read | Split out of that .s; the _a part stays as assembly and keeps its slot in A 32-frame effect actor riding on a parent: each frame it advances |
| `OvlFunc_881_200c004` | `0x0200c004` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in The MIRROR of OvlFunc_881_200bfb4, which sits immediately before i |
| `OvlFunc_882_200c378` | `0x0200c378` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_882_200c3c8` | `0x0200c3c8` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_883_200dcc4` | `0x0200dcc4` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_883_200dd14` | `0x0200dd14` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_884_2008030` | `0x02008030` | — | read | Turns an actor one step toward its target: takes the angle to the target with atan2, clamps the change to +/-0x1000 of a full circle, and ap |
| `OvlFunc_884_200a39c` | `0x0200a39c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_884_200a3ec` | `0x0200a3ec` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_885_2008030` | `0x02008030` | — | read | Turn one step toward the target, one of ten identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360/ov |
| `OvlFunc_886_2008030` | `0x02008030` | — | read | Split out of that .s; the sibling parts stay as assembly. Turn one step toward the target, one of eleven identical copies -- one per overlay |
| `OvlFunc_887_2008030` | `0x02008030` | — | read | Turn one step toward the target, one of ten identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360/ov |
| `OvlFunc_887_2008118` | `0x02008118` | — | read | A three-message prompt: says the opening line, runs a check, and delivers one of two follow-ups at base+1 or base+2. One of seven identical  |
| `OvlFunc_887_20093b4` | `0x020093b4` | — | read | Overlay 887: two map edits applied back to back. Split out of the seventeen-part chain at neighbouring parts stay as assembly and are listed |
| `OvlFunc_887_20095e8` | `0x020095e8` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_887_2009638` | `0x02009638` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_888_200a660` | `0x0200a660` | — | read | Overlay 888: detach slot 14's per-frame hook and park it at the origin. Split out of asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c.s; the neigh |
| `OvlFunc_895_2008030` | `0x02008030` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. GetEntrances, three-way form: selects one of three edge-transiti |
| `OvlFunc_896_2008314` | `0x02008314` | — | read | Turn one step toward the target, one of ten identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360/ov |
| `OvlFunc_897_200ae0c` | `0x0200ae0c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_897_200ae5c` | `0x0200ae5c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_898_200906c` | `0x0200906c` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_793768/overlay.ld, so the ROM layo |
| `OvlFunc_901_20084b4` | `0x020084b4` | — | read | Overlay 901: a talk stub taking its slot as an argument. Split out of asm/overlays/rom_797990/ovl_314_c_c_a_a_a.s; the neighbouring parts st |
| `OvlFunc_901_20084d8` | `0x020084d8` | — | read | Talk with staging: turns slot 8 toward the player, records save bit 0x305, and delivers line 0x1cab. __Func_809280c is deliberately left und |
| `OvlFunc_901_2008754` | `0x02008754` | — | read | Overlay 901: hold an actor still while it delivers a line. Whole-file conversion of asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_c.s -- i |
| `OvlFunc_901_20087d4` | `0x020087d4` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in The same shape as OvlFunc_901_2008754 in src/overlays/rom_797990/o |
| `OvlFunc_901_2008bf8` | `0x02008bf8` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_797990/overlay.ld, so the ROM layo |
| `OvlFunc_906_2008314` | `0x02008314` | — | read | GetEntrances. Picks one of two edge-transition tables from a gState halfword. Head of a 22-member family, the largest in the overlays. This  |
| `OvlFunc_906_2008350` | `0x02008350` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_906_20083e4` | `0x020083e4` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_907_2008030` | `0x02008030` | — | read | Turn one step toward the target, one of ten identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360/ov |
| `OvlFunc_907_2008088` | `0x02008088` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_907_200811c` | `0x0200811c` | — | read | GetEntrances, 4-way form. Returns a named global from at least one arm, which is why the family sweeps in batches 08-13 missed it -- they ma |
| `OvlFunc_907_2008198` | `0x02008198` | — | read | GetEntrances, four-way form: selects one of four edge-transition tables from a gState halfword, falling through to the last. One of a 24-mem |
| `OvlFunc_909_2008030` | `0x02008030` | — | read | Turn one step toward the target, one of ten identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360/ov |
| `OvlFunc_909_2008100` | `0x02008100` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_910_2008030` | `0x02008030` | — | read | Turn one step toward the target, one of eleven identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360 |
| `OvlFunc_910_2008154` | `0x02008154` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_910_20088e8` | `0x020088e8` | — | read | Overlay 910: play a sound, run a map edit, record it happened. Split out of asm/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c.s; the neighbouring p |
| `OvlFunc_911_2008114` | `0x02008114` | — | read | Split out of that .s; the sibling parts stay as assembly. Turn one step toward the target, one of eleven identical copies -- one per overlay |
| `OvlFunc_911_200816c` | `0x0200816c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_911_2008284` | `0x02008284` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_916_2008054` | `0x02008054` | — | read | .L12c0 is a 4-byte .lcomm in ovl_30_c_c_c_c_c.s, read as a pointer here and passed to two different overlay routines. The ROM keeps its ADDR |
| `OvlFunc_920_2008040` | `0x02008040` | — | read | GetEntrances, four-way form: selects one of four edge-transition tables from a gState halfword, falling through to the last. Head of a 24-me |
| `OvlFunc_920_20080a0` | `0x020080a0` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_920_2008280` | `0x02008280` | — | read | Overlay 920: react-and-turn cutscene stub for slot 0xF. Split out of asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a.s. One of three identical stub |
| `OvlFunc_920_20082ac` | `0x020082ac` | — | read | Overlay 920: react-and-turn cutscene stub for slot 0x10. Split out of asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a.s. One of three identical stu |
| `OvlFunc_920_20082d8` | `0x020082d8` | — | read | Overlay 920: react-and-turn cutscene stub for slot 0x11. Split out of asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a.s. One of three identical stu |
| `OvlFunc_921_20080d8` | `0x020080d8` | — | read | Split out of that .s; the sibling parts stay as assembly. Turn one step toward the target, one of eleven identical copies -- one per overlay |
| `OvlFunc_921_2008130` | `0x02008130` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_921_20081ec` | `0x020081ec` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_922_2008050` | `0x02008050` | — | read | tools/asmfacts.py, not inferred from the function count. GetEntrances, 8-way form: selects one of 8 per-area tables from the gState halfword |
| `OvlFunc_924_2008e20` | `0x02008e20` | — | read | GetEntrances, four-way form: selects one of four edge-transition tables from a gState halfword, falling through to the last. One of a 24-mem |
| `OvlFunc_924_2008e80` | `0x02008e80` | — | read | GetEntrances, 5-way form: selects one of 5 per-area tables from the gState halfword at +0x1C0, falling through to the last. THE EARLIER FAMI |
| `OvlFunc_924_2008f30` | `0x02008f30` | — | read | THE LAST MEMBER OF THE 24-FUNCTION FAMILY, and the only one that needed its .s split by hand. That .s held one function and FOURTEEN .incbin |
| `OvlFunc_924_200cf90` | `0x0200cf90` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_7ac2d8/overlay.ld, so the ROM layo |
| `OvlFunc_926_200834c` | `0x0200834c` | — | read | GetEntrances, two-way form. This one returns a NAMED GLOBAL from its first arm rather than a local `.L` table, which is why the earlier fami |
| `OvlFunc_927_2008ee0` | `0x02008ee0` | — | read | GetEntrances, four-way form: selects one of four edge-transition tables from a gState halfword, falling through to the last. One of a 24-mem |
| `OvlFunc_927_2008f40` | `0x02008f40` | — | read | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script, so the ROM layout does not move. |
| `OvlFunc_927_200a4ac` | `0x0200a4ac` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_930_200807c` | `0x0200807c` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_930_20080b8` | `0x020080b8` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_930_2009180` | `0x02009180` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_931_2008030` | `0x02008030` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. GetEntrances, three-way form: selects one of three edge-transiti |
| `OvlFunc_931_200811c` | `0x0200811c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_932_2008040` | `0x02008040` | — | read | Turn one step toward the target, one of eleven identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360 |
| `OvlFunc_932_2008388` | `0x02008388` | — | read | THE FIRST FUNCTION ELEVATED OUT OF THE pool-tell BLOCKER. The ROM loads 0x4d from the literal pool rather than building it with `mov r0, #0x |
| `OvlFunc_932_20083b4` | `0x020083b4` | — | read | Second of three identical stubs differing only in the pooled id -- see ovl_30_a_c_c_a_a_a_a_b.c for why _AREA_4f has to be a symbol rather t |
| `OvlFunc_932_20083e0` | `0x020083e0` | — | read | Third of three identical stubs differing only in the pooled id -- see ovl_30_a_c_c_a_a_a_a_b.c for why _AREA_51 has to be a symbol rather th |
| `OvlFunc_932_200b428` | `0x0200b428` | — | read | Overlay 932: branch on the player's height. Whole-file conversion of asm/overlays/rom_7b9cb4/ovl_30_a_c_c_c_a_c.s -- it holds only this func |
| `OvlFunc_934_2008d20` | `0x02008d20` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_934_2008dcc` | `0x02008dcc` | — | read | Overlay 934: apply a map edit at row 0xF. Split out of asm/overlays/rom_7bdeb0/ovl_d20_c_c_a.s. One of a pair differing only in the row -- 0 |
| `OvlFunc_934_2008de8` | `0x02008de8` | — | read | Overlay 934: apply a map edit at row 0x11. Split out of asm/overlays/rom_7bdeb0/ovl_d20_c_c_a.s. One of a pair differing only in the row --  |
| `OvlFunc_934_2009378` | `0x02009378` | — | read | Overlay 934: put slot 8 back to its idle animation, inside a cutscene. Split out of asm/overlays/rom_7bdeb0/ovl_1300_c.s; the neighbouring p |
| `OvlFunc_934_200969c` | `0x0200969c` | — | read | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script, so the ROM layout does not move. |
| `OvlFunc_935_2008030` | `0x02008030` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_935_200808c` | `0x0200808c` | — | read | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script, so the ROM layout does not move. |
| `OvlFunc_935_20080e0` | `0x020080e0` | — | read | GetEntrances, 4-way form. Returns a named global from at least one arm, which is why the family sweeps in batches 08-13 missed it -- they ma |
| `OvlFunc_935_20082e0` | `0x020082e0` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_936_2008180` | `0x02008180` | — | read | GetEntrances, 5-way form: selects one of 5 per-area tables from the gState halfword at +0x1C0, falling through to the last. THE EARLIER FAMI |
| `OvlFunc_936_2008240` | `0x02008240` | — | read | tools/asmfacts.py, not inferred from the function count. GetEntrances, 7-way form: selects one of 7 per-area tables from the gState halfword |
| `OvlFunc_936_20095e0` | `0x020095e0` | — | read | Overlay 936: set the display-offset flag on the player actor. Split out of asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c.s; the neighbouring pa |
| `OvlFunc_936_2009ea4` | `0x02009ea4` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in Points an actor at a turn target, gives it a randomised tick offse |
| `OvlFunc_937_2008030` | `0x02008030` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. GetEntrances, three-way form: selects one of three edge-transiti |
| `OvlFunc_938_2008030` | `0x02008030` | — | read | GetEntrances, two-way form. This one returns a NAMED GLOBAL from its first arm rather than a local `.L` table, which is why the earlier fami |
| `OvlFunc_938_20080a4` | `0x020080a4` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_941_2008044` | `0x02008044` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_942_2008040` | `0x02008040` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_942_2008b68` | `0x02008b68` | — | read | Overlay 942: hide an actor and mark it non-interactive. Split out of asm/overlays/rom_7c6bac/ovl_30_c_c_c.s; the neighbouring parts stay as  |
| `OvlFunc_943_200b380` | `0x0200b380` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_7c7b9c/overlay.ld, so the ROM layo |
| `OvlFunc_945_2008340` | `0x02008340` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_946_2008cc4` | `0x02008cc4` | — | read | GetEntrances, 6-way form: selects one of 6 per-area tables from the gState halfword at +0x1C0, falling through to the last. THE EARLIER FAMI |
| `OvlFunc_946_2008ec4` | `0x02008ec4` | — | read | tools/asmfacts.py, not inferred from the function count. GetEntrances, 6-way form: selects one of 6 per-area tables from the gState halfword |
| `OvlFunc_947_2009440` | `0x02009440` | — | read | tools/asmfacts.py, not inferred from the function count. GetEntrances, 6-way form: selects one of 6 per-area tables from the gState halfword |
| `OvlFunc_947_2009544` | `0x02009544` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in A six-word struct is filled by one routine and passed BY VALUE to  |
| `OvlFunc_948_20089f0` | `0x020089f0` | — | read | GetEntrances, 4-way form. Returns a named global from at least one arm, which is why the family sweeps in batches 08-13 missed it -- they ma |
| `OvlFunc_948_2008a50` | `0x02008a50` | — | read | GetEntrances, 4-way form. Returns a named global from at least one arm, which is why the family sweeps in batches 08-13 missed it -- they ma |
| `OvlFunc_948_2008ec8` | `0x02008ec8` | — | read | Overlay 948: the same idle-animation reset, for slot 15. Split out of asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_c.s; the remaining part |
| `OvlFunc_948_2008ee0` | `0x02008ee0` | — | read | GetEntrances, 4-way form, returning a named global from one arm. That is why the family sweeps in batches 08-13 missed it -- they matched on |
| `OvlFunc_951_2008044` | `0x02008044` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_951_2008074` | `0x02008074` | — | read | Split out of that .s; the sibling parts stay as assembly. A three-message prompt: says the opening line, runs a check, and delivers one of t |
| `OvlFunc_951_20080bc` | `0x020080bc` | — | read | A three-message prompt: says the opening line, runs a check, and delivers one of two follow-ups at base+1 or base+2. One of seven identical  |
| `OvlFunc_952_2008030` | `0x02008030` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_953_2008030` | `0x02008030` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. GetEntrances, three-way form: selects one of three edge-transiti |
| `OvlFunc_956_20081b4` | `0x020081b4` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in Registers OvlFunc_956_200804c as a task at priority 0xc80. THE LOC |
| `OvlFunc_957_2008a00` | `0x02008a00` | — | read | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script, so the ROM layout does not move. |
| `OvlFunc_958_2008cc0` | `0x02008cc0` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_958_2008d20` | `0x02008d20` | — | read | Split out of that .s; the sibling part stays as assembly and keeps its slot in goldensun/overlays/rom_7e636c/overlay.ld, so the ROM layout d |
| `OvlFunc_958_2008d88` | `0x02008d88` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in goldensun/overlays/rom_7e636c/overlay.ld, so the ROM layout |
| `OvlFunc_959_20089dc` | `0x020089dc` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_959_2008af8` | `0x02008af8` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_960_200834c` | `0x0200834c` | — | read | GetEntrances, 4-way form, returning a named global from one arm. That is why the family sweeps in batches 08-13 missed it -- they matched on |
| `OvlFunc_960_2008e5c` | `0x02008e5c` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_7eaf28/overlay.ld, so the ROM layo |
| `OvlFunc_961_2008068` | `0x02008068` | — | read | Split out of that .s; the _c part stays as assembly. A three-message prompt: says the opening line, runs a check, and delivers one of two fo |
| `OvlFunc_961_20080b0` | `0x020080b0` | — | read | A three-message prompt: says the opening line, runs a check, and delivers one of two follow-ups at base+1 or base+2. One of seven identical  |
| `OvlFunc_962_20081d4` | `0x020081d4` | — | read | Split out of that .s; the _a part stays as assembly. A three-message prompt: says the opening line, runs a check, and delivers one of two fo |
| `OvlFunc_963_2008040` | `0x02008040` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. GetEntrances, three-way form: selects one of three edge-transiti |
| `OvlFunc_963_20080e4` | `0x020080e4` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_964_2009270` | `0x02009270` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_964_200a370` | `0x0200a370` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_965_2008f58` | `0x02008f58` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_965_2008fdc` | `0x02008fdc` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_965_200a7a0` | `0x0200a7a0` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_966_20080c4` | `0x020080c4` | — | read | Split out of that .s; the _a and _c parts stay as assembly. A three-message prompt: says the opening line, runs a check, and delivers one of |
| `OvlFunc_967_200804c` | `0x0200804c` | — | read | GetEntrances, two-way form. This one returns a NAMED GLOBAL from its first arm rather than a local `.L` table, which is why the earlier fami |
| `OvlFunc_968_2008594` | `0x02008594` | — | read | Overlay 968: forward an actor's turn target, masked to its low nibble. Split out of asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_c.s; the neighbou |
| `OvlFunc_968_2008fbc` | `0x02008fbc` | — | read | Overlay 968: a talk sequence with a positioned speaker. Split out of asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_a_c.s; the neighbouring parts st |
| `OvlFunc_969_200a15c` | `0x0200a15c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_969_200a1ac` | `0x0200a1ac` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_970_20083c0` | `0x020083c0` | — | read | Overlay 970: record slot 0's height for later comparison. Split out of asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a.s. One of four near-identical  |
| `OvlFunc_970_20083dc` | `0x020083dc` | — | read | Overlay 970: record slot 1's height for later comparison. Split out of asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a.s. One of four near-identical  |
| `OvlFunc_970_20083f8` | `0x020083f8` | — | read | Overlay 970: record slot 3's height for later comparison. Split out of asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a.s. One of four near-identical  |
| `OvlFunc_970_2008414` | `0x02008414` | — | read | Overlay 970: record slot 2's height for later comparison. Split out of asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a.s. One of four near-identical  |
| `OvlFunc_971_2009050` | `0x02009050` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_7fb4a8/overlay.ld. Stops the curre |
| `OvlFunc_974_2008130` | `0x02008130` | — | read | Overlay 974: the first of the seven message-range dispatch stubs. Split out of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_a.s. See ovl_30_a_ |
| `OvlFunc_974_2008148` | `0x02008148` | — | read | Overlay 974: the second of the seven message-range dispatch stubs. Split out of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_a.s. See ovl_30_a |
| `OvlFunc_974_2008160` | `0x02008160` | — | read | Overlay 974: one of a family of seven near-identical dispatch stubs. Split out of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a.s; the _a and _ |
| `OvlFunc_974_2008180` | `0x02008180` | — | read | Overlay 974: four of the seven message-range dispatch stubs. Whole-part conversion of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_c.s -- it h |
| `OvlFunc_974_2008198` | `0x02008198` | — | read | Overlay 974: four of the seven message-range dispatch stubs. Whole-part conversion of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_c.s -- it h |
| `OvlFunc_974_20081b8` | `0x020081b8` | — | read | Overlay 974: four of the seven message-range dispatch stubs. Whole-part conversion of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_c.s -- it h |
| `OvlFunc_974_20081d8` | `0x020081d8` | — | read | Overlay 974: four of the seven message-range dispatch stubs. Whole-part conversion of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_c.s -- it h |

201 functions: 37 named, 164 read
