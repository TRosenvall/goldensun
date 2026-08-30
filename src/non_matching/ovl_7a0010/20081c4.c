/* OvlFunc_912_20081c4  [overlays/rom_7a0010]
 *
 * Source asm: goldensun/asm/overlays/rom_7a0010/ovl_30_c_c_c.s
 *
 * BLOCKER CLASS: ldrh/ldrsh CSE -- LIKELY A CLASS, NOT A ONE-OFF. 10 of 105.
 *
 * The ROM reads the SAME halfword twice, once unsigned and once signed:
 *
 *     ldrh  r2, [r3]        @ kept live; feeds mov r3,r2 / sub r3,#8 / lsl #16
 *     mov   r1, #0
 *     ldrsh r3, [r3, r1]    @ cmp r3, #7
 *
 * gcc emits only the `ldrsh` and derives `(e - 8) << 16` from the sign-extended
 * value with `lsl #16 / ldr r2,=0xfff80000 / add`. The merge is LEGAL -- the
 * low sixteen bits are identical either way -- which is why no spelling of the
 * source prevents it. That accounts for about six of the ten.
 *
 * MEASURED: `(short)e == 7` on one unsigned read 13; an aliased
 * `extern unsigned short gStateH[] __asm__("gState")` 10, but it poisons the
 * symbol into a `ldr =0x2000240` pool entry that the ROM does not have; a
 * `volatile` alias 13. Reading the u16 first in source order does not survive
 * into the output.
 *
 * WHY THIS IS PROBABLY A CLASS: OvlFunc_957_2008bc8 in
 * asm/overlays/rom_7e3e08/ovl_30_c_c_a_c_c_c_c_c_c_a_a.s carries the identical
 * shape -- `ldrh r1,[r3] / mov r0,#0 / ldrsh r2,[r3,r0]` with the ldrh value
 * sign-extended later by lsl/asr. Worth a find_shape.py sweep before anyone
 * spends another round on a single instance.
 *
 * SOLVED ON THE WAY, and reusable: `*(int *)(base + (0xe0 << 1)) = 0x209` with
 * a bare literal lets gcc strength-reduce 0x209 out of the offset register
 * (`add r2, #0x49`). Naming it -- `int v; v = 0x209;` in the SAME block --
 * restores the ROM's `ldr r2, =0x209`. That is the batch-153 inverse lever
 * working exactly as documented.
 */
extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;

extern int __GetFlag(int flag);
extern void *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int y);

int OvlFunc_912_20081c4(void)
{
    unsigned char *base;
    int off;
    unsigned int i;
    unsigned short e;
    unsigned short d;
    int s1, s2;
    int v;

    base = iwram_3001ebc;
    v = 0x209;
    *(int *)(base + (0xe0 << 1)) = v;
    if (__GetFlag(0x845) == 0) {
        for (i = 8; i <= 0x16; i++)
            __Actor_SetSpriteFlags(__MapActor_GetActor(i), 0);
    }
    off = 0xe1 << 1;
    e = *(unsigned short *)(gState + off);
    if (*(short *)(gState + off) == 7) {
        s1 = 0xd; s2 = 8;
        __CopyMapTiles(0x22, 0x22, 0x12, 0x10, s1, s2);
        __CopyMapTiles(0x22, 0x5e, 0x12, 0x4c, s1, s2);
        __CopyMapTiles(0x5e, 0x22, 0x4e, 0x10, s1, s2);
    } else {
        d = e - 8;
        if (d <= 1) {
            s1 = 0xb; s2 = 8;
            __CopyMapTiles(0x22, 0x2b, 0x13, 0x17, s1, s2);
            __CopyMapTiles(0x22, 0x5e, 0x13, 0x53, s1, s2);
            __CopyMapTiles(0x5e, 0x22, 0x4f, 0x17, s1, s2);
            __MapActor_SetPos(0xa, 0, 0);
            __MapActor_SetPos(0xb, 0, 0);
            __MapActor_SetPos(0xc, 0, 0);
        }
    }
    return 0;
}
