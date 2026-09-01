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

This file proposes **507** of them (15.4%).

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

## Battle — 81 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80b5ad4` | `0x080b5ad4` | `Battle_DmaViaR3` | read | Battle | A DMA3 transfer issued through a register-pinned trampoline, the interworking call shape this bank uses in several places. | `src/battle/battle_main.c` |
| `Func_80b5b14` | `0x080b5b14` | `NullSub_80b5b14` | read | Battle | Empty body. | `src/battle/battle_main.c` |
| `Func_80b5e10` | `0x080b5e10` | `NullSub_80b5e10` | read | Battle | Empty body. | `src/battle/battle_main.c` |
| `Func_80b606c` | `0x080b606c` | `Battle_FormatName` | read | Battle | Builds an eight-byte name string into a stack buffer, padding the unused tail. | `src/battle/battle_text.c` |
| `Func_80b60a0` | `0x080b60a0` | `Battle_WaitFade` | read | Battle | Spins a frame at a time on the two EWRAM fade flags and the halfword at iwram_3001f64. | `src/battle/battle_main.c` |
| `Func_80b6378` | `0x080b6378` | `Battle_GetTargetList` | read+callee | Battle | Collects the eligible target ids for the current action into an eight-entry buffer and returns the count. | `src/battle/battle_target.c` |
| `Func_80b63b0` | `0x080b63b0` | `Battle_ClearOverlayBuf` | read | Battle | Calls the RAM-resident clear over the 0x10-byte overlay block at ewram_2002224. | `src/battle/battle_main.c` |
| `Func_80b6c90` | `0x080b6c90` | `Battle_CreateEnemyOverlays` | read+family | Battle | Builds a 0x1c-byte descriptor for group 3 and creates the sprite overlays with the enemy-side flag. | `src/battle/battle_sprite.c` |
| `Func_80b6cb0` | `0x080b6cb0` | `Battle_CreatePartyOverlays` | read+family | Battle | The party-side twin of Battle_CreateEnemyOverlays, identical but for the trailing 0. | `src/battle/battle_sprite.c` |
| `Func_80b6cd0` | `0x080b6cd0` | `GetBattleActorSprite` | read | Battle | Returns the sprite pointer at +0x14 of the current battle actor. | `src/battle/battle_sprite.c` |
| `Func_80b6cdc` | `0x080b6cdc` | `Battle_CountReadyUnits` | read | Battle | Walks the battle state's unit records counting those whose ready flag is set. | `src/battle/battle_main.c` |
| `Func_80b6e30` | `0x080b6e30` | `Battle_PreloadUnitGFX` | read | Battle | Preloads the sprite graphics for one battle slot from the state block's resource list. | `src/battle/battle_sprite.c` |
| `Func_80b7410` | `0x080b7410` | `GetBattlePositionOffset` | read | Battle | Reads a signed (x, y) pair out of the .Lc2a62 table for a formation index. | `src/battle/battle_layout.c` |
| `Func_80b7514` | `0x080b7514` | `Battle_CountEnemies` | read | Battle | Counts the occupied enemy slots, ids 0x80 through 0x85. | `src/battle/battle_main.c` |
| `Func_80b770c` | `0x080b770c` | `Battle_FindSlotFor` | read | Battle | Searches a halfword list for a unit id, biasing enemy ids by 0x78 first. | `src/battle/battle_target.c` |
| `Func_80b7b30` | `0x080b7b30` | `Battle_DeleteActorSprites` | read+callee | Battle | Deletes every sprite hanging off the current battle actor, resolving each through GetActorSpriteByKind. | `src/battle/battle_sprite.c` |
| `Func_80b7e04` | `0x080b7e04` | `Battle_ClearSpriteRefs` | read | Battle | Zeroes the four sprite reference words at +0x28 of a record, if the record exists. | `src/battle/battle_sprite.c` |
| `Func_80b7e24` | `0x080b7e24` | `Battle_ReleaseActorSprites` | read+callee | Battle | Switches on the actor kind nibble at +0x54 and clears the sprite references belonging to that kind. | `src/battle/battle_sprite.c` |
| `Func_80b7e60` | `0x080b7e60` | `Battle_PrepareUnitSprite` | read+callee | Battle | Preloads a slot's graphics and returns its battle actor ready for use. | `src/battle/battle_sprite.c` |
| `Func_80b7e7c` | `0x080b7e7c` | `Battle_ReleaseAllSprites` | read | Battle | Battle teardown: releases every combatant's sprite. Corrected in batch 01 -- it does not take the arguments its inherited annotation claimed. | `src/battle/battle_sprite.c` |
| `Func_80b7eb4` | `0x080b7eb4` | `GetBattleUnitRecord` | read | Battle | Indexes the battle state's 0x2c-byte unit records from +0x80. | `src/battle/battle_main.c` |
| `Func_80b7ed8` | `0x080b7ed8` | `Battle_SetupViewMatrix` | read | Battle | Builds the battle view matrix from the look vectors at .Lc2a7c, choosing a variant on a flag. | `src/battle/battle_camera.c` |
| `Func_80b7f20` | `0x080b7f20` | `Battle_MoveActorTo` | read+callee | Battle | Runs PhysMove on a unit's actor toward a destination in the view's frame. | `src/battle/battle_camera.c` |
| `Func_80b7f70` | `0x080b7f70` | `GetActorSpriteByKind` | read | Battle | Returns one of an actor's sprite pointers, selected by the kind nibble at +0x54 and an index. | `src/battle/battle_sprite.c` |
| `Func_80b7f9c` | `0x080b7f9c` | `Battle_SetViewAngles` | read | Battle | Writes the view record's three vectors plus its pitch and yaw. | `src/battle/battle_camera.c` |
| `Func_80b8064` | `0x080b8064` | `BattleActor_StepBack` | read+family | Battle | Stops an actor, sets its two speed words and walks it backward, then sets its animation. One of five near-identical step routines differing only in the speeds. | `src/battle/anim_actor.c` |
| `Func_80b80b8` | `0x080b80b8` | `Battle_LerpActorPos` | read | Battle | Interpolates an actor record's position words toward a target over the stored step count. | `src/battle/battle_camera.c` |
| `Func_80b8144` | `0x080b8144` | `BattleActor_ResetMotion` | read | Battle | Resets an actor's speeds, its 0x48 angle word and the byte at +0x5a to their defaults. | `src/battle/anim_actor.c` |
| `Func_80b8178` | `0x080b8178` | `BattleActor_StepForwardSlow` | read+family | Battle | The slow forward member of the step family: speeds 0x10000 and 0x40000. | `src/battle/anim_actor.c` |
| `Func_80b81c8` | `0x080b81c8` | `BattleActor_StepBackSlow` | read+family | Battle | The slow backward member of the step family. | `src/battle/anim_actor.c` |
| `Func_80b8394` | `0x080b8394` | `BattleActor_SetIdleAnim` | read | Battle | Stops the current battle actor and puts it in animation 2. | `src/battle/anim_actor.c` |
| `Func_80b83b0` | `0x080b83b0` | `NullSub_80b83b0` | read | Battle | Empty body. | `src/battle/battle_main.c` |
| `Func_80b83b4` | `0x080b83b4` | `Battle_LerpActorPosB` | read | Battle | A second interpolator over a different field set of the same record. | `src/battle/battle_camera.c` |
| `Func_80b8418` | `0x080b8418` | `BattleActor_PlayHitFlash` | read | Battle | Adds a sprite layer to the current actor, runs its animation and waits the frames out. | `src/battle/anim_actor.c` |
| `Func_80b845c` | `0x080b845c` | `Battle_MoveActorToB` | read+family | Battle | The variant of Battle_MoveActorTo that also runs Func_80b8530 on arrival. | `src/battle/battle_camera.c` |
| `Func_80b86ec` | `0x080b86ec` | `Battle_PollCameraKeys` | read | Battle | Reads gKeyHeld against the view record and nudges the camera. One of two identical copies in this bank. | `src/battle/battle_camera.c` |
| `Func_80b8808` | `0x080b8808` | `IsEnemyUnitId` | read | Battle | Classifies a unit id: party ids 0-7 give 0, enemy ids 0x80-0x85 give their index. | `src/battle/battle_main.c` |
| `Func_80b8888` | `0x080b8888` | `Battle_ShowUnitMessage` | read+callee | Battle | Opens the battle message for a unit, routing party and enemy ids through different text hooks. | `src/battle/battle_text.c` |
| `Func_80b8b48` | `0x080b8b48` | `Battle_QueueAnimCmd` | read | Battle | Fills an animation command record from a queued battle command. | `src/battle/battle_main.c` |
| `Func_80b8ec4` | `0x080b8ec4` | `Battle_PlayDissolve` | read+callee | Battle | Sets the unit's sprite animation and runs the tile-scramble dissolve over it. | `src/battle/battle_sprite.c` |
| `Func_80b8f08` | `0x080b8f08` | `Battle_PickRandomTarget` | read+callee | Battle | Builds the candidate list for a command and picks one at random. | `src/battle/battle_target.c` |
| `Func_80b98b4` | `0x080b98b4` | `Battle_FadePaletteRow` | read+family | Battle | Adds a signed delta to each channel of one palette row, clamping. Byte-for-byte the same routine as Menu_FadePaletteRow in rom_a1000. | `src/battle/battle_draw.c` |
| `Func_80b9a44` | `0x080b9a44` | `GetBattleSlotOffset` | read | Battle | Maps a unit id to its offset in the battle state block, taking a different base for enemy ids. | `src/battle/battle_main.c` |
| `Func_80b9a70` | `0x080b9a70` | `Battle_FindSlotByKey` | read | Battle | Scans the state block's slot table for one matching the given key. | `src/battle/battle_main.c` |
| `Func_80b9acc` | `0x080b9acc` | `Battle_PollCameraKeysB` | read+family | Battle | The second copy of Battle_PollCameraKeys; the ROM has both. | `src/battle/battle_camera.c` |
| `Func_80b9b2c` | `0x080b9b2c` | `NullSub_80b9b2c` | read | Battle | Empty body. | `src/battle/battle_main.c` |
| `Func_80ba27c` | `0x080ba27c` | `Battle_ShowResultText` | read+callee | Battle | Runs the end-of-action text sequence: opens the line, waits for the prompt, then the follow-up. | `src/battle/battle_text.c` |
| `Func_80bace8` | `0x080bace8` | `Battle_SetSpriteFlags` | read | Battle | Copies the flag bytes from a command record into the actor's sprite record. | `src/battle/battle_sprite.c` |
| `Func_80bb8d8` | `0x080bb8d8` | `Battle_SetEndFlag` | read | Battle | Writes 1 to the third word of the block at iwram_3001ee4. | `src/battle/battle_main.c` |
| `Func_80bb8e8` | `0x080bb8e8` | `Battle_KillUnit` | read+callee | Battle | Zeroes a unit's HP, runs its death handling and deletes its battle actor. | `src/battle/battle_main.c` |
| `Func_80bb928` | `0x080bb928` | `Battle_MarkUnitActed` | read | Battle | Sets bit 0 of the word at +0x16c of a unit record. | `src/battle/battle_main.c` |
| `Func_80bbabc` | `0x080bbabc` | `Battle_CountQueueEntries` | read | Battle | Counts the populated entries of one of the battle state's queues, indexed off the caller's offset. | `src/battle/battle_main.c` |
| `Func_80bbae8` | `0x080bbae8` | `IsSpecialMove` | read | Battle | A switch listing specific move ids (0x1f, 0x20, 0x3c, 0x45 and others) that take a different code path. What they have in common is not established here. | `src/battle/battle_move.c` |
| `Func_80bd3c8` | `0x080bd3c8` | `MoveTargetsAll` | read+callee | Battle | Returns true for move 0x7e outright, otherwise reads the targeting byte at +9 of the move record. | `src/battle/battle_move.c` |
| `Func_80bd7dc` | `0x080bd7dc` | `Battle_BeginTurnPhase` | read | Battle | Sets the phase word at state+0x800 if it is still clear. | `src/battle/battle_turn.c` |
| `Func_80bd808` | `0x080bd808` | `Battle_StartTurnTask` | read+callee | Battle | Starts Func_80bd898 as the turn task and records its handle in the state block. | `src/battle/battle_turn.c` |
| `Func_80bdfec` | `0x080bdfec` | `Battle_ResetTurnPhase` | read | Battle | Clears the three phase words at state+0x7fc, +0x800 and +0x804. | `src/battle/battle_turn.c` |
| `Func_80be070` | `0x080be070` | `Battle_GetTargetsForMove` | read+callee | Battle | Builds the target list for a move's targeting kind and returns how many entries it produced. | `src/battle/battle_target.c` |
| `Func_80bf65c` | `0x080bf65c` | `Battle_TickAllStatus` | read+family | Battle | Runs the per-unit status tick twenty times -- once per combatant slot. The counterpart to the TickStatusCounter family. | `src/battle/status.c` |
| `Func_80bf674` | `0x080bf674` | `NullSub_80bf674` | read | Battle | Empty body. | `src/battle/battle_main.c` |
| `Func_80c0098` | `0x080c0098` | `Battle_InitOrderTable` | read | Battle | Fills a table with the ascending byte pattern 0x03020100 through the RAM-resident fill routine. | `src/battle/battle_turn.c` |
| `Func_80c0184` | `0x080c0184` | `Battle_SelectBGVariant` | read | Battle | Picks a background variant from .Lc5a30 using the counter at the head of the block at iwram_3001ef8, bounded at 0x1f. | `src/battle/battle_draw.c` |
| `Func_80c01bc` | `0x080c01bc` | `Battle_UpdateScroll` | read | Battle | Feeds the current scroll values into the background update hook each frame. | `src/battle/battle_draw.c` |
| `Func_80c0298` | `0x080c0298` | `Battle_ResetBG0Scroll` | read | Battle | Writes zero to BG0VOFS. | `src/battle/battle_draw.c` |
| `Func_80c08a8` | `0x080c08a8` | `Battle_AllocEffectBlock` | read | Battle | Reserves the 0x2a0-byte EWRAM block under tag 0xa and stores it at iwram_3001f00. | `src/battle/battle_draw.c` |
| `Func_80c08e0` | `0x080c08e0` | `Battle_FreeEffectBlock` | read | Battle | Releases the tag-0xa block Battle_AllocEffectBlock reserved. | `src/battle/battle_draw.c` |
| `Func_80c0df4` | `0x080c0df4` | `Battle_GetActorXZ` | read | Battle | Reads a battle actor's x and z position words out of its record. | `src/battle/battle_camera.c` |
| `Func_80c0e38` | `0x080c0e38` | `Battle_FadeIn` | read+family | Battle | Steps BLDCNT and the blend weights up over successive frames. | `src/battle/battle_draw.c` |
| `Func_80c0e70` | `0x080c0e70` | `Battle_FadeOut` | read+family | Battle | The mirror of Battle_FadeIn, in the same file. | `src/battle/battle_draw.c` |
| `Func_80c0ea8` | `0x080c0ea8` | `Battle_SetBlendAll` | read | Battle | Writes 0xbf to BLDCNT, enabling the blend across every layer. | `src/battle/battle_draw.c` |
| `Func_80c0eb8` | `0x080c0eb8` | `Battle_ResetMatrix` | read | Battle | Resets a matrix to identity through the register-pinned unrolled store. | `src/battle/battle_camera.c` |
| `Func_80c0edc` | `0x080c0edc` | `TileFromPixels` | read | Battle | Divides a pixel coordinate by sixteen to give a tile coordinate. | `src/battle/battle_layout.c` |
| `Func_80c1084` | `0x080c1084` | `Battle_ApplyShakeOffset` | read | Battle | Applies the next signed offset from the shake table .Lc5c10 to the battle view. | `src/battle/battle_draw.c` |
| `Func_80c16d0` | `0x080c16d0` | `Battle_StopPreAnimTask` | read+callee | Battle | Stops Task_BlitPreAnim and its companion, then releases their block through the RAM-resident routine. | `src/battle/battle_main.c` |
| `Func_80c1a14` | `0x080c1a14` | `Battle_ResetCamera` | read+callee | Battle | Calls the camera setter with both arguments zero. | `src/battle/battle_camera.c` |
| `Func_80c1fa8` | `0x080c1fa8` | `Battle_PickEncounterVariant` | read | Battle | Chooses one of an encounter's listed variants at random from the .Lc5c38 table. | `src/battle/battle_main.c` |
| `Func_80c2368` | `0x080c2368` | `GetEnemyGroupSize` | read | Battle | Returns the top three bits of byte 3 of an eight-byte .Lc7420 row -- the group's member count. | `src/battle/battle_main.c` |
| `Func_80c2384` | `0x080c2384` | `GetEnemyGroupId` | read | Battle | Returns the first halfword of a .Lc7420 row, falling back to row 0 above index 0xab. | `src/battle/battle_main.c` |
| `Func_80c23a0` | `0x080c23a0` | `GetEnemyGroupEntry` | read | Battle | Returns a pointer into the .Lc7420 row for an encounter index, bounded at 0xab. | `src/battle/battle_main.c` |
| `Func_80c2470` | `0x080c2470` | `GetItemBattleEffect` | read | Battle | Masks an item id to 0x1ff and reads the battle effect byte out of its info record. | `src/battle/battle_move.c` |
| `Func_80c2a08` | `0x080c2a08` | `NullSub_80c2a08` | read | Battle | Empty body. | `src/battle/battle_main.c` |

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

