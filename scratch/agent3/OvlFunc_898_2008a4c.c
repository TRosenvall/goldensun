/* OvlFunc_898_2008a4c -- MATCHES.  This UNPARKS src/non_matching/overlays/2008a4c.c,
 * whose C body is unchanged below; only the verdict was wrong.
 * ref: asm/overlays/rom_793768/ovl_314_c_c_a_c_c_c_a_a.s
 *
 * tools/tryc.py reports "25 differing of 50" and that report is an ARTIFACT.
 * gcc's create_fix_barrier puts the pool skip label immediately before the
 * `if`'s own join label, so the output carries two labels at the same address:
 *
 *     ours   strh r3,[r2] / b .L5 / <pool> / .L5: / .L3: / mov r0,#0xe
 *     rom    strh r3,[r2] / b .La98 / <pool> / .La98: / mov r0,#0xe
 *
 * A label emits no bytes.  tryc.py keeps branched-to label definitions in the
 * stream (deliberately -- see "What the screen must NOT normalise away"), so
 * the extra one shifts every later position and the positional diff cascades
 * to 25.  The park note read that as "we emit two labels and two branches,
 * which is the extra instruction"; there is only one branch.
 *
 * VERIFIED AT THE BYTE LEVEL.  Assembling this file and the ROM's function
 * standalone with `arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork` gives
 * 128 bytes of .text each, and `objdump -d` differs in exactly one word: our
 * pool slot at +0x40 is 0x00000000 carrying R_ARM_ABS32 _CONST_2, where the
 * ROM has 0x00000002.  const.sym already defines `_CONST_2 = 0x2`, and an
 * absolute symbol emits no bytes, so the linked result is identical.
 * (scratch/agent3/bytecheck.sh reproduces this.)
 *
 * LESSON FOR THE SCREEN: when a diff opens at a label and everything before it
 * matches, assemble both sides before believing the count.
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
