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
| `OvlFunc_879_20081c0` | `0x020081c0` | `LoadTitleSprite` | named | OvlFunc_879_20081c0  --  0x020081c0 Cut out of goldensun/asm/overlays/rom_779188/ovl_30_c_c_b.s. Loads the cursor sprite: allocate a scratch |
| `OvlFunc_880_20092c8` | `0x020092c8` | `Crc16Ccitt` | named | unsigned int OvlFunc_880_20092c8(unsigned int n, unsigned char *p) { unsigned int crc; unsigned int i; int j; crc = 0xffff; i = 0; if (n !=  |
| `OvlFunc_883_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the _c part stays as assembly and keeps its slot in FindEntityAtPosition. Scans entity slots 8..0x41 -- the map-object |
| `OvlFunc_883_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the _a part stays as assembly and keeps its slot in FillMapRectCollisionByte. Writes one byte into every cell of a rec |
| `OvlFunc_895_2008200` | `0x02008200` | `TalkPassageA` | named | TalkPassageA. Says line 0x1034 once the passage has been opened (save bit 0x81a), otherwise 0x1031 -- and in the not-yet-opened case, if the |
| `OvlFunc_895_200856c` | `0x0200856c` | `TrackBlock9West` | named | Slotted between ovl_30_c_c_a_c_a.o and the rest of the overlay. EIGHT NEAR-TWINS IN ONE OBJECT, AND THAT IS DELIBERATE. All eight need -fno- |
| `OvlFunc_895_20085ac` | `0x020085ac` | `TrackBlockAWest` | named | Slotted between ovl_30_c_c_a_c_a.o and the rest of the overlay. EIGHT NEAR-TWINS IN ONE OBJECT, AND THAT IS DELIBERATE. All eight need -fno- |
| `OvlFunc_895_20085ec` | `0x020085ec` | `TrackBlock9East` | named | Slotted between ovl_30_c_c_a_c_a.o and the rest of the overlay. EIGHT NEAR-TWINS IN ONE OBJECT, AND THAT IS DELIBERATE. All eight need -fno- |
| `OvlFunc_895_2008634` | `0x02008634` | `TrackBlockAEast` | named | Slotted between ovl_30_c_c_a_c_a.o and the rest of the overlay. EIGHT NEAR-TWINS IN ONE OBJECT, AND THAT IS DELIBERATE. All eight need -fno- |
| `OvlFunc_895_200867c` | `0x0200867c` | `TrackBlockBEast` | named | Slotted between ovl_30_c_c_a_c_a.o and the rest of the overlay. EIGHT NEAR-TWINS IN ONE OBJECT, AND THAT IS DELIBERATE. All eight need -fno- |
| `OvlFunc_895_20086c4` | `0x020086c4` | `TrackBlockCEast` | named | Slotted between ovl_30_c_c_a_c_a.o and the rest of the overlay. EIGHT NEAR-TWINS IN ONE OBJECT, AND THAT IS DELIBERATE. All eight need -fno- |
| `OvlFunc_895_200870c` | `0x0200870c` | `TrackBlockDEast` | named | Slotted between ovl_30_c_c_a_c_a.o and the rest of the overlay. EIGHT NEAR-TWINS IN ONE OBJECT, AND THAT IS DELIBERATE. All eight need -fno- |
| `OvlFunc_895_2008754` | `0x02008754` | `TrackBlockEEast` | named | Slotted between ovl_30_c_c_a_c_a.o and the rest of the overlay. EIGHT NEAR-TWINS IN ONE OBJECT, AND THAT IS DELIBERATE. All eight need -fno- |
| `OvlFunc_895_200892c` | `0x0200892c` | `SetupArea13` | named | The target was the FIRST of six functions, so there is no _a part. Area-entry fixups behind three independent flag guards. TWO SPELLINGS ARE |
| `OvlFunc_895_2009ac8` | `0x02009ac8` | `ShakeTask` | named | extern int L269c __asm__(".L269c"); extern int __Random(void); extern void __PlaySound(int id); extern void __Func_8012330(int a, int b, int |
| `OvlFunc_900_2008094` | `0x02008094` | `GreetSlot8` | named | tools/asmfacts.py. GreetSlot8. The slot-8 NPC's scripted greeting: turn toward slots 9 and 0xA in turn forty frames apart, speak a line, re- |
| `OvlFunc_902_2008204` | `0x02008204` | `TalkInventoryGate` | named | extern unsigned char *iwram_3001ebc; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __CutsceneWait(int n);  |
| `OvlFunc_902_2008570` | `0x02008570` | `SpawnTrackedObject` | named | Placed in the run in goldensun/overlays/rom_7987ac/overlay.ld. Byte-identical to OvlFunc_899_200c698 in overlays/rom_794ac0; this C is share |
| `OvlFunc_905_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_905_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_908_200835c` | `0x0200835c` | `TalkOnceThenRepeat` | named | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_912_2008030` | `0x02008030` | `ResetRecordArray` | named | Overlay 912: initialise a fifteen-entry slot table. Whole-file conversion of asm/overlays/rom_7a0010/ovl_30_a_a.s. |
| `OvlFunc_913_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_913_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_914_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_914_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_915_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_915_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_916_2008b8c` | `0x02008b8c` | `FindRegionContaining` | named | OvlFunc_916_2008b8c  --  asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c_a.s FindRegionContaining: walk 0x0c-byte records until the halfword at +0  |
| `OvlFunc_919_200815c` | `0x0200815c` | `ApplyRasterBlend` | named | confirmed data-free by split_s.py. From the branch-over-pool class; the pool emitted at the ROM's position unaided, with the pool words in t |
| `OvlFunc_919_200826c` | `0x0200826c` | `RasterSplitHandler` | named | Picks one of two scroll values by a VCOUNT threshold and writes it to BG3HOFS. A LEAF FUNCTION -- no calls at all, which is why it was reach |
| `OvlFunc_919_20082a0` | `0x020082a0` | `UpdateRasterSplit` | named | OvlFunc_919_20082a0, the whole of goldensun/asm/overlays/rom_7a67d8/ovl_30_c_a_c_c.s. no linker-script change was needed. UpdateRasterSplit  |
| `OvlFunc_920_2008214` | `0x02008214` | `RevealSlotF` | named | OvlFunc_920_2008214  --  0x02008214, cut from the tail of RevealSlotF: teleport slot 8 to the origin, set a save bit, and after a forty-fram |
| `OvlFunc_920_20084b4` | `0x020084b4` | `SetupArea31` | named | Slotted between ovl_30_c_c_a_a_a_a.o and the rest of the overlay. Stack-arg-pair lever, standard form: both values named, in the order the R |
| `OvlFunc_923_2008350` | `0x02008350` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_923_2008528` | `0x02008528` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_924_2008350` | `0x02008350` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_924_2008528` | `0x02008528` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_924_200a844` | `0x0200a844` | `PaletteFadeToWhite` | named | extern void __WaitFrames(int n); void OvlFunc_924_200a844(void) { volatile unsigned short *p; unsigned int n; unsigned int i; int r, g, b; d |
| `OvlFunc_924_200ae08` | `0x0200ae08` | `PaletteFadeToWhite` | named | extern void __WaitFrames(int n); void OvlFunc_924_200ae08(void) { volatile unsigned short *p; unsigned int n; unsigned int i; int r, g, b; d |
| `OvlFunc_927_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_927_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_929_2008524` | `0x02008524` | `TalkStaged` | named | TalkStaged. Slot 9 delivers a line, turns to face slot 0x0a for sixty frames, turns back to slot 0 for twenty, then delivers a closing line. |
| `OvlFunc_934_2008350` | `0x02008350` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_934_2008528` | `0x02008528` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_937_20081fc` | `0x020081fc` | `ShopCounter` | named | A sanctum attendant, in the SHIFTED-ADDITIVE variant of the facing test: ldr r2, =0x5fff / add r3, r2 / ldr r2, =0x3ffe0000 / lsl r3, #16 /  |
| `OvlFunc_937_2008240` | `0x02008240` | `ClearSlotFlags` | named | OvlFunc_937_2008240  --  0x02008240 ONE OF THE MAP-EXIT FAMILY. A search of every .s for the opening `push {r5,r6,r7,lr} / ldr r3, =iwram_30 |
| `OvlFunc_937_200833c` | `0x0200833c` | `StageArea64` | named | Code to this file, the trailing .section .data to its _c sibling. OK on the first screen, on two reads. IT RETURNS void. The epilogue is `ad |
| `OvlFunc_946_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_946_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_947_2008350` | `0x02008350` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_947_2008528` | `0x02008528` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script. FillMapRectCollisionByte, one of |
| `OvlFunc_948_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_948_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_950_20087b0` | `0x020087b0` | `TalkTwoBits` | named | extern int _MSG_2399; extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __Cutscene |
| `OvlFunc_957_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_957_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_958_2008350` | `0x02008350` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_958_2008528` | `0x02008528` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_959_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_959_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_964_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_964_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_964_2009038` | `0x02009038` | `WaitForField` | named | translation unit and the linker script is untouched. Waits up to 0x3c frames for the actor's field at +0xc to fall to or below the one at +0 |
| `OvlFunc_965_200806c` | `0x0200806c` | `FindEntityAtPosition` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FindEntityAtPosition, one of s |
| `OvlFunc_965_2008244` | `0x02008244` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_968_20084f4` | `0x020084f4` | `FillMapRectCollisionByte` | named | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. FillMapRectCollisionByte, one  |
| `OvlFunc_970_2008168` | `0x02008168` | `DecayCounter` | named | A LEAF FUNCTION, matched on the first screen. Ticks a counter and, every 0x28 ticks, decrements a second value down to a floor of 4 and rese |
| `OvlFunc_common0_0` | `` | `SetActorFacingBits` | named | First in the common0 run, ahead of the _a_b piece, in all NINETEEN overlay.ld scripts. Writes a two-bit selector into bits 2-3 of the sprite |
| `OvlFunc_common0_18` | `` | `SpawnPropForeground` | named | of the same .s in goldensun/overlays/rom_7b4558/overlay.ld. ONE OF FOUR OPERAND-IDENTICAL COPIES, elevated together from a single source wit |
| `OvlFunc_common0_70` | `` | `SpawnPropBackground` | named | OvlFunc_common0_70  --  asm/overlays/common/common0_a_b.s Create an actor and initialise it; returns 0 if the spawn failed. TWO THINGS DECID |
| `OvlFunc_common0_d4` | `` | `StepPropPhysics` | named | Slotted between the _a and _c pieces in all NINETEEN overlay.ld scripts that name this object. The _c piece keeps the .data and .data1 secti |
| `OvlFunc_common1_1254` | `` | `RunTurnAnimation` | named | extern unsigned char iwram_3001e68; extern unsigned char gState[]; extern int L49 __asm__(".L49"); extern int L20 __asm__(".L20"); extern in |
| `OvlFunc_common1_1490` | `` | `StartSpriteEffect` | named | extern short L45[] __asm__(".L45"); extern short L29[] __asm__(".L29"); extern short L19[] __asm__(".L19"); extern short L26[] __asm__(".L26 |
| `OvlFunc_common1_172c` | `` | `MaybeSpawnDustPuff` | named | OvlFunc_common1_172c  --  shared by ovl_7db0c8, ovl_7ddb88 and ovl_7e0928 keeps its name (`asm/%.o: src/%.c`) and all three overlay linker s |
| `OvlFunc_common1_17c0` | `` | `TeleportPlayerToProp` | named | OvlFunc_common1_17c0  --  common overlay, +0x17c0 Cut out of goldensun/asm/overlays/common/common1_c_a_c_c_a.s. Sends the actor whose slot i |
| `OvlFunc_common1_1fb4` | `` | `StartPuzzle` | named | Slotted between common1_c_a_c_c_a.o and the rest of the overlay. THE 1 STORED INTO THE FIRST TWO HALFWORDS IS A NAMED int, and both defects  |
| `OvlFunc_common1_2018` | `` | `FindEntityAtPosition` | named | First in the run, ahead of the _b piece, in the THREE overlay.ld scripts that name this object; the _b piece keeps the .data, .data1 and .bs |
| `OvlFunc_common1_3e4` | `` | `RunPartySizeBranch` | named | extern unsigned char *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern int __GetPar |
| `OvlFunc_common1_e10` | `` | `StartTransitionEffect` | named | extern short L33[] __asm__(".L33"); extern short L22[] __asm__(".L22"); extern short L36[] __asm__(".L36"); extern short L46[] __asm__(".L46 |
| `OvlFunc_common1_ea0` | `` | `RunDoorSequence` | named | so all three linker scripts list the three parts. A cutscene with a short first arm for argument 0 and a long one otherwise. L11 IS DECLARED |
| `ActorAttrOp_unk64` | `` | — | read | ActorAttrOp_waitTimer, ActorAttrOp_unk64 and ActorAttrOp_unk66 -- the whole of goldensun/asm/rom_9000/rom_e220_c_a.s. no linker-script chang |
| `ActorAttrOp_unk66` | `` | — | read | ActorAttrOp_waitTimer, ActorAttrOp_unk64 and ActorAttrOp_unk66 -- the whole of goldensun/asm/rom_9000/rom_e220_c_a.s. no linker-script chang |
| `ActorAttrOp_waitTimer` | `` | — | read | ActorAttrOp_waitTimer, ActorAttrOp_unk64 and ActorAttrOp_unk66 -- the whole of goldensun/asm/rom_9000/rom_e220_c_a.s. no linker-script chang |
| `ActorAttrOp_width` | `` | — | read | Actor attribute opcode: collision radius. Whole-file conversion of asm/rom_9000/rom_e220_a_c.s -- one function, so the ROM layout is preserv |
| `ActorCmd_CallNative` | `` | — | read | Actor script VM: the opcode that calls a native predicate. Whole-file conversion of asm/rom_9000/rom_d654_a_c_a_a_a_a.s -- one function, so  |
| `ActorCmd_CmpAttr` | `` | — | read | The remaining part of that .s held only this function and no data. Script opcode: compare an actor attribute. Identical to ActorCmd_SetAttr  |
| `ActorCmd_GotoIfNZ` | `` | — | read | Actor script VM: the two conditional-jump opcodes. Whole-file conversion of asm/rom_9000/rom_d654_a_c_a_a_c.s -- it holds both of these and  |
| `ActorCmd_GotoIfZ` | `` | — | read | Actor script VM: the two conditional-jump opcodes. Whole-file conversion of asm/rom_9000/rom_d654_a_c_a_a_c.s -- it holds both of these and  |
| `ActorCmd_IncAttr` | `` | — | read | Split out of that .s; the sibling parts stay as assembly. Script opcode: increment an actor attribute. Identical to ActorCmd_SetAttr but for |
| `ActorCmd_SetAttr` | `` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in Script opcode: set an actor attribute. Reads a field id from scrip |
| `ActorCmd_SetScript` | `` | — | read | Actor script VM: the opcode that makes the script jump to a new base. Whole-file conversion of asm/rom_9000/rom_ca2c_a.s -- one function, so |
| `Actor_IsNotMoving` | `` | — | read | True when the actor has no move in progress. Which axes count depends on the flag at 0x55: with it clear all three targets must be idle, wit |
| `Actor_SetAnimAndSpeed` | `` | — | read | extern void Sprite_SetAnimSpeed(void *s, int a); void Actor_SetAnimAndSpeed(unsigned char *e, int anim, int speed) { void **list; void *s; i |
| `Actor_SetAnimSpeed` | `` | — | read | The speed-setting sibling of src/rom_9000/rom_c004_c_a_a_a_a_b.c: identical control flow, calling Sprite_SetAnimSpeed instead of Sprite_SetA |
| `Actor_SetBehavior` | `` | — | read | Actor_SetBehavior  --  0x08093a6c Cut out of goldensun/asm/rom_8a000/rom_93304_a_c_c_c.s. Gives an actor one of seven stock behaviour script |
| `AddMenuBarOption` | `` | — | read | struct Ui { unsigned char pad0[0x8e]; short count; }; extern unsigned char *iwram_3001f38; extern int AllocSpriteSlot(int id); extern void L |
| `AllocGlobal1F` | `` | — | read | Slotted between rom_8d9a4_c_c_a_c_a.o and the rest of stage1.ld. Allocates a 0x540-byte block in EWRAM, zeroes it with DMA3_CLEAR, and retur |
| `Anim_Func` | `` | — | read | Three tagged allocations, a dispatch through a function-pointer table, and the three frees in reverse order.  Two readings settled it. THE I |
| `Anim_Kite` | `` | — | read | Anim_Kite  --  0x080e6948 Cut out of goldensun/asm/rom_c9000/rom_e6638_a.s; the other five functions stay as assembly beside it. One of the  |
| `Anim_Summon` | `` | — | read | Anim_Summon  --  0x080d6578 Cut out of goldensun/asm/rom_c9000/rom_d6504_a_c.s. Allocates the three summon-animation buffers, dispatches on  |
| `BreakItem` | `` | — | read | BreakItem  --  0x08078a34, was goldensun/asm/rom_77000/rom_78414_c_c_c_a.s. script's existing line for that object now picks up this file's. |
| `Camera_SetTarget` | `` | — | read | Behaviour: install the 0x135F0 script, optionally retuning the movement. Whole-file conversion of asm/rom_9000/rom_c004_c_a_a_c_a_c_c_c_c_c. |
| `CanEquipItem` | `` | — | read | Equipment: may this unit's class use this item? Whole-file conversion of asm/rom_77000/rom_78414_a_c.s -- one function, so the ROM layout is |
| `CheckLure` | `` | — | read | extern unsigned char gState[]; extern void ClearFlag(int id); extern void SetFlag(int id); extern int GetPartySize(void); extern unsigned ch |
| `CreateParticleActor` | `` | — | read | CreateParticleActor  --  0x08096c80 The function half of goldensun/asm/rom_8a000/rom_944ec_c_c.s; the four .rodata blobs stay behind in rom_ |
| `Debug_TestEquipAndStatus` | `` | — | read | extern unsigned char gState[]; extern int _GiveItemTo(int who, int item); extern int _EquipItem(int who, int slot); extern unsigned char *_G |
| `DecompressStatusIcon` | `` | — | read | Placed in the run in goldensun/stage1.ld. Points the icon block at one of the compressed status icons and asks LoadIcon to unpack it, with b |
| `DeleteActor` | `` | — | read | #include "dma.h" extern void DeleteSprite(void *s); void DeleteActor(unsigned char *e) { void **list; void *s; int i; if (e == 0) return; sw |
| `DeleteSpriteLayer` | `` | — | read | Slotted between rom_b798_c_c_a.o and the rest of stage1.ld. Zeroes a 0x18-byte sprite-layer record, if the pointer is non-null, with DMA3_CL |
| `DialogueBox` | `` | — | read | extern unsigned char *iwram_3001e8c; extern int BufferString(int id, int mode); extern void Func_801868c(int n, int a, int b, int c, int e,  |
| `Field_Frost_Target` | `` | — | read | Placed in the run in goldensun/stage1.ld. The targeted form of the field Frost psynergy: if a target actor exists, sets a flag on the caster |
| `Field_Growth` | `` | — | read | struct Obj { int f0; int x; int y; int z; }; struct PActor { unsigned char pad00[0x55]; unsigned char f55; }; extern unsigned int iwram_3001 |
| `Field_Growth_Target` | `` | — | read | Slotted between rom_97b54_a_c_a_a.o and the rest of stage1.ld. THE READ-MODIFY-WRITE AT +0x23 HAS TO BE SPELLED OUT COMPLETELY -- pointer, l |
| `Field_MindRead` | `` | — | read | #include "dma.h" struct T { unsigned char pad[0x290]; unsigned short f290; unsigned short f292; }; extern void *galloc_ewram(int tag, int si |
| `Func_8003e10` | `0x08003e10` | — | read | Func_8003e10, the whole of goldensun/asm/rom_c0/rom_3d04_c.s. so no linker-script change was needed. Copies the ARM routine Func_8001dc8 int |
| `Func_8005810` | `0x08005810` | — | read | extern unsigned char *iwram_3001f1c; extern int Func_8005b24(void); extern int Random(void); int Func_8005810(void) { int v[16]; unsigned ch |
| `Func_80058ac` | `0x080058ac` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" extern unsigned char *iwram_3001f1c; extern void ReadFlash(unsigned short sector |
| `Func_8005a78` | `0x08005a78` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" extern unsigned char *iwram_3001f1c; extern unsigned int Func_8005b24(void); ext |
| `Func_8005b64` | `0x08005b64` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" struct SoundChan { unsigned char f0[7]; unsigned char f7; unsigned short f8; uns |
| `Func_80060e8` | `0x080060e8` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" struct SndState { 0x00 |
| `Func_800be20` | `0x0800be20` | — | read | extern unsigned char *_GetSpriteInfo(int id); int Func_800be20(int id, unsigned int idx, int count) { unsigned char *info; unsigned char *ip |
| `Func_800be70` | `0x0800be70` | — | read | struct SpriteSlot { unsigned short size; unsigned short vramOffset; }; extern struct SpriteSlot gSpriteSlots[]; extern unsigned char scrambl |
| `Func_800c548` | `0x0800c548` | — | read | Func_800c548 and Func_800c570, the whole of goldensun/asm/rom_9000/rom_c004_c_a_c_a.s. no linker-script change was needed. Two guarded bit-s |
| `Func_800c570` | `0x0800c570` | — | read | Func_800c548 and Func_800c570, the whole of goldensun/asm/rom_9000/rom_c004_c_a_c_a.s. no linker-script change was needed. Two guarded bit-s |
| `Func_800c5b4` | `0x0800c5b4` | — | read | Func_800c5b4  --  0x0800c5b4, was goldensun/asm/rom_9000/rom_c004_c_a_c_c_a.s. script's existing line for that object now picks up this file |
| `Func_800d304` | `0x0800d304` | — | read | Slotted between the _a and _c pieces in goldensun/stage1.ld; the _c piece keeps the .rodata blob. Copies an ARM routine into RAM, runs it, a |
| `Func_800d924` | `0x0800d924` | — | read | extern unsigned char *iwram_3001e64; extern int Func_800eba0(void *p, int x, int b, int y); int Func_800d924(unsigned char *a, int b) { unsi |
| `Func_800d98c` | `0x0800d98c` | — | read | extern unsigned char *iwram_3001e64; extern int Func_800eba0(void *p, int x, int b, int y); unsigned char *Func_800d98c(unsigned char *a, in |
| `Func_800ea60` | `0x0800ea60` | — | read | extern int _GetFlag(int); extern unsigned char *iwram_3001ebc; extern volatile unsigned int gKeyPress; int Func_800ea60(unsigned int arg) {  |
| `Func_800eba0` | `0x0800eba0` | — | read | int Func_800eba0(int *a, int ra, int *b, int rb) { int dx; int dy; int dz; int r; int lim; dx = (*a++ - *b++) >> 16; dy = (*a++ - *b++) >> 1 |
| `Func_8010704` | `0x08010704` | — | read | extern unsigned int gBuffer[]; void Func_8010704(int sx, int sy, int w, int h, int dx, int dy) { unsigned int *src; unsigned int *dst; unsig |
| `Func_8011590` | `0x08011590` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" extern unsigned char *iwram_3001e6c[]; extern unsigned int iwram_3001e40; extern |
| `Func_8011b00` | `0x08011b00` | — | read | extern char *galloc_ewram(int tag, int size); void Func_8011b00(void) { char *base; char *p; unsigned short i; unsigned short j; int off; in |
| `Func_8012038` | `0x08012038` | — | read | Appended after the _a piece in goldensun/stage1.ld. GetTileFlags / SetTileFlags -- the read and write halves of one accessor pair over the l |
| `Func_8012078` | `0x08012078` | — | read | Appended after the _a piece in goldensun/stage1.ld. GetTileFlags / SetTileFlags -- the read and write halves of one accessor pair over the l |
| `Func_8012204` | `0x08012204` | — | read | typedef unsigned char u8; #define ewram_202c800 ((u8 *)0x202c800) #define ewram_202c000 ((u8 *)0x202c000) int Func_8012204(int *p) { int zc, |
| `Func_80122c8` | `0x080122c8` | — | read | extern unsigned char ewram_2020000[]; extern unsigned char L1353c[] __asm__(".L1353c"); extern int Func_8012204(int *v); int Func_80122c8(in |
| `Func_8012d70` | `0x08012d70` | — | read | extern unsigned char *iwram_3001e60; extern unsigned char *_GetSpriteInfo(int id); void Func_8012d70(int group, int anim) { unsigned char *b |
| `Func_8016230` | `0x08016230` | — | read | extern unsigned char *iwram_3001e8c; extern void Func_80170f8(int a, int b, int c, int d); extern void Func_8017248(int a, int b, int c, int |
| `Func_801671c` | `0x0801671c` | — | read | Fills the 0xF00-byte text scratch at 0x6002500 with 0 (transparent). Func_80008d8 is an ARM routine in IWRAM, so the call goes through a fun |
| `Func_8016738` | `0x08016738` | — | read | Twin of Func_801671c next door, differing only in the fill value: this one writes 0x44444444 into the same 0xF00-byte text scratch at 0x6002 |
| `Func_8016758` | `0x08016758` | — | read | extern char *iwram_3001e8c; extern void Func_801671c(void); void Func_8016758(void) { char *p; char *found; char *rec; int i; int z, f, a; p |
| `Func_80167ac` | `0x080167ac` | — | read | Slotted after asm/rom_15000/rom_15e8c_a_c_c_c_c_c_a.o in goldensun/stage1.ld. RECOVERED FROM A PARK BY TYPING, and it was TWO INSTRUCTIONS S |
| `Func_8016868` | `0x08016868` | — | read | typedef unsigned char u8; typedef unsigned short u16; struct MsgRec { u8 pad00[0x12]; u16 pending; u16 unk14; u16 flags; int busy; }; struct |
| `Func_8017004` | `0x08017004` | — | read | typedef unsigned char u8; typedef unsigned short u16; struct Win { u8 pad0[8]; u16 f8; u16 fa; u16 fc; u16 fe; u8 pad10[8]; short f18; short |
| `Func_80173ac` | `0x080173ac` | — | read | Placed in the run in goldensun/stage1.ld. Seeds five halfwords of the text/window block: the colour at +0xeae, a width at +0xea8, a count at |
| `Func_8017a64` | `0x08017a64` | — | read | Placed in the run in goldensun/stage1.ld. Measures the pixel width of a NUL-terminated halfword string: a space is 4, anything above 0xff is |
| `Func_8019000` | `0x08019000` | — | read | extern unsigned int iwram_3001e8c; struct Win { unsigned char pad00[8]; unsigned short w; unsigned short h; unsigned short x; unsigned short |
| `Func_80197c4` | `0x080197c4` | — | read | extern unsigned char *iwram_3001e8c; extern void CloseUIBox(void *box, int mode); extern void WaitFrames(int n); void Func_80197c4(void) { u |
| `Func_8019854` | `0x08019854` | — | read | struct R { unsigned char pad00[8]; unsigned short f8; unsigned short fa; unsigned short fc; unsigned short fe; unsigned char pad10[2]; unsig |
| `Func_801999c` | `0x0801999c` | — | read | extern int iwram_3001e8c; extern volatile int gKeyHeld; extern int _Func_80f954c(void); int Func_801999c(unsigned char *p) { char *g; int f; |
| `Func_80199ec` | `0x080199ec` | — | read | extern int iwram_3001e8c; extern int iwram_3001af8; extern volatile int gKeyPress; extern int _Func_80f954c(void); int Func_80199ec(unsigned |
| `Func_8019a54` | `0x08019a54` | — | read | struct Box { unsigned char pad0[0x14]; unsigned short f14; unsigned short f16; int f18; }; extern unsigned char *iwram_3001e8c; extern void  |
| `Func_8019e48` | `0x08019e48` | — | read | extern char *iwram_3001e8c; extern int GetPortrait(void); extern void CloseUIBox(void *p, int n); void Func_8019e48(void) { char *b; char *p |
| `Func_801a778` | `0x0801a778` | — | read | extern int iwram_3001e98; void Func_801a778(void) { char *p; unsigned short *f; int zero; p = (char *)iwram_3001e98; zero = 0; (int *)(p + 0 |
| `Func_801a7c0` | `0x0801a7c0` | — | read | Slotted between the _a and _c pieces in goldensun/stage1.ld. PushScreenEntry -- appends one (a, b) pair to two parallel sixteen-entry short  |
| `Func_801b148` | `0x0801b148` | — | read | Teardown: close the box, walk two linked lists silencing every node that has a live handle, then free the block.  Matched on the second scre |
| `Func_801b398` | `0x0801b398` | — | read | A modal input loop, and it turns entirely on WHICH loop-invariant address gcc is allowed to hoist.  The ROM hoists exactly one of the two an |
| `Func_801c188` | `0x0801c188` | — | read | #include "gba/types.h" struct Panel { u8  pad_00[8]; u16 kind;     /* 0x08 |
| `Func_801c21c` | `0x0801c21c` | — | read | UI panels: release a panel's OBJ tiles. Split out of asm/rom_15000/rom_1aeec_c_a_a_a_a_a_c_a_a.s; the neighbours are ROM layout is unchanged |
| `Func_801c46c` | `0x0801c46c` | — | read | Placed in the run in goldensun/stage1.ld. Steps a counter byte at gState+0x205 one way or the other depending on bit 5 of its argument. TWO  |
| `Func_801c9c8` | `0x0801c9c8` | — | read | struct Slot { unsigned char pad00[0xa]; short flag; unsigned char pad0c[0x34 - 0xc]; }; struct Menu { unsigned char pad000[0x400]; struct Sl |
| `Func_801ca1c` | `0x0801ca1c` | — | read | typedef struct { unsigned char b[704]; } GlobalState; extern GlobalState gState; extern unsigned char tbl[] __asm__(".L36750"); void Func_80 |
| `Func_801cae0` | `0x0801cae0` | — | read | typedef unsigned short u16; typedef volatile unsigned short vu16; extern u16 Func_801cbd4(void *t, unsigned int a, unsigned int b, unsigned  |
| `Func_801cbd4` | `0x0801cbd4` | — | read | extern int Func_8000888(int, int); static inline int call_via(int (*f)(int, int), int a, int b) { register int _a __asm__("r0") = a; registe |
| `Func_801d980` | `0x0801d980` | — | read | Slotted between rom_1ca1c_c_c_a_a.o and the rest of stage1.ld. Allocates a 0x628-byte EWRAM block, zeroes it with DMA3_CLEAR, and starts a t |
| `Func_801e260` | `0x0801e260` | — | read | extern unsigned char *iwram_3001e8c; void Func_801e260(int x, int y, unsigned int w, unsigned int h) { unsigned char *base; unsigned short * |
| `Func_801e7c0` | `0x0801e7c0` | — | read | struct P { unsigned char pad00[0xc]; unsigned short fc; unsigned short fe; }; extern unsigned char *iwram_3001e8c; extern void BufferString( |
| `Func_801e858` | `0x0801e858` | — | read | extern void *Func_8004970(int size); extern void Func_8017aa4(void *buf, int b, int c, int d); extern void free(void *p); void Func_801e858( |
| `Func_801e8b0` | `0x0801e8b0` | — | read | typedef unsigned char u8; typedef unsigned short u16; struct W { unsigned char pad0[0xc]; u16 fc; u16 fe; }; extern u16 *Func_8004970(int si |
| `Func_801ebd8` | `0x0801ebd8` | — | read | Func_801ebd8  --  0x0801ebd8 The whole of goldensun/asm/rom_15000/rom_1de5c_c_c_c_a_a_c_a.s, which held this function and nothing else, so t |
| `Func_801ee68` | `0x0801ee68` | — | read | Slotted between the _a and _c pieces in goldensun/stage1.ld. Fills a rectangle of halfwords in VRAM at 0x6002000 with a value passed on the  |
| `Func_801eea0` | `0x0801eea0` | — | read | extern unsigned char *iwram_3001e90; extern int _Func_80b6a60(int n); extern int _GetPartySize(void); void Func_801eea0(int flags) { unsigne |
| `Func_801ef08` | `0x0801ef08` | — | read | Func_801ef08  --  0x0801ef08 Cut out of goldensun/asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c.s. Builds one UI box: allocate the descriptor, rai |
| `Func_801f5f0` | `0x0801f5f0` | — | read | struct S { unsigned char pad00[0xc]; unsigned short fc; unsigned short fe; }; extern unsigned char *iwram_3001e8c; void Func_801f5f0(struct  |
| `Func_801f9b4` | `0x0801f9b4` | — | read | typedef unsigned char u8; extern u8 ewram_2002004[]; extern u8 ewram_2000000[]; extern int _MSG_0a; extern int _MSG_0b; extern int Func_8005 |
| `Func_801fda8` | `0x0801fda8` | — | read | extern int iwram_3001e8c; void Func_801fda8(void *a, int x, int y, int w, int h) { char *base; int off; int i; unsigned short *p; base = (ch |
| `Func_801ff14` | `0x0801ff14` | — | read | .rodata line is repointed there. THE LEVER IS WHICH OPERAND IS THE POINTER. The ROM addresses the table with the OFFSET first: ldr r0, [r5,  |
| `Func_8020088` | `0x08020088` | — | read | .rodata line is repointed there. A structural twin of src/rom_15000/rom_1fe2c_b.c (Func_801ff14): the same sprite-slot teardown loop with 0x |
| `Func_8020a60` | `0x08020a60` | — | read | struct Win { unsigned char pad00[0xc]; unsigned short fc; unsigned short fe; }; extern unsigned char *iwram_3001e8c; void Func_8020a60(struc |
| `Func_8020b14` | `0x08020b14` | — | read | extern char *iwram_3001e8c; extern void Func_8018850(int a, int *b, int *c, int d); int Func_8020b14(unsigned char *s) { char *p; unsigned s |
| `Func_8021360` | `0x08021360` | — | read | Slotted between rom_20198_c_c_c_a_a_a_a.o and the rest of stage1.ld. Picks a halfword from one of two tables by index, choosing the table on |
| `Func_8021848` | `0x08021848` | — | read | extern int L37250[] __asm__(".L37250"); extern void Func_80008d8(int *dst, int n, int v); typedef void (*Fn)(int *dst, int n, int v); int Fu |
| `Func_8021a18` | `0x08021a18` | — | read | extern unsigned short _TBL_372c0[] __asm__(".L372c0"); void Func_8021a18(unsigned char *dst0) { unsigned short *dp; unsigned short *src; uns |
| `Func_8021c34` | `0x08021c34` | — | read | Slotted between rom_20198_c_c_c_c_a_a.o and the rest of stage1.ld. Creates a UI box, draws one string into it, and RETURNS THE BOX. The retu |
| `Func_8025180` | `0x08025180` | — | read | Func_8025180  --  0x08025180 Cut out of goldensun/asm/rom_15000/rom_23178_a_a_a_a_a.s. Classifies an item for the equip menu: 1 means "not u |
| `Func_80251d4` | `0x080251d4` | — | read | Slotted between rom_23178_a_a_a_a_a.o and the rest of stage1.ld. Copies 0x20 bytes between two VRAM tile slots. A leaf -- no prologue at all |
| `Func_80284dc` | `0x080284dc` | — | read | Slotted between rom_23178_a_a_a_a_c_a.o and the rest of stage1.ld. Allocates a 0x98-byte EWRAM block, zeroes it, starts a task on it and ret |
| `Func_802851c` | `0x0802851c` | — | read | A UI teardown: stop the task, close the box if one is open, release each of `count` entries, free the tag, wait a frame. THE LOOP MUST BE WR |
| `Func_80289e8` | `0x080289e8` | — | read | extern signed char L3740f[] __asm__(".L3740f"); extern short ewram_200200c; extern short ewram_2002010; extern int Func_801f77c(int a); exte |
| `Func_8028aa8` | `0x08028aa8` | — | read | extern unsigned char *iwram_3001f38; extern int _MSG_c7b; extern void Func_80164d4(void *w, int a, int b, int c, int d); extern void Func_80 |
| `Func_8028b80` | `0x08028b80` | — | read | extern unsigned char *iwram_3001f38; extern void Func_8016478(void *w); extern void DrawSmallText(int id, void *w, int x, int y); void Func_ |
| `Func_80782a0` | `0x080782a0` | — | read | void Func_80782a0(void *unit, int n) { void *r5; int r0; int r1; int r2; int r3; r5 = unit; r2 = 0x34; r3 = *(short *)((char *)r5 + r2); r0  |
| `Func_8078320` | `0x08078320` | — | read | void Func_8078320(void *unit, int n) { void *r5; int r0; int r1; int r2; int r3; r5 = unit; r2 = 0x36; r3 = *(short *)((char *)r5 + r2); r2  |
| `Func_8078500` | `0x08078500` | — | read | extern unsigned int gState; extern int FindEmptyInventorySlot(int id); extern int Func_80796c4(short *buf); int Func_8078500(void) { short b |
| `Func_807882c` | `0x0807882c` | — | read | Func_807882c  --  0x0807882c, from asm/rom_77000/rom_78414_c_c_a_c_a.s. The same scan as GetEquippedItem next door, but it returns the item  |
| `Func_8078948` | `0x08078948` | — | read | PARKED, AND NOW MATCHED, on the same lever as LoadStatusIcon. It sat at 22 of 23 instructions with one pair swapped in the argument setup fo |
| `Func_8079754` | `0x08079754` | — | read | A LEAF FUNCTION, matched on the first screen. Adds a delta to the signed byte at gState+0x11c, clamps it to 0..0x1c, stores it back and retu |
| `Func_80797fc` | `0x080797fc` | — | read | extern void *GetEnemyInfo(int id); extern void *GetPCBaseStats(int id); extern unsigned char L88e38[] __asm__(".L88e38"); int Func_80797fc(i |
| `Func_80798b4` | `0x080798b4` | — | read | Slotted between rom_79460_c_c_c_a_a.o and the rest of stage1.ld. Looks up an enemy's row in a 24-byte-stride table and returns its first wor |
| `Func_80798e0` | `0x080798e0` | — | read | extern void *GetUnit(int id); extern void *GetEnemyInfo(int id); extern void Func_80797fc(int a, unsigned char *b, int *out); struct EnemyRo |
| `Func_8079c8c` | `0x08079c8c` | — | read | Slotted between rom_79460_c_c_c_c_a_c_c_a_a.o and the rest of stage1.ld. BRANCH POLARITY. The non-null path is the FALL-THROUGH: written as  |
| `Func_8079d1c` | `0x08079d1c` | — | read | Func_8079d1c  --  0x08079d1c Cut out of goldensun/asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_c.s. Rolls for a weapon's unleash: the chance is t |
| `Func_8079d7c` | `0x08079d7c` | — | read | Func_8079d7c  --  0x08079d7c Cut out of goldensun/asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_c_c.s. Maps a status or class id to a numeric weig |
| `Func_8079e9c` | `0x08079e9c` | — | read | extern unsigned char *GetEnemyInfo(int id); extern unsigned char *GetClassInfo(int id); int Func_8079e9c(unsigned char *rec, int needle) { u |
| `Func_807a2bc` | `0x0807a2bc` | — | read | Slotted between rom_79460_c_c_c_c_a_c_c_c_a_c_a.o and the rest of stage1.ld. A bit test on a per-unit word. The FIRST PARAMETER IS UNUSED -- |
| `Func_807a550` | `0x0807a550` | — | read | extern unsigned char *Func_8077330(int a); int Func_807a550(unsigned char *out) { unsigned char *t; unsigned char *p; int *q; int count; int |
| `Func_808adf0` | `0x0808adf0` | — | read | extern int _Func_80122c8(void *pos, int *out); extern int _GetFlag(int); extern void Func_808b2b0(int); extern short tbl[] __asm__(".L9d7a8" |
| `Func_808b25c` | `0x0808b25c` | — | read | extern unsigned char gState[]; extern unsigned char L9e270[] __asm__(".L9e270"); void Func_808b25c(void) { unsigned char *g; int *p; int e;  |
| `Func_808b2b0` | `0x0808b2b0` | — | read | Func_808b2b0  --  0x0808b2b0 The middle function of goldensun/asm/rom_8a000/rom_8ace0_a_a_c_c.s. Maps a door index to the area it leads to a |
| `Func_808bc9c` | `0x0808bc9c` | — | read | extern int iwram_3001ebc; int Func_808bc9c(void) { short *p; p = (short *)iwram_3001ebc; return p[0xb6] + p[0xb7] + p[0xb8] + p[0xb9] + p[0x |
| `Func_808c44c` | `0x0808c44c` | — | read | Func_808c44c  --  0x0808c44c Cut out of goldensun/asm/rom_8a000/rom_8ba38_a_a.s, which holds eleven functions. A mode-3 hook: when the game  |
| `Func_808d5a4` | `0x0808d5a4` | — | read | Func_808d5a4 extracted from goldensun/asm/rom_8a000/rom_8ba38_c_c.s. HAND SPLIT. The .s held this function AND a `.section .rodata` carrying |
| `Func_808e0b0` | `0x0808e0b0` | — | read | extern unsigned int iwram_3001e40; extern unsigned char L9e6b8[] __asm__(".L9e6b8"); struct Ent { unsigned char pad0[5]; unsigned char f5; u |
| `Func_808e118` | `0x0808e118` | — | read | Slotted between rom_8d9a4_a_c_a_a_a_a.o and the rest of stage1.ld. Clears a halfword and, if a second halfword is non-zero, hands off. TWO T |
| `Func_808e5d8` | `0x0808e5d8` | — | read | Func_808e5d8 (SortMapObjects) -- MATCHES on the default flags (and unchanged under -fno-rerun-cse-after-loop).  ref: asm/rom_8a000/rom_8d9a4 |
| `Func_808ed4c` | `0x0808ed4c` | — | read | Map interaction: read the slot record for whatever the player is facing. Whole-file conversion of asm/rom_8a000/rom_8d9a4_c_a_c_c_c_a.s -- o |
| `Func_808edac` | `0x0808edac` | — | read | extern unsigned char *iwram_3001ebc; extern int GetMapActorIndex(int a); extern int _Func_8011f54(int a, int b, int c); void Func_808edac(in |
| `Func_808eee4` | `0x0808eee4` | — | read | typedef unsigned char u8; struct Sub { u8 pad0[9]; u8 b9; }; struct Actor { u8 pad00[8]; int f8; int fc; int f10; u8 pad14[0x28 - 0x14]; int |
| `Func_808f0d8` | `0x0808f0d8` | — | read | Func_808f0d8  --  0x0808f0d8 Func_808f140  --  0x0808f140 The first two functions of goldensun/asm/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c.s. Func |
| `Func_808f140` | `0x0808f140` | — | read | Func_808f0d8  --  0x0808f0d8 Func_808f140  --  0x0808f140 The first two functions of goldensun/asm/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c.s. Func |
| `Func_808f28c` | `0x0808f28c` | — | read | Func_808f28c  --  0x0808f28c The last function of goldensun/asm/rom_8a000/rom_8d9a4_c_a_c_c_c_c_c.s; see rom_8d9a4_c_a_c_c_c_c_c_a.c for how |
| `Func_8090584` | `0x08090584` | — | read | #include "gba/types.h" #include "gba/io.h" extern unsigned char *iwram_3001e70; void Func_8090584(void) { unsigned int vc; unsigned char *g; |
| `Func_80907b0` | `0x080907b0` | — | read | #include "dma.h" extern int iwram_3001ecc; void Func_80907b0(int v) { int *p; int pat; int i; int *q; p = (int *)iwram_3001ecc; DMA3_FILL((v |
| `Func_8091174` | `0x08091174` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" extern char *galloc_ewram(int tag, int size); extern void Func_8090a5c(int a, vo |
| `Func_8091660` | `0x08091660` | — | read | Func_8091660  --  0x08091660 The first function of goldensun/asm/rom_8a000/rom_91584_a_c_a_c_c.s; CutsceneStart stays as assembly beside it. |
| `Func_8091814` | `0x08091814` | — | read | extern int _GetFlag(int member); extern int _HasMove(int member, int ability); int Func_8091814(unsigned int req) { int member; int ability; |
| `Func_8091858` | `0x08091858` | — | read | Slotted between rom_91584_c_a_c_c_c_a_a.o and the rest of stage1.ld. Two identical guards over two gState halfwords: hand each to Func_80918 |
| `Func_8091c1c` | `0x08091c1c` | — | read | Cutscene layer: hand an item to a party member. Split out of asm/rom_8a000/rom_91584_c_a_c_c_c.s; the preceding functions stay in asm/rom_8a |
| `Func_8091ff0` | `0x08091ff0` | — | read | Cutscene layer: start a looping sound. Whole-file conversion of asm/rom_8a000/rom_91584_c_c_a_c_a_c_c_a_a_c.s -- one function, so the ROM la |
| `Func_8092208` | `0x08092208` | — | read | struct Actor { unsigned char pad00[8]; int f8; int fc; int f10; }; extern struct Actor *GetFieldActor(int a); extern void _Actor_Stop(struct |
| `Func_80925e0` | `0x080925e0` | — | read | Particles: move one along a decaying ballistic arc. Split out of asm/rom_8a000/rom_925e0_a_a_a.s; the _a and _c parts stay as assembly and a |
| `Func_8092708` | `0x08092708` | — | read | typedef unsigned char u8; struct A { u8 pad0[8]; int f8; int fc; int f10; int f14; u8 pad18[0x28 - 0x18]; int f28; u8 pad2c[0x3c - 0x2c]; in |
| `Func_8092848` | `0x08092848` | — | read | Cutscene layer: turn two field actors to face each other. Split out of asm/rom_8a000/rom_925e0_a_a_c.s, which also holds Func_8092878; the r |
| `Func_8092878` | `0x08092878` | — | read | #include "actor.h" extern int atan2(int dz, int dx); extern void WaitFrames(int n); void Func_8092878(Actor *a, Actor *b) { int ang; int ang |
| `Func_8092980` | `0x08092980` | — | read | extern unsigned int iwram_3001e40; extern unsigned char L9ed80[] __asm__(".L9ed80"); void Func_8092980(char *a) { char *o; unsigned char v;  |
| `Func_80929d8` | `0x080929d8` | — | read | void Func_80929d8(char *a, int v) { char *o; int cnt; int n; char **p; char *e; if ((*(unsigned char *)(a + 0x54) & 0xf) == 1) { o = *(char  |
| `Func_8092a1c` | `0x08092a1c` | — | read | THE PARKED C WAS ALREADY CORRECT and had been written off on the retired branch-over-pool claim. Nothing about it needed changing; the mid-b |
| `Func_8092be0` | `0x08092be0` | — | read | extern unsigned char *iwram_3001ebc; int Func_8092be0(int id) { unsigned char *base; unsigned char *a; unsigned char *t; int i; int r; int o |
| `Func_8093168` | `0x08093168` | — | read | extern unsigned char *iwram_3001ebc; extern int _Func_8017658(int id, int a, int b, int c); extern int _Func_8017394(int h); extern void Wai |
| `Func_80935d4` | `0x080935d4` | — | read | extern int iwram_3001e70; extern int iwram_3001af4; extern void *galloc_ewram(int a, int b); extern int Func_8000888(int a, int b); extern v |
| `Func_809397c` | `0x0809397c` | — | read | extern void _Actor_TravelTo(void *a, int x, int y, int z); extern void _Actor_SetAnim(void *a, int anim); extern int Func_8000948(int v); in |
| `Func_8093a14` | `0x08093a14` | — | read | The MAIN-ROM original of src/overlays/rom_784360/ovl_30_a_a.c: turn an actor one step toward its target, clamped to +/-0x1000. The overlay c |
| `Func_8094730` | `0x08094730` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" extern unsigned char *galloc_ewram(int tag, int size); extern int StartTask(void |
| `Func_80958a8` | `0x080958a8` | — | read | Slotted between rom_944ec_a_c_a_a_a_a.o and the rest of stage1.ld. Allocates a 0x720-byte IWRAM block, zeroes it with DMA3_CLEAR, and starts |
| `Func_80958e4` | `0x080958e4` | — | read | translation unit and stage1.ld is untouched. Walks 24 records of 0x48 bytes, calling Func_809bb34 on each whose flag byte at +0x9d is set. T |
| `Func_8095b8c` | `0x08095b8c` | — | read | in goldensun/stage1.ld. Returns an entry from .L9f0a4 selected by bit 2 of iwram_1800 -- which of two ride variants is configured -- and wri |
| `Func_8096ab0` | `0x08096ab0` | — | read | Appended after the _a piece in goldensun/stage1.ld. Clears a byte on the party record when the field state is 2 and the stored facing at gSt |
| `Func_8096af0` | `0x08096af0` | — | read | Slotted between rom_944ec_a_c_c_a_a_a.o and rom_944ec_a_c_c_a_a_c.o in A three-way dispatch on a mode halfword. THE SELECTOR IS UNSIGNED AND |
| `Func_8096cdc` | `0x08096cdc` | — | read | extern unsigned char *GetFieldActor(int i); extern void _Actor_SetAnimSpeed(void *a, int s); extern short ewram_200048a; void Func_8096cdc(v |
| `Func_8096d2c` | `0x08096d2c` | — | read | extern void _Actor_SetScript(void *a, void *s); extern int sin(int x); extern unsigned char Data_9f0b0[]; void Func_8096d2c(int *a) { unsign |
| `Func_80970f8` | `0x080970f8` | — | read | extern int iwram_3001f30; extern void *GetFieldActor(int id); extern void vec3_translate(int a, int b, void *p); void Func_80970f8(int i0, i |
| `Func_8097194` | `0x08097194` | — | read | extern char *iwram_3001f30; extern char *iwram_3001e64; extern void Func_809bb34(void *a); extern void WaitFrames(int n); extern int StopTas |
| `Func_809728c` | `0x0809728c` | — | read | struct Actor { unsigned char pad00[8]; int f8; int fc; int f10; unsigned char pad14[0x10]; int f24; int f28; int f2c; unsigned char pad30[8] |
| `Func_8097868` | `0x08097868` | — | read | Placed in the run in goldensun/stage1.ld. Arms a DMA0 scroll effect from a per-map table of 81-word records, but only while the flag byte at |
| `Func_80978c4` | `0x080978c4` | — | read | extern unsigned char *iwram_3001ea8; extern void Func_8097948(int ang, int *a, int *b, int *c); void Func_80978c4(void) { unsigned char *b;  |
| `Func_8097a10` | `0x08097a10` | — | read | extern int Func_80008ac(int a, int b); extern int Func_8000888(int a, int b); static inline int call_via_r4(int (*f)(int, int), int a, int b |
| `Func_8097a54` | `0x08097a54` | — | read | Movement: restart the idle script once an actor has stopped moving. Split out of asm/rom_8a000/rom_97384_c_c.s. The neighbours are the _a an |
| `Func_8097a7c` | `0x08097a7c` | — | read | split_s.py, so no split was needed. TWO LEVERS, and the second is the general one. `volatile` ON THE STORE POINTER STOPS THE FINAL-INCREMENT |
| `Func_8097adc` | `0x08097adc` | — | read | FROM THE BRANCH-OVER-POOL CLASS; the pool needed no help. READ THE POOL ORDER OFF THE HAND-WRITTEN .s BEFORE COMPILING ANYTHING. The ROM pul |
| `Func_8097b70` | `0x08097b70` | — | read | struct Ent { unsigned char pad0[6]; unsigned short f6; int f8; int fc; int f10; unsigned char pad14[0x34]; int f48; unsigned char pad4c[9];  |
| `Func_8098184` | `0x08098184` | — | read | extern void _Actor_WaitMovement(unsigned char *a); void Func_8098184(unsigned char *a) { int v; int w; if (a == 0) return; v = *(int *)(a +  |
| `Func_80984c0` | `0x080984c0` | — | read | extern unsigned char iwram_3001f30[]; extern unsigned char gState[]; extern void _PlaySound(int id); extern void StopTask(void *fn); extern  |
| `Func_80990cc` | `0x080990cc` | — | read | extern void vec3_translate(int a, int b, int *v); extern void Func_8099040(void); void Func_80990cc(unsigned char *e) { int v[3]; int *p; sh |
| `Func_80992f0` | `0x080992f0` | — | read | extern int sin(int); extern int Func_8000888(int, int); static inline int call_via_r3(int (*f)(int, int), int a, int b) { register int (*_f) |
| `Func_8099340` | `0x08099340` | — | read | extern int *iwram_3001f30; extern unsigned char Data_9f0b0[]; extern void vec3_translate(int a, int b, int *v); extern void _Actor_SetScript |
| `Func_80993b0` | `0x080993b0` | — | read | struct Obj { int f0; int f4; int f8; int fc; }; struct Ent { unsigned char pad0[8]; int f8; int fc; int f10; unsigned char pad14[4]; int f18 |
| `Func_8099920` | `0x08099920` | — | read | typedef unsigned char u8; typedef unsigned short u16; struct A { u8 pad00[8]; int f8; int fc; int f10; int f14; int f18; int f1c; u8 pad20[8 |
| `Func_8099d18` | `0x08099d18` | — | read | Spawns one particle at a jittered offset from a source entity.  Two screens: the body was right first time and the only defect was a narrow  |
| `Func_809a44c` | `0x0809a44c` | — | read | Placed ahead of the _c piece in goldensun/stage1.ld. Advances an entity by its per-axis deltas and spins its attached sprite. Three position |
| `Func_809a65c` | `0x0809a65c` | — | read | The MAIN-ROM original of src/overlays/rom_7a5214/ovl_17ec_c_b.c, a per-frame integrator with damping: add the stored velocities to the posit |
| `Func_809ad70` | `0x0809ad70` | — | read | Idle flicker: nudge a resting actor's palette at random. Split out of asm/rom_8a000/rom_9ad70_a_a_a.s; the rest stays in ..._a_a_a_c.s, list |
| `Func_809ad90` | `0x0809ad90` | — | read | extern unsigned int gState; extern unsigned char *GetFieldActor(int slot); extern void _Actor_SetAnimSpeed(unsigned char *a, int n); extern  |
| `Func_809ade8` | `0x0809ade8` | — | read | Func_809ade8  --  0x0809ade8 The second function of goldensun/asm/rom_8a000/rom_9ad70_a_a_a_c.s; Func_809ad90 stays as assembly in rom_9ad70 |
| `Func_809b0dc` | `0x0809b0dc` | — | read | struct S { unsigned char pad00[6]; unsigned short f6; unsigned char pad08[4]; int fc; unsigned char pad10[8]; int f18; int f1c; unsigned cha |
| `Func_809b364` | `0x0809b364` | — | read | extern unsigned char gState[]; extern int _CONST_1; extern void _DeleteActor(unsigned char *a); void Func_809b364(unsigned char *a) { unsign |
| `Func_809b3d8` | `0x0809b3d8` | — | read | extern unsigned char gState[]; extern int _CONST_1; extern void _DeleteActor(unsigned char *a); void Func_809b3d8(unsigned char *a) { unsign |
| `Func_809b5dc` | `0x0809b5dc` | — | read | extern unsigned char gState[]; extern int _CONST_1; extern void Func_809b450(unsigned char *e); void Func_809b5dc(unsigned char *e) { unsign |
| `Func_809b648` | `0x0809b648` | — | read | Slotted between rom_9ad70_c_c_a.o and the rest of stage1.ld. THE OFFSET VARIABLE IS REUSED AS THE STORED VALUE. The ROM computes the address |
| `Func_809b804` | `0x0809b804` | — | read | extern void Func_809b8f4(unsigned char *a); extern void Func_809b86c(unsigned char *a); void Func_809b804(unsigned char *a) { signed char *p |
| `Func_809b86c` | `0x0809b86c` | — | read | extern int Func_8000888(int, int); extern void _UpdateSprite(int *a, int *b, int *c, int d); static inline int call_via(int (*f)(int, int),  |
| `Func_809bb34` | `0x0809bb34` | — | read | Slotted between rom_9b698_c_c_a.o and the rest of stage1.ld. Deletes the sprite a struct points at, if any, then zeroes the struct. FIRST US |
| `Func_80a10d0` | `0x080a10d0` | — | read | extern void _Func_8016498(void *p); extern void *_CreateUIBox(int a, int b, int c, int d, int e); int Func_80a10d0(void **slot, int a, int b |
| `Func_80a14f0` | `0x080a14f0` | — | read | extern void _Func_801e9d4(int a, int n, int b, int c, int d); void Func_80a14f0(int a0, int a1, int a2, int a3) { int v; int n; v = a0; n =  |
| `Func_80a17c4` | `0x080a17c4` | — | read | translation unit and the linker script is untouched. Resets a sprite's animation state: sets a flag byte, copies a 9-bit field from +6 into  |
| `Func_80a1814` | `0x080a1814` | — | read | extern void Func_80a10d0(void *p, int b, int c, int d, int e, int f); extern char *Func_80a1778(void *q, int b, int c); void *Func_80a1814(u |
| `Func_80a195c` | `0x080a195c` | — | read | extern int iwram_3001f2c; extern int _Func_80796c4(void *out); extern void _DeleteSprite(void *s); extern void StopTask(void *fn); extern vo |
| `Func_80a19a0` | `0x080a19a0` | — | read | struct Sprite { unsigned char pad00[9]; unsigned char b0 : 2, b2 : 2, b4 : 4; }; struct Blk { unsigned char pad000[0x114]; struct Sprite *ac |
| `Func_80a1bdc` | `0x080a1bdc` | — | read | extern char *iwram_3001f2c; extern void Func_80a1c2c(void *node, int i, int x, int y, int cols); void Func_80a1bdc(int x, int y, int cols) { |
| `Func_80a1cb0` | `0x080a1cb0` | — | read | extern char *iwram_3001f2c; extern void Func_a1c6c(void *node, int i, int a, int k, int n); void Func_80a1cb0(int mode) { char *s; char *p;  |
| `Func_80a21b0` | `0x080a21b0` | — | read | extern void _Func_8019000(int win, int tile, int col, int row, int pal); void Func_80a21b0(int win, int total, int perPage, int page, int co |
| `Func_80a2268` | `0x080a2268` | — | read | struct Win { unsigned char pad00[0xc]; unsigned short fc; unsigned short fe; }; extern unsigned char *iwram_3001e8c; void Func_80a2268(struc |
| `Func_80a23c0` | `0x080a23c0` | — | read | Menu: draw the party's coin total. Split out of asm/rom_a1000/rom_a1814_c_a_a_c_a_c.s, which holds eleven functions; the _a and _c parts sta |
| `Func_80a3c98` | `0x080a3c98` | — | read | extern int iwram_3001f2c; extern void _Sprite_SetAnim(void *sprite, int anim); extern void StopTask(void *fn); extern void Func_80a3c08(void |
| `Func_80a3d24` | `0x080a3d24` | — | read | extern unsigned char *iwram_3001f2c; extern void Func_80a17c4(void *node); void Func_80a3d24(unsigned short *src) { unsigned char *state; un |
| `Func_80a45cc` | `0x080a45cc` | — | read | extern int _MSG_b33; extern void _SetTextColor(int color); extern void _Func_801e7c0(int msg, int win, int x, int y); void Func_80a45cc(sign |
| `Func_80a4754` | `0x080a4754` | — | read | Slotted between rom_a1814_c_c_c_a.o and the rest of stage1.ld. A chance-based item break: read the equipped id, check its category, roll, an |
| `Func_80a47b4` | `0x080a47b4` | — | read | Func_80a47b4  --  0x080a47b4 The first function of goldensun/asm/rom_a1000/rom_a47b4_a.s; the other three stay as assembly in rom_a47b4_a_c. |
| `Func_80a4db4` | `0x080a4db4` | — | read | Func_80a4db4  --  0x080a4db4 Cut out of goldensun/asm/rom_a1000/rom_a47b4_a_c.s. Draws a signed number and then its sign glyph, right-aligni |
| `Func_80a5578` | `0x080a5578` | — | read | extern unsigned int iwram_3001f2c; extern int _GetUnit(int id); extern int Func_80a3d6c(int id); int Func_80a5578(int *dest, int cursor) { u |
| `Func_80a68a8` | `0x080a68a8` | — | read | extern unsigned char *iwram_3001f2c; extern void _Func_801bcd4(int a, int b, int c, int d); extern void Func_80a3d24(void *p); void Func_80a |
| `Func_80a6a00` | `0x080a6a00` | — | read | extern unsigned int iwram_3001f2c; extern int _GetUnit(int id); int Func_80a6a00(int *dest, int cursor) { unsigned char *state; unsigned cha |
| `Func_80a735c` | `0x080a735c` | — | read | Menu: is this item selectable? Split out of asm/rom_a1000/rom_a5534_c_c_c.s. The preceding functions stay in ..._c_c_c_a.s; the trailing .ro |
| `Func_80a8034` | `0x080a8034` | — | read | extern int iwram_3001f2c; extern void *Func_80a1814(void *p); extern void Func_80a1870(void *p, int a, int b, int c, int d); void Func_80a80 |
| `Func_80a8b8c` | `0x080a8b8c` | — | read | extern unsigned int iwram_3001f2c; extern int _GetUnit(int id); int Func_80a8b8c(int *dest, int cursor) { unsigned char *state; unsigned cha |
| `Func_80a99b0` | `0x080a99b0` | — | read | int Func_80a99b0(int *pcol, int *prow, int dir) { int col; int row; col = *pcol; row = *prow; switch (dir) { case 0x40: row--; if (row < 0)  |
| `Func_80a9b94` | `0x080a9b94` | — | read | extern char *iwram_3001f2c; extern void Func_80a9bd8(void *node, int i, int x, int y, int cols); void Func_80a9b94(int x, int y, int cols) { |
| `Func_80a9cbc` | `0x080a9cbc` | — | read | Walks 32 object slots at [iwram_3001f2c]+0x48 and, for each non-null one, writes two halfwords and calls Func_80a17c4. THE ONE THING THAT MA |
| `Func_80a9cf8` | `0x080a9cf8` | — | read | extern unsigned char *iwram_3001f2c; extern void *_Func_801eb64(int a, int b, int c, int d, int e); int Func_80a9cf8(int a) { unsigned char  |
| `Func_80a9d3c` | `0x080a9d3c` | — | read | extern unsigned char *iwram_3001f2c; extern void Func_80a9d84(void); extern void Func_80a17c4(void *x); void Func_80a9d3c(unsigned char *fla |
| `Func_80a9dc4` | `0x080a9dc4` | — | read | Func_80a9dc4  --  0x080a9dc4 Cut out of goldensun/asm/rom_a1000/rom_a8604_c_c_a_c_a_c.s. Plays one status-effect cue per set flag in a five- |
| `Func_80aa460` | `0x080aa460` | — | read | Func_80aa460  --  0x080aa460 The whole of goldensun/asm/rom_a1000/rom_a8604_c_c_c_c_a_c.s. Picks the sound a move makes: element 1 plays it  |
| `Func_80aac84` | `0x080aac84` | — | read | typedef unsigned short u16; void Func_80aac84(int add) { int i, j, row, base; int idx, b, g, r, v; int mask; unsigned int c; row = 15; i = 0 |
| `Func_80aca04` | `0x080aca04` | — | read | extern char *iwram_3001f2c; extern void Func_80acab8(int win, int a, int b, int id, int s0, int s1, int s2, int s3, int s4); int Func_80aca0 |
| `Func_80ad5f4` | `0x080ad5f4` | — | read | Field actor scale table: store one slot's value. Split out of asm/rom_a1000/rom_ad274_c_a.s, which holds six functions; the neighbours are a |
| `Func_80b010c` | `0x080b010c` | — | read | OpenShopState: allocate the module's 0xa70-byte block, DMA-clear it, reserve six OBJ tile slots and register the per-frame task.  Matched on |
| `Func_80b06ec` | `0x080b06ec` | — | read | Func_80b06ec -- 0x080b06ec, from Copies four groups of four bytes into a 2x2 tile layout -- offsets 0, 1, 0x1e, 0x1f from a destination that |
| `Func_80b0744` | `0x080b0744` | — | read | #include "dma.h" extern unsigned char L_b3e80[] __asm__(".Lb3e80"); extern unsigned char *galloc_ewram(int index, int size); extern void Fun |
| `Func_80b0840` | `0x080b0840` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" extern char *iwram_3001ebc[]; extern void _Func_8091200(int a, int b); extern vo |
| `Func_80b10cc` | `0x080b10cc` | — | read | Func_80b10cc  --  0x080b10cc, cut from DrawRowText: emit a row's label and its number, skipping both if the row count is zero. DECLARE gStat |
| `Func_80b19cc` | `0x080b19cc` | — | read | extern unsigned char *_GetItemInfo(int item); int Func_80b19cc(int item) { int v; v = (short)*(unsigned short *)_GetItemInfo(item); if ((_Ge |
| `Func_80b26cc` | `0x080b26cc` | — | read | extern unsigned char Lb41ac[] __asm__(".Lb41ac"); extern int _GetFlag(int id); extern void _SetFlag(int id); extern void _Func_8078ad0(int a |
| `Func_80b27b0` | `0x080b27b0` | — | read | extern void *_GetUnit(int id); int Func_80b27b0(int id, int kind) { unsigned char *u; int r; u = (unsigned char *)_GetUnit(id); r = 0; if (k |
| `Func_80b2884` | `0x080b2884` | — | read | Slotted between the _a and _c pieces in goldensun/stage1.ld. Shifts a message id by a per-language offset chosen from a signed byte at iwram |
| `Func_80b606c` | `0x080b606c` | — | read | Slotted between the _a and _c pieces in goldensun/stage1.ld. Narrows four halfword characters into a four-byte buffer, substituting '_' (0x5 |
| `Func_80b60a0` | `0x080b60a0` | — | read | extern unsigned int iwram_3001e74; extern unsigned int ewram_2002024; extern unsigned int ewram_2002224; extern unsigned short iwram_3001f64 |
| `Func_80b6378` | `0x080b6378` | — | read | extern char *iwram_3001e74; extern int Func_80b6a60(unsigned short *buf); int Func_80b6378(void) { unsigned short buf[8]; char *p; int i; in |
| `Func_80b63b0` | `0x080b63b0` | — | read | Seven instructions. Clears 0x10 bytes at ewram_2002224 through Func_80008d4, called via the `_call_via_r3` veneer. PARKED SINCE JUNE as a pe |
| `Func_80b6cdc` | `0x080b6cdc` | — | read | extern int Func_80c23c0(void); extern int iwram_3001e74; int Func_80b6cdc(void) { char *s; int flag; int i; int off; int a; flag = Func_80c2 |
| `Func_80b6e30` | `0x080b6e30` | — | read | FROM THE BRANCH-OVER-POOL CLASS. The pool shape caused no trouble at all -- gcc emits the `b` over the pool at the ROM's position unaided. W |
| `Func_80b7e7c` | `0x080b7e7c` | — | read | Battle teardown: release every combatant's sprite. Whole-file conversion of asm/rom_b5000/rom_b7410_a_c_c_c.s -- one function, so the ROM la |
| `Func_80b7f9c` | `0x080b7f9c` | — | read | Camera reset: zero two vectors, set pitch and yaw, then build the matrix and transform one point. TWO THINGS CARRY THE MATCH: - `bl _call_vi |
| `Func_80b80b8` | `0x080b80b8` | — | read | struct A { unsigned char pad0[8]; int f8; unsigned char pad0c[4]; int f10; unsigned char pad14[0x14]; int f28; unsigned char pad2c[4]; int f |
| `Func_80b83b4` | `0x080b83b4` | — | read | Func_80b83b4  --  0x080b83b4 The whole of goldensun/asm/rom_b5000/rom_b8228_c_a_a.s, which held this function and no data, so the .o keeps i |
| `Func_80b86ec` | `0x080b86ec` | — | read | extern int iwram_3001e80[]; extern volatile int gKeyHeld; extern void Func_80c0a24(int a, int b, int c, int d, int e); void Func_80b86ec(voi |
| `Func_80b8b48` | `0x080b8b48` | — | read | Matched on the FIRST screen, and it is worth saying which reads of the assembly made that happen, because none of them was a lever: THE EPIL |
| `Func_80b8f08` | `0x080b8f08` | — | read | Func_80b8f08  --  0x080b8f08 Cut out of goldensun/asm/rom_b5000/rom_b8228_c_a_c_c_a.s. Picks a random target from a candidate list: fetch th |
| `Func_80b98b4` | `0x080b98b4` | — | read | typedef unsigned short u16; void Func_80b98b4(int add) { int i, j, row, base; int idx, b, g, r, v; unsigned int c; row = 15; i = 0; do { j = |
| `Func_80b9a70` | `0x080b9a70` | — | read | extern char *iwram_3001e74; int Func_80b9a70(int key) { char *b; int i; int off; int flag; short v; b = iwram_3001e74; if ((unsigned int)key |
| `Func_80b9acc` | `0x080b9acc` | — | read | extern int iwram_3001e80[]; extern volatile int gKeyHeld; extern void Func_80c0a24(int a, int b, int c, int d, int e); void Func_80b9acc(voi |
| `Func_80bace8` | `0x080bace8` | — | read | struct Q { unsigned char pad00[5]; unsigned char f5; unsigned char pad06[0x16 - 6]; unsigned char f16; }; struct Spr { unsigned char pad00[0 |
| `Func_80be070` | `0x080be070` | — | read | extern int Func_80b6c08(int kind, unsigned short *buf); int Func_80be070(unsigned int a) { unsigned short buf[8]; int kind; int n; int i; ki |
| `Func_80bf250` | `0x080bf250` | — | read | was needed and the linker script is unchanged. TickStatusCounter, ten times. Each decrements one per-combatant status counter in the persist |
| `Func_80bf2b4` | `0x080bf2b4` | — | read | was needed and the linker script is unchanged. TickStatusCounter, ten times. Each decrements one per-combatant status counter in the persist |
| `Func_80bf318` | `0x080bf318` | — | read | was needed and the linker script is unchanged. TickStatusCounter, ten times. Each decrements one per-combatant status counter in the persist |
| `Func_80bf37c` | `0x080bf37c` | — | read | was needed and the linker script is unchanged. TickStatusCounter, ten times. Each decrements one per-combatant status counter in the persist |
| `Func_80bf3bc` | `0x080bf3bc` | — | read | was needed and the linker script is unchanged. TickStatusCounter, ten times. Each decrements one per-combatant status counter in the persist |
| `Func_80bf400` | `0x080bf400` | — | read | was needed and the linker script is unchanged. TickStatusCounter, ten times. Each decrements one per-combatant status counter in the persist |
| `Func_80bf440` | `0x080bf440` | — | read | was needed and the linker script is unchanged. TickStatusCounter, ten times. Each decrements one per-combatant status counter in the persist |
| `Func_80bf484` | `0x080bf484` | — | read | was needed and the linker script is unchanged. TickStatusCounter, ten times. Each decrements one per-combatant status counter in the persist |
| `Func_80bf4c4` | `0x080bf4c4` | — | read | was needed and the linker script is unchanged. TickStatusCounter, ten times. Each decrements one per-combatant status counter in the persist |
| `Func_80bf524` | `0x080bf524` | — | read | was needed and the linker script is unchanged. TickStatusCounter, ten times. Each decrements one per-combatant status counter in the persist |
| `Func_80bf54c` | `0x080bf54c` | — | read | Func_80bf54c -- 0x080bf54c, the whole of linker script is unchanged. The eleventh TickStatusCounter, and the simplest: decrement one status  |
| `Func_80c0098` | `0x080c0098` | — | read | Writes two ramps of packed byte-index words -- 0x03020100, 0x07060504, ... -- 64 words then 56 words, and then clears 0x220 bytes past the e |
| `Func_80c01bc` | `0x080c01bc` | — | read | extern int iwram_3001ef8; extern short iwram_3001ad0[]; extern void Func_80c0cec(int a, int b, int c, int d); void Func_80c01bc(void) { int  |
| `Func_80c08a8` | `0x080c08a8` | — | read | Slotted between rom_bffb8_a_c_a_a.o and the rest of stage1.ld. Allocates a 0x2a0-byte EWRAM block, zeroes it, and clears a field of the iwra |
| `Func_80c0df4` | `0x080c0df4` | — | read | Func_80c0df4  --  0x080c0df4, cut from the tail of in goldensun/stage1.ld. AimCameraAtCombatant: point the camera at the midpoint of two com |
| `Func_80c0e38` | `0x080c0e38` | — | read | Func_80c0e38 and Func_80c0e70  --  0x080c0e38 / 0x080c0e70, cut from the tail of goldensun/asm/rom_b5000/rom_bffb8_a_c_c.s. A matched pair o |
| `Func_80c0e70` | `0x080c0e70` | — | read | Func_80c0e38 and Func_80c0e70  --  0x080c0e38 / 0x080c0e70, cut from the tail of goldensun/asm/rom_b5000/rom_bffb8_a_c_c.s. A matched pair o |
| `Func_80c1084` | `0x080c1084` | — | read | From the branch-over-pool class; the pool needed no help. DELETING A LOCAL WON THIS ONE. The previous park kept a separate `unsigned short * |
| `Func_80c1fa8` | `0x080c1fa8` | — | read | extern unsigned char Lc5c38[] __asm__(".Lc5c38"); extern unsigned int Random(void); int Func_80c1fa8(unsigned int id) { int buf[5]; unsigned |
| `Func_80c23a0` | `0x080c23a0` | — | read | extern unsigned char Lc7420[] __asm__(".Lc7420"); int Func_80c23a0(unsigned int i) { unsigned char *t; unsigned int idx; unsigned char *p; i |
| `Func_80c90e4` | `0x080c90e4` | — | read | Placed in the run in goldensun/stage1.ld. A frame counter in the effects block: bumps the count at +0x7790 and, when it reaches the limit at |
| `Func_80c9138` | `0x080c9138` | — | read | #include "gba/types.h" #include "gba/io.h" extern char *iwram_3001eec; void Func_80c9138(void) { char *b; int *ctr; int *xp; int *yp; int n; |
| `Func_80ccbdc` | `0x080ccbdc` | — | read | A teardown: stop three tasks, clear the 0x4000-byte tile block at 0x6004000, release two allocator tags. Eighteen instructions. Fourth out o |
| `Func_80cd418` | `0x080cd418` | — | read | #include "gba/io.h" extern int iwram_3001eec; void Func_80cd418(void) { char *p; p = (char *)iwram_3001eec; REG_WIN0H = *(unsigned short *)( |
| `Func_80cd488` | `0x080cd488` | — | read | Copies the cached affine reference point from [iwram_3001eec]+0x77d0 and +0x77d4 into REG_BG2X and REG_BG2Y. UNPARKED BY TWO THINGS, NEITHER |
| `Func_80cd4b4` | `0x080cd4b4` | — | read | extern char *iwram_3001e74[]; extern void _UploadBGPalette(void *a, void *b, int c, int d); void Func_80cd4b4(void) { char *p; char *g; int  |
| `Func_80cd508` | `0x080cd508` | — | read | Ten instructions. Clears eight bytes at +0x7818 into the block whose address lives at iwram_3001eec, through Func_80008d4 on the `_call_via_ |
| `Func_80dfddc` | `0x080dfddc` | — | read | void Func_80dfddc(unsigned char *src, unsigned char *dst, int n, int m) { int i; int j; int srcoff; int k; unsigned char *p; unsigned char * |
| `Func_80e3994` | `0x080e3994` | — | read | extern int Func_8000888(int, int); static inline int call_via(int (*f)(int, int), int a, int b) { register int _a __asm__("r0") = a; registe |
| `Func_80f037c` | `0x080f037c` | — | read | Placed in the run in goldensun/stage1.ld. Fills a 512-word buffer in four runs: 32 words of 0x01ff01ff, 240 words of a value stepping by 0x0 |
| `Func_80f7db4` | `0x080f7db4` | — | read | The .s is split in two around this function: the earlier functions move to rom_f6008_c_a.s and the later ones stay in rom_f6008_c.s, which a |
| `GetEquippedItem` | `` | — | read | GetEquippedItem  --  0x080787dc, from asm/rom_77000/rom_78414_c_c_a_c_a.s. Scans a unit's 15-entry item array (unit + 0xd8, one halfword eac |
| `GetJupiterDjinni` | `` | — | read | extern unsigned char gState[]; extern unsigned char *iwram_3001f30; extern unsigned char *MapActor_GetActor(int slot); extern void Func_8095 |
| `GetMoveDisplayEffect` | `` | — | read | extern int _Func_8079ef8(int a); int GetMoveDisplayEffect(unsigned char *m) { int t; int k; int r; r = 0; t = m[1] & 0xf; if (t == 1) r = 1; |
| `GetNumDjinn` | `` | — | read | extern int Func_80796c4(unsigned short *buf); extern unsigned char *GetUnit(int id); int GetNumDjinn(int which) { unsigned short buf[16]; un |
| `GetSpriteVoice` | `` | — | read | GetSpriteVoice extracted from goldensun/asm/rom_8a000/rom_91584_a_c_a_a.s. UNPARKED. The park was 20 lines against the ROM's 21, which is th |
| `GetSpriteVoiceEntry` | `` | — | read | The .rodata stays in the original .s, which keeps its name and its .rodata line in goldensun/stage1.ld; only the .text line is repointed her |
| `GetWeaponSpriteID` | `` | — | read | GetWeaponSpriteID  --  0x080b6eb4 The function half of goldensun/asm/rom_b5000/rom_b6eb4.s. The five per-class sprite tables it indexes are  |
| `GiveItem` | `` | — | read | extern int Func_80796c4(short *buf); extern int GiveItemTo(int item, int who); int GiveItem(int who) { short buf[10]; short *p; int n; int i |
| `HasMove` | `` | — | read | Slotted between rom_78b9c_a_c_a.o and the rest of stage1.ld. Scans 32 move slots for one whose low 14 bits match. THE MASK IS A NAMED LOCAL  |
| `HeightTile_3` | `` | — | read | Placed in the run in goldensun/stage1.ld. One of the height-tile corner resolvers. Reads the two signed corner heights out of a tile record, |
| `HeightTile_7` | `` | — | read | Interpolates between two signed height samples by a weight looked up from a 16-wide table. UNPARKED. It was filed as "reg-alloc/scheduling d |
| `HuffStr_Start` | `` | — | read | First in the run, ahead of the _b piece, in goldensun/stage1.ld; the _b piece keeps the .rodata. Copies the ARM Huffman-string routine Func_ |
| `InitSprite` | `` | — | read | #include "gba/types.h" struct SpritePart { 0x00 |
| `InitSpriteLayer` | `` | — | read | Sprites: bind a part to its resource. Split out of asm/rom_9000/rom_b798_c_a_a.s; the _a and _c parts stay as assembly and are listed around |
| `LoadInventoryIcon` | `` | — | read | The same function as LoadStatusIcon next door: allocate the tag-0x11 scratch, draw into it, upload 0x80 bytes from +0x400, release the arena |
| `LoadItemIcon` | `` | — | read | typedef struct { unsigned char pad[0x600]; short f600; short f602; int f604; } Blk; extern Blk *iwram_3001e94; extern unsigned char *_GetIte |
| `LoadItemIconID` | `` | — | read | typedef struct { unsigned char pad[0x600]; short f600; short f602; int f604; } Blk; extern Blk *galloc_iwram(int tag, int size); extern void |
| `LoadMoveIcon` | `` | — | read | Menu icons: load the move icon for a move. Split out of asm/rom_15000/rom_19ebc_a_c_c.s, which holds twelve functions; the neighbouring _a/_ |
| `LoadMoveIconID` | `` | — | read | typedef struct { unsigned char pad[0x600]; short f600; short f602; int f604; } Blk; extern Blk *galloc_iwram(int tag, int size); extern void |
| `LoadMoveRangeIcons` | `` | — | read | extern void Func_80008d8(void *dst, int n, int v); extern int Laf23c[] __asm__(".Laf23c"); typedef void (*FillFn)(void *dst, int n, int v);  |
| `LoadOldMoveIcon` | `` | — | read | Menu icons: load the item icon for a move. Split out of asm/rom_15000/rom_19ebc_a_c_c.s, which holds twelve functions; the neighbouring _a/_ |
| `LoadStatusIcon` | `` | — | read | THIS WAS PARKED, and the park note was wrong about what it would take. It sat at 26 of 27 instructions on the arg-fill-order class: the ROM  |
| `LoadUIIcon` | `` | — | read | Slotted between rom_23178_a_a_a_a_c_c_a.o and the rest of stage1.ld. Allocates a buffer, decompresses one entry of an icon file into it, upl |
| `LoadVFXFile` | `` | — | read | keeps its name and its slot in goldensun/stage1.ld is unchanged. From the annotation on the original .s: r0=resource id, r1=destination, r2= |
| `MapActor_SetIdle` | `` | — | read | Cutscene layer: park a field actor where it stands. Whole-file conversion of ROM layout is preserved without splitting the translation unit. |
| `OvlFunc_880_20081fc` | `0x020081fc` | — | read | extern short L16b2 __asm__(".L16b2"); extern short L16b4 __asm__(".L16b4"); extern short L16b6 __asm__(".L16b6"); extern short gScript_930__ |
| `OvlFunc_880_20082f4` | `0x020082f4` | — | read | void OvlFunc_880_20082f4(int c, unsigned char *out) { out[1] = 0; out[2] = 0; if (c <= 0x7) { out[0] = c + 0x41; return; } if (c <= 0xc) { o |
| `OvlFunc_880_2008cfc` | `0x02008cfc` | — | read | extern unsigned char *iwram_3001e8c; extern void *__Func_8004970(int size); extern int __DecompressLZ(void *src, void *dst); extern void __f |
| `OvlFunc_880_2008d74` | `0x02008d74` | — | read | extern unsigned char *iwram_3001e8c; extern void *__Func_8004970(int size); extern void __free(void *p); struct S { unsigned char pad00[0xc] |
| `OvlFunc_881_2008030` | `0x02008030` | — | read | OvlFunc_881_2008030  --  0x02008030 Cut out of goldensun/asm/overlays/rom_77a7c8/ovl_30_a_a_a.s. A fatigue check: if the counter in gState h |
| `OvlFunc_881_200808c` | `0x0200808c` | — | read | extern char *iwram_3001ebc; extern unsigned char gState[]; extern void __Func_8091f14(int a, int b); void OvlFunc_881_200808c(void) { char * |
| `OvlFunc_881_20080d4` | `0x020080d4` | — | read | extern char *iwram_3001ebc; extern unsigned char gState[]; extern void __Func_8091f14(int a, int b); void OvlFunc_881_20080d4(void) { char * |
| `OvlFunc_881_200813c` | `0x0200813c` | — | read | OvlFunc_881_200813c  --  0x0200813c Cut out of goldensun/asm/overlays/rom_77a7c8/ovl_30_a_a_a_c.s. Per-frame hook on one effect actor: set i |
| `OvlFunc_881_2008314` | `0x02008314` | — | read | Resets an actor's sprite state, and on one save bit clears its position. ONE ZERO, USED FOUR TIMES. The ROM keeps it in r6 across the branch |
| `OvlFunc_881_200837c` | `0x0200837c` | — | read | OvlFunc_881_200837c  --  0x0200837c Cut out of goldensun/asm/overlays/rom_77a7c8/ovl_30_c_a_c_a_c_a.s. Chooses the innkeeper's script by are |
| `OvlFunc_881_20084a0` | `0x020084a0` | — | read | extern unsigned int gState; extern unsigned char *iwram_3001ebc; extern unsigned char *__MapActor_GetActor(int slot); extern void __PlaySoun |
| `OvlFunc_881_20084f0` | `0x020084f0` | — | read | extern unsigned int gState; extern unsigned char *iwram_3001ebc; extern unsigned char *__MapActor_GetActor(int slot); extern void __PlaySoun |
| `OvlFunc_881_200a768` | `0x0200a768` | — | read | struct E1 { int f0; short f4; short f6; void (*f8)(void); }; struct E2 { short f0; short f2; int f4; int f8; int fc; int f10; short f14; sho |
| `OvlFunc_881_200a8a8` | `0x0200a8a8` | — | read | A cutscene that sets one halfword in the iwram block if a flag is set. A NEW COROLLARY OF narrow_constant, and it runs the opposite way to e |
| `OvlFunc_881_200b130` | `0x0200b130` | — | read | extern char *iwram_3001ebc; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __PlaySound(int id); extern void |
| `OvlFunc_881_200b448` | `0x0200b448` | — | read | Given a small selector, pick a base flag id and return the first entry of a word table whose corresponding flag is set, or 0 if none of nine |
| `OvlFunc_881_200b4a0` | `0x0200b4a0` | — | read | OvlFunc_881_200b4a0 -- MATCHES on the default flags (and unchanged under -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_77a7c8/ovl_30_c_ |
| `OvlFunc_881_200b678` | `0x0200b678` | — | read | OvlFunc_881_200b678 Cut out of goldensun/asm//overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_b.s. A per-frame countdown on a save byte, with a s |
| `OvlFunc_881_200b6dc` | `0x0200b6dc` | — | read | OvlFunc_881_200b6dc -- MATCHES, but ONLY with --cflags "-fno-rerun-cse-after-loop". ref: asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_c_c.s  |
| `OvlFunc_881_200b8fc` | `0x0200b8fc` | — | read | extern int iwram_3001e40; extern unsigned short L67a0[] __asm__(".L67a0"); extern void __SetRegAnimDest(int dest, int val); void OvlFunc_881 |
| `OvlFunc_881_200bfb4` | `0x0200bfb4` | — | read | Split out of that .s; the _a part stays as assembly and keeps its slot in A 32-frame effect actor riding on a parent: each frame it advances |
| `OvlFunc_881_200c004` | `0x0200c004` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in The MIRROR of OvlFunc_881_200bfb4, which sits immediately before i |
| `OvlFunc_882_2008064` | `0x02008064` | — | read | OvlFunc_882_2008064  --  0x02008064, cut from goldensun/asm/overlays/rom_77dd1c/ovl_30_a_a.s. A four-phase nudge on a pair of 20.12 offsets  |
| `OvlFunc_882_200810c` | `0x0200810c` | — | read | UNPARKED BY THE STACK-ARG-PAIR LEVER. This family was parked as "STACK-ARGUMENT REGISTER REUSE": the ROM builds both stack constants into se |
| `OvlFunc_882_2008134` | `0x02008134` | — | read | The immediate neighbour of the exemplar: it CLEARS the flag its sibling one .o earlier sets, and re-runs the same map edit one column over.  |
| `OvlFunc_882_2008198` | `0x02008198` | — | read | keeps its name and its slot in goldensun/overlays/rom_77dd1c/overlay.ld is unchanged. Eighteen instructions: a sound, a table-driven call, a |
| `OvlFunc_882_2008398` | `0x02008398` | — | read | fakematch FAKEMATCH -- matched by pinning a register with inline asm, not by finding the construct. Authorised as an interim measure; every  |
| `OvlFunc_882_20083cc` | `0x020083cc` | — | read | fakematch FAKEMATCH -- matched by pinning a register with inline asm, not by finding the construct. Authorised as an interim measure; every  |
| `OvlFunc_882_2008400` | `0x02008400` | — | read | fakematch FAKEMATCH -- matched by pinning a register with inline asm, not by finding the construct. Authorised as an interim measure; every  |
| `OvlFunc_882_20092f0` | `0x020092f0` | — | read | Slotted between ovl_30_c_c_c_a_c_c_c_c_a.o and the rest of the overlay. TWO HELD VALUES AND A FOURTH THAT MUST NOT REUSE EITHER. r5 = 0x2a f |
| `OvlFunc_882_2009600` | `0x02009600` | — | read | OvlFunc_882_2009600  --  0x02009600 Cut out of goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c_c_b.s. A one-shot conversation guard |
| `OvlFunc_882_200998c` | `0x0200998c` | — | read | extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __CutsceneWait(int n); extern unsigned char *__MapActor_GetA |
| `OvlFunc_882_2009a64` | `0x02009a64` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __MapActor_SetPos(int slot, int x, int z); extern void __MapActor_SetSpeed( |
| `OvlFunc_882_200a0fc` | `0x0200a0fc` | — | read | OvlFunc_882_200a0fc  --  0x0200a0fc, cut from Sets the same option on four actors, with the value chosen by two bits of the frame counter sh |
| `OvlFunc_882_200ad28` | `0x0200ad28` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_882_200c378` | `0x0200c378` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_882_200c3c8` | `0x0200c3c8` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_883_2008b28` | `0x02008b28` | — | read | OvlFunc_883_2008b28 Cut out of goldensun/asm//overlays/rom_780898/ovl_30_c_c_a_c_b.s. A three-way conversation on two flags. BUILT WITH CSE_ |
| `OvlFunc_883_2008ba8` | `0x02008ba8` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_883_2008d70` | `0x02008d70` | — | read | Byte-identical to src/overlays/rom_77dd1c/ovl_30_c_c_a_a_b.c -- see that header for the stack-arg-pair lever this family needed. |
| `OvlFunc_883_2008d98` | `0x02008d98` | — | read | overlay in goldensun/overlays/rom_780898/overlay.ld. Byte-identical to src/overlays/rom_77dd1c/ovl_30_c_c_a_a_c.c in a different overlay. AN |
| `OvlFunc_883_2008eb4` | `0x02008eb4` | — | read | OvlFunc_883_2008eb4  --  0x02008eb4 A shop-keeper who has one conversation before a flag is set and a different one after, with the "after"  |
| `OvlFunc_883_20090d8` | `0x020090d8` | — | read | Borrows the camera: copies three words out of actor 0, points the iwram slot at the local copy, walks its third word up over forty frames an |
| `OvlFunc_883_2009244` | `0x02009244` | — | read | overlay in goldensun/overlays/rom_780898/overlay.ld. A cutscene: one map edit, one local four-argument call, one flag set. Its near-twin two |
| `OvlFunc_883_2009280` | `0x02009280` | — | read | the overlay in goldensun/overlays/rom_780898/overlay.ld. The undo of src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_b.c: the same thre |
| `OvlFunc_883_200b45c` | `0x0200b45c` | — | read | struct Actor { unsigned char pad00[8]; int f8; int fc; int f10; unsigned char pad14[0x55 - 0x14]; unsigned char f55; }; extern struct Actor  |
| `OvlFunc_883_200d75c` | `0x0200d75c` | — | read | struct S { unsigned char pad00[9]; unsigned char f9_lo : 2; unsigned char f9_mid : 2; unsigned char f9_hi : 4; }; struct A { unsigned char p |
| `OvlFunc_883_200d928` | `0x0200d928` | — | read | in goldensun/overlays/rom_780898/overlay.ld. One map edit, then two local calls. THE FIRST FUNCTION FOUND BY tools/match_shapes.py --near. I |
| `OvlFunc_883_200da94` | `0x0200da94` | — | read | keeps its name and its slot in goldensun/overlays/rom_780898/overlay.ld is unchanged. Picks one of two actors depending on a save flag and s |
| `OvlFunc_883_200db48` | `0x0200db48` | — | read | OvlFunc_883_200db48  --  0x0200db48, cut from Dresses an actor as a portrait: clear two sprite-attribute fields, set the animation mode, hid |
| `OvlFunc_883_200dcc4` | `0x0200dcc4` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_883_200dd14` | `0x0200dd14` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_884_2008030` | `0x02008030` | — | read | Turns an actor one step toward its target: takes the angle to the target with atan2, clamps the change to +/-0x1000 of a full circle, and ap |
| `OvlFunc_884_20083b4` | `0x020083b4` | — | read | OvlFunc_884_20083b4  --  0x020083b4 OvlFunc_884_2008444  --  0x02008444 Cut from goldensun/asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a. |
| `OvlFunc_884_2008444` | `0x02008444` | — | read | OvlFunc_884_20083b4  --  0x020083b4 OvlFunc_884_2008444  --  0x02008444 Cut from goldensun/asm/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a. |
| `OvlFunc_884_20085e8` | `0x020085e8` | — | read | overlay in goldensun/overlays/rom_784360/overlay.ld. A two-way talk on save bit 0x840. ONE MESSAGE ID IS A SYMBOL AND THE OTHER IS A LITERAL |
| `OvlFunc_884_2008634` | `0x02008634` | — | read | Slotted between ovl_30_c_a_a_a_c_c_a_c_c_a.o and the rest of the overlay. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefi |
| `OvlFunc_884_2008674` | `0x02008674` | — | read | extern unsigned char gScript_884__0200ae34[]; extern unsigned char *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern |
| `OvlFunc_884_200a39c` | `0x0200a39c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_884_200a3ec` | `0x0200a3ec` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_885_2008030` | `0x02008030` | — | read | Turn one step toward the target, one of ten identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360/ov |
| `OvlFunc_885_20080dc` | `0x020080dc` | — | read | extern int _MSG_f76; extern int __GetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Cutscen |
| `OvlFunc_886_2008030` | `0x02008030` | — | read | Split out of that .s; the sibling parts stay as assembly. Turn one step toward the target, one of eleven identical copies -- one per overlay |
| `OvlFunc_886_2008088` | `0x02008088` | — | read | OvlFunc_886_2008088, the whole of goldensun/asm/overlays/rom_786f0c/ovl_30_a_a_c.s. The .s is replaced outright, so no linker-script change  |
| `OvlFunc_886_20081e8` | `0x020081e8` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_886_20084dc` | `0x020084dc` | — | read | OvlFunc_886_20084dc  --  0x020084dc, cut from A shop counter: the attendant opens shop 0x1 when the player is inside the facing arc, and spe |
| `OvlFunc_886_200855c` | `0x0200855c` | — | read | Slotted between ovl_30_c_c_c_c_c_c_c_c_c_c_a.o and the rest of the overlay. A RANGE CHECK WRITTEN AS AN ADDITION OF A NEGATIVE CONSTANT, the |
| `OvlFunc_886_20085d4` | `0x020085d4` | — | read | OvlFunc_886_20085d4  --  0x020085d4, cut from A shop counter: the attendant opens shop 0x3 when the player is inside the facing arc, and spe |
| `OvlFunc_886_20090c0` | `0x020090c0` | — | read | extern unsigned int iwram_3001e40; extern unsigned char *__MapActor_GetActor(int slot); extern void __MapActor_SetPos(int a, int b, int c);  |
| `OvlFunc_887_2008030` | `0x02008030` | — | read | Turn one step toward the target, one of ten identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360/ov |
| `OvlFunc_887_2008118` | `0x02008118` | — | read | A three-message prompt: says the opening line, runs a check, and delivers one of two follow-ups at base+1 or base+2. One of seven identical  |
| `OvlFunc_887_200933c` | `0x0200933c` | — | read | Slotted between ..._a_a.o and the rest of the overlay. A two-way talk. The second arm reaches its follow-up line with `add r5, #1`, so the b |
| `OvlFunc_887_20093b4` | `0x020093b4` | — | read | Overlay 887: two map edits applied back to back. Split out of the seventeen-part chain at neighbouring parts stay as assembly and are listed |
| `OvlFunc_887_20095e8` | `0x020095e8` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_887_2009638` | `0x02009638` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_888_2008070` | `0x02008070` | — | read | First in the run, ahead of the _b piece, in goldensun/overlays/rom_7892c8/overlay.ld. Picks one of five script tables by the sub-area word a |
| `OvlFunc_888_200814c` | `0x0200814c` | — | read | OvlFunc_888_200814c  --  0x0200814c The whole of goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_a_a_a_b.s. Returns the behaviour script  |
| `OvlFunc_888_200827c` | `0x0200827c` | — | read | Slotted between ..._a_a.o and the rest of the overlay. Near-twin of ovl_30_c_c_a_a_a_a_b.c, differing in four message ids. Same treatment: _ |
| `OvlFunc_888_20082ec` | `0x020082ec` | — | read | Slotted between ovl_30_c_c_a_a_a_a_a.o and the rest of the overlay. __ActorMessage is DECLARED (r0 first in the ROM) while __MapActor_SetAni |
| `OvlFunc_888_20084e8` | `0x020084e8` | — | read | AN ELEVEN-ARGUMENT CALL, and it is the clearest demonstration yet of the batch-149 stack-argument rule -- generalised from "each site needs  |
| `OvlFunc_888_20085cc` | `0x020085cc` | — | read | OvlFunc_888_20085cc  --  0x020085cc changes name in goldensun/overlays/rom_7892c8/overlay.ld and nothing else moves. Per-area arrival hook:  |
| `OvlFunc_888_20086e8` | `0x020086e8` | — | read | in goldensun/overlays/rom_7892c8/overlay.ld. A thirteen-call cutscene, straight-line, and a clean example of the declaration lever used SUBT |
| `OvlFunc_888_200a5c4` | `0x0200a5c4` | — | read | struct Ent { unsigned char pad0[5]; unsigned char f5; unsigned char pad6[0x10 - 6]; int f10; }; struct Obj { unsigned char pad0[9]; unsigned |
| `OvlFunc_888_200a660` | `0x0200a660` | — | read | Overlay 888: detach slot 14's per-frame hook and park it at the origin. Split out of asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c.s; the neigh |
| `OvlFunc_888_200a67c` | `0x0200a67c` | — | read | OvlFunc_888_200a67c  --  asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a.s Copy the player's position into this actor, drop it 0x20000 in z,  |
| `OvlFunc_888_200b144` | `0x0200b144` | — | read | OvlFunc_888_200b144  --  asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a.s Per-frame step for a spinning actor: advance y by the short at +0x |
| `OvlFunc_888_200b270` | `0x0200b270` | — | read | Slotted between ovl_30_c_c_a_a_a_c_c_a.o and the rest of the overlay. TWO STACK-ARG PAIRS IN ONE FUNCTION, each named and stored before its  |
| `OvlFunc_888_200b334` | `0x0200b334` | — | read | OvlFunc_888_200b334  --  0x0200b334 The whole of goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_c_c.s. The sanctum attendant's line, chose |
| `OvlFunc_890_2008054` | `0x02008054` | — | read | extern int OvlFunc_890_200a5b0(void); extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __ClearFlag(int id); extern v |
| `OvlFunc_890_2008108` | `0x02008108` | — | read | A one-shot scene: if a flag is clear, run it, set that flag and clear two neighbouring ones. Built with CSE_CFLAGS (-fno-rerun-cse-after-loo |
| `OvlFunc_890_2008150` | `0x02008150` | — | read | OvlFunc_890_2008150  --  0x02008150 changes name in goldensun/overlays/rom_78b2ac/overlay.ld and nothing else moves. Two mutually exclusive  |
| `OvlFunc_890_20081ec` | `0x020081ec` | — | read | Slotted between ovl_30_c_c_a_a_c_a.o and the rest of the overlay. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile. A fl |
| `OvlFunc_890_2008238` | `0x02008238` | — | read | OvlFunc_890_2008238  --  0x02008238 OvlFunc_890_20082cc  --  0x020082cc OvlFunc_890_2008360  --  0x02008360 OvlFunc_890_20083f4  --  0x02008 |
| `OvlFunc_890_20082cc` | `0x020082cc` | — | read | OvlFunc_890_2008238  --  0x02008238 OvlFunc_890_20082cc  --  0x020082cc OvlFunc_890_2008360  --  0x02008360 OvlFunc_890_20083f4  --  0x02008 |
| `OvlFunc_890_2008360` | `0x02008360` | — | read | OvlFunc_890_2008238  --  0x02008238 OvlFunc_890_20082cc  --  0x020082cc OvlFunc_890_2008360  --  0x02008360 OvlFunc_890_20083f4  --  0x02008 |
| `OvlFunc_890_20083f4` | `0x020083f4` | — | read | OvlFunc_890_2008238  --  0x02008238 OvlFunc_890_20082cc  --  0x020082cc OvlFunc_890_2008360  --  0x02008360 OvlFunc_890_20083f4  --  0x02008 |
| `OvlFunc_891_2008054` | `0x02008054` | — | read | Two flag pairs select an argument, or the function refuses with -1. THE `-1` EXIT IS A `goto` TO THE END, not two `return -1`s. Both refusal |
| `OvlFunc_891_200901c` | `0x0200901c` | — | read | Slotted between ovl_30_c_c_a_c_c_a_a_a.o and the rest of the overlay. THE BASIC-BLOCK LEVER, TWICE IN ONE FUNCTION, on two different blocker |
| `OvlFunc_891_2009624` | `0x02009624` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __Func_8012078(int a, int b, int c, int d); void OvlFunc_891_2009624(void)  |
| `OvlFunc_891_200995c` | `0x0200995c` | — | read | OvlFunc_891_200995c Cut out of goldensun/asm//overlays/rom_78c76c/ovl_30_c_c_a_c_c_c_c_a_c_c_b.s. Walks the guard and the player together on |
| `OvlFunc_891_200a244` | `0x0200a244` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_891_200a2f4` | `0x0200a2f4` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_892_2008054` | `0x02008054` | — | read | SPLIT BY HAND, after getting it wrong. The .s held this function AND a .data section of four .incbin blobs, and I deleted it having checked  |
| `OvlFunc_893_2008054` | `0x02008054` | — | read | Code to this file, the trailing .section .data to its _c sibling. A STRUCT POINTER IS A REGISTER-ALLOCATION LEVER, not just a readability ch |
| `OvlFunc_894_2008054` | `0x02008054` | — | read | Code to this file, the trailing .section .data to its _c sibling. Byte-identical twin of OvlFunc_893_2008054 in overlay rom_78dd40; one solu |
| `OvlFunc_895_2008030` | `0x02008030` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. GetEntrances, three-way form: selects one of three edge-transiti |
| `OvlFunc_895_200807c` | `0x0200807c` | — | read | extern unsigned char gState[]; extern int _AREA_10; extern int _AREA_13; extern unsigned char L1fd8[] __asm__(".L1fd8"); extern unsigned cha |
| `OvlFunc_895_20080ec` | `0x020080ec` | — | read | extern unsigned char gState[]; extern int _AREA_10; extern int _AREA_13; extern unsigned char L1fd8[] __asm__(".L1fd8"); extern unsigned cha |
| `OvlFunc_895_20088f4` | `0x020088f4` | — | read | Slotted between ovl_30_c_c_c_a_a.o and the rest of the overlay. Area dispatch, two-arm form: read the area halfword at gState+0x1c0 and call |
| `OvlFunc_896_2008314` | `0x02008314` | — | read | Turn one step toward the target, one of ten identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360/ov |
| `OvlFunc_896_200a674` | `0x0200a674` | — | read | A three-way talk on two save flags. One of a twin pair differing only in the actor slot and the four message ids -- see the sibling file. TH |
| `OvlFunc_896_200a6e0` | `0x0200a6e0` | — | read | further split was needed. A three-way talk on two save flags. One of a twin pair differing only in the actor slot and the four message ids - |
| `OvlFunc_896_200c328` | `0x0200c328` | — | read | OvlFunc_896_200c328  --  0x0200c328 Cut out of goldensun/asm/overlays/rom_78ef88/ovl_314_c_c_c_c.s. The forced-sale shopkeeper: he will not  |
| `OvlFunc_896_200c3bc` | `0x0200c3bc` | — | read | The target was the FIRST of three functions, so there is no _a part; the other two and the trailing .data travel together in _c. Spawns a bu |
| `OvlFunc_897_2008f64` | `0x02008f64` | — | read | struct Sub { unsigned char pad0[9]; unsigned char lo : 2; unsigned char sel : 2; unsigned char hi : 4; unsigned char pad_a[0x14]; short f1e; |
| `OvlFunc_897_200a8dc` | `0x0200a8dc` | — | read | OvlFunc_897_200a8dc  --  0x0200a8dc The second function of goldensun/asm/overlays/rom_791794/ovl_30_c_c_a_c_a_c.s; OvlFunc_897_200a84c stays |
| `OvlFunc_897_200aba0` | `0x0200aba0` | — | read | struct E { unsigned char pad00[0xc]; int f0c; unsigned char pad10[8]; int f18; int f1c; unsigned char pad20[0x1c]; int f3c; }; extern unsign |
| `OvlFunc_897_200ac9c` | `0x0200ac9c` | — | read | extern void __Func_8010704(int sx, int sy, int w, int h, int dx, int dy); void OvlFunc_897_200ac9c(void) { int z; int e; int n; int s; int t |
| `OvlFunc_897_200ae0c` | `0x0200ae0c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_897_200ae5c` | `0x0200ae5c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_898_2008314` | `0x02008314` | — | read | OvlFunc_898_2008314  --  0x02008314, cut from Hands an actor to the interaction handler twice: once with kind 0x20, and if that comes back z |
| `OvlFunc_898_20087ec` | `0x020087ec` | — | read | One of three byte-identical siblings in overlay rom_793768, differing only in the actor slot and the message id. From the branch-over-pool c |
| `OvlFunc_898_200885c` | `0x0200885c` | — | read | One of three byte-identical siblings in overlay rom_793768, differing only in the actor slot and the message id. From the branch-over-pool c |
| `OvlFunc_898_2008938` | `0x02008938` | — | read | keeps its name and its slot in goldensun/overlays/rom_793768/overlay.ld is unchanged. INSTRUCTION-FOR-INSTRUCTION IDENTICAL to src/overlays/ |
| `OvlFunc_898_2008a4c` | `0x02008a4c` | — | read | OvlFunc_898_2008a4c Cut out of goldensun/asm//overlays/rom_793768/ovl_314_c_c_a_c_c_c_a_a_b.s. WAS PARKED, AND THE PARKED C WAS ALREADY CORR |
| `OvlFunc_898_2008acc` | `0x02008acc` | — | read | One of three byte-identical siblings in overlay rom_793768, differing only in the actor slot and the message id. From the branch-over-pool c |
| `OvlFunc_898_2008cfc` | `0x02008cfc` | — | read | OvlFunc_898_2008cfc  --  0x02008cfc OvlFunc_898_2008d78  --  0x02008d78 The whole of goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a |
| `OvlFunc_898_2008d78` | `0x02008d78` | — | read | OvlFunc_898_2008cfc  --  0x02008cfc OvlFunc_898_2008d78  --  0x02008d78 The whole of goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_a_c_a |
| `OvlFunc_898_2008ea4` | `0x02008ea4` | — | read | Same family as src/overlays/rom_77dd1c/ovl_30_c_c_a_a_b.c, differing only in its five constants. See that header for the stack-arg-pair leve |
| `OvlFunc_898_2008ecc` | `0x02008ecc` | — | read | overlay in goldensun/overlays/rom_793768/overlay.ld. 0x17 appears three times here -- as both leading arguments and as the value stored at [ |
| `OvlFunc_898_2008f3c` | `0x02008f3c` | — | read | A sound, a table-driven call, and a placement. Fifteen instructions. THE DECLARATION LEVER REORDERS MORE THAN r0. Every previous use of it i |
| `OvlFunc_898_2008f64` | `0x02008f64` | — | read | The remaining .s held ONLY this function and no data, so no further split was needed. Twin of src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_ |
| `OvlFunc_898_2008fb4` | `0x02008fb4` | — | read | OvlFunc_898_2008fb4  --  0x02008fb4 OvlFunc_898_2009010  --  0x02009010 Cut from goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_a.s |
| `OvlFunc_898_2009010` | `0x02009010` | — | read | OvlFunc_898_2008fb4  --  0x02008fb4 OvlFunc_898_2009010  --  0x02009010 Cut from goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_a.s |
| `OvlFunc_898_200906c` | `0x0200906c` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_793768/overlay.ld, so the ROM layo |
| `OvlFunc_898_200913c` | `0x0200913c` | — | read | OvlFunc_898_200913c  --  0x0200913c, cut from Opens a door: play the sound, run the tile animation, repaint the attribute block, and clear o |
| `OvlFunc_898_20091b0` | `0x020091b0` | — | read | OvlFunc_898_20091b0  --  0x020091b0, cut from goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_c_a.s. The cut was at the head of the .s,  |
| `OvlFunc_898_2009238` | `0x02009238` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_898_2009674` | `0x02009674` | — | read | extern int OvlFunc_898_2009638(int *a, int *b); extern int __atan2(int y, int x); extern void __Actor_SetAnim(char *a, int anim); int OvlFun |
| `OvlFunc_899_2008048` | `0x02008048` | — | read | typedef struct { unsigned char _bytes[704]; } GlobalState; extern GlobalState gState; extern unsigned char L5cc8[] __asm__(".L5cc8"); extern |
| `OvlFunc_899_2008080` | `0x02008080` | — | read | struct Actor { unsigned char pad00[6]; unsigned short f6; }; extern struct Actor *__MapActor_GetActor(int slot); extern void __Func_8093c00( |
| `OvlFunc_899_2008310` | `0x02008310` | — | read | typedef struct { unsigned char pad00[0x1c2]; short f1c2; unsigned char pad1c4[0x2c0 - 0x1c4]; } GlobalState; extern GlobalState gState; exte |
| `OvlFunc_899_2008378` | `0x02008378` | — | read | keeps its name and its slot in goldensun/overlays/rom_794ac0/overlay.ld is unchanged. Sets a per-actor byte while a short cutscene plays, th |
| `OvlFunc_899_200852c` | `0x0200852c` | — | read | A one-shot conversation: check two save flags, say one of three lines, and on the first pass set a flag and run a short beat. PARKED ON cons |
| `OvlFunc_899_200891c` | `0x0200891c` | — | read | extern unsigned char *iwram_3001ebc; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __CutsceneWait(int n);  |
| `OvlFunc_899_200c698` | `0x0200c698` | — | read | Placed in the run in goldensun/overlays/rom_794ac0/overlay.ld. Byte-identical to OvlFunc_902_2008570 in overlays/rom_7987ac; this C is share |
| `OvlFunc_899_200c704` | `0x0200c704` | — | read | extern const unsigned char _TBL_4f2c[] __asm__(".L4f2c"); unsigned char *OvlFunc_899_200c704(int *p) { unsigned char *t; unsigned char *r; i |
| `OvlFunc_899_200c7bc` | `0x0200c7bc` | — | read | extern void *__GetFieldActor(int id); int OvlFunc_899_200c7bc(int x, int y, int id) { int *a; int ax; int ay; int m; int r; int p; int q; a  |
| `OvlFunc_899_200c7fc` | `0x0200c7fc` | — | read | extern void *__GetFieldActor(int id); int OvlFunc_899_200c7fc(int x, int y, int id) { int *a; int ax; int ay; int m; int r; int p; int q; a  |
| `OvlFunc_899_200c840` | `0x0200c840` | — | read | OvlFunc_899_200c840  --  0x0200c840 Cut out of goldensun/asm/overlays/rom_794ac0/ovl_30_c_c_c_c_c_c_a_b.s. Validates a two-byte tile coordin |
| `OvlFunc_900_20081e4` | `0x020081e4` | — | read | Code to this file, the trailing .section .data to its _c sibling. TWENTY-FIVE SPELLINGS. The plain `\|=` body is 24 differing: gcc cross-jum |
| `OvlFunc_901_2008350` | `0x02008350` | — | read | PARKED ON "REGISTER PRESSURE" AND THAT WAS THE WRONG DIAGNOSIS.  The old note read the r8-r11 spills off the prologue, observed that the ROM |
| `OvlFunc_901_2008400` | `0x02008400` | — | read | OvlFunc_901_2008400  --  0x02008400, cut from Hands an actor to the interaction handler twice: once with kind 0x20, and if that comes back z |
| `OvlFunc_901_20084b4` | `0x020084b4` | — | read | Overlay 901: a talk stub taking its slot as an argument. Split out of asm/overlays/rom_797990/ovl_314_c_c_a_a_a.s; the neighbouring parts st |
| `OvlFunc_901_20084d8` | `0x020084d8` | — | read | Talk with staging: turns slot 8 toward the player, records save bit 0x305, and delivers line 0x1cab. __Func_809280c is deliberately left und |
| `OvlFunc_901_2008754` | `0x02008754` | — | read | Overlay 901: hold an actor still while it delivers a line. Whole-file conversion of asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_c.s -- i |
| `OvlFunc_901_20087d4` | `0x020087d4` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in The same shape as OvlFunc_901_2008754 in src/overlays/rom_797990/o |
| `OvlFunc_901_2008804` | `0x02008804` | — | read | OvlFunc_901_2008804  --  0x02008804 OvlFunc_901_2008864  --  0x02008864 Cut from the head of remaining function follows as ovl_314_c_c_a_a_c |
| `OvlFunc_901_2008864` | `0x02008864` | — | read | OvlFunc_901_2008804  --  0x02008804 OvlFunc_901_2008864  --  0x02008864 Cut from the head of remaining function follows as ovl_314_c_c_a_a_c |
| `OvlFunc_901_2008970` | `0x02008970` | — | read | OvlFunc_901_2008970  --  0x02008970, cut from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c_a.s. Sends an actor to a target an |
| `OvlFunc_901_20089f8` | `0x020089f8` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_901_2008ac8` | `0x02008ac8` | — | read | A CROSS-OVERLAY TWIN of src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_b.c and its own sibling, found by tools/match_shapes.py: same instru |
| `OvlFunc_901_2008af0` | `0x02008af0` | — | read | further split was needed. A CROSS-OVERLAY TWIN of src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_b.c and its own sibling, found by tools/ma |
| `OvlFunc_901_2008b40` | `0x02008b40` | — | read | OvlFunc_901_2008b40  --  0x02008b40 OvlFunc_901_2008b9c  --  0x02008b9c Cut from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_a.s |
| `OvlFunc_901_2008b9c` | `0x02008b9c` | — | read | OvlFunc_901_2008b40  --  0x02008b40 OvlFunc_901_2008b9c  --  0x02008b9c Cut from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_a.s |
| `OvlFunc_901_2008bf8` | `0x02008bf8` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_797990/overlay.ld, so the ROM layo |
| `OvlFunc_901_2008cc8` | `0x02008cc8` | — | read | OvlFunc_901_2008cc8  --  0x02008cc8 Cut from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_c_a.s. More of the door-opening family  |
| `OvlFunc_901_2008d24` | `0x02008d24` | — | read | Byte-identical to src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_b.c -- see src/overlays/rom_77dd1c/ovl_30_c_c_a_a_b.c for the lever. |
| `OvlFunc_901_2008d4c` | `0x02008d4c` | — | read | Byte-identical to src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_a_b.c in a different overlay. ANOTHER MEMBER of the nine-function family h |
| `OvlFunc_901_2008e30` | `0x02008e30` | — | read | Slotted between ovl_314_c_c_a_c_a_a_a.o and the rest of the overlay. TWO STACK-ARG PAIRS. The second one stores the SAME register into both  |
| `OvlFunc_901_2008e60` | `0x02008e60` | — | read | OvlFunc_901_2008e60 extracted from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_c_a_a_c.s. Near-twin of ovl_314_c_c_a_c_a_a_b.c; see that |
| `OvlFunc_902_20084e4` | `0x020084e4` | — | read | OvlFunc_902_20084e4  --  0x020084e4 keeps its name (`asm/%.o: src/%.c`) and the linker script is unchanged. Arrival fixups for one map, by e |
| `OvlFunc_903_2008348` | `0x02008348` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __SetFlag(int id); extern void __ClearFlag(int id); extern void __Func_8010 |
| `OvlFunc_903_2008d04` | `0x02008d04` | — | read | OvlFunc_903_2008d04  --  0x02008d04, cut from Opens a passage, but only from one tile row: read the player's z as a whole tile, and if it is |
| `OvlFunc_903_2008d68` | `0x02008d68` | — | read | extern unsigned int iwram_3001f30; extern void __Func_8096fb0(int a, int b); extern void __Func_80970f8(int a, int b); extern void __Func_80 |
| `OvlFunc_904_2008054` | `0x02008054` | — | read | Code to this file, the trailing .section .data to its _c sibling. THE BASIC-BLOCK LEVER, IN ITS WORKING DIRECTION. First body was 2 differin |
| `OvlFunc_905_2008a00` | `0x02008a00` | — | read | OvlFunc_905_2008a00  --  asm/overlays/rom_799abc/ovl_30_a_a_a_c_c_c_c.s A three-case switch on the short at +0x66 that adds the two velocity |
| `OvlFunc_905_20090c8` | `0x020090c8` | — | read | OvlFunc_905_20090c8  --  0x020090c8 Cut out of goldensun/asm/overlays/rom_799abc/ovl_30_c_c_c.s. A frame-counted cutscene beat table: tick a |
| `OvlFunc_906_2008314` | `0x02008314` | — | read | GetEntrances. Picks one of two edge-transition tables from a gState halfword. Head of a 22-member family, the largest in the overlays. This  |
| `OvlFunc_906_2008350` | `0x02008350` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_906_20083e4` | `0x020083e4` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_906_2008414` | `0x02008414` | — | read | typedef struct { unsigned char _bytes[704]; } GlobalState; extern GlobalState gState; extern unsigned int iwram_3001ebc; extern int _AREA_1c |
| `OvlFunc_907_2008030` | `0x02008030` | — | read | Turn one step toward the target, one of ten identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360/ov |
| `OvlFunc_907_2008088` | `0x02008088` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_907_20080dc` | `0x020080dc` | — | read | Returns a table pointer in one area and NULL everywhere else. UNPARKED, AND IT HAD TWO WRONG SYMBOLS. The park read "logic faithful, does NO |
| `OvlFunc_907_200811c` | `0x0200811c` | — | read | GetEntrances, 4-way form. Returns a named global from at least one arm, which is why the family sweeps in batches 08-13 missed it -- they ma |
| `OvlFunc_907_2008198` | `0x02008198` | — | read | GetEntrances, four-way form: selects one of four edge-transition tables from a gState halfword, falling through to the last. One of a 24-mem |
| `OvlFunc_907_2008240` | `0x02008240` | — | read | A conversation that bumps a counter the second time through. Built with CSE_CFLAGS: the flag id 0x301 is read at the top and written at the  |
| `OvlFunc_907_2008890` | `0x02008890` | — | read | Writes an interaction word, then dispatches on the area id. TWO THINGS SHARE ONE VARIABLE HERE, deliberately. `off` is 0xe0 << 1 and is used |
| `OvlFunc_907_20088f0` | `0x020088f0` | — | read | extern unsigned int gState; extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __ClearFlag(int id); extern void __MapA |
| `OvlFunc_907_20089cc` | `0x020089cc` | — | read | The target was the FIRST of two functions, so there is no _a part. Reads tile coordinates out of two actors (>> 20, arithmetic -- the ROM us |
| `OvlFunc_907_2008cb4` | `0x02008cb4` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __Func_8010704(int a, int b, int c, int d, int e, int f); extern unsigned c |
| `OvlFunc_907_2008d10` | `0x02008d10` | — | read | OvlFunc_907_2008d10  --  0x02008d10, cut from Arm the next map, then -- on two of the area ids and only while a save bit is clear -- snap th |
| `OvlFunc_907_2008f3c` | `0x02008f3c` | — | read | extern unsigned char *__CreateActor(int a, int b, int c, int d); extern void __Actor_SetScript(void *a, void *s); extern void __Sprite_SetAn |
| `OvlFunc_907_2008fa0` | `0x02008fa0` | — | read | file, data into ovl_30_c_c_c_c.s. WHY THE HAND SPLIT, AND WHY IT IS SAFE. The original .s held this function followed by a `.section .data`  |
| `OvlFunc_908_2008124` | `0x02008124` | — | read | A three-way talk gated on the player's facing and a save flag. FOUND BY tools/find_bb_lever.py, which lists functions the basic-block lever  |
| `OvlFunc_908_20081a8` | `0x020081a8` | — | read | fakematch further split was needed. FAKEMATCH -- matched by pinning a register with inline asm, not by finding the construct. Authorised as  |
| `OvlFunc_908_20084c8` | `0x020084c8` | — | read | The .data section stays behind in the original .s, which keeps its name and its .data line in goldensun/overlays/rom_79c0c4/overlay.ld; only |
| `OvlFunc_909_2008030` | `0x02008030` | — | read | Turn one step toward the target, one of ten identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360/ov |
| `OvlFunc_909_2008100` | `0x02008100` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_909_20081b4` | `0x020081b4` | — | read | A one-shot line of dialogue: say the standard message, add a second one the first time through, then speak and set the flag. The near-twin o |
| `OvlFunc_909_200828c` | `0x0200828c` | — | read | A one-shot line of dialogue: say the standard message, add a second one the first time through, then speak and set the flag. THIS WAS PARKED |
| `OvlFunc_909_2008338` | `0x02008338` | — | read | extern unsigned char *iwram_3001ebc; extern int __GetFlag(int id); extern void __PlaySound(int id); extern void __Func_80118a8(int n); exter |
| `OvlFunc_909_20084ec` | `0x020084ec` | — | read | OvlFunc_909_20084ec  --  0x020084ec, cut from the head of The cut was at the head of the .s, so this file takes the first .text slot and the |
| `OvlFunc_909_2008568` | `0x02008568` | — | read | extern int __GetFlag(int flag); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __CutsceneWait(int n); exter |
| `OvlFunc_909_20085f4` | `0x020085f4` | — | read | extern unsigned char *iwram_3001ebc; extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern  |
| `OvlFunc_910_2008030` | `0x02008030` | — | read | Turn one step toward the target, one of eleven identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360 |
| `OvlFunc_910_200809c` | `0x0200809c` | — | read | Placed in the run in goldensun/overlays/rom_79dd90/overlay.ld. Picks the area's script table, patching two bytes inside it first when the co |
| `OvlFunc_910_2008154` | `0x02008154` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_910_20084bc` | `0x020084bc` | — | read | Placed in the run in goldensun/overlays/rom_79dd90/overlay.ld. Seeds a word in the area block, resets actor 8's flag byte, sets its sprite's |
| `OvlFunc_910_200850c` | `0x0200850c` | — | read | OvlFunc_910_200850c  --  0x0200850c Four independent arrival fixups, each gated on its own flag. EIGHT LEVERED CONSTANTS, which is the most  |
| `OvlFunc_910_20088e8` | `0x020088e8` | — | read | Overlay 910: play a sound, run a map edit, record it happened. Split out of asm/overlays/rom_79dd90/ovl_30_c_c_c_c_a_c.s; the neighbouring p |
| `OvlFunc_910_2008974` | `0x02008974` | — | read | OvlFunc_910_2008974  --  0x02008974, cut from Dresses an actor as a portrait: clear two sprite-attribute fields, set the animation mode, hid |
| `OvlFunc_911_2008050` | `0x02008050` | — | read | A byte-for-byte cross-overlay copy of src/overlays/rom_7a04ac/ovl_30_a_a_c.c. Both levers are that file's and both are needed here: - actor  |
| `OvlFunc_911_20080a0` | `0x020080a0` | — | read | Placed in the run in goldensun/overlays/rom_79e5c0/overlay.ld. Initialises nine 0x18-byte slots starting at +0x48: a type halfword that is 0 |
| `OvlFunc_911_20080cc` | `0x020080cc` | — | read | OvlFunc_911_20080cc, the whole of goldensun/asm/overlays/rom_79e5c0/ovl_30_a_c_a_a_c_b.s. no linker-script change was needed. One per-frame  |
| `OvlFunc_911_2008114` | `0x02008114` | — | read | Split out of that .s; the sibling parts stay as assembly. Turn one step toward the target, one of eleven identical copies -- one per overlay |
| `OvlFunc_911_200816c` | `0x0200816c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_911_20081ac` | `0x020081ac` | — | read | Returns a table pointer in one area and NULL everywhere else. UNPARKED, AND THE PARK WAS WRONG ABOUT WHAT WAS BLOCKING IT. It was filed as " |
| `OvlFunc_911_20081dc` | `0x020081dc` | — | read | OvlFunc_911_20081dc extracted from goldensun/asm/overlays/rom_79e5c0/ovl_30_c_a_a_a_a.s. A script selector with a SIDE EFFECT in one arm: ar |
| `OvlFunc_911_2008230` | `0x02008230` | — | read | Another sanctum attendant: stand in the right facing arc and the menu opens, otherwise you get a line of dialogue. Same shape as src/overlay |
| `OvlFunc_911_2008284` | `0x02008284` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_911_2008304` | `0x02008304` | — | read | OvlFunc_911_2008304  --  0x02008304 Cut out of goldensun/asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c.s. Opens a cutscene, picks a message |
| `OvlFunc_911_2008800` | `0x02008800` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern unsigned int iwram_3001e40; void OvlFunc_911_2008800(void) { unsigned char *a; i |
| `OvlFunc_911_200a608` | `0x0200a608` | — | read | OvlFunc_911_200a608  --  0x0200a608 keeps its name and the linker script is unchanged. One frame in eight, spawn a falling-debris actor. THI |
| `OvlFunc_911_200a910` | `0x0200a910` | — | read | OvlFunc_911_200a910  --  0x0200a910 Cut out of goldensun/asm/overlays/rom_79e5c0/ovl_30_c_c.s. Two independent map fixups on arrival, each r |
| `OvlFunc_913_20089fc` | `0x020089fc` | — | read | An actor update. Advances the gradual-turn target at +0x64 by a random amount, picks one of two configuration values from it, and wraps it t |
| `OvlFunc_913_2008a68` | `0x02008a68` | — | read | typedef struct { int a; int b; int c; int d; int e; int f; } S; extern int OvlFunc_913_2008474(S *s); extern void OvlFunc_913_2008608(S s);  |
| `OvlFunc_913_2008b1c` | `0x02008b1c` | — | read | extern char *__MapActor_GetActor(int slot); extern int __TestCollision(char *a, int *v); extern void __CutsceneStart(void); extern void __Cu |
| `OvlFunc_913_2008c68` | `0x02008c68` | — | read | OvlFunc_913_2008244 IS DELIBERATELY LEFT UNDECLARED. The first transcription was 4 of 85, all at that one call: the ROM wants `mov r0, #0x2` |
| `OvlFunc_913_200a7c8` | `0x0200a7c8` | — | read | OvlFunc_913_200a7c8  --  0x0200a7c8 keeps its name and the linker script is unchanged. One frame in eight, spawn a falling-debris actor: pla |
| `OvlFunc_913_200aad8` | `0x0200aad8` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern unsigned int iwram_3001e40; void OvlFunc_913_200aad8(void) { unsigned char *a; i |
| `OvlFunc_914_20089f8` | `0x020089f8` | — | read | Near-twin of src/overlays/rom_7d0e88/ovl_1528_c_c_c_c_c_b.c -- the same 24-byte struct passed by value, with no else arm. Read that header f |
| `OvlFunc_914_2008abc` | `0x02008abc` | — | read | OvlFunc_914_2008abc  --  0x02008abc, cut from The palette fade driver: walk palette entries 0..0xdf, skipping two ranges, and pass each colo |
| `OvlFunc_914_2008b24` | `0x02008b24` | — | read | OvlFunc_914_2008b24  --  0x02008b24, cut from goldensun/asm/overlays/rom_7a1ff0/ovl_30_c_c_c_c_a.s. A BGR555 warming fade: pull `rate`-th pa |
| `OvlFunc_914_2008bcc` | `0x02008bcc` | — | read | Slotted between ovl_30_c_c_c_c_c_c_a_a.o and the rest of the overlay. Two DMA3 transfers of 0x1c0 bytes from palette RAM into the iwram_3001 |
| `OvlFunc_914_2008c0c` | `0x02008c0c` | — | read | #include "dma.h" extern void *iwram_3001ed0; extern unsigned char L17b0[] __asm__(".L17b0"); extern unsigned char L10b0[] __asm__(".L10b0"); |
| `OvlFunc_914_2008cb4` | `0x02008cb4` | — | read | OvlFunc_914_2008cb4  --  0x02008cb4, cut from Dresses an actor as a portrait: clear two sprite-attribute fields, set the animation mode, hid |
| `OvlFunc_915_2008aac` | `0x02008aac` | — | read | extern char *__MapActor_GetActor(int slot); extern int __TestCollision(char *a, int *v); extern void __CutsceneStart(void); extern void __Cu |
| `OvlFunc_915_2008bf8` | `0x02008bf8` | — | read | extern unsigned char *iwram_3001ebc; extern int OvlFunc_915_20088c0(int a); extern int __GetFlag(int id); extern unsigned char *__MapActor_G |
| `OvlFunc_915_2008c8c` | `0x02008c8c` | — | read | OvlFunc_915_2008c8c  --  0x02008c8c, cut from The palette fade driver: walk palette entries 0..0xdf, skipping two ranges, and pass each colo |
| `OvlFunc_915_2008cf4` | `0x02008cf4` | — | read | OvlFunc_915_2008cf4  --  0x02008cf4, cut from goldensun/asm/overlays/rom_7a2bf0/ovl_30_c_c_c_a.s. A BGR555 warming fade: pull `rate`-th part |
| `OvlFunc_915_2008d9c` | `0x02008d9c` | — | read | Slotted between ovl_30_c_c_c_c_c_a.o and the rest of the overlay. Two DMA3 transfers of 0x1c0 bytes from palette RAM into the iwram_3001ed0  |
| `OvlFunc_915_2008ddc` | `0x02008ddc` | — | read | #include "dma.h" extern void *iwram_3001ed0; extern unsigned char L17e0[] __asm__(".L17e0"); extern unsigned char L10e0[] __asm__(".L10e0"); |
| `OvlFunc_916_2008054` | `0x02008054` | — | read | .L12c0 is a 4-byte .lcomm in ovl_30_c_c_c_c_c.s, read as a pointer here and passed to two different overlay routines. The ROM keeps its ADDR |
| `OvlFunc_916_2008150` | `0x02008150` | — | read | extern int L12c4 __asm__(".L12c4"); extern void __Func_8010704(int a, int b, int c, int d, int e, int f); void OvlFunc_916_2008150(void) { s |
| `OvlFunc_916_200836c` | `0x0200836c` | — | read | The same function as src/overlays/rom_7d0e88/ovl_1528_a_a_a_c_c.c with a different VCOUNT bound (0x34 rather than 0x2e) and different tables |
| `OvlFunc_916_2008b3c` | `0x02008b3c` | — | read | struct Region { short f0; short x0; short z0; short flag; int f8; }; extern unsigned char gBuffer[]; void OvlFunc_916_2008b3c(struct Region  |
| `OvlFunc_916_2008be4` | `0x02008be4` | — | read | extern unsigned char gBuffer[]; extern unsigned char ewram_202c000[]; int OvlFunc_916_2008be4(int x, int z, int f) { unsigned char *q; int i |
| `OvlFunc_916_2008e64` | `0x02008e64` | — | read | OvlFunc_916_2008e64  --  0x02008e64, cut from The palette fade driver: walk palette entries 0..0xdf, skipping two ranges, and pass each colo |
| `OvlFunc_916_2008ecc` | `0x02008ecc` | — | read | OvlFunc_916_2008ecc  --  0x02008ecc, cut from goldensun/asm/overlays/rom_7a37f0/ovl_30_c_c_c_a_c.s. A BGR555 warming fade: pull `rate`-th pa |
| `OvlFunc_916_2008f74` | `0x02008f74` | — | read | Slotted between ovl_30_c_c_c_c_c_a.o and the rest of the overlay. Two DMA3 transfers of 0x1c0 bytes from palette RAM into the iwram_3001ed0  |
| `OvlFunc_916_2008fb4` | `0x02008fb4` | — | read | #include "dma.h" extern void *iwram_3001ed0; extern unsigned char L19d0[] __asm__(".L19d0"); extern unsigned char L12d0[] __asm__(".L12d0"); |
| `OvlFunc_917_2009218` | `0x02009218` | — | read | OvlFunc_917_2009218  --  0x02009218 keeps its name and the linker script is unchanged. One frame in four, spawn a falling-debris actor. THIS |
| `OvlFunc_917_20092b4` | `0x020092b4` | — | read | A thirty-frame cycle counter in an overlay-local word: two events at frames 0 and 0x14, then increment and wrap. BUILT AT -O1, AND IT IS NOT |
| `OvlFunc_917_200952c` | `0x0200952c` | — | read | OvlFunc_917_200952c  --  0x0200952c Cut out of goldensun/asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_c.s. The per-frame task for one falling- |
| `OvlFunc_917_200972c` | `0x0200972c` | — | read | A cross-overlay copy of src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_b.c, found by tools/match_shapes.py. Read that file for the two load-bearing  |
| `OvlFunc_917_2009768` | `0x02009768` | — | read | OvlFunc_917_2009768  --  0x02009768, cut from The palette fade driver: walk palette entries 0..0xdf, skipping two ranges, and pass each colo |
| `OvlFunc_917_20097d0` | `0x020097d0` | — | read | OvlFunc_917_20097d0  --  0x020097d0, cut from goldensun/asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_c.s. A BGR555 warming fade: pull `rate`-th  |
| `OvlFunc_917_2009878` | `0x02009878` | — | read | Slotted between ovl_30_c_c_c_c_c_c_a.o and the rest of the overlay. Two DMA3 transfers of 0x1c0 bytes from palette RAM into the iwram_3001ed |
| `OvlFunc_917_20098b8` | `0x020098b8` | — | read | #include "dma.h" extern void *iwram_3001ed0; extern unsigned char L24e0[] __asm__(".L24e0"); extern unsigned char L1de0[] __asm__(".L1de0"); |
| `OvlFunc_918_2008f58` | `0x02008f58` | — | read | Six six-argument calls, and it matched on the first screen because the batch-149/150 stack-argument reading answers every one of them off th |
| `OvlFunc_918_20095ac` | `0x020095ac` | — | read | struct Actor { unsigned char pad00[8]; int x; int y; int z; unsigned char pad14[0x18 - 0x14]; int f18; int f1c; unsigned char pad20[0x38 - 0 |
| `OvlFunc_918_200985c` | `0x0200985c` | — | read | A per-frame integrator with damping: add the stored velocities to the position, bleed 1/0x12 off the X velocity and 1/16 off the Z, then add |
| `OvlFunc_919_2008200` | `0x02008200` | — | read | split_s.py. The previous park was right about everything it claimed, and its offset-clobber head is kept verbatim. Its remaining 18 differin |
| `OvlFunc_920_2008040` | `0x02008040` | — | read | GetEntrances, four-way form: selects one of four edge-transition tables from a gState halfword, falling through to the last. Head of a 24-me |
| `OvlFunc_920_20080a0` | `0x020080a0` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_920_20080f4` | `0x020080f4` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. GetEntrances, 4-way form: sele |
| `OvlFunc_920_2008148` | `0x02008148` | — | read | Slotted between ovl_30_c_a_c_a_c_c_a.o and the rest of the overlay. Stack-arg pair named as two locals, stored before the call. The callee I |
| `OvlFunc_920_2008168` | `0x02008168` | — | read | Slotted between ovl_30_c_a_c_a_c_c_c_a.o and the rest of the overlay. Stack-arg pair named as two locals, stored before the call. The callee |
| `OvlFunc_920_2008188` | `0x02008188` | — | read | Slotted between ovl_30_c_a_c_a_c_c_c_c_a.o and the rest of the overlay. Two stack-arg pairs, each named as its own two locals and stored bef |
| `OvlFunc_920_20081bc` | `0x020081bc` | — | read | OvlFunc_920_20081bc extracted from goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_a_c_a_c_c_c_c_c.s. Near-twin of ovl_30_c_a_c_a_c_c_c_c_b.c imm |
| `OvlFunc_920_2008280` | `0x02008280` | — | read | Overlay 920: react-and-turn cutscene stub for slot 0xF. Split out of asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a.s. One of three identical stub |
| `OvlFunc_920_20082ac` | `0x020082ac` | — | read | Overlay 920: react-and-turn cutscene stub for slot 0x10. Split out of asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a.s. One of three identical stu |
| `OvlFunc_920_20082d8` | `0x020082d8` | — | read | Overlay 920: react-and-turn cutscene stub for slot 0x11. Split out of asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a.s. One of three identical stu |
| `OvlFunc_920_200846c` | `0x0200846c` | — | read | OvlFunc_920_200846c extracted from goldensun/asm/overlays/rom_7a6ae4/ovl_30_c_c_a_a_a_a.s. keeps its name and its slot in the overlay's link |
| `OvlFunc_921_2008030` | `0x02008030` | — | read | struct A { unsigned char pad00[0x18]; int f18; int f1c; unsigned char pad20[0x44]; short f64; short f66; }; extern unsigned int __Random(voi |
| `OvlFunc_921_20080d8` | `0x020080d8` | — | read | Split out of that .s; the sibling parts stay as assembly. Turn one step toward the target, one of eleven identical copies -- one per overlay |
| `OvlFunc_921_2008130` | `0x02008130` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_921_200816c` | `0x0200816c` | — | read | translation unit and the linker script is untouched. Selects a script pointer from the AREA ID at gState+0x1C0, patching the selected table  |
| `OvlFunc_921_20081ec` | `0x020081ec` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_921_20082b8` | `0x020082b8` | — | read | extern int __GetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __CutsceneWait(int n); extern  |
| `OvlFunc_921_2008384` | `0x02008384` | — | read | extern int __GetFlag(int); extern void __SetFlag(int); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Cut |
| `OvlFunc_921_20085dc` | `0x020085dc` | — | read | fakematch FAKEMATCH -- matched by pinning a register with inline asm, not by finding the construct. Authorised as an interim measure; every  |
| `OvlFunc_921_20086c0` | `0x020086c0` | — | read | extern unsigned char *iwram_3001ebc; extern void *L3190[] __asm__(".L3190"); extern short L31a8[][2] __asm__(".L31a8"); extern void __Cutsce |
| `OvlFunc_921_20087a4` | `0x020087a4` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern int __GetFlag(int id); extern void __Func_80b0278(int a, int b); extern void __C |
| `OvlFunc_921_200888c` | `0x0200888c` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern int __GetFlag(int id); extern void __Func_80b0278(int a, int b); extern void __C |
| `OvlFunc_921_2008974` | `0x02008974` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern int __GetFlag(int id); extern void __CutsceneStart(void); extern void __Cutscene |
| `OvlFunc_921_2008a3c` | `0x02008a3c` | — | read | overlay in goldensun/overlays/rom_7a7298/overlay.ld. A talk gated on the player's facing, then on a save flag. `v = 0xc0 << 6;` at the top i |
| `OvlFunc_921_2008abc` | `0x02008abc` | — | read | OvlFunc_921_2008abc  --  0x02008abc Cut out of goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_a_a_c.s. The sanctum attendant. Which li |
| `OvlFunc_921_200954c` | `0x0200954c` | — | read | extern int L31f0[] __asm__(".L31f0"); extern void __vec3_translate(int a, int b, int *v); extern void __DeleteActor(void *a); struct A { uns |
| `OvlFunc_921_2009704` | `0x02009704` | — | read | #include "gba/types.h" #include "actor.h" extern void __Func_80929d8(struct Actor *a, int n); extern void __Actor_SetSpriteFlags(struct Acto |
| `OvlFunc_921_200974c` | `0x0200974c` | — | read | Placed in the run in goldensun/overlays/rom_7a7298/overlay.ld. One frame of a rising particle: horizontal position takes the angle word at + |
| `OvlFunc_921_2009794` | `0x02009794` | — | read | Slotted before asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_a_c_c_c.o in Spawns up to five props, each on its own phase of a 0x3c-frame cycle. R |
| `OvlFunc_921_20098c4` | `0x020098c4` | — | read | extern int iwram_3001e70; extern unsigned char *__MapActor_GetActor(int slot); extern int __GetFlag(int id); extern void __SetFlag(int id);  |
| `OvlFunc_921_2009f24` | `0x02009f24` | — | read | OvlFunc_921_2009f24  --  0x02009f24, cut from goldensun/asm/overlays/rom_7a7298/ovl_30_c_c_c_c_c_c_c.s. The player-facing turn step. While t |
| `OvlFunc_922_2008050` | `0x02008050` | — | read | tools/asmfacts.py, not inferred from the function count. GetEntrances, 8-way form: selects one of 8 per-area tables from the gState halfword |
| `OvlFunc_922_20080f8` | `0x020080f8` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. GetEntrances, 7-way form: sele |
| `OvlFunc_922_2008180` | `0x02008180` | — | read | extern unsigned char gState[]; extern unsigned char *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern void __Cutscen |
| `OvlFunc_922_20085b8` | `0x020085b8` | — | read | keeps its name and its slot in goldensun/overlays/rom_7a8c8c/overlay.ld is unchanged. A lever being thrown: play a sound, move something to  |
| `OvlFunc_922_20087f0` | `0x020087f0` | — | read | extern void __PlaySound(int id); extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __ClearFlag(int id); extern void O |
| `OvlFunc_922_2008920` | `0x02008920` | — | read | extern void __PlaySound(int id); extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __ClearFlag(int id); extern void O |
| `OvlFunc_922_2008f30` | `0x02008f30` | — | read | overlay in goldensun/overlays/rom_7a8c8c/overlay.ld. GetEntrances, 8-way form -- the widest yet elevated. Selects one of eight per-area tabl |
| `OvlFunc_922_2009004` | `0x02009004` | — | read | Placed in the run in goldensun/overlays/rom_7a8c8c/overlay.ld. Byte-identical to OvlFunc_934_2009938 in overlays/rom_7bdeb0; this C is share |
| `OvlFunc_922_2009050` | `0x02009050` | — | read | Three save-bit tests, each choosing between two six-argument calls.  Matched on the first screen by reading the `str` operands to decide whi |
| `OvlFunc_922_20095dc` | `0x020095dc` | — | read | extern unsigned char gState[]; extern int __GetFlag(int id); extern void OvlFunc_922_2009004(int a, int b, int c); extern void __Func_801070 |
| `OvlFunc_922_2009750` | `0x02009750` | — | read | UNPARKED. This was filed under constant-CSE and was a documented counter-example: the offset 0x1c0 is built twice in the ROM with a __GetFla |
| `OvlFunc_922_2009a34` | `0x02009a34` | — | read | Appended after the _a piece in goldensun/overlays/rom_7a8c8c/overlay.ld. Two area-gated cutscene nudges. Reads a signed short out of the blo |
| `OvlFunc_922_2009d78` | `0x02009d78` | — | read | typedef struct { int a; int b; int c; int d; int e; int f; } V; extern volatile int iwram_3001e40; extern int __Random(void); extern void __ |
| `OvlFunc_922_2009e08` | `0x02009e08` | — | read | extern void __PlaySound(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Func_808e118(void); exter |
| `OvlFunc_922_2009fac` | `0x02009fac` | — | read | OvlFunc_922_2009fac  --  0x02009fac The whole of goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_a.s, which held this function and no |
| `OvlFunc_922_200a014` | `0x0200a014` | — | read | OvlFunc_922_200a014  --  0x0200a014, cut from goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c.s. The player-facing turn step. While t |
| `OvlFunc_923_2008cc0` | `0x02008cc0` | — | read | OvlFunc_923_2008cc0  --  0x02008cc0, cut from goldensun/asm/overlays/rom_7aa430/ovl_314_a_c_c_c_c.s. Spawns one particle burst: fills a 0x28 |
| `OvlFunc_923_2008d58` | `0x02008d58` | — | read | OvlFunc_923_2008d58  --  0x02008d58, cut from as ovl_314_c_c.o and this file takes the first .text line. Turn toward the player, unless the  |
| `OvlFunc_923_2008d98` | `0x02008d98` | — | read | extern unsigned int iwram_3001e40; extern int L291c __asm__(".L291c"); extern int L2924 __asm__(".L2924"); extern int gOvl_0200a920; void Ov |
| `OvlFunc_923_2008ed0` | `0x02008ed0` | — | read | Slotted between ovl_e90_c_c_a_a_c_a.o and the rest of the overlay. A map-exit cutscene: install an update hook on slot 0, set its speed, wal |
| `OvlFunc_923_2009208` | `0x02009208` | — | read | OvlFunc_923_2009208 -- MATCHES on the default flags (and unchanged under -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_7aa430/ovl_1150_ |
| `OvlFunc_923_200996c` | `0x0200996c` | — | read | extern char *iwram_3001ebc; extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __Cu |
| `OvlFunc_923_2009c20` | `0x02009c20` | — | read | Placed in the run in goldensun/overlays/rom_7aa430/overlay.ld. One frame of a 32-step animation: bumps the step counter at +0x64, and while  |
| `OvlFunc_923_200a370` | `0x0200a370` | — | read | extern unsigned int gState; extern unsigned char *iwram_3001edc; extern void OvlFunc_923_2009bc8(int arg); extern int __Random(void); void O |
| `OvlFunc_924_2008cd0` | `0x02008cd0` | — | read | OvlFunc_924_2008cd0  --  0x02008cd0, cut from goldensun/asm/overlays/rom_7ac2d8/ovl_314_c.s. slot as ovl_314_c_c.o and this file takes the . |
| `OvlFunc_924_2008d58` | `0x02008d58` | — | read | extern unsigned int iwram_3001e40; extern int gScript_969__0200e004; extern int L6008 __asm__(".L6008"); extern int L600c __asm__(".L600c"); |
| `OvlFunc_924_2008e20` | `0x02008e20` | — | read | GetEntrances, four-way form: selects one of four edge-transition tables from a gState halfword, falling through to the last. One of a 24-mem |
| `OvlFunc_924_2008e80` | `0x02008e80` | — | read | GetEntrances, 5-way form: selects one of 5 per-area tables from the gState halfword at +0x1C0, falling through to the last. THE EARLIER FAMI |
| `OvlFunc_924_2008f30` | `0x02008f30` | — | read | THE LAST MEMBER OF THE 24-FUNCTION FAMILY, and the only one that needed its .s split by hand. That .s held one function and FOURTEEN .incbin |
| `OvlFunc_924_2008f84` | `0x02008f84` | — | read | Split out of that .s; the sibling part stays as assembly. A map-exit cutscene: install an update hook on slot 0, set its speed, walk it, cle |
| `OvlFunc_924_2009420` | `0x02009420` | — | read | struct Actor { unsigned char pad00[8]; int f8; int fc; int f10; unsigned char pad14[0x3c - 0x14]; int f3c; }; extern unsigned char L6010[] _ |
| `OvlFunc_924_2009568` | `0x02009568` | — | read | OvlFunc_924_2009568 Cut out of goldensun/asm//overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_b.s. Raises the platform once its flag is set. Needs CSE |
| `OvlFunc_924_2009bf0` | `0x02009bf0` | — | read | struct Actor { unsigned char pad00[8]; int f8; int fc; int f10; unsigned char pad14[0x3c - 0x14]; int f3c; }; extern unsigned char L6064[] _ |
| `OvlFunc_924_2009d3c` | `0x02009d3c` | — | read | OvlFunc_924_2009d3c Cut out of goldensun/asm//overlays/rom_7ac2d8/ovl_f84_c_c_b.s. The sibling of OvlFunc_924_2009568 with different tiles a |
| `OvlFunc_924_200a51c` | `0x0200a51c` | — | read | extern void __CutsceneStart(void); extern int *__MapActor_GetActor(int a); extern void __SetFlag(int f); extern void __ClearFlag(int f); ext |
| `OvlFunc_924_200b788` | `0x0200b788` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern int  __GetFlag(int id); extern void __CutsceneStart(void); extern void __Cutscen |
| `OvlFunc_924_200cf90` | `0x0200cf90` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_7ac2d8/overlay.ld, so the ROM layo |
| `OvlFunc_924_200d1b0` | `0x0200d1b0` | — | read | Slotted between the _a and _c pieces in goldensun/overlays/rom_7ac2d8/overlay.ld. BYTE-IDENTICAL TWIN of OvlFunc_923_2009c20 in overlays/rom |
| `OvlFunc_924_200d388` | `0x0200d388` | — | read | extern unsigned char *iwram_3001ebc; extern unsigned char gState[]; extern unsigned char gScript_924__0200de14[]; extern unsigned char *__Cr |
| `OvlFunc_924_200d900` | `0x0200d900` | — | read | extern unsigned int gState; extern unsigned char *iwram_3001edc; extern void OvlFunc_924_200d158(int arg); extern int __Random(void); void O |
| `OvlFunc_925_20088cc` | `0x020088cc` | — | read | extern unsigned char *iwram_3001ebc; extern unsigned char *__MapActor_GetActor(int slot); extern int OvlFunc_925_2008890(void *a, void *b);  |
| `OvlFunc_925_2008928` | `0x02008928` | — | read | extern void *__MapActor_GetActor(int slot); extern void __vec3_translate(unsigned int a, unsigned int b, int *c); extern int __TestCollision |
| `OvlFunc_925_20089fc` | `0x020089fc` | — | read | extern unsigned char *__MapActor_GetActor(int); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __SetFlag(in |
| `OvlFunc_925_200addc` | `0x0200addc` | — | read | extern char *iwram_3001e70; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Func_800fe9c(void); extern voi |
| `OvlFunc_926_200834c` | `0x0200834c` | — | read | GetEntrances, two-way form. This one returns a NAMED GLOBAL from its first arm rather than a local `.L` table, which is why the earlier fami |
| `OvlFunc_926_2008484` | `0x02008484` | — | read | extern unsigned char iwram_3001ebc[]; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __MessageID(int id); e |
| `OvlFunc_926_2008f80` | `0x02008f80` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __Func_80921c4(int a, int b, int c); extern void __Func_8092adc(int a, int  |
| `OvlFunc_926_2009334` | `0x02009334` | — | read | OvlFunc_926_2009334  --  0x02009334, cut from the head of functions follow as ovl_314_c_c_a_c_c_c_a_a_c_c.o. Dispatch on the player's facing |
| `OvlFunc_926_20093b8` | `0x020093b8` | — | read | struct Actor { unsigned char pad00[6]; unsigned short f6; unsigned char pad08[0x12 - 8]; short f12; }; extern struct Actor *__MapActor_GetAc |
| `OvlFunc_926_200a484` | `0x0200a484` | — | read | THREE SITES IN ONE FUNCTION, which is what makes this the best evidence so far that the basic-block lever is a rule and not a coincidence. ` |
| `OvlFunc_926_200a508` | `0x0200a508` | — | read | A sanctum attendant. Fifth of this shape and the third distinct variant of the facing test, so the family is worth stating in one place: UNS |
| `OvlFunc_926_200a574` | `0x0200a574` | — | read | A three-way selector on the area id at gState+0x1C0 and a second halfword at +0x1C2. ELEVATED BY NAMING A CONSTANT. The ROM pools 0x3c where |
| `OvlFunc_926_200a5b8` | `0x0200a5b8` | — | read | extern unsigned int iwram_3001e40; extern unsigned char *__MapActor_GetActor(int slot); extern unsigned int __Random(void); extern void OvlF |
| `OvlFunc_926_200a6d8` | `0x0200a6d8` | — | read | OvlFunc_926_200a6d8  --  0x0200a6d8 Cut out of goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_a_b.s. The escape cutscene: start a backg |
| `OvlFunc_926_200c140` | `0x0200c140` | — | read | OvlFunc_926_200c140  --  0x0200c140 Cut out of goldensun/asm/overlays/rom_7b2078/ovl_314_c_c_c_c_c_c_c_a.s. Eight bursts of the same particl |
| `OvlFunc_926_200c1c4` | `0x0200c1c4` | — | read | Slotted between ovl_314_c_c_c_c_c_c_c_a.o and the rest of the overlay. ONE `.global` WAS ADDED to the .s to split this out. tools/split_s.py |
| `OvlFunc_926_200c1ec` | `0x0200c1ec` | — | read | Code to this file, the trailing .section .data to its _c sibling. Two levers took this from 36 differing to exact. SEPARATE LOCALS FOR SHORT |
| `OvlFunc_927_20089dc` | `0x020089dc` | — | read | Appended after the _a piece in goldensun/overlays/rom_7b4558/overlay.ld. Writes a two-bit selector into bits 2-3 of the sprite flag byte at  |
| `OvlFunc_927_20089f4` | `0x020089f4` | — | read | of the same .s in goldensun/overlays/rom_7b4558/overlay.ld. ONE OF FOUR OPERAND-IDENTICAL COPIES, elevated together from a single source wit |
| `OvlFunc_927_2008a4c` | `0x02008a4c` | — | read | OvlFunc_927_2008a4c, the whole of goldensun/asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_c_a.s. no linker-script change was needed. Spawns an  |
| `OvlFunc_927_2008ab0` | `0x02008ab0` | — | read | Slotted between the _a and _c pieces in goldensun/overlays/rom_7b4558/overlay.ld. Advances an entity by its per-axis deltas and spins its at |
| `OvlFunc_927_2008cd0` | `0x02008cd0` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern int __TestCollision(unsigned char *a, unsigned char *b); extern void __CutsceneS |
| `OvlFunc_927_2008e18` | `0x02008e18` | — | read | struct P { int f0; unsigned char pad4[0x28 - 4]; }; struct V { int x; int y; int z; }; struct A { unsigned char pad0[8]; int f8; unsigned ch |
| `OvlFunc_927_2008ee0` | `0x02008ee0` | — | read | GetEntrances, four-way form: selects one of four edge-transition tables from a gState halfword, falling through to the last. One of a 24-mem |
| `OvlFunc_927_2008f40` | `0x02008f40` | — | read | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script, so the ROM layout does not move. |
| `OvlFunc_927_2008f94` | `0x02008f94` | — | read | struct S { int f00; int f04; int f08; int f0c; int f10; int f14; }; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); exte |
| `OvlFunc_927_2009150` | `0x02009150` | — | read | The body is the OvlFunc_927 cutscene template of batch 146 -- see OvlFunc_927_2009de0 for the same call sequence at a different actor slot.  |
| `OvlFunc_927_2009244` | `0x02009244` | — | read | The OvlFunc_927 cutscene template at actor slot 0xb, with two things the other members of the family do not have: TWO __MapActor_GetActor(0) |
| `OvlFunc_927_2009328` | `0x02009328` | — | read | The OvlFunc_927 cutscene template again, at actor slot 0xc.  Two values are held in callee-saved registers and both are written where the RO |
| `OvlFunc_927_2009454` | `0x02009454` | — | read | The same frame as OvlFunc_927_20099b8: a 24-byte struct filled by OvlFunc_927_2008474 and passed BY VALUE to OvlFunc_927_2008608, which is w |
| `OvlFunc_927_2009520` | `0x02009520` | — | read | struct Actor { unsigned char pad00[8]; int x; int fc; int z; int f14; unsigned char pad18[0x3d]; unsigned char f55; }; extern struct Actor * |
| `OvlFunc_927_20095d0` | `0x020095d0` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Cutsc |
| `OvlFunc_927_20096f0` | `0x020096f0` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Cutsc |
| `OvlFunc_927_2009880` | `0x02009880` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Cutsc |
| `OvlFunc_927_20099b8` | `0x020099b8` | — | read | 112 instructions, and it took four separate levers.  Recorded in the order they were needed, because each one only became visible after the  |
| `OvlFunc_927_2009b84` | `0x02009b84` | — | read | OvlFunc_927_2009b84 -- MATCHES on the default flags (and unchanged under -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_7b4558/ovl_30_c_ |
| `OvlFunc_927_2009de0` | `0x02009de0` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Cutsc |
| `OvlFunc_927_2009ef0` | `0x02009ef0` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Cutsc |
| `OvlFunc_927_200a078` | `0x0200a078` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Cutsc |
| `OvlFunc_927_200a4ac` | `0x0200a4ac` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_927_200ac0c` | `0x0200ac0c` | — | read | OvlFunc_927_200ac0c  --  0x0200ac0c, cut from Dresses an actor as a portrait: clear two sprite-attribute fields, set the animation mode, hid |
| `OvlFunc_928_2008370` | `0x02008370` | — | read | #include "gba/types.h" #include "actor.h" struct Cmd { int f0; int f4; unsigned char pad08[0x18 - 0x08]; short f18; unsigned char pad1a[0x1c |
| `OvlFunc_928_2008968` | `0x02008968` | — | read | OvlFunc_928_2008968  --  0x02008968, cut from Reveal a bridge: clear two of slot 0x14's flag bytes, repaint the map attributes under it from |
| `OvlFunc_928_2008e4c` | `0x02008e4c` | — | read | OvlFunc_928_2008e4c  --  0x02008e4c Cut out of goldensun/asm/overlays/rom_7b6668/ovl_314_c_c_a_c_c_c_c.s. Plays the door-opening cue for whi |
| `OvlFunc_929_2008598` | `0x02008598` | — | read | Code to this file, the trailing .section .data to its _c sibling. THE DECLARATION LEVER HAS A THIRD FORM: `void` versus `int` RETURN on the  |
| `OvlFunc_930_200807c` | `0x0200807c` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_930_20080b8` | `0x020080b8` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_930_20088e0` | `0x020088e0` | — | read | A sanctum attendant, sixth of the shape. Identical to src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_a_b.c apart from the actor id and the me |
| `OvlFunc_930_2008ff0` | `0x02008ff0` | — | read | Slotted between ovl_30_c_c_a_c_c_c_c_c_a.o and the rest of the overlay. Near-twin of OvlFunc_930_2009028 next door (differing only in the fi |
| `OvlFunc_930_2009028` | `0x02009028` | — | read | Slotted between ovl_30_c_c_a_c_c_c_c_c_b.o and the rest of the overlay. Near-twin of OvlFunc_930_2008ff0 in the .o immediately above, differ |
| `OvlFunc_930_20090b8` | `0x020090b8` | — | read | extern void OvlFunc_930_2009060(void); struct Actor { unsigned char pad00[0x6c]; void *f6c; }; extern unsigned char *__MapActor_GetActor(int |
| `OvlFunc_930_2009180` | `0x02009180` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_931_2008030` | `0x02008030` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. GetEntrances, three-way form: selects one of three edge-transiti |
| `OvlFunc_931_200811c` | `0x0200811c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_931_20081d0` | `0x020081d0` | — | read | struct Actor { unsigned char pad0[8]; int f8; int fc; int f10; }; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern |
| `OvlFunc_931_2008360` | `0x02008360` | — | read | Split out of that .s; the sibling parts stay as assembly. A three-way talk: before flag 0x242 is set, one line; after it, either a hand-off  |
| `OvlFunc_931_20083d4` | `0x020083d4` | — | read | the overlay in goldensun/overlays/rom_7b8cb0/overlay.ld. The three-way talk two .o slots after src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_ |
| `OvlFunc_931_2008448` | `0x02008448` | — | read | The third member of the three-way-talk family in this overlay, after ovl_30_c_c_c_c_c_c_c_c_c_a_a_b.c and ..._a_a_c_b.c. Same structure: a f |
| `OvlFunc_931_2008874` | `0x02008874` | — | read | OvlFunc_931_2008874  --  0x02008874 Three independent flag fixups on entry, then a two-way area check. THE AREA IDS ARE NAMED CONSTANTS. 0x4 |
| `OvlFunc_931_2008c0c` | `0x02008c0c` | — | read | piece and the overlay script's .data line is repointed there. UNPARKED. This sat in src/non_matching/ from batch 53 at 1 of 24, and is the S |
| `OvlFunc_931_2008c44` | `0x02008c44` | — | read | A particle step: advance position by a random jitter, drift differently above and below a size threshold, occasionally retrigger, and delete |
| `OvlFunc_932_2008040` | `0x02008040` | — | read | Turn one step toward the target, one of eleven identical copies -- one per overlay, byte-for-byte the same body. See src/overlays/rom_784360 |
| `OvlFunc_932_20080e4` | `0x020080e4` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. GetEntrances, 12-way form: sel |
| `OvlFunc_932_20081c8` | `0x020081c8` | — | read | translation unit and the linker script is untouched. Selects a script/table pointer from the AREA ID at gState+0x1C0. The compared constants |
| `OvlFunc_932_200820c` | `0x0200820c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. GetEntrances, 10-way form: sel |
| `OvlFunc_932_2008388` | `0x02008388` | — | read | THE FIRST FUNCTION ELEVATED OUT OF THE pool-tell BLOCKER. The ROM loads 0x4d from the literal pool rather than building it with `mov r0, #0x |
| `OvlFunc_932_20083b4` | `0x020083b4` | — | read | Second of three identical stubs differing only in the pooled id -- see ovl_30_a_c_c_a_a_a_a_b.c for why _AREA_4f has to be a symbol rather t |
| `OvlFunc_932_20083e0` | `0x020083e0` | — | read | Third of three identical stubs differing only in the pooled id -- see ovl_30_a_c_c_a_a_a_a_b.c for why _AREA_51 has to be a symbol rather th |
| `OvlFunc_932_200840c` | `0x0200840c` | — | read | OvlFunc_932_200840c  --  0x0200840c The whole of goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_c_c_c.s, which held this function an |
| `OvlFunc_932_20084cc` | `0x020084cc` | — | read | UNPARKED BY THE STACK-ARG-PAIR LEVER. This was parked at 27 against 27 with twenty-three identical, on the ROM materialising both stack valu |
| `OvlFunc_932_20089ec` | `0x020089ec` | — | read | OvlFunc_932_20089ec Cut out of goldensun/asm//overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_b.s. Toggles the bridge tiles and the flag  |
| `OvlFunc_932_2008a94` | `0x02008a94` | — | read | OvlFunc_932_2008a94 Cut out of goldensun/asm//overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_c_a.s. The sibling of 20089ec for the second bridge. BUIL |
| `OvlFunc_932_2008b3c` | `0x02008b3c` | — | read | extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Func_801776c(int a, int b); extern void __PlaySound(int id |
| `OvlFunc_932_2008bd8` | `0x02008bd8` | — | read | extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Func_801776c(int a, int b); extern void __PlaySound(int id |
| `OvlFunc_932_2009678` | `0x02009678` | — | read | typedef struct { unsigned char _bytes[704]; } GlobalState; extern GlobalState gState; extern int __GetFlag(int id); extern int _AREA_4d; ext |
| `OvlFunc_932_200a020` | `0x0200a020` | — | read | A nine-way dispatch on the area id.  Matched on the first screen; two readings decided it and both are already on record. ALL NINE COMPARISO |
| `OvlFunc_932_200a310` | `0x0200a310` | — | read | Slotted after asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a.o in THE OFFSET IS NAMED, NOT THE BASE -- and that distinction is the w |
| `OvlFunc_932_200a428` | `0x0200a428` | — | read | OvlFunc_932_200a428  --  0x0200a428 Cut out of goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a.s, which holds eighteen functions; |
| `OvlFunc_932_200a490` | `0x0200a490` | — | read | with no data, so no split was needed and overlay.ld is untouched. EXACT ON THE FIRST SCREEN, because it is a structural sibling of OvlFunc_9 |
| `OvlFunc_932_200a5c0` | `0x0200a5c0` | — | read | Slotted between ..._a.o and ..._c.o in goldensun/overlays/rom_7b9cb4/overlay.ld. RECOVERED FROM A PARK, and the park's diagnosis was wrong i |
| `OvlFunc_932_200a9dc` | `0x0200a9dc` | — | read | The target was the LAST of six functions, so there is no _c part. WAS PARKED, and the park concluded "NEXT: nothing.  This is the documented |
| `OvlFunc_932_200aa10` | `0x0200aa10` | — | read | Matched on the first screen.  The read-modify-write on byte +0x09 of the actor is ONE variable throughout: `z` is 0 for the strb at +0x55, t |
| `OvlFunc_932_200b428` | `0x0200b428` | — | read | Overlay 932: branch on the player's height. Whole-file conversion of asm/overlays/rom_7b9cb4/ovl_30_a_c_c_c_a_c.s -- it holds only this func |
| `OvlFunc_932_200b9c8` | `0x0200b9c8` | — | read | #include "dma.h" extern unsigned char *iwram_3001ed0; extern short L525c[] __asm__(".L525c"); extern unsigned short L5260[] __asm__(".L5260" |
| `OvlFunc_932_200ba44` | `0x0200ba44` | — | read | OvlFunc_932_200ba44  --  0x0200ba44, cut from Clears two halfwords in the overlay's own storage and starts a task. `ldr r2, =0` IS NOT THE P |
| `OvlFunc_933_2008344` | `0x02008344` | — | read | struct P { unsigned char pad00[8]; int f8; int fc; unsigned char pad10[0x22 - 0x10]; unsigned short f22; unsigned char pad24[4]; }; struct A |
| `OvlFunc_933_20083ac` | `0x020083ac` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. GetEntrances, 5-way form: sele |
| `OvlFunc_933_200841c` | `0x0200841c` | — | read | translation unit and the linker script is untouched. Selects a script/table pointer from the AREA ID at gState+0x1C0. The compared constants |
| `OvlFunc_933_2008498` | `0x02008498` | — | read | Slotted between ovl_314_c_c_a_a.o and the rest of the overlay. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile and the  |
| `OvlFunc_933_2008c38` | `0x02008c38` | — | read | `__Func_8091f90` TAKES AN AREA ID, which is why 0x5b is written as `(int)(&_AREA_5b)`. The ROM pools it although it would fit in a `mov`, an |
| `OvlFunc_933_2008c6c` | `0x02008c6c` | — | read | Picks a random one of three slots, retrying up to three times while its flag is already set, then claims it and speaks the matching line. TH |
| `OvlFunc_934_2008d20` | `0x02008d20` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_934_2008d80` | `0x02008d80` | — | read | translation unit and the linker script is untouched. Selects a script/table pointer from the AREA ID at gState+0x1C0. The compared constants |
| `OvlFunc_934_2008dcc` | `0x02008dcc` | — | read | Overlay 934: apply a map edit at row 0xF. Split out of asm/overlays/rom_7bdeb0/ovl_d20_c_c_a.s. One of a pair differing only in the row -- 0 |
| `OvlFunc_934_2008de8` | `0x02008de8` | — | read | Overlay 934: apply a map edit at row 0x11. Split out of asm/overlays/rom_7bdeb0/ovl_d20_c_c_a.s. One of a pair differing only in the row --  |
| `OvlFunc_934_20090e0` | `0x020090e0` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern int __TestCollision(unsigned char *a, int *b); extern void __CutsceneStart(void) |
| `OvlFunc_934_20091a0` | `0x020091a0` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_934_2009258` | `0x02009258` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_934_2009378` | `0x02009378` | — | read | Overlay 934: put slot 8 back to its idle animation, inside a cutscene. Split out of asm/overlays/rom_7bdeb0/ovl_1300_c.s; the neighbouring p |
| `OvlFunc_934_20095cc` | `0x020095cc` | — | read | in goldensun/overlays/rom_7bdeb0/overlay.ld.  The target was the LAST function and the .s carries a trailing .data, so the split is genuinel |
| `OvlFunc_934_200969c` | `0x0200969c` | — | read | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script, so the ROM layout does not move. |
| `OvlFunc_934_20096f0` | `0x020096f0` | — | read | keeps its name and its slot in goldensun/overlays/rom_7bdeb0/overlay.ld is unchanged. Writes a mode word into the block at iwram_3001ebc, an |
| `OvlFunc_934_2009770` | `0x02009770` | — | read | Draws a map rectangle, then redraws a one-tile strip differently depending on save bit 0x301 -- the two arms differ only in which column the |
| `OvlFunc_934_20097d8` | `0x020097d8` | — | read | extern unsigned char gState[]; extern unsigned char *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern void __Cutscen |
| `OvlFunc_934_2009938` | `0x02009938` | — | read | Placed in the run in goldensun/overlays/rom_7bdeb0/overlay.ld. Byte-identical to OvlFunc_922_2009004 in overlays/rom_7a8c8c; this C is share |
| `OvlFunc_935_2008030` | `0x02008030` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_935_200808c` | `0x0200808c` | — | read | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script, so the ROM layout does not move. |
| `OvlFunc_935_20080e0` | `0x020080e0` | — | read | GetEntrances, 4-way form. Returns a named global from at least one arm, which is why the family sweeps in batches 08-13 missed it -- they ma |
| `OvlFunc_935_20082e0` | `0x020082e0` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_935_2008368` | `0x02008368` | — | read | OvlFunc_935_2008368 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_a_c_a.s. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS i |
| `OvlFunc_935_2008398` | `0x02008398` | — | read | The twin of src/overlays/rom_7bf5a8/ovl_2e0_c_a_a.c, same overlay, twenty-odd .o slots apart, differing only in its nine constants. Read tha |
| `OvlFunc_935_20083e0` | `0x020083e0` | — | read | OvlFunc_935_20083e0 extracted from goldensun/asm/overlays/rom_7bf5a8/ovl_2e0_a_c_c.s. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS i |
| `OvlFunc_935_2008410` | `0x02008410` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Three map-rect edits in a row. Two six-argument calls to __Func_ |
| `OvlFunc_935_200848c` | `0x0200848c` | — | read | A one-shot reward: if the flag is clear and a condition holds and a second flag is clear, play a chime, give the thing, and set the flag. Bu |
| `OvlFunc_935_20084d0` | `0x020084d0` | — | read | Near-twin of src/overlays/rom_7bf5a8/ovl_2e0_c_a_c.c in the same overlay, differing only in that this one does not call OvlFunc_935_2008170  |
| `OvlFunc_935_2008944` | `0x02008944` | — | read | struct Actor { unsigned char pad00[8]; int x; int fc; int z; unsigned char pad14[0x14]; int f28; unsigned char pad2c[0x1c]; int f48; }; exte |
| `OvlFunc_935_2008aa0` | `0x02008aa0` | — | read | A per-frame flicker: bump a counter, and every 64th tick pick a random one of six actors; then sweep all six, resetting whichever disagree w |
| `OvlFunc_935_2008b8c` | `0x02008b8c` | — | read | OvlFunc_935_2008b8c  --  asm/overlays/rom_7bf5a8/ovl_b8c_a.s Spawns up to four actors at the caller's position, each with jittered velocity, |
| `OvlFunc_936_2008040` | `0x02008040` | — | read | OvlFunc_936_2008040  --  0x02008040 The whole of goldensun/asm/overlays/rom_7c097c/ovl_30_a_c_a_a.s, which held this function and no data. A |
| `OvlFunc_936_20080ac` | `0x020080ac` | — | read | Placed in the run in goldensun/overlays/rom_7c097c/overlay.ld. Byte-identical to OvlFunc_969_2008424 in overlays/rom_7f6e64; this C is share |
| `OvlFunc_936_20080ec` | `0x020080ec` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. GetEntrances, 7-way form: sele |
| `OvlFunc_936_2008180` | `0x02008180` | — | read | GetEntrances, 5-way form: selects one of 5 per-area tables from the gState halfword at +0x1C0, falling through to the last. THE EARLIER FAMI |
| `OvlFunc_936_2008240` | `0x02008240` | — | read | tools/asmfacts.py, not inferred from the function count. GetEntrances, 7-way form: selects one of 7 per-area tables from the gState halfword |
| `OvlFunc_936_20082e8` | `0x020082e8` | — | read | OvlFunc_936_20082e8  --  0x020082e8 The whole of goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_a_c_a.s, which held this function and no data, |
| `OvlFunc_936_20083d8` | `0x020083d8` | — | read | keeps its name and its slot in goldensun/overlays/rom_7c097c/overlay.ld is unchanged. Another facing-gated interaction: stand in the right a |
| `OvlFunc_936_2008464` | `0x02008464` | — | read | OvlFunc_936_2008464  --  0x02008464 Cut out of goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c_a_a.s. ONE OF THE MAP-EXIT FAMILY. A sea |
| `OvlFunc_936_200958c` | `0x0200958c` | — | read | Slotted between ovl_30_c_c_c_a_a_c_a_a.o and the rest of the overlay. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile.  |
| `OvlFunc_936_20095e0` | `0x020095e0` | — | read | Overlay 936: set the display-offset flag on the player actor. Split out of asm/overlays/rom_7c097c/ovl_30_c_c_c_a_a_c.s; the neighbouring pa |
| `OvlFunc_936_200964c` | `0x0200964c` | — | read | Slotted between ovl_30_c_c_c_a_c_a_c_a.o and the rest of the overlay. A FIVE-ARM area dispatcher -- the same family as the two-arm members i |
| `OvlFunc_936_2009858` | `0x02009858` | — | read | in goldensun/overlays/rom_7c097c/overlay.ld. Three independent guards run in sequence: a one-shot init, a facing reset, and an area check. N |
| `OvlFunc_936_20098a4` | `0x020098a4` | — | read | OvlFunc_936_20098a4  --  0x020098a4 Cut out of goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_a_c.s. Arrival fixup for one map: if the |
| `OvlFunc_936_2009ea4` | `0x02009ea4` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in Points an actor at a turn target, gives it a randomised tick offse |
| `OvlFunc_936_2009f14` | `0x02009f14` | — | read | OvlFunc_936_2009f14  --  0x02009f14 Cut out of goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c_c.s. The step counter it drives, `.L51 |
| `OvlFunc_936_200b6f8` | `0x0200b6f8` | — | read | extern void __DeleteActor(unsigned char *a); void OvlFunc_936_200b6f8(unsigned char *a) { unsigned char *q; int s; int vx; int vy; int z; q  |
| `OvlFunc_936_200ba3c` | `0x0200ba3c` | — | read | OvlFunc_936_200ba3c  --  0x0200ba3c, cut from Dresses an actor as a portrait: clear two sprite-attribute fields, set the animation mode, hid |
| `OvlFunc_937_2008030` | `0x02008030` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. GetEntrances, three-way form: selects one of three edge-transiti |
| `OvlFunc_937_200807c` | `0x0200807c` | — | read | extern unsigned char gState[]; extern int _AREA_64; extern int _AREA_65; extern unsigned char L8d4[] __asm__(".L8d4"); extern unsigned char  |
| `OvlFunc_937_20080e4` | `0x020080e4` | — | read | extern unsigned char gState[]; extern int _AREA_64; extern int _AREA_65; extern unsigned char L8d4[] __asm__(".L8d4"); extern unsigned char  |
| `OvlFunc_937_2008308` | `0x02008308` | — | read | Slotted between ovl_30_c_c_c_c_c_c_a.o and the rest of the overlay. Writes 0x209 into the iwram_3001ebc block at +0x1c0 and dispatches on th |
| `OvlFunc_938_2008030` | `0x02008030` | — | read | GetEntrances, two-way form. This one returns a NAMED GLOBAL from its first arm rather than a local `.L` table, which is why the earlier fami |
| `OvlFunc_938_200806c` | `0x0200806c` | — | read | OvlFunc_938_200806c extracted from goldensun/asm/overlays/rom_7c37ac/ovl_30_c_c_a_a.s. A SCRIPT SELECTOR: pick a script pointer by area id a |
| `OvlFunc_938_20080a4` | `0x020080a4` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_938_2008184` | `0x02008184` | — | read | extern unsigned char *iwram_3001ebc; extern void __PlaySound(int id); extern void __Func_80118a8(int n); extern void __Func_80118c0(int n);  |
| `OvlFunc_938_2008230` | `0x02008230` | — | read | Slotted between ovl_30_c_c_c_c_c_a.o and the rest of the overlay. Writes 0x209 into the iwram_3001ebc block at +0x1c0 and dispatches on the  |
| `OvlFunc_938_2008264` | `0x02008264` | — | read | The target was the FIRST of five functions, so there is no _a part; the remaining four functions and the trailing .data travel together in _ |
| `OvlFunc_939_2008314` | `0x02008314` | — | read | translation unit and the linker script is untouched. Selects a script/table pointer from the AREA ID at gState+0x1C0. ELEVATED BY USING SYMB |
| `OvlFunc_939_2008350` | `0x02008350` | — | read | translation unit and the linker script is untouched. Selects a script/table pointer from the AREA ID at gState+0x1C0. ELEVATED BY USING SYMB |
| `OvlFunc_939_2008388` | `0x02008388` | — | read | translation unit and the linker script is untouched. Selects a script/table pointer from the AREA ID at gState+0x1C0. ELEVATED BY USING SYMB |
| `OvlFunc_939_20083f4` | `0x020083f4` | — | read | OvlFunc_939_20083f4  --  0x020083f4 The whole of goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_a_a_c_a_a.s, which held this function and no  |
| `OvlFunc_939_2008468` | `0x02008468` | — | read | Slotted between ovl_314_a_c_a_a_c_a_a.o and the rest of the overlay. TWO ARMS JOINING AT A SHARED STORE, so the tail is a join rather than a |
| `OvlFunc_939_20085f0` | `0x020085f0` | — | read | ALL TEN STACK ARGUMENTS ARE NAMED, and it is all-or-nothing. The first transcription was 20 of 81 and the whole diff was one shape: the ROM  |
| `OvlFunc_939_20086e4` | `0x020086e4` | — | read | OvlFunc_939_20086e4 extracted from goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_a_a_c_c.s. A SCRIPT SELECTOR: pick a script pointer by area |
| `OvlFunc_939_2008764` | `0x02008764` | — | read | OvlFunc_939_2008764 Cut out of goldensun/asm//overlays/rom_7c460c/ovl_314_a_c_a_c_a_a.s. A villager whose line depends on a flag, with a cou |
| `OvlFunc_939_20087f4` | `0x020087f4` | — | read | UNPARKED, BUILT AT -O1, AND THE C DID NOT CHANGE. This was parked in batch 32 on SPECULATIVE LITERAL HOIST: for a two-way pick between two p |
| `OvlFunc_939_20088ec` | `0x020088ec` | — | read | A three-way talk on a gState halfword and two save flags. THREE LEVERS, all previously established, and worth listing because this is a good |
| `OvlFunc_939_2008ac4` | `0x02008ac4` | — | read | THIS TU NEEDS -fno-rerun-cse-after-loop (CSE_CFLAGS in the Makefile), which is why the function is split into a TU of its own. The flag ID 0 |
| `OvlFunc_939_2008b6c` | `0x02008b6c` | — | read | OvlFunc_939_2008b6c  --  0x02008b6c Cut out of goldensun/asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_b.s. The entrance table it indexes, `.L |
| `OvlFunc_939_2008c10` | `0x02008c10` | — | read | Slotted between ovl_314_a_c_c_a_c_a.o and the rest of the overlay. `n = -8;` is assigned before the two guards, which is the basic-block lev |
| `OvlFunc_939_2008fa0` | `0x02008fa0` | — | read | Slotted between ovl_314_c_a_a.o and the rest of the overlay. Three calls with the same four register arguments and the same SECOND stack slo |
| `OvlFunc_939_200918c` | `0x0200918c` | — | read | Slotted between ovl_314_c_a_c_a.o and the rest of the overlay. Near-twin of ovl_314_c_a_b.c (OvlFunc_939_2008fa0, batch 57): three calls sha |
| `OvlFunc_939_20091d0` | `0x020091d0` | — | read | OvlFunc_939_20091d0  --  0x020091d0, cut from the head of functions follow as ovl_314_c_a_c_c_c.o. Leaves an area: clear two save bits, and  |
| `OvlFunc_939_2009240` | `0x02009240` | — | read | extern unsigned char iwram_3001ebc[]; extern unsigned char gState[]; struct A { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int  |
| `OvlFunc_939_2009840` | `0x02009840` | — | read | OvlFunc_939_2009840  --  0x02009840, cut from Dresses an actor as a portrait: clear two sprite-attribute fields, set the animation mode, hid |
| `OvlFunc_940_200808c` | `0x0200808c` | — | read | OvlFunc_940_200808c  --  0x0200808c, cut from The inn counter: the attendant opens inn 8 when the player is inside the facing arc AND the sa |
| `OvlFunc_940_200816c` | `0x0200816c` | — | read | keeps its name and its slot in goldensun/overlays/rom_7c5974/overlay.ld is unchanged. Talking to a sanctum attendant: if the player is stand |
| `OvlFunc_940_2008224` | `0x02008224` | — | read | keeps its name and its slot in goldensun/overlays/rom_7c5974/overlay.ld is unchanged. Near-twin of src/overlays/rom_7c5974/ovl_30_c_c_a_c_c_ |
| `OvlFunc_940_20083dc` | `0x020083dc` | — | read | OvlFunc_940_20083dc Cut out of goldensun/asm//overlays/rom_7c5974/ovl_30_c_c_c_c_a_c.s. UNPARKS src/non_matching/overlays/20083dc.c. That pa |
| `OvlFunc_941_2008044` | `0x02008044` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_941_20080d4` | `0x020080d4` | — | read | Eight six-argument calls, so sixteen stack arguments, and the whole function is a clean demonstration of WHICH of them get names. TWO SHARED |
| `OvlFunc_941_200833c` | `0x0200833c` | — | read | Slotted between ovl_30_c_a_c_c_c_a_a.o and the rest of the overlay. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile. A  |
| `OvlFunc_941_2008384` | `0x02008384` | — | read | extern void __Func_80105d4(int a, int b, int c, int d, int e, int f); extern void __Func_8010704(int a, int b, int c, int d, int e, int f);  |
| `OvlFunc_941_2008460` | `0x02008460` | — | read | Slotted between ovl_30_c_a_c_c_c_a_c_a.o and the rest of the overlay. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile.  |
| `OvlFunc_941_20091b8` | `0x020091b8` | — | read | A dialogue state machine: nine tests over seven predicate calls, a loop that re-enters itself two different ways, and three exits.  Written  |
| `OvlFunc_941_2009a0c` | `0x02009a0c` | — | read | typedef struct { unsigned char _bytes[704]; } GlobalState; extern GlobalState gState; extern unsigned int iwram_3001ebc; extern int _AREA_6a |
| `OvlFunc_942_2008040` | `0x02008040` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_942_20080a0` | `0x020080a0` | — | read | OvlFunc_942_2008144 as assembly) in goldensun/overlays/rom_7c6bac/overlay.ld. Selects a script/table pointer from the AREA ID at gState+0x1C |
| `OvlFunc_942_200819c` | `0x0200819c` | — | read | OvlFunc_942_2008144 as assembly) in goldensun/overlays/rom_7c6bac/overlay.ld. Selects a script/table pointer from the AREA ID at gState+0x1C |
| `OvlFunc_942_2008260` | `0x02008260` | — | read | OvlFunc_942_2008260 Cut out of goldensun/asm//overlays/rom_7c6bac/ovl_30_c_c_a_c_a_a.s. A villager with a one-shot line. Needs CSE_CFLAGS fo |
| `OvlFunc_942_20086c8` | `0x020086c8` | — | read | extern unsigned char *iwram_3001ebc; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern int __GetFlag(int id); exter |
| `OvlFunc_942_200886c` | `0x0200886c` | — | read | Slotted between ovl_30_c_c_a_c_c_c_c_a.o and the rest of the overlay. TWO INDEPENDENT READS OUT OF gState, AND THEY NEED SEPARATE VARIABLES. |
| `OvlFunc_942_20088cc` | `0x020088cc` | — | read | OvlFunc_942_20088cc  --  0x020088cc Cut out of goldensun/asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_c.s. Three independent arrival fixups, |
| `OvlFunc_942_2008af8` | `0x02008af8` | — | read | OvlFunc_942_2008af8  --  0x02008af8 The whole of goldensun/asm/overlays/rom_7c6bac/ovl_30_c_c_c_a.s, which held this function and no data. R |
| `OvlFunc_942_2008b68` | `0x02008b68` | — | read | Overlay 942: hide an actor and mark it non-interactive. Split out of asm/overlays/rom_7c6bac/ovl_30_c_c_c.s; the neighbouring parts stay as  |
| `OvlFunc_943_2008030` | `0x02008030` | — | read | OvlFunc_943_2008030  --  0x02008030, cut from goldensun/asm/overlays/rom_7c7b9c/ovl_30_a_a_a_a.s. script's existing line for that object now |
| `OvlFunc_943_2008514` | `0x02008514` | — | read | extern unsigned int __Random(void); struct A { unsigned char pad00[0xc]; int f0c; }; int OvlFunc_943_2008514(struct A *a) { short *d; unsign |
| `OvlFunc_943_2008a48` | `0x02008a48` | — | read | OvlFunc_943_2008a48  --  0x02008a48 OvlFunc_943_2008af0  --  0x02008af0 .o only changes name in goldensun/overlays/rom_7c7b9c/overlay.ld. Th |
| `OvlFunc_943_2008af0` | `0x02008af0` | — | read | OvlFunc_943_2008a48  --  0x02008a48 OvlFunc_943_2008af0  --  0x02008af0 .o only changes name in goldensun/overlays/rom_7c7b9c/overlay.ld. Th |
| `OvlFunc_943_2008bb8` | `0x02008bb8` | — | read | Slotted between ovl_30_c_a_a_c_a_c_a_a_a.o and the rest of the overlay. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile |
| `OvlFunc_943_2008bf0` | `0x02008bf0` | — | read | OvlFunc_943_2008bf0 extracted from goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_a_c.s. BUILT WITH -fno-rerun-cse-after-loop; see C |
| `OvlFunc_943_2008c28` | `0x02008c28` | — | read | in goldensun/overlays/rom_7c7b9c/overlay.ld. Dispatches on an interaction halfword, then either moves the player or plays a refusal sound. T |
| `OvlFunc_943_20093d4` | `0x020093d4` | — | read | OvlFunc_943_20093d4  --  0x020093d4 Cut out of goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_b.s. Resets the fight-scene state: |
| `OvlFunc_943_20097a0` | `0x020097a0` | — | read | A placement routine: seven __MapActor_SetPos calls and four halfword writes to the actors they place.  Three levers, and two of them are the |
| `OvlFunc_943_2009d0c` | `0x02009d0c` | — | read | extern unsigned char L5160[] __asm__(".L5160"); extern char *iwram_3001ebc; extern void __CutsceneStart(void); extern void __LoadFieldActors |
| `OvlFunc_943_200b150` | `0x0200b150` | — | read | Slotted between ovl_30_c_a_a_c_a_c_a_c_a.o and the rest of the overlay. A SWITCH AND AN UN-ROTATED LOOP, and both selectors are UNSIGNED. Th |
| `OvlFunc_943_200b1a8` | `0x0200b1a8` | — | read | extern unsigned char iwram_3001e70[]; extern int L5b38 __asm__(".L5b38"); extern int L5b50[] __asm__(".L5b50"); extern int L5b58 __asm__(".L |
| `OvlFunc_943_200b284` | `0x0200b284` | — | read | OvlFunc_943_200b284 -- MATCHES on the default flags (and unchanged under -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_7c7b9c/ovl_30_c_ |
| `OvlFunc_943_200b380` | `0x0200b380` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_7c7b9c/overlay.ld, so the ROM layo |
| `OvlFunc_943_200b3b8` | `0x0200b3b8` | — | read | extern int L5b70[] __asm__(".L5b70"); extern int OvlFunc_943_200b150(int i); extern int OvlFunc_943_200b464(int i); void OvlFunc_943_200b3b8 |
| `OvlFunc_943_200b464` | `0x0200b464` | — | read | Given a small selector, pick a base flag id and return the first entry of a word table whose corresponding flag is set, or 0 if none of nine |
| `OvlFunc_943_200b950` | `0x0200b950` | — | read | OvlFunc_943_200b950  --  0x0200b950 Cut out of goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_c_a.s. Repaints a doorway in four strips and |
| `OvlFunc_943_200b9b8` | `0x0200b9b8` | — | read | Slotted between ovl_30_c_a_a_c_c_a.o and the rest of the overlay. SHARED STACK-ARG VALUE, held in r5 across both calls -- a callee-saved reg |
| `OvlFunc_944_2008030` | `0x02008030` | — | read | OvlFunc_944_2008030, the whole of goldensun/asm/overlays/rom_7ca63c/ovl_30_a_a_a.s. no linker-script change was needed. Places an actor at a |
| `OvlFunc_944_20080c0` | `0x020080c0` | — | read | extern int __Random(void); int OvlFunc_944_20080c0(char *p) { short *q; int v; int n1, n2, n3, n4; int t1, t2, t3, t4, tm; int sv; q = (shor |
| `OvlFunc_944_2008a84` | `0x02008a84` | — | read | OvlFunc_944_2008a84  --  0x02008a84 Cut out of goldensun/asm/overlays/rom_7ca63c/ovl_30_c_c_a_c_c.s; the other seven functions stay as assem |
| `OvlFunc_944_200915c` | `0x0200915c` | — | read | THE THIRD AND LAST of the three byte-identical copies named in src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_b.c -- one per overlay, differin |
| `OvlFunc_945_2008284` | `0x02008284` | — | read | struct Actor { unsigned char pad00[6]; short f6; unsigned char pad08[0x5b - 8]; unsigned char f5b; unsigned char pad5c[0x62 - 0x5c]; unsigne |
| `OvlFunc_945_2008340` | `0x02008340` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_945_200854c` | `0x0200854c` | — | read | OvlFunc_945_200854c  --  0x0200854c Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_a_a_a.s. Chooses which script the innkeeper  |
| `OvlFunc_945_2008670` | `0x02008670` | — | read | A four-way talk on two save flags and a yes/no answer. `v = 0xd0 << 8;` at the top is the basic-block lever -- the call that uses it is thre |
| `OvlFunc_945_2008728` | `0x02008728` | — | read | extern unsigned char *iwram_3001ebc; extern int __GetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); exter |
| `OvlFunc_945_20087f8` | `0x020087f8` | — | read | OvlFunc_945_20087f8  --  0x020087f8 Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_a_c.s. The Psynergy tutor. Once his flag is  |
| `OvlFunc_945_20088ec` | `0x020088ec` | — | read | extern int __GetFlag(int flag); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __MessageID(int id); extern  |
| `OvlFunc_945_20089b4` | `0x020089b4` | — | read | OvlFunc_945_20089b4  --  0x020089b4 Cut from goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_a.s. The shop-visit script: with the party |
| `OvlFunc_945_2008b84` | `0x02008b84` | — | read | The target was the FIRST of two functions, so there is no _a part. BUILT WITH CSE_CFLAGS -- see the rule in the Makefile.  The flag 0x300 is |
| `OvlFunc_945_2008cc8` | `0x02008cc8` | — | read | OvlFunc_945_2008b84 was split out of the original, so no further split was needed and overlay.ld is untouched. BUILT WITH CSE_CFLAGS, for th |
| `OvlFunc_945_2008e14` | `0x02008e14` | — | read | OvlFunc_945_2008e14  --  0x02008e14 OvlFunc_945_2008ee0  --  0x02008ee0 OvlFunc_945_2008fac  --  0x02008fac OvlFunc_945_2009078  --  0x02009 |
| `OvlFunc_945_2008ee0` | `0x02008ee0` | — | read | OvlFunc_945_2008e14  --  0x02008e14 OvlFunc_945_2008ee0  --  0x02008ee0 OvlFunc_945_2008fac  --  0x02008fac OvlFunc_945_2009078  --  0x02009 |
| `OvlFunc_945_2008fac` | `0x02008fac` | — | read | OvlFunc_945_2008e14  --  0x02008e14 OvlFunc_945_2008ee0  --  0x02008ee0 OvlFunc_945_2008fac  --  0x02008fac OvlFunc_945_2009078  --  0x02009 |
| `OvlFunc_945_2009078` | `0x02009078` | — | read | OvlFunc_945_2008e14  --  0x02008e14 OvlFunc_945_2008ee0  --  0x02008ee0 OvlFunc_945_2008fac  --  0x02008fac OvlFunc_945_2009078  --  0x02009 |
| `OvlFunc_945_2009280` | `0x02009280` | — | read | OvlFunc_945_2009280  --  0x02009280 Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_a_a_c_a_c_c.s. Asks whether the party leader can |
| `OvlFunc_945_2009804` | `0x02009804` | — | read | OvlFunc_945_2009804  --  0x02009804 Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_a.s. The shared body of the "f |
| `OvlFunc_945_2009894` | `0x02009894` | — | read | extern char *__MapActor_GetActor(int slot); extern int __GetFlag(int); extern void __UI_Sanctum(int n); extern void __CutsceneStart(void); e |
| `OvlFunc_945_2009978` | `0x02009978` | — | read | typedef struct { unsigned char _bytes[704]; } GlobalState; extern GlobalState gState; extern int _AREA_6f; extern void __CutsceneStart(void) |
| `OvlFunc_945_2009a08` | `0x02009a08` | — | read | OvlFunc_945_2009a08  --  0x02009a08 A short cue that only runs once its flag is set. Three constants need the BASIC-BLOCK LEVER with the `if |
| `OvlFunc_945_2009a60` | `0x02009a60` | — | read | OvlFunc_945_2009a60  --  0x02009a60 The long version of the same scene: seventeen calls of stage direction between one flag test and one fla |
| `OvlFunc_945_200b66c` | `0x0200b66c` | — | read | functions in that .s, which is the largest cluster split so far. Area-entry dispatch: a chain of flag tests, each arm placing actors and ret |
| `OvlFunc_945_200b7d8` | `0x0200b7d8` | — | read | extern int __GetFlag(int id); extern int OvlFunc_945_200cfa8(int a, int b); extern void OvlFunc_945_200c890(int a, int b, int c, int d); ext |
| `OvlFunc_945_200bf94` | `0x0200bf94` | — | read | Slotted after asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a.o in in that .s, so there is no _c part. A ledge-climb cutscene:  |
| `OvlFunc_945_200c198` | `0x0200c198` | — | read | Split out of that .s; the sibling part stays as assembly. A staging cutscene: two slots configured, a helper run, a third slot configured, o |
| `OvlFunc_945_200c5d0` | `0x0200c5d0` | — | read | extern int L6968 __asm__(".L6968"); extern int __GetFlag(int id); extern void __SetFlag(int id); extern unsigned char *__CreateActor(int a,  |
| `OvlFunc_945_200c7cc` | `0x0200c7cc` | — | read | OvlFunc_945_200c7cc  --  0x0200c7cc Cut out of goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_c.s. Gives each of nine villagers  |
| `OvlFunc_945_200cfa8` | `0x0200cfa8` | — | read | Slotted between ovl_30_c_c_c_c_c_c_a_c_c_a_c_a.o and the rest of the overlay. A SWITCH AND AN UN-ROTATED LOOP, and both selectors are UNSIGN |
| `OvlFunc_945_200d068` | `0x0200d068` | — | read | A staging cutscene, near-twin of OvlFunc_945_200c198 (src/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_c_b.c). PARKED FOR A WHOLE ROUND ON |
| `OvlFunc_945_200d6dc` | `0x0200d6dc` | — | read | extern unsigned char gScript_945__0200e840[]; extern unsigned char gScript_945__0200e8e4[]; extern int OvlFunc_945_200cfa8(int a, int b); ex |
| `OvlFunc_945_200dc48` | `0x0200dc48` | — | read | extern int L7f84 __asm__(".L7f84"); extern int __Random(void); extern void __PlaySound(int id); extern void __Func_8012330(int a, int b, int |
| `OvlFunc_945_200e3ac` | `0x0200e3ac` | — | read | extern int __GetFlag(int flag); extern void __ClearFlag(int flag); extern void __SetFlag(int flag); void OvlFunc_945_200e3ac(int a, int b) { |
| `OvlFunc_946_20089dc` | `0x020089dc` | — | read | Appended after the _a piece in goldensun/overlays/rom_7ced6c/overlay.ld. Writes a two-bit selector into bits 2-3 of the sprite flag byte at  |
| `OvlFunc_946_20089f4` | `0x020089f4` | — | read | of the same .s in goldensun/overlays/rom_7ced6c/overlay.ld. ONE OF FOUR OPERAND-IDENTICAL COPIES, elevated together from a single source wit |
| `OvlFunc_946_2008a4c` | `0x02008a4c` | — | read | OvlFunc_946_2008a4c, the whole of goldensun/asm/overlays/rom_7ced6c/ovl_30_a_a_c_c_c_c_a.s. no linker-script change was needed. Spawns an ac |
| `OvlFunc_946_2008ab0` | `0x02008ab0` | — | read | Slotted between the _a and _c pieces in goldensun/overlays/rom_7ced6c/overlay.ld. Advances an entity by its per-axis deltas and spins its at |
| `OvlFunc_946_2008cc4` | `0x02008cc4` | — | read | GetEntrances, 6-way form: selects one of 6 per-area tables from the gState halfword at +0x1C0, falling through to the last. THE EARLIER FAMI |
| `OvlFunc_946_2008d48` | `0x02008d48` | — | read | translation unit and the linker script is untouched. Two equality tests on the AREA ID at gState+0x1C0, then a RANGE test over the area spac |
| `OvlFunc_946_2008da4` | `0x02008da4` | — | read | One of five byte-identical copies of the per-frame integrator, one per overlay. See src/overlays/rom_7a5214/ovl_17ec_c_b.c for the full acco |
| `OvlFunc_946_2008ec4` | `0x02008ec4` | — | read | tools/asmfacts.py, not inferred from the function count. GetEntrances, 6-way form: selects one of 6 per-area tables from the gState halfword |
| `OvlFunc_946_2009214` | `0x02009214` | — | read | typedef unsigned char u8; struct A { u8 pad00[0xc]; int fc; u8 pad10[0x13]; u8 f23; u8 pad24[0x31]; u8 f55; }; extern unsigned char gState[] |
| `OvlFunc_946_20092b4` | `0x020092b4` | — | read | extern unsigned char gState[]; extern int _AREA_7e; extern unsigned char *__MapActor_GetActor(int slot); extern int __GetFlag(int id); exter |
| `OvlFunc_946_20093ac` | `0x020093ac` | — | read | OvlFunc_946_20093ac  --  0x020093ac Cut out of goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_a_a.s. Runs the arrival cutscene for one o |
| `OvlFunc_946_2009508` | `0x02009508` | — | read | in goldensun/overlays/rom_7ced6c/overlay.ld. THE ARG-INTERLEAVE LEVER: assign the shifted constant to a local in a DIFFERENT BASIC BLOCK fro |
| `OvlFunc_946_2009548` | `0x02009548` | — | read | in goldensun/overlays/rom_7ced6c/overlay.ld. THE ARG-INTERLEAVE LEVER: assign the shifted constant to a local in a DIFFERENT BASIC BLOCK fro |
| `OvlFunc_946_200958c` | `0x0200958c` | — | read | further split was needed. THE ARG-INTERLEAVE LEVER: assign the shifted constant to a local in a DIFFERENT BASIC BLOCK from the call. The ROM |
| `OvlFunc_946_2009624` | `0x02009624` | — | read | translation unit and the linker script is untouched -- gcc regenerates the .s at the same path. Matched on the first screen with the stack-a |
| `OvlFunc_946_200967c` | `0x0200967c` | — | read | extern int __GetFlag(int id); extern void __Func_8010704(int a, int b, int c, int d, int e, int f); extern void __MapActor_SetPos(int slot,  |
| `OvlFunc_946_2009740` | `0x02009740` | — | read | Third and fourth member of the 24-BYTE STRUCT-BY-VALUE family, byte-identical to src/overlays/rom_7a1ff0/ovl_30_c_c_a_a.c apart from the two |
| `OvlFunc_946_200985c` | `0x0200985c` | — | read | OvlFunc_946_200985c  --  0x0200985c OvlFunc_946_20098b0  --  0x020098b0 The back two functions of goldensun/asm/overlays/rom_7ced6c/ovl_30_c |
| `OvlFunc_946_20098b0` | `0x020098b0` | — | read | OvlFunc_946_200985c  --  0x0200985c OvlFunc_946_20098b0  --  0x020098b0 The back two functions of goldensun/asm/overlays/rom_7ced6c/ovl_30_c |
| `OvlFunc_946_2009a44` | `0x02009a44` | — | read | extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Actor_SetAnim(char *, int); extern void __WaitFrames(int); |
| `OvlFunc_946_2009b14` | `0x02009b14` | — | read | Slotted between ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a.o and the rest of the overlay. One of the "position triple on the stack" family |
| `OvlFunc_946_2009b68` | `0x02009b68` | — | read | OvlFunc_946_2009b68 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a.s. One of the "position tr |
| `OvlFunc_946_2009bbc` | `0x02009bbc` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __WaitFrames(int n); extern void OvlFunc_946_2009774(int a, int b, int c);  |
| `OvlFunc_946_2009de0` | `0x02009de0` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void OvlFunc_946_2009774(int a, int b, int c); extern void __WaitFrames(int n);  |
| `OvlFunc_946_2009e5c` | `0x02009e5c` | — | read | struct A { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; }; extern struct A *__MapActor_GetActor(int slot); extern void O |
| `OvlFunc_946_2009ef4` | `0x02009ef4` | — | read | struct Actor { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; }; extern struct Actor *__MapActor_GetActor(int slot); exter |
| `OvlFunc_946_2009f78` | `0x02009f78` | — | read | function alone with no data, so no split was needed. The smallest member of the map-dispatcher family: only two actor values and a three-arm |
| `OvlFunc_946_200a004` | `0x0200a004` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void OvlFunc_946_2009774(int a, int b, int c); extern void __WaitFrames(int n);  |
| `OvlFunc_946_200a080` | `0x0200a080` | — | read | in the overlay script. Map-dispatcher family, four actor values and a five-arm chain with two double-call arms. Call in every arm; gcc cross |
| `OvlFunc_946_200a200` | `0x0200a200` | — | read | struct A { unsigned char pad0[8]; int f8; unsigned char padc[4]; int f10; }; extern struct A *__MapActor_GetActor(int id); extern void OvlFu |
| `OvlFunc_946_200a2c8` | `0x0200a2c8` | — | read | The fifth and largest of the map-dispatcher family in this overlay, and the one that shows the cross-jumping rule most clearly: three of its |
| `OvlFunc_946_200a450` | `0x0200a450` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void OvlFunc_946_2009774(int a, int b, int c); extern void __WaitFrames(int n);  |
| `OvlFunc_946_200a4c8` | `0x0200a4c8` | — | read | chunk are consecutive. Another of the map-dispatcher family (see batch 152). Reads several actor fields as `*(int *)(actor + off) >> 20`, di |
| `OvlFunc_946_200a5f0` | `0x0200a5f0` | — | read | chunk are consecutive. Another of the map-dispatcher family (see batch 152). Reads several actor fields as `*(int *)(actor + off) >> 20`, di |
| `OvlFunc_946_200a700` | `0x0200a700` | — | read | Another member of the map-dispatcher family (batch 152). The call goes in EVERY arm and gcc cross-jumps the identical tails; arms sharing a  |
| `OvlFunc_946_200a848` | `0x0200a848` | — | read | Another member of the map-dispatcher family (batch 152). The call goes in EVERY arm and gcc cross-jumps the identical tails; arms sharing a  |
| `OvlFunc_946_200a984` | `0x0200a984` | — | read | chunk are consecutive. Another of the map-dispatcher family (see batch 152). Reads several actor fields as `*(int *)(actor + off) >> 20`, di |
| `OvlFunc_946_200aa98` | `0x0200aa98` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __WaitFrames(int n); extern void OvlFunc_946_2009774(int a, int b, int c);  |
| `OvlFunc_946_200ab80` | `0x0200ab80` | — | read | split siblings; the four functions of the original chunk are consecutive. One of a family of four near-identical map dispatchers in this ove |
| `OvlFunc_946_200ac4c` | `0x0200ac4c` | — | read | split siblings; the four functions of the original chunk are consecutive. One of a family of four near-identical map dispatchers in this ove |
| `OvlFunc_946_200ad0c` | `0x0200ad0c` | — | read | split siblings; the four functions of the original chunk are consecutive. One of a family of four near-identical map dispatchers in this ove |
| `OvlFunc_946_200aed8` | `0x0200aed8` | — | read | OvlFunc_946_200aed8  --  0x0200aed8, cut from Dresses an actor as a portrait: clear two sprite-attribute fields, set the animation mode, hid |
| `OvlFunc_947_2008ec8` | `0x02008ec8` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern int OvlFunc_947_2008ddc(int a, int *b, int *c, int *d, int *e, int *f); extern v |
| `OvlFunc_947_2008fcc` | `0x02008fcc` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" extern unsigned char *iwram_3001e70; void OvlFunc_947_2008fcc(int a, int b, int  |
| `OvlFunc_947_200901c` | `0x0200901c` | — | read | OvlFunc_947_200901c  --  asm/overlays/rom_7d0e88/ovl_314_c_a_a.s Copies two bitfields and two bytes of one 4-byte map cell onto another. Sam |
| `OvlFunc_947_20091c4` | `0x020091c4` | — | read | struct P { int f0; int f4; int f8; int fc; unsigned char pad10[0x28 - 0x10]; }; struct A { unsigned char pad00[8]; int f8; int fc; int f10;  |
| `OvlFunc_947_2009440` | `0x02009440` | — | read | tools/asmfacts.py, not inferred from the function count. GetEntrances, 6-way form: selects one of 6 per-area tables from the gState halfword |
| `OvlFunc_947_20094c4` | `0x020094c4` | — | read | GetEntrances, 5-way form: selects one of five per-area tables from the gState halfword at +0x1c0, falling through to the last. Same shape as |
| `OvlFunc_947_2009544` | `0x02009544` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in A six-word struct is filled by one routine and passed BY VALUE to  |
| `OvlFunc_947_2009578` | `0x02009578` | — | read | translation unit and the linker script is untouched. Picks one of two three-word scroll-offset tables by a VCOUNT window and a weighted rand |
| `OvlFunc_947_200a0f0` | `0x0200a0f0` | — | read | keeps its name and its slot in goldensun/overlays/rom_7d0e88/overlay.ld is unchanged. Installs an update hook, hands the actor's integer pos |
| `OvlFunc_947_200a230` | `0x0200a230` | — | read | struct Cfg { int f0; int f4; int f8; int fc; int f10; int f14; int f18; int f1c; short f20; short f22; void (*f24)(void); }; extern volatile |
| `OvlFunc_947_200a2d8` | `0x0200a2d8` | — | read | struct P { int f0; int f4; int f8; int fc; unsigned char pad10[0x28 - 0x10]; }; struct A { unsigned char pad00[8]; int f8; int fc; int f10;  |
| `OvlFunc_947_200a4cc` | `0x0200a4cc` | — | read | OvlFunc_947_200a4cc Cut out of goldensun/asm//overlays/rom_7d0e88/ovl_1528_c_c_c_c_c_a.s. Drops the bridge once the player reaches the right |
| `OvlFunc_947_200a53c` | `0x0200a53c` | — | read | Fills a 24-byte block from a helper and, if that succeeds, passes it BY VALUE to another; otherwise runs a three-call fallback. A 24-BYTE ST |
| `OvlFunc_947_200a580` | `0x0200a580` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. GetEntrances, 6-way form: sele |
| `OvlFunc_947_200a5f8` | `0x0200a5f8` | — | read | #include "gba/types.h" #include "actor.h" extern struct Actor *__MapActor_GetActor(int slot); extern void __Actor_SetSpriteFlags(struct Acto |
| `OvlFunc_947_200a63c` | `0x0200a63c` | — | read | Slotted between ovl_2580_a_c_a.o and the rest of the overlay. Sets an actor's animation, hangs a callback off it, and hands its tile coordin |
| `OvlFunc_947_200a6b8` | `0x0200a6b8` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __Func_8092b08(int a, int b); void OvlFunc_947_200a6b8(void) { unsigned cha |
| `OvlFunc_948_20089f0` | `0x020089f0` | — | read | GetEntrances, 4-way form. Returns a named global from at least one arm, which is why the family sweeps in batches 08-13 missed it -- they ma |
| `OvlFunc_948_2008a50` | `0x02008a50` | — | read | GetEntrances, 4-way form. Returns a named global from at least one arm, which is why the family sweeps in batches 08-13 missed it -- they ma |
| `OvlFunc_948_2008ad0` | `0x02008ad0` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void |
| `OvlFunc_948_2008ec8` | `0x02008ec8` | — | read | Overlay 948: the same idle-animation reset, for slot 15. Split out of asm/overlays/rom_7d30e0/ovl_30_c_a_c_c_a_a_a_c_c.s; the remaining part |
| `OvlFunc_948_2008ee0` | `0x02008ee0` | — | read | GetEntrances, 4-way form, returning a named global from one arm. That is why the family sweeps in batches 08-13 missed it -- they matched on |
| `OvlFunc_948_2008f40` | `0x02008f40` | — | read | extern void __PlaySound(int id); extern void __WaitFrames(int n); extern void __Func_8012330(int a, int b, int c); extern void __Func_80105d |
| `OvlFunc_948_2008fdc` | `0x02008fdc` | — | read | extern void __PlaySound(int id); extern void __WaitFrames(int n); extern void __Func_8012330(int a, int b, int c); extern void __Func_80105d |
| `OvlFunc_948_2009070` | `0x02009070` | — | read | Slotted between ovl_30_c_a_c_c_a_a_c_a_a_a.o and the rest of the overlay. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefi |
| `OvlFunc_948_20090b8` | `0x020090b8` | — | read | Slotted between ..._c_a_a.o and the rest of the overlay. Three guards, each an early return to the shared epilogue. The flag id is `n + (0x9 |
| `OvlFunc_948_2009120` | `0x02009120` | — | read | the overlay in goldensun/overlays/rom_7d30e0/overlay.ld. First of a twin pair differing only in the sub-state number. Runs sub-state 2, then |
| `OvlFunc_948_200915c` | `0x0200915c` | — | read | further split was needed. Twin of ovl_30_c_a_c_c_a_a_c_c_c_a_a_b.c, differing only in the sub-state number. Runs sub-state 3, then -- only w |
| `OvlFunc_948_200952c` | `0x0200952c` | — | read | struct Actor { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; }; extern unsigned char gState[]; extern int L2f74[] __asm__ |
| `OvlFunc_948_20095f0` | `0x020095f0` | — | read | extern unsigned char gState[]; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __CutsceneWait(int n); extern |
| `OvlFunc_948_20098e0` | `0x020098e0` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __ClearFlag(in |
| `OvlFunc_948_20099e8` | `0x020099e8` | — | read | OvlFunc_948_20099e8  --  0x020099e8 The whole of goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_a_a_c_a.s, which held this function and nothin |
| `OvlFunc_948_2009a70` | `0x02009a70` | — | read | in goldensun/overlays/rom_7d30e0/overlay.ld. One map edit, then clear-and-set a byte on actor 8. FOUND BY tools/match_shapes.py --near, two  |
| `OvlFunc_948_2009a9c` | `0x02009a9c` | — | read | overlay in goldensun/overlays/rom_7d30e0/overlay.ld. A map edit followed by a byte write on an actor. The fourth, fifth and sixth members of |
| `OvlFunc_948_2009b60` | `0x02009b60` | — | read | OvlFunc_948_2009b60 Cut out of goldensun/asm//overlays/rom_7d30e0/ovl_30_c_c_a_a_c_c_a_c_c_a_b.s. Dispatches on where the player and the fol |
| `OvlFunc_948_2009bc4` | `0x02009bc4` | — | read | Slotted between ovl_30_c_c_a_a_c_c_a_c_c_a.o and the rest of the overlay. Both arms of the `if` pass the SAME stack-arg pair (0x2d, 0x2b) an |
| `OvlFunc_948_2009c6c` | `0x02009c6c` | — | read | OvlFunc_948_2009c6c extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_a_c_a_a.s. SHARED STACK-ARG VALUE, held in r5 across both ca |
| `OvlFunc_948_2009ca0` | `0x02009ca0` | — | read | A map edit followed by a byte write on an actor. The fourth, fifth and sixth members of the family headed by src/overlays/rom_7d30e0/ovl_30_ |
| `OvlFunc_948_2009ccc` | `0x02009ccc` | — | read | Differs from its immediate neighbour ovl_30_c_c_a_c_a_b.c in ONE constant -- the [sp] value, 0x2a against 0x26. Everything else is identical |
| `OvlFunc_948_2009cf8` | `0x02009cf8` | — | read | OvlFunc_948_2009cf8 Cut out of goldensun/asm//overlays/rom_7d30e0/ovl_30_c_c_a_c_a_c_c.s. Three position tests that pick which follow-up run |
| `OvlFunc_948_2009da0` | `0x02009da0` | — | read | Slotted between ovl_30_c_c_c_a_a.o and the rest of the overlay. Four calls with identical register arguments, sharing the SECOND stack slot  |
| `OvlFunc_948_2009edc` | `0x02009edc` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __Func_8092b08(int a, int b); void OvlFunc_948_2009edc(void) { int v; if (* |
| `OvlFunc_948_2009f78` | `0x02009f78` | — | read | OvlFunc_948_2009f78  --  0x02009f78 Cut out of goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c.s. Runs whichever of three area-spec |
| `OvlFunc_948_2009fd8` | `0x02009fd8` | — | read | OvlFunc_948_2009fd8  --  0x02009fd8 Cut out of goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c.s. `.L2f80` -- the frame counter t |
| `OvlFunc_948_200a188` | `0x0200a188` | — | read | in goldensun/overlays/rom_7d30e0/overlay.ld.  The trailing .data follows the last function and travels with _c. Restores four actor position |
| `OvlFunc_949_2008170` | `0x02008170` | — | read | OvlFunc_949_2008170  --  0x02008170, cut from Hands an actor to the interaction handler twice: once with kind 0x20, and if that comes back z |
| `OvlFunc_949_2008224` | `0x02008224` | — | read | THE FIRST FUNCTION EVER MATCHED THROUGH THE ARG-INTERLEAVE BLOCKER. THE ARG-INTERLEAVE LEVER: assign the shifted constant to a local in a DI |
| `OvlFunc_949_20085dc` | `0x020085dc` | — | read | Slotted between ovl_30_c_c_a_c_c_c_c_c_c_a.o and the rest of the overlay. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefi |
| `OvlFunc_949_2008644` | `0x02008644` | — | read | OvlFunc_949_2008644  --  0x02008644 Cut out of goldensun/asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_c.s. ONE OF THE MAP-EXIT FAMILY. A |
| `OvlFunc_950_200809c` | `0x0200809c` | — | read | Slotted between ovl_30_c_c_a_c_a_a_a.o and the rest of the overlay. Sets two fields of the iwram_3001ebc block and hands off. The argument i |
| `OvlFunc_950_2008328` | `0x02008328` | — | read | extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __MessageID(int id); extern void __CutsceneWait(int n); exte |
| `OvlFunc_950_2008500` | `0x02008500` | — | read | Same three-line-exchange shape as OvlFunc_962_200806c: the message base is a SYMBOL, `(int)(&_MSG_1fd5)`, which is what makes gcc spend the  |
| `OvlFunc_950_20085a8` | `0x020085a8` | — | read | data-free by split_s.py, so no split was needed. TWO LEVERS, and the second is the more interesting one. The message base is a SYMBOL, `(int |
| `OvlFunc_950_200866c` | `0x0200866c` | — | read | OvlFunc_950_200866c  --  0x0200866c The last function of goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c.s; the other six stay as ass |
| `OvlFunc_950_20086ec` | `0x020086ec` | — | read | A three-message prompt inside a cutscene, byte-for-byte the shape of src/overlays/rom_7d5838/ovl_30_c_c_a_c_c_b.c one round earlier -- found |
| `OvlFunc_950_2008760` | `0x02008760` | — | read | A three-message prompt wrapped in a cutscene, with a ten-frame wait on the yes arm. Same two levers as src/overlays/rom_7d768c/ovl_30_c_a_a_ |
| `OvlFunc_950_20088cc` | `0x020088cc` | — | read | A three-message prompt inside a cutscene, byte-for-byte the shape of src/overlays/rom_7d5838/ovl_30_c_c_a_c_c_b.c one round earlier -- found |
| `OvlFunc_950_200891c` | `0x0200891c` | — | read | OvlFunc_950_200891c  --  0x0200891c The function half of goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_c_c.s; the .data half (nine .incbin bl |
| `OvlFunc_951_2008044` | `0x02008044` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_951_2008074` | `0x02008074` | — | read | Split out of that .s; the sibling parts stay as assembly. A three-message prompt: says the opening line, runs a check, and delivers one of t |
| `OvlFunc_951_20080bc` | `0x020080bc` | — | read | A three-message prompt: says the opening line, runs a check, and delivers one of two follow-ups at base+1 or base+2. One of seven identical  |
| `OvlFunc_951_20081a8` | `0x020081a8` | — | read | Split out of that .s; the sibling parts stay as assembly. GetEntrances, 2-way form: selects one of 2 per-area tables from the gState halfwor |
| `OvlFunc_951_20088f8` | `0x020088f8` | — | read | REQUIRES `_MSG_e23 = 0xe23;` in message.sym. Four levers, 89 of 98 down to exact, and three of them were lifted off the sibling src/overlays |
| `OvlFunc_951_20089f8` | `0x020089f8` | — | read | A coin-check: multiply a party total by ten, compare it against gState+0x10 UNSIGNED, and take one of two message paths.  The early-return a |
| `OvlFunc_951_2008d70` | `0x02008d70` | — | read | OvlFunc_951_2008d70  --  0x02008d70 Cut out of goldensun/asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c.s. Picks a line of dialogue for one of six |
| `OvlFunc_952_2008030` | `0x02008030` | — | read | GetEntrances for this map: picks one of two edge-transition tables from a gState halfword. One of an 18-member family; see src/overlays/rom_ |
| `OvlFunc_952_2008070` | `0x02008070` | — | read | translation unit and the linker script is untouched. A four-way selector on the area id at gState+0x1C0. UNPARKED BY NAMING A CONSTANT. This |
| `OvlFunc_952_20080c8` | `0x020080c8` | — | read | A three-message prompt: set the base message, have the actor turn, ask, and then show base+1 or base+2 depending on the answer. TWO LEVERS,  |
| `OvlFunc_952_200849c` | `0x0200849c` | — | read | OvlFunc_952_200849c Cut out of goldensun/asm//overlays/rom_7d768c/ovl_30_c_a_a_c_a_b.s. A two-stage conversation with a one-shot branch. The |
| `OvlFunc_952_2008524` | `0x02008524` | — | read | A three-message prompt: set the base message, have the actor turn, ask, and then show base+1 or base+2 depending on the answer. TWO LEVERS,  |
| `OvlFunc_952_2008564` | `0x02008564` | — | read | A three-message prompt: set the base message, have the actor turn, ask, and then show base+1 or base+2 depending on the answer. TWO LEVERS,  |
| `OvlFunc_952_20085a4` | `0x020085a4` | — | read | PARKED FOR SEVERAL BATCHES AT 3 DIFFERING; TWO SEPARATE LEVERS CLOSED IT. 1. THE MESSAGE BASE IS A SYMBOL, NOT AN INTEGER.  The ROM keeps 0x |
| `OvlFunc_952_200bf84` | `0x0200bf84` | — | read | Slotted between ovl_30_c_a_c_a_a.o and the rest of the overlay. BUILT WITH -fno-rerun-cse-after-loop; see CSE_CFLAGS in the Makefile and the |
| `OvlFunc_952_200bfc4` | `0x0200bfc4` | — | read | A three-message prompt, the variant that repeats the actor line in BOTH arms rather than joining first. See src/overlays/rom_7d768c/ovl_30_c |
| `OvlFunc_952_200c034` | `0x0200c034` | — | read | The remaining function and the .data section stay in the original .s, which keeps its name and its .data line in goldensun/overlays/rom_7d76 |
| `OvlFunc_953_2008030` | `0x02008030` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. GetEntrances, three-way form: selects one of three edge-transiti |
| `OvlFunc_953_200807c` | `0x0200807c` | — | read | OvlFunc_953_200807c  --  0x0200807c The whole of goldensun/asm/overlays/rom_7d95dc/ovl_30_c_c_a.s. Chooses a shopkeeper script: one area get |
| `OvlFunc_953_2008238` | `0x02008238` | — | read | The first function stays in the .s, which keeps its name and its slot; this piece is added after it in goldensun/overlays/rom_7d95dc/overlay |
| `OvlFunc_953_200839c` | `0x0200839c` | — | read | extern char *iwram_3001ebc; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __CutsceneWait(int n); extern in |
| `OvlFunc_953_200855c` | `0x0200855c` | — | read | OvlFunc_953_200855c  --  0x0200855c changes name in goldensun/overlays/rom_7d95dc/overlay.ld and nothing else moves. The tutorial conversati |
| `OvlFunc_953_200960c` | `0x0200960c` | — | read | OvlFunc_953_200960c  --  0x0200960c, cut from A cutscene script: arm the next map, fade in, wait, speak, and on a save bit bump a counter be |
| `OvlFunc_953_2009a14` | `0x02009a14` | — | read | Slotted between ovl_30_c_c_c_a_a_c_a.o and the rest of the overlay. Area dispatch, two-arm form: read the area halfword at gState+0x1c0 and  |
| `OvlFunc_953_2009a4c` | `0x02009a4c` | — | read | OvlFunc_953_2009a4c  --  0x02009a4c changes name in goldensun/overlays/rom_7d95dc/overlay.ld and nothing else moves. One frame of wait, then |
| `OvlFunc_953_2009c6c` | `0x02009c6c` | — | read | OvlFunc_953_2009c6c  --  0x02009c6c Cut out of goldensun/asm/overlays/rom_7d95dc/ovl_30_c_c_c_c_a.s. Repaints a doorway three ways depending |
| `OvlFunc_953_200a3e0` | `0x0200a3e0` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __Func_8079664(int n); extern void __AddPartyMember(int n); extern  |
| `OvlFunc_953_200a820` | `0x0200a820` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __Func_8079664(int n); extern void __AddPartyMember(int n); extern  |
| `OvlFunc_953_200ab1c` | `0x0200ab1c` | — | read | OvlFunc_953_200ab1c  --  0x0200ab1c, cut from Assigns palettes 3, 0, 4, 1, 5, 2, 6 to slots 0xc..0x12 -- the ordering is the ROM's and is tr |
| `OvlFunc_954_2008178` | `0x02008178` | — | read | Slotted between ovl_30_c_c_a_a_a_c_a.o and the rest of the overlay. A spin-wait with a timeout, written as an un-rotated `do`/`while` with a |
| `OvlFunc_954_20081a8` | `0x020081a8` | — | read | extern void __Func_8010704(int a, int b, int c, int d, int e, int f); extern unsigned char *__MapActor_GetActor(int slot); extern int __Func |
| `OvlFunc_954_2008490` | `0x02008490` | — | read | extern unsigned char gState[]; extern volatile int gKeyHeld; extern unsigned char *__MapActor_GetActor(int slot); extern void __Func_8010704 |
| `OvlFunc_954_20095e0` | `0x020095e0` | — | read | in goldensun/overlays/rom_7db0c8/overlay.ld.  The trailing .data follows the last function and travels with _c. TWO DIFFERENT LEVERS, PULLIN |
| `OvlFunc_955_200805c` | `0x0200805c` | — | read | OvlFunc_955_200805c extracted from goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_a_c_a.s. FOUND BY THE CONSTANT-CSE SEARCH AND IT IS NOT THAT |
| `OvlFunc_955_20080c0` | `0x020080c0` | — | read | extern void __Func_8010704(int a, int b, int c, int d, int e, int f); extern unsigned char *__MapActor_GetActor(int slot); extern void __Set |
| `OvlFunc_955_2008258` | `0x02008258` | — | read | Slotted between ovl_30_c_c_c_a_a_a_a.o and the rest of the overlay. Stack-arg-pair lever in its SHARED form -- 0x11 is both the second argum |
| `OvlFunc_955_200828c` | `0x0200828c` | — | read | Slotted between ovl_30_c_c_c_a_a_a_c_a.o and the rest of the overlay. Twin of ovl_30_c_c_c_a_a_a_b.c. |
| `OvlFunc_955_20082c0` | `0x020082c0` | — | read | Stack-arg-pair. Note 0x20 appears as both the first argument and the [sp] value and is NOT shared: the ROM builds r0 fresh with its own `mov |
| `OvlFunc_955_2008400` | `0x02008400` | — | read | extern unsigned char gState[]; extern unsigned int gKeyHeld; extern unsigned char *__MapActor_GetActor(int slot); extern void OvlFunc_955_20 |
| `OvlFunc_955_20089b0` | `0x020089b0` | — | read | OvlFunc_955_20089b0  --  0x020089b0, cut from Tears down a set piece: detach the entity hook, record the save bit, clear a second flag word  |
| `OvlFunc_955_20092f0` | `0x020092f0` | — | read | THIS FUNCTION WAS PARKED AND THE PARK WAS WRONG, which is worth more than the function.  It sat at 123 of 123 lines with 15 differing, the i |
| `OvlFunc_956_20081b4` | `0x020081b4` | — | read | Split out of that .s; the _c part stays as assembly and keeps its slot in Registers OvlFunc_956_200804c as a task at priority 0xc80. THE LOC |
| `OvlFunc_956_2008204` | `0x02008204` | — | read | First in the run, ahead of the _b piece, in goldensun/overlays/rom_7e0928/overlay.ld. Nudges two actors back by 0xcccc on the x axis when th |
| `OvlFunc_956_200824c` | `0x0200824c` | — | read | The one member that SETS its flag rather than clearing it. Here 0x3d is both the second argument and the value at [sp,#4]; writing the secon |
| `OvlFunc_956_2008274` | `0x02008274` | — | read | OvlFunc_956_2008274  --  0x02008274 Cut out of goldensun/asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_c.s. Sends two villagers walking off in  |
| `OvlFunc_956_2008404` | `0x02008404` | — | read | struct Actor { unsigned char pad0[8]; int f8; unsigned char pad0c[4]; int f10; unsigned char pad14[0x1c]; int f30; int f34; unsigned char pa |
| `OvlFunc_956_200858c` | `0x0200858c` | — | read | keeps its name and its slot in goldensun/overlays/rom_7e0928/overlay.ld is unchanged. Records an actor's tile column in a flag byte, then ma |
| `OvlFunc_956_20085e0` | `0x020085e0` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern int __Func_8011f54(int a, int b, int c); extern void __Func_8010704(int a, int b |
| `OvlFunc_956_2008658` | `0x02008658` | — | read | extern unsigned int gState; extern unsigned char *__MapActor_GetActor(int slot); extern void __Func_8012078(int a, int x, int y, int t); voi |
| `OvlFunc_956_2008714` | `0x02008714` | — | read | OvlFunc_956_2008714  --  0x02008714 Cut out of goldensun/asm/overlays/rom_7e0928/ovl_30_c_a_c_c.s. Probes four points around a position -- t |
| `OvlFunc_956_2008a44` | `0x02008a44` | — | read | keeps its name and its slot in goldensun/overlays/rom_7e0928/overlay.ld is unchanged. Sets an actor moving on a script: clear its interact b |
| `OvlFunc_956_2008b30` | `0x02008b30` | — | read | extern unsigned char gState[]; extern unsigned char *__MapActor_GetActor(int slot); extern void __Actor_SetAnim(void *a, int anim); extern v |
| `OvlFunc_956_200937c` | `0x0200937c` | — | read | Slotted between the _a and _c pieces in goldensun/overlays/rom_7e0928/overlay.ld. One per-frame integration step for a projectile-ish entity |
| `OvlFunc_956_200a4d0` | `0x0200a4d0` | — | read | ONE RESIDUE, ONE LEVER, AND THE LEVER IS AN ABSENT DECLARATION. Written with every callee prototyped, this is 96 of 96 lines with THREE diff |
| `OvlFunc_957_2008a00` | `0x02008a00` | — | read | Split out of that .s; the sibling part stays as assembly and keeps its slot in the overlay's linker script, so the ROM layout does not move. |
| `OvlFunc_957_2008c2c` | `0x02008c2c` | — | read | Slotted between the _a and _c pieces in Guards five identical calls on the AREA ID at gState+0x1C0. `_AREA_97` already existed. The five `__ |
| `OvlFunc_957_2008c98` | `0x02008c98` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" #include "actor.h" extern char *iwram_3001f30; extern void __Func_8092adc(int a, |
| `OvlFunc_957_2008cf8` | `0x02008cf8` | — | read | OvlFunc_957_2008cf8  --  0x02008cf8 Cut out of goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_c.s; the rest of that file stays |
| `OvlFunc_957_2008d58` | `0x02008d58` | — | read | Slotted between ovl_30_c_c_a_c_c_c_c_c_c_c_a_a.o and the rest of the overlay. Builds slot 0xb's position on the stack and hands it to a coll |
| `OvlFunc_957_2008d90` | `0x02008d90` | — | read | OvlFunc_957_2008d90  --  0x02008d90 Cut out of goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_c_a_c.s. A once-only map repaint g |
| `OvlFunc_957_2008eac` | `0x02008eac` | — | read | Third and fourth member of the 24-BYTE STRUCT-BY-VALUE family, byte-identical to src/overlays/rom_7a1ff0/ovl_30_c_c_a_a.c apart from the two |
| `OvlFunc_957_2008ee0` | `0x02008ee0` | — | read | OvlFunc_957_2008ee0  --  0x02008ee0, cut from the head of remaining function follows as ovl_30_c_c_a_c_c_c_c_c_c_c_c_c.o. A four-phase cycle |
| `OvlFunc_957_200b4bc` | `0x0200b4bc` | — | read | A three-message prompt, the variant where BOTH arms repeat the wait, the line and the actor speech rather than joining first. Two levers, bo |
| `OvlFunc_957_200b518` | `0x0200b518` | — | read | Split out of that .s; the sibling parts stay as assembly. A one-shot scene guarded by two flags: it runs only if 0x960 is set and 0x962 is n |
| `OvlFunc_957_200b598` | `0x0200b598` | — | read | Split out of that .s. tools/split_s.py refused the cut until the seven tables this function returns were declared .global -- they are data t |
| `OvlFunc_957_200b610` | `0x0200b610` | — | read | First in the run, ahead of the _b piece, in goldensun/overlays/rom_7e3e08/overlay.ld. The _b piece keeps the .data section. Clears a flag by |
| `OvlFunc_958_2008cc0` | `0x02008cc0` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_958_2008d20` | `0x02008d20` | — | read | Split out of that .s; the sibling part stays as assembly and keeps its slot in goldensun/overlays/rom_7e636c/overlay.ld, so the ROM layout d |
| `OvlFunc_958_2008d88` | `0x02008d88` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in goldensun/overlays/rom_7e636c/overlay.ld, so the ROM layout |
| `OvlFunc_958_2008f44` | `0x02008f44` | — | read | Matched on the first screen with NO lever, and the reason is worth recording because it cuts against a habit this corpus encourages. The ROM |
| `OvlFunc_958_2008fd0` | `0x02008fd0` | — | read | OvlFunc_958_2008fd0  --  0x02008fd0 Cut out of goldensun/asm/overlays/rom_7e636c/ovl_cc0_c_a_c_a_c_c_b.s. UNPARKS src/non_matching/ovl_7e636 |
| `OvlFunc_958_2009158` | `0x02009158` | — | read | OvlFunc_958_2009158  --  0x02009158 Cut out of goldensun/asm/overlays/rom_7e636c/ovl_cc0_c_a_c_c_c_c.s. Moves the guard aside once the playe |
| `OvlFunc_958_2009394` | `0x02009394` | — | read | Slotted before asm/overlays/rom_7e636c/ovl_cc0_c_c_c_c_c.o in SPLIT BY HAND, because tools/split_s.py correctly refused: the .s held ONE fun |
| `OvlFunc_959_20089dc` | `0x020089dc` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. Confirmed with tools/split_s.py, which refuses this shortcut whe |
| `OvlFunc_959_2008a34` | `0x02008a34` | — | read | translation unit and the linker script is untouched. Selects a script/table pointer from the AREA ID at gState+0x1C0. The compared constants |
| `OvlFunc_959_2008a80` | `0x02008a80` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. GetEntrances, 6-way form: sele |
| `OvlFunc_959_2008af8` | `0x02008af8` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_959_2008b4c` | `0x02008b4c` | — | read | keeps its name and its slot in goldensun/overlays/rom_7e7574/overlay.ld is unchanged. Two six-argument setup calls, then an actor is fetched |
| `OvlFunc_959_2008bec` | `0x02008bec` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __Func_8012330(int a, int b, int c); extern void __Func_8010704(int a, int  |
| `OvlFunc_959_2008c90` | `0x02008c90` | — | read | Slotted between ovl_9dc_a_c_c_a_a_a_a.o and the rest of the overlay. A TABLE OF PAIRS INDEXED BY THE PARAMETER, read with an ADVANCING OFFSE |
| `OvlFunc_959_2008ce0` | `0x02008ce0` | — | read | THE BASIC-BLOCK LEVER BREAKS CONSTANT-CSE, which is new and is what this function is worth reading for. `__Func_8012330(0xc0 << 10, 0xc0 <<  |
| `OvlFunc_959_2008dcc` | `0x02008dcc` | — | read | OvlFunc_959_2008dcc  --  0x02008dcc Cut out of goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_a.s. One of three identical shrine-offe |
| `OvlFunc_959_2008e30` | `0x02008e30` | — | read | Slotted between ovl_9dc_a_c_c_a_a_c_a.o and the rest of the overlay. A TABLE OF PAIRS INDEXED BY THE PARAMETER, read with an ADVANCING OFFSE |
| `OvlFunc_959_2008e80` | `0x02008e80` | — | read | OvlFunc_959_2008e80  --  0x02008e80 One of three identical shrine-offering handlers, found together by tools/prologue_families.py. They diff |
| `OvlFunc_959_2008ee0` | `0x02008ee0` | — | read | Slotted between ovl_9dc_a_c_c_a_a_c_c_a.o and the rest of the overlay. A TABLE OF PAIRS INDEXED BY THE PARAMETER, read with an ADVANCING OFF |
| `OvlFunc_959_2008f30` | `0x02008f30` | — | read | OvlFunc_959_2008f30  --  0x02008f30 One of three identical shrine-offering handlers, found together by tools/prologue_families.py. They diff |
| `OvlFunc_959_2009038` | `0x02009038` | — | read | keeps its name and its slot in goldensun/overlays/rom_7e7574/overlay.ld is unchanged. A short cutscene that succeeds or fails: spawn a helpe |
| `OvlFunc_959_20090a8` | `0x020090a8` | — | read | keeps its name and its slot in goldensun/overlays/rom_7e7574/overlay.ld is unchanged. A straight-line cutscene beat: walk the actor, wait, p |
| `OvlFunc_959_20092e0` | `0x020092e0` | — | read | A short beat: park an actor, face it, animate it, emote, hand off. Straight-line, twenty-five instructions. THIS CORRECTS A CLAIM MADE IN sr |
| `OvlFunc_959_2009718` | `0x02009718` | — | read | A polling task. Once its sub-check passes and a gState halfword has gone to zero, it unregisters ITSELF and writes 0x5f to [iwram_3001ebc]+0 |
| `OvlFunc_959_200981c` | `0x0200981c` | — | read | OvlFunc_959_200981c  --  0x0200981c Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_a_c_c_c_b.s. Is the actor within six tiles v |
| `OvlFunc_959_2009880` | `0x02009880` | — | read | struct Actor { unsigned char pad00[8]; int x; unsigned char pad0c[4]; int z; }; extern struct Actor *__MapActor_GetActor(int slot); int OvlF |
| `OvlFunc_959_2009918` | `0x02009918` | — | read | OvlFunc_959_2009918  --  0x02009918 Cut out of goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_a_a.s. Is the given actor within fo |
| `OvlFunc_959_2009980` | `0x02009980` | — | read | OvlFunc_959_2009980  --  0x02009980 Cut out of goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_a_a.s. Is the given actor inside a  |
| `OvlFunc_959_20099e8` | `0x020099e8` | — | read | OvlFunc_959_20099e8  --  0x020099e8 Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_a_a_c_c_b.s. A trigger tile: if th |
| `OvlFunc_959_2009a44` | `0x02009a44` | — | read | OvlFunc_959_2009a44  --  0x02009a44 Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_a_a_c_c_c.s. The same trigger shap |
| `OvlFunc_959_2009ab0` | `0x02009ab0` | — | read | OvlFunc_959_2009ab0 Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_a_b.s. A two-line exchange that walks the message  |
| `OvlFunc_959_2009b24` | `0x02009b24` | — | read | extern int _MSG_240d; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __MapActor_Emote(int slot, int a, int  |
| `OvlFunc_959_2009be4` | `0x02009be4` | — | read | OvlFunc_959_2009be4  --  0x02009be4, cut from Dispatches on two bits of an overlay word to one of four handlers. A PLAIN `switch` REPRODUCES |
| `OvlFunc_959_200a134` | `0x0200a134` | — | read | extern int _MSG_240d; extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __Func_809228c(int a, int b, int c); e |
| `OvlFunc_959_200a26c` | `0x0200a26c` | — | read | Slotted between ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_a.o and the rest of the overlay. A STACK-ARG VALUE SHARED ACROSS BOTH CALLS, and the ROM says  |
| `OvlFunc_959_200a2a0` | `0x0200a2a0` | — | read | Slotted between ..._c_a.o and the rest of the overlay. SHARED STACK-ARG VALUE, held in r5 across both calls -- a callee-saved register the p |
| `OvlFunc_959_200a2d4` | `0x0200a2d4` | — | read | Slotted between ..._c_c_a.o and the rest of the overlay. SHARED STACK-ARG VALUE, held in r5 across both calls -- a callee-saved register the |
| `OvlFunc_959_200a308` | `0x0200a308` | — | read | OvlFunc_959_200a308 Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_c_c_a_b.s. Opens the sealed door when both |
| `OvlFunc_959_200a38c` | `0x0200a38c` | — | read | OvlFunc_959_200a38c Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_c_c_a_b.s. Opens the sealed door when both |
| `OvlFunc_959_200a410` | `0x0200a410` | — | read | Slotted between ..._c_a.o and the rest of the overlay. TWO HELD VALUES ALTERNATING INTO ONE STACK SLOT. r6 and r5 are both pushed callee-sav |
| `OvlFunc_959_200a468` | `0x0200a468` | — | read | OvlFunc_959_200a468 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_c_c_c_c_c_c_c_c_c.s. TWO HELD VALUES ALTERNATING IN |
| `OvlFunc_959_200a52c` | `0x0200a52c` | — | read | extern unsigned char *__MapActor_GetActor(int slot); extern void __MapActor_SetPos(int slot, int x, int z); extern void __Func_8092adc(int a |
| `OvlFunc_959_200c638` | `0x0200c638` | — | read | OvlFunc_959_200c638  --  0x0200c638 Cut out of goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a.s. Speaks one of eight lines depending on a |
| `OvlFunc_959_200cbfc` | `0x0200cbfc` | — | read | data, so no split was needed and overlay.ld is untouched. BUILT WITH CSE_CFLAGS -- see the rule in the Makefile.  The flag id 0x226 is used  |
| `OvlFunc_959_200cd50` | `0x0200cd50` | — | read | Sets the active message, has an actor speak it, and -- only if the party is carrying item 0xea -- runs the message two ids later through the |
| `OvlFunc_960_200834c` | `0x0200834c` | — | read | GetEntrances, 4-way form, returning a named global from one arm. That is why the family sweeps in batches 08-13 missed it -- they matched on |
| `OvlFunc_960_20083ac` | `0x020083ac` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. GetEntrances, 4-way form: sele |
| `OvlFunc_960_2008400` | `0x02008400` | — | read | OvlFunc_960_2008400 Cut out of goldensun/asm//overlays/rom_7eaf28/ovl_314_c_a_c_a_c_b.s. A per-frame countdown on a save byte, with a scene  |
| `OvlFunc_960_2008464` | `0x02008464` | — | read | extern unsigned char gState[]; extern unsigned char *iwram_3001ebc; extern unsigned char *__MapActor_GetActor(int slot); extern int __GetFla |
| `OvlFunc_960_2008adc` | `0x02008adc` | — | read | extern int __GetFlag(int id); extern void __SetFlag(int id); extern void __MapActor_SetPos(int slot, int x, int z); extern void __MapActor_S |
| `OvlFunc_960_2008e5c` | `0x02008e5c` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_7eaf28/overlay.ld, so the ROM layo |
| `OvlFunc_961_2008068` | `0x02008068` | — | read | Split out of that .s; the _c part stays as assembly. A three-message prompt: says the opening line, runs a check, and delivers one of two fo |
| `OvlFunc_961_20080b0` | `0x020080b0` | — | read | A three-message prompt: says the opening line, runs a check, and delivers one of two follow-ups at base+1 or base+2. One of seven identical  |
| `OvlFunc_962_200806c` | `0x0200806c` | — | read | split_s.py, so no split was needed. THE MESSAGE BASE IS A SYMBOL. The ROM holds it in a callee-saved register and reaches the other two line |
| `OvlFunc_962_2008100` | `0x02008100` | — | read | OvlFunc_962_2008100  --  0x02008100 OvlFunc_962_200816c  --  0x0200816c The back two thirds of goldensun/asm/overlays/rom_7ec19c/ovl_30_c_a_ |
| `OvlFunc_962_200816c` | `0x0200816c` | — | read | OvlFunc_962_2008100  --  0x02008100 OvlFunc_962_200816c  --  0x0200816c The back two thirds of goldensun/asm/overlays/rom_7ec19c/ovl_30_c_a_ |
| `OvlFunc_962_20081d4` | `0x020081d4` | — | read | Split out of that .s; the _a part stays as assembly. A three-message prompt: says the opening line, runs a check, and delivers one of two fo |
| `OvlFunc_962_2008a78` | `0x02008a78` | — | read | struct Spr { unsigned char pad0[9]; unsigned char f9_0 : 2; unsigned char f9_2 : 2; unsigned char f9_4 : 4; unsigned char pad_a[0x14]; unsig |
| `OvlFunc_963_2008040` | `0x02008040` | — | read | .o keeps its name and its slot in the overlay's linker script is unchanged. GetEntrances, three-way form: selects one of three edge-transiti |
| `OvlFunc_963_200808c` | `0x0200808c` | — | read | translation unit and the linker script is untouched. Selects a script/table pointer from the AREA ID at gState+0x1C0. UNBLOCKED BY NAMING TW |
| `OvlFunc_963_20080e4` | `0x020080e4` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_963_2008730` | `0x02008730` | — | read | OvlFunc_963_2008730  --  0x02008730, cut from the tail of The follow-up scene: a different line depending on a save bit, and on one path a c |
| `OvlFunc_964_20089dc` | `0x020089dc` | — | read | Appended after the _a piece in goldensun/overlays/rom_7ed0a0/overlay.ld. Writes a two-bit selector into bits 2-3 of the sprite flag byte at  |
| `OvlFunc_964_20089f4` | `0x020089f4` | — | read | of the same .s in goldensun/overlays/rom_7ed0a0/overlay.ld. ONE OF FOUR OPERAND-IDENTICAL COPIES, elevated together from a single source wit |
| `OvlFunc_964_2008a4c` | `0x02008a4c` | — | read | OvlFunc_964_2008a4c, the whole of goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_a_a_c_c_c_c_c_a.s. no linker-script change was needed. Spawns a |
| `OvlFunc_964_2008ab0` | `0x02008ab0` | — | read | Slotted between the _a and _c pieces in goldensun/overlays/rom_7ed0a0/overlay.ld. Advances an entity by its per-axis deltas and spins its at |
| `OvlFunc_964_2008cd0` | `0x02008cd0` | — | read | struct Ent { unsigned char pad0[6]; unsigned short f6; unsigned int f8; unsigned int fc; unsigned int f10; }; extern unsigned char *__MapAct |
| `OvlFunc_964_2008dc8` | `0x02008dc8` | — | read | Slotted between ovl_30_a_a_c_a_a.o and the rest of the overlay. One of the "position triple on the stack" family: read slot 0`s x/y/z, offse |
| `OvlFunc_964_2008df4` | `0x02008df4` | — | read | Slotted between ovl_30_a_a_c_a_c_a.o and the rest of the overlay. One of the "position triple on the stack" family: read slot 0`s x/y/z, off |
| `OvlFunc_964_2008e20` | `0x02008e20` | — | read | OvlFunc_964_2008e20  --  0x02008e20 The whole of goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_a_c_a_c_c_a.s. Decides whether the camera should |
| `OvlFunc_964_2008ec8` | `0x02008ec8` | — | read | Slotted between ovl_30_a_a_c_a_c_c_a.o and the rest of the overlay. A proximity test on two actors: set bit 1 of the flags byte at +0x23, th |
| `OvlFunc_964_2008f4c` | `0x02008f4c` | — | read | extern unsigned int iwram_3001e40; extern void __Actor_SetAnim(unsigned char *a, int n); extern unsigned int __Random(void); extern void Ovl |
| `OvlFunc_964_2008fe8` | `0x02008fe8` | — | read | Slotted between ovl_30_a_a_c_c_a_a_a.o and the rest of the overlay. WHETHER A CALL RESULT GOES THROUGH A NAMED VARIABLE DECIDES WHICH REGIST |
| `OvlFunc_964_2009068` | `0x02009068` | — | read | One of five byte-identical copies of the per-frame integrator, one per overlay. See src/overlays/rom_7a5214/ovl_17ec_c_b.c for the full acco |
| `OvlFunc_964_20091e0` | `0x020091e0` | — | read | struct Actor { unsigned char pad00[8]; int x; int y; int z; }; struct Cfg { int f00; int f04; unsigned char pad08[0x1c]; void (*f24)(void);  |
| `OvlFunc_964_2009270` | `0x02009270` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_964_20092b0` | `0x020092b0` | — | read | OvlFunc_964_20092b0 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_a_c_c_c.s. UNPARKED. The park held `if (area == X) return scri |
| `OvlFunc_964_20092e0` | `0x020092e0` | — | read | OvlFunc_964_20092e0 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_a.s. A SCRIPT SELECTOR: pick a script pointer by area id |
| `OvlFunc_964_2009348` | `0x02009348` | — | read | OvlFunc_964_2009348 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_a.s. PREVIOUSLY PARKED AS A SCHEDULING BLOCKER, AND TH |
| `OvlFunc_964_20093b4` | `0x020093b4` | — | read | OvlFunc_964_20093b4 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_a_c_a.s. One of the "position triple on the stack" family: r |
| `OvlFunc_964_20093e0` | `0x020093e0` | — | read | Two map edits inside a cutscene. The stack-arg-pair lever in its SHARED form: 0x19 is the [sp] value for BOTH calls, so it is named once and |
| `OvlFunc_964_200970c` | `0x0200970c` | — | read | overlay in goldensun/overlays/rom_7ed0a0/overlay.ld. Two animations on actor 0x14 with a call between them, then clear bit 1 of its +0x23 by |
| `OvlFunc_964_2009a10` | `0x02009a10` | — | read | OvlFunc_964_2009a10  --  0x02009a10 The whole of goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_c_c_c_c_c.s, which held this function and no d |
| `OvlFunc_964_200a370` | `0x0200a370` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_964_200a3a0` | `0x0200a3a0` | — | read | OvlFunc_964_200a3a0  --  0x0200a3a0 OvlFunc_964_200a410  --  0x0200a410 OvlFunc_964_200a480  --  0x0200a480 OvlFunc_964_200a52c  --  0x0200a |
| `OvlFunc_964_200a410` | `0x0200a410` | — | read | OvlFunc_964_200a3a0  --  0x0200a3a0 OvlFunc_964_200a410  --  0x0200a410 OvlFunc_964_200a480  --  0x0200a480 OvlFunc_964_200a52c  --  0x0200a |
| `OvlFunc_964_200a480` | `0x0200a480` | — | read | OvlFunc_964_200a3a0  --  0x0200a3a0 OvlFunc_964_200a410  --  0x0200a410 OvlFunc_964_200a480  --  0x0200a480 OvlFunc_964_200a52c  --  0x0200a |
| `OvlFunc_964_200a52c` | `0x0200a52c` | — | read | OvlFunc_964_200a3a0  --  0x0200a3a0 OvlFunc_964_200a410  --  0x0200a410 OvlFunc_964_200a480  --  0x0200a480 OvlFunc_964_200a52c  --  0x0200a |
| `OvlFunc_965_20089dc` | `0x020089dc` | — | read | Appended after the _a piece in goldensun/overlays/rom_7ef4f4/overlay.ld. Writes a two-bit selector into bits 2-3 of the sprite flag byte at  |
| `OvlFunc_965_20089f4` | `0x020089f4` | — | read | of the same .s in goldensun/overlays/rom_7ef4f4/overlay.ld. ONE OF FOUR OPERAND-IDENTICAL COPIES, elevated together from a single source wit |
| `OvlFunc_965_2008a4c` | `0x02008a4c` | — | read | OvlFunc_965_2008a4c, the whole of goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c_c_a.s. no linker-script change was needed. Spawns a |
| `OvlFunc_965_2008ab0` | `0x02008ab0` | — | read | Slotted between the _a and _c pieces in goldensun/overlays/rom_7ef4f4/overlay.ld. Advances an entity by its per-axis deltas and spins its at |
| `OvlFunc_965_2008cf0` | `0x02008cf0` | — | read | One of five byte-identical copies of the per-frame integrator, one per overlay. See src/overlays/rom_7a5214/ovl_17ec_c_b.c for the full acco |
| `OvlFunc_965_2008f58` | `0x02008f58` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_965_2008fac` | `0x02008fac` | — | read | OvlFunc_965_2008fac extracted from goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_a_c_c_c.s. UNPARKED. The park held `if (area == X) return scri |
| `OvlFunc_965_2008fdc` | `0x02008fdc` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_965_2009030` | `0x02009030` | — | read | OvlFunc_965_2009030 -- MATCHES on the default flags (and unchanged under -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_7ef4f4/ovl_30_a_ |
| `OvlFunc_965_200a46c` | `0x0200a46c` | — | read | extern int __GetFlag(int id); extern void __Func_8010788(int a, int b, int c, int d, int e, int f); void OvlFunc_965_200a46c(void) { int m;  |
| `OvlFunc_965_200a4b0` | `0x0200a4b0` | — | read | Slotted between ovl_30_a_c_c_c_c_c_c_a.o and the rest of the overlay. Stack-arg pair named as two locals, stored before the call. The callee |
| `OvlFunc_965_200a4d0` | `0x0200a4d0` | — | read | OvlFunc_965_200a4d0 Cut out of goldensun/asm//overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_c_b.s. Moves the two guards aside once the gate flag  |
| `OvlFunc_965_200a548` | `0x0200a548` | — | read | OvlFunc_965_200a548 Cut out of goldensun/asm//overlays/rom_7ef4f4/ovl_30_a_c_c_c_c_c_c_c_c_b.s. The sibling of 200a4d0 that puts them back.  |
| `OvlFunc_965_200a5c8` | `0x0200a5c8` | — | read | extern char *iwram_3001ebc; extern int __GetFlag(int id); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __ |
| `OvlFunc_965_200a738` | `0x0200a738` | — | read | OvlFunc_965_200a738 Cut out of goldensun/asm//overlays/rom_7ef4f4/ovl_30_c_a_c_a_b.s. Picks which door routine runs from the players facing  |
| `OvlFunc_965_200a7a0` | `0x0200a7a0` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script, so the ROM layout does not mov |
| `OvlFunc_965_200a820` | `0x0200a820` | — | read | struct Actor { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; }; extern struct Actor *__MapActor_GetActor(int slot); exter |
| `OvlFunc_966_20080c4` | `0x020080c4` | — | read | Split out of that .s; the _a and _c parts stay as assembly. A three-message prompt: says the opening line, runs a check, and delivers one of |
| `OvlFunc_966_2008158` | `0x02008158` | — | read | extern unsigned char *iwram_3001ebc; extern short L1ca8[] __asm__(".L1ca8"); extern unsigned char L1cee[] __asm__(".L1cee"); extern unsigned |
| `OvlFunc_967_2008030` | `0x02008030` | — | read | fakematch FAKEMATCH -- matched by pinning a register with inline asm, not by finding the construct. Authorised as an interim measure; every  |
| `OvlFunc_967_200804c` | `0x0200804c` | — | read | GetEntrances, two-way form. This one returns a NAMED GLOBAL from its first arm rather than a local `.L` table, which is why the earlier fami |
| `OvlFunc_967_2008084` | `0x02008084` | — | read | Slotted between ovl_30_c_c_a_a.o and the rest of the overlay. A three-way script selector: area, then a flag inside the matching area. `pop  |
| `OvlFunc_967_20080c8` | `0x020080c8` | — | read | #include "gba/types.h" #include "actor.h" extern int _MSG_26e3; extern Actor *__MapActor_GetActor(int slot); extern void __Func_80b0278(int  |
| `OvlFunc_967_200815c` | `0x0200815c` | — | read | OvlFunc_967_200815c  --  0x0200815c Cut out of goldensun/asm/overlays/rom_7f21b8/ovl_30_c_c_a_c.s. One of the four Lemuria attendants. Each  |
| `OvlFunc_967_20081c8` | `0x020081c8` | — | read | OvlFunc_967_20081c8  --  0x020081c8 Cut out of goldensun/asm/overlays/rom_7f21b8/ovl_30_c_c_a_c.s. One of the four Lemuria attendants. Each  |
| `OvlFunc_967_2008234` | `0x02008234` | — | read | OvlFunc_967_2008234  --  0x02008234 Cut out of goldensun/asm/overlays/rom_7f21b8/ovl_30_c_c_a_c.s. One of the four Lemuria attendants. Each  |
| `OvlFunc_967_200829c` | `0x0200829c` | — | read | OvlFunc_967_200829c  --  0x0200829c Cut out of goldensun/asm/overlays/rom_7f21b8/ovl_30_c_c_a_c.s. One of the four Lemuria attendants. Each  |
| `OvlFunc_967_20084b0` | `0x020084b0` | — | read | Slotted between ovl_30_c_c_c_c_a.o and the rest of the overlay. A script selector on the area AND a flag: four scripts from a two-level deci |
| `OvlFunc_967_2008eec` | `0x02008eec` | — | read | The original .s held three functions and a .data section.  The data follows the THIRD function, so it travels with _c and this TU is pure te |
| `OvlFunc_968_2008058` | `0x02008058` | — | read | Slotted between ovl_30_a_a_a_c_a_a.o and the rest of the overlay. THE ARGUMENTS ARE SHUFFLED ON THE WAY IN: the ROM saves r0-r2, then calls  |
| `OvlFunc_968_2008098` | `0x02008098` | — | read | #include "gba/types.h" #include "actor.h" extern struct Actor *__CreateActor(int kind, fx32 x, fx32 y, fx32 z); extern void OvlFunc_968_2008 |
| `OvlFunc_968_20080e0` | `0x020080e0` | — | read | Slotted between the _a and _c pieces in goldensun/overlays/rom_7f2f14/overlay.ld. Advances an entity by its per-axis deltas and spins its at |
| `OvlFunc_968_200832c` | `0x0200832c` | — | read | Placed in the run in goldensun/overlays/rom_7f2f14/overlay.ld. Scans entries 8..0x41 of the actor table for the one standing on the same til |
| `OvlFunc_968_2008558` | `0x02008558` | — | read | translation unit and the linker script is untouched -- gcc regenerates the .s at the same path. Walks the object table at [iwram_3001ebc]+0x |
| `OvlFunc_968_2008594` | `0x02008594` | — | read | Overlay 968: forward an actor's turn target, masked to its low nibble. Split out of asm/overlays/rom_7f2f14/ovl_30_a_a_a_c_c.s; the neighbou |
| `OvlFunc_968_20085ac` | `0x020085ac` | — | read | Two independent reads of the same global with different masks. Both are spelled out separately rather than cached in a local: gcc reloads, w |
| `OvlFunc_968_2008754` | `0x02008754` | — | read | THE THIRD TWIN of the map-exit cutscene: install an update hook on slot 0, set its speed, walk it, clear the hook, fade out and hand off. Th |
| `OvlFunc_968_20088c8` | `0x020088c8` | — | read | OvlFunc_968_20088c8 extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_a_c_a_a.s. A proximity test on two actors: set bit 1 of the flag |
| `OvlFunc_968_200896c` | `0x0200896c` | — | read | One of five byte-identical copies of the per-frame integrator, one per overlay. See src/overlays/rom_7a5214/ovl_17ec_c_b.c for the full acco |
| `OvlFunc_968_2008b08` | `0x02008b08` | — | read | struct Actor { unsigned char pad00[8]; int x; int y; int z; }; struct Cfg { int f00; int f04; unsigned char pad08[0x1c]; void (*f24)(void);  |
| `OvlFunc_968_2008b98` | `0x02008b98` | — | read | typedef struct { int f0; int f1; int f2; int f3; int f4; int f5; short f6; short f7; int f8; int f9; int f10; } V; extern int iwram_3001e40; |
| `OvlFunc_968_2008e04` | `0x02008e04` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. GetEntrances, 6-way form: sele |
| `OvlFunc_968_2008e88` | `0x02008e88` | — | read | extern unsigned char gState[]; extern int _AREA_b5; extern int _AREA_b6; extern int _AREA_b7; extern int _AREA_b8; extern int _AREA_b9; exte |
| `OvlFunc_968_2008f38` | `0x02008f38` | — | read | OvlFunc_968_2008f38 Cut out of goldensun/asm//overlays/rom_7f2f14/ovl_30_c_a_c_a_a_c_a.s. A ten-step flashing sequence with a decreasing del |
| `OvlFunc_968_2008fbc` | `0x02008fbc` | — | read | Overlay 968: a talk sequence with a positioned speaker. Split out of asm/overlays/rom_7f2f14/ovl_30_c_a_c_a_a_c.s; the neighbouring parts st |
| `OvlFunc_968_2009048` | `0x02009048` | — | read | extern void __ClearFlag(int id); extern void __Func_8010704(int a, int b, int c, int d, int e, int f); extern void __CopyMapTiles(int a, int |
| `OvlFunc_968_20090cc` | `0x020090cc` | — | read | extern void __SetFlag(int id); extern void __Func_8010704(int a, int b, int c, int d, int e, int f); extern void __CopyMapTiles(int a, int b |
| `OvlFunc_968_2009644` | `0x02009644` | — | read | extern void *__MapActor_GetActor(int slot); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __CutsceneWait(i |
| `OvlFunc_968_20096a4` | `0x020096a4` | — | read | struct Actor { unsigned char pad0[0xa]; short fa; int fc; unsigned char pad10[2]; short f12; int f14; unsigned char pad18[0x55 - 0x18]; unsi |
| `OvlFunc_968_20098f8` | `0x020098f8` | — | read | OvlFunc_968_20098f8 Cut out of goldensun/asm//overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_b.s. Repaints two tile blocks, then walks three actor |
| `OvlFunc_968_2009a14` | `0x02009a14` | — | read | #include "gba/types.h" #include "actor.h" extern void __Func_8010704(int a, int b, int c, int d, int e, int f); void OvlFunc_968_2009a14(str |
| `OvlFunc_968_2009a50` | `0x02009a50` | — | read | Slotted between ovl_30_c_a_c_c_c_a_c_a.o and the rest of the overlay. THREE LEVERS AT ONCE, and it is worth listing which, because at 36 ins |
| `OvlFunc_968_2009f28` | `0x02009f28` | — | read | THE STACK-ARG-PAIR LEVER. This is the function that produced it, so the account is here and the other members point at it. The blocker: wher |
| `OvlFunc_968_200a26c` | `0x0200a26c` | — | read | Sibling of src/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_b.c in the same overlay -- same structure, four constants different, and a different t |
| `OvlFunc_968_200a3d4` | `0x0200a3d4` | — | read | OvlFunc_968_200a3d4 -- MATCHES on the default flags (and unchanged under -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_7f2f14/ovl_30_c_ |
| `OvlFunc_968_200aee4` | `0x0200aee4` | — | read | Two map edits behind a guard. The stack-arg-pair lever twice in one function, with `m` and `n` REASSIGNED between the calls rather than give |
| `OvlFunc_968_200af8c` | `0x0200af8c` | — | read | The first function stays in the .s; this piece is added after it in A six-way selector on the AREA ID at gState+0x1C0, using _AREA_b5 throug |
| `OvlFunc_969_2008314` | `0x02008314` | — | read | struct A { unsigned char pad00[0x18]; int f18; int f1c; unsigned char pad20[0x44]; short f64; }; extern unsigned int __Random(void); int Ovl |
| `OvlFunc_969_20083a0` | `0x020083a0` | — | read | void OvlFunc_969_20083a0(unsigned char *p) { int vx; int vy; int vz; vx = *(int *)(p + 0x44); (int *)(p + 0x08) += vx; vy = *(int *)(p + 0x4 |
| `OvlFunc_969_2008424` | `0x02008424` | — | read | OvlFunc_969_2008424, the whole of goldensun/asm/overlays/rom_7f6e64/ovl_314_a_a_c.s. so no linker-script change was needed. Byte-identical t |
| `OvlFunc_969_20084bc` | `0x020084bc` | — | read | extern unsigned char *iwram_3001ebc; extern unsigned char *__MapActor_GetActor(int slot); extern int OvlFunc_969_2008480(void *a, void *b);  |
| `OvlFunc_969_2008518` | `0x02008518` | — | read | extern void *__MapActor_GetActor(int slot); extern void __vec3_translate(unsigned int a, unsigned int b, int *c); extern int __TestCollision |
| `OvlFunc_969_20085ec` | `0x020085ec` | — | read | extern unsigned char *__MapActor_GetActor(int); extern void __CutsceneStart(void); extern void __CutsceneEnd(void); extern void __SetFlag(in |
| `OvlFunc_969_2009280` | `0x02009280` | — | read | in goldensun/overlays/rom_7f6e64/overlay.ld. UNPARKED. This was the fourth member of the pool-load-first class (batch 32), where gcc emits e |
| `OvlFunc_969_200a15c` | `0x0200a15c` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. A 32-frame sine effect actor r |
| `OvlFunc_969_200a1ac` | `0x0200a1ac` | — | read | Split out of that .s; the sibling parts stay as assembly and keep their slots in the overlay's linker script. The mirror half of the 32-fram |
| `OvlFunc_970_20080b0` | `0x020080b0` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" extern unsigned short L181c __asm__(".L181c"); extern unsigned short L14ac[] __a |
| `OvlFunc_970_2008100` | `0x02008100` | — | read | OvlFunc_970_2008100  --  0x02008100 Cut out of goldensun/asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_a_a.s. A per-frame jitter for a hovering act |
| `OvlFunc_970_20083c0` | `0x020083c0` | — | read | Overlay 970: record slot 0's height for later comparison. Split out of asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a.s. One of four near-identical  |
| `OvlFunc_970_20083dc` | `0x020083dc` | — | read | Overlay 970: record slot 1's height for later comparison. Split out of asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a.s. One of four near-identical  |
| `OvlFunc_970_20083f8` | `0x020083f8` | — | read | Overlay 970: record slot 3's height for later comparison. Split out of asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a.s. One of four near-identical  |
| `OvlFunc_970_2008414` | `0x02008414` | — | read | Overlay 970: record slot 2's height for later comparison. Split out of asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a.s. One of four near-identical  |
| `OvlFunc_970_2008f30` | `0x02008f30` | — | read | Slotted between the _a and _c pieces in goldensun/overlays/rom_7fa4ec/overlay.ld. Kicks off a DMA0 raster effect. Picks a 0x780-byte scanlin |
| `OvlFunc_970_20090d4` | `0x020090d4` | — | read | #include "gba/types.h" #include "gba/io.h" #include "dma.h" extern unsigned char *__galloc_ewram(int tag, int size); extern int __StartTask( |
| `OvlFunc_971_200853c` | `0x0200853c` | — | read | extern unsigned int gState; extern int __GetPartySize(void); int OvlFunc_971_200853c(short *out) { unsigned char *p; unsigned int g; unsigne |
| `OvlFunc_971_2009050` | `0x02009050` | — | read | Split out of that .s; the _a and _c parts stay as assembly and keep their slots in goldensun/overlays/rom_7fb4a8/overlay.ld. Stops the curre |
| `OvlFunc_971_200906c` | `0x0200906c` | — | read | OvlFunc_971_200906c  --  0x0200906c Cut out of goldensun/asm/overlays/rom_7fb4a8/ovl_30_c_a_a_a_c.s. UNPARKS src/non_matching/overlays/20090 |
| `OvlFunc_971_20091bc` | `0x020091bc` | — | read | TWO DECLARATIONS OF ONE CALLEE, AND THE CALL SITES PICK BETWEEN THEM. This is the lever, and it is new. The function calls __CloseUIBox twic |
| `OvlFunc_971_2009228` | `0x02009228` | — | read | The twin of OvlFunc_971_20091bc, differing in exactly two constants -- the message ids 0x292c/0x292d against 0x292a/0x292b.  Read src/overla |
| `OvlFunc_971_2009294` | `0x02009294` | — | read | extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f); extern void __Func_800fe9c(void); int OvlFunc_971_2009294(int n) { int |
| `OvlFunc_973_200804c` | `0x0200804c` | — | read | fakematch FAKEMATCH -- matched by pinning a register with inline asm, not by finding the construct. Authorised as an interim measure; every  |
| `OvlFunc_973_20086f8` | `0x020086f8` | — | read | Slotted between ovl_30_c_a_c_c_c_a_a.o and the rest of the overlay. INSTRUCTION-FOR-INSTRUCTION IDENTICAL to src/overlays/rom_7d5838/ovl_30_ |
| `OvlFunc_974_200807c` | `0x0200807c` | — | read | extern unsigned char gState[]; extern volatile unsigned int gKeyHeld; extern int __Func_8019da8(int a, int b, int c, int d); extern void __F |
| `OvlFunc_974_2008130` | `0x02008130` | — | read | Overlay 974: the first of the seven message-range dispatch stubs. Split out of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_a.s. See ovl_30_a_ |
| `OvlFunc_974_2008148` | `0x02008148` | — | read | Overlay 974: the second of the seven message-range dispatch stubs. Split out of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_a.s. See ovl_30_a |
| `OvlFunc_974_2008160` | `0x02008160` | — | read | Overlay 974: one of a family of seven near-identical dispatch stubs. Split out of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a.s; the _a and _ |
| `OvlFunc_974_2008180` | `0x02008180` | — | read | Overlay 974: four of the seven message-range dispatch stubs. Whole-part conversion of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_c.s -- it h |
| `OvlFunc_974_2008198` | `0x02008198` | — | read | Overlay 974: four of the seven message-range dispatch stubs. Whole-part conversion of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_c.s -- it h |
| `OvlFunc_974_20081b8` | `0x020081b8` | — | read | Overlay 974: four of the seven message-range dispatch stubs. Whole-part conversion of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_c.s -- it h |
| `OvlFunc_974_20081d8` | `0x020081d8` | — | read | Overlay 974: four of the seven message-range dispatch stubs. Whole-part conversion of asm/overlays/rom_7fcd20/ovl_30_a_c_a_c_c_a_c.s -- it h |
| `OvlFunc_974_20088c4` | `0x020088c4` | — | read | extern void __Func_801776c(int a, int b); extern int __GiveDjinni(int slot, int elem, int n); extern int __SetDjinni(int slot, int elem, int |
| `OvlFunc_974_2008bb8` | `0x02008bb8` | — | read | Stocks the four party members and recomputes their stats. __GiveItemTo IS DELIBERATELY LEFT UNDECLARED -- do not add a prototype. With one,  |
| `OvlFunc_974_2008f14` | `0x02008f14` | — | read | split was needed; the .c replaces the .s at the same stem and the three overlay scripts that reference the .o are unchanged. Stocks the four |
| `OvlFunc_974_20090a8` | `0x020090a8` | — | read | extern void __Func_8079664(int a); extern void __AddPartyMember(int a); extern void __SetMinLevel(int a, int b); extern void __CalcStats(int |
| `OvlFunc_common2_28c` | `` | — | read | (and identically in rom_7e7574). Built with COMMON2_CFLAGS -- no -mthumb-interwork, and -fcall-saved-r4 rather than the tree-wide -fcall-use |
| `OvlFunc_common2_380` | `` | — | read | Built with -fcall-saved-r4, NOT the tree-wide -fcall-used-r4: this function opens `push {r4, lr}`, which -fcall-used-r4 makes unreachable. S |
| `PrintBattleText` | `` | — | read | extern unsigned char *iwram_3001e8c; extern int BufferString(int id, int mode); extern void *CreateUIBox(int a, int b, int c, int d, int e); |
| `SetCameraTarget` | `` | — | read | SetCameraTarget  --  0x0809335c Cut out of goldensun/asm/rom_8a000/rom_93304_a_a_a_a_a.s. Points the camera at a field actor. When the secon |
| `Sprite_DeleteLayer` | `` | — | read | extern void DeleteSpriteLayer(void *layer); void Sprite_DeleteLayer(char *a, void *layer) { unsigned int i; unsigned int j; int count; int o |
| `Sprite_DeleteLayerIndex` | `` | — | read | extern void DeleteSpriteLayer(void *layer); void Sprite_DeleteLayerIndex(char *a, unsigned int i) { unsigned int j; int count; int off; void |
| `StartMenu` | `` | — | read | extern unsigned char gSleepMode; extern int StartMenu_Main(void); extern int Menu_Save(void); extern int Menu_Settings(void); extern void Fu |
| `StartMenu_AddOption` | `` | — | read | StartMenu_AddOption  --  0x080216e8 Cut out of goldensun/asm/rom_15000/rom_20198_c_c_c_a_a_c_a.s. Decompresses one menu icon out of file 0xF |
| `Task_BlitPreAnim` | `` | — | read | Two levers, both required and neither sufficient alone: - `pop {r1} / bx r1` says the function RETURNS A VALUE: r0 is still live at the epil |
| `Task_Cutscene` | `` | — | read | extern int iwram_3001ebc; extern unsigned char gDebugMode; extern volatile int gKeyPress; void Task_Cutscene(void) { char *p; p = (char *)iw |
| `TextBox` | `` | — | read | extern unsigned char *iwram_3001e8c; extern int BufferString(int id, int mode); extern void Func_801868c(int n, int a, int b, int c, int e,  |
| `UIDrawText` | `` | — | read | extern void *Func_8004970(int size); extern void Func_8017c8c(void *buf, int b, unsigned int c, unsigned int d); extern void free(void *p);  |
| `UpdatePoison` | `` | — | read | extern unsigned char gState[]; extern int _GetPartySize(void); extern void *_GetUnit(int id); extern void _ModifyHP(int id, int delta); exte |
| `UpdateRespawnMap` | `` | — | read | extern char gState[]; extern short L9e1d8[] __asm__(".L9e1d8"); void UpdateRespawnMap(void) { char *g; short *p; short a; short b; g = gStat |
| `call_via` | `` | — | read | struct Sub { unsigned char pad0[9]; unsigned char lo : 2; unsigned char sel : 2; unsigned char hi : 4; unsigned char pad_a[0x14]; short f1e; |
| `call_via` | `` | — | read | extern int Func_8000888(int, int); static inline int call_via(int (*f)(int, int), int a, int b) { register int _a __asm__("r0") = a; registe |
| `call_via` | `` | — | read | extern int Func_8000888(int, int); extern void _UpdateSprite(int *a, int *b, int *c, int d); static inline int call_via(int (*f)(int, int),  |
| `call_via` | `` | — | read | extern int Func_8000888(int, int); static inline int call_via(int (*f)(int, int), int a, int b) { register int _a __asm__("r0") = a; registe |
| `call_via_r3` | `` | — | read | extern int sin(int); extern int Func_8000888(int, int); static inline int call_via_r3(int (*f)(int, int), int a, int b) { register int (*_f) |
| `call_via_r3` | `` | — | read | struct Obj { int f0; int f4; int f8; int fc; }; struct Ent { unsigned char pad0[8]; int f8; int fc; int f10; unsigned char pad14[4]; int f18 |
| `call_via_r4` | `` | — | read | extern int iwram_3001e70; extern int iwram_3001af4; extern void *galloc_ewram(int a, int b); extern int Func_8000888(int a, int b); extern v |
| `call_via_r4` | `` | — | read | extern int Func_80008ac(int a, int b); extern int Func_8000888(int a, int b); static inline int call_via_r4(int (*f)(int, int), int a, int b |

1341 functions: 81 named, 1260 read