## Debug menu — 9 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80251d4` | `0x080251d4` | `PackCoordPair` | read | Debug menu | Masks both arguments to ten bits and stores them as a packed coordinate pair. Which coordinates is not established by the body. | `src/ui/debug_menu.c` |
| `Func_80284dc` | `0x080284dc` | `Debug_StartMenuTask` | read | Debug menu | Reserves the debug menu's EWRAM block and starts Func_8028194 as its task. | `src/ui/debug_menu.c` |
| `Func_802851c` | `0x0802851c` | `Debug_MenuTick` | read | Debug menu | The debug menu's per-frame update over its box handle and entry count. | `src/ui/debug_menu.c` |
| `Func_802899c` | `0x0802899c` | `Debug_BuildMenuBar` | read+callee | Debug menu | Populates the debug menu bar with its options and steps the fade in. | `src/ui/debug_menu.c` |
| `Func_80289e8` | `0x080289e8` | `Debug_ApplyWarp` | read | Debug menu | Applies the selected warp destination from .L3740f into the two EWRAM coordinate halfwords. | `src/ui/debug_menu.c` |
| `Func_8028aa8` | `0x08028aa8` | `Debug_ShowMessage` | read+callee | Debug menu | Opens message _MSG_c7b in the debug window. | `src/ui/debug_menu.c` |
| `Func_8028b80` | `0x08028b80` | `Debug_ShowSmallText` | read+callee | Debug menu | Closes the debug window and redraws it with the small-text renderer. | `src/ui/debug_menu.c` |
| `Func_8028edc` | `0x08028edc` | `Debug_StartWarpMenu` | read | Debug menu | Starts Debug_WarpMenu at priority 0xc80. | `src/ui/debug_menu.c` |
| `Func_80294d0` | `0x080294d0` | `NullSub_80294d0` | read | Debug menu | Empty body. | `src/ui/debug_menu.c` |

