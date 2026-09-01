# Name proposals by subsystem

**Generated — do not hand-edit.** Regenerate with:

    python3 tools/name_evidence.py --subsystems > docs/name-proposals.md

The source of truth is `tools/name_proposals.tsv`. `docs/names.md` holds the
EVIDENCE for every elevated function; this holds the DECISION for those that
have one, grouped the way a rename pass would actually be run — by subsystem,
so that a family sharing a body gets one name rather than one name per batch.

Style follows this tree's own conventions: `PascalCase` for a plain routine,
`Subsystem_Verb` where a module owns it (`Actor_TravelTo`, `MapActor_SetIdle`,
`UI_SellMenu`, `Anim_Judgment`), `g*` for globals. The upstream decomp's `src/`
was not read.

Read the `Basis` column before trusting a row; `docs/attribution.md` records
that the inherited annotation corpus gets mechanism right and purpose wrong
often enough to matter. Where the code does not establish an identity — which
status effect a counter belongs to, say — the name carries an `Unk<offset>`
suffix and the `Why` column says so. That is deliberate: a guess dressed as a
fact is worse than an honest placeholder.

451 elevated functions carry a real name already; **3294 still carry a `Func_`/`OvlFunc_` placeholder**, and those are the naming job.

This file proposes **130** of them (3.9%).

## Actor engine — 7 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_800c46c` | `0x0800c46c` | `Actor_InstallScript13590` | read | Actor engine | Installs the script at .L13590 on an actor. What that script does is not established here; the name should follow it, as Camera_SetTarget did for 0x135f0. | `src/field/actor_script.c` |
| `Func_800c47c` | `0x0800c47c` | `Actor_InstallScript135A8` | read | Actor engine | Installs the script at .L135a8. Same caveat as Actor_InstallScript13590. | `src/field/actor_script.c` |
| `Func_800c48c` | `0x0800c48c` | `Actor_InstallScript135C0` | read | Actor engine | Installs the script at .L135c0. Same caveat. | `src/field/actor_script.c` |
| `Func_800c49c` | `0x0800c49c` | `Actor_InstallScript135D8` | read | Actor engine | Installs the script at .L135d8. Same caveat. | `src/field/actor_script.c` |
| `Func_800d8e8` | `0x0800d8e8` | `ActorCmd_Delete` | read+family | Actor engine | Script opcode: deletes the actor and returns 0. Joins the ActorCmd_* family already named in this bank. | `src/field/actor_script.c` |
| `Func_800d924` | `0x0800d924` | `Actor_IsBlockedAt` | read+callee | Actor engine | Walks 64 actor records at iwram_3001e64 (stride 0x70), skipping empty, non-collidable (+0x59 & 1) and self, asking Func_800eba0 for radius overlap. Returns -1 on the first hit. | `src/field/actor_collision.c` |
| `Func_800d98c` | `0x0800d98c` | `Actor_FindBlockerAt` | read+callee | Actor engine | Twin of Actor_IsBlockedAt, identical but for returning the overlapping record or NULL. | `src/field/actor_collision.c` |

