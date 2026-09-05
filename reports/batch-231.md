# Batch 231 — a wall that hid 230 functions, and a scheduler tie decided by alias sets

Seven functions. Two of them cost almost nothing once a boundary in the method
doc turned out to be a boundary on a single lever rather than on the shape it
described.

## Elevated

| # | function | address | file |
|---|---|---|---|
| 1 | `OvlFunc_883_200b2b0` | `0x0200b2b0` | [ovl_30_…_a_c.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_c.c) |
| 2 | `OvlFunc_883_200b380` | `0x0200b380` | [ovl_30_…_a_c.c](src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_c.c) |
| 3 | `OvlFunc_957_200ac44` | `0x0200ac44` | [ovl_30_…_a_b.c](src/overlays/rom_7e3e08/ovl_30_c_c_c_a_a_a_b.c) |
| 4 | `OvlFunc_967_2008508` | `0x02008508` | [ovl_30_…_c_a.c](src/overlays/rom_7f21b8/ovl_30_c_c_c_c_c_a.c) |
| 5 | `OvlFunc_884_20084d4` | `0x020084d4` | [ovl_30_…_a_c.c](src/overlays/rom_784360/ovl_30_c_a_a_a_c_c_a_c_a_c.c) |
| 6 | `OvlFunc_943_2009c14` | `0x02009c14` | [ovl_30_…_a_c.c](src/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_c.c) |
| 7 | `OvlFunc_954_2008270` | `0x02008270` | [ovl_30_…_a_b.c](src/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_b.c) |

## The wall hid 230 functions

Batch 230 reopened the "zero interleaved into a shifted build" population by a
single counterexample and said, deliberately, that one counterexample is not a
cure. Re-derived properly, it is much larger than that:

| | count |
|---|---|
| carry the interleave shape | 546 |
| guarded sites only — the branch cure applies | 314 |
| straight-line only — the recorded "98" | **83** |
| **mixed: at least one straight-line site** | **147** |

`tools/guarded_interleave.py` selects on `g and not u`, so the **147 mixed**
functions were excluded as well — one straight-line site was enough to wall off
an entire function. **83 + 147 = 230** sat behind a boundary that only ever
applied to one lever.

The re-screen also **reused that existing tool** rather than writing a detector,
importing its `sites()` verbatim to report the complement it discards. After
last batch's duplicate-tool episode that was the whole point.

**And the cure is simpler than I recorded it.** I wrote it up as a pinned
whole-value fill with the zero *hand-placed first*. That is a special case. The
general form is the uniform whole-value ascending fill — one statement per
argument, in order, shifted constants written whole rather than as a `mov`+`lsl`
pair. Hand-placing the zero matters only when other work is scheduled into the
block.

The two cheapest cases prove it. `OvlFunc_943_2009c14` has **no conditional
branch at all** and five interleave sites — exact on the first screen.
`OvlFunc_954_2008270` closes the `neg` variant straight-line **from a bare
call, with no pins whatsoever**.

Refinement worth keeping: pin the single-instruction arguments and leave the
split build bare. Dropping the split build's own pin measured inert everywhere;
dropping the interleaved argument's pin costs 2–3. Pin size stays per-site and
unpredictable — two sites of identical shape in one function wanted two pins and
one.

## A scheduler tie decided by alias sets

`OvlFunc_883_200b2b0` and `_200b380` were solved **independently by two agents
that converged on the same mechanism from different directions**. Their two
candidates for `200b380` are different source producing identical objects.

Both came down to one `mov` wedged between two independent byte stores. Read out
of `haifa-sched.c` rather than inferred: `rank_for_schedule` compares
`INSN_PRIORITY` first — all four candidates tie at 13 — and then applies a
**class test the doc did not record**, sitting *before* the known dependent-count
tie-break:

```
1  data dependence on the last-scheduled insn
2  anti- or OUTPUT dependence
3  independent, or latency 1        (highest class wins)
```

Through `unsigned char *` both stores share **alias set 0**, so the second
carries an output dependence on the first, lands in class 2, and loses outright
to the independent `mov` in class 3 — before its nine dependents against three
would have won it the slot.

