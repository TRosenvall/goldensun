# Batch 307 brief G -- recon only, NO figures invented

Two of the four targets were not reconstructed.  No objcmp or aligncmp number
exists for either and none is claimed.  What follows is measured structure and
the split shape, so whoever picks them up does not repeat the survey.

## OvlFunc_959_200a7b0 -- 810 instructions (overlay 959, rom_7e7574)

    asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c.s

**Structure: 810 instructions, 5 branches, 6 labels, ZERO high-register
mentions.**  That is the same profile as OvlFunc_881_2008c28 -- a straight-line
cutscene script whose reference does NOT use r8-r11.  Expect the same blocker
(cse1 parks more constants than the original did) and expect the low
call-saved set to be the whole argument.  Read `PARK_OvlFunc_881_2008c28.c`
before starting; it is the closer analogue of the two parked here.

**SPLIT IS REQUIRED and it is a THREE-way split, not two.**  The file holds
four functions -- OvlFunc_959_200a69c, OvlFunc_959_200a718,
OvlFunc_959_200a7b0, OvlFunc_959_200b054 -- and `split_s.py --dry-run` produces

    ovl_9dc_c_c_a_a_a_c_c_a.s   ovl_9dc_c_c_a_a_a_c_c_b.s   ovl_9dc_c_c_a_a_a_c_c_c.s

rewriting `overlays/rom_7e7574/overlay.ld`, with the target landing as
`src/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c_b.c`.

**`.global` requirements: NONE.**  datacheck.py on the file is SILENT -- no
data section, no label read by any of the four functions.

## Anim_Ramses -- 818 instructions (rom_c9000)

    asm/rom_c9000/rom_e7320_c_c.s

**Structure: 818 instructions, 54 branches, 50 labels, 65 high-register
mentions.**  THIS IS A COMPLETELY DIFFERENT ANIMAL from the three cutscenes and
should NOT be briefed alongside them.  54 branches over 818 instructions means
~15 instructions per basic block, so cse pass 1's window is SHORT and the
constant-commoning blocker that dominates the cutscenes should not dominate
here.  The established 500-instruction lever set is much more likely to apply
region by region.  Its closest family parks really are the right reading:
`Anim_Froth` (4 of 529, size/count/relocations exact) and `Anim_Whirlwind`
(26 of 472), plus several landed `Anim_*` in `src/rom_c9000/`.

**SPLIT IS REQUIRED, IT IS A TEXT/DATA SPLIT, AND IT NEEDS FOUR EXPORTS.**
datacheck.py:

    data sections : .rodata
    Anim_Ramses   reads .Leeed8, .Leeee1, .Leeeea, .Leeef8
    *** SPLIT MUST EXPORT: .global .Leeed8 .global .Leeee1 .global .Leeeea .global .Leeef8

Per the tree's own discipline, emit those four `.global` lines and verify
`make compare` is still green BEFORE running the split, so the export and the
split stay separable.  A `.global` emits no bytes.

**THE ASM-LABEL CAPTURE HAZARD DOES NOT APPLY TO THESE FOUR, and here is the
check rather than the assumption.**  The screen is "contains a hex letter",
because gcc's own label counter is DECIMAL.  All four names carry letters --
`eeed8`, `eeee1`, `eeeea`, `eeef8` -- so gcc cannot mint a colliding `.LNNN`.
The five-digit length is not what makes them safe; the letters are.  Note that
the sibling functions in the same file need their own exports if they are ever
converted (`BaseAnim_Meteor` four, `Anim_Ragnarok` two, `Anim_TitanBlade` two),
and those are all lettered too.
