/* OvlFunc_923_2009bc8 -- LANDING, byte-identical.  Batch 328, brief C.
 *
 * 0 differing encodings of 40.  88 bytes, 40 encodings and 4 relocations
 * identical; --whole against the post-split reference is identical for the
 * whole file.  ZERO pins, no devices, no flag group.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/overlays/rom_7aa430/ovl_1a3c_a_a_a_b.c asm/overlays/rom_7aa430/ovl_1a3c_a_a_a_b.s --whole
 *
 * THIS IS OvlFunc_924_200d158's BODY WITH TWO RENAMES.  tools/dupfuncs.py pairs
 * them, and the pairing is exact at the FILE level, not just the function level:
 * asm/overlays/rom_7aa430/ovl_1a3c_a_a_a.s and
 * asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.s are both 244 lines, both hold
 * exactly two functions with the labels at lines 13 and 204, and a normalised
 * diff of the two files is EMPTY after mapping 2 function names, 3 gScript names
 * and every .L<hex> label.  So `ovl_1a3c_a_a_a.s` is a wholesale copy of
 * `ovl_35b8_a_a_c_c_a.s`, and the renames are:
 *     OvlFunc_924_200cfcc   -> OvlFunc_923_2009a3c
 *     OvlFunc_924_200d158   -> OvlFunc_923_2009bc8
 *     gScript_924__0200de38 -> gScript_923__0200a7e8
 *     gScript_924__0200de20 -> gScript_923__0200a7d0
 *     gScript_924__0200de08 -> gScript_923__0200a7b8
 *
 * SPLIT SHAPE.  Identical to the twin's: two-way TEXT split, no _c part.
 *     would write ovl_1a3c_a_a_a_a.s  (1 function(s), 195 lines)
 *     would write ovl_1a3c_a_a_a_b.s  (1 function(s),  47 lines)
 * tools/datacheck.py prints nothing.  EXPORT LIST: EMPTY.
 *
 * For the mechanism -- a REG_UNUSED corpse from combine holding r3, plus a
 * sched2 LUID tie -- see the twin at src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a_b.c.
 * docs/elevation.md's "A REG_UNUSED insn still takes a hard register" section is
 * written up against THIS function's number and is the same defect.
 */
#include "gba/types.h"

struct SpriteHost {
    u8 pad_00[9];
    u8 f9;
    u8 pad_0a[0x1c];
    u8 f26;
};

struct Actor {
    u8 pad_00[8];
    int f8;
    int fc;
    int f10;
    u8 pad_14[0x22 - 0x14];
    u8 f22;
    u8 f23;
    u8 pad_24[0x50 - 0x24];
    struct SpriteHost *f50;
    u8 pad_54;
    u8 f55;
};

extern unsigned char gScript_923__0200a7b8[];
extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void __Sprite_SetAnim(struct SpriteHost *h, int n);

void OvlFunc_923_2009bc8(struct Actor *src)
{
    struct Actor *ac;
    struct SpriteHost *h;

    ac = __CreateActor(0x18, src->f8, src->fc, src->f10);
    if (ac != NULL) {
        int zi = 0;
        u8 z = zi;
        u8 *p;

        h = ac->f50;
        __Actor_SetScript(ac, gScript_923__0200a7b8);
        p = &ac->f22;
        ac->f55 = z;
        *p = 1;
        p++;
        *p = 2;
        if (h != NULL) {
            __Sprite_SetAnim(h, 2);
            h->f26 = z;
            h->f9 |= 0xc;
        }
    }
}
