/* Cluster OvlFunc_951_20089f8..OvlFunc_951_20089f8 extracted from
 * goldensun/asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a.s.
 *
 * A coin-check: multiply a party total by ten, compare it against gState+0x10
 * UNSIGNED, and take one of two message paths.  The early-return arm jumps past
 * __CutsceneEnd to the epilogue, so it is a real `return`, not a merge.
 *
 * Three levers, all previously recorded, all needed here:
 *
 *   NAME THE gState BASE.  `*(unsigned int *)(gState + 0x10)` folds to
 *   `ldr r3, =gState+16 / ldr r2, [r3]`; through a named pointer it is
 *   `ldr r3, =gState / ldr r2, [r3, #0x10]`, the ROM's form.  5 differing -> 3.
 *
 *   __Func_8092c40 IS CALLED TWICE AND THE SITES DISAGREE.  The first wants
 *   `mov r0, #9 / mov r1, #0` and the second the reverse.  One declaration
 *   cannot do both, so the second call goes through C40I, an __asm__ alias with
 *   a different return type.  This is the third function to need that lever.
 *
 *   THE MAP ID IS A SYMBOL.  `ldr r0, =0x89` for a value an eight-bit `mov`
 *   builds is the pooled-constant tell, and __Func_8091f90's first argument is
 *   an area id -- two already-elevated files pass _AREA_35 and _AREA_51 to it.
 *   `_AREA_89 = 0x89;` added to area.sym; the entry emits no bytes.
 */
extern unsigned char gState[];
extern int ewram_2001000;
extern unsigned char *iwram_3001ebc;

extern int __Func_8077348(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __Func_8092c40(int a, int b);
extern int C40I(int a, int b) __asm__("__Func_8092c40");
extern void __Func_808ba38(void);
extern void __Func_8019908(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __SetDestMap2(int a, int b);
extern void __Func_8091f90(int a, int b);
extern int _AREA_89;

void OvlFunc_951_20089f8(void)
{
    unsigned char *p;
    unsigned char *g;
    unsigned int v;
    unsigned int n;

    n = __Func_8077348() * 10;
    __CutsceneStart();
    g = gState;
    v = *(unsigned int *)(g + 0x10);
    if (v < n) {
        __MessageID(0xe12);
        __Func_8092c40(9, 0);
        return;
    }
    ewram_2001000 = v;
    __Func_808ba38();
    __MessageID(0xe0e);
    __Func_8019908(n, 5);
    C40I(9, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __ActorMessage(9, 0);
        __Func_80921c4(0, 0x78, 0x80);
        __Func_80921c4(0, 0x78, 0x98);
        __Func_8092adc(0, 0x80 << 8, 0);
        __CutsceneWait(0x14);
        __SetDestMap2(0x1fd, 0);
        __Func_8091f90((int)&_AREA_89, 0xd);
    } else {
        p = iwram_3001ebc;
        *(unsigned short *)(p + (0xec << 1)) += 1;
        __ActorMessage(9, 0);
    }
    __CutsceneEnd();
}