## Field — 5 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_800c0c4` | `0x0800c0c4` | `NullSub_800c0c4` | read | Field | Empty body. | `src/field/field.c` |
| `Func_800c5b4` | `0x0800c5b4` | `StartFieldRender` | read | Field | Registers the two per-frame field hooks Func_800c62c and Func_800c880, opens the panel and unblanks. | `src/field/field.c` |
| `Func_800c5fc` | `0x0800c5fc` | `StopFieldRender` | read | Field | Unregisters both field hooks and restores the blend register. | `src/field/field.c` |
| `Func_800c628` | `0x0800c628` | `ReturnTrue_800c628` | read | Field | Returns 1 unconditionally. A predicate stub; nothing in the body says what it is standing in for. | `src/field/field.c` |
| `Func_800c87c` | `0x0800c87c` | `ReturnTrue_800c87c` | read | Field | Returns 1 unconditionally, the twin of ReturnTrue_800c628. | `src/field/field.c` |

## Field / cutscene — 25 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_808d7d8` | `0x0808d7d8` | `Field_RunActorMessage` | read+callee | Field / cutscene | Opens a cutscene, delivers the actor's message and closes it again. | `src/field/cutscene.c` |
| `Func_808e078` | `0x0808e078` | `MapActor_PlayAnimWithSound` | read+callee | Field / cutscene | Plays a sound, sets a map actor's animation and waits the frames out. | `src/field/cutscene.c` |
| `Func_808ec4c` | `0x0808ec4c` | `NullSub_808ec4c` | read | Field / cutscene | Empty body. | `src/field/cutscene.c` |
| `Func_808ec50` | `0x0808ec50` | `MapActor_PlayCutsceneAnimA` | read+family | Field / cutscene | Sets the faced actor's animation, plays a sound and waits. First of four near-identical cutscene animation helpers. | `src/field/cutscene.c` |
| `Func_808ec8c` | `0x0808ec8c` | `MapActor_PlayCutsceneAnimB` | read+family | Field / cutscene | The variant that also sets the actor's sprite flags. | `src/field/cutscene.c` |
| `Func_808ece0` | `0x0808ece0` | `MapActor_PlayCutsceneAnimC` | read+family | Field / cutscene | The variant taking the actor id as an argument. | `src/field/cutscene.c` |
| `Func_808ed1c` | `0x0808ed1c` | `MapActor_SetCutsceneAnim` | read+family | Field / cutscene | The bare variant: sets the animation and nothing else. | `src/field/cutscene.c` |
| `Func_8091778` | `0x08091778` | `NullSub_8091778` | read | Field / cutscene | Empty body. | `src/field/cutscene.c` |
| `Func_8091780` | `0x08091780` | `Field_InitCutsceneActors` | read+callee | Field / cutscene | Initialises the map actors for a cutscene and waits for the field actor to appear. | `src/field/cutscene.c` |
| `Func_80917d0` | `0x080917d0` | `Field_JoinPartyMember` | read+family | Field / cutscene | Adds a party member and runs the join UI. One of two variants differing in which UI hook they call. | `src/field/cutscene.c` |
| `Func_80917f4` | `0x080917f4` | `Field_JoinPartyMemberB` | read+family | Field / cutscene | The second join variant. | `src/field/cutscene.c` |
| `Func_8091c1c` | `0x08091c1c` | `Field_GiveItemToMember` | read | Field / cutscene | Hands an item to a party member, returning the member id or -1. The second argument is unused, in the ROM too. | `src/field/cutscene.c` |
| `Func_8091c3c` | `0x08091c3c` | `NullSub_8091c3c` | read | Field / cutscene | Empty body. | `src/field/cutscene.c` |
| `Func_8091d84` | `0x08091d84` | `Field_AskYesNo` | read | Field / cutscene | Puts up the yes/no prompt with the three trailing arguments zeroed. | `src/field/cutscene.c` |
| `Func_8091f90` | `0x08091f90` | `Field_SetWarpDest` | read+family | Field / cutscene | Writes the destination pair at gState+0x1ce/+0x1d0. | `src/field/cutscene.c` |
| `Func_8091fa8` | `0x08091fa8` | `Field_SetWarpDestB` | read+family | Field / cutscene | Writes the second destination pair at gState+0x1d2/+0x1d4. | `src/field/cutscene.c` |
| `Func_8091ff0` | `0x08091ff0` | `Field_StartLoopSound` | read | Field / cutscene | Records a looping sound id in the map state and starts it, treating -1 as stop. | `src/field/cutscene.c` |
| `Func_809202c` | `0x0809202c` | `Field_StopLoopSound` | read+callee | Field / cutscene | Stops whatever Field_StartLoopSound left running. | `src/field/cutscene.c` |
| `Func_809218c` | `0x0809218c` | `MapActor_TravelToWait` | read+family | Field / cutscene | MapActor_TravelTo plus a wait for the movement to finish; shares its file. | `src/field/cutscene.c` |
| `Func_80921c4` | `0x080921c4` | `MapActor_TravelToAnim` | read+family | Field / cutscene | The variant that also sets the actor's animation. | `src/field/cutscene.c` |
| `Func_8092208` | `0x08092208` | `MapActor_WalkTo` | read+family | Field / cutscene | Stops, animates and walks a field actor to an absolute position, then waits. | `src/field/cutscene.c` |
| `Func_809228c` | `0x0809228c` | `MapActor_MoveBy` | read+family | Field / cutscene | The relative-offset member of the travel family. | `src/field/cutscene.c` |
| `Func_80922c4` | `0x080922c4` | `MapActor_MoveByAnim` | read+family | Field / cutscene | Relative move with an animation set first. | `src/field/cutscene.c` |
| `Func_8092304` | `0x08092304` | `MapActor_MoveByAnimWait` | read+family | Field / cutscene | Relative move with animation, waiting for arrival. | `src/field/cutscene.c` |
| `Func_809255c` | `0x0809255c` | `NullSub_809255c` | read | Field / cutscene | Empty body. | `src/field/cutscene.c` |

## Field / field move — 5 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_808e118` | `0x0808e118` | `Field_InitMoveTargets` | read | Field / field move | Clears the field-move target list in the map state block. | `src/field/field_move.c` |
| `Func_808e5d8` | `0x0808e5d8` | `Field_UseFieldMove` | read+callee | Field / field move | Runs a field move: reads its record, resolves the target actor and drives the two effect hooks. | `src/field/field_move.c` |
| `Func_808e96c` | `0x0808e96c` | `Field_QueryMoveTarget` | read+callee | Field / field move | Asks the map query layer for the target of move kind 0x70000005. | `src/field/field_move.c` |
| `Func_8091814` | `0x08091814` | `Field_PartyHasMove` | read+callee | Field / field move | True when the requested member is present and knows the requested move. | `src/field/field_move.c` |
| `Func_8091858` | `0x08091858` | `Field_RefreshMoveFlags` | read+callee | Field / field move | Recomputes which field moves the party can currently use into the global state. | `src/field/field_move.c` |

## Field / map — 32 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_808ab48` | `0x0808ab48` | `Map_LoadOverlayCode` | read | Field / map | Loads the current map's code overlay from the table at .L9f1a8 into the overlay region. | `src/field/map_load.c` |
| `Func_808adf0` | `0x0808adf0` | `Field_CheckEncounterTile` | read+callee | Field / map | Resolves the party's tile, looks up its encounter row in .L9d7a8 and reports whether an encounter is due. | `src/field/encounter.c` |
| `Func_808b02c` | `0x0808b02c` | `Map_RunEventAt` | read | Field / map | Runs the map event whose id sits at index arg0 of the state block's event list. | `src/field/map_event.c` |
| `Func_808b048` | `0x0808b048` | `Map_CheckEventAt` | read+callee | Field / map | The query half of Map_RunEventAt, in the same file. | `src/field/map_event.c` |
| `Func_808b074` | `0x0808b074` | `GetMapEncounterGroup` | read+callee | Field / map | Reads an (encounter, group) pair out of .L9d8b0 and resolves it to an enemy group. | `src/field/encounter.c` |
| `Func_808b248` | `0x0808b248` | `GetCurrentMapId` | read | Field / map | Returns the halfword at gState+0x1d6. | `src/field/map_load.c` |
| `Func_808b25c` | `0x0808b25c` | `Map_LoadAreaTable` | read | Field / map | Copies the area descriptor at .L9e270 into the global state. | `src/field/map_load.c` |
| `Func_808b2b0` | `0x0808b2b0` | `Map_SetAreaName` | read | Field / map | Selects one of the _AREA_* name ids for the current area and stores it. | `src/field/map_load.c` |
| `Func_808b398` | `0x0808b398` | `IsPartyMemberAvailable` | read+callee | Field / map | True for ids up to 8 once flag 0x20 is set -- the party-joined gate. | `src/field/map_event.c` |
| `Func_808b3d0` | `0x0808b3d0` | `GetPartyMemberMapId` | read | Field / map | Remaps a party index to its map actor id through a chain of equality tests. | `src/field/map_event.c` |
| `Func_808b9f8` | `0x0808b9f8` | `Map_StoreState` | read | Field / map | Writes the current map fields back into the persistent block. | `src/field/map_load.c` |
| `Func_808bc9c` | `0x0808bc9c` | `Map_SumEventCounters` | read | Field / map | Sums the six event counters at state+0x16c onward. | `src/field/map_event.c` |
| `Func_808c44c` | `0x0808c44c` | `Map_OpenWorldView` | read+callee | Field / map | Allocates the world view's buffers and brings the map view up. | `src/field/map_load.c` |
| `Func_808c4c0` | `0x0808c4c0` | `Map_OpenWorldViewB` | read+family | Field / map | The second world-view entry, differing in which view routine it calls. | `src/field/map_load.c` |
| `Func_808d428` | `0x0808d428` | `Field_IsFlagSetOrAlways` | read | Field / map | Returns true for the sentinel -1, otherwise reads the flag. | `src/field/map_event.c` |
| `Func_808d458` | `0x0808d458` | `Field_IsFlagClear` | read+family | Field / map | The inverted twin of Field_IsFlagSetOrAlways, in the same file. | `src/field/map_event.c` |
| `Func_808d5a4` | `0x0808d5a4` | `Field_FindEventForActor` | read+callee | Field / map | Looks up the event record attached to a map actor via FindMapActorEvent. | `src/field/map_event.c` |
| `Func_808e0b0` | `0x0808e0b0` | `MapActor_UpdateEntity` | read | Field / map | Per-frame update of one map entity's flag byte from the table at .L9e6b8. | `src/field/map_actor.c` |
| `Func_808e990` | `0x0808e990` | `Field_HasEventAt` | read+callee | Field / map | Normalises the event lookup to 0 or 1 with the neg-or-self sign trick. | `src/field/map_event.c` |
| `Func_808e9a8` | `0x0808e9a8` | `Field_TestCollisionAt` | read+callee | Field / map | Tests the collision layer at a record's position. | `src/field/map_event.c` |
| `Func_808ed4c` | `0x0808ed4c` | `Map_GetFacedSlot` | read | Field / map | Reads the slot record for whatever the player is facing, or -1. | `src/field/map_event.c` |
| `Func_808ed78` | `0x0808ed78` | `Map_ClearFacedSlot` | read+callee | Field / map | Clears the record Map_GetFacedSlot would return. | `src/field/map_event.c` |
| `Func_808edac` | `0x0808edac` | `Map_TestFacedCollision` | read+callee | Field / map | Runs the collision test against the faced slot's position. | `src/field/map_event.c` |
| `Func_808eee4` | `0x0808eee4` | `MapActor_SetSubFlag` | read | Field / map | Writes the flag byte at +9 of a map actor's sub-record. | `src/field/map_actor.c` |
| `Func_808f0d8` | `0x0808f0d8` | `MapActor_StepTowardA` | read+family | Field / map | Steps an actor's position toward a target. First of three variants differing in which coordinate fields they use. | `src/field/map_actor.c` |
| `Func_808f140` | `0x0808f140` | `MapActor_StepTowardB` | read+family | Field / map | The second stepping variant. | `src/field/map_actor.c` |
| `Func_808f28c` | `0x0808f28c` | `MapActor_StepTowardC` | read+family | Field / map | The third stepping variant. | `src/field/map_actor.c` |
| `Func_808f304` | `0x0808f304` | `Map_GetStateWord` | read | Field / map | Returns a word out of the map state block. | `src/field/map_load.c` |
| `Func_8090378` | `0x08090378` | `Map_SetLayerVisible` | read | Field / map | Toggles one map layer's visibility in the view block. | `src/field/map_load.c` |
| `Func_8091660` | `0x08091660` | `MapActor_SetMotionFields` | read | Field / map | Writes an actor's three motion words at +0x24, +0x2c and +0x30. | `src/field/map_actor.c` |
| `Func_8091e9c` | `0x08091e9c` | `Map_GetEventFlagWord` | read | Field / map | Reads the event flag halfword at map state +0x170. | `src/field/map_event.c` |
| `Func_809259c` | `0x0809259c` | `MapActor_InstallScript9EBFC` | read | Field / map | Installs the script at .L9ebfc on a field actor. What the script does is not established here. | `src/field/map_actor.c` |

