/* Cluster Func_80a4754..Func_80a4754 extracted from goldensun/asm/rom_a1000/rom_a1814_c_c_c.s.
 *
 * Slotted between rom_a1814_c_c_c_a.o and the rest of stage1.ld.
 *
 * A chance-based item break: read the equipped id, check its category, roll,
 * and on success break it and play a sound.
 *
 * DO NOT HOIST THE -1. The last call's arguments interleave -- the ROM has
 * `mov r2,#1 / ldr r0,=0xb86 / neg r2,r2 / mov r1,#0` -- which looks like the
 * basic-block lever's shape, and the call IS inside a guarded block, so the
 * lever applies on paper. Applying it is 2 of 36: assigning `n = -1;` above the
 * guards moves `mov r1, #0` one instruction EARLY. The plain literal `-1` at the
 * call site matches exactly.
 *
 * The lever is for constants gcc will not place where the ROM has them. Here
 * gcc already places them correctly, and hoisting perturbs what was right. Try
 * the plain form first.
 *
 * The two offsets 0x21a and 0x174 are written as literals, not as a reused
 * variable, even though the ROM derives the second from the first with
 * `sub r2, #0xa6`. That is the batch-56 test: writing literals makes gcc
 * produce the chain itself, so the chain is gcc's.
 */
extern unsigned char *iwram_3001f2c;
extern unsigned char *_GetItemInfo(int id);
extern unsigned int Random(void);
extern void _BreakItem(int a, int b);
extern void _PlaySound(int id);
extern void Func_80a1d08(int a, int b, int c);

void Func_80a4754(void)
{
    unsigned char *p;
    unsigned char *info;
    int n;

    n = -1;
    p = iwram_3001f2c;
    info = _GetItemInfo(*(unsigned short *)(p + (0xbc << 1)) & 0x1ff);
    if (info[0xc] != 2)
        return;
    if (Random() >= (unsigned int)(0x80 << 6))
        return;
    _BreakItem(*(unsigned char *)(p + 0x21a), *(unsigned short *)(p + 0x174));
    _PlaySound(0x8a);
    Func_80a1d08(0xb86, 0, -1);
}