## Battle / status — 12 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80bf208` | `0x080bf208` | `RollStatusRecovery` | read | Battle / status | With five or fewer turns left, rolls (luck*3 - turnsLeft*5 + strength) * 0x28f against RPGRandom() & 0xffff. The reason every TickStatusCounter rolls at all. | `src/battle/status.c` |
| `Func_80bf250` | `0x080bf250` | `TickStatusCounterUnk132` | named | Battle / status | Counter at unit+0x132, companion signed byte 0x133, recovery strength 0x1e. Which status effect this is is not established by the body. | `src/battle/status.c` |
| `Func_80bf2b4` | `0x080bf2b4` | `TickStatusCounterUnk134` | named | Battle / status | Counter 0x134, companion 0x135, strength 0x14. Effect identity not established by the body. | `src/battle/status.c` |
| `Func_80bf318` | `0x080bf318` | `TickStatusCounterUnk136` | named | Battle / status | Counter 0x136, companion 0x137, strength 0x14. Effect identity not established by the body. | `src/battle/status.c` |
| `Func_80bf37c` | `0x080bf37c` | `TickStatusCounterUnk138` | named | Battle / status | Counter 0x138, no companion, strength 0x1e. Effect identity not established by the body. | `src/battle/status.c` |
| `Func_80bf3bc` | `0x080bf3bc` | `TickStatusCounterUnk139` | named | Battle / status | Counter 0x139 -- odd, so pooled rather than built with mov+lsl. Strength 0x3c. | `src/battle/status.c` |
| `Func_80bf400` | `0x080bf400` | `TickStatusCounterUnk13A` | named | Battle / status | Counter 0x13a, strength 0x46 -- the highest in the family, so the easiest effect to shake off early. | `src/battle/status.c` |
| `Func_80bf440` | `0x080bf440` | `TickStatusCounterUnk13B` | named | Battle / status | Counter 0x13b (odd, pooled), strength 0x28. | `src/battle/status.c` |
| `Func_80bf484` | `0x080bf484` | `TickStatusCounterUnk13C` | named | Battle / status | Counter 0x13c, strength 0x32. | `src/battle/status.c` |
| `Func_80bf4c4` | `0x080bf4c4` | `TickStatusCounterUnk13D` | named | Battle / status | Counter 0x13d, and the one real variant: a 3-bit count with a carry above it, brought under 8 first, decremented only when the low bits are set, failed outright above 7. | `src/battle/status.c` |
| `Func_80bf524` | `0x080bf524` | `TickStatusCounterUnk13E` | named | Battle / status | Counter 0x13e, and the only member with no recovery roll and no companion -- hence the bare push {lr} prologue. | `src/battle/status.c` |
| `Func_80bf54c` | `0x080bf54c` | `TickStatusCounterUnk13F` | named | Battle / status | Twelfth family member, in its own TU: counter 0x13f (odd, pooled), no companion, no recovery roll. | `src/battle/status.c` |

## Battle animation — 18 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80c90e4` | `0x080c90e4` | `Anim_InitContext` | read | Battle animation | Zeroes the counter and coordinate words at +0x7790..+0x779c of the animation context block. Which counters they are is not established here. | `src/battle/anim_engine.c` |
| `Func_80c9138` | `0x080c9138` | `Anim_StepCounters` | read | Battle animation | Advances the same +0x7790 counter group by one step per call. | `src/battle/anim_engine.c` |
| `Func_80c91a4` | `0x080c91a4` | `Anim_DmaWindowRegs` | read | Battle animation | Disables DMA3, then re-arms it to feed gBuffer into the window registers at 0x04000040 -- a per-scanline window effect. | `src/battle/anim_engine.c` |
| `Func_80ccbdc` | `0x080ccbdc` | `Anim_Teardown` | read | Battle animation | Stops the animation task and its blit task, releases the tag, and runs the RAM-resident cleanup. | `src/battle/anim_engine.c` |
| `Func_80cd418` | `0x080cd418` | `Anim_ApplyWindowRegs` | read | Battle animation | Copies WIN0H/WIN0V/WIN1H/WIN1V/WININ straight out of the context block at +0x77bc onward into the hardware. | `src/battle/anim_engine.c` |
| `Func_80cd488` | `0x080cd488` | `Anim_ApplyBG2Affine` | read | Battle animation | Writes the two words at context +0x77d0/+0x77d4 into the BG2 reference-point registers at 0x04000028. | `src/battle/anim_engine.c` |
| `Func_80cd4b4` | `0x080cd4b4` | `Anim_UploadPalette` | read | Battle animation | Uploads the animation's BG palette from the layer table at iwram_3001e74. | `src/battle/anim_engine.c` |
| `Func_80cd508` | `0x080cd508` | `Anim_RunRAMBlit` | read | Battle animation | Calls the RAM-resident blit routine against the context block at +0x7818. | `src/battle/anim_engine.c` |
| `Func_80cdd14` | `0x080cdd14` | `Anim_RunRegSequence` | read | Battle animation | Drives a register-animation sequence: arms the destination, then steps it frame by frame. The precise effect is not established here. | `src/battle/anim_engine.c` |
| `Func_80d6960` | `0x080d6960` | `Anim_EndWithMode1` | read | Battle animation | Calls Func_80cdb24 with mode 1 and then AnimEnd -- the tail shared by one group of animations. | `src/battle/anim_engine.c` |
| `Func_80dbb98` | `0x080dbb98` | `NullSub_80dbb98` | read | Battle animation | Empty body. | `src/battle/anim_engine.c` |
| `Func_80dbb9c` | `0x080dbb9c` | `Anim_DmaScanlineTable` | read | Battle animation | Re-arms DMA3 from the context block at +0x6980 to feed a per-scanline table. | `src/battle/anim_engine.c` |
| `Func_80df8b8` | `0x080df8b8` | `BattleActor_StepForward` | read | Battle animation | Sets a battle actor's two speed words to 0x20000 and 0x80000 and starts it travelling, then sets its animation. | `src/battle/anim_actor.c` |
| `Func_80dfddc` | `0x080dfddc` | `Anim_BlitTiles` | read | Battle animation | Copies an n by m tile block between two buffers, row-major, with the source stride carried separately. | `src/battle/anim_engine.c` |
| `Func_80e3944` | `0x080e3944` | `Anim_MoveWithDrop` | read+callee | Battle animation | Runs PhysMove between two records and then lowers the destination's y by 0x10. | `src/battle/anim_actor.c` |
| `Func_80e3994` | `0x080e3994` | `Anim_CallVia` | read | Battle animation | Calls Func_8000888 through a register-held pointer with an explicit bx, the ARM/Thumb interworking trampoline. | `src/battle/anim_engine.c` |
| `Func_80e3a14` | `0x080e3a14` | `Anim_PollSkip` | read | Battle animation | Reads gKeyHeld once the context's counter at +0x24 passes 0x7f -- the skip poll. The discarded read is in the ROM too. | `src/battle/anim_engine.c` |
| `Func_80e72e0` | `0x080e72e0` | `Anim_DmaContextBlock` | read | Battle animation | Another re-armed DMA3 out of the animation context block, same shape as Anim_DmaWindowRegs. | `src/battle/anim_engine.c` |

