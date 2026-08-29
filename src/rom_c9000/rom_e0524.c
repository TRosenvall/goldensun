/* Cluster LoadVFXFile..LoadVFXFile extracted from goldensun/asm/rom_c9000/rom_e0524.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/stage1.ld is unchanged.
 *
 * From the annotation on the original .s:
 *
 *   r0=resource id, r1=destination, r2=skip-palette flag, r3=upload-palette
 *   flag. Decompresses the resource with GetFile, then: when r3 is set, DMAs
 *   the first 0x80 bytes (a 64-colour palette) to 0x5000000 through the
 *   RAM-resident copier Func_1af8; when r2 is set, advances past that palette
 *   so only the pixel data is used; and finally copies the remainder to the
 *   destination with DecompressLZ. Every animation in this module stages its
 *   graphics through here.
 *
 * THE INDIRECT CALL IS THE POINT, and it is the first one matched in the tree.
 *
 * The ROM does not `bl Func_8001af8`. It loads the address and branches
 * through it:
 *
 *     ldr r3, =Func_8001af8 / ... / bl _call_via_r3
 *
 * which is what gcc emits for a call through a POINTER. A direct call is one
 * instruction shorter and screens at 28 against 29. Writing the callee into a
 * local of function-pointer type first:
 *
 *     CopyFn copy;  ...  copy = Func_8001af8;  copy(pal, data, 0x80);
 *
 * matches. gcc-2.96 does NOT constant-propagate the pointer back into a direct
 * call at -O2, which is the thing worth knowing -- a modern compiler would, and
 * that is presumably why this shape had not been tried.
 *
 * That makes the whole `_call_via_rN` class reachable. Every other file in the
 * tree that mentions `_call_via_r3` is currently parked
 * (src/non_matching/rom_c9000/rom_dc968.c, rom_b5000/rom_b63b0.c,
 * rom_15000/rom_1671c.c), though none of them is parked ON this -- they are
 * large functions with other residue -- so this is an opening, not a fix for
 * those three.
 *
 * The palette address is built at runtime -- `mov r0, #0xa0 / lsl r0, #19` for
 * 0x05000000 -- so it is written as separate statements over a named local
 * rather than as the constant, the same statement-form lever GetEntrances
 * needed. Spelled as `0x5000000` it becomes a pool load.
 */
#include "gba/types.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, s32 len);

extern void Func_8001af8(volatile u16 *dst, void *src, s32 len);
extern u8 *GetFile(s32 id);
extern void DecompressLZ(void *src, void *dst);

void LoadVFXFile(s32 id, void *dst, s32 skipPalette, s32 uploadPalette)
{
    u8 *data;
    u32 pal;
    CopyFn copy;

    data = GetFile(id);
    if (uploadPalette != 0) {
        pal = 0xa0;
        copy = Func_8001af8;
        pal <<= 19;
        copy((volatile u16 *)pal, data, 0x80);
    }
    if (skipPalette != 0)
        data += 0x80;
    DecompressLZ(data, dst);
}
