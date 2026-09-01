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

Covering **18 of 1341** elevated functions (1%).

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

## Town UI — 1 function

| Function | Address | Proposed | Basis | ROM area | Why the name | Suggested home |
|---|---|---|---|---|---|---|
| `Func_80b06ec` | `0x080b06ec` | `UI_DrawIconRow` | read | Town UI | Draws four 2x2-tile icons across a 30-column tilemap: 32-byte source rows at .Lb3d40 + a*32, destination from .Lb413c[c], writing +0/+1/+30/+31 and advancing by four. A zero byte blanks the rest of that icon. | `src/menu/shop_ui.c` |

