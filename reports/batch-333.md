# Batch 333 — seven landings, and the length-gap criterion holds at scale

Five briefs, 25 targets, **7 landings**. **699 → 692 parked.**
**4,919 → 4,926 of 5,710 (86.3%).** Gated green at every phase; SHA1
`5c4695205413df7db52b9a184815a07783999971` throughout.

## Every target was chosen on one test, for the second batch running

All 25 came from `reports/park-figure-audit.md`: **the two sides' 16-bit
instruction counts differ by 1 or 2.** Nothing about mechanism, bank or
diagnosis entered the choice.

| brief | landed | parks re-priced |
|---|---|---|
| D | **5** | 1 |
| E | **2** | 3 |
| A | 0 | 4 |
| B | 0 | 3 |
| C | 0 | 5 |

**Every brief located the missing or extra instruction on every target** — 25
for 25. Seven closed outright; the other eighteen moved or were re-priced.

**In eight of the re-priced parks the LENGTHS NOW AGREE**, so those figures are
true distances for the first time: `80b2ed8` 26 → **3**, `20086e8` 18 → **2**,
`80c0228` 36 → **21**, `80ae9f0` 26 → **13**, `20087b0` 26 → **14**,
`80ae99c` 38 → **18**, `2008388` 26 → **19**, `801e3c8` 29 → **25**.

## Landings

| function | from | what closed it |
|---|---|---|
| `Func_80a2144` | 31 | a `(u16)` cast making `0x1f` pool in HImode |
| `Func_80e727c` | 38 | the same cast |
| `Func_80e38b8` | 39 | removing *other* named locals to raise pressure |
| `Func_80e3908` | 29 | same piece as the above |
| `Func_80d67dc` | 30 | `DMA3_SET_RW`, then `volatile` on the local |
| `OvlFunc_891_2008098` | 25 | loop stack arguments as literals in a real `do/while` |
| `Func_8079008` | 29 | a **second read of the same byte** |

## The mechanism that explained work in four briefs at once

**Every HImode constant in Thumb reaches the pool.** `*thumb_movhi_insn`
(`arm.md:4318-4320`) constrains operand 1 as `"l,mn,l,*h,*r,I"` — alternative
**1** is `mn`, where `n` matches any `const_int`, and the immediate `I` is
alternative **5**. `recog` takes the first match. `*thumb_movsi_insn`
(`:3838-3840`) has the order **reversed** — `I` at 1, the pool path `mi` at 6 —
so a small SImode constant can **never** pool.

That contrast ran in both directions: brief D *wanted* the pool and got it with
a `(u16)` cast (two landings); brief B wanted to *avoid* it and did so with
`int z = 0` and a store reorder. It also refuted `200b1b8`'s "the pooled zero is
a literal, CLOSED" and made `8091eb0`'s standing argument exhaustive rather than
value-specific.

## Two inverse levers on copies, now both named

- **Batch 332:** a copy survives when its **source** is not trivially a copy —
  `off = slot + K` keeps it where `off = slot; off += K;` is propagated away.
- **This batch:** a copy survives when its **destination** is a named variable —
  `v = d[9]; t = n; t &= v; t |= m; d[9] = t;` emits one `and`, while
  `d[9] = (n & d[9]) | m;` emits the ROM's `mov`+`and`. **Naming `t` is what
  gives regmove a pseudo to retarget onto.**

`OvlFunc_949_20086e8`'s two sites wanted **opposite** two-address directions,
which is why its own table missed it — the diagonal-of-a-square trap again.

## Four park headers blamed the allocator; the pass is GCSE at 07

Verified in the vendored source: `gcse.c:3902` prints
`"COPY-PROP: Replacing reg %d in insn %d"` and `:3994` the `copy props` tally,
and gcse is pass **07** — the copy is present in `.03.cse` and gone by
`.09.cse2`. That corrects `80b90ac`, `8006088`, `80c1014` and
`ovl_77dd1c/2009498`, and explains why three specimens' worth of
pointer-splitting spellings were exactly inert. The obvious follow-up is also
dead: `loop.c` runs at pass 08, *after* gcse, so putting the copy in a loop body
does not save it — gcse propagates it where it stands.

This is the second time in three batches that boundary or allocator work was
aimed at the wrong pass. **Trace the insn through every dump and name the pass
before building anything.**

## Two mistakes of mine

**I split one piece across two briefs.** `asm/rom_c9000/rom_e28f4_c_c_a.s` holds
four functions, two of them parked, and I gave `Func_80e38b8` to brief D and
`Func_80e3908` to brief A. D noticed and landed both; A spent part of its round
on a function that was landing underneath it. **Check a candidate piece for
other parked members before assigning, not just the target.**

**My same-length pairing was a bad heuristic, and both briefs said so.** I
paired `2008388`/`20087b0` (both 26 at 53/54) and `200b1b8`/`2008098` (both
65/64) as likely shared mechanisms. Both checked first, as asked, and both found
coincidence. Brief E named the flaw exactly: equal length is *"a LENGTH
coincidence, which is exactly what figure-ranked selection selects for."*
**Pairing on the axis you already selected on finds nothing.** The pairing still
paid for E, but by **cross-reading** — `2008098`'s park carried the goto-loop
lever `801e3c8` was blocked on, and `2008098`'s own fix was to *undo* it.

## A brief's headline lead was wrong, and it would have aimed a batch

Brief B reported `asm/overlays/rom_7fb4a8/ovl_30_a_c_c_c_a_c_c_a_a.s` as one TU
with three parked functions, `OvlFunc_971_2008580` **"at 1 differing"** and
`2008860` **"at 12"**, calling it the highest-value item in the batch. The piece
and its three parks are real — verified — but `parkcheck` reads **171** and
**120**, and the headers say so in their first lines. `2008580`'s header reads
*"SHIMS: 1 pin … 1 of 3 functions, no data"*, which is where a "1" was available
to misread. **Prose read as a figure, by an agent this time rather than a tool.**

## Two more park files were behind their own headers

`80bac6c`'s header described three batch-261 fixes and named a scratch candidate
that no longer exists; the installed body never carried them. All three are in
now and loop 3 is byte-exact — which also answers its open item: the constant's
pooling was **ours**, so no `const.sym` entry is wanted. And `80f0614`'s own
proposed next step measures one instruction **worse**; its copy is
`expmed.c:3243`'s `copy_to_mode_reg`, dying in `.03.cse` three passes before any
allocator, not to coalescing as claimed.

## For the owner

- **`_CONST_1f` now has no beneficiary anywhere in the tree.** Its `const.sym`
  entry rested entirely on `Func_80a2144`'s pooled mask, which the `(u16)` cast
  now supplies. Candidate for retirement; untouched.
- `_MSG_741` (`message.sym`) and `_CONST_24` (`const.sym`) still stand as the
  two evidenced symbol requests.

## Carried forward

- **The audit shortlist has ~158 unworked parks left at gap ≤ 2.** Two batches
  have now run it: 8 of 15, then 7 of 25.
- **Two-for-one pieces found by the piece check**: `8006088` shares with parked
  `Func_8005fcc`; `80c00d8` shares with parked `Func_80c0130`.
- `Func_80b1bd0` is **one `adds r2, r0, #0` from exact** (234 of 238 aligned).
- `Func_8006088`'s missing copy **is reachable** — an if/else on the first guard
  emits it at 20 of 44 with the first twenty-one instructions exact — but costs
  a branch, which is the trap that park's own header records.
