/* OvlFunc_898_2008a4c
 *
 * Cut out of goldensun/asm//overlays/rom_793768/ovl_314_c_c_a_c_c_c_a_a_b.s.
 *
 * WAS PARKED, AND THE PARKED C WAS ALREADY CORRECT -- unchanged here except
 * for this note.
 *
 * tools/tryc.py screens it at 25 differing of 50 and every one of those is a
 * cascade from ONE redundant label. gcc puts the pool-skip label immediately
 * before the ifs own join label, so two label definitions land at the same
 * address:
 *
 *     ours   strh r3,[r2] / b .L5 / <pool> / .L5: / .L3: / mov r0, #0xe
 *     rom    strh r3,[r2] / b .La98 / <pool> / .La98:      / mov r0, #0xe
 *
 * A label emits no bytes. tryc deliberately keeps branched-to label
 * definitions in the stream -- which is right, and here it shifts every later
 * position and the positional count cascades.
 *
 * The lesson is symmetric to the one already in docs/elevation.md. A CLEAN
 * screen on a function with an inline pool is unproven until make compare; so
 * is a DIRTY screen whose first difference is a label. Both need the byte
 * check, and this one passes it.
 *
 * Screened by a parallel agent; re-verified here before wiring.
 */
struct A {
    unsigned char pad00[6];
    short f6;
    unsigned char pad08[0x64 - 8];
    unsigned short f64;
};

extern int _CONST_2;
extern char *iwram_3001ebc;
extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __MapActor_SetAnim(int slot, int n);
extern void __WaitFrames(int n);
extern void OvlFunc_898_200973c(int a, int b, int c);
extern void OvlFunc_898_2009724(int a, int b);

void OvlFunc_898_2008a4c(void)
{
    struct A *a;
    unsigned short *p;
    unsigned short *q;
    unsigned short two;
    short saved;

    a = __MapActor_GetActor(0xe);
    p = &a->f64;
    saved = a->f6;
    two = (unsigned short)(int)&_CONST_2;
    *p = two | *p;
    __CutsceneStart();
    __MessageID(0x1339);
    if (__GetFlag(2)) {
        q = (unsigned short *)(iwram_3001ebc + (0xec << 1));
        *q = *q + 1;
    }
    __MapActor_SetAnim(0xe, 0);
    OvlFunc_898_200973c(0xe, 0, 2);
    OvlFunc_898_2009724(0xe, 0xa);
    a->f6 = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *p = *p & 1;
}
