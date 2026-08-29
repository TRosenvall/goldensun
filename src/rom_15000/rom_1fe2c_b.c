/* Cluster Func_801ff14..Func_801ff14 extracted from goldensun/asm/rom_15000/rom_1fe2c.s.
 *
 * Total .text for this TU = 62 bytes (= 0x3e).
 * Preserves the original ROM layout when slotted between
 * asm/rom_15000/rom_1fe2c_a.o and asm/rom_15000/rom_1fe2c_c.o in
 * goldensun/stage1.ld. The .rodata stays in the _c piece and stage1.ld's
 * .rodata line is repointed there.
 *
 * THE LEVER IS WHICH OPERAND IS THE POINTER. The ROM addresses the table with
 * the OFFSET first:
 *
 *      ldr r0, [r5, r7]      r5 = the walking offset, r7 = the loaded base
 *      str r3, [r5, r7]
 *
 * Written the natural way -- `base` a pointer, `off` an integer -- gcc emits
 * `[r7, r5]`, the base first. Reversing the source's addition, `off + base`
 * instead of `base + off`, DOES NOTHING: gcc canonicalises pointer-plus-integer
 * and the output is byte-identical.
 *
 * What decides it is the TYPES. Make the walking offset the pointer-typed
 * variable and the loaded global a plain integer, and the operands swap:
 *
 *      unsigned int   base = (unsigned int)iwram_3001f2c;
 *      unsigned char *w    = (unsigned char *)0x8a;
 *      ... *(void **)(w + base)
 *
 * That was the only difference between 2 of 29 and an exact match.
 *
 * The rest follows the ROM's statement order: 0x8a is assigned BEFORE the
 * StopTask call and shifted after it, matching `mov r5, #0x8a / bl StopTask /
 * ... / lsl r5, #1`, and `t = z;` reproduces `mov r3, r8` -- r8 is not
 * addressable by `str`, so the copy is forced.
 */
extern unsigned char *iwram_3001f2c;
extern void Func_801ff58(void);
extern void StopTask(void (*fn)(void));
extern void _DeleteSprite(void *p);

void Func_801ff14(void)
{
    unsigned int base;
    unsigned char *w;
    int z;
    int i;
    int t;
    void *p;

    base = (unsigned int)iwram_3001f2c;
    w = (unsigned char *)0x8a;
    StopTask(Func_801ff58);
    z = 0;
    w = (unsigned char *)((unsigned int)w << 1);
    i = 3;
    do {
        p = *(void **)(w + base);
        if (p != 0) {
            _DeleteSprite(p);
            t = z;
            *(int *)(w + base) = t;
        }
        i--;
        w += 4;
    } while (i >= 0);
}
