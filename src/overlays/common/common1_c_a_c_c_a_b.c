/* OvlFunc_common1_17c0  --  common overlay, +0x17c0
 *
 * Cut out of goldensun/asm/overlays/common/common1_c_a_c_c_a.s.
 *
 * Sends the actor whose slot id is stored in the caller's +0x64 field walking to
 * the caller's own position, drops its busy flag, gives it a script, plays a
 * sound, and clears the slot id.
 *
 * THE ZEROES ARE NOT A SOURCE SIGNAL HERE -- measured, and worth saying because
 * the assembly looks like it decides them. `mov r5, #0` feeds both the busy
 * byte at +0x55 and the slot halfword at +0x64, while the RETURN value gets its
 * own `mov r0, #0`, which reads as one named local and one bare literal.
 *
 * All three spellings compile to the same thirty-five instructions: the local
 * below, all three as plain literals, and all three as the one local including
 * the return. gcc decides on its own that two of the three stores share a
 * register. The local is kept because it reads well, not because it was forced.
 *
 * Contrast src/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c_b.c, where a named zero IS
 * forced -- there the value has to survive three calls, and a bare literal
 * costs four instructions. The discriminator is whether the value crosses a
 * CALL, not whether the ROM happens to share a register between two stores.
 *
 * THE SLOT ID IS A SIGNED SHORT. `ldrsh r0, [r2, r3]` with r3 zeroed is the
 * register-offset form, the only ldrsh thumb has, so the field is `short` and
 * not `unsigned short`. The same pointer is then reused for the halfword store
 * at the end, which is why it is saved in r8 across the calls.
 *
 * `pushal` in the reference is `push` -- an `al` condition suffix is "always",
 * the default, and both assemble to b560. tools/tryc.py now normalises it; see
 * the comment on CONDAL there. That one word is the only place in the whole
 * asm/ tree it appears, and without the normalisation this byte-perfect
 * function reads as one differing instruction.
 *
 * Matched on the first screen.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x55 - 0x14];
    unsigned char f55;
};

extern unsigned char L8[] __asm__(".L8");
extern struct A *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(struct A *a, int x, int y, int z);
extern void __Actor_SetScript(struct A *a, unsigned char *s);
extern void __PlaySound(int id);

int OvlFunc_common1_17c0(struct A *a)
{
    short *p;
    struct A *b;
    int zero;

    p = (short *)((char *)a + 0x64);
    b = __MapActor_GetActor(*p);
    __Actor_TravelTo(b, a->f8, a->fc + (0x90 << 14), a->f10);
    zero = 0;
    b->f55 = zero;
    __Actor_SetScript(b, L8);
    __PlaySound(0x53);
    *p = zero;
    return 0;
}
