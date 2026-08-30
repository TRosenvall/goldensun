# Batch 153 — a four-agent pass, and two documentation faults that cost real functions

Verified from a clean build: `make clean` → host recovery for the five
`old_agbcc` objects ([batch-61](batch-61.md)) → `make -j8 && make compare` →
`goldensun.gba: OK`, SHA1 `5c4695205413df7db52b9a184815a07783999971`, green on
the first attempt, `git status` clean, `0 orphaned linker references`. Every
address below was read back from the linked ELF.

**Ten elevated, which overshoots the 5–8 guidance.** A four-agent pass landed as
one block and splitting it across two reports would have separated findings from
the functions that produced them.

| Function | Address | Overlay / ROM |
|---|---|---|
| `OvlFunc_946_2009f78` | `02009f78` | ovl_7ced6c |
| `OvlFunc_946_200a080` | `0200a080` | ovl_7ced6c |
| `OvlFunc_946_200a4c8` | `0200a4c8` | ovl_7ced6c |
| `OvlFunc_946_200a5f0` | `0200a5f0` | ovl_7ced6c |
| `OvlFunc_946_200a700` | `0200a700` | ovl_7ced6c |
| `OvlFunc_946_200a848` | `0200a848` | ovl_7ced6c |
| `OvlFunc_946_200a984` | `0200a984` | ovl_7ced6c |
| `Func_802851c` | `0802851c` | main ROM |
| `Func_80b7f9c` | `080b7f9c` | main ROM |
| `OvlFunc_931_2008c44` | `02008c44` | ovl_7b8cb0 |

## The documentation was wrong in two places, and both cost functions

This is the batch's most important content, because both faults had been sitting
in the tree for dozens of batches while actively misdirecting work.

**1. The compiler is gcc-2.96. `docs/matching.md` said it was agbcc.** That file
opened with the question "Is agbcc the right compiler?" and answered "**Yes.**"
The README already carried the correction and its evidence — agbcc never emits
Thumb register-offset addressing (768 functions need it) and never allocates
`lr` in leaf functions (117 more) — but anyone reading the docs by name hit the
wrong answer first. `agbcc` and `old_agbcc` build **five objects** in the entire
tree, all prebuilt Nintendo library code under `src/lib/m4a` and
`src/lib/agb_flash`. `docs/elevation.md` now opens with a banner saying so, and
stating the rule that follows from it: **never explain an elevation blocker in
terms of agbcc or old_agbcc.**

**2. The branch-over-pool "ceiling" is not a ceiling, and the stale claim was
still stated twice.** `docs/elevation.md` contained the claim in two places and
its refutation in a third, with the wrong version appearing first and reading
authoritatively. The cost was immediate and measurable: **an agent this round
skipped `StartSnow` without writing a line of C**, quoting the stale text as its
reason. The claim rests on old_agbcc's pool behaviour, which is irrelevant here;
gcc-2.96 emits mid-function pools readily — 64 of them in already-matching code
— and the shape is `.word`, not `.pool`. Both sites now carry a superseded block
naming the correction, with the original text kept beneath so the reasoning
stays visible. **This class is several hundred functions and it has been sitting
in the census as a ceiling that is not there.**

## The agent pass

Four agents, twelve functions, screening only — barred from the Makefile,
linker scripts, `.s` files, `split_s.py`, and from committing. Everything they
returned was re-verified here before installing. Seven of their twelve matched.

One agent re-screened each result four times on its own initiative, which is the
right instinct: a single `OK` proves little when roughly one compile in thirty of
an affected file diverges (README "Determinism").

**What they found that transfers:**

- **A single exit beats five returns.** On `OvlFunc_946_2008f70`, replacing five
  `return 0;` with one `goto out;` was 152 → 98 differing, the single largest win
  in the batch and a lever already recorded elsewhere in that overlay applying
  five times over.
- **One variable serving as both loop counter and table byte-offset.** On the
  15-copy `20088c0` group, a separate offset variable costs an extra `mov` and
  misaligns the whole loop; 101 → 57.
- **Table typing.** `extern int L31b4[][4]` rather than `unsigned char[]` with
  byte offsets — with byte offsets gcc folds to a pool word `=.L31b4+4` and
  subtracts to reach element zero.
- **A store forcing a reload.** Ordering two stores controls whether an
  intervening write kills a CSE, which is what produces the ROM's reload.
- **Loop shape.** `Func_802851c` needs a subscript, not a walking pointer: the
  pointer form hoists its preheader above the guard, 26 differing.
- **`(short)` casts are not `int` intermediates.** `int d = (short)(x)` loses the
  `lsl/asr #16` when the operand came from an unsigned halfword; a `short`
  variable keeps it.

## Parks

`OvlFunc_924_200cf44` (2 of 28) and `OvlFunc_922_2009b1c` (4 of 57) are
arg-interleave at **unguarded first-block sites** — the third and fourth
confirmed instances this batch of the boundary that a function with no branch
has no dominating block to assign the constant in. `OvlFunc_945_200bdec` (2 of
26) is a fifth, and `Anim_UnleashIntro` a sixth.

`Func_80b5864` is 56 differing under the tree's flags and **4** under
`-fno-rerun-cse-after-loop` — a third independent candidate for the `CSE_CFLAGS`
group, whose own Makefile comment flags it for review as thin evidence. Recorded
rather than acted on: whether that TU should get the flag is a judgement about
the original build.

`OvlFunc_907_2008fa0` **matches byte-exact but is not installed.** Its `.s`
carries 21 `.incbin` blobs and 19 global labels the rest of the overlay
references, so the file cannot be converted wholesale. It is filed in the park
corpus with a header saying plainly that it matches and that the blocker is file
structure. It needs a hand split of code from data.

## An operational mistake worth recording

I deleted that `.s` on the assumption it held only the function — my check for
data looked for `.word`/`.byte`, and `.incbin` is neither — and the link failed
with a page of undefined references. Reverted; no lasting damage.

**`tools/split_s.py` already guards this and says so.** Run on that file it
refuses outright and explains why. I did not see it because I piped the tool
through `grep -E "^asm.*->"` to pull out the success line, which discarded the
refusal. **Filtering a tool's output down to the shape you expect throws away the
case where it disagrees with you.** On the next file I read the whole output,
and it cleared that one as data-free and safe to convert directly.
