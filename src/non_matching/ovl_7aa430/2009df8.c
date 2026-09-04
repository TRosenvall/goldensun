/* OvlFunc_923_2009df8  --  0x02009df8  [asm/overlays/rom_7aa430/ovl_1a3c_a_c_a.s]
 *
 * NOT MATCHING. Best 94 of 89 -- ours is 100 lines against the ROM's 89 and the
 * excess is a REGISTER SPILL, so the diff count is not meaningful past the
 * prologue. The blocker is pressure, not spelling.
 *
 * Creates two actors from one table entry, wires a script and some sprite
 * fields onto each, plays a sound. The ROM pushes r5, r6, r7 and uses r4 free
 * (that is `-fcall-used-r4`, already documented); ours pushes r5, r6, r7 AND
 * spills r8 with `mov r7, r8 / push {r7}` and a matching teardown -- six
 * instructions the ROM does not spend.
 *
 * ONE THING WAS FOUND AND IS WORTH KEEPING. The table entry is reached through
 * `gState`, and it MUST use the array idiom. Written with the two globals
 * inlined into one expression, gcc folds the base and the offset into a single
 * pool word -- `ldr r3, =gState+500` -- where the ROM loads the base
 * (`ldr r3, =0x2000240`) and adds a computed offset. That is the same finding
 * as src/non_matching/ovl_7ced6c/2008f70.c records, confirmed here from the
 * other direction: inlining the global is what triggers the fold, so the named
 * local is not stylistic.
 *
 * MEASURED, three forms, all spilling:
 *
 *     w and g as named locals, e/a/s/p/z at function scope     99 differ, 100 lines
 *     w and g inlined into the e expression                    94 differ,  95 lines
 *                                                              (and gState folds)
 *     array idiom restored, p and z scoped into their blocks   99 differ, 100 lines
 *
 * Scoping the short-lived locals into their blocks is INERT -- byte-identical
 * to leaving them at function scope -- so the pressure is not coming from them.
 * The live set that matters is the table entry, the actor and the sprite, which
 * is three values across two calls, and that is exactly what the ROM keeps.
 *
 * NEXT: find what gcc is keeping that the ROM is not. The obvious candidate is
 * the three `*(int *)(e + N)` arguments to __CreateActor, which the ROM reloads
 * at the second call site from the same `e`; if gcc is caching any of them
 * across the first block that would account for the fourth register. Reading
 * the generated .s for what actually lands in r8 is the next step and was not
 * done -- the function was set aside to finish others in the round.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern unsigned char gScript_923__0200a7c4[];

extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Sprite_SetAnim(unsigned char *s, int n);
extern void __PlaySound(int id);

void OvlFunc_923_2009df8(void)
{
    unsigned char *w;
    unsigned char *g;
    unsigned char *e;
    unsigned char *a;
    unsigned char *s;

    w = iwram_3001ebc;
    g = gState;
    e = *(unsigned char **)(w + (*(int *)(g + (0xfa << 1)) << 2) + 0x14);
    a = __CreateActor(0x1a, *(int *)(e + 8), *(int *)(e + 0xc),
                      *(int *)(e + 0x10));
    if (a != 0) {
        *(int *)(a + 0x14) = *(int *)(e + 0x14);
        s = *(unsigned char **)(a + 0x50);
        __Actor_SetScript(a, gScript_923__0200a7c4);
        {
            unsigned char *p = a + 0x55;
            *p = 0;
            p += 0xf;
            *(short *)p = 0;
        }
        *(unsigned char **)(a + 0x68) = e;
        if (s != 0) {
            int z = 0xd;
            __Sprite_SetAnim(s, 2);
            s[0x26] = 0;
            s[9] = (s[9] & -z) | 4;
        }
    }
    a = __CreateActor(0x1a, *(int *)(e + 8), *(int *)(e + 0xc),
                      *(int *)(e + 0x10));
    if (a != 0) {
        *(int *)(a + 0x14) = *(int *)(e + 0x14);
        s = *(unsigned char **)(a + 0x50);
        __Actor_SetScript(a, gScript_923__0200a7c4);
        {
            unsigned char *p = a + 0x55;
            *p = 0;
            p += 0xf;
            *(short *)p = 0;
        }
        a[0x23] = 2;
        *(unsigned char **)(a + 0x68) = e;
        if (s != 0) {
            __Sprite_SetAnim(s, 1);
            s[0x26] = 0;
        }
    }
    __PlaySound(0x82);
}
