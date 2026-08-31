# Batch 161

Five elevated. Verified after a clean `make clean && make compare`; SHA1
`5c4695205413df7db52b9a184815a07783999971`. Every address checked against the
linked images.

| function | address | image |
| --- | --- | --- |
| OvlFunc_913_2008a68 | 0x02008a68 | overlays/rom_7a04ac/overlay.elf |
| OvlFunc_959_2008bec | 0x02008bec | overlays/rom_7e7574/overlay.elf |
| OvlFunc_922_2009d78 | 0x02009d78 | overlays/rom_7a8c8c/overlay.elf |
| OvlFunc_948_2008f40 | 0x02008f40 | overlays/rom_7d30e0/overlay.elf |
| OvlFunc_948_2008fdc | 0x02008fdc | overlays/rom_7d30e0/overlay.elf |

## An eleven-function park was a stale claim, and re-testing it produced the batch

`src/non_matching/ovl_7c460c/2008c74.c` held eleven functions sharing one
two-line residue, with eleven failed spellings, three failed flags, and a corpus
result of "zero of 2987 generated .s files contain this shape". A later batch had
flagged that zero as *probably* a tab-versus-space regex artifact and asked for
the scan to be re-run with `\s+` before it was cited again. Nobody had.

Re-run correctly over the 3336 SOLVED `.s` files, the shape appears at **13 sites
in 13 already-matching files**. And one of the 13 is `OvlFunc_965_2009030` --
**a member of that family's own list**, solved at some point with the family note
never updated.

Reading it gives the construct: name every split two-instruction build as a local
in the function's ENTRY BLOCK, which dominates the call sites. Counting
conditional branches before the `neg` site across all eleven members settles the
family completely:

    OvlFunc_965_2009030   SOLVED -- site inside two nested guards
    the other ten         0 guards before the site

Every unsolved member is straight-line at its site; the solved one is not. That
also explains the single anomalous measurement in the park -- naming the constant
at the top scored 43 instead of 2 -- because with no guard to cross the local
stays live and costs a register, which is what the argument-order section already
records for straight-line functions. **The lever was being applied without its
precondition.**

So the family is not its own blocker class. It is the straight-line half of the
argument-interleave class, already sized at 98 functions and already known to be
out of reach. Eleven functions did not need eleven more spellings.

Six of the eleven paths in that list were also stale -- the `.s` files had been
split since -- so family members have to be resolved by NAME, never by path.

**The generalisable rule:** a park that lists members and a shared residue is
claiming those members are equivalent. Before adding a twelfth spelling, check
whether any member has been solved since, and re-run any corpus result the park
itself flags as doubtful. One scan retired a family that had absorbed rounds.

## `tools/guarded_interleave.py`, and four elevations from it

The finding above is only useful as a selector, so it became one. The tool splits
the interleave population tree-wide into sites that are dominated by a conditional
branch (the lever works) and sites that are straight-line (nothing reaches them),
and reports only functions where EVERY site is guarded. 81-83 unparked functions
qualify.

Four of this batch's five came from it, and the trajectory is the point:

  * `OvlFunc_959_2008bec` -- 2 of 59 on the first attempt, then matched.
  * `OvlFunc_922_2009d78` -- matched after one unrelated fix.
  * `OvlFunc_948_2008f40` and `OvlFunc_948_2008fdc` -- **both matched on the
    first screen, with no iteration at all**, and since their `.s` held exactly
    those two functions the whole translation unit converted with no split.

The recipe, applied without variation: name the two-instruction builds AND the
pool load in the dominating block, name both stack arguments per call site, let
the guard do the work.

It counts `mov`+`lsl` and `mov`+`neg` alike. A filter written the round before
implemented only the `lsl` example from the docs and let a `neg` case through --
the doc had already generalised the shape, and the generalisation is what should
have been coded.

## Two sharpenings of the lever

**A pooled argument needs naming too.** `OvlFunc_959_2008bec` sat at 2 of 59 with
every shifted and negated constant named, and the last two instructions were a
pool load and a `neg` transposed. Adding `e = 0xe666;` to the same entry-block
group matched outright. A pool load is one instruction and looks like it needs no
help, but it participates in the same scheduling decision, and left as a literal
it is pinned in the wrong slot. So: name every argument the ROM materialises
inside the interleaved run.

**The per-call-site stack-argument rule has to be paid for.** On the parked
`OvlFunc_924_20096c4` it buys the post-loop call outright (16 to 13 differing) and
then backfires inside the loop: one extra named local is free, a second forces a
spill and costs five lines (13 to 94, and 92 lines becomes 97). The lever buys two
registers at the call and the surrounding block has to have them spare. In
straight-line code that is nearly always true; in a loop already spending r8, r9
and r10 it usually is not.

## A global RE-READ across a branch is a `volatile` tell

`OvlFunc_922_2009d78` reads `iwram_3001e40` twice, once either side of its guard,
and the ROM emits the second `ldr` where gcc commons the two loads -- exactly the
one missing line, with everything after it shifted by one.
`extern volatile int` matched outright.

Measured alongside: a pointer local for the global's address is inert on its own
AND still fails combined with `volatile`. The address is a link-time constant, so
the local folds away and only costs ordering -- a third confirmation of "a local
that only holds an ADDRESS can cost the ordering, delete it".

## A misdiagnosis worth recording

`OvlFunc_968_2009150` (parked at 14 of 81) has a read-modify-write that looks
exactly like the constant-as-destination lever:

    rom   ldrb r2, [r0, #0] / mov r3, #0xfe / and r3, r2
    ours  ldrb r3, [r0, #0] / mov r2, #0xfe / and r2, r3

But the constant IS the destination in both -- r3 in the ROM, r2 in ours. Only
the register differs, so this is the register-role swap and the lever has nothing
to do. Screened before that was noticed: `int m` inert, `unsigned char n` inert,
both together much worse. **Check which operand is the destination in BOTH
streams before reaching for that lever**; the two shapes are hard to tell apart
because each shows a `mov` of a constant beside a load, and only the operand
order of the `and` distinguishes them.

That function also gave 59 differing to 14 on one edit -- consuming the call
result directly instead of routing it through a named local, which was the
previous batch's lever and is now three-for-three.
