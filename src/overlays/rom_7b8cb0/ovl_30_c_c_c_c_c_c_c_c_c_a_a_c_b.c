/* Cluster OvlFunc_931_20083d4..OvlFunc_931_20083d4 extracted from goldensun/asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_a_c_a.o and the rest of
 * the overlay in goldensun/overlays/rom_7b8cb0/overlay.ld.
 *
 * The three-way talk two .o slots after
 * src/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_a_b.c: same structure,
 * flags 0x241 and 0x909, actors 0x14 and 0x11.
 *
 * READ THE EXEMPLAR FOR WHERE THE SECOND `if` ENDS. Flag 0x909 guards only the
 * extra __MessageID; the __ActorMessage after it runs either way. Written with
 * both inside the guard the function differs from the ROM by a single byte --
 * the beq's offset -- and that was invisible to tools/tryc.py until the screen
 * was taught to keep label POSITIONS. It is the same trap twice, in adjacent
 * functions, so it is worth stating twice.
 *
 * ONE DIFFERENCE FROM THE EXEMPLAR: there the first arm calls __Func_8093054
 * with its declaration withheld, so r0 is filled last. Here the first arm calls
 * __ActorMessage and the ROM fills r0 FIRST, so it is declared. The two
 * functions have the same skeleton -- tools/match_shapes.py collapses callee
 * names and cannot see the difference -- which is the standing reason a shape
 * match is a lead and not a proof.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int b);
extern void __Func_80b0278(int a, int b);
extern int OvlFunc_931_2008338(void);

void OvlFunc_931_20083d4(void)
{
    if (!__GetFlag(0x241)) {
        __CutsceneStart();
        __MessageID(0x18ed);
        __ActorMessage(0x14, 0);
        __CutsceneEnd();
    } else if (OvlFunc_931_2008338()) {
        __Func_80b0278(0x14, 0x11);
    } else {
        __CutsceneStart();
        __MessageID(0x18ee);
        if (__GetFlag(0x909))
            __MessageID(0x1943);
        __ActorMessage(0x11, 0);
        __CutsceneEnd();
    }
}
