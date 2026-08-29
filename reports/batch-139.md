# Batch 139 — a documented floor was not a floor

Verified on a clean `make clean && make compare` — `goldensun.gba` SHA1
`5c4695205413df7db52b9a184815a07783999971`, all 96 overlays comparing — with
every address below read out of the linked overlay ELFs.

**remaining 2219 · elevated 3198 · parked 321**

## Elevated (6)

| function | address | ELF | what it took |
|---|---|---|---|
| `OvlFunc_937_200807c` | `0200807c` | rom_7c3044 | switch + `_AREA_64/65` |
| `OvlFunc_937_20080e4` | `020080e4` | rom_7c3044 | same; its park is deleted |
| `OvlFunc_899_2008048` | `02008048` | rom_794ac0 | switch; park deleted |
| `OvlFunc_899_2008310` | `02008310` | rom_794ac0 | switch; park deleted |
| `OvlFunc_895_200807c` | `0200807c` | rom_78dee8 | switch + `_AREA_10/13` |
| `OvlFunc_895_20080ec` | `020080ec` | rom_78dee8 | same |

## The finding: use a switch, not an expression

Three park notes and `docs/elevation.md` all recorded the same wall. gcc-2.96
canonicalises every signed lower bound to `cmp #(K-1) / ble` where the ROM has
`cmp #K / blt`. One park called it *"the cleanest example so far because every
other difference is gone"*; the document's stated prediction was that any
function whose only remaining difference is a lower bound *"has a 2-line floor
and is not worth another round"*.

Every spelling that had been tried was an **expression** — `v < K`, `v <= K-1`,
an inverted `v >= K`, `!(v >= K)`, `int` against `short` operands. The lever is
a different **statement**: gcc lowers a `switch` through a path that emits the
bound test directly and never canonicalises it.

```c
switch (e) {
case 9: case 0xa: case 0xb: case 0xc:
case 0xd: case 0xe: case 0xf: case 0x11:
    r = table_a; break;
default:
    r = table_b; break;
}
```

**The counterexample was already in the tree and cost one grep.** Fifteen sites
where already-matching C emits `cmp #K / blt`. Fourteen are `cmp #0`, which
cannot canonicalise because `-1` is not an encodable immediate — those explain
nothing. The fifteenth is `cmp #31 / blt` in `src/rom_b5000/rom_bb588_c_c_b.c`,
from a switch.

**Scope, measured and then narrowed.** 242 `cmp #K / blt` sites sit in 158
unelevated functions, but that is the population where this *can* apply, not a
worklist. Of the 12 parks mentioning a lower bound, exactly one was really on
this floor. `OvlFunc_965_200a6fc` compares two registers, so a switch cannot
express it at all. Only 4 of the 158 are unparked single-function files. The
precondition is a constant discriminant over a range small enough to enumerate.

`Func_80a3ce4` — the function `docs/elevation.md` used to *state* the floor —
also screens OK as a switch. It stays parked only because its `.s` holds four
functions and there is nowhere to put it yet, and its note now says exactly
that so nobody re-derives the floor from it.

## One unreachable function was holding two reachable ones

`OvlFunc_895_2008154` shares its `.s` with the two `895` selectors above and
does not match. `tools/split_s.py` separated them; the split was verified
byte-neutral with a green `make compare` **before** any `.c` was written, which
is what that tool's own instructions ask for and which keeps a layout mistake
from hiding behind a decompilation mistake.

That function is parked as ARGUMENT PRECOMPUTE, and this time the class's
predictive rule was applied *before* screening rather than rediscovered after.
`MapActor_SetSpeed(0, 0x9999, 0x4ccc)` mixes a cheap zero with two pool loads
and puts the cheap one first, which HANDOFF.md says will misorder. It does,
5 of 64. Nothing else was tried.

## Parked (2)

`OvlFunc_895_2008154` — argument precompute, 5 of 64, as above.

`OvlFunc_926_2008388` — CONSTANT CSE ACROSS A CALL, 25 of 60, and worth reading
because it is a **counterexample to a documented remedy**. `docs/elevation.md`
says recovering the ROM's reload needs both a control-flow boundary between the
two uses and `-fno-rerun-cse-after-loop`. Both preconditions hold here — the
first use dominates the second and a `beq` separates them — and the flag does
not move it, nor do `-fno-gcse`, `-fno-cse-follow-jumps`,
`-fno-expensive-optimizations` or `-fno-force-mem`. So that rule states a
necessary condition, not a sufficient one, and `blocked_cse.py`'s 585-function
count is a population rather than a worklist.

Ruled out explicitly: that the stored `0x895` is a symbol, which would explain
the ROM's two pool entries naturally. It is in none of `area.sym`,
`message.sym`, `const.sym` or `file_table.sym`. If a save-bit namespace ever
gets a `.sym` file, this is the first function to re-try.

## Method notes

The `_AREA_*` technique carried four of the six elevations. That is HANDOFF's
"209 functions are elevatable NOW" pool, and these were the first rounds to
draw on it; 50 such functions remain listed by `tools/sym_candidates.py`.

A counting slip was caught by tooling rather than by reading: after one build,
`elevated` had risen by one where three functions were expected, because a
function that screened OK was never placed. `tools/remaining.py` counts files,
so the gap surfaced immediately. Screening a function and elevating it are
separate acts and the count is what tells them apart.

**The general lesson.** *"Every spelling I tried gives the same output"* is
evidence about the spellings tried, not about the compiler. When a class looks
like a floor, ask which construct has not been tried, and check whether the
matching corpus already contains the instruction that is supposedly
unreachable. That check is cheap, and it is what broke this one.
