/* Cluster OvlFunc_917_20082ec..OvlFunc_917_20082ec extracted from
 * goldensun/asm/overlays/rom_7a4370/ovl_30_c_c_c_a.s (3 functions; the other two stay in assembly).
 *
 * `.L1dd0` is reached as `extern int L1dd0 __asm__(".L1dd0");` -- the idiom is already in the
 * same directory's ovl_30_c_c_c_c_a_a_a.c for the same word.
 *
 * 155 instructions. Never attempted before batch 280. FOUR PIN SITES at greedy-drop fixpoint (drop costs 18 / 22 / 3 / 2) -- one fakematch row.
 * A fifth pin the precedent file uses was DROPPED: with the first site pinned, the second use of the
 * same constant reloads on its own, 0 either way. The ladder was re-run after the drop. No flags.
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
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __StartTask(void *fn, int arg);
extern void __StopTask(void *fn);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8092b08(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_8092158(int a, int b, int c);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __ActorMessage(int a, int b);
extern void __PlayMapMusic(void);
extern void __MapActor_Jump(int slot, int a, int b);
extern void OvlFunc_917_2009218(void);
extern int L1dd0 __asm__(".L1dd0");

#define PIN2 register int q0 __asm__("r0"); \
             register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

void OvlFunc_917_20082ec(void)
{
    unsigned char *a;
    unsigned char *p;
    int v;
    int w;

    a = __MapActor_GetActor(0);
    __Func_8092adc(0, 0xc000, 0);
    __Func_8091200(0x406218, 1);
    __Func_8091254(0x14);
    __WaitFrames(0x28);
    __PlaySound(0x11);
    L1dd0 = 1;
    __StartTask((void *)OvlFunc_917_2009218, 0xc80);
    __WaitFrames(0x1e);
    L1dd0 = 0;
    __Func_80933f8(0xa4 << 17, -1, 0xeb0000, 1);
    __Func_8092b08(0, 1);
    p = __MapActor_GetActor(0) + 0x5a;
    *p = *p & 0xfe;
    __MapActor_SetAnim(0, 0x10);
    {
        register int s1 __asm__("r1");
        register int s2 __asm__("r2");
        register int s0 __asm__("r0");
        s1 = 0x80;
        s2 = 0x80;
        s1 <<= 10;
        s2 <<= 10;
        s0 = 0;
        __MapActor_SetSpeed(s0, s1, s2);
    }
    __PlaySound(0x85);
    *(int *)(a + 0x28) = 0xa0 << 11;
    *(int *)(a + 0x48) = 0x80 << 7;
    *(int *)(a + 0x44) = 0xa0 << 8;
    { PIN3; q2 = 0x81; q0 = 0; q1 = 0x14f; q2 <<= 1; __Func_8092158(q0, q1, q2); }
    while (*(int *)(a + 0x28) >= 0) {
        __WaitFrames(1);
    }
    do {
        __WaitFrames(1);
    } while (*(int *)(a + 0x28) <= 0);
    __PlaySound(0xa1);
    __MapActor_SetAnim(0, 0x13);
    __CutsceneWait(0x78);
    v = 0x80 << 7;
    __StopTask((void *)OvlFunc_917_2009218);
    __WaitFrames(0x28);
    *(int *)(a + 0x44) = v;
    p = __MapActor_GetActor(0) + 0x5a;
    { int t = 1; t |= *p; *p = t; }
    __CutsceneWait(0x50);
    __MessageID(0x14cc);
    { PIN3; q2 = 0x14; q0 = 0x200e; q1 = 0; __Func_8093040(q0, q1, q2); }
    __Func_80925cc(0, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x200e, 0);
    __PlayMapMusic();
    { PIN2; q0 = 0x80; q0 <<= 9; q1 = 1; __Func_8091200(q0, q1); }
    __Func_8091254(0x14);
    __WaitFrames(0x28);
    w = 0xc0 << 8;
    *(unsigned short *)(a + 6) = w;
    *(int *)(a + 0x48) = 0x80 << 9;
    *(int *)(a + 0x44) = v;
    __Func_80925cc(0, 2);
    __CutsceneWait(0x28);
    __MapActor_Jump(0, 4, 0);
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(0x14);
}
