/* Cluster OvlFunc_969_200b924..OvlFunc_969_200b924 extracted from
 * goldensun/asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_c_c_a.s (2 functions; the sibling
 * OvlFunc_969_200bbc8 is parked at 3 differing -- see src/non_matching/ovl_7f6e64/200bbc8.c, whose
 * residue is a pool-placement decision about sixteen bytes below gcc's own mid-function break
 * threshold).
 *
 * 266 instructions. Never attempted before batch 280. FOUR PIN SITES, all load-bearing (drop costs 7 / 2 / 2 / 4) -- one fakematch row. The named
 * `int` stack-argument pairs for the two six-argument calls are ORDINARY C rather than scaffolding,
 * and are also load-bearing at 3 each: both stack slots are in registers before either `str`, so
 * both get named, in the ROM's mov order -- WHICH DIFFERS BETWEEN THE TWO CALLS. No flags.
 *
 * ===== PUT EACH `mov` ADJACENT TO ITS OWN `lsl` INSIDE A PINNED ARGUMENT BLOCK (new) =====
 *
 * This is what closed one of the two functions landed together here, and it is new. Inside a pinned
 * argument block, write each value and its shift as a PAIR:
 *
 *     q0 = 0xc0; q0 <<= 16; q2 = 0xee; q2 <<= 16; ...
 *
 * gcc's sched2 then spreads them back to the ROM's positions. GROUPING the movs and then the shifts
 * gets the movs in REVERSE order, and no permutation of the grouped form reaches it -- six other
 * orderings tied at 2 differing.
 *
 * And the winning shape is not always the ROM's apparent emission order: for one callee it is `q0`
 * FIRST and then each value+shift pair, which is the OPPOSITE of what the listing shows, with five
 * other orderings all tied at 4. So write pairs, then try the orders -- do not read the order off
 * the ROM and stop there.
 *
 * ===== `|=` ON A BYTE NEEDS THE CONSTANT AS THE ACCUMULATOR -- AND `&` DOES NOT =====
 *
 * The ROM has `ldrb r2 / mov r3,#1 / orr r3,r2`, tying the OR's output to the constant's register.
 * `*p |= 1`, `*p = *p | 1` and `*p = 1 | *p` ALL give the reversed pair (2 differing);
 * `{ int t = 1; t |= *p; *p = t; }` is exact.
 *
 * THE ASYMMETRY IS THE POINT: `*p = *p & 0xfe` needs no such treatment, because gcc commutes the
 * AND on its own. So the recorded "reuse a variable as its own accumulator" lever applies to OR and
 * not to AND, and the two are worth testing separately.
 *
 * ===== SPLIT A REUSED POINTER TO SHORTEN LIVE RANGES AND FREE A CALLEE-SAVED REGISTER =====
 *
 * On the sibling function this was worth 56 differing to 15 -- the whole frame defect. One actor
 * pointer reused across two blocks is 20 references across 49 insns, so two other locals beat it to
 * r5/r6 and it took r7, a SIXTH callee-saved register the ROM does not push. Two separate pointers
 * shorten both ranges enough that they land in r5, and the ROM's copies appear.
 *
 * Entry point: FRAME LARGER THAN THE ROM'S -> live ranges. And `-ffixed-r7` is NOT the answer -- it
 * displaces the value to r11 and goes 56 to 77.
 *
 * ===== A `.word` RUN IS NOT NECESSARILY DATA =====
 *
 * Worth recording because it cost a target in this batch: an in-function run of `.word .L<hex>`
 * entries following `ldr rN, =.L<table> / lsl / ldr / mov pc, rN` is GCC'S OWN SWITCH TABLE, not
 * data needing a text/data split. `tools/datacheck.py` looks only for `.section .rodata/.data/.bss`
 * and is RIGHT to ignore these. A 31-case jump table with three fallthroughs and a cross-jumped
 * shared tail came out of ordinary C written in the ROM's case order in this same batch.
 *
 * Also confirmed here: `strh` of a halfword constant pools unless the value passes through an `int`
 * local first -- the HImode anti-tell, which recurred five and eight times in two neighbours.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned char *__Func_8093554(void);
extern void __CutsceneStart(void);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_800fe9c(void);
extern void __DeleteFieldActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void OvlFunc_969_200d688(void *a);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8092950(int a, int b);
extern void __StartTask(void *fn, int arg);
extern void OvlFunc_969_200da28(void);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_969_200cbec(void);
extern void __SetFlag(int id);
extern void __Func_8091e9c(int n);
extern unsigned char *iwram_3001ebc;

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

void OvlFunc_969_200b924(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int e, f, g;
    int y, z, d, w;
    int m, n;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    p = __Func_8093554() + 0x55;
    *p = 0;
    n = 0x17;
    m = 0x12;
    __CopyMapTiles(0x66, 4, 0x4a, 4, m, n);
    m = 0x10;
    n = 0x14;
    __CopyMapTiles(0x27, 0x48, 0xb, 0x48, m, n);
    e = 0x16;
    f = 6;
    __Func_8010704(0x13, 6, 3, 7, e, f);
    g = 0xd;
    __Func_8010704(0x13, 6, 3, 7, g, f);
    __Func_8010704(0x13, 6, 3, 7, e, g);
    __Func_8010704(0x13, 6, 3, 7, g, g);
    __WaitFrames(1);
    { PIN4; q0 = 0xc0; q0 <<= 16; q2 = 0xee; q2 <<= 16; q3 = 0; q1 = 0xffc00000;
      __Func_80933f8(q0, q1, q2, q3); }
    __WaitFrames(1);
    __Func_800fe9c();
    __WaitFrames(1);
    __DeleteFieldActor(0x14);
    __DeleteFieldActor(0x13);
    __MapActor_SetAnim(0, 0x13);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    *(int *)(a + 8) = 0xad << 17;
    *(int *)(a + 0x10) = 0xcd << 16;
    y = 0x80 << 14;
    w = 0xc0 << 7;
    *(int *)(a + 0xc) = y;
    *(unsigned short *)(a + 6) = w;
    OvlFunc_969_200d688(a);
    __MapActor_SetAnim(1, 0x12);
    __Actor_SetSpriteFlags(__MapActor_GetActor(1), 0);
    b = __MapActor_GetActor(1);
    *(int *)(b + 8) = 0xb2 << 17;
    *(int *)(b + 0x10) = 0xc0 << 16;
    w = 0xa0 << 8;
    *(unsigned short *)(b + 6) = w;
    *(int *)(b + 0xc) = y;
    OvlFunc_969_200d688(b);
    __MapActor_SetAnim(2, 0x12);
    __Actor_SetSpriteFlags(__MapActor_GetActor(2), 0);
    b = __MapActor_GetActor(2);
    *(int *)(b + 8) = 0xb4 << 17;
    z = 0xde << 16;
    w = 0x80 << 6;
    *(unsigned short *)(b + 6) = w;
    *(int *)(b + 0xc) = y;
    *(int *)(b + 0x10) = z;
    OvlFunc_969_200d688(b);
    __MapActor_SetAnim(3, 0x12);
    __Actor_SetSpriteFlags(__MapActor_GetActor(3), 0);
    b = __MapActor_GetActor(3);
    *(int *)(b + 8) = 0xa7 << 17;
    w = 0x80 << 8;
    *(unsigned short *)(b + 6) = w;
    *(int *)(b + 0xc) = y;
    *(int *)(b + 0x10) = z;
    OvlFunc_969_200d688(b);
    { PIN3; q0 = 0x15; q1 = 0xc4; q1 <<= 16; q2 = 0xdc; q2 <<= 16; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetAnim(0x15, 5);
    { PIN3; q0 = 6; q1 = 0xbc; q1 <<= 16; q2 = 0x9e; q2 <<= 17; __MapActor_SetPos(q0, q1, q2); }
    __MapActor_SetAnim(6, 5);
    __Actor_SetSpriteFlags(__MapActor_GetActor(6), 0);
    b = __MapActor_GetActor(8);
    d = 0xfff00000;
    *(int *)(b + 8) = *(int *)(b + 8) + d;
    OvlFunc_969_200d688(b);
    b = __MapActor_GetActor(9);
    *(int *)(b + 8) = *(int *)(b + 8) + d;
    OvlFunc_969_200d688(b);
    b = __MapActor_GetActor(0xa);
    d = 0x80 << 13;
    *(int *)(b + 8) = *(int *)(b + 8) + d;
    OvlFunc_969_200d688(b);
    b = __MapActor_GetActor(0xb);
    *(int *)(b + 8) = *(int *)(b + 8) + d;
    OvlFunc_969_200d688(b);
    { PIN3; q2 = 0x80; q1 = 0x80; q0 = 0x80; q0 <<= 9; q1 <<= 9; q2 <<= 9; __Func_8012330(q0, q1, q2); }
    __Actor_SetSpriteFlags(__MapActor_GetActor(0x17), 0);
    p = __MapActor_GetActor(0x17) + 0x55;
    *p = 4;
    __Func_8092950(0x17, 4);
    b = __MapActor_GetActor(0x17);
    *(int *)(b + 0xc) = 0xa0 << 14;
    __StartTask((void *)OvlFunc_969_200da28, 0xc80);
    {
        unsigned char *q;
        q = iwram_3001ebc;
        *(int *)(q + (0xe0 << 1)) = 0x200;
        *(int *)(q + 0x1c8) = 0x18;
    }
    __MapTransitionIn();
    __WaitMapTransition();
    __CutsceneWait(0x28);
    OvlFunc_969_200cbec();
    __SetFlag(0x9a7);
    __Func_8091e9c(2);
}
