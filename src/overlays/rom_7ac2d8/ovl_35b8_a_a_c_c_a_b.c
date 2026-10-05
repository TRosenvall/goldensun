/* OvlFunc_924_200d158 -- LANDING, byte-identical.  Batch 328, brief C.
 *
 * 0 differing encodings of 40.  88 bytes, 40 encodings and 4 relocations
 * identical, verified BOTH --func against the two-function piece and --whole
 * against the post-split single-function reference (whole file identical,
 * section tail included).  ZERO pins (tools/shimcount.py), no devices, no
 * per-file flag group, production flags.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a_b.c asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a_b.s --whole
 *
 * SPLIT SHAPE.  TWO functions in asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.s,
 * OvlFunc_924_200cfcc FIRST and this one SECOND, so a two-way TEXT split with
 * no _c part.  tools/split_s.py --dry-run:
 *     would write ovl_35b8_a_a_c_c_a_a.s  (1 function(s), 195 lines)
 *     would write ovl_35b8_a_a_c_c_a_b.s  (1 function(s),  47 lines)
 *     would REMOVE ovl_35b8_a_a_c_c_a.s ; rewrite overlays/rom_7ac2d8/overlay.ld
 * tools/datacheck.py prints nothing (no data section).  EXPORT LIST: EMPTY --
 * checked by label, not by assumption: the head defines .L500a .L5026 .L5036
 * .L50aa .L50b0 .L50b6 .L5112 .L5118 .L5140 .L5146 and the tail defines
 * .L51a4, with ZERO references in either direction.
 *
 * ============ WHAT WAS ACTUALLY WRONG, AND IT WAS NOT THE ALLOCATOR PRIORITY ============
 *
 * The park sat at 7 of 40 and called it "a genuine LOCAL OPTIMUM between two
 * requirements that currently exclude each other" -- p needing span 1 to win r3
 * while q had to be materialised before p's store to stay independent.  THAT
 * TENSION WAS AN ARTEFACT.  There were TWO defects and the park had neither:
 *
 *   1. A DEAD INSN WAS HOLDING r3.  The residue was one run, encodings 12-18,
 *      and the only allocation difference was p (the &f55 address) getting r1
 *      where the ROM has r3.  `.18.greg` says why in one line:
 *          ;; 43 conflicts: 33 34 35 37 43 45 3 13     <- trailing 3 = HARD r3
 *          43 in 1
 *      Pseudo 43 is p.  Pseudo 45 is `(set (reg:QI 45) (const_int 0))` carrying
 *      REG_UNUSED, sitting between p's address insn and p's store, and
 *      local-alloc still gave it r3 (`;; Register 45 in 3.`).  Traced through
 *      the dumps: in `.12.life` reg 45 is still live, consumed as
 *      `(subreg:SI (reg:QI 45) 0)` by the `*thumb_iorsi3` of `h->f9 |= 0xc`;
 *      combine substitutes it away and from `.13.combine` on it is REG_UNUSED.
 *      gcc-2.96 runs NO flow pass between combine and allocation, so the corpse
 *      is still an allocno at `.17.lreg`.
 *      THIS MECHANISM WAS ALREADY WRITTEN UP IN docs/elevation.md ("A REG_UNUSED
 *      insn still takes a hard register") -- worked end to end on
 *      OvlFunc_923_2009bc8, WHICH IS THIS FUNCTION'S dupfuncs TWIN.  The park
 *      never connected the two because parks are filed by their own address.
 *
 *   2. A sched2 TIE ON LUID.  Fixing (1) alone reads 2 of 40: encodings 14 and
 *      15 are `mov r7,#0` and `mov r2,r5` SWAPPED.  Two independent movs, so
 *      rank_for_schedule falls through to INSN_LUID, and the zero has to be
 *      established BEFORE the f22 pointer in source order.
 *
 * THE LEVER, AND WHY IT NEEDS TWO VARIABLES.  Expand only mints the QI corpse
 * because `z` is int and `f55` is u8, so the store needs a conversion.  Giving
 * the QImode zero its own named, really-read pseudo means no corpse is born.
 * BOTH an SImode zero and a QImode zero must exist as real pseudos:
 *     int z         alone, zero-first order            7 of 40
 *     u8  z         alone, zero-first order            7 of 40
 *     either alone with the f55 store before the pointer -- ONE INSTRUCTION
 *       SHORT (relocations slide 0x3a->0x38, 0x54->0x50)
 *     int zi = 0; u8 z = zi;                           0  <- this body
 * The cast is NOT the lever: `int z; ac->f55 = (u8)z;` is 7.
 *
 * MEASURED, three statement orders all land (zi/z first, then any of
 * p=&ac->f22 / ac->f55=z / *p=1 in the two remaining orders); putting
 * `p = &ac->f22` BEFORE the zero reads 2, and putting the f55 store AFTER
 * `*p = 1` reads 8.  The park's three levers (two independent bases, the
 * walking pointer, the shared zero) ALL STILL STAND and are all still here.
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

extern unsigned char gScript_924__0200de08[];
extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);
extern void __Sprite_SetAnim(struct SpriteHost *h, int n);

void OvlFunc_924_200d158(struct Actor *src)
{
    struct Actor *ac;
    struct SpriteHost *h;

    ac = __CreateActor(0x18, src->f8, src->fc, src->f10);
    if (ac != NULL) {
        int zi = 0;
        u8 z = zi;
        u8 *p;

        h = ac->f50;
        __Actor_SetScript(ac, gScript_924__0200de08);
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
