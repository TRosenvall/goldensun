/* OvlFunc_923_2009208 -- MATCHES on the default flags (and unchanged under
 * -fno-rerun-cse-after-loop).  ref: asm/overlays/rom_7aa430/ovl_1150_c_c.s
 * tryc.py: OK (80 lines).  Byte-verified: 216 bytes of .text identical to the
 * ROM's, pool included (scratch/agent1/bytecheck.sh).
 *
 * THREE levers, all needed, all measured (literal form is 67 of 80):
 *  1. THREE separate `-1` locals (n1, n2, n3) assigned before the early
 *     `if (f != 0) return;`, with all three uses inside the guarded block.
 *     This is the basic-block lever against constant CSE -- gcc otherwise
 *     builds `mov r?,#1 / neg` once and copies it twice.
 *  2. s1 = 0x6666 / s2 = 0x3333 named the same way, which is what puts the
 *     ROM's `str r3,[r5,#0x6c]` BEFORE the two `ldr =0x6666 / =0x3333`
 *     (the "pool loads come first" shape, batch 105's correction).
 *  3. `m = -13;` as an int local for the bitfield mask.  Written
 *     `s[9] = (s[9] & ~0xc) | 4;` the mask narrows to a byte and gcc emits
 *     `mov r3,#0xf3` where the ROM has `mov r3,#0xd / neg r3,r3`.
 * And `p = a + 0x55;` must be written AFTER the __Func_80933f8 call, not
 * before it: written before, `add r7,#0x55` is scheduled three slots early
 * (2 of 80).
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern int  __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8092950(int a, int b);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_809202c(void);
extern void OvlFunc_923_2008cc0(void);

void OvlFunc_923_2009208(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned char *s;
    int f;
    int n1;
    int n2;
    int n3;
    int s1;
    int s2;
    int m;

    n1 = -1;
    n2 = -1;
    n3 = -1;
    s1 = 0x6666;
    s2 = 0x3333;
    a = __MapActor_GetActor(0);
    f = __GetFlag(0x109);
    if (f != 0)
        return;
    __CutsceneStart();
    __Func_80933f8(n1, n2, n3, 0);
    p = a + 0x55;
    *p = f;
    __MapActor_SetPos(0, *(short *)(a + 0xa) << 16,
                      (*(short *)(a + 0x12) << 16) + 0xfff00000);
    __Func_8092950(0, 0xf);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __MapTransitionIn();
    __WaitMapTransition();
    __PlaySound(0xe4);
    *(void **)(a + 0x6c) = (void *)OvlFunc_923_2008cc0;
    __MapActor_SetSpeed(0, s1, s2);
    __Func_8092304(0, 0, 8);
    __Func_8092950(0, 0);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 1);
    s = *(unsigned char **)(a + 0x50);
    m = -13;
    s[9] = (m & s[9]) | 4;
    __Func_8092304(0, 0, 0xa);
    *p = 3;
    *(int *)(a + 0x6c) = f;
    __Func_809202c();
    __CutsceneEnd();
}
