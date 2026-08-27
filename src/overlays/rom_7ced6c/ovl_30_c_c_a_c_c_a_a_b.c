/* OvlFunc_946_20093ac  --  0x020093ac
 *
 * Cut out of goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_a_c_c_a_a.s.
 *
 * Runs the arrival cutscene for one of nine neighbouring areas -- unless its
 * "already seen" flag is set, in which case it just prints the line and plays
 * the chime.
 *
 * `_AREA_7e` IS THE WHOLE FUNCTION. The ROM holds 0x7e in r7 across three
 * calls and derives the flag id from it:
 *
 *     ldr r7, =0x7e / ldr r3, =0x8c8 / sub r3, r7 / add r0, r3
 *     ...
 *     ldrsh r3, [r5, r2] / sub r3, r7 / cmp r3, #8
 *
 * A literal 0x7e cannot produce that. 0x7e fits in `mov`, so a literal gives
 * `sub r3, #0x7e` and lets gcc fold `- 0x7e + 0x8c8` into one pooled 0x84a --
 * measured, 6 differing. Only a LINKER SYMBOL survives to the register: gcc
 * cannot fold `0x8c8 - (int)&_AREA_7e` at compile time, so it loads both and
 * subtracts, and the symbol is then live for the switch as well.
 *
 * This is the same reading `src/non_matching/ovl_7ced6c/2009494.c` established
 * for its neighbour in this overlay; that park found the shape, this is the
 * first function it closes.
 *
 * WHERE the subtraction is written matters too. Hoisting it into locals
 * (`base = (int)&_AREA_7e; d = 0x8c8 - base;`) puts both pool loads AHEAD of
 * the `ldrsh`, which the ROM does not do -- 6 differing again. Written inline
 * in the argument, the area is read first and the constants follow.
 *
 * The nine cases are in numeric order and gcc cross-jumps the pairs that share
 * a second argument by itself.
 */
extern unsigned char gState[];
extern int _AREA_7e;
extern unsigned char gOvl_0200b2bc[];
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __PlaySound(int id);
extern void __Func_8091f90(int a, int b);
extern void __Func_8091eb0(int a, int b);
extern void __Func_8091e9c(int n);
extern void __Func_8010560(unsigned char *s, int a, int b);

void OvlFunc_946_20093ac(void)
{
    unsigned char *g;
    short *e;

    g = gState;
    e = (short *)(g + (0xe0 << 1));
    if (__GetFlag(*e + (0x8c8 - (int)&_AREA_7e)) == 0) {
        __CutsceneStart();
        __Func_8091f90(*e, 5);
        g[0x22b] = 3;
        switch (*e - (int)&_AREA_7e) {
        case 0: __Func_8091eb0(0x3f, 0); break;
        case 1: __Func_8091eb0(0x3f, 1); break;
        case 2: __Func_8091eb0(0x3f, 2); break;
        case 3: __Func_8091eb0(0x3f, 3); break;
        case 4: __Func_8091eb0(0x54, 0); break;
        case 5: __Func_8091eb0(0x54, 1); break;
        case 6: __Func_8091eb0(0x54, 2); break;
        case 7: __Func_8091eb0(0x54, 3); break;
        case 8: __Func_8091eb0(0x54, 4); break;
        }
        __CutsceneEnd();
    } else {
        __Func_8010560(gOvl_0200b2bc, 0x2c, 7);
        __PlaySound(0xb7);
        __Func_8091e9c(3);
    }
}