## Boot / system — 2 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8003e10` | `0x08003e10` | `RunRAMRoutine` | read | Boot / system | Allocates a buffer, DMAs the ARM routine Func_8001dc8 into it, calls it on the caller's argument, and frees it. The generic run-from-RAM trampoline. | `src/system/ram_exec.c` |
| `Func_800d304` | `0x0800d304` | `RunFromRAM_800a494` | read | Boot / system | The specific run-from-RAM call for Func_800a494, the same trampoline shape as RunRAMRoutine. | `src/system/ram_exec.c` |

## Compression — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80f7db4` | `0x080f7db4` | `InitDictionary` | read | Compression | Initialises the 0x400-entry dictionary at ewram_2004c00, writing each entry's index alongside a zeroed link field. | `src/compress/dictionary.c` |

## Field — 5 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_800c0c4` | `0x0800c0c4` | `NullSub_800c0c4` | read | Field | Empty body. | `src/field/field.c` |
| `Func_800c5b4` | `0x0800c5b4` | `StartFieldRender` | read | Field | Registers the two per-frame field hooks Func_800c62c and Func_800c880, opens the panel and unblanks. | `src/field/field.c` |
| `Func_800c5fc` | `0x0800c5fc` | `StopFieldRender` | read | Field | Unregisters both field hooks and restores the blend register. | `src/field/field.c` |
| `Func_800c628` | `0x0800c628` | `ReturnTrue_800c628` | read | Field | Returns 1 unconditionally. A predicate stub; nothing in the body says what it is standing in for. | `src/field/field.c` |
| `Func_800c87c` | `0x0800c87c` | `ReturnTrue_800c87c` | read | Field | Returns 1 unconditionally, the twin of ReturnTrue_800c628. | `src/field/field.c` |

## Graphics — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80f037c` | `0x080f037c` | `BuildAffineRampTable` | read | Graphics | Fills a 512-word buffer in four runs -- 32 words of 0x01ff01ff, 240 stepping by 0x00020002 from 0x00010000, 48 more of 0x01ff01ff and 192 zeroes. A packed pair of 16-bit ramps. | `src/graphics/affine.c` |

