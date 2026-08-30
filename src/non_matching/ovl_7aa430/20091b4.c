/*
 * OvlFunc_923_20091b4 -- asm/overlays/rom_7aa430/ovl_1150_c_c_a.s
 *
 * BLOCKER: emission order of two pooled loads. 28 lines against 28, TWO
 * differing:
 *
 *      rom   ldr r3, =gState / ldr r2, =0x22b
 *      ours  ldr r2, =0x22b  / ldr r3, =gState
 *
 * Same values, same `add`, same store. Only which pool entry is loaded first.
 *
 * TRIED AND REJECTED, all measured:
 *
 *   * Naming the offset in a local (`o = 0x22b; gp[o] = 3;`) -- 27 lines, ONE
 *     SHORT, 11 differing. Naming it lets gcc fold something away.
 *   * Naming the address in a pointer local (`q = gp + 0x22b; *q = 3;`) --
 *     byte-identical to the version below.
 *   * `*(unsigned char *)(0x22b + (int)gp) = 3;` -- byte-identical.
 *   * Splitting the base as `gp = gState + 0x200; gp[0x2b] = 3;` -- 27 lines,
 *     11 differing.
 *
 * SETTLED, and it is why this is 2 rather than unmatched:
 *
 *   The map id passed to __Func_8091f90 is POOLED as 0x35, which an eight-bit
 *   `mov` could build, so it is a symbol -- `(int)&_AREA_35`, which already
 *   exists in area.sym and is semantically right here since the callee is the
 *   map loader.
 *
 *   The stored value is DERIVED from the offset: the ROM does
 *   `mov r2, #0xe0 / lsl r2, #1 / add r3, r2 / add r2, #0x40 / str r2, [r3]`,
 *   so 0x200 is 0x1c0 plus 0x40 rather than a constant of its own. Writing the
 *   store as `*(int *)(p + (0xe0 << 1)) = (0xe0 << 1) + 0x40;` reproduces it.
 *   Spelling the value 0x200 directly would need const.sym's _CONST_200 or a
 *   two-instruction build, and neither is what the ROM has.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern int _AREA_35;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80925cc(int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_8091f90(int a, int b);
extern void __Func_8091eb0(int a, int b);

void OvlFunc_923_20091b4(void)
{
    unsigned char *p;
    unsigned char *gp;

    __CutsceneStart();
    __Func_80925cc(8, 2);
    __CutsceneWait(0x14);
    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = (0xe0 << 1) + 0x40;
    __Func_8091f90((int)&_AREA_35, 0x1f);
    gp = gState;
    gp[0x22b] = 3;
    __Func_8091eb0(0x24, 1);
    __CutsceneEnd();
}