gcc-2.96 gives a distinct alias set per struct tag, so reaching the two bytes
through two tags removes the dependence. **The new part is that it takes both
sides**: casting either store back regresses (2 and 3 differing). So it is
neither the recorded "give the store a `char` lvalue" lever — which uses set 0
to *pin* a store, the opposite direction — nor "name one field."

It also made three register pins evaporate. Before the alias reading they took a
screen 8 → 2, with *any two of the three* working, which is exactly the profile
of a real lever. With the struct tags in, all three measure **exactly zero** and
none ships. That is "a pin can be a symptom of a different defect" at its
sharpest.

**Standing hazard:** this TU must never fall under an `-fno-strict-aliasing`
rule, which destroys precisely the separation the match depends on.
`ALIAS_CFLAGS` already exists in the Makefile for the opposite case — the two
levers are live in the same tree and pull opposite ways.

## Uniform fill is correct, not merely cheaper

`OvlFunc_957_200ac44` measured **71 differing against 0** for transcribing the
ROM's emitted order into all 51 pinned fills.

The screening note called this the first case where transcription costs
anything. It is not — "DO NOT TRANSCRIBE THE ROM'S SHIFT ORDER" already records
transcription producing actively wrong output. What is new is **scale**: the
neighbouring entry records uniform measuring *byte-identical* to transcription
on two functions, so the pair together said "usually identical, occasionally
wrong." This is the first whole-file measurement where transcription is
systematically worse — not a per-site accident.

The procedure does not change. The reason does: uniform is not merely cheaper to
write, it is the form that is usually right.

## The length tell now has three outcomes

`200ac44` came out **shorter** than the ROM in plain C — 2156 bytes against 2168
— because removed rematerialisations outweighed the widened push. On
`OvlFunc_962_2008240` the two cancelled exactly; on `OvlFunc_965_2009238` and
`967_2008508` it fired long as documented.

Longer, equal, and shorter are all now on record for the same tell. Reading the
diff text and counting `mov rN, r5..r11` copies by destination named all eleven
held values in `200ac44` directly, and survives all three cases.

## `__Func_8092c40` gets a corpus, and a refuted hypothesis

The descending-fill tell has been a run of seven functions. It now has a count:
of **324** `bl __Func_8092c40` sites in `asm/`, **284 are r1-first** — 88%.

Better, the screening run formed the *opposite* hypothesis from its own
r0-first pooled site, tested it against the corpus, and had it refuted: a pooled
first argument does not predict the order, since 25 of 27 pooled-r0 sites are
still r1-first. That is the right way round.

It also found that **the descending fill and the no-prototype lever are the same
lever**, measuring byte-identical. Both stop the prototype forcing r0 to be
evaluated first, which explains why the tell reads as binary rather than graded.
The doc records the two separately and never connects them. The prototyped form
shipped, keeping type-checking on all 31 callees.

## Housekeeping

`asm/overlays/rom_7f21b8/ovl_30_c_c_c_c_c_b.s` was a **compiler-generated `.s`
that had been committed**, against the CLAUDE.md rule, with its `.c` already
present. It was found by a screening run reading its own overlay, not by the
guard — `guard_generated.sh` only inspects what a commit *stages*, so a
violation already in history is invisible to it. Dropped from tracking.

`OvlFunc_965_2009238` is verified byte-exact (2264 bytes, 866 encodings) but is
**not landed**: the `rom_7ef4f4/ovl_30_a_c_c_c_c_c%` wildcard applies `-O1` and
the same source measures 286 differing there against byte-identical at `-O2`. It
would be the *fourth* explicit override in that one directory, so narrowing the
pattern at source is the better fix and is left for the next batch rather than
bolted on here.

One process note from the re-screen: two of its first picks were elevated by a
concurrent session while it worked them. The cause is structural — the scan
excludes a function by testing whether its `src/**.c` exists, and the `.s` stays
in `asm/` until landing, so a solved function looks unsolved for the whole
window. Re-run the scan immediately before committing to a target.
