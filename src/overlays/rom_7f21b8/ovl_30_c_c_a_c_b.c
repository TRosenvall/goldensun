/* OvlFunc_967_200815c  --  0x0200815c
 *
 * Cut out of goldensun/asm/overlays/rom_7f21b8/ovl_30_c_c_a_c.s.
 *
 * One of the four Lemuria attendants. Each asks the same question -- is the
 * player facing the shrine? -- and does something different if so.
 *
 * THE FACING TEST IS THE QUADRANT FORM (docs/elevation.md, batch 91):
 *
 *     unsigned short d = (a->facing + 0x2000) & ~0x3fff;
 *     if (d == 0xc000)
 *
 * Two things are forced and both are in that write-up: the mask is spelled
 * `~0x3fff` so gcc pools 0xffffc000 in one `ldr` rather than building 0xc000
 * with `mov`+`lsl`, and the result is `unsigned short` so the `lsl r3, #16`
 * against the pre-shifted 0xc0000000 appears.
 *
 * ALL FOUR OF THESE WERE FOUND AT ONCE by tools/prologue_families.py, which
 * clusters every remaining function in asm/ on its first twelve instructions.
 * They differ only in the true arm's call and two message ids; the first one
 * screened OK and the other three were constant substitutions into its body.
 */
struct Actor {
    unsigned char pad00[6];
    unsigned short facing;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __Func_80b0278(int a, int slot);

void OvlFunc_967_200815c(int slot)
{
    unsigned short d;

    d = (__MapActor_GetActor(0)->facing + 0x2000) & ~0x3fff;
    if (d == 0xc000) {
        __Func_80b0278(0x21, slot);
    } else if (__GetFlag(0x9a7)) {
        __MessageID(0x28f2);
        __ActorMessage(slot, 0);
    } else {
        __MessageID(0x26e7);
        __ActorMessage(slot, 0);
    }
}
