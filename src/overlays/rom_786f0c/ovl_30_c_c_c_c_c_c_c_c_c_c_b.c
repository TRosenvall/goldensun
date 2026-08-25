/* Cluster OvlFunc_886_200855c..OvlFunc_886_200855c extracted from goldensun/asm/overlays/rom_786f0c/ovl_30_c_c_c_c_c_c_c_c_c_c.s.
 *
 * Slotted between ovl_30_c_c_c_c_c_c_c_c_c_c_a.o and the rest of the overlay.
 *
 * A RANGE CHECK WRITTEN AS AN ADDITION OF A NEGATIVE CONSTANT, then an unsigned
 * compare:
 *
 *     t = *(unsigned short *)(a + 6) + 0xffff5fff;
 *     if (t <= 0x3ffe) ...
 *
 * The ROM's `ldr r2, =0xffff5fff / add r3, r2` is the addition, not a
 * subtraction of 0xa001 -- the same "the ROM adds, including when the constant
 * is negative" reading as the position-triple family in batch 46. The `bhi`
 * says the comparison is unsigned, which is what makes the single test do the
 * work of two bounds.
 *
 * The fall-through after `bhi` is the SHORT arm, so that arm is the `if` body
 * and the cutscene is the else.
 */
extern void *__MapActor_GetActor(int slot);
extern void __Func_80b0278(int a, int b);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int b);

void OvlFunc_886_200855c(void)
{
    unsigned char *a;
    unsigned int t;

    a = (unsigned char *)__MapActor_GetActor(0);
    t = *(unsigned short *)(a + 6) + 0xffff5fff;
    if (t <= 0x3ffe) {
        __Func_80b0278(2, 0x16);
        return;
    }
    __CutsceneStart();
    if (__GetFlag(0x87a))
        __MessageID(0x1c09);
    else if (__GetFlag(0x815))
        __MessageID(0x11a3);
    else
        __MessageID(0xf54);
    __ActorMessage(0x16, 0);
    __CutsceneEnd();
}
