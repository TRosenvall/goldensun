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

Covering **42 of 1341** elevated functions (3%).

## Actor engine — 2 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_800d924` | `0x0800d924` | `Actor_IsBlockedAt` | read+callee | Actor engine | Walks 64 actor records at iwram_3001e64 (stride 0x70), skipping empty, non-collidable (+0x59 & 1) and self, asking Func_800eba0 for radius overlap. Returns -1 on the first hit. | `src/field/actor_collision.c` |
| `Func_800d98c` | `0x0800d98c` | `Actor_FindBlockerAt` | read+callee | Actor engine | Twin of Actor_IsBlockedAt, identical but for returning the overlapping record or NULL. | `src/field/actor_collision.c` |

## Battle / status — 11 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
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

## Boot / system — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8003e10` | `0x08003e10` | `RunRAMRoutine` | read | Boot / system | Allocates a buffer, DMAs the ARM routine Func_8001dc8 into it, calls it on the caller's argument, and frees it. The generic run-from-RAM trampoline. | `src/system/ram_exec.c` |

## Compression — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80f7db4` | `0x080f7db4` | `InitDictionary` | read | Compression | Initialises the 0x400-entry dictionary at ewram_2004c00, writing each entry's index alongside a zeroed link field. | `src/compress/dictionary.c` |

## Graphics — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80f037c` | `0x080f037c` | `BuildAffineRampTable` | read | Graphics | Fills a 512-word buffer in four runs -- 32 words of 0x01ff01ff, 240 stepping by 0x00020002 from 0x00010000, 48 more of 0x01ff01ff and 192 zeroes. A packed pair of 16-bit ramps. | `src/graphics/affine.c` |

## Menus — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80a1814` | `0x080a1814` | `Menu_CreatePanel` | read | Menus | Builds a 13x5 window at g+0x10, attaches a text layer, stores the handle at g+0x14, writes both OBJ priorities and the 0xff/0 no-selection sentinel. Returns the window. | `src/menu/panel.c` |

## Overlay 974 / debug — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `OvlFunc_974_20088c4` | `0x020088c4` | `DebugGiveAllDjinn` | read | Overlay 974 / debug | 53 calls: GiveDjinni then SetDjinni for elements 0-3 across four party slots, then CalcStats on all four. A test fixture, not reachable play. | `src/overlays/ovl_974/debug_djinn.c` |

## Party / equipment — 2 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_807882c` | `0x0807882c` | `GetEquippedItemInfo` | read | Party / equipment | The same scan as GetEquippedItem but returns the ItemInfo record rather than the slot index, and takes the unit directly instead of looking it up. | `src/party/item.c` |
| `GetEquippedItem` | `` | `GetEquippedItem` | named | Party / equipment | Keeps the ROM's own name; body agrees -- scans the 15 slots at unit+0xd8 for the first equipped entry (0x200) whose info record carries the requested kind, returns the slot index or -1. | `src/party/item.c` |

## Party / inventory — 2 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8078500` | `0x08078500` | `PartyHasInventorySpace` | read | Party / inventory | Returns 1 if the lead unit or any listed member has a free slot -- FindEmptyInventorySlot returning anything other than 0xf. | `src/party/inventory.c` |
| `Func_8078948` | `0x08078948` | `RemoveInventoryItem` | read | Party / inventory | Reads the item in the slot, delegates the removal to Func_80788c4, and on success notifies Func_8078ad0 and the UI hook _Func_8091858. | `src/party/inventory.c` |

## Party / stats — 6 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80782a0` | `0x080782a0` | `SetUnitHP` | read | Party / stats | Clamps the argument to 0..maxHP (unit+0x34), stores it as current HP (unit+0x38), then recomputes the 0..0x4000 bar fraction at unit+0x14. | `src/party/stats.c` |
| `Func_8078320` | `0x08078320` | `SetUnitPP` | read | Party / stats | The PP twin of SetUnitHP: clamps to 0..maxPP (unit+0x36), stores current PP (unit+0x3a), and refreshes the same bar fraction from HP. | `src/party/stats.c` |
| `Func_8079754` | `0x08079754` | `AddPsynergyPoints` | read | Party / stats | Adds a delta to the signed byte at gState+0x11c and clamps it to 0..0x1c. Which counter this is is not established by the body. | `src/party/stats.c` |
| `Func_80797fc` | `0x080797fc` | `GetBaseStatSpread` | read | Party / stats | Fills four output stats, each ten times a packed byte. Ids above 7 read the enemy row via GetEnemyInfo and .L88e38 at stride 24; ids 0-7 read the unit's own block at +0x24. | `src/party/stats.c` |
| `Func_80798b4` | `0x080798b4` | `GetEnemyStatRowWord` | read | Party / stats | Looks up the unit's enemy row in .L88e38 (24-byte stride, clamped at 0x2b) and returns its first word. | `src/party/stats.c` |
| `Func_80798e0` | `0x080798e0` | `GetUnitStatSpread` | read | Party / stats | Builds the full stat spread for a unit, taking the enemy table branch when unit+0x129 is zero and the class branch otherwise. | `src/party/stats.c` |

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

## Sound — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80060e8` | `0x080060e8` | `UpdateSoundStateChecksum` | read | Sound | Walks the block hanging off ewram_2002240+0x28 accumulating a sum and writing it back into the state record. SUBSYSTEM UNCERTAIN in the same way as Func_8005b64. | `src/sound/sound_state.c` |

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

