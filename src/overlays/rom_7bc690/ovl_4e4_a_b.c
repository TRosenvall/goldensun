/* Cluster OvlFunc_933_2008c6c..OvlFunc_933_2008c6c extracted from goldensun/asm/overlays/rom_7bc690/ovl_4e4_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7bc690/ovl_4e4_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7bc690/overlay.ld.
 *
 * Picks a random one of three slots, retrying up to three times while its flag
 * is already set, then claims it and speaks the matching line.
 *
 * THE RETRY IS AN UN-ROTATED LOOP WITH A `continue`, and it is written with
 * gotos because that is the only way to get the ROM's block order:
 *
 *     b .check / .next: i++ / .check: if (i > 2) goto after / ... /
 *     if (GetFlag(f)) goto next / SetFlag(f) / .after:
 *
 * The increment sits BEFORE the test label, and the success path FALLS OUT of
 * the loop rather than breaking to a separate exit. A `for` or a `while` puts
 * the test at the top and the increment at the bottom and is several
 * instructions out.
 *
 * `r` is read after the loop and is only assigned inside it. That is safe
 * because the loop always runs at least once, and it is what the ROM does --
 * r6 survives to the __MessageID computation. It reads like a bug and is not.
 *
 * The message id is `a * 3 + r + 0x1a10` and the ROM builds the multiply as
 * `lsl r0, r3, #1 / add r0, r8` -- shift-and-add rather than a multiply, which
 * `* 3` gives for free.
 */
extern unsigned int __Random(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);

void OvlFunc_933_2008c6c(int a)
{
    unsigned int r;
    int flag;
    int i;

    i = 0;
    goto check;
next:
    i++;
check:
    if (i <= 2) {
        r = (__Random() * 3) >> 16;
        flag = r + 0x303;
        if (__GetFlag(flag))
            goto next;
        __SetFlag(flag);
    }
    __CutsceneStart();
    __MessageID(a * 3 + r + 0x1a10);
    __ActorMessage(r + 1, 0);
    __CutsceneEnd();
}
