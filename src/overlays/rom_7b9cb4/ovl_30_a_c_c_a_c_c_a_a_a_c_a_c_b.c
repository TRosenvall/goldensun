/* OvlFunc_932_200a934  --  0x0200a934
 *
 * The tail of goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c_a_c.s;
 * the two functions ahead of it stay in _a.s.
 *
 * MATCHED ON THE FIRST CANDIDATE, which is worth recording because
 * filtered.py offered it as [cse] -- it repeats `ldr r0, =0x109` with labels
 * between the two sites, and the classifier's advice for that shape is a
 * CSE_CFLAGS build rule. No flag was needed. THE TWO SITES ARE IN MUTUALLY
 * EXCLUSIVE ARMS, and rerun-CSE does not common across those; the flag is for
 * the guard/set shape, where one use DOMINATES the other. So the [cse] marker
 * is a hint about where to look, not a verdict, and the cheap check is whether
 * either site can reach the other.
 *
 * Two shapes reused verbatim from elsewhere in this batch, which is most of why
 * the first candidate landed:
 *
 *   THE FLAG IS A NAMED LOCAL AND ITS VALUE IS WHAT GETS STORED. The ROM copies
 *   __GetFlag's result into a callee-saved r5 and later stores it with
 *   `strb r5` on the path where the branch has proved it zero, with no
 *   preceding `mov`. Same as OvlFunc_901_20088a8 in this batch: a callee-saved
 *   register for a value used once means a live range crossing a call, and the
 *   flag's is the only one available.
 *
 *   THE STATEMENT-FORM RUNTIME OFFSET BUILD for gState + 0x1c2. Written folded,
 *   gcc emits a single pool word `=gState+450`; the ROM has
 *   `ldr =gState / mov r2,#0xe1 / lsl r2,#1 / add r3,r2` and then `ldrsh` with a
 *   zero index register, because Thumb's ldrsh has no immediate-offset form.
 *   `off = 0xe1; off <<= 1; g = gState + off; off = 0;` gives all six.
 *
 * The actor pointers are not variables -- written through the call, gcc destroys
 * r0 in place, which is what the ROM does at all three sites.
 *
 * NOTE ON THE SCREEN: tryc.py warned that the reference keeps its literal pool
 * INSIDE the function, so pool loads normalising to `=value` hide any
 * PC-relative distance error. That warning is why make compare, not the screen,
 * is the authority here; it is green.
 */
extern unsigned char gState[];

extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Func_8092b08(int slot, int b);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void OvlFunc_932_200b028(void);

void OvlFunc_932_200a934(void)
{
    unsigned char *g;
    unsigned int off;
    short v;
    int f;
    unsigned char *a;

    f = __GetFlag(0x909);
    if (f) {
        __MapActor_SetPos(8, 0, 0);
        __MapActor_SetPos(9, 0, 0);
    } else {
        __Actor_SetSpriteFlags(__MapActor_GetActor(8), 0);
        __Func_8092b08(9, 3);
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        __MapActor_GetActor(9)[0x59] = f;
    }
    off = 0xe1;
    off <<= 1;
    g = gState + off;
    off = 0;
    v = *(short *)(g + off);
    if (v == 1 || v == 0x62) {
        if (!__GetFlag(0x109)) {
            a = __MapActor_GetActor(0);
            __CutsceneStart();
            *(int *)(a + 0xc) = 0x80 << 13;
            __CutsceneEnd();
        }
    } else if (v == 0x63) {
        if (!__GetFlag(0x109))
            OvlFunc_932_200b028();
    }
}
