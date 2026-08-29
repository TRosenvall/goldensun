/* CreateParticleActor  --  0x08096c80
 *
 * The function half of goldensun/asm/rom_8a000/rom_944ec_c_c.s; the four
 * .rodata blobs stay behind in rom_944ec_c_c_b.s, and stage1.ld lists the two
 * objects where the one used to be, in both the .text and the .rodata run.
 *
 * A thin wrapper over _CreateActor: forward all four arguments, reject the
 * actor if its type byte came back zero, then copy one field off the current
 * context, stamp two constants into it, clear two sprite flag bits, and start
 * its animation.
 *
 * THE MASK MUST BE A NAMED `int`, and this function is an unusually clean
 * demonstration of it -- it is the ONLY instruction in forty-two that moves:
 *
 *      rom    mov r3, #4 ... strb r3 ... strb r3 ... sub r3, #0x11
 *      ours   mov r3, #4 ... strb r3 ... strb r3 ... mov r3, #0xf3
 *
 * `p->f9 &= ~0xc` and `p->f9 = p->f9 & ~0xc` both build the mask at BYTE width
 * (`mov r3, #0xf3`), because the destination is a `unsigned char` and gcc
 * narrows the whole expression. Assigning ~0xc to a named `int` first forces
 * it to 32 bits -- and once it is 32 bits, 0xfffffff3 is reachable from the 4
 * already sitting in r3 by a single `sub`, which is what the ROM does. So the
 * width rule and the derive-from-a-nearby-constant behaviour compose here: the
 * cast is what makes the derivation available.
 *
 * Where the assignment sits does not matter -- at the top of the function or
 * immediately before the use, both match. Only the TYPE does.
 *
 * The two `return` paths are deliberately different: the rejected-actor path
 * returns a literal 0, and both other paths return `act`, which is why the ROM
 * has `mov r0, #0 / b` on one side and a shared `mov r0, r5` on the other.
 */
struct Spr { unsigned char pad00[9]; unsigned char f9; };

struct Actor {
    unsigned char pad00[0x14];
    int f14;
    unsigned char pad18[0x23 - 0x18];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Spr *f50;
    unsigned char f54;
    unsigned char f55;
};

struct C2 { unsigned char pad00[0x14]; int f14; };
struct Ctx { unsigned char pad00[0x10]; struct C2 *f10; };

extern struct Ctx *iwram_3001f30;
extern struct Actor *_CreateActor(int a, int b, int c, int d);
extern void _DeleteActor(struct Actor *a);
extern void _Actor_SetSpriteFlags(struct Actor *a, int flags);
extern void _Actor_SetAnim(struct Actor *a, int anim);

struct Actor *CreateParticleActor(int a, int b, int c, int d)
{
    struct Ctx *ctx;
    struct Actor *act;
    int mask;

    ctx = iwram_3001f30;
    act = _CreateActor(a, b, c, d);
    if (act != 0) {
        if (act->f54 == 0) {
            _DeleteActor(act);
            return 0;
        }
        act->f14 = ctx->f10->f14;
        act->f55 = 4;
        act->f23 = 4;
        mask = ~0xc;
        act->f50->f9 = act->f50->f9 & mask;
        _Actor_SetSpriteFlags(act, 0);
        _Actor_SetAnim(act, 1);
    }
    return act;
}