## Field / transition — 13 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_808feb0` | `0x0808feb0` | `Field_StartScreenTransition` | read+callee | Field / transition | Installs the screen-window transition and its companion hook. | `src/field/transition.c` |
| `Func_8090584` | `0x08090584` | `Field_ScanlineHook` | read | Field / transition | The per-scanline hook: reads VCOUNT and writes the matching row of the effect buffer. | `src/field/transition.c` |
| `Func_80907b0` | `0x080907b0` | `Transition_FillPattern` | read | Field / transition | Fills the transition buffer with a repeating word pattern. | `src/field/transition.c` |
| `Func_8090824` | `0x08090824` | `Transition_Start300` | read+callee | Field / transition | Allocates the transition block, primes its pattern and starts Task_Transition300. | `src/field/transition.c` |
| `Func_8091174` | `0x08091174` | `Transition_StartWindow` | read+callee | Field / transition | The window-transition entry: allocates, primes and starts its own task. | `src/field/transition.c` |
| `Func_80911e8` | `0x080911e8` | `Transition_Stop` | read | Field / transition | Stops the transition task and releases the tag-0x20 block. | `src/field/transition.c` |
| `Func_8091200` | `0x08091200` | `Transition_SetParams` | read+family | Field / transition | Writes the transition's two parameters into its block. | `src/field/transition.c` |
| `Func_8091220` | `0x08091220` | `Transition_SetParamsB` | read+family | Field / transition | The second parameter setter, with the arguments the other way round. | `src/field/transition.c` |
| `Func_8091240` | `0x08091240` | `Transition_SetMode` | read | Field / transition | Writes the mode halfword at the head of the transition block. | `src/field/transition.c` |
| `Func_8091294` | `0x08091294` | `ClampBrightnessField` | read+family | Field / transition | Clamps to 0..0x1f. Byte-for-byte the same routine as ClampBrightness in rom_f2000; the ROM has both. | `src/field/transition.c` |
| `Func_80912a8` | `0x080912a8` | `ClampScaleField` | read+family | Field / transition | Clamps to at most 0x7c00. The duplicate of ClampScale in rom_f2000. | `src/field/transition.c` |
| `Func_8091540` | `0x08091540` | `Field_AddFadeHook` | read+family | Field / transition | Installs Func_80912b8 as a per-frame hook. | `src/field/transition.c` |
| `Func_8091550` | `0x08091550` | `Field_RemoveFadeHook` | read+family | Field / transition | Removes it again. | `src/field/transition.c` |

## Graphics — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80f037c` | `0x080f037c` | `BuildAffineRampTable` | read | Graphics | Fills a 512-word buffer in four runs -- 32 words of 0x01ff01ff, 240 stepping by 0x00020002 from 0x00010000, 48 more of 0x01ff01ff and 192 zeroes. A packed pair of 16-bit ramps. | `src/graphics/affine.c` |

## Inventory UI — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8025180` | `0x08025180` | `GetItemDisplayFlags` | read | Inventory UI | Reads the display flag bytes out of an item record at +2 and +0xc. | `src/ui/inventory_ui.c` |

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

