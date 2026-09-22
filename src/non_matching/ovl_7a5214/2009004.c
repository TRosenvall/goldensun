/* OvlFunc_918_2009004 -- NON-MATCHING, 5 ENCODINGS OF 220.  Size 544 = 544,
 * relocations identical, instruction count 213 = 213.
 *
 * Blocker class: A SINGLE FOUR-INSTRUCTION sched2 INTERLEAVE.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7a5214/2009004.c \
 *     asm/overlays/rom_7a5214/ovl_314_c_c_a_c.s
 * ONE function in the reference -- it CONVERTS WHOLE, no split, no linker edit.
 *
 * THIS FILE CARRIES ITS OWN `__asm__(".set _AREA_2d, 0x2d");` SHIM so the park is
 * self-verifying.  _AREA_2d is ALREADY in area.sym:237 and nothing needs adding;
 * without the shim objcmp reports 6 encodings plus one extra relocation, with it 5
 * and relocations identical.  In-function control: 0x844, 0x109, 0x201, 0x20d,
 * 0x20f and 0x213 are all pooled plain literals in the same function.
 *
 * THE RESIDUE:
 *
 *     ref                      ours
 *     adds r3, #236            adds r3, #236
 *     ldr  r0, [r3, #0]        ldr  r0, [r3, #0]
 *     movs r5, #130            movs r5, #130
 *     movs r3, #160            (absent here)
 *     lsls r3, #16             (absent here)
 *     lsls r5, #1              lsls r5, #1
 *     add  r5, lr              add  r5, lr
 *     ldr  r4, [pc]            ldr  r4, [pc]
 *     (absent here)            movs r3, #160
 *     (absent here)            lsls r3, #16
 *     adds r0, r3              adds r0, r3
 *
 * The ROM builds the `0xa0 << 16` addend BETWEEN the two halves of
 * `w = base + (0x82 << 1)`; gcc emits it after.  Same instructions, same registers,
 * same count.
 *
 * MEASURED FLOOR: 5, ACROSS 70 SPELLINGS.  The addend as a named int before, after
 * and inside a block (5 each); 0xa00000 against 0xa0 << 16 (5);
 * `register int hi __asm__("r3")` (5); `w = base + 0x104` against `base + (0x82<<1)`
 * against a two-step local (5 each); six do{}while(0) positions (5-8, two of them
 * worth the step from 7 to 5); four orderings of `w` against the argument
 * expression (5-8); `+=` split into a named temp (5).
 *
 * ================================================================
 * `.call_via r4` MUST NOT CLOBBER `lr`, AND MUST CLOBBER r2/r3 -- the landed
 * template in the tree is WRONG IN BOTH DIRECTIONS
 * ================================================================
 *
 * src/rom_8a000/rom_97384_c_c_a_b.c lists `"memory", "lr", "r12"`.
 *
 *   - `mov r12, pc / bx r4` puts the return address in R12, SO `lr` SURVIVES.  This
 *     function proves it: the ROM keeps a pointer in `lr` ACROSS BOTH CALLS.
 *     Listing "lr" as clobbered makes gcc refuse to use it -- worth 12 regions.
 *   - The ARM callee DOES clobber r0-r3.  Without "r2","r3" gcc happily kept the
 *     base pointer in r2 across the call -- A REAL MISCOMPILE, not merely a
 *     mismatch.  Adding them is what moved the value into `lr` and matched the ROM.
 *
 * The working form, and the tree's first shared-r4 two-call site, is a MACRO over a
 * FUNCTION-SCOPE `register int (*fv)(int,int) __asm__("r4")` -- an inline function
 * rematerialises the callee address at every site.  Binding it once and assigning
 * `fv = Func_8000888;` IMMEDIATELY BEFORE THE FIRST CALL put the single
 * `ldr r4, =...` exactly where the ROM has it, worth 11 regions in one step.
 *
 * UN-PINNING A POINTER WAS WORTH 22 REGIONS, AND THE SIGN FLIPPED MID-SESSION.
 * `register short *pp __asm__("r8")` was worth 11 early (57 -> 46) and then COST 26
 * later (42 -> 20) once the surrounding roles were right.  RE-RUNNING THE DROP
 * LADDER AFTER EVERY STRUCTURAL CHANGE IS NOT OPTIONAL AT THIS SIZE -- this park
 * would have been 20 regions worse without it.
 *
 * A greedy ladder found the `zb` r6 pin and the `w` r5 pin individually AND JOINTLY
 * inert, so both are dropped.  `register int d __asm__("r5")` is load-bearing (drop
 * = 23), as is the PIN1 on the first __GetFlag(0x109) (drop = 45, and the
 * instruction count goes wrong -- gcc CSEs the flag id into a callee-saved register
 * where the ROM reloads it twice).  Path: 87 -> 57 -> 46 -> 20 -> 9 -> 5.
 *
 * .L2dd0, .L2dc0 and .L2dcc are already .global in
 * asm/overlays/rom_7a5214/ovl_314_c_c_c_c.s, so the __asm__(".L...") rename works
 * with no label.sym entry and no .s edit.
 *
 * No per-file Makefile flag override applies to this stem.
 */
