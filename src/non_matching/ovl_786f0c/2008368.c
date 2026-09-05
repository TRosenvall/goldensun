/* OvlFunc_886_2008368 -- 0x02008368
 * [asm/overlays/rom_786f0c/ovl_30_c_c_c_c_c_c_c_c_c_c_a_a.s]
 *
 * Best 145 lines against the ROM's 143, 50 differing. Two residues left, and
 * one finding worth more than the function.
 *
 * THE FINDING: THE EXTERN'S ELEMENT TYPE DECIDES WHETHER gcc FOLDS A
 * SYMBOL-PLUS-OFFSET INTO THE POOL.
 *
 * The ROM reads gState twice, at gState + 0x1c2, and BOTH times it
 * materialises the bare symbol and adds the offset at run time:
 *
 *     ldr r3, =gState        (the .s spells it =0x2000240; wram.sym:3 confirms
 *     mov r2, #0xe1           gState = 0x02000240, so it is the same object)
 *     lsl r2, #1
 *     add r3, r2
 *     mov r2, #0
 *     ldrsh r3, [r3, r2]
 *
 * Written against `extern unsigned char gState[]` as
 * `*(short *)(gState + (0xe1 << 1))`, gcc folds the whole address to ONE pool
 * word `=gState+450` -- three instructions instead of six, at each of the two
 * sites. That is 139 lines against 143: SHORT, not long, which is the tell.
 *
 * Written against `extern short gState[]` as `gState[0xe1]`, gcc does NOT fold,
 * and emits the ROM's base-and-add at both sites. 139 -> 145 lines, 54 -> 50
 * differing.
 *
 * FORCING THE BASE THROUGH A NAMED LOCAL IS INERT. `*(short *)((g = gState) +
 * (0xe1 << 1))` measures 145 lines and 50 differing -- BYTE-IDENTICAL to the
 * subscript form. So it is not "give the symbol a register of its own"; the
 * declared element type is doing the work, presumably because the byte-offset
 * form reaches expand as (plus (symbol_ref) (const_int)) ready to fold while
 * the subscript keeps the scaling separate. Cheap to try on any function that
 * comes out SHORT with a folded `=sym+offset` where the ROM has a bare symbol.
 *
 * WHAT IS STILL WRONG (both in the 145-line version):
 *
 *  - AN EXTRA `b L2` IMMEDIATELY BEFORE `L2:`, a jump to the next instruction
 *    that jump.c would normally delete. It sits at the join of the first
 *    `if (__GetFlag(0x834))` block, so the block structure written here is not
 *    the block structure the ROM has. This is the +2 over the ROM's length and
 *    it drags the label numbering, which is most of the 50.
 *  - A POOLED `1`: `ldr r2, =0x1` where the ROM has `mov r2, #0x1`, for the
 *    `strh` of 1 through (&iwram_3001ebc)[3] + 0x1f84. Pooling a one-byte
 *    immediate means that store's address arithmetic is being built in a way
 *    that costs the register the immediate would use.
 *
 * WHAT IS ALREADY RIGHT, so it does not need re-deriving:
 *  - RETURN TYPE IS int, RETURNING 0. The `pop {r1} / bx r1` epilogue against
 *    `mov r0, #0` is the documented live-return-value tell.
 *  - THE SECOND GLOBAL IS DERIVED OFF THE FIRST. r5 holds &iwram_3001ebc and
 *    `[r5, #0xc]` reaches iwram_3001ec8; written as `(&iwram_3001ebc)[3]` this
 *    keeps the single pool entry the ROM has.
 *  - 0x209 IS DERIVED FROM 0x1c0. The ROM does `mov r2,#0xe0 / lsl r2,#1 /
 *    add r3,r2 / add r2,#0x49`: the stored value is built by adding 0x49 to
 *    the offset register. Writing the store plainly gets this for free.
 *  - THE ACTOR POINTER IS NOT NAMED, so `add r0, #0x59` may clobber r0 --
 *    0x59 is past the 5-bit byte-offset range, so no [r0, #0x59] form exists.
 *  - Fifteen consecutive __MapActor_SetPos(slot, 0, 0) calls for slots
 *    8..0x16 are separate calls in source, not a loop; the LAST one has a
 *    crossed fill (mov r1 / mov r0 / mov r2) and will want a pin.
 *
 * NEXT: fix the block structure of the first `if` so the stray `b` goes, then
 * re-measure. The two constant residues are almost certainly downstream of it.
 */
struct Actor {
    unsigned char pad00[0x59];
    unsigned char f59;
};

extern char *iwram_3001ebc;
extern short gState[];

extern int __GetFlag(int id);
extern void __StartThunder(void);
extern void __WaitFrames(int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Func_8095240(void);
extern void __Func_8095268(void);
extern void __Func_80118a8(int a);
extern void OvlFunc_886_2008658(void);

int OvlFunc_886_2008368(void)
{
    *(int *)(iwram_3001ebc + 0x1c0) = 0x209;
    if (__GetFlag(0x834)) {
    __MapActor_SetPos(0x8, 0, 0);
    __MapActor_SetPos(0x9, 0, 0);
    __MapActor_SetPos(0xa, 0, 0);
    __MapActor_SetPos(0xb, 0, 0);
    __MapActor_SetPos(0xc, 0, 0);
    __MapActor_SetPos(0xd, 0, 0);
    __MapActor_SetPos(0xe, 0, 0);
    __MapActor_SetPos(0xf, 0, 0);
    __MapActor_SetPos(0x10, 0, 0);
    __MapActor_SetPos(0x11, 0, 0);
    __MapActor_SetPos(0x12, 0, 0);
    __MapActor_SetPos(0x13, 0, 0);
    __MapActor_SetPos(0x14, 0, 0);
    __MapActor_SetPos(0x15, 0, 0);
    __MapActor_SetPos(0x16, 0, 0);
        __StartThunder();
        *(short *)((&iwram_3001ebc)[3] + 0x1f84) = 1;
        __Func_8095240();
        __WaitFrames(0x1e);
        __MapTransitionIn();
        __WaitMapTransition();
        __Func_8095268();
    }
    if (__GetFlag(0x87a)) {
        if (gState[0xe1] == 6 && !__GetFlag(0x81d))
            OvlFunc_886_2008658();
        __MapActor_GetActor(0xa)->f59 |= 0x80;
    }
    if (gState[0xe1] == 2 && __GetFlag(0x815)) {
        __MapActor_SetPos(0xd, 0xe3 << 17, 0x96 << 16);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
        __MapActor_SetAnim(0xd, 5);
        __Func_80118a8(4);
    }
    return 0;
}
