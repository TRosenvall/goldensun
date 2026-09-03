/* OvlFunc_939_2009668  --  0x02009668
 *
 * The tail of goldensun/asm/overlays/rom_7c460c/ovl_314_c_a_c_c_c_c_c.s; the
 * two functions it calls stay in _a.s.
 *
 * A per-area entry hook: on one area it repositions a camera value, runs an
 * intro and reveals an actor; on another it repositions, sets a counter, runs
 * one of two branches on a sub-area, starts two tasks and clears ten flags.
 *
 * TWO LEVERS, and the second inverts a note recorded three commits ago.
 *
 * DELETE THE ADDRESS-ONLY POINTER LOCAL. Written `p = iwram_3001ebc;` then
 * `*(int *)(p + 0x1c0) = 0x100;`, the loaded value gets its own pseudo and takes
 * r2, which pushes the offset into r3 and blocks the scheduler from hoisting
 * `mov r2, #0xe0` above the deref -- the whole `ldr r3,[r3] / mov r2,#0xe0`
 * interleave inverts. Written as one expression the address pseudo and the
 * loaded value coalesce into r3, which is the ROM. 34 aligned to 19, fixing
 * both blocks at once. The recorded lever is for a local holding an address;
 * this confirms it for a GLOBAL POINTER DEREF, and it drives a six-instruction
 * register-birth-order mirror that reads like blocker class 2 until it closes.
 *
 * TO STOP A HALFWORD STORE CONSTANT BEING POOLED, NAME IT AS AN int IN A
 * DOMINATING BLOCK. `int t = 0xa;` in the entry block, stored through
 * `*(short *)` inside the guarded body, is worth 19 of 123 -- a literal at the
 * store gives `ldrh r3, .L2 / .word 10`, and the same assignment written inside
 * the arm gives 19. This is the INVERSE of the blocker-1b note added earlier in
 * this batch, where an int local crossing a block boundary was the thing that
 * FORCED an unwanted `ldr rN, =0xffff`. Both are the same mechanism read from
 * opposite ends: the load width follows the eventual store, so a value that must
 * exist in a register before the storing block cannot narrow. When the ROM pools
 * it, keep the value inside the store's expression; when the ROM has
 * `mov / strh`, hoist it out. 1b's rule now has a direction switch, and the ROM
 * tells you which way to set it.
 *
 * TWO SMALLER ONES. `b2 = &gState` re-taken at the second 0x1c2 read is worth 2:
 * without it gcc folds to `ldr r3, =gState+450`, where the ROM re-loads the base
 * and does a destructive `add`. The fresh load with a destructive add is the
 * tell that the source took the address again, even though the base is already
 * live in another register. And `_AREA_68` / `_AREA_9f` from area.sym are worth
 * 21; both symbols already existed.
 *
 * MEASURED INERT, worth knowing: nested `if`s against a fused `&&` (this
 * function's conditions branch to the same join, so the compound costs nothing),
 * and __StartTask declared `int` against `void`. And the ROM's two __StartTask
 * sites have DIFFERENT argument orderings -- `mov r1 / ldr r0 / lsl r1` at one,
 * `mov r1 / lsl r1 / ldr r0` at the other -- yet both fall out of the same plain
 * literal at each call site once the surrounding blocks are right. A useful
 * negative for the arg-interleave family: two contradictory orderings of the
 * same call do not require two source spellings.
 *
 * NOTE FOR THE SELECTION FILTER. filtered.py offered this as [split] and that
 * was a false positive -- the function has NO STACK FRAME at all, so there is no
 * stack argument for the split lever to act on, and splitting was actively
 * harmful (33 to 38). What it flagged was a struct offset, and gcc-2.96 Thumb
 * builds a shiftable constant with mov/lsl FROM A BARE LITERAL, so the family
 * habit of writing `off = 0xe0; off <<= 1;` is unnecessary here and costs the
 * match. Plain literal offsets on a named base are what reproduce the ROM's
 * per-block rebuild. 0x242 is genuinely pooled and is not the symbol tell.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int iwram_3001ebc;
extern int _AREA_68;
extern int _AREA_9f;

extern void OvlFunc_939_20085f0(void);
extern void OvlFunc_939_2009840(int n);
extern void OvlFunc_939_200931c(void);
extern void OvlFunc_939_20095bc(void);
extern void OvlFunc_939_2008ac4(void);
extern void OvlFunc_939_2009240(void);

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern int __StartTask(void (*fn)(void), int n);

int OvlFunc_939_2009668(void)
{
    unsigned int base;
    unsigned int b2;
    unsigned char *a;
    int t;

    base = (unsigned int)&gState;
    t = 0xa;
    if (*(short *)((char *)base + 0x1c0) == (int)(&_AREA_68)) {
        *(int *)((unsigned char *)iwram_3001ebc + 0x1c0) = 0x100;
        OvlFunc_939_20085f0();
        if (__GetFlag(0xfd1) == 0)
            OvlFunc_939_2009840(0x14);
        a = __MapActor_GetActor(8);
        if (a != 0)
            __Actor_SetSpriteFlags(a, 0);
        __SetFlag(0x201);
    }
    if (*(short *)((char *)base + 0x1c0) == (int)(&_AREA_9f)) {
        *(int *)((unsigned char *)iwram_3001ebc + 0x1c0) = 0x100;
        *(short *)((char *)base + 0x242) = t;
        if (*(short *)((char *)base + 0x1c2) == 4) {
            if (__GetFlag(0x109) == 0)
                OvlFunc_939_200931c();
        }
        b2 = (unsigned int)&gState;
        if (*(short *)(b2 + 0x1c2) == 3) {
            if (__GetFlag(0x109) == 0)
                OvlFunc_939_20095bc();
        }
        if (__GetFlag(0x941) != 0) {
            if (__GetFlag(0x94d) == 0)
                __StartTask(OvlFunc_939_2008ac4, 0xc8 << 4);
        }
        __StartTask(OvlFunc_939_2009240, 0xc8 << 4);
        __ClearFlag(0x944);
        __ClearFlag(0x945);
        __ClearFlag(0x946);
        __ClearFlag(0x947);
        __ClearFlag(0x948);
        __ClearFlag(0x943);
        __ClearFlag(0x949);
        __ClearFlag(0x94a);
        __ClearFlag(0x94b);
        __ClearFlag(0x94c);
    }
    return 0;
}
