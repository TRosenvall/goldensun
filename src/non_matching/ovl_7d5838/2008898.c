/* OvlFunc_950_2008898  [ovl_7d5838]
 *
 * Source asm: goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_c.s
 *
 * NOT SPLIT. The .s still holds all three of its functions.
 *
 * Sixteen instructions against sixteen, fourteen identical. Blocker:
 * ARG-INTERLEAVE on __MapActor_Emote --
 *
 *     rom    mov r2, #0x28 / mov r0, #0x1f  / ldr r1, =0x103
 *     ours   mov r2, #0x28 / ldr r1, =0x103 / mov r0, #0x1f
 *
 * -- r0 in the middle of the argument block. Four lever combinations tried, all
 * byte-identical to the form below: __MapActor_Emote declared, left implicit,
 * __MessageID declared, and both declared.
 *
 * THE DISCRIMINATOR FOR THIS CLASS IS STILL NOT IDENTIFIED, and this function
 * is the fourth data point. Batch 26 corrected an earlier claim that a
 * middle-position r0 is never reachable -- it sometimes is -- so what separates
 * the cases matters. What is known:
 *
 *   OvlFunc_959_20092e0   mov r2 / mov r0 / mov r1              REACHABLE
 *   OvlFunc_899_2008428   mov r1,#imm / mov r0 / lsl r1 / mov r2   no
 *   OvlFunc_924_2008ffc   ldr r2,= / mov r0 / ldr r1,=           no
 *   OvlFunc_950_2008898   mov r2 / mov r0 / ldr r1,=             no
 *
 * "All plain movs" is the only case that works, but the sample is one, and two
 * plausible rules die on it: it is not "a pool load after r0" (2008428 has a
 * `lsl`), and it is not "a multi-instruction argument" (2008898's r1 is a
 * single pool load). Recorded as a table rather than a rule, because a rule
 * from one positive is what produced the claim batch 26 had to correct.
 */
extern void __ActorMessage(int actor, int b);

void OvlFunc_950_2008898(int slot)
{
    __CutsceneStart();
    __MessageID(0x23a8);
    __MapActor_Emote(0x1f, 0x103, 0x28);
    __ActorMessage(slot, 0);
    __CutsceneEnd();
}
