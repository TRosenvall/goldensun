/* OvlFunc_948_2009fd8  --  0x02009fd8
 *
 * Cut out of goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_c_c_c_c_c_c_c.s.
 * `.L2f80` -- the frame counter this function drives -- stays in the sibling
 * piece and is reached through `.global .L2f80`, which emits no bytes.
 *
 * A per-frame tick: advance a counter modulo 17 and, on six of those
 * seventeen frames, do one thing.
 *
 * PARKED IN BATCH 104 ON THE r0-AGAINST-A-SHIFT ROTATION, FOUR TIMES OVER.
 * The BASIC-BLOCK LEVER (docs/elevation.md) closes all four at once. Each
 * `__MapActor_SetPos` sits inside an `if (__GetFlag(...) == 0)`, so assigning
 * its two coordinates to locals in the case's own block puts the assignment
 * and the use in different blocks; gcc then rematerialises each coordinate at
 * the call as a split `mov`/`lsl` pair with the slot number scheduled into the
 * gap, which is the ROM's shape. Twelve differing to zero, with no other
 * change to the file.
 *
 * Batch 104 recorded this as a shape the argument levers do not reach. That
 * was wrong in a specific way worth keeping: everything tried there was a
 * CALL-SITE property (an unprototyped callee, the return-type lever). The
 * lever is where the value is ASSIGNED.
 *
 * The counter is read three separate times -- incremented, compared against
 * 0x10 in the same expression that wrote it, then re-read for the switch --
 * which is why it is a plain global rather than a cached local.
 *
 * Eleven slots from 2, and the case bodies are in DESCENDING order
 * (12, 10, 8, 6, 4, 2) with the odd numbers falling to the default.
 */
extern int L2f80 __asm__(".L2f80");
extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_948_200a0c4(int a, int b);

void OvlFunc_948_2009fd8(void)
{
    int x0, y0, x1, y1, x2, y2, x3, y3;

    L2f80++;
    if (L2f80 > 0x10)
        L2f80 = 0;
    switch (L2f80) {
    case 12:
        x0 = 0xe8 << 16;
        y0 = 0xda << 18;
        if (__GetFlag(0xee7) == 0)
            __MapActor_SetPos(0x8, x0, y0);
        x1 = 0x94 << 17;
        y1 = 0xce << 18;
        if (__GetFlag(0xee8) == 0)
            __MapActor_SetPos(0x9, x1, y1);
        x2 = 0xa4 << 17;
        y2 = 0xbe << 18;
        if (__GetFlag(0xee9) == 0)
            __MapActor_SetPos(0xa, x2, y2);
        x3 = 0xb4 << 17;
        y3 = 0xda << 18;
        if (__GetFlag(0xeea) == 0)
            __MapActor_SetPos(0xb, x3, y3);
        break;
    case 10: OvlFunc_948_200a0c4(8, 0); break;
    case 8:  OvlFunc_948_200a0c4(9, 0); break;
    case 6:  OvlFunc_948_200a0c4(0xa, 0); break;
    case 4:  OvlFunc_948_200a0c4(0xb, 0); break;
    case 2:  OvlFunc_948_200a0c4(0xc, 1); break;
    }
}
