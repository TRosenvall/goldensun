/* OvlFunc_965_2009030 -- MATCHES on the default flags (and unchanged under
 * -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_7ef4f4/ovl_30_a_c_a_c.s
 * tryc.py: OK (81 lines).
 *
 * ONE lever, applied at FIVE sites at once: every two-instruction constant that
 * the ROM splits around another argument is named as its own local in the
 * function's entry block, which dominates all of them (basic-block lever).
 * e1=0x80<<1, s1=0x80<<10, s2=0x80<<9, m1=0x80<<8, m2=0x80<<7, n1=-0x10.
 * With the literals at the call sites the function is 10 of 81 and every one of
 * the ten is a `lsl` or a `neg` sitting one slot too early.  Note n1 = -0x10 is
 * a `mov`/`neg` pair, not a shift -- the lever is about two-instruction builds.
 *
 * The double range test reads `(unsigned short)(h + 0x4fff) <= 0x1fff ||
 * (unsigned short)(h - 0x3001) <= 0x1fff`; gcc spells the first as a shifted
 * compare against 0x1fff0000 and the second as lsl/lsr against 0x1fff, which is
 * exactly what the ROM has.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_809228c(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8092adc(int slot, int a, int b);
extern void OvlFunc_965_200a820(void);

void OvlFunc_965_2009030(void)
{
    unsigned char *a;
    int x;
    int h;
    int e1;
    int s1;
    int s2;
    int m1;
    int m2;
    int n1;

    e1 = 0x80 << 1;
    s1 = 0x80 << 10;
    s2 = 0x80 << 9;
    m1 = 0x80 << 8;
    m2 = 0x80 << 7;
    n1 = -0x10;
    a = __MapActor_GetActor(0);
    __CutsceneStart();
    x = *(int *)(a + 8) >> 20;
    if (x == 6 || x == 0x12) {
        if (*(int *)(a + 0x10) >> 20 == 0x14) {
            *(int *)(a + 0x38) = 0x80 << 24;
            *(int *)(a + 0x40) = 0x80 << 24;
            __MapActor_Emote(0, e1, 0x14);
            __MapActor_SetSpeed(0, s1, s2);
            __MapActor_Jump(0, 4, 0);
            h = *(unsigned short *)(a + 6);
            if ((unsigned short)(h + 0x4fff) <= 0x1fff
             || (unsigned short)(h - 0x3001) <= 0x1fff) {
                __Func_809228c(0, 0x10, 0);
                __MapActor_WaitMovement(0);
                __Func_8092adc(0, m1, 0x14);
            } else {
                __Func_809228c(0, 0, n1);
                __MapActor_WaitMovement(0);
                __Func_8092adc(0, m2, 0x14);
            }
        }
    }
    OvlFunc_965_200a820();
    __CutsceneEnd();
}
