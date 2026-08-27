/* OvlFunc_958_2008fd0  --  0x02008fd0
 *
 * Cut out of goldensun/asm/overlays/rom_7e636c/ovl_cc0_c_a_c_a_c_c_b.s.
 *
 * UNPARKS src/non_matching/ovl_7e636c/2008fd0.c, and the whole
 * "message base held in a callee-saved register" class with it.
 *
 * The park's blocker was that the ROM keeps a message id in r5 and reaches the
 * second id with `mov r0, r5 / add r0, #8`, and it recorded that "a named
 * `int base` does not change it -- gcc constant-folds `base + 8`". That is
 * correct and it is the whole clue: an INT constant folds, a SYMBOL ADDRESS
 * does not. `base = (int)&_MSG_23cc;` cannot be folded, so gcc holds the symbol
 * in r5 and adds 8 to a copy -- the ROM's exact sequence, push list included.
 *
 * `_MSG_23cc` was added to message.sym in its own commit, named by value like
 * the 68 ids already in that block.
 *
 * The `int` return type on __Func_8092c40 is the second fix; it puts
 * `mov r1, #0` before `mov r0, #8`.
 *
 * Drafted by a parallel screening agent; re-screened here before wiring.
 */
extern char *iwram_3001ebc;
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern int _MSG_23cc;

void OvlFunc_958_2008fd0(void)
{
    int base;
    unsigned short *p;

    base = (int)&_MSG_23cc;
    __MessageID(base);
    __Func_8092c40(8, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        if (__GetFlag(0x95 << 4) && __GetFlag(0x96f) == 0)
            __MessageID(base + 8);
        __ActorMessage(8, 0);
    } else {
        p = (unsigned short *)(iwram_3001ebc + (0xec << 1));
        *p = *p + 1;
        __ActorMessage(8, 0);
    }
}
