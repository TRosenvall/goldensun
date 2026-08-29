/* OvlFunc_890_2008150  --  0x02008150
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * changes name in goldensun/overlays/rom_78b2ac/overlay.ld and nothing else
 * moves.
 *
 * Two mutually exclusive room states, each guarded by its own "already done"
 * flag: fade, set this room's flag, clear the other two.
 *
 * BUILT WITH CSE_CFLAGS, AND THAT IS THE FINDING. Written with plain literals
 * under the default flags this is 44 differing of 53, because gcc CSEs the
 * repeated flag ids -- `0x80 << 2` twice in one arm, `0x201` twice in the
 * other -- into callee-saved registers and the function grows a `push {r5}`
 * the ROM does not have. `-fno-rerun-cse-after-loop` alone matches it exactly,
 * with no change to the C.
 *
 * The BASIC-BLOCK LEVER also matches it -- five `int` locals holding constants,
 * assigned above the outer `if` and used once each -- and that is the wrong
 * answer here even though it screens OK. Five locals whose only job is to hold
 * a flag id is not source anybody wrote; a per-file flag that already exists in
 * five other places in this tree is.
 *
 * THE TWO MECHANISMS ARE DISTINGUISHABLE, which is worth more than this
 * function. Screened against the three functions the lever closed in batch 105:
 * `-fno-rerun-cse-after-loop` leaves OvlFunc_948_2009fd8 at 12 of 97,
 * OvlFunc_911_2008304 at 2 of 85 and OvlFunc_943_2008a48 at 2 of 57 -- it does
 * not reach any of them. Those are argument SCHEDULING (a constant split around
 * another argument); this is constant CSE (one value shared between two calls).
 * The lever reaches both, the flag reaches only the second. When both work,
 * prefer the flag and keep the literals.
 */
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int n);
extern int OvlFunc_890_200a5b0(void);
extern void OvlFunc_890_20089f4(void);

void OvlFunc_890_2008150(void)
{
    if (OvlFunc_890_200a5b0()) {
        if (__GetFlag(0x80 << 2) == 0) {
            __CutsceneStart();
            __Func_8091200(0x80 << 9, 1);
            __Func_8091254(0x14);
            __SetFlag(0x80 << 2);
            __ClearFlag(0x201);
            __ClearFlag(0x202);
            __CutsceneEnd();
        }
    } else {
        if (__GetFlag(0x201) == 0) {
            __CutsceneStart();
            __Func_8091200(0x2051cc, 1);
            __Func_8091254(0x14);
            __SetFlag(0x201);
            __ClearFlag(0x80 << 2);
            __ClearFlag(0x202);
            if (__GetFlag(0x80a) == 0)
                OvlFunc_890_20089f4();
            __CutsceneEnd();
        }
    }
}
