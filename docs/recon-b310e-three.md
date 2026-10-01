# Batch 310 brief E -- recon for the three targets NOT reconstructed

Two of the five were reconstructed and measured end to end (PARK_OvlFunc_959_200a7b0.c,
PARK_OvlFunc_886_2008658.c).  **No objcmp or aligncmp number exists for the three below and
none is claimed.**  What follows is measured structure so whoever picks them up does not
repeat the survey.

## Shared results that apply to all three

**NO `_AREA_*` SYMBOL IS NEEDED BY ANY OF THEM.**  The pool-tell screen -- a pooled
constant that fits in an eight-bit `mov` -- comes up EMPTY on all three.  (959_200a7b0 by
contrast needed exactly one, `_AREA_a3`, which is why it is worth running the screen per
function rather than per band.)

**ALL THREE REFERENCES USE THE HIGH BANK**: `push {r5, r6, r7, lr}` followed by further
pushes, with 10-13 high-register mentions each.  With 886_2008658's reference (which parks
`&iwram_3001ebc` in r8) that makes FIVE witnesses in this band for the batch-307
retraction of `-ffixed-r8..r11`.  Do not revive it.

**`datacheck.py` IS SILENT ON ALL THREE FILES** -- no data section, no label read, so
**no `.global` is required and the asm-label capture hazard does not arise**.

**ALL THREE ARE THE STRAIGHT-LINE POPULATION AND SHOULD BE EXPECTED TO SHOW THE SAME ONE
BLOCKER** as the two reconstructed here: cse pass 1 commoning a repeated `mov`+`lsl`
constant into a register the reference rebuilds at every site.  Before anything else, run
the two cheap gates from the parks -- the DISTINCT-CONSTANT SET and the IMMEDIATE MULTISET
(`mov` bases and `lsl` shift amounts).  A residue that is entirely "ours short on 0x80 /
0xa0 / 0xd0 and on shift counts 1/7/8" is this blocker and nothing else.

## OvlFunc_969_200cbec -- 1041 instructions (overlay 969, rom_7f6e64)

    asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_c.s

1041 instructions, 5 branches, 5 labels, 13 high-register mentions, 266 calls over 40
distinct callees.  16 distinct pooled numerics; 7 pooled symbols
(`OvlFunc_969_2008400`, `gScript_969__0200e074`, `..e324`, `..e360`, `..e39c`, `..e3c0`,
`iwram_3001ebc`).

**SPLIT: NONE NEEDED.**  `grep -c thumb_func_start` = 1, so landing is a whole-file
conversion to `src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_c.c`.

**THE FRAME IS NOT A DECLARATION LIST.**  `sub sp, #8`, and all sixteen `[sp]` /
`[sp, #4]` references are STORES immediately before a call, never read, with no
`add rX, sp` anywhere -- a two-word OUTGOING ARGUMENT BLOCK for six-argument calls.
Sorting those two offsets yields nothing.  Branches: `bne .L5046`, `b .L5010`, `b .L507e`,
`beq .L5094`, `b .L5458`.

## OvlFunc_899_200b6f8 -- 1459 instructions (overlay 899, rom_794ac0)

    asm/overlays/rom_794ac0/ovl_30_c_a_a_c_c.s

1459 instructions, 6 branches, 6 labels, 11 high-register mentions, **387 calls** over 43
distinct callees -- the call-densest of the five, and 875 of its instructions are `mov`.
13 distinct pooled numerics; 6 pooled symbols (five `gScript_899__*` plus
`iwram_3001ebc`).

**SPLIT: NONE NEEDED** (one function in the file) -- lands as
`src/overlays/rom_794ac0/ovl_30_c_a_a_c_c.c`.

**NO FRAME AT ALL**: no `sub sp`, zero `[sp]` references.  So it has no spilled scalar and
no stack argument, which makes it the CLEANEST test in the band of the cse-commoning
question with nothing else in the way.  Its two 37-call siblings
(`OvlFunc_899_200c5f4`, `OvlFunc_899_200c63c`) are called in a strict alternation worth
reading as a two-actor dialogue loop before transcribing.

## OvlFunc_883_20095dc -- 1996 instructions (overlay 883, rom_780898)

    asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_c_c_a_a_a_a.s

1996 instructions, 9 branches, 9 labels, 10 high-register mentions, 496 calls over 46
distinct callees.  **45 distinct pooled numerics** -- three times either of the others, so
the distinct-constant-set gate will be worth the most here.  4 pooled symbols (three
`gScript_883__*` plus `iwram_3001ebc`).

**SPLIT IS REQUIRED, TWO-WAY.**  The file holds TWO functions, `OvlFunc_883_20095dc` and
`OvlFunc_883_200aa54`; the target is FIRST, so it lands as the `_a` part.  Run
`tools/split_s.py --dry-run` to get the exact names before touching anything, and verify
`make compare` is green after the split and BEFORE writing any `.c`.

**THIS IS THE ONE OF THE THREE WHERE THE SPILL-SLOT MAP DOES PAY, AND ONLY FOR TWO SLOTS.**
`sub sp, #0x24` (36 bytes, 9 words), 28 `[sp]` references.  Slots 0x00-0x18 are an
outgoing argument block, exactly as in the other two -- including one eleven-argument call
(`str` to 0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18 at reference lines 1215-1225), the same
`__Func_80931ec`-shaped call that 886_2008658 has.  But slots **0x1c and 0x20 are REAL
LOCALS**: 0x1c is written early and READ at the end (`ldr r2, [sp, #0x1c]`), and 0x20 has
its ADDRESS TAKEN (`add r1, sp, #0x20`) and passed to a call.  So the declaration list is
two entries, sorted descending: the address-taken one at 0x20 first, then the scalar at
0x1c.  **The general rule the two reconstructed functions establish: in this population a
frame is outgoing-argument space by default, and only slots that are READ, or whose
address is taken, are declarations.  The screen is `add rX, sp` and a load, not the
`sub sp` immediate.**
