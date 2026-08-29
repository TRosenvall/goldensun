# Batch 136 — a blocker class turned out to be a missing type

Verified on a clean `make clean && make compare` — `goldensun.gba` SHA1
`5c4695205413df7db52b9a184815a07783999971` — with every address read out of the
linked ELFs. The clean rebuild regenerated every committed `.s` intermediate
byte-identically, which re-checks the whole elevated corpus rather than just this
batch.

**remaining 2235 · elevated 3184 · parked 312**

## Elevated (8)

| function | address | ELF | what it took |
|---|---|---|---|
| `OvlFunc_945_200b66c` | 0x0200b66c | rom_7cb2c0 | 2 of 127 on the first screen |
| `OvlFunc_896_200c3bc` | 0x0200c3bc | rom_78ef88 | the frame size, and only that |
| `OvlFunc_907_20089cc` | 0x020089cc | rom_79b154 | six interleave sites, one naming pass |
| `OvlFunc_934_20095cc` | 0x020095cc | rom_7bdeb0 | struct by value + a shared constant |
| `OvlFunc_945_2008b84` | 0x02008b84 | rom_7cb2c0 | CSE_CFLAGS + one dropped prototype |
| `OvlFunc_945_2008cc8` | 0x02008cc8 | rom_7cb2c0 | the sibling's levers, unchanged |
| `OvlFunc_932_200a5c0` | 0x0200a5c0 | rom_7b9cb4 | **park recovered — a missing type** |
| `Func_80167ac` | 0x080167ac | goldensun.elf | **park recovered — a missing type** |

## The headline: "register-role swap" was at least two problems

`OvlFunc_932_200a5c0` sat parked at 2 of 107 under the commutative
register-role class — 21 parks describe it, and it had survived seven spellings
and four flag groups across two rounds. It was not an allocation wall:

    p += 0x23;  *p = 2 | *p;      ours: loaded byte in the orr's destination
    p->flags |= 2;                ROM:  constant there.  Exact, first try.

`Func_80167ac` then fell the same way from 14 differing AND two instructions
short, filed as "formed pointer vs register-offset store". Its offsets
(0xea8..0xeae) are far past any store displacement, so a struct makes gcc form an
address per store — which is what the ROM does and what no arrangement of the
pointer arithmetic produced.

**73 parks reach memory through raw pointer arithmetic.** Three have been
type-screened; two matched immediately and one did not move. So this is a cheap
candidate, not a universal key — but the selection rule is now clear:

> A park describing an addressing-mode or register-role difference is worth
> type-screening even when its note calls the blocker scheduling or allocation.
> Those notes were written from the diff, and the diff cannot distinguish "gcc
> allocated differently" from "gcc was told the wrong type".

## A ceiling, measured: 14% of what remains cannot match

`tools/poolblocked.py` finds functions that jump over their own literal pool:

        b .L6a0
        .pool_aligned
    .L6a0:

old_agbcc emits pools at `.func_end` and never early, so that `b` is unreachable
— zero occurrences across all elevated translation units, with the
translation-unit hypothesis separately tested and refuted. **312 of the 2,239
remaining functions carry it, 13.9%.**

That is a real limit rather than a backlog. It also means any candidate ranking
that does not exclude them keeps floating them up: `OvlFunc_974_200829c` sat at
the top of the dense queue with the most attractive profile in it — 588
instructions, three callees, no branches, no shifts, reuse 0 — and is
unreachable. The scan cost one pass and saved 196 calls of transcription.

## Levers: what scaled and what did not

**The interleave lever scaled to six sites at once** (`OvlFunc_907_20089cc`),
including both arms of a nested branch — naming before the OUTER `if` covers
arms that never both execute. When `pool.py`'s site count equals the differing
count, the whole diff is one lever.

**"r0 in the middle" and "r0 at the end" are different problems.** Inside a split
build, name the other arguments; last of all, drop the prototype. One function
wanted both, at different sites. Reaching for the wrong one looks exactly like
the right one failing.

**A `sub sp` difference is the local's size, not the body.** `OvlFunc_896_200c3bc`
screened at 2 of 97 with an eight-argument call, 16.16 fixed-point masking and a
high-register counter all already right; the struct was padded to 0x40 and needed
0x28. ROM frame minus outgoing argument area gives the size exactly.

**The source-order lever does not scale past two values.** All six assignment
permutations of four held constants land at 85 or 86 differing
(`OvlFunc_945_200b51c`).

**A twin's spelling transfers only under an equal register budget.**
`OvlFunc_945_2008cc8` matched on its first screen with its sibling's two levers;
`OvlFunc_955_2009424` is a near-twin of an elevated function whose spelling does
NOT carry, because naming the gState base there costs a fourth callee-saved
register. Same shape is not enough — count what the ROM pushes.

## Parked this batch

`OvlFunc_945_200b51c`, `OvlFunc_955_2009424`, `Func_80a33d4`,
`OvlFunc_974_200829c` (parked on the pool pre-screen, not on a failed match), and
`OvlFunc_931_2008d08` (type-screened, unchanged).

`Func_80a33d4` is worth a note: naming a repeated stack argument INVERTS there.
A single named local is hoisted out of all three loops and the function comes out
four lines short, because the ROM re-materialises the constant before every loop;
three separate assignments give the same result since gcc commons them back.
