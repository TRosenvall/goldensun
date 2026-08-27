/* OvlFunc_898_20087ec  --  0x020087ec
 * OvlFunc_898_200885c  --  0x0200885c
 * asm/overlays/rom_793768/ovl_314_c_c_a_c_a_c_c_c_a_a_c.s
 *
 * Source asm: goldensun/asm/overlays/rom_793768/ovl_314_c_c_a_c_a_c_c_c_a_a_c.s
 *
 * BLOCKER CLASS: literal pool PLACEMENT, four bytes out.
 * Status: 44 lines against 44 for each, with three of the four differences
 * being one instruction sliding past the pool and the fourth cosmetic.
 *
 *     rom    bl __CutsceneEnd / b .L848 / <pool> / .L848: / ldrh r2, [r5]
 *     ours   b L0 / <pool> / L0: / bl __CutsceneEnd / ldrh r2, [r5]
 *
 * The instruction streams are otherwise identical; the pool sits one 4-byte
 * `bl` too early.
 *
 * THE SYMBOL IS RIGHT AND IT IS WHAT MOVES THE POOL. The ORed 2 needs
 * `_CONST_2` from const.sym for the same reason batch 83 established -- only
 * the symbol gives both the pool load and the ROM's `orr` operand order -- and
 * the three positions measured are:
 *
 *     `*p |= 2`                       pool AFTER `ldrh r2, [r5]`   (6 differ)
 *     `two = (u16)(int)&_CONST_2`     pool BEFORE `bl __CutsceneEnd` (4 differ)
 *     rom                             pool BETWEEN them
 *
 * so the ROM's placement is between the two spellings and neither reaches it.
 * The dump point is `create_fix_barrier`'s, and it moves with the MODE of the
 * pool's first entry: a wider reference lets gcc scan further before
 * manufacturing the barrier. There is no third mode to try.
 *
 * ALSO MEASURED, all 4 of 44: `int two`, `*p |= two`, `*p = *p | two`,
 * `unsigned short _CONST_2` as the extern's type. Inlining the whole
 * expression is 12 differ and 42 lines.
 *
 * The two functions are identical apart from slot (0xe/0xf) and message id
 * (0x122c/0x122d), so whatever fixes one fixes both.
 */
struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x5c];
    unsigned short f64;
};

extern int _CONST_2;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int a);
extern void __WaitFrames(int n);
extern void OvlFunc_898_200973c(int a, int b, int c);
extern void OvlFunc_898_2009724(int a, int b);

void OvlFunc_898_20087ec(void)
{
    struct A *a;
    unsigned short *p;
    int two;
    short saved;

    a = __MapActor_GetActor(0xe);
    saved = a->f6;
    p = &a->f64;
    two = (int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    __MessageID(0x122c);
    __MapActor_SetAnim(0xe, 0);
    OvlFunc_898_200973c(0xe, 0, 2);
    OvlFunc_898_2009724(0xe, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *p &= 1;
}
