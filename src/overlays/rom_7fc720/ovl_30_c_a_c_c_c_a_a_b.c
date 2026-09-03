/* Cluster OvlFunc_973_20080ec..OvlFunc_973_20080ec extracted from
 * goldensun/asm/overlays/rom_7fc720/ovl_30_c_a_c_c_c_a_a.s.
 *
 * The level-up allocation box: draw three headings, then loop redrawing the
 * stat panel whenever a key changed it, until B closes the box and recalculates
 * all four party members.
 *
 * TWO LEVERS, and the second is a new one worth keeping.
 *
 * 1. `_MSG_c20` has to be a symbol. 0xc20 is 0xc2 << 4, so a plain literal is
 *    built with `mov / lsl` where the ROM pools it -- and the knock-on is much
 *    larger than those two instructions. Once the id lives in a register rather
 *    than a pool slot, cse's `related_value` stops deriving 0xc21 and 0xc22
 *    from the base, so the ROM's `add r0, r5, #1` / `add r5, #2` pair goes too
 *    and the whole r5/r6/r7 assignment rotates. 27 aligned regions with the
 *    literal; 13 with the symbol.
 *
 * 2. AN INITIALISER IS NOT AN ASSIGNMENT, for allocation purposes. Writing
 *    `int redraw; ... redraw = 1;` before the loop and writing
 *    `int redraw = 1;` produce the SAME instruction at the SAME index -- and
 *    different register allocation. The initialiser makes the flag's first def
 *    function entry, which lengthens its live range, which lowers its
 *    `n_refs/live_length` priority in `allocno_compare`, which demotes it from
 *    r5 to r7 and lets the CSE-hoisted `&gKeyPress` take r5. That is the ROM.
 *    13 aligned regions to 3, for one character.
 *
 *    This is Blocker 2 (register birth order) with a source-level control the
 *    notebook did not have: to LOWER a local's allocation priority, move its
 *    first definition to the declaration.
 *
 * The last region closed on `redraw = 0;`. Anywhere in the guarded block except
 * after the final `bl` gives the ROM: sched2 sinks the store freely down into
 * the argument block, but will not hoist it back over a call. Placed at the top
 * of the block, which is the idiomatic dirty-flag spelling, it lands exactly
 * where the ROM has it. Exact.
 *
 * Return types came off the argument fill order and did not need revisiting:
 * r0 filled last at __GetUnit, __CreateUIBox and __CloseUIBox, first at the
 * other eight.
 */
extern unsigned char gState[];
extern int _MSG_c20;
extern volatile int gKeyPress;
extern unsigned char s_Lv_973__020088d0[];

extern void *__GetUnit(int party);
extern void *__CreateUIBox(int a, int b, int c, int d, int e);
extern void __DrawSmallText(int id, void *box, int x, int y);
extern void __Func_8016498(void *box);
extern void __Func_801e8b0(void *unit, void *box, int c, int d);
extern void __UIDrawText(unsigned char *text, void *box, int x, int y);
extern void __Func_801ea08(unsigned int a, int b, void *box, int d, int e);
extern void OvlFunc_973_20080c0(int levels);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern int __CloseUIBox(void *box, int b);
extern void __CalcStats(int who);

void OvlFunc_973_20080ec(void)
{
    unsigned char *g;
    unsigned char *unit;
    void *box;
    int redraw = 1;

    g = gState;
    unit = __GetUnit(*(int *)(g + 0x1f4));
    box = __CreateUIBox(0, 0, 0x1e, 9, 2);
    __DrawSmallText((int)&_MSG_c20, box, 0, 0);
    __DrawSmallText((int)&_MSG_c20 + 1, box, 0, 0x10);
    __DrawSmallText((int)&_MSG_c20 + 2, box, 0, 0x20);
    for (;;) {
        if (redraw) {
            redraw = 0;
            __Func_8016498(box);
            __Func_801e8b0(unit, box, 0, 0x30);
            __UIDrawText(s_Lv_973__020088d0, box, 0x30, 0x30);
            __Func_801ea08(unit[0xf], 0, box, 0x48, 0x30);
        }
        if ((gKeyPress & 8) || (gKeyPress & 4)) {
            OvlFunc_973_20080c0(5);
            __PlaySound(0x5d);
            redraw = 1;
        }
        if (gKeyPress & 1) {
            OvlFunc_973_20080c0(1);
            __PlaySound(0x5b);
            redraw = 1;
        }
        if (gKeyPress & 2) {
            __PlaySound(0x71);
            __Func_8016498(box);
            __WaitFrames(1);
            __CloseUIBox(box, 1);
            __CalcStats(0);
            __CalcStats(1);
            __CalcStats(3);
            __CalcStats(2);
            return;
        }
        __WaitFrames(1);
    }
}
