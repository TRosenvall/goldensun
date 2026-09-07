/* OvlFunc_914_200834c -- FindPushableFacingPlayer, overlay rom_7a1ff0, ovl_30_a_a_c_c_a.
 *
 * ONE OF SEVENTEEN BYTE-IDENTICAL COPIES.  The method, the measurements and
 * the family table are in scratch_elev/b254/f834c/NOTES.md; this file is that
 * one source with the function name and the three table symbols substituted.
 *
 *   OK OvlFunc_914_200834c -- 296 bytes, 143 encodings and 5 relocations identical
 *   (three passes, together with the other sixteen)
 *
 * LANDING: WHOLE, no split, no linker edit.  asmfacts.py says
 * `WHOLE  convert directly`; the .s holds one function and zero
 * .section/.global/incbin/.lcomm.  tryc.makefile_flags() = set() (plain -O2,
 * no wildcard rule fires).  The .ld line stays VERBATIM on the asm/ path:
 *   overlays/rom_7a1ff0/overlay.ld:22
 *       asm/overlays/rom_7a1ff0/ovl_30_a_a_c_c_a.o(.text)
 * The `.L` table symbols reached through __asm__ labels below are defined in
 * asm/overlays/rom_7a1ff0/ovl_30_c_c_c_c_c_c_c_b.s and are ALREADY `.global`
 * there -- grepped, no export to add, no asm file to touch.
 */
struct Model { unsigned char pad00[0x28]; short *f28; };

struct Ent {
    unsigned char pad00[6];
    unsigned short facing;
    int x;
    unsigned char pad0c[4];
    int z;
    unsigned char pad14[0x3c];
    struct Model *f50;
};

struct Rect { int x0, z0, x1, z1; };

extern unsigned char iwram_3001ebc[];
extern int gStep_914__02008ec8[] __asm__(".Lec8");
extern int gModelId_914__02008f08[] __asm__(".Lf08");
extern struct Rect gBox_914__02008f20[] __asm__(".Lf20");
extern struct Ent *__MapActor_GetActor(int slot);

struct Ent *OvlFunc_914_200834c(int *facingOut, int *slotOut, int *modelOut)
{
    struct Ent **tbl;
    struct Ent *pl;
    struct Ent *e;
    unsigned int slot, i;
    int tx, tz, x0, z0, x1, z1, ex, ez;

    tbl = (struct Ent **)(*(char **)iwram_3001ebc + 0x14);
    pl = __MapActor_GetActor(0);
    *facingOut = pl->facing >> 12;
    for (slot = 8; slot <= 0x41; slot++) {
        e = tbl[slot];
        for (i = 0; i <= 5; i++) {
            if (*e->f50->f28 != gModelId_914__02008f08[i])
                continue;
            *modelOut = i;
            tx = ((pl->x >> 16) + (gStep_914__02008ec8[*facingOut] >> 16)) >> 4;
            tz = ((pl->z >> 16) + (short)gStep_914__02008ec8[*facingOut]) >> 4;
            ex = *(short *)((char *)e + 0xa);
            x0 = (ex + gBox_914__02008f20[i].x0) >> 4;
            ez = *(short *)((char *)e + 0x12);
            z0 = (ez + gBox_914__02008f20[i].z0) >> 4;
            x1 = (ex + gBox_914__02008f20[i].x1) >> 4;
            z1 = (ez + gBox_914__02008f20[i].z1) >> 4;
            if (x0 > tx)
                continue;
            if (tx >= x1)
                continue;
            if (z0 > tz)
                continue;
            if (tz >= z1)
                continue;
            if (i & 1) {
                if (x0 == (pl->x >> 20))
                    continue;
                *slotOut = slot;
                return e;
            } else {
                if (z0 == (pl->z >> 20))
                    continue;
                *slotOut = slot;
                return e;
            }
        }
    }
    return 0;
}
