/* Cluster OvlFunc_957_200b610..OvlFunc_957_200b610 extracted from
 * goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_c_c.s.
 *
 * Total .text for this TU = 50 bytes (= 0x32).
 * First in the run, ahead of the _b piece, in goldensun/overlays/rom_7e3e08/overlay.ld.
 * The _b piece keeps the .data section.
 *
 * Clears a flag byte on the actor and copies a two-bit selector from the sprite
 * of actor 0 onto its own sprite.
 *
 * UNPARKED BY THE BITFIELD LEVER plus one statement swap. The park had it at
 * 3 of 25 and read the residue as a register-allocation floor, noting that the
 * ROM writes r4 without pushing it and concluding "gcc will not produce that
 * from any source spelling". That conclusion was wrong twice over: r4 is
 * caller-saved here because GCC296_CFLAGS carries -fcall-used-r4, so gcc is
 * free to use it; and the mask half of the function was the real obstacle.
 *
 * With the field declared as a bitfield, twenty-two of the twenty-five lines
 * fall into place and the residue is the three that mention the sprite pointer:
 * gcc puts it in r0, freshly dead after the call, where the ROM uses r4.
 *
 * THE FIX IS TO READ IT EARLIER IN THE SOURCE THAN THE ROM READS IT.
 *
 *     s = a->spr;      <-- our source order
 *     t = o->spr;
 *
 *     ldr r3, [r0, #0x50]   <-- the ROM's, and ours, after scheduling
 *     ldr r4, [r5, #0x50]
 *
 * Statement order fixes the register BIRTH order, and the scheduler then puts
 * the two loads back in the ROM's emission order. Written the way the ROM reads,
 * `s` is born after `o` is dead and inherits r0. Worth remembering: matching the
 * ROM's instruction order in the source is not always how you match its
 * registers.
 */

struct Sprite {
    unsigned char pad[9];
    unsigned char lo : 2;
    unsigned char sel : 2;
    unsigned char hi : 4;
};

struct Actor {
    unsigned char pad[0x23];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Sprite *spr;
};

extern struct Actor *__MapActor_GetActor(int which);

void OvlFunc_957_200b610(struct Actor *a)
{
    struct Actor *o;
    struct Sprite *s;
    struct Sprite *t;

    if (a != 0) {
        a->f23 = 0;
        o = __MapActor_GetActor(0);
        s = a->spr;
        t = o->spr;
        s->sel = t->sel;
    }
}
