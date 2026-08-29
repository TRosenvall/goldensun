# Batch 137 — name the offset, not the base

Verified on a clean `make clean && make compare` — `goldensun.gba` SHA1
`5c4695205413df7db52b9a184815a07783999971` — with every address read out of the
linked overlay ELFs. The rebuild regenerated every committed `.s` intermediate
byte-identically, re-checking the whole elevated corpus.

**remaining 2230 · elevated 3189 · parked 315**

## Elevated (5)

| function | address | ELF | what it took |
|---|---|---|---|
| `OvlFunc_945_200bf94` | 0x0200bf94 | rom_7cb2c0 | nothing — exact on the first screen |
| `OvlFunc_958_2009394` | 0x02009394 | rom_7e636c | area symbols + interleave; hand-split |
| `OvlFunc_932_200a310` | 0x0200a310 | rom_7b9cb4 | **the offset named, not the base** |
| `OvlFunc_932_200a490` | 0x0200a490 | rom_7b9cb4 | its sibling — first screen, levers reused |
| `OvlFunc_921_2009794` | 0x02009794 | rom_7a7298 | five interleaves + a linker alias |

## The new lever: name the OFFSET, not the base

Reading a global at a fixed offset pulls two ways, and there is a third spelling
that satisfies both:

    ROM   ldr r3, =gState / mov r2, #0xe1 / lsl r2, #1 / add r3, r2

  * base named — gcc hoists it across the preceding call into a callee-saved
    register and the function gains a push. **9 differing.**
  * fully inline — gcc folds the address into one pooled `=gState+450` and the
    function comes out THREE INSTRUCTIONS SHORT. **93 differing.**
  * **offset named** (`off = 0xe1 << 1; ... gState + off`) — the fold cannot
    happen and nothing crosses the call. **Exact.**

**Its precondition, learned by getting it wrong.** The commit that introduced it
claimed it explained `OvlFunc_955_2009424`, parked earlier on what looked like
the same tension. Retried there: 111 lines and 97 differing against that park's
own 108 and 78. The fold has to be COSTING something — on 200a310 the inline form
is three instructions short, on 2009424 it already matches length, so there is
nothing to prevent and the extra value is pure pressure. **Check that the inline
form is short before reaching for this.**

## Two levers that keep paying

**An int intermediate for a negated mask.** `p[9] = (p[9] & -13) | 4` truncates
to a byte and gcc emits one `mov #0xf3`; the ROM builds -13 in SImode with
`mov`+`neg`. Through an int local it builds it — and because that is two
instructions rather than one it fixes the LENGTH as well. Worth 33 lines on one
function and 53 on another. On the second I had already applied it to one mask
chain in the same function and missed the other: the tell shows up in the length
before the diff, so check length first.

**Typing both sides of a memory access.** Continued from batch 136. Writing
candidates typed from the start now costs nothing and occasionally removes a
whole class of difference before it appears.

## When a linker alias pays for itself

`OvlFunc_921_2009794`'s five `%` operations emit `bl __umodsi3` where the ROM
calls `_umodsi3_RAM`, and rom_7a7298 lacked the alias three sibling overlays
carry. Added, and it completed the match.

That is the test an earlier deferral failed. On `OvlFunc_932_200abe0` the same
alias would have left three real differences behind, so adding it would have been
a link-script change that bought nothing. **Add the alias when it is the only
thing left; defer it when it is not.**

Checked first that no elevated C in the overlay uses `%` and that the asm files
mentioning umodsi3 name the ROM symbol directly, then ran the byte-neutral
compare with the alias in place before the C landed.

## Splitting a .s that holds ONE function plus its data

`split_s.py` refuses these and is right to: with no second function there is no
boundary to split at, and converting the file would delete the data. When the
boundary is clean — text through `.func_end`, then `.section .data` — the manual
split is two files, both listed in BOTH the `.text` and `.data` lists of the
overlay script. `OvlFunc_958_2009394` is the worked example.

## A recurring mistake, named

Twice this batch I generalised a lever one function too far — the offset lever
onto 2009424, and in batch 136 the type fix onto the `OvlFunc_943_20090a0`
twins. Both times the diffs looked identical and the causes differed.

**What has to transfer is the REASON the lever worked, not the shape of the
diff.** Stated as a check: before reusing a lever, name the condition it exploited
and confirm that condition holds.

## Parked this batch

`OvlFunc_931_2008b2c` (third counterexample to the commoned-constant two-remedy
rule), `OvlFunc_907_2008328` (78 → 60 via the int intermediate, then register
roles), `OvlFunc_936_20096bc` (65 → 7 via four stacked levers).