## Map rendering — 24 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_800ea60` | `0x0800ea60` | `HandleMapCursorInput` | read | Map rendering | Reads gKeyPress against the map view's slot record and moves the selection. The exact control mapping is not fully established. | `src/map/map_view.c` |
| `Func_8010704` | `0x08010704` | `Map_CopyRect` | read | Map rendering | Block-copies a w by h rectangle inside the 128-word-wide gBuffer from (sx,sy) to (dx,dy), a row at a time. | `src/map/map_view.c` |
| `Func_8011568` | `0x08011568` | `Map_UploadBG1` | read | Map rendering | Sets BG1CNT to 0x682 and DMAs 0x9600 bytes of gBuffer to the BG1 map at 0x6006a00. | `src/map/map_view.c` |
| `Func_8011590` | `0x08011590` | `Map_StartView` | read | Map rendering | Brings up the map view: installs the per-frame hook Func_801179c, uploads the background and waits out the fade. | `src/map/map_view.c` |
| `Func_801161c` | `0x0801161c` | `Map_UploadBG1Tiles` | read | Map rendering | The tile-data twin: BG1CNT 0x501, 0x8000 bytes from ewram_2038000 to 0x6008000. | `src/map/map_view.c` |
| `Func_801173c` | `0x0801173c` | `Map_LoadViewData` | read | Map rendering | Loads and LZ-decompresses file _FILE_d5 into gBuffer, then installs the view's hooks. | `src/map/map_view.c` |
| `Func_80118a8` | `0x080118a8` | `Map_ClearMarker` | read | Map rendering | Writes 0 to the halfword at +0x22 of marker record i (12-byte stride) in the table at iwram_3001e70. | `src/map/map_view.c` |
| `Func_80118c0` | `0x080118c0` | `Map_SetMarker` | read | Map rendering | The mirror of Map_ClearMarker, writing 1 into the same halfword. | `src/map/map_view.c` |
| `Func_8011984` | `0x08011984` | `Map_AddRedrawHook` | read | Map rendering | Registers Func_801179c as a per-frame hook, but only while the state byte at +0xfc is zero. | `src/map/map_view.c` |
| `Func_80119a8` | `0x080119a8` | `Map_RemoveRedrawHook` | read | Map rendering | The unregister half of Map_AddRedrawHook, gated on the same byte. | `src/map/map_view.c` |
| `Func_8011ae0` | `0x08011ae0` | `Map_AddCursorHook` | read | Map rendering | Registers Func_80119cc as a per-frame hook, unconditionally. | `src/map/map_view.c` |
| `Func_8011af0` | `0x08011af0` | `Map_RemoveCursorHook` | read | Map rendering | Unregisters Func_80119cc. | `src/map/map_view.c` |
| `Func_8011b00` | `0x08011b00` | `Map_AllocMarkerTable` | read | Map rendering | Reserves the 0xb4-byte EWRAM block under tag 0x1c and zeroes its four marker rows. | `src/map/map_view.c` |
| `Func_8011bc8` | `0x08011bc8` | `Map_StopTask` | read | Map rendering | Stops Func_8011bf4 and releases the tag-0x1c block. | `src/map/map_view.c` |
| `Func_8011be0` | `0x08011be0` | `Map_StartTask` | read | Map rendering | Starts Func_8011bf4 at priority 200 << 4. | `src/map/map_view.c` |
| `Func_8011ce0` | `0x08011ce0` | `GetTileHeightFixed` | read | Map rendering | Sign-extends the byte at the pointer and shifts it left 19 -- a tile height into the engine's fixed-point scale. | `src/map/terrain.c` |
| `Func_8011f3c` | `0x08011f3c` | `GetTileHeightFixedAt` | read | Map rendering | The same conversion taking the address as an integer rather than a pointer. One of three spellings of one operation in this bank. | `src/map/terrain.c` |
| `Func_8011f48` | `0x08011f48` | `GetTileHeightFixedPtr` | read | Map rendering | The third spelling, loading through a named local first. Kept distinct because the ROM has three separate routines. | `src/map/terrain.c` |
| `Func_8012038` | `0x08012038` | `GetLayerTileAt` | read | Map rendering | Reads the tile byte for a layer at a world position, indexing gBuffer through the layer's 0x30-byte block at +0x130. | `src/map/terrain.c` |
| `Func_80120b4` | `0x080120b4` | `GetCollisionAt` | read | Map rendering | Returns the top two bits of byte 1 of the 4-byte gBuffer cell covering (x/16, y/16) -- the cell's collision class. | `src/map/terrain.c` |
| `Func_8012204` | `0x08012204` | `GetTerrainAt` | read | Map rendering | Resolves a 3D position to a terrain code: a 64x64 cell index into the map at 0x6005000, then a sub-cell nibble out of ewram_202c800. | `src/map/terrain.c` |
| `Func_80122ac` | `0x080122ac` | `IsWalkableTerrain` | read+callee | Map rendering | Returns 0 for terrain codes 5 through 12 and -1 otherwise -- the walkability test over GetTerrainAt. | `src/map/terrain.c` |
| `Func_80122c8` | `0x080122c8` | `GetGroundHeightAt` | read+callee | Map rendering | Combines the terrain code with the height byte from ewram_2020000 for the 32x32 coarse cell, writing the adjusted ground height out. | `src/map/terrain.c` |
| `Func_8012330` | `0x08012330` | `SetCameraTarget` | read | Map rendering | Writes the three camera words at +4/+8/+0xc of the view record, skipping any argument that is negative. | `src/map/camera.c` |

