/* OvlFunc_901_200858c -- 0x0200858c,
 * asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_a_a.s
 *
 * 70 of 70 lines, TWO differing.  Candidate at scratch/L858c.c.  Needs
 * CSE_CFLAGS and `_MSG_1cb1 = 0x1cb1;` in message.sym (a one-line addition,
 * deliberately NOT made while the function stays parked).
 *
 * SOLVED, and three of these are worth reusing:
 *   - `unsigned short *p` for the +0x64 field: a signed `short *` gives
 *     `ldrsh rD,[rB,rI]` with a zero index register where the ROM has
 *     `ldrh rD,[rB,#0]`.
 *   - `*p |= 2;` as a COMPOUND assignment gives the ROM's pooled constant
 *     (`ldr r2, =0x2`) and destructive orr.  Written as three steps --
 *     `v = 2; v |= *p; *p = v;` -- gcc emits `mov r3,#0x2` instead and the
 *     block is 45 differing.  Neither `*p = 2 | *p;` nor an int `v` reaches it.
 *   - the final `*p = 1;` needs the OPPOSITE treatment: an int intermediate,
 *     or gcc pools it (`ldr r3, =0x1`) where the ROM has `mov r3, #0x1`.
 *
 * So the same halfword pointer wants a pooled constant at one operation and a
 * mov-built one at the next, and the spelling that gets each is different.
 * That is the sharpest instance yet of the HImode-literal rule not being one
 * rule.
 *
 * BLOCKER: the OR's register roles.
 *      rom   orr r2, r3 / strh r2, [r6, #0x0]
 *      ours  orr r3, r2 / strh r3, [r6, #0x0]
 * Two instructions, and the destination register decides both.  `*p = 2 | *p;`
 * and a named constant both leave it at 2.
 */
