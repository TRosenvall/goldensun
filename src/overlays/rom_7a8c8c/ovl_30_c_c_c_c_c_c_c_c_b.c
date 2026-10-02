/* OvlFunc_922_200a094 -- *** BYTE-IDENTICAL. LANDS. ***  452 bytes, 199
 * encodings and 24 relocations identical.  PER-FLAG FIGURE: this object needs
 * the EXISTING GCSE_CFLAGS group (-fno-gcse).  Under production -O2 the same
 * body is 1 of 199, i.e. the park's own figure.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     -e OBJCMP_EXTRA="-fno-gcse" goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7a8c8c/200a094.c \
 *     asm/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_c.s --func OvlFunc_922_200a094
 *   OK OvlFunc_922_200a094 -- 452 bytes, 199 encodings and 24 relocations identical
 * (OBJCMP_EXTRA is the harness hook; the SHIPPING form is a GCSE_CFLAGS rule.)
 *
 * SPLIT.  `python3 tools/datacheck.py asm/overlays/rom_7a8c8c/ovl_30_c_c_c_c_c_c_c_c.s`
 *   data sections : .data            functions : OvlFunc_922_200a094
 *   -> converting a function here needs a TEXT/DATA SPLIT
 *   OvlFunc_922_200a094 reads .L2464 -- *** SPLIT MUST EXPORT: .global .L2464 ***
 *
 * SHIMS: 6 register pins (tools/shimcount.py).  NEEDS A fakematch.txt ROW.
 *
 * ================= THIS BODY IS THE TWIN'S BODY, PORTED =================
 * `python3 tools/dupfuncs.py` (9 groups / 18 functions) puts this function and
 * OvlFunc_921_2009fa4 in ONE duplicate group, and the two reference streams are
 * the same 199 encodings.  So this file is ovl_7a7298/2009fa4.c with three
 * renames and nothing else: OvlFunc_921_2009fa4 -> OvlFunc_922_200a094,
 * OvlFunc_921_2009f24 -> OvlFunc_922_200a014, L2430/.L2430 -> L2464/.L2464.
 * The struct-based spelling the old park used is DISCARDED -- it reached the
 * same 199 encodings, but keeping one body for both twins is what makes the
 * next change to either of them a single edit.
 *
 * *** THE PORT WAS MEASURED, NOT ASSUMED. ***  The brief's rule, from batch 316's
 * saturated twin, is that a duplicate group is a transfer opportunity WITH WORK
 * ATTACHED.  Here it transferred exactly: all five surviving step-block pin
 * orders that are byte-identical on 2009fa4 are byte-identical here too
 * (q0,q1,q2,shift / q0,q1,shift,q2 / q1,q0,q2,shift / q1,q2,q0,shift / the
 * q1+q2-only pin).  So on THIS pair the residues were the same encoding, and
 * the overlays differ in nothing the duplicate detector cannot see.
 *
 * The mechanism, the forced-ness proof for gcse's PRE, the refutation of the
 * old "update_equiv_regs" landing story, and the measured inert/worse lists are
 * all written up ONCE, in src/non_matching/ovl_7a7298/2009fa4.c's header -- or
 * in the landed file that replaces it.  In one line: the residue was the single
 * PRE subst of `(plus sfp -12)` in the step block, that deletion is forced
 * under -fgcse, and with -fno-gcse the cure is the park's own rejected `int *vp`
 * entry (unpinned, assigned inside the loop) crossed with the step block's pin
 * order.
 *
 * CORRECTION TO THIS PARK'S HEADER, beyond the shared one: it recorded "the
 * same pin at BOTH call sites 3, first diff moves to 57 -- WORSE".  That stands
 * at -O2, but the landed body does not pin the first call site at all; what
 * mattered was never the second pin, it was taking the step block's address
 * temp down to ONE use so it is allocated r2.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned int gKeyHeld;
extern short L2464[] __asm__(".L2464");

extern unsigned char *__GetFieldActor(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern int __Func_8012038(int a, int b, int c);
extern int __Func_8011f54(int a, int b, int c);
extern void __vec3_translate(int len, int dir, int *v);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetAnimSpeed(unsigned char *a, int n);
extern void __Actor_WaitMovement(unsigned char *a);
extern void OvlFunc_922_200a014(void);

void OvlFunc_922_200a094(void)
{
    unsigned char *a;
    unsigned char *b22;
    unsigned char *f;
    int v[3];
    unsigned int base;
    unsigned int off;
    int dir;
    int first;
    int t;
    int px;
    int pz;
    register int lim __asm__("r11");
    int *vp;

    base = (unsigned int)&gState;
    off = 0xfa;
    off <<= 1;
    base += off;
    a = __GetFieldActor(*(int *)base);
top:
    dir = L2464[(gKeyHeld >> 4) & 0xf];
    if (dir << 16 == (int)0xffff0000)
        return;
    __CutsceneStart();
    lim = 0x80 << 12;
    vp = v;
    vp[0] = (*(int *)(a + 8) & 0xfff00000) + lim;
    vp[1] = *(int *)(a + 0xc);
    vp[2] = (*(int *)(a + 0x10) & 0xfff00000) + lim;
    pz = vp[2];
    px = vp[0];
    b22 = a + 0x22;
    first = __Func_8012038(*b22, px, pz);
    __vec3_translate(0x80 << 13, dir, v);
    t = __Func_8012038(*b22, vp[0], vp[2]);
    if (t == 0xff)
        goto face;
    if (__Func_8011f54(*b22, vp[0], vp[2]) - *(int *)(a + 0xc) > lim)
        goto face;
    vp[0] = px;
    vp[2] = pz;
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x1999;
    { register int h3 __asm__("r3"); h3 = 0; *(short *)(a + 0x64) = h3; }
    __Actor_TravelTo(a, px, *(int *)(a + 0xc), pz);
    __Actor_SetAnim(a, 2);
    __Actor_SetAnimSpeed(a, 0x30);
    __Actor_WaitMovement(a);
    *(int *)(a + 0x6c) = (int)OvlFunc_922_200a014;
    goto step;
face:
    *(short *)(a + 6) = dir;
    goto tail;
walk:
    if (__Func_8011f54(*b22, vp[0], vp[2]) - *(int *)(a + 0xc) > (0x80 << 12))
        goto settle;
    px = vp[0];
    pz = vp[2];
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x1999;
    __Actor_TravelTo(a, vp[0], vp[1], vp[2]);
    __Actor_WaitMovement(a);
    if (t != first)
        goto stop;
step:
    { register int q0 __asm__("r0"); register int q1 __asm__("r1");
      register int q2 __asm__("r2");
      q0 = 0x80; q1 = dir; q2 = (int)v; q0 <<= 13;
      __vec3_translate(q0, q1, (int *)q2); }
    t = __Func_8012038(*b22, vp[0], vp[2]);
    if (t != 0xff)
        goto walk;
settle:
    *(int *)(a + 0x30) = 0x80 << 10;
    *(int *)(a + 0x34) = 0x80 << 9;
    __Actor_TravelTo(a, px, *(int *)(a + 0xc), pz);
    __Actor_WaitMovement(a);
    __WaitFrames(2);
    goto top;
stop:
    *(int *)(a + 0x6c) = 0;
    f = a + 0x5a;
    { register int m3 __asm__("r3"); m3 = 1; m3 |= *f; *f = m3; }
    *(int *)(a + 0x34) = 0x80 << 7;
tail:
    __WaitFrames(0xa);
    __CutsceneEnd();
}
