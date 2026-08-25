/* Cluster Func_80958e4..Func_80958e4 extracted from goldensun/asm/rom_8a000/rom_944ec_a_c_a_a_a_c.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and stage1.ld is untouched.
 *
 * Walks 24 records of 0x48 bytes, calling Func_809bb34 on each whose flag byte
 * at +0x9d is set.
 *
 * Two pointers, spelled differently on purpose because the ROM treats them
 * differently:
 *   - `f = p + 0x9d;` is a DERIVED initialiser, which is what produces the
 *     ROM's `mov r6, r5 / add r6, #0x9d` -- the copy plus the add.  Written as
 *     `f = p; f += 0x9d;` gcc coalesces the two and the copy disappears.
 *     See docs/elevation.md.
 *   - `p += 0x58;` is destructive, matching `add r5, #0x58`: the base pointer
 *     itself becomes the walking one.
 *
 * The flag test is a shift, not a mask: `t = *f; t <<= 24; if (t != 0)`
 * reproduces `ldrb r3, [r6] / lsl r3, #24 / cmp r3, #0`.
 */
extern unsigned char *iwram_3001f30;
extern void Func_8095884(void);
extern void StopTask(void (*fn)(void));
extern void Func_809bb34(void *p);
extern void gfree(int n);
extern void WaitFrames(int n);

void Func_80958e4(void)
{
    unsigned char *p;
    unsigned char *f;
    int i;
    int t;

    p = iwram_3001f30;
    StopTask(Func_8095884);
    f = p + 0x9d;
    p += 0x58;
    i = 0x17;
    do {
        t = *f;
        t <<= 24;
        f += 0x48;
        if (t != 0)
            Func_809bb34(p);
        i--;
        p += 0x48;
    } while (i >= 0);
    gfree(0x38);
    WaitFrames(1);
}