## Math — 3 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80f40b4` | `0x080f40b4` | `DivS16` | read | Math | Signed 16-bit divide built out of sign-extended shifts, with the operands pinned to r0 and r3. | `src/math/fixed.c` |
| `Func_80f40d0` | `0x080f40d0` | `DivFixed8` | read | Math | Returns (a << 8) / b -- an 8.8 fixed-point divide. | `src/math/fixed.c` |
| `Func_80f40e8` | `0x080f40e8` | `ReciprocalFixed16` | read | Math | Returns 0x10000 / x, the 16.16 reciprocal. | `src/math/fixed.c` |

## Menus — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80a1814` | `0x080a1814` | `Menu_CreatePanel` | read | Menus | Builds a 13x5 window at g+0x10, attaches a text layer, stores the handle at g+0x14, writes both OBJ priorities and the 0xff/0 no-selection sentinel. Returns the window. | `src/menu/panel.c` |

## Overlay 974 / debug — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `OvlFunc_974_20088c4` | `0x020088c4` | `DebugGiveAllDjinn` | read | Overlay 974 / debug | 53 calls: GiveDjinni then SetDjinni for elements 0-3 across four party slots, then CalcStats on all four. A test fixture, not reachable play. | `src/overlays/ovl_974/debug_djinn.c` |

## Party — 4 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8077f40` | `0x08077f40` | `InitStartingParty` | read | Party | Sets flag 0x20 then runs Func_8079ae8 and CalcStats over units 0, 1 and 5 -- the three starting members recomputed together. | `src/party/party.c` |
| `Func_8078228` | `0x08078228` | `NullSub_8078228` | read | Party | Empty body. Named as a null sub so the ROM slot is accounted for rather than silently dropped. | `src/party/party.c` |
| `Func_8078ecc` | `0x08078ecc` | `GetPartyLeaderSlot` | read | Party | A pure tail call to Func_80792c4. The name follows that callee and should be revisited when it is named; nothing in this body claims more. | `src/party/party.c` |
| `Func_80796c4` | `0x080796c4` | `GetPartyMemberIds` | read | Party | Fills the caller's buffer with one id per living party member, taken from gState+0x1f8, and returns the count. Null buffer returns zero. | `src/party/party.c` |

## Party / equipment — 3 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_807845c` | `0x0807845c` | `CanUseItem` | read | Party / equipment | Returns 1 for anything Func_8078480 says is not class-restricted, otherwise defers to CanEquipItem. The permissive wrapper around the class-mask test. | `src/party/item.c` |
| `Func_807882c` | `0x0807882c` | `GetEquippedItemInfo` | read | Party / equipment | The same scan as GetEquippedItem but returns the ItemInfo record rather than the slot index, and takes the unit directly instead of looking it up. | `src/party/item.c` |
| `GetEquippedItem` | `` | `GetEquippedItem` | named | Party / equipment | Keeps the ROM's own name; body agrees -- scans the 15 slots at unit+0xd8 for the first equipped entry (0x200) whose info record carries the requested kind, returns the slot index or -1. | `src/party/item.c` |

