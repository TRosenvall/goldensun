# Batch 307 brief D -- the three targets dropped mid-brief, and what the screen actually says

The brief was amended mid-flight to drop three of four targets as "structurally blocked by
INLINE `.call_via` sites", citing `docs/elevation.md:1473`. **That section is retracted at
`docs/elevation.md:5482` and was never struck.** Below is what I measured.

## The screen, re-run correctly

The amendment's recipe

    awk '/func_start NAME/,/func_end/' <ref.s> | grep -cE '^\s*\.call_via'

is not a valid awk program once `NAME` and the path are interpolated by zsh -- the shell
splits the pattern and awk bails with a syntax error on every invocation, printing `0`
after the error. That is a sixth instance of the word-splitting trap HANDOFF.md already
records. A form that works:

    awk -v f=NAME '$0 ~ ("\\.thumb_func_start "f"([^A-Za-z0-9_]|$)"){p=1} \
                   p&&/^\.func_end/{print "";p=0} p' <ref.s> | grep -cE '^\s*\.call_via'

| function | ref .s | inline `.call_via` sites | amendment said |
|---|---|---|---|
| `ActorCmd_Player` | asm/rom_9000/rom_ebec_a.s | **2** | 4 |
| `ActorCmd_Player_World` | asm/rom_9000/rom_ebec_a.s | **2** | (not screened) |
| `UpdateActors` | asm/rom_9000/rom_ca6c_a_c_c.s | **31** | 31 |
| `Func_8090a5c` | asm/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c_a_c_c.s | **3** | 3 |
| `BattleMain` | asm/rom_b5000/rom_b5a0c_c_c_a_a_a_c_a.s | **0** | 0 |

## Inline `.call_via` is NOT a blocker, and this batch re-proved it

1. `docs/elevation.md:5482` -- "RETRACTED: `.call_via rN` is a hard wall. It is reachable,
   and 51 functions were written off" -- names `Func_8097a10` as **elevated and
   byte-exact** on a `static inline` asm helper, and gives the helper verbatim at :5505.
2. **I compiled it.** `scratch_elev/b307d/p4.c` (the ActorCmd_Player candidate) emits
   `mov r12, pc / bx r4` TWICE, at the two offsets where the ROM has `.call_via r4`, and
   aligncmp scores both pairs ALIGNED-EQUAL. gcc-2.96 did emit the veneer.
3. `ActorCmd_Player_World` -- parked in batch 306 at 86.5% aligned with exactly this
   helper -- has the SAME two sites. If inline sites blocked a function, that park is void.

**So elevation.md:1473 needs striking in place.** It is batch 302's rule a third time: a
correction that leaves the original claim standing has not landed, and a reader who reaches
:1473 first gets the write-off. The section even lists the same functions by name and
site count, which is how the amendment reproduced it exactly.

`ActorCmd_Player` was therefore NOT abandoned; it is parked at
`scratch_elev/b307d/PARK_ActorCmd_Player.c`, 62.8% aligned in 130 hunks.

## The two I did not attempt, and why

Neither is *blocked*, but neither is cheap, and I had budget for two functions:

* **`UpdateActors`** (683 insns, 31 inline sites). Thirty-one helper call sites is the real
  cost, not a wall: each needs the pinned-register helper and the ROM's `bx` register read
  off the site, and `src/lib/call_via.s`' veneer-name rule (elevation.md:5528) applies per
  register. One `fakematch.txt` row would cover the file. Worth assigning to a brief whose
  whole budget is this one function.
* **`Func_8090a5c`** (806 insns, 3 inline sites). Three sites is the same shape as
  `ActorCmd_Player`'s two, so the twin's helper transfers directly. This is the cheaper of
  the two and the better next assignment.

Do not re-screen either as blocked.
