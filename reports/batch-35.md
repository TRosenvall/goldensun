# Batch 35 — the shape queue, drained and regenerating

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–34 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted — 96 overlays
compared byte-for-byte and `goldensun.gba: OK`. Every address below was read
back from the linked ELF with `nm`, including the one added `.L` export.

## Eighteen for eighteen, and then the queue refilled itself

`tools/match_shapes.py` (batch 33) built a queue of twenty leads. Across four
rounds every one of the eighteen attempted matched **on the first screen**, and
this batch drains the queue.

It did not stay drained. Adding these eight `.c` files added eight exemplars,
and re-running the tool immediately found **two new leads** — functions whose
shape was not in the solved corpus an hour earlier. The queue is a function of
how much has been solved, so it regenerates as the work proceeds. That is the
opposite of how the other rankers behave, and it is the argument for running
this one every round rather than treating it as a one-off sweep.

## The caveat finally bit, and it is worth reading

The tool's docstring says a shape match is **a lead, not a proof**, because
callee names are collapsed. `OvlFunc_931_20083d4` is the first case where that
mattered.

It and its neighbour `OvlFunc_931_2008360` have identical skeletons. They fill
`r0` in **opposite orders** in their first arm:

| | first arm calls | `r0` filled | so the C |
|---|---|---|---|
| `OvlFunc_931_2008360` | `__Func_8093054` | last | withholds the declaration |
| `OvlFunc_931_20083d4` | `__ActorMessage` | first | declares it |

Same skeleton, opposite lever. Nothing in the skeleton can see it, because the
skeleton is `mov R,I / mov R,I` either way — the *register* is what differs and
that is exactly what the tool collapses.

**So copy the exemplar's structure, then re-derive its levers from the ROM.**
Eighteen leads transferred wholesale and one did not; the ratio is good enough
to keep using, and bad enough to keep checking.

## A second hand split

`OvlFunc_944_200915c` needed the same treatment as batch 34's
`OvlFunc_947_20094c4`, and the pattern is now worth stating as a method rather
than an incident:

`tools/split_s.py` refuses a whole-file conversion when the `.s` also carries
data, because deleting it breaks the link. But **the function and the data go
into different output sections**, and the linker script lists `.text`, `.data`
and `.bss` separately. So the split is:

1. cut the `.s` at its first `.section` directive;
2. function half becomes `<stem>_a.s` → replaced by the `.c`;
3. data half becomes `<stem>_b.s`, keeping every `.section`;
4. in the linker script, point `(.text)` at `_a.o` and `(.data)` / `(.bss)` at
   `_b.o`.

Byte-neutral by construction, and verified as such before any C was written
both times. This one also needed one `.global .L18f8` added beside the two
labels already exported — the fourth such line in this tree, after batch 09's
two files and batch 34's three. A `.global` emits no bytes.

Two of the six leads left at the start of the round are still assembly for want
of time, not for want of a lever.

## An overlay is carrying a copy of main-ROM code

`Func_8093a14` is the main-ROM original of `OvlFunc_884_2008030`, elevated back
in **batch 16**. Same function, same clamps, same load-bearing `unsigned short`
narrowing; the overlay copy calls the imported `__atan2` and the main-ROM one
calls `atan2` directly.

The overlay is not calling into the main ROM here — it carries a duplicate. The
other rankers cannot see that category at all: `find_twins.py` compares callee
names and would never group them, and neither candidate ranker looks across the
two corpora. Collapsing callee names is what makes main-ROM twins of already
solved overlay code visible, and there are likely more.

## Functions

| function | address | overlay | note |
|---|---|---|---|
| `OvlFunc_917_200972c` | `0x0200972c` | rom_7a4370 | named-offset store |
| `Actor_SetAnimSpeed` | `0x0800c344` | main ROM | draw-kind dispatch |
| `OvlFunc_947_20094c4` | `0x020094c4` | rom_7d0e88 | GetEntrances 5-way; hand split |
| `OvlFunc_909_20081b4` | `0x020081b4` | rom_79c738 | seventh `-fno-rerun-cse-after-loop` TU |
| `OvlFunc_931_20083d4` | `0x020083d4` | rom_7b8cb0 | the lead that needed a different lever |
| `OvlFunc_911_2008050` | `0x02008050` | rom_79e5c0 | narrow_constant inverted |
| `OvlFunc_944_200915c` | `0x0200915c` | rom_7ca63c | unsigned switch; hand split |
| `Func_8093a14` | `0x08093a14` | main ROM | twin of an overlay function |

Four `.global` lines added across two `.s` files (`.L3174`, `.L3264`, `.L32dc`
in rom_7d0e88; `.L18f8` in rom_7ca63c). No symbol additions to `message.sym`
this batch.

## Corrections to earlier reports

**`HANDOFF.md` said two TUs are built with `-fno-rerun-cse-after-loop`.** That
was true in batch 25 and there are now **seven**. The standing item is updated,
including the part that still holds: the original sweep over 85 parked files
matched only the first two, and the five added since were each found by
recognising the shape on a fresh candidate. The sweep's negative result stands;
the count did not.

## Parked

Nothing new.

## Counts

314 functions elevated in total. 2,981 hand-written functions remain in `asm/`
of 5,714. 95 parked functions, plus 6 files that document blocker classes rather
than individual functions.