## Menus — 89 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80a1050` | `0x080a1050` | `Menu_LeaveMapView` | read | Menus | Tears down the map-view hook and clears the two menu-open flags at 0x166 and 0x152. | `src/menu/menu_main.c` |
| `Func_80a1070` | `0x080a1070` | `Menu_EnterMapView` | read | Menus | The mirror: sets both menu-open flags, installs the map redraw hook and returns the task handle. | `src/menu/menu_main.c` |
| `Func_80a10d0` | `0x080a10d0` | `Menu_OpenBox` | read | Menus | Opens a UI box into a caller-held slot, reusing the existing box when the flag bit 0x100 is set rather than recreating it. | `src/menu/menu_box.c` |
| `Func_80a1114` | `0x080a1114` | `Menu_CloseBox` | read | Menus | Closes the box held in the slot and nulls the slot, if there is one. | `src/menu/menu_box.c` |
| `Func_80a14f0` | `0x080a14f0` | `Menu_DrawNumber` | read | Menus | Counts a value's decimal digits up to fifteen and hands the count to the number-drawing primitive. | `src/menu/menu_draw.c` |
| `Func_80a172c` | `0x080a172c` | `Menu_CreateIconSprite` | read | Menus | Allocates a sprite slot, uploads the icon sheet at .Laea4c and registers the sprite. | `src/menu/menu_sprite.c` |
| `Func_80a1778` | `0x080a1778` | `Menu_CreateCursorSprite` | read | Menus | The same allocate-and-upload against .Laea4c, returning the slot; used for the menu's movable cursor. | `src/menu/menu_sprite.c` |
| `Func_80a17c4` | `0x080a17c4` | `Menu_LayoutNode` | read | Menus | Recomputes one grid node's width, height and two positions from its record. | `src/menu/menu_grid.c` |
| `Func_80a1804` | `0x080a1804` | `Menu_ClearTextArea` | read | Menus | Calls the text-area primitive with a zero rect. Note the ROM reads an uninitialised argument here and the C reproduces it. | `src/menu/menu_draw.c` |
| `Func_80a1814` | `0x080a1814` | `Menu_CreatePanel` | read | Menus | Builds a 13x5 window at g+0x10, attaches a text layer, stores the handle at g+0x14, writes both OBJ priorities and the 0xff/0 no-selection sentinel. Returns the window. | `src/menu/panel.c` |
| `Func_80a195c` | `0x080a195c` | `Menu_TeardownPanel` | read | Menus | Deletes the panel's sprites, stops the panel task and releases the state block. | `src/menu/menu_main.c` |
| `Func_80a19a0` | `0x080a19a0` | `Menu_PanelTask` | read | Menus | The panel's per-frame task: walks the eight actor sprites in the state block and updates their bitfields. | `src/menu/menu_main.c` |
| `Func_80a1bc8` | `0x080a1bc8` | `NullSub_80a1bc8` | read | Menus | Empty body. | `src/menu/menu_main.c` |
| `Func_80a1bcc` | `0x080a1bcc` | `Menu_LayoutGridDefault` | read+callee | Menus | Calls Menu_LayoutGrid with the default origin (0x6c, 0x28) and eight columns. | `src/menu/menu_grid.c` |
| `Func_80a1bdc` | `0x080a1bdc` | `Menu_LayoutGrid` | read | Menus | Places every node in the state block's 0x48 list on a grid of the given origin and column count. | `src/menu/menu_grid.c` |
| `Func_80a1c2c` | `0x080a1c2c` | `Menu_PlaceGridNode` | read | Menus | Positions one node from its index: row times 16 plus y into +8, column times 16 plus x into +6, with the index wrapped at 0x1f. | `src/menu/menu_grid.c` |
| `Func_80a1cb0` | `0x080a1cb0` | `Menu_RefreshGrid` | read | Menus | Re-runs the per-node layout across the grid for the given mode. | `src/menu/menu_grid.c` |
| `Func_80a21b0` | `0x080a21b0` | `Menu_DrawPageDots` | read | Menus | Draws the page indicator: total divided by page size, rounded up, one tile from 0x31 per page. | `src/menu/menu_draw.c` |
| `Func_80a2268` | `0x080a2268` | `Menu_SetWindowRect` | read | Menus | Writes a window's position and size halfwords and selects its tile bank. | `src/menu/menu_box.c` |
| `Func_80a23c0` | `0x080a23c0` | `Menu_DrawCoins` | read | Menus | Draws the party's coin total from gState+0x10 with its label. | `src/menu/menu_draw.c` |
| `Func_80a23f4` | `0x080a23f4` | `Menu_SetNodeRect` | read | Menus | Writes a node's four position and size halfwords at +8/+0xa/+0xc/+0xe, if the node exists. | `src/menu/menu_grid.c` |
| `Func_80a2408` | `0x080a2408` | `Menu_SetDirtyFlag` | read | Menus | Sets the redraw byte at +0xea6 of the menu's window block. | `src/menu/menu_main.c` |
| `Func_80a2420` | `0x080a2420` | `Menu_ClearDirtyFlag` | read | Menus | The mirror of Menu_SetDirtyFlag on the same byte. | `src/menu/menu_main.c` |
| `Func_80a2438` | `0x080a2438` | `Menu_PlayConfirm` | read | Menus | Plays the confirmation sound and returns 1 so it can stand in as a predicate. | `src/menu/menu_main.c` |
| `Func_80a2444` | `0x080a2444` | `Menu_WaitStartTask` | read | Menus | Per-frame task: on the 0x08 key it plays sound 0x71, sets flag 0x150 and stops itself. | `src/menu/menu_main.c` |
| `Func_80a2474` | `0x080a2474` | `Menu_StartWaitStart` | read+callee | Menus | Clears flag 0x150 and starts Menu_WaitStartTask at priority 0xc80. | `src/menu/menu_main.c` |
| `Func_80a2490` | `0x080a2490` | `Menu_StopWaitStart` | read+callee | Menus | Stops Menu_WaitStartTask if flag 0x150 is still clear. | `src/menu/menu_main.c` |
| `Func_80a24ac` | `0x080a24ac` | `Menu_SetTextColorF` | read | Menus | Selects text colour 15. Which palette entry that is is not established here, so the name carries the number. | `src/menu/menu_draw.c` |
| `Func_80a24b8` | `0x080a24b8` | `Menu_SetTextColor2` | read | Menus | Selects text colour 2. Same caveat. | `src/menu/menu_draw.c` |
| `Func_80a24c4` | `0x080a24c4` | `Menu_SetTextColor4` | read | Menus | Selects text colour 4. Same caveat. | `src/menu/menu_draw.c` |
| `Func_80a345c` | `0x080a345c` | `Menu_ResetGridIcons` | read | Menus | Walks all 32 grid nodes writing icon index 0xd into each. | `src/menu/menu_grid.c` |
| `Func_80a34c0` | `0x080a34c0` | `Menu_ClosePanel` | read+callee | Menus | The close sequence: reset the icons, tear down the panel, wait out the fade and close the boxes. | `src/menu/menu_main.c` |
| `Func_80a38a8` | `0x080a38a8` | `Menu_SelectEntry` | read | Menus | Moves the selection to the given entry, refreshing the box and the cursor sprite. | `src/menu/menu_main.c` |
| `Func_80a3c98` | `0x080a3c98` | `Menu_EndCursorAnim` | read | Menus | Restores the cursor sprite's idle animation and stops the cursor task. | `src/menu/menu_sprite.c` |
| `Func_80a3cf8` | `0x080a3cf8` | `Menu_ShowMessage` | read | Menus | Clears the message box held at state+0x10c and opens the given message id into it. | `src/menu/menu_draw.c` |
| `Func_80a3d24` | `0x080a3d24` | `Menu_FillGridFromList` | read | Menus | Copies a halfword list into the grid nodes, re-laying out each as it goes. | `src/menu/menu_grid.c` |
| `Func_80a3e88` | `0x080a3e88` | `Menu_ShowUnitItems` | read | Menus | Builds the item grid for one unit and opens the description box against it. | `src/menu/menu_main.c` |
| `Func_80a3eec` | `0x080a3eec` | `NullSub_80a3eec` | read | Menus | Empty body. | `src/menu/menu_main.c` |
| `Func_80a4110` | `0x080a4110` | `GetMenuGridEntry` | read | Menus | Bounds-checked lookup into the three-by-three table at .Laf2e4; out of range gives zero. | `src/menu/menu_grid.c` |
| `Func_80a413c` | `0x080a413c` | `GetMenuRowHeight` | read | Menus | Returns 0x1e or 0x26 depending on the second argument. The first argument is unused, in the ROM as well. | `src/menu/menu_grid.c` |
| `Func_80a45cc` | `0x080a45cc` | `Menu_DrawStatDelta` | read | Menus | Draws a stat comparison line, switching text colour to 0xe when the leading byte is -1. | `src/menu/menu_draw.c` |
| `Func_80a4754` | `0x080a4754` | `Menu_BreakRandomItem` | read | Menus | Picks an equipped item at random, breaks it, plays the break sound and refreshes the row. | `src/menu/menu_main.c` |
| `Func_80a47b4` | `0x080a47b4` | `Menu_OpenSubBox` | read+callee | Menus | Opens the sub-box for a menu index through Menu_OpenBox and primes its contents. | `src/menu/menu_box.c` |
| `Func_80a4db4` | `0x080a4db4` | `Menu_DrawValueWithUnit` | read | Menus | Draws a three-digit value and then the unit string that follows it, from .Laf224/.Laf228. | `src/menu/menu_draw.c` |
| `Func_80a4e20` | `0x080a4e20` | `Menu_SetCursorRectA` | read+callee | Menus | Calls Menu_SetNodeRect with (0xd, 5, 0x11, 0xa). One of six fixed-argument variants in adjacent files, distinguished only by the y offset and the column count. | `src/menu/menu_grid.c` |
| `Func_80a4e44` | `0x080a4e44` | `Menu_SetCursorRectB` | read+callee | Menus | Menu_SetNodeRect with (0xd, 3, 0x11, 0xa). | `src/menu/menu_grid.c` |
| `Func_80a4e68` | `0x080a4e68` | `Menu_SetCursorRectC` | read+callee | Menus | The third of the six Menu_SetNodeRect variants in this run of adjacent files. | `src/menu/menu_grid.c` |
| `Func_80a4e90` | `0x080a4e90` | `Menu_SetCursorRectD` | read+callee | Menus | Menu_SetNodeRect with (0xd, 0, 0x11, 6). | `src/menu/menu_grid.c` |
| `Func_80a4eb8` | `0x080a4eb8` | `Menu_SetCursorRectE` | read+callee | Menus | Menu_SetNodeRect with (0xd, 0, 0x11, 7). | `src/menu/menu_grid.c` |
| `Func_80a4ee0` | `0x080a4ee0` | `Menu_SetCursorRectF` | read+callee | Menus | Menu_SetNodeRect with (0xd, 0, 0x11, 3). | `src/menu/menu_grid.c` |
| `Func_80a5534` | `0x080a5534` | `Menu_LoadStatusSprites` | read | Menus | Allocates sprite slots and uploads the two sheets at .Laebcc and .Laeb4c for the status screen. | `src/menu/menu_sprite.c` |
| `Func_80a5578` | `0x080a5578` | `Menu_GetSelectedUnit` | read | Menus | Resolves the cursor index to a unit record and writes it out. One of three near-identical resolvers in this bank, differing in which state offset they read. | `src/menu/menu_main.c` |
| `Func_80a5780` | `0x080a5780` | `ReturnTrue_80a5780` | read | Menus | Returns 1 unconditionally; one of six such predicate stubs in this bank. | `src/menu/menu_main.c` |
| `Func_80a5784` | `0x080a5784` | `NullSub_80a5784` | read | Menus | Empty body. | `src/menu/menu_main.c` |
| `Func_80a6384` | `0x080a6384` | `Menu_OpenGridFor` | read+callee | Menus | Lays out the grid for one unit's entries and returns the resulting count. | `src/menu/menu_grid.c` |
| `Func_80a63dc` | `0x080a63dc` | `ReturnTrue_80a63dc` | read | Menus | Returns 1 unconditionally. | `src/menu/menu_main.c` |
| `Func_80a63e0` | `0x080a63e0` | `NullSub_80a63e0` | read | Menus | Empty body. | `src/menu/menu_main.c` |
| `Func_80a6874` | `0x080a6874` | `Menu_CloseSubmenu` | read+callee | Menus | Closes the submenu's UI box and tears down its panel. | `src/menu/menu_box.c` |
| `Func_80a68a8` | `0x080a68a8` | `Menu_FillGridFromMoves` | read | Menus | Copies a move list into the grid nodes and lays each one out, the move-screen twin of Menu_FillGridFromList. | `src/menu/menu_grid.c` |
| `Func_80a6a00` | `0x080a6a00` | `Menu_GetSelectedUnitMoves` | read | Menus | The move-screen resolver of the Menu_GetSelectedUnit trio. | `src/menu/menu_main.c` |
| `Func_80a735c` | `0x080a735c` | `Menu_IsItemSelectable` | read | Menus | Reads the move record behind an item and answers whether the menu may select it. | `src/menu/menu_main.c` |
| `Func_80a8034` | `0x080a8034` | `Menu_OpenStatusPanel` | read+callee | Menus | Builds the status panel through Menu_CreatePanel and primes its four rows. | `src/menu/menu_main.c` |
| `Func_80a8904` | `0x080a8904` | `Menu_SpinDelay` | read | Menus | A 256-iteration empty loop with an asm barrier -- a busy wait, present in the ROM. | `src/menu/menu_main.c` |
| `Func_80a8b8c` | `0x080a8b8c` | `Menu_GetSelectedUnitStats` | read | Menus | The stats-screen resolver of the Menu_GetSelectedUnit trio. | `src/menu/menu_main.c` |
| `Func_80a9370` | `0x080a9370` | `ReturnTrue_80a9370` | read | Menus | Returns 1 unconditionally. | `src/menu/menu_main.c` |
| `Func_80a9374` | `0x080a9374` | `Menu_ShowUnitMoves` | read+callee | Menus | Resets the grid icons and refills the grid from the selected unit's move list. | `src/menu/menu_main.c` |
| `Func_80a939c` | `0x080a939c` | `NullSub_80a939c` | read | Menus | Empty body. | `src/menu/menu_main.c` |
| `Func_80a99b0` | `0x080a99b0` | `Menu_MoveCursorDir` | read | Menus | Steps the cursor's column and row for a direction key, switching on the raw key bits (0x40 and friends). | `src/menu/menu_grid.c` |
| `Func_80a9a58` | `0x080a9a58` | `ReturnTrue_80a9a58` | read | Menus | Returns 1 unconditionally. | `src/menu/menu_main.c` |
| `Func_80a9b94` | `0x080a9b94` | `Menu_LayoutGridB` | read | Menus | A second grid layout routine with the same shape as Menu_LayoutGrid but driven off a different node list. | `src/menu/menu_grid.c` |
| `Func_80a9bd8` | `0x080a9bd8` | `Menu_PlaceGridNodeB` | read | Menus | The per-node half of Menu_LayoutGridB; identical arithmetic to Menu_PlaceGridNode with x and y swapped. | `src/menu/menu_grid.c` |
| `Func_80a9cbc` | `0x080a9cbc` | `Menu_RefreshGridB` | read | Menus | Re-runs Menu_LayoutNode across the second grid's nodes. | `src/menu/menu_grid.c` |
| `Func_80a9cf8` | `0x080a9cf8` | `Menu_OpenGridBoxes` | read | Menus | Creates one UI box per populated node of the second grid and returns how many it made. | `src/menu/menu_box.c` |
| `Func_80a9d3c` | `0x080a9d3c` | `Menu_ApplyGridFlags` | read | Menus | Walks the second grid applying a caller-supplied per-node flag byte and re-laying each node out. | `src/menu/menu_grid.c` |
| `Func_80a9dc4` | `0x080a9dc4` | `Menu_CountGridEntries` | read | Menus | Counts the grid nodes whose byte at +0xe is set. | `src/menu/menu_grid.c` |
| `Func_80a9e34` | `0x080a9e34` | `Menu_ResetGridAndIcon` | read+callee | Menus | Resets the grid and then sets icon 0xd, the pair used when a submenu closes. | `src/menu/menu_grid.c` |
| `Func_80a9e44` | `0x080a9e44` | `NullSub_80a9e44` | read | Menus | Empty body. | `src/menu/menu_main.c` |
| `Func_80a9f0c` | `0x080a9f0c` | `ReturnTrue_80a9f0c` | read | Menus | Returns 1 unconditionally. | `src/menu/menu_main.c` |
| `Func_80aa448` | `0x080aa448` | `Menu_ShowItemMove` | read+callee | Menus | Takes the move id packed in an item's info at +0x28 (masked to 0x3fff) and shows that move's description. | `src/menu/menu_main.c` |
| `Func_80aa460` | `0x080aa460` | `Menu_ShowMoveDesc` | read | Menus | Looks up a move record and drives the description panel from its kind and flag bytes. | `src/menu/menu_main.c` |
| `Func_80aa538` | `0x080aa538` | `WrapIndex` | read | Menus | Returns (a + b) % b -- the cursor wrap used throughout the grid code. | `src/menu/menu_grid.c` |
| `Func_80aa544` | `0x080aa544` | `Menu_FindEntryOffset` | read | Menus | Scans the halfword table at state+0x134 for the entry matching the biased index. | `src/menu/menu_grid.c` |
| `Func_80aac84` | `0x080aac84` | `Menu_FadePaletteRow` | read | Menus | Adds a signed delta to each of the red, green and blue channels of one palette row, clamping each. | `src/menu/menu_draw.c` |
| `Func_80ab2ec` | `0x080ab2ec` | `Menu_DrawStatRowEx` | read | Menus | Six-argument forwarder to the five-argument row drawer Func_80ab21c, reordering as it goes. | `src/menu/menu_draw.c` |
| `Func_80aca04` | `0x080aca04` | `Menu_OpenStatWindow` | read | Menus | Opens one of two stat windows, selected by the first argument, with a nine-argument layout call. | `src/menu/menu_box.c` |
| `Func_80ad318` | `0x080ad318` | `Menu_TeardownFieldSprites` | read | Menus | Deletes the field sprites held in the menu state block and stops their task. | `src/menu/menu_sprite.c` |
| `Func_80ad5f4` | `0x080ad5f4` | `SetActorScale` | read | Menus | Stores one slot's value in the field actor scale table at state+0x244. | `src/menu/menu_field.c` |
| `Func_80ad658` | `0x080ad658` | `Menu_StopFieldTasks` | read | Menus | Stops the field tasks in slots 0x89 through 0x8c. | `src/menu/menu_field.c` |
| `Func_80ae88c` | `0x080ae88c` | `Menu_LoadSummonSprites` | read | Menus | Allocates and uploads the two sheets at .Laed4c and .Laedcc, the same shape as Menu_LoadStatusSprites. | `src/menu/menu_sprite.c` |

