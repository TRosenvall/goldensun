/* OvlFunc_898_20091b0  --  0x020091b0, cut from goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_c_c_a.s.
 *
 * The cut was at the head of the .s, so this file takes the first .text slot
 * and the remaining functions follow as ovl_314_c_c_c_a_c_c_a_c.o.
 * Sends an actor to a target and waits for it to arrive: raise its speed, clear
 * one flag word and set another, hide the sprite, start the move, place it, and
 * then spin for up to sixty frames until its arrival halfword goes to zero --
 * whichever comes first. Then show the sprite again and drop the speed back.
 *
 * One of TWO byte-identical copies -- OvlFunc_898_20091b0 and OvlFunc_901_2008970.
 * Found with tools/find_twins.py.
 *
 * MATCHED ON THE FIRST SCREEN, 64 lines against 64 with nothing differing.
 * Three readings carried it and all three are rules already on the books:
 *
 *   THE TIMEOUT LOOP IS A PLAIN `for`, NOT A `goto` SHAPE. The ROM enters at
 *   the test with `b .L120c` and puts the decrement in its own block above it,
 *   which reads like a hand-written loop and is just gcc's un-rotated `for`
 *   with the `break` as a second exit. `for (i = 0x3c; i != 0; i--)` gives it
 *   exactly.
 *
 *   +0x2a IS THE HIGH HALF OF THE int AT +0x28, read as a signed halfword. The
 *   struct declares the int; the arrival test reads
 *   `*(short *)((char *)a + 0x2a)`, which is the same overlap the entity
 *   coordinates use and the reason Thumb's register-offset `ldrsh` shows up
 *   with a `mov r2, #0x2a` beside it.
 *
 *   THE POSITION IS SHIFTED AT THE CALL, NOT BEFORE IT. `x << 16` and `z << 16`
 *   are written as arguments; the ROM's `lsl r6, #16` and `lsl r3, #16` land in
 *   the argument block for exactly that reason, and hoisting them into locals
 *   moves them above the preceding call.
 */

struct Act {
    unsigned char pad00[0x28];
    int f28;
    unsigned char pad2c[0x18];
    int f44;
    int f48;
};

extern struct Act *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __Actor_SetSpriteFlags(struct Act *a, int f);
extern void __Func_8092158(int slot, int x, int z);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __WaitFrames(int n);

void OvlFunc_898_20091b0(int slot, int x, int z, int opt)
{
    struct Act *a;
    int i;

    a = __MapActor_GetActor(slot);
    __MapActor_SetSpeed(slot, 0xc0 << 10, 0xc0 << 9);
    a->f48 = 0x80 << 8;
    a->f44 = 0;
    a->f28 = opt;
    __Actor_SetSpriteFlags(a, 0);
    __Func_8092158(slot, x, z);
    __MapActor_SetPos(slot, x << 16, z << 16);
    for (i = 0x3c; i != 0; i--) {
        __WaitFrames(1);
        if (*(short *)((char *)a + 0x2a) == 0)
            break;
    }
    __Actor_SetSpriteFlags(a, 1);
    a->f48 = 0x80 << 9;
}
