/* OvlFunc_945_200b7b4 -- NON-MATCHING.
 * Blocker class: DEAD CALLEE-SAVED REGISTER.
 * 17 lines against the ROM's 17, 11 differing, and the whole cause is the
 * first instruction:
 *
 *     rom    push {r5, r6, r7, r14}
 *     ours   push {r5, r6, r14}
 *
 * The ROM sets `mov r7, #0` before the loop, never reads r7 again, and
 * restores it in the epilogue. Six instructions of prologue and epilogue
 * bookkeeping for a value with no consumer. gcc will not reserve a register it
 * does not use, and no source form asks it to -- a local assigned zero and
 * never read is removed before allocation.
 *
 * This is the shape HANDOFF.md's register-pressure section names directly,
 * with OvlFunc_935_2008704 as its example. This function is a second instance
 * and a cleaner one: everything else here matches, so the dead register is the
 * entire difference rather than one cause among several.
 *
 * The body is believed correct and is worth keeping: slots 0x1c through 0x23
 * inclusive, each actor's interactFlags at 0x59 ORed with 8.
 */
extern char *__MapActor_GetActor(int slot);

void OvlFunc_945_200b7b4(void)
{
    int i;
    int bit;
    unsigned char *p;

    i = 0x1c;
    bit = 8;
    do {
        p = (unsigned char *)(__MapActor_GetActor(i) + 0x59);
        *p |= bit;
        i++;
    } while (i <= 0x23);
}