## Name entry — 3 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_801d0f0` | `0x0801d0f0` | `Menu4_Stop` | read | Name entry | Stops Func_801cf48 and releases the tag-0x14 block. | `src/ui/name_entry.c` |
| `Func_801d980` | `0x0801d980` | `Menu4_Start` | read | Name entry | Reserves the 0x628-byte EWRAM block under tag 0x14 and starts the module's task. | `src/ui/name_entry.c` |
| `Func_801d9bc` | `0x0801d9bc` | `Menu4_Stop2` | read+family | Name entry | The second stop routine, against Func_801d94c rather than Func_801cf48. | `src/ui/name_entry.c` |

## Option menu — 12 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_801a778` | `0x0801a778` | `Menu2_Reset` | read | Option menu | Zeroes the option menu block's cursor and count fields. | `src/ui/option_menu.c` |
| `Func_801a7c0` | `0x0801a7c0` | `Menu2_AddPoint` | read | Option menu | Appends an (x, y) pair to the sixteen-entry point arrays at +0x354 and bumps the count. | `src/ui/option_menu.c` |
| `Func_801a90c` | `0x0801a90c` | `NullSub_801a90c` | read | Option menu | Empty body. | `src/ui/option_menu.c` |
| `Func_801a968` | `0x0801a968` | `Menu2_StartTask` | read | Option menu | Starts Func_801a98c at priority 200 << 4. | `src/ui/option_menu.c` |
| `Func_801a97c` | `0x0801a97c` | `Menu2_StopTask` | read | Option menu | Stops Func_801a98c. | `src/ui/option_menu.c` |
| `Func_801b148` | `0x0801b148` | `Menu2_UnlinkNode` | read | Option menu | Removes a node from the option menu's linked list. | `src/ui/option_menu.c` |
| `Func_801b1ec` | `0x0801b1ec` | `Menu2_SetCursorPos` | read | Option menu | Writes the cursor's two coordinate halfwords at block +0x396. | `src/ui/option_menu.c` |
| `Func_801b228` | `0x0801b228` | `Menu2_DrawArrows` | read+callee | Option menu | Draws both menu arrow cursors, left and right. | `src/ui/option_menu.c` |
| `Func_801b398` | `0x0801b398` | `Menu2_RunCursor` | read | Option menu | The cursor loop: reads gKeyPress and gKeyRepeat, moves the selection and redraws each frame. | `src/ui/option_menu.c` |
| `Func_801ba34` | `0x0801ba34` | `Menu2_BuildOptionList` | read | Option menu | Builds a six-entry option id list from the record at block +0x348 and hands it to the battle text layer. | `src/ui/option_menu.c` |
| `Func_801c2f0` | `0x0801c2f0` | `Menu2_ResetAndWait` | read+callee | Option menu | Resets the option menu block and waits a frame for the redraw. | `src/ui/option_menu.c` |
| `Func_801c304` | `0x0801c304` | `Menu2_Open` | read+callee | Option menu | Brings the option menu up: builds its nodes, draws the arrows, starts its task and selects the first entry. | `src/ui/option_menu.c` |

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

## Save UI — 8 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_801f704` | `0x0801f704` | `GetSaveSlotState` | read | Save UI | Reads the state byte for the current save slot out of the flash scratch block. | `src/ui/save_ui.c` |
| `Func_801f9b4` | `0x0801f9b4` | `Save_ShowResultMessage` | read | Save UI | Picks between the two save result messages _MSG_0a and _MSG_0b and shows it. | `src/ui/save_ui.c` |
| `Func_801fa3c` | `0x0801fa3c` | `Save_WriteAndConfirm` | read+callee | Save UI | Writes the save header and payload, then reports the outcome through the message pair. | `src/ui/save_ui.c` |
| `Func_801fb48` | `0x0801fb48` | `Save_AskOverwrite` | read+callee | Save UI | Puts up the yes/no prompt before overwriting a slot and plays the confirm sound. | `src/ui/save_ui.c` |
| `Func_801fba8` | `0x0801fba8` | `Save_LoadSlot` | read+callee | Save UI | Reads a slot back through the flash layer and reports success or failure. | `src/ui/save_ui.c` |
| `Func_801fc84` | `0x0801fc84` | `Save_ConfirmLoad` | read+callee | Save UI | The yes/no confirmation wrapped around Save_LoadSlot. | `src/ui/save_ui.c` |
| `Func_801fd84` | `0x0801fd84` | `Save_StartTask` | read | Save UI | Starts Func_801fd34 at priority 0xc80. | `src/ui/save_ui.c` |
| `Func_801fd98` | `0x0801fd98` | `Save_StopTask` | read | Save UI | Stops Func_801fd34. | `src/ui/save_ui.c` |