extern unsigned char gState[];
extern unsigned char *L2dd0 __asm__(".L2dd0");
extern unsigned char ewram_2001000[];
extern unsigned char *iwram_3001e70[];
extern int _AREA_2d;
__asm__(".set _AREA_2d, 0x2d");

extern int Func_8000888(int a, int b);

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __SetDestMap(int map, int entrance);
extern void __MapTransitionIn(void);
extern void __StartTask(void *f, int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Surprise(int slot, int a);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8092950(int a, int b);
extern void __Func_8092b08(int slot, int a);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void OvlFunc_918_2009224(void);
extern void OvlFunc_918_2009244(void);
extern void OvlFunc_918_2009424(int a);
extern void OvlFunc_918_20097ec(void);
extern void OvlFunc_918_20098b8(void);
extern void OvlFunc_918_2008f58(int a);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

#define CALL_VIA_R4(dst, A, B) do { \
        register int _a __asm__("r0") = (A); \
        register int _b __asm__("r1") = (B); \
        __asm__ volatile ( \
            "\t.align\t2, 0\n" \
            "\tmov\tr12, pc\n" \
            "\tbx\tr4" \
            : "=r" (_a) \
            : "r" (fv), "0" (_a), "r" (_b) \
            : "memory", "r2", "r3", "r12"); \
        (dst) = _a; \
    } while (0)

int OvlFunc_918_2009004(void)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *pa;
    unsigned char *gs;
    unsigned char *base;
    unsigned char *w;
    short *pp;
    int slot;
    int v;
    register int d __asm__("r5");
    int zb;
    int r;
    int a0;
    register int (*fv)(int, int) __asm__("r4");

    p = __MapActor_GetActor(8);
    L2dd0 = ewram_2001000;
    OvlFunc_918_2009224();
    d = 0xfff60000;
    zb = 0;
    p[0x55] = zb;
    *(int *)(p + 0xc) = d;
    q = __MapActor_GetActor(9);
    q[0x55] = zb;
    *(int *)(q + 0xc) = d;
    __Func_8092950(9, 0xf);
    OvlFunc_918_2009424(0);
    gs = gState;
    pp = (short *)(gs + (0xe1 << 1));
    if (*pp != 0x13)
        __StartTask(OvlFunc_918_2009244, 0xc8 << 4);
    if (__GetFlag(0x844) != 0) {
        __MapActor_SetPos(9, 0, 0);
        __MapActor_SetPos(8, 0, 0);
    }
    { PIN1; q0 = 0x109;
      if (__GetFlag(q0) != 0)
        OvlFunc_918_20097ec(); }
    base = iwram_3001e70[0];
    a0 = *(int *)(base + 0xec) + (0xa0 << 16);
    do { } while (0);
    w = base + (0x82 << 1);
    do { } while (0);
    fv = Func_8000888;
    CALL_VIA_R4(r, a0, 0x1999);
    *(int *)(w + 8) += r;
    CALL_VIA_R4(r, *(int *)(base + 0xf0) + (0x88 << 16), 0x1999);
    *(int *)(w + 0xc) += r;
    *(int *)(w + 0x10) = 0xe666;
    *(int *)(w + 0x14) = 0xe666;
    __SetFlag(0x201);
    __SetFlag(0x20d);
    __SetFlag(0x20f);
    __SetFlag(0x213);
    __WaitFrames(1);
    OvlFunc_918_2008f58(0);
    *(int *)(iwram_3001e70[0x13] + (0xe0 << 1)) = 0x202;
    v = *pp;
    slot = *(int *)(gs + 0x1f4);
    pa = __MapActor_GetActor(slot);
    if (v == 0x32 || v == 0x28 || v == 0x1e || v == 0x14) {
        __MapTransitionIn();
        __MapActor_SetAnim(slot, 0x1b);
        __Actor_SetSpriteFlags(__MapActor_GetActor(slot), 0);
        { PIN1; q0 = slot;
          __MapActor_Surprise(q0, 0x101); }
        { PIN3; q0 = -1; q1 = -1; q2 = -1;
          __Func_80933f8(q0, q1, q2, 0); }
        pa[0x55] = 2;
        *(int *)(pa + 0xc) = 0xc8 << 15;
        *(int *)(pa + 0x14) = 0xff600000;
        *(int *)(pa + 0x48) = 0x80 << 8;
        __PlaySound(0xcc);
        __SetDestMap((int)&_AREA_2d, v - 0xa);
        __CutsceneWait(0x14);
        pa[0x22] = 2;
        __Func_8092b08(slot, 3);
        __CutsceneWait(2);
        { PIN1; q0 = slot;
          __MapActor_Surprise(q0, 0x80 << 1); }
        __CutsceneWait(8);
    } else if (v == 0xa) {
        PIN1; q0 = 0x109;
        if (__GetFlag(q0) == 0)
            OvlFunc_918_20098b8();
    } else if (v == 0x13) {
        OvlFunc_918_2008f58(1);
    }
    return 0;
}
