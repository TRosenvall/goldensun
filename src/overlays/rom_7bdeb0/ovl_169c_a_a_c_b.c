/* Cluster OvlFunc_934_2009770..OvlFunc_934_2009770 extracted from goldensun/asm/overlays/rom_7bdeb0/ovl_169c_a_a_c.s.
 *
 * Total .text for this TU = 104 bytes (= 0x68).
 * Preserves the original ROM layout when slotted before
 * asm/overlays/rom_7bdeb0/ovl_169c_a_a_c_c.o in
 * goldensun/overlays/rom_7bdeb0/overlay.ld.
 *
 * Draws a map rectangle, then redraws a one-tile strip differently depending on
 * save bit 0x301 -- the two arms differ only in which column they clear and in
 * one of the two trailing arguments.
 *
 * THE ONE LEVER, and it is the whole function: __Func_8010704 takes six
 * arguments, so two of them travel on the stack, and the ROM sets those two up
 * ONCE in callee-saved registers before the first call:
 *
 *      mov r6, #0x17 / mov r5, #0x22   <- before the first __Func_8010704
 *      str r6, [sp] / str r5, [sp, #4] <- at each of the three call sites
 *
 * Naming them as FUNCTION-SCOPE locals assigned before the first call is what
 * produces that; passing the literals at each site builds them again per call.
 * The `else` arm passes 0x23 in the first stack slot instead, and writing the
 * literal there (rather than reassigning e5) is what keeps the `mov r3, #0x23 /
 * str r3, [sp]` ahead of the argument setup instead of folding into it.
 *
 * This is the mirror image of batch 89's map-repaint case, where the ROM did
 * NOT keep the shared values live and function-scope locals were wrong. Both
 * spellings exist; the ROM's push list says which one to write -- {r5, r6, lr}
 * here means two values survive across calls.
 *
 * Matched on the first screen.
 */
extern int __GetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_934_2009938(int a, int b, int c);

void OvlFunc_934_2009770(void)
{
    int e5, e6;

    e5 = 0x17;
    e6 = 0x22;
    __Func_8010704(0, 0x22, 0xd, 3, e5, e6);
    if (__GetFlag(0x301)) {
        OvlFunc_934_2009938(0xb, 0x23, 0x23);
        __Func_8010704(0x18, 0x22, 1, 3, e5, e6);
    } else {
        OvlFunc_934_2009938(0xb, 0x17, 0x23);
        __Func_8010704(0x18, 0x22, 1, 3, 0x23, e6);
    }
}
