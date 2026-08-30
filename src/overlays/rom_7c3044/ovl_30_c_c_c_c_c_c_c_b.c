/* Cluster OvlFunc_937_200833c..OvlFunc_937_200833c split out of goldensun/asm/overlays/rom_7c3044/ovl_30_c_c_c_c_c_c_c.s.
 *
 * Code to this file, the trailing .section .data to its _c sibling.
 *
 * OK on the first screen, on two reads.
 *
 * IT RETURNS void. The epilogue is `add sp, #8 / pop {r0} / bx r0` -- the
 * `pop {r0}` tell -- with no `mov r0, #0`, so there is no `return 0;`.
 *
 * IT IS A `switch`, NOT AN IF-CHAIN. `cmp #0xf / bgt / cmp #9 / bge /
 * cmp #3 / beq / b` is gcc's balanced case tree, and writing cases 9 through
 * 0xf out individually lets group_case_nodes merge them into the single range
 * node the ROM tests. An `if (e >= 9 && e <= 0xf)` would have produced the
 * `sub/cmp/bhi` unsigned-range idiom instead.
 *
 * The original .s header comment was stale: it named Func_92924 on slots
 * 0x0A-0x0E and 0x11-0x13, but the body calls __DeleteFieldActor on nine
 * slots with __Func_8092950(0xd, 2) on the else path.
 */
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __DeleteFieldActor(int slot);
extern void __Func_8092950(int a, int b);

void OvlFunc_937_200833c(void)
{
    unsigned char *g;
    int s0;
    int s1;
    int e;

    g = gState;
    e = *(short *)(g + (0xe1 << 1));
    switch (e) {
    case 3:
        s0 = 4;
        s1 = 2;
        __CopyMapTiles(0x1e, 0xe, 0x1e, 0x10, s0, s1);
        break;
    case 9:
    case 0xa:
    case 0xb:
    case 0xc:
    case 0xd:
    case 0xe:
    case 0xf:
    case 0x11:
        if (__GetFlag(0x911)) {
            __DeleteFieldActor(0xa);
            __DeleteFieldActor(0xb);
            __DeleteFieldActor(0xc);
            __DeleteFieldActor(0xd);
            __DeleteFieldActor(0xe);
            __DeleteFieldActor(0x11);
            __DeleteFieldActor(0x12);
            __DeleteFieldActor(0x13);
            __DeleteFieldActor(0xf);
        } else {
            __Func_8092950(0xd, 2);
        }
        break;
    default:
        if (__GetFlag(0x911)) {
            __DeleteFieldActor(0x10);
            __DeleteFieldActor(0x11);
        }
        break;
    }
}
