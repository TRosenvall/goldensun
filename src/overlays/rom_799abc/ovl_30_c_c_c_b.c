/* OvlFunc_905_20090c8  --  0x020090c8
 *
 * Cut out of goldensun/asm/overlays/rom_799abc/ovl_30_c_c_c.s.
 *
 * A frame-counted cutscene beat table: tick a counter and, on five specific
 * frames out of 480, do one thing. Then a separate unconditional check that
 * arms the next scene.
 *
 * THE HImode-CONSTANT RULE NEEDS THE BASIC-BLOCK LEVER TOO, and that is new.
 * Batch 104 established that storing a literal through a `short *` gives
 * `ldr r3, =0x63` -- gcc-2.96 has no immediate alternative for an HImode
 * constant -- so the ROM's `mov r3, #0x63` means the source's right-hand side
 * is int-typed. True, and not sufficient:
 *
 *     int k = 0x63; *(short *)(...) = k;   in the store's OWN block   46 of 69
 *     int k;  k = 0x63;                    same block, split          46 of 69
 *     int k = 0x63; at the top of the function, used in the `if`      MATCH
 *
 * The int-typed value has to be rematerialised at the store, which is the
 * lever's placement rule, not just its type. That also explains why
 * src/non_matching/ovl_7c097c/2009f14.c is stuck at 12 with per-case `int zero`
 * declarations -- they are all in their stores' own blocks.
 *
 * The counter is switched on WITHOUT being re-read: `n = ++gOvl_020098ec;`.
 * Writing `gOvl_020098ec++;` and then switching on the global re-loads it.
 *
 * Five cases and gcc builds a comparison TREE, not a table -- 0x3c, 0xb4, 0xf0,
 * 0x10e, 0x1e0 over a 420-wide range is far too sparse. The `mov r2, #0x87 /
 * lsl r2, #1` for 0x10e and the `add r2, #0xd2` for 0x1e0 are gcc's own
 * arithmetic on the tree's pivots; they are not written that way.
 *
 * The 0x80 << 6 argument to __Func_8092adc needs the basic-block lever in its
 * usual form -- the ROM splits the `mov`/`lsl` pair around `mov r0, #0xd`.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern int gOvl_020098ec;
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MapActor_Emote(int slot, int id, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int n);

void OvlFunc_905_20090c8(void)
{
    char *p;
    unsigned char *g;
    int n;
    int v;
    int k;

    p = iwram_3001ebc;
    v = 0x80 << 6;
    k = 0x63;
    n = ++gOvl_020098ec;
    switch (n) {
    case 0x3c:
        __Func_8092adc(0xd, v, 0);
        __MapActor_Emote(0xd, 2, 0);
        break;
    case 0xb4:
        __Func_809259c(0xd, 3);
        break;
    case 0xf0:
    case 0x10e:
        __MapActor_Jump(0xd, 4, 0);
        break;
    case 0x1e0:
        __MapActor_SetAnim(0xd, 4);
        break;
    }
    g = gState;
    if (*(short *)(g + (0x8d << 2)) == 0)
        *(short *)(p + (0xc1 << 1)) = k;
}