## Save menu — 7 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_801c3e8` | `0x0801c3e8` | `Menu3_Close` | read | Save menu | Closes the menu's box and stops its own task. | `src/ui/save_menu.c` |
| `Func_801c428` | `0x0801c428` | `Menu3_CloseBox` | read | Save menu | Closes the box held at block +0x230 without stopping the task. | `src/ui/save_menu.c` |
| `Func_801c458` | `0x0801c458` | `Menu3_Select` | read | Save menu | Forwards the selection to the party layer and returns 0. | `src/ui/save_menu.c` |
| `Func_801c924` | `0x0801c924` | `Menu3_Refresh` | read | Save menu | A pure forward to the save menu's redraw. | `src/ui/save_menu.c` |
| `Func_801c930` | `0x0801c930` | `Menu3_AllocBlock` | read | Save menu | Reserves the 0x1004-byte EWRAM block under tag 0x13 and zeroes its header. | `src/ui/save_menu.c` |
| `Func_801c9bc` | `0x0801c9bc` | `NullSub_801c9bc` | read | Save menu | Empty body. | `src/ui/save_menu.c` |
| `Func_801c9c8` | `0x0801c9c8` | `Menu3_Tick` | read | Save menu | The save menu's per-frame update over its 0x400-byte slot array. | `src/ui/save_menu.c` |

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

## Start menu — 5 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8021360` | `0x08021360` | `GetStartMenuEntry` | read | Start menu | Returns an entry from .L37206 or .L37216 depending on a flag, bounded at index 8. | `src/ui/start_menu.c` |
| `Func_8021750` | `0x08021750` | `StartMenu_AddIconOption` | read+callee | Start menu | Allocates a sprite for an option's icon and adds the option to the start menu. | `src/ui/start_menu.c` |
| `Func_8021848` | `0x08021848` | `StartMenu_ResetOptions` | read | Start menu | Clears the option table at .L37250 through the RAM-resident fill. | `src/ui/start_menu.c` |
| `Func_80219c8` | `0x080219c8` | `StartMenu_DrawOption` | read | Start menu | Draws one start-menu option using the layout table at .L37280. | `src/ui/start_menu.c` |
| `Func_8021a18` | `0x08021a18` | `StartMenu_BuildPalette` | read | Start menu | Expands the packed palette at .L372c0 into the caller's buffer. | `src/ui/start_menu.c` |

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

## UI panels — 8 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_801c0c8` | `0x0801c0c8` | `NullSub_801c0c8` | read | UI panels | Empty body; one of five consecutive empty subs in this file. | `src/ui/panel.c` |
| `Func_801c17c` | `0x0801c17c` | `UI_FreeTiles` | read | UI panels | A pure forward to the OBJ tile free routine. | `src/ui/panel.c` |
| `Func_801c188` | `0x0801c188` | `UI_CreatePanel` | read | UI panels | Allocates a panel's handle, tiles and graphics and fills in its record. | `src/ui/panel.c` |
| `Func_801c21c` | `0x0801c21c` | `UI_ReleasePanelTiles` | read | UI panels | Releases a panel's OBJ tile allocation. | `src/ui/panel.c` |
| `Func_801c2d0` | `0x0801c2d0` | `UI_StepFade` | read | UI panels | Advances the fade by one step and waits a frame. | `src/ui/panel.c` |
| `Func_801c2e4` | `0x0801c2e4` | `UI_Refresh` | read | UI panels | A pure forward to the panel refresh routine. | `src/ui/panel.c` |
| `Func_801ff14` | `0x0801ff14` | `Menu5_Stop` | read | UI panels | Stops the panel task and deletes its sprites. | `src/ui/panel.c` |
| `Func_8020088` | `0x08020088` | `Menu5_Stop2` | read+family | UI panels | The same teardown against a different task and sprite set. | `src/ui/panel.c` |

## UI sprites — 15 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_801eb64` | `0x0801eb64` | `UI_CreateIconSprite` | read+callee | UI sprites | Uploads an icon and registers it as a sprite, returning zero if the upload fails. First of five near-identical creators differing only in which loader they call. | `src/ui/ui_sprite.c` |
| `Func_801eb90` | `0x0801eb90` | `UI_CreateItemIconSprite` | read+family | UI sprites | The LoadInventoryIcon member of the UI_Create*Sprite family. | `src/ui/ui_sprite.c` |
| `Func_801ebd8` | `0x0801ebd8` | `UI_CreateOldIconSprite` | read+family | UI sprites | The LoadOldUIIcon member of the family; it also clears the byte at +0xf of the result. | `src/ui/ui_sprite.c` |
| `Func_801ec24` | `0x0801ec24` | `UI_CreateBannerSprite` | read+family | UI sprites | The LoadUIBanner member of the family. | `src/ui/ui_sprite.c` |
| `Func_801edcc` | `0x0801edcc` | `UI_ResetSpriteRec` | read | UI sprites | Zeroes a UI sprite record's fields, if the record exists. | `src/ui/ui_sprite.c` |
| `Func_801eddc` | `0x0801eddc` | `UI_SetSpritePriority` | read | UI sprites | Writes the complement of the argument into the priority byte at +0xf. | `src/ui/ui_sprite.c` |
| `Func_80209b0` | `0x080209b0` | `UI_CreateCursorSprite` | read | UI sprites | Allocates a sprite slot and uploads the cursor sheet at Data_310a4. | `src/ui/ui_sprite.c` |
| `Func_8020aec` | `0x08020aec` | `UI_UploadCursorGFX` | read+family | UI sprites | Uploads 0x80 bytes of cursor graphics from Data_310a4 to a slot. | `src/ui/ui_sprite.c` |
| `Func_8020b00` | `0x08020b00` | `UI_UploadArrowGFX` | read+family | UI sprites | The Data_317e4 twin of UI_UploadCursorGFX. | `src/ui/ui_sprite.c` |
| `Func_80215e0` | `0x080215e0` | `UI_LoadIconSheet` | read | UI sprites | Allocates scratch, LZ-decompresses an icon sheet from Data_31864 into it, uploads it and frees the scratch. | `src/ui/ui_sprite.c` |
| `Func_8021ab0` | `0x08021ab0` | `UI_LoadStatusIcon` | read+family | UI sprites | Decompresses a status icon into scratch, uploads it to a slot and frees the scratch. One of three identical loaders differing only in the decompressor. | `src/ui/ui_sprite.c` |
| `Func_8021af0` | `0x08021af0` | `UI_LoadInventoryIcon` | read+family | UI sprites | The inventory-icon member of that loader trio. | `src/ui/ui_sprite.c` |
| `Func_8021b30` | `0x08021b30` | `UI_LoadMoveIcon` | read+family | UI sprites | The move-icon member of that loader trio. | `src/ui/ui_sprite.c` |
| `Func_8021c64` | `0x08021c64` | `UI_LoadFontSheet` | read | UI sprites | Loads file _FILE_f1, decompresses it into IWRAM scratch and uploads it as sprite graphics. | `src/ui/ui_sprite.c` |
| `Func_8022a38` | `0x08022a38` | `UI_CreateMoveIconSprite` | read+family | UI sprites | Allocates a slot, loads a move icon into it and registers the sprite. | `src/ui/ui_sprite.c` |

## UI text — 18 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8017a64` | `0x08017a64` | `GetStringWidth` | read | UI text | Measures a string in pixels using the width table at Data_32224. | `src/ui/text.c` |
| `Func_8018790` | `0x08018790` | `UI_DrawBufferedString` | read+callee | UI text | Buffers a string id and draws the result at the given position. | `src/ui/text.c` |
| `Func_801999c` | `0x0801999c` | `UI_PollTextAdvanceHeld` | read+family | UI text | Text advance on a held key, gated on the music fade state. Paired with the press variant next door. | `src/ui/text.c` |
| `Func_80199ec` | `0x080199ec` | `UI_PollTextAdvancePress` | read+family | UI text | The gKeyPress twin of UI_PollTextAdvanceHeld. | `src/ui/text.c` |
| `Func_8019ba0` | `0x08019ba0` | `BufferStringLine` | read+callee | UI text | Buffers a string id with the line flag set. | `src/ui/text.c` |
| `Func_801c46c` | `0x0801c46c` | `SetTextSpeed` | read | UI text | Writes the text-speed byte at gState+0x205 from a flag word. | `src/ui/text.c` |
| `Func_801ca1c` | `0x0801ca1c` | `BuildPartyStatusList` | read | UI text | Builds a per-member status list into the caller's buffer from gState and the table at .L36750. | `src/ui/text.c` |
| `Func_801cae0` | `0x0801cae0` | `SetTextPalette` | read+callee | UI text | Writes four blended entries into the text palette at 0x50001e8 through Func_801cbd4. | `src/ui/text.c` |
| `Func_801cbd4` | `0x0801cbd4` | `BlendPaletteEntry` | read | UI text | Blends two colour masks through the ARM helper Func_8000888 and returns the result; SetTextPalette calls it four times. | `src/ui/text.c` |
| `Func_801e7c0` | `0x0801e7c0` | `UI_DrawMessage` | read | UI text | Draws a message id into a box at the given offset, using the box's stored size. | `src/ui/text.c` |
| `Func_801e858` | `0x0801e858` | `UI_DrawStringTemp` | read | UI text | Allocates a scratch buffer, renders a string into it, draws it and frees the buffer. | `src/ui/text.c` |
| `Func_801e8b0` | `0x0801e8b0` | `UI_DrawStringInWindow` | read+family | UI text | The window-targeted variant of UI_DrawStringTemp. | `src/ui/text.c` |
| `Func_801e9a0` | `0x0801e9a0` | `UI_DrawNumber` | read+callee | UI text | Formats a number into a sixteen-byte stack buffer with PrintNum and draws it. | `src/ui/text.c` |
| `Func_801e9d4` | `0x0801e9d4` | `UI_DrawNumberInWindow` | read+family | UI text | The window-targeted member of the UI_DrawNumber trio. | `src/ui/text.c` |
| `Func_801ea08` | `0x0801ea08` | `UI_DrawNumberText` | read+family | UI text | The third member, drawing through UIDrawText rather than the window path. | `src/ui/text.c` |
| `Func_8020b14` | `0x08020b14` | `UI_DrawMenuLabel` | read | UI text | Draws one menu label string into the UI block's current window. | `src/ui/text.c` |
| `Func_8021e48` | `0x08021e48` | `UI_ShowTextAndWait` | read+callee | UI text | Opens a text box and spins until the text has finished printing. | `src/ui/text.c` |
| `Func_80228bc` | `0x080228bc` | `UI_FormatNumberString` | read+callee | UI text | Renders a number into gStringBuffer with FormatDecimalString and copies it out as halfwords. | `src/ui/text.c` |

