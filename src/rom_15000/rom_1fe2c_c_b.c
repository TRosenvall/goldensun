/* Cluster Func_8020088..Func_8020088 extracted from goldensun/asm/rom_15000/rom_1fe2c_c.s.
 *
 * Total .text for this TU = 62 bytes (= 0x3e).
 * Preserves the original ROM layout when slotted between
 * asm/rom_15000/rom_1fe2c_c_a.o and asm/rom_15000/rom_1fe2c_c_c.o in
 * goldensun/stage1.ld. The .rodata stays in the _c_c piece and stage1.ld's
 * .rodata line is repointed there.
 *
 * A structural twin of src/rom_15000/rom_1fe2c_b.c (Func_801ff14): the same
 * sprite-slot teardown loop with 0x89 << 2 in place of 0x8a << 1 and a
 * different task to stop. It matched on the FIRST screen by transferring that
 * function's lever unchanged -- the walking OFFSET is the pointer-typed
 * variable and the loaded base is a plain `unsigned int`, which is what puts
 * the offset first in `ldr r0, [r5, r7]`.
 *
 * See docs/elevation.md; the point of this file is that the lever is
 * transferable rather than a one-off fit.
 */
extern unsigned char *iwram_3001f2c;
extern void Func_80200cc(void);
extern void StopTask(void (*fn)(void));
extern void _DeleteSprite(void *p);

void Func_8020088(void)
{
    unsigned int base;
    unsigned char *w;
    int z;
    int i;
    int t;
    void *p;

    base = (unsigned int)iwram_3001f2c;
    w = (unsigned char *)0x89;
    StopTask(Func_80200cc);
    z = 0;
    w = (unsigned char *)((unsigned int)w << 2);
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