## Party / inventory — 3 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8077330` | `0x08077330` | `GetItemContainer` | read | Party / inventory | Picks the item container: argument zero gives the party's shared block at ewram_200024c, anything else gives unit 0x83's. Func_807a550 walks the result at +8. | `src/party/inventory.c` |
| `Func_8078500` | `0x08078500` | `PartyHasInventorySpace` | read | Party / inventory | Returns 1 if the lead unit or any listed member has a free slot -- FindEmptyInventorySlot returning anything other than 0xf. | `src/party/inventory.c` |
| `Func_8078948` | `0x08078948` | `RemoveInventoryItem` | read | Party / inventory | Reads the item in the slot, delegates the removal to Func_80788c4, and on success notifies Func_8078ad0 and the UI hook _Func_8091858. | `src/party/inventory.c` |

## Party / save — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80773f4` | `0x080773f4` | `CopyRecordBytes` | read | Party / save | Copies n bytes between two buffers in whichever direction the fourth argument selects -- the save/load direction switch, one routine serving both. | `src/party/record.c` |

## Party / stats — 9 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80782a0` | `0x080782a0` | `SetUnitHP` | read | Party / stats | Clamps the argument to 0..maxHP (unit+0x34), stores it as current HP (unit+0x38), then recomputes the 0..0x4000 bar fraction at unit+0x14. | `src/party/stats.c` |
| `Func_8078320` | `0x08078320` | `SetUnitPP` | read | Party / stats | The PP twin of SetUnitHP: clamps to 0..maxPP (unit+0x36), stores current PP (unit+0x3a), and refreshes the same bar fraction from HP. | `src/party/stats.c` |
| `Func_8079754` | `0x08079754` | `AddPsynergyPoints` | read | Party / stats | Adds a delta to the signed byte at gState+0x11c and clamps it to 0..0x1c. Which counter this is is not established by the body. | `src/party/stats.c` |
| `Func_80797ec` | `0x080797ec` | `GetClassStatEntry` | read | Party / stats | Indexes .L88db8 as a four-wide table: row times four plus column. A plain accessor; the table's meaning is not established here. | `src/party/stats.c` |
| `Func_80797fc` | `0x080797fc` | `GetBaseStatSpread` | read | Party / stats | Fills four output stats, each ten times a packed byte. Ids above 7 read the enemy row via GetEnemyInfo and .L88e38 at stride 24; ids 0-7 read the unit's own block at +0x24. | `src/party/stats.c` |
| `Func_807987c` | `0x0807987c` | `GetBaseStat` | read | Party / stats | One stat of the four GetBaseStatSpread produces, divided back down by ten. Out-of-range indices return zero. | `src/party/stats.c` |
| `Func_80798b4` | `0x080798b4` | `GetEnemyStatRowWord` | read | Party / stats | Looks up the unit's enemy row in .L88e38 (24-byte stride, clamped at 0x2b) and returns its first word. | `src/party/stats.c` |
| `Func_80798e0` | `0x080798e0` | `GetUnitStatSpread` | read | Party / stats | Builds the full stat spread for a unit, taking the enemy table branch when unit+0x129 is zero and the class branch otherwise. | `src/party/stats.c` |
| `Func_8079ae8` | `0x08079ae8` | `RefreshUnitClass` | read | Party / stats | Recomputes the unit's class byte at +0x129 from its id and Djinn block, then refreshes the derived stats. Paired with CalcStats at every call site. | `src/party/stats.c` |

## Save / flash — 3 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8005810` | `0x08005810` | `PickFreeSaveSlot` | read | Save / flash | Scans the sixteen slot-status bytes at iwram_3001f1c for zeros, and returns a random free index, the single free one, or 0x10 for none. | `src/save/save.c` |
| `Func_80058ac` | `0x080058ac` | `VerifySaveSector` | read | Save / flash | Reads one 0x1000-byte flash sector into the scratch block, copies its sixteen-byte header out, and returns the recomputed checksum minus the stored one -- zero meaning intact. | `src/save/save.c` |
| `Func_8005a78` | `0x08005a78` | `LoadSaveData` | read | Save / flash | Resolves the current slot, verifies its sector, then DMA-copies 0xff0 bytes of save payload to the caller. Returns 1 when the slot index is out of range. | `src/save/save.c` |

