# Batch 133 — the cheapest queue was the tool I wasn't running

Verified on a clean `make clean && make compare` — `goldensun.gba: OK` — with
every address read out of the linked ELF.

## Elevated (2208 → 2202)

| function | address | notes |
|---|---|---|
| `OvlFunc_907_20088f0` | 0x020088f0 | first screen |
| `OvlFunc_927_2008f94` | 0x02008f94 | struct passed BY VALUE |
| `OvlFunc_899_200891c` | 0x0200891c | twin template, first screen |
| `OvlFunc_902_2008204` | 0x02008204 | its twin, one `sed` |
| `OvlFunc_945_200dc48` | 0x0200dc48 | twin template |
| `OvlFunc_895_2009ac8` | 0x02009ac8 | its twin, one `sed` |

## Two unsolved twins are a two-for-one

`solved_twins.py` searches remaining functions against SOLVED ones, so it cannot
see a pair where neither is done yet. `twin_families.py` groups the remaining
functions against **each other**, and that is where those pairs live. I had been
running the first and not the second.

`OvlFunc_899_200891c` / `OvlFunc_902_2008204` are 87 instructions with identical
opcode streams in different overlays; the filtered diff showed **two** differing
lines. `OvlFunc_945_200dc48` / `OvlFunc_895_2009ac8` are 37 instructions with
four. In both cases the template matched on its first screen and the twin was a
`sed`.

**41 families of 2+ cover 90 remaining functions, and 13 have no parked member**
— 26 functions in shapes nobody has tried. A family where every member is parked
is a shape already known blocked; one with none parked simply has not been
attempted. That distinction makes the list a work queue.

Method: diff the two listings with `grep -vE "^\.L|\tb\t|bne|beq|bhi|bls"` on
both sides. What survives is the whole edit.

## Three new readings

**A struct passed by value.** `ldmia`/`stmia` copying from a local into the
stack argument area, with the first four words also loaded into r0–r3, is gcc
passing a six-word struct by value — not hand-marshalled arguments.
`OvlFunc_927_2008f94` matched on the first screen once the callee was declared
`void f(struct S s)`.

**Name a POOLED argument to reach an interleave.** The interleave lever is not
restricted to `mov`-built constants: `OvlFunc_945_200dc48` needed
`e = 0xe666;` named in the dominating block to move a pooled third argument past
two `neg`s. This does not contradict "leave pooled constants inline" — that rule
is about a value used at several sites, where naming makes gcc hold it. Used
once, there is nothing to hold.

**The HImode-literal rule is not one rule.** `OvlFunc_901_200858c` (parked at 2
of 70) uses one `unsigned short *` twice and needs opposite spellings: `*p |= 2;`
as a compound assignment to get the ROM's **pooled** constant, and an `int`
intermediate at `*p = 1;` to get a **mov**-built one. Measure per operation.

## Parked

- `OvlFunc_896_200c78c` + its twin — 8 of 88. Two levers did the work (assigning
  the walking pointer before the DMA call, and consuming the actor pointer
  rather than indexing it: 44 differing → 8). The residue includes a shape not
  seen before: the ROM uses **opposite reg+reg operand orders** for two
  otherwise identical table loads, and reversing the address expressions does
  not reach it — gcc canonicalises the addition before choosing.
- `OvlFunc_901_200858c` — 2 of 70, above.
- `OvlFunc_953_20091c4` — 10 of 75. Second instance of the base/offset
  register-role swap that also blocks `OvlFunc_904_2008054`; four more spellings
  tried this batch and none moved it. The mechanism is now stated: the ROM
  derefs the global into a NEW register, freeing the address register for the
  offset; ours reuses the address register for the result.

## Still owed

- 12 `.s` TUs under non-default wildcards (measured; `pool.py` flags them).
- ~3,300 lines of duplicate Makefile rule blocks.
- 297 parks, deferred by request.
