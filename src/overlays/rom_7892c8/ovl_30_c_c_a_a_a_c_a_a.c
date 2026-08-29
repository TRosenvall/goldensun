/* OvlFunc_888_20085cc  --  0x020085cc
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * changes name in goldensun/overlays/rom_7892c8/overlay.ld and nothing else
 * moves.
 *
 * Per-area arrival hook: twenty-six slots from 0xa, four live groups.
 *
 * THIS WAS PARKED IN BATCH 104 ON THE r0-AGAINST-A-SHIFT ROTATION and the
 * BASIC-BLOCK LEVER (docs/elevation.md) closes it. The two coordinates are
 * assigned to locals in the case's own block and used inside the `if` -- a
 * different block -- so gcc rematerialises them at the call, split, with
 * `mov r0, #0xa` scheduled into the gap. 25 differing to 22. That rotation is
 * NOT a separate class from arg-interleave, which is what batch 100 had
 * concluded.
 *
 * THE LAST INSTRUCTION WAS A LABEL, NOT A LABEL'S WORTH OF CODE. The ROM's
 * `.L6a0` is ONE label doing three jobs: the jump-table target for cases
 * 0x1d/0x20/0x23, the `b` target from the 0xa/0xb/0xc arm, and the fallthrough
 * from case 0x14. Writing the 0xa/0xb/0xc arm to end with its own
 * `__ClearFlag(0x12f); break;` gives code gcc then cross-jumps into that block
 * -- correct instructions, but it leaves a second coincident label behind and
 * the stream is one line long. A `goto` to the join makes it one label, and
 * case 0x14 reaches it by C fallthrough.
 *
 * The case order is off the table and is not numeric: 0xa/0xb/0xc, 0x14,
 * 0x1d/0x20/0x23, then 0x15 LAST -- case 0x15 has its own copy of the
 * ClearFlag tail (`.L6c0`), which is what puts it after the shared one.
 *
 * The stored 0x209 at the top is derived by gcc from the 0x1c0 offset already
 * in a register; it is not written that way.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneWait(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);
extern void OvlFunc_888_200b270(void);
extern void OvlFunc_888_200888c(void);

int OvlFunc_888_20085cc(void)
{
    char *p;
    int x;
    int y;
    unsigned char *g;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x209;
    __Func_8091200(0x80 << 9, 0);
    __Func_8091254(1);
    __CutsceneWait(1);
    g = gState;
    switch (*(short *)(g + (0xe1 << 1))) {
    case 0xa:
    case 0xb:
    case 0xc:
        x = 0xc8 << 16;
        y = 0xa0 << 15;
        if (__GetFlag(0x855))
            __MapActor_SetPos(0xa, x, y);
        goto clear;
    case 0x14:
        OvlFunc_888_200b270();
        if (__GetFlag(0x109) == 0)
            OvlFunc_888_200888c();
    case 0x1d:
    case 0x20:
    case 0x23:
    clear:
        __ClearFlag(0x12f);
        break;
    case 0x15:
        OvlFunc_888_200b270();
        __SetFlag(0x201);
        if (__GetFlag(0x109) == 0)
            OvlFunc_888_200888c();
        __ClearFlag(0x12f);
        break;
    }
    return 0;
}
