/* OvlFunc_958_2008fd0 -- NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7e636c/ovl_cc0_c_a_c_a_c_c.s
 * Best screen: 42 instructions against the ROM's 45.
 *
 * BLOCKER CLASS: a message base held in a callee-saved register -- the same
 * class as src/non_matching/overlays/message_base_register.c, which this file
 * should be read alongside.
 *
 * The ROM loads 0x23cc once into r5 and reaches the second id with an add:
 *
 *      ldr r5, =0x23cc / mov r0, r5 / bl __MessageID
 *      ...
 *      mov r0, r5 / add r0, #8 / bl __MessageID
 *
 * and pushes {r5, lr} to keep it. We get two independent pool loads and no r5
 * at all, three instructions shorter. A named `int base` does not change it --
 * gcc constant-folds `base + 8` long before any pass could notice r5 already
 * holds 0x23cc.
 *
 * That is exactly what the earlier park records for OvlFunc_962_200806c and
 * OvlFunc_950_2008500, where three consecutive ids are reached the same way.
 * This is a two-id instance of it, so the class now has THREE members and one
 * of them needs only a single add. `-fcall-used-r4` was ruled out for the class
 * in batch 92; nothing else has been found.
 *
 * The rest reads clean and is worth keeping: the flag ladder is
 * `if (GetFlag(0x950) && GetFlag(0x96f) == 0)` as one short-circuit condition,
 * the counter bump at [iwram_3001ebc]+0x1d8 is a plain halfword increment, and
 * both arms end with the same __ActorMessage call, which gcc cross-jumps.
 */
extern char *iwram_3001ebc;
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int arg);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_958_2008fd0(void)
{
    int base;
    unsigned short *p;

    base = 0x23cc;
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