## UI windows — 47 functions

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_8015ec0` | `0x08015ec0` | `UI_UnlinkWindow` | read | UI windows | Removes a window record from the window list held in the UI block at iwram_3001e8c. | `src/ui/window.c` |
| `Func_8015ef4` | `0x08015ef4` | `UI_LinkWindow` | read | UI windows | The insert half of UI_UnlinkWindow, in the same file. | `src/ui/window.c` |
| `Func_8016230` | `0x08016230` | `UI_ClearWindow` | read | UI windows | Clears a window's tile and attribute buffers and re-runs its two layout hooks. | `src/ui/window.c` |
| `Func_8016478` | `0x08016478` | `UI_CloseAndFree` | read+callee | UI windows | Redraws a window's rect one last time and then releases its chain. | `src/ui/window.c` |
| `Func_8016498` | `0x08016498` | `UI_RedrawWindowRect` | read | UI windows | Re-runs the tile fill over a window's stored rect, reading position and size from +8 through +0xe. | `src/ui/window.c` |
| `Func_80164ac` | `0x080164ac` | `UI_FreeWindowChain` | read+callee | UI windows | Walks a window's child chain releasing each in turn. | `src/ui/window.c` |
| `Func_801656c` | `0x0801656c` | `UI_ListTail` | read | UI windows | Follows a singly-linked list to its last node and returns it. | `src/ui/window.c` |
| `Func_8016584` | `0x08016584` | `UI_ListAppend` | read | UI windows | Appends a node using the tail pointer cached at +4, updating both. | `src/ui/window.c` |
| `Func_8016594` | `0x08016594` | `UI_ReleaseWindow` | read+callee | UI windows | Unlinks a window and frees its OBJ tile allocation if it holds one. | `src/ui/window.c` |
| `Func_801671c` | `0x0801671c` | `UI_ClearTextLayer` | read | UI windows | Fills the 0xf00-byte text map at 0x6002500 with zero. | `src/ui/text.c` |
| `Func_8016738` | `0x08016738` | `UI_FillTextLayer4` | read+family | UI windows | The same fill with 0x44444444 -- palette index 4 across the layer. | `src/ui/text.c` |
| `Func_8016758` | `0x08016758` | `UI_ResetTextLayer` | read+callee | UI windows | Finds the text window record and clears its layer. | `src/ui/text.c` |
| `Func_80167ac` | `0x080167ac` | `UI_CopyWindowRect` | read | UI windows | Copies the three rect halfwords from one window record to another. | `src/ui/window.c` |
| `Func_80167d8` | `0x080167d8` | `UI_SetWindowState2` | read | UI windows | Writes state 2 into the halfword at +0x1c of a window record. | `src/ui/window.c` |
| `Func_8016868` | `0x08016868` | `UI_UpdateMessage` | read | UI windows | Advances a message record: consumes pending characters and updates its flag word. | `src/ui/text.c` |
| `Func_8017004` | `0x08017004` | `UI_UpdateWindowAnim` | read | UI windows | Steps a window's open/close animation through its three size halfwords. | `src/ui/window.c` |
| `Func_8017364` | `0x08017364` | `UI_IsTextDone` | read | UI windows | Reports whether the text object at UI block +0x620 has finished printing. | `src/ui/text.c` |
| `Func_8017394` | `0x08017394` | `UI_IsBoxIdle` | read | UI windows | True when a box's two pending counters at +0x16 and +0x1a are both zero. | `src/ui/window.c` |
| `Func_80173ac` | `0x080173ac` | `UI_TickPanel` | read | UI windows | The per-frame panel update over the UI block's +0xea8 field group. | `src/ui/window.c` |
| `Func_8017464` | `0x08017464` | `UI_StartPanelTask` | read | UI windows | Uploads the panel sprite sheet and starts Func_801789c as its task. | `src/ui/window.c` |
| `Func_80174d8` | `0x080174d8` | `UI_CloseActiveBox` | read | UI windows | Closes the box held in the first slot of the block at iwram_3001ee4. | `src/ui/window.c` |
| `Func_80175a0` | `0x080175a0` | `UI_PrintAndWait` | read+callee | UI windows | Prints the queued battle text and spins a frame at a time until UI_IsTextDone agrees. | `src/ui/text.c` |
| `Func_8017620` | `0x08017620` | `UI_SetTextFlags` | read | UI windows | Sets the text control bytes at UI block +0x12fa from the caller's flag bits. | `src/ui/text.c` |
| `Func_801789c` | `0x0801789c` | `UI_TickAll` | read+callee | UI windows | The UI task body: runs the window, message and panel updates in order. | `src/ui/window.c` |
| `Func_8019000` | `0x08019000` | `UI_DrawWindowTiles` | read | UI windows | Writes a window's border and fill tiles from its size and palette fields. | `src/ui/window.c` |
| `Func_80197b4` | `0x080197b4` | `UI_ClearHandle` | read | UI windows | Zeroes a handle word if it is set, returning the pointer. | `src/ui/window.c` |
| `Func_80197c4` | `0x080197c4` | `UI_CloseAllBoxes` | read+callee | UI windows | Closes every open box in the UI block and waits out the close animation. | `src/ui/window.c` |
| `Func_8019854` | `0x08019854` | `UI_SetBoxRect` | read | UI windows | Writes a box's four rect halfwords at +8 through +0xe. | `src/ui/window.c` |
| `Func_8019a54` | `0x08019a54` | `UI_StepBoxScroll` | read | UI windows | Advances a box's scroll counters at +0x14/+0x16 toward the target at +0x18. | `src/ui/window.c` |
| `Func_8019e48` | `0x08019e48` | `UI_ClosePortrait` | read+callee | UI windows | Closes the portrait box currently held by the UI block. | `src/ui/window.c` |
| `Func_801a5a0` | `0x0801a5a0` | `NullSub_801a5a0` | read | UI windows | Empty body. | `src/ui/window.c` |
| `Func_801ce6c` | `0x0801ce6c` | `Menu_TickBlinkCounter` | read | UI windows | Advances the blink counter at +0x574 and wraps it past 0x20000. | `src/ui/window.c` |
| `Func_801cf44` | `0x0801cf44` | `NullSub_801cf44` | read | UI windows | Empty body. | `src/ui/window.c` |
| `Func_801e260` | `0x0801e260` | `UI_FillRect` | read | UI windows | Fills a rectangle of the window map with a constant, row by row. | `src/ui/window.c` |
| `Func_801e418` | `0x0801e418` | `NullSub_801e418` | read | UI windows | Empty body. | `src/ui/window.c` |
| `Func_801ee68` | `0x0801ee68` | `UI_FillMapRect` | read | UI windows | Fills a halfword rectangle of a tilemap with a constant, taking the stride from the caller. | `src/ui/window.c` |
| `Func_801eea0` | `0x0801eea0` | `UI_BuildPartyRows` | read+callee | UI windows | Builds one status row per living party member, sized by GetPartySize. | `src/ui/window.c` |
| `Func_801ef08` | `0x0801ef08` | `UI_StepBoxAnim` | read | UI windows | Advances a box's open/close animation by one frame through its five leading fields. | `src/ui/window.c` |
| `Func_801f5d4` | `0x0801f5d4` | `UI_CloseStatusBox` | read | UI windows | Closes the status box held at iwram_3001e90 and releases its block. | `src/ui/window.c` |
| `Func_801f5f0` | `0x0801f5f0` | `UI_DrawStatusRow` | read | UI windows | Draws one party status row into its window from the record's size fields. | `src/ui/window.c` |
| `Func_801fda8` | `0x0801fda8` | `UI_DrawBoxFrame` | read | UI windows | Draws a box's border tiles around the given rect. | `src/ui/window.c` |
| `Func_8020a60` | `0x08020a60` | `UI_DrawWinContents` | read | UI windows | Draws a window's interior from its size halfwords. | `src/ui/window.c` |
| `Func_8021bc8` | `0x08021bc8` | `GetDefaultUIPalette` | read | UI windows | Returns Data_73968[0]; any non-zero index is forced to zero, so the table has one live entry. | `src/ui/window.c` |
| `Func_8021c34` | `0x08021c34` | `UI_CreateDebugBox` | read | UI windows | Creates a UI box and draws the fixed string at .L37300 into it. | `src/ui/window.c` |
| `Func_8021dfc` | `0x08021dfc` | `UI_SetBG1Priority3` | read+family | UI windows | Sets BG1's priority bits to 3. | `src/ui/window.c` |
| `Func_8021e14` | `0x08021e14` | `UI_ClearBG1Priority` | read+family | UI windows | Clears BG1's priority bits. | `src/ui/window.c` |
| `Func_8021e28` | `0x08021e28` | `UI_ResetBG0AndIntr` | read | UI windows | Zeroes BG0VOFS and reinstalls the interrupt handler. | `src/ui/window.c` |

