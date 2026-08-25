/* OvlFunc_911_200a608  --  0x0200a608, asm/overlays/rom_79e5c0/ovl_30_c_a_c.s
 *
 * BLOCKER CLASS: argument precompute (calls.c:805).
 * Status: 6 of 65 with -fno-rerun-cse-after-loop, 44 of 65 without it.
 *
 * WHAT IT DOES
 * On every eighth frame, optionally plays a sound, spawns actor type 0x1a at a
 * fixed spot, clears bit 0 of its flag byte and a byte on its sprite, sets the
 * sprite's two-bit selector to 1, seeds three motion words, and sends it
 * travelling with a script attached.
 *
 * THE CONSTANT 0xc4 << 15 IS USED AT TWO CALL SITES, and that is where the two
 * halves of this park separate.
 *
 *   WITHOUT -fno-rerun-cse-after-loop, gcc hoists it into r7 -- a callee-saved
 *   register it then has to push and pop -- and passes `mov r1, r7` at both
 *   sites. The ROM rebuilds `mov r1, #0xc4 / lsl r1, #0xf` at each. That is
 *   exactly the symptom the CSE_CFLAGS group in the Makefile exists for, and
 *   the flag removes it completely. THIS TU IS A CANDIDATE FOR THAT GROUP.
 *
 *   WITH the flag, six lines are left and they are all one call's argument
 *   setup:
 *
 *      rom   mov r1,#0xc4 / mov r3,#0xd2 / mov r0,#0x1a / lsl r1,#0xf
 *            / mov r2,#0x0 / lsl r3,#0xf
 *      ours  mov r1,#0xc4 / mov r3,#0xd2 / lsl r1,#0xf / lsl r3,#0xf
 *            / mov r0,#0x1a / mov r2,#0x0
 *
 * __CreateActor is called with (0x1a, 0xc4 << 15, 0, 0xd2 << 15): TWO expensive
 * arguments and two cheap ones, with a cheap one not last. That is precisely
 * what precompute_register_parameters predicts -- it copies every argument with
 * rtx_cost > 2 into a pseudo BEFORE any hard register is loaded, and
 * load_register_parameters then fills r0..r3 forward. The ROM interleaves.
 *
 * NOT REACHABLE FROM C, and the flag probe says so rather than my judgement:
 *   - `0x620000` / `0x690000` written as plain literals instead of shifts:
 *     byte-identical. gcc synthesises them with mov/lsl either way and the
 *     rtx_cost is the same.
 *   - -fno-gcse: no effect on either half.
 *
 * EVERYTHING ELSE MATCHES INSTRUCTION FOR INSTRUCTION, including the two masked
 * writes, which need OPPOSITE spellings -- the byte-width `mov r3, #0xfe` on
 * the actor's flag byte is hand-written masking, and the 32-bit
 * `mov r3, #0xd / neg r3, r3` on the sprite is a bitfield. See
 * docs/elevation.md.
 *
 * THE ZERO STORED AT SPRITE +0x26 AND ACTOR +0x55 IS THE GUARD VALUE. The ROM
 * reuses r6, which holds `iwram_3001e40 & 7` and is provably zero on this path.
 * `t` below is that variable, and writing a literal 0 there costs an extra
 * `mov`.
 */

struct Sprite {
    unsigned char pad[9];
    unsigned char lo : 2;
    unsigned char sel : 2;
    unsigned char hi : 4;
    unsigned char pad0a[0x1c];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[0x18];
    int f18;
    unsigned char pad1c[7];
    unsigned char f23;
    unsigned char pad24[0xc];
    int f30;
    int f34;
    unsigned char pad38[0x18];
    struct Sprite *spr;
    unsigned char pad54;
    unsigned char f55;
};

extern unsigned int iwram_3001e40;
extern int L36a0[] __asm__(".L36a0");
extern unsigned char gScript_911__0200b5d8[];
extern void __PlaySound(int id);
extern struct Actor *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, void *s);

void OvlFunc_911_200a608(void)
{
    struct Actor *act;
    struct Sprite *s;
    unsigned int t;

    t = iwram_3001e40 & 7;
    if (t != 0)
        return;
    if (L36a0[0] != 0)
        __PlaySound(0xc8);
    act = __CreateActor(0x1a, 0xc4 << 15, 0, 0xd2 << 15);
    if (act == 0)
        return;
    s = act->spr;
    s->f26 = t;
    act->f23 = 0xfe & act->f23;
    s->sel = 1;
    act->f18 = 0x1999;
    act->f30 = 0x80 << 12;
    act->f34 = 0x80 << 12;
    act->f55 = t;
    __Actor_SetAnim(act, 2);
    __Actor_TravelTo(act, 0xc4 << 15, 0, 0x10d0000);
    __Actor_SetScript(act, gScript_911__0200b5d8);
}
