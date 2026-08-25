/* OvlFunc_946_2009494  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_a_a.s
 * Best screen: 4 instructions in disagreeing regions, of 35 (streams same length).
 *
 * BLOCKER CLASS: argument precompute (calls.c:805) -- a compiler difference,
 * NOT fixable from C. See src/non_matching/ovl_780898/2008dc0.c.
 *
 * Both differences are the same shape, in two different calls:
 *
 *      rom   ldr r1, =0x6666 / mov r0, #0    / ldr r2, =0x3333
 *      ours  ldr r1, =0x6666 / ldr r2, =0x3333 / mov r0, #0
 *
 *      rom   mov r2, #0x10 / mov r1, #3 / neg r2, r2
 *      ours  mov r2, #0x10 / neg r2, r2 / mov r1, #3
 *
 * The cheap argument sinks past the expensive ones. Exactly the predicted
 * failing shape: a call mixing cheap constants with two or more expensive
 * values.
 *
 * THE AREA PART IS SOLVED, and that is why this park is worth reading. The
 * ROM does not COMPARE the area id here -- it does ARITHMETIC on it:
 *
 *      ldr r2, =0x7e / ldr r3, =0x8c8 / sub r3, r2 / add r0, r3 / bl __SetFlag
 *
 * i.e. the flag is `0x8c8 + (area - 0x7e)`, a per-area flag run. Written as
 * `d = 0x8c8 - (int)(&_AREA_7e); __SetFlag(v + d);` this reproduces exactly,
 * including the operand order of the subtract.
 *
 * That matters beyond this function: it shows an `_AREA_` symbol works in
 * ARITHMETIC and not only in a comparison, so the area namespace is not limited
 * to selector functions. `_AREA_7e` was added in batch 67 on comparison
 * evidence found elsewhere; this use corroborates it independently, since a
 * flag run based at an unrelated file id would be a coincidence.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_7e;
extern unsigned char gOvl_0200b2bc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int a, int x, int y);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __Func_8010560(void *p, int a, int b);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8091e9c(int a);

void OvlFunc_946_2009494(void)
{
    unsigned char *g;
    unsigned int k;
    int v;
    int d;
    int n;

    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x6666, 0x3333);
    __MapTransitionIn();
    __WaitMapTransition();
    k = 0xe0 << 1;
    g = (unsigned char *)&gState + k;
    v = *(short *)(g + (unsigned int)0);
    d = 0x8c8 - (int)(&_AREA_7e);
    __SetFlag(v + d);
    __CutsceneWait(0x1e);
    __Func_8010560(gOvl_0200b2bc, 0x2c, 7);
    n = 0x10;
    n = -n;
    __Func_8092208(0, 3, n);
    __Func_8091e9c(3);
    __CutsceneEnd();
}
