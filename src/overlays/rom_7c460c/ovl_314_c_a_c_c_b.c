/* OvlFunc_939_20091d0  --  0x020091d0, cut from the head of
 * goldensun/asm/overlays/rom_7c460c/ovl_314_c_a_c_c.s; the remaining five
 * functions follow as ovl_314_c_a_c_c_c.o.
 *
 * Leaves an area: clear two save bits, and if the player is standing inside a
 * particular rectangle stop the running task and post a message id, then clear
 * a third bit on the way out.
 *
 * THE STORED CONSTANT IS A VARIABLE ASSIGNED AT THE TOP. `*(u16 *)(...) = 0x5b`
 * pools the value as a HImode constant -- `ldr r3, =0x5b` -- where the ROM has
 * `mov r3, #0x5b`. An `int` local widens it to SImode and gcc uses the
 * immediate, which is batch 84's rule; but WHERE the local is assigned decides
 * whether it matches. Assigned next to the store it is 10 of 40; assigned at
 * the top of the function, so it is live across the calls, it matches. Both
 * were measured.
 *
 * THE RECTANGLE TEST IS THREE NESTED `if`s, not one condition. The x range is
 * an unsigned compare over the whole word after an add of 0xff97ffff; the two
 * z bounds are separate signed compares. Written with `&&` they fuse.
 */
struct A { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; };

extern unsigned char iwram_3001ebc[];
extern struct A *__MapActor_GetActor(int slot);
extern void __ClearFlag(int id);
extern void __StopTask(void *fn);
extern void OvlFunc_939_2009240(void);
extern void OvlFunc_939_200918c(void);

void OvlFunc_939_20091d0(void)
{
    char *base;
    struct A *a;
    int z;
    int v;

    v = 0x5b;
    base = *(char **)iwram_3001ebc;
    __ClearFlag(0x241);
    __ClearFlag(0x90 << 2);
    a = __MapActor_GetActor(0);
    if ((unsigned int)(a->f8 + 0xff97ffff) <= 0x87fffe) {
        z = a->f10;
        if (z > (0xa0 << 16)) {
            if (z < (0xf8 << 16)) {
                __StopTask(OvlFunc_939_2009240);
                *(unsigned short *)(base + (0xc1 << 1)) = v;
            }
        }
    }
    OvlFunc_939_200918c();
    __ClearFlag(0x91 << 2);
}
