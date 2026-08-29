# Batch 34 — five more from the shape matcher, and what `find_twins` cannot see

*Status: ready to port. Reviewed by nobody yet.*

For porting into `Coaltergeist/goldensun-decomp`. Assumes batches 01–33 are in.

Verified against `baserom.gba` (SHA1 `5c4695205413df7db52b9a184815a07783999971`)
from a clean `make clean && make -j8 && make compare`, unassisted — 96 overlays
compared byte-for-byte and `goldensun.gba: OK`. Every address below was read
back from the linked overlay ELF with `nm`, and both new `message.sym` symbols
from `stage1.o`.

## Ten for ten

`tools/match_shapes.py` (batch 33) has now produced two full batches, and
**every one of the ten functions matched on the first screen.** No round was
spent on a wall in either.

That says the strictness is calibrated about right. The tool requires
instruction count, mnemonic sequence, and every operand's *kind* to agree
exactly; it collapses only constants, registers and callee names. When those
agree, the exemplar's `.c` has transferred without exception so far — change the
numbers, keep the levers, screen once.

Ten shape leads remain.

## The two grouping tools are complements, not rivals

The `OvlFunc_901` pair is the case that makes the distinction concrete.

`OvlFunc_901_2008ac8` and `OvlFunc_898_2008f3c` are **the same fifteen
instructions in two different overlays**. They never grouped under
`find_twins.py`, because each calls its *own overlay's* copy of the helper —
`OvlFunc_901_2008a80` in one, `OvlFunc_898_2008ef4` in the other — and that tool
compares callee names.

| tool | scope | callee names | finds |
|---|---|---|---|
| `find_twins.py` | within remaining assembly | compared | groups worth solving once |
| `match_shapes.py` | remaining vs. solved | collapsed | functions already solved elsewhere |

Per-overlay copies of a helper are not rare in this ROM, so this is a systematic
blind spot rather than one awkward pair. `find_twins.py`'s docstring now says
so — reaching for the wrong tool costs a round, and the two look
interchangeable from their names.

## The functions

| function | address | overlay | family |
|---|---|---|---|
| `OvlFunc_901_2008ac8` | `0x02008ac8` | rom_797990 | declaration lever, argument order |
| `OvlFunc_901_2008af0` | `0x02008af0` | rom_797990 | twin of the above |
| `OvlFunc_950_20086ec` | `0x020086ec` | rom_7d5838 | three-message prompt in a cutscene |
| `OvlFunc_950_20088cc` | `0x020088cc` | rom_7d5838 | same |
| `OvlFunc_935_2008398` | `0x02008398` | rom_7bf5a8 | stack-arg-pair, two of three cases |

Two symbols added to `message.sym`, both by value with no semantic claim:
`_MSG_239e`, `_MSG_23ac`.

Nothing new is learned about the three levers themselves — batches 31 and 32
have their accounts, and each file here cross-references the exemplar rather
than restating it. The point of this batch is the rate, not the findings.

## Parked

Nothing new.

## Counts

306 functions elevated in total. 2,989 hand-written functions remain in `asm/`
of 5,714. 95 parked functions, plus 6 files that document blocker classes rather
than individual functions.
