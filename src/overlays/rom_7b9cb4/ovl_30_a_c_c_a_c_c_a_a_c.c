/* OvlFunc_932_200aa48 and OvlFunc_932_200ab58  --  0x0200aa48 / 0x0200ab58, was
 * asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_c.s (these two, no data), so it
 * converts whole; verified from this combined file with objcmp --whole. 200ab58
 * was not a target -- it closed alongside.
 *
 * Two `switch (*(int *)L523c) { case 0: ... case 0x8000: ... }` and every a+0x64
 * access written straight on the field; the pointer in r6 the ROM builds even on
 * the default path is GCSE's, not the source's. 200ab58: `int *q = (int *)L5240`
 * and a `short *p` walk with v1, then v2 assigned after `p++`.
 */
extern unsigned char L523c[] __asm__(".L523c");
extern unsigned int __Random(void);
extern void __Func_80929d8(void *a, int n);
extern void __DeleteActor(void *a);

void OvlFunc_932_200aa48(unsigned char *a)
{
    switch (*(int *)L523c) {
    case 0:
        *(int *)(a + 8) += (*(short *)(a + 0x64) << 12) + ((int)((((__Random() << 1) >> 16) - 1) << 16) >> 1);
        break;
    case 0x8000:
        *(int *)(a + 8) -= (*(short *)(a + 0x64) << 12) + ((int)((((__Random() << 1) >> 16) - 1) << 16) >> 1);
        break;
    }
    if (*(short *)(a + 0x64) <= 3) {
        switch (*(int *)L523c) {
        case 0:
            *(int *)(a + 8) += 0x8000;
            break;
        case 0x8000:
            *(int *)(a + 8) += -0x8000;
            break;
        }
        *(int *)(a + 0x18) += 0x1999;
        *(int *)(a + 0x1c) += -0xccc;
    } else {
        *(int *)(a + 0x10) += 0x13333;
        *(int *)(a + 0x18) += 0x7ae;
        *(int *)(a + 0x1c) += 0x7ae;
    }
    if ((*(short *)(a + 0x64) * __Random()) >> 16 == 0)
        __Func_80929d8(a, 7);
    if (*(short *)(a + 0x64) != 0)
        *(short *)(a + 0x64) -= 2;
    else
        *(short *)(a + 0x64) = ((__Random() * 5) >> 16) * 2 + 2;
    if (--*(int *)(a + 0x68) == 0)
        __DeleteActor(a);
}

extern unsigned int iwram_3001e40;
extern unsigned char L5240[] __asm__(".L5240");
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetAnim(void *a, int anim);
extern void OvlFunc_932_200aa10(unsigned char *a);

void OvlFunc_932_200ab58(void)
{
    unsigned char *a;
    int *q;

    if ((iwram_3001e40 & 3) == 0) {
        q = (int *)L5240;
        a = __CreateActor(0xde, q[0], q[1], q[2]);
        if (a != 0) {
            {
                short *p = (short *)(a + 0x64);
                int v1 = 0x1e;
                int v2;

                *p = v1;
                p++;
                v2 = 1;
                *p = v2;
            }
            *(int *)(a + 0x68) = 0x14;
            OvlFunc_932_200aa10(a);
            *(void **)(a + 0x6c) = OvlFunc_932_200aa48;
            __Actor_SetAnim(a, 1);
        }
    }
}