## Save / sound — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8005b64` | `0x08005b64` | `SetupSoundChannelEntry` | read | Save / sound | Clears a sixteen-byte entry, seeds it from the eight-byte template at .L79b8, and hands it to Func_8005868 for the given channel. SUBSYSTEM UNCERTAIN -- it sits in the flash-save file but the record it fills was read as a sound-channel struct at elevation time. Re-check before renaming. | `src/save/save.c` |

## Screen effects — 8 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80f2b6c` | `0x080f2b6c` | `NullSub_80f2b6c` | read | Screen effects | Empty body. | `src/effects/effects.c` |
| `Func_80f2eb8` | `0x080f2eb8` | `NullSub_80f2eb8` | read | Screen effects | Empty body. | `src/effects/effects.c` |
| `Func_80f37ec` | `0x080f37ec` | `StopEffectTask` | read | Screen effects | Stops Func_80f2f10 and releases the tag-0x20 block. | `src/effects/effects.c` |
| `Func_80f3804` | `0x080f3804` | `Effect_RenderToBuffer` | read | Screen effects | Runs the effect kernel over the module block, passing the block and the halfway point at +0x1000 as the two working buffers. | `src/effects/effects.c` |
| `Func_80f3824` | `0x080f3824` | `Effect_RenderToBufferSmall` | read | Screen effects | The same call with a 0x400-byte split instead of 0x1000. | `src/effects/effects.c` |
| `Func_80f3844` | `0x080f3844` | `Effect_SetMode` | read | Screen effects | Writes the mode halfword at the head of the module block, if the block exists. | `src/effects/effects.c` |
| `Func_80f3898` | `0x080f3898` | `ClampBrightness` | read | Screen effects | Clamps to 0..0x1f, the GBA blend-coefficient range. | `src/effects/effects.c` |
| `Func_80f38ac` | `0x080f38ac` | `ClampScale` | read | Screen effects | Clamps to at most 0x7c00, leaving negatives alone. | `src/effects/effects.c` |

## Sound — 7 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80060e8` | `0x080060e8` | `UpdateSoundStateChecksum` | read | Sound | Walks the block hanging off ewram_2002240+0x28 accumulating a sum and writing it back into the state record. SUBSYSTEM UNCERTAIN in the same way as Func_8005b64. | `src/sound/sound_state.c` |
| `Func_80f954c` | `0x080f954c` | `GetMusicFadeState` | read | Sound | Returns the fade-state byte at ewram_2003000, the flag WaitMusicFade spins on. | `src/sound/music.c` |
| `Func_80f9570` | `0x080f9570` | `SetSoundFlagByte` | read | Sound | Writes ewram_2003040, XORing rather than assigning when bit 7 of the argument is set. | `src/sound/music.c` |
| `Func_80f9594` | `0x080f9594` | `GetSoundState303C` | read | Sound | Returns the byte at ewram_200303c. What it tracks is not established here. | `src/sound/music.c` |
| `Func_80f95a0` | `0x080f95a0` | `WaitMusicFade` | read+callee | Sound | Spins a frame at a time until the fade-state byte clears, giving up after 300 frames. | `src/sound/music.c` |
| `Func_80fa260` | `0x080fa260` | `NullSub_80fa260` | read | Sound | Empty body. | `src/sound/music.c` |
| `Func_80fb790` | `0x080fb790` | `NullSub_80fb790` | read | Sound | Empty body. | `src/sound/music.c` |

## Sprites — 7 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_800be20` | `0x0800be20` | `GetSpriteTileCount` | read | Sprites | Sums the tile counts of a sprite's parts from its info block, bounded by the part count at info[5]. | `src/sprite/sprite.c` |
| `Func_800be70` | `0x0800be70` | `ScrambleSpriteTiles` | read | Sprites | Permutes one step of a sprite's VRAM tiles through the .L1314c order table -- the dissolve effect's per-step worker. | `src/sprite/sprite.c` |
| `Func_800befc` | `0x0800befc` | `RunSpriteScramble` | read+callee | Sprites | Drives ScrambleSpriteTiles four steps a frame across all 0x80 steps, waiting a frame between groups. | `src/sprite/sprite.c` |
| `Func_800c548` | `0x0800c548` | `Sprite_SetSelectField` | read | Sprites | Writes the two-bit selector inside the sprite byte at +5 as a bitfield. Which selector it is is not established; this is the batch-71 narrow-constant specimen. | `src/sprite/sprite.c` |
| `Func_800c570` | `0x0800c570` | `Sprite_SetFlag1D` | read | Sprites | Writes the single-bit field inside the sprite byte at +0x1d. Same caveat as Sprite_SetSelectField. | `src/sprite/sprite.c` |
| `Func_8012d70` | `0x08012d70` | `SetLayerGroupAnim` | read | Sprites | Walks a layer group's ten sprite entries and points each at the requested animation in the group's sprite info. | `src/sprite/sprite.c` |
| `Func_8012de8` | `0x08012de8` | `InitLayerGroupSprites` | read | Sprites | Binds a layer group's ten sprite entries to a sprite resource via InitSpriteLayer. | `src/sprite/sprite.c` |

## Town UI — 9 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80b010c` | `0x080b010c` | `StartShopSession` | read | Town UI | Reserves the module's 0xa70-byte IWRAM block, primes the field layer, allocates and uploads the sprite sheet, and starts Func_80b00f4 as the module's task. | `src/menu/shop.c` |
| `Func_80b06ec` | `0x080b06ec` | `UI_DrawDigit` | read+callee | Town UI | Draws one decimal digit glyph into a tilemap buffer: 32-byte source rows at .Lb3d40 + digit*32, destination slot from .Lb413c[slot], four 2x2 writes at +0/+1/+30/+31 advancing by four, a zero byte ending the glyph early. Its caller Func_80b0744 passes value % 10, which is what establishes `digit` -- an earlier proposal of UI_DrawIconRow was made without that caller in view and was wrong. | `src/menu/number_sprite.c` |
| `Func_80b0744` | `0x080b0744` | `UI_BuildNumberSprite` | read | Town UI | Allocates a 0x400-byte EWRAM tile buffer, seeds it from .Lb3e80, draws the value one digit at a time with UI_DrawDigit dividing by ten, uploads it as sprite graphics and frees the buffer. | `src/menu/number_sprite.c` |
| `Func_80b0840` | `0x080b0840` | `UI_LoadPanelGfx` | read | Town UI | Two DMA transfers from the module's fifth buffer into the field layer's tile area at +0x236, then the two panel hooks _Func_8091200 and _Func_8091254. | `src/menu/shop.c` |
| `Func_80b10cc` | `0x080b10cc` | `UI_ShowShopMessage` | read | Town UI | Reads the active record off iwram_3001f2c and, when it is non-null, opens message 0xc8a against it and lays out the window with the party's coin total. | `src/menu/shop.c` |
| `Func_80b19cc` | `0x080b19cc` | `GetItemSellPrice` | read | Town UI | The item's base price, zeroed when info[3] bit 3 marks it unsellable, halved for one item class and otherwise three quarters -- the shop's buy-back formula. | `src/menu/shop.c` |
| `Func_80b26cc` | `0x080b26cc` | `GrantShopStock` | read | Town UI | Flag-gated once per shop id: sets the visited flag, walks that shop's 0x42-byte row in .Lb41ac and hands each listed entry to _Func_8078ad0. | `src/menu/shop.c` |
| `Func_80b27b0` | `0x080b27b0` | `UnitHasCondition` | read | Town UI | Answers one of several per-unit condition questions selected by the second argument -- downed at +0x38, the signed byte at +0x131, the byte at +0x140 and so on. Which condition each index means is not established by this body. | `src/menu/sanctum.c` |
| `Func_80b2884` | `0x080b2884` | `GetSanctumMessageId` | read | Town UI | Offsets a base message id by the gap between _MSG_d24 and one of _MSG_d2e/_MSG_d38/_MSG_d42, chosen by the state byte at iwram_3001f2c+0x3aa. | `src/menu/sanctum.c` |

