/* OvlFunc_924_200d158 -- 0x0200d158, asm/overlays/rom_7ac2d8/ovl_35b8_a_a_c_c_a.s
 * (two functions; tools/datacheck.py confirms no data section).
 *
 * BLOCKER CLASS: a two-register rotation between two address bases. SIZE EXACT
 * -- 40 instructions against 40 -- with 7 encodings differing.
 *
 * WHAT IT DOES: spawns actor kind 0x18 at the caller's coordinates, gives it a
 * script, sets three bytes on it, and if it has a sprite host sets that host's
 * animation and two more bytes.
 *
 * THREE LEVERS, 29 differing to 7, and all three are about byte stores whose
 * offsets exceed Thumb's `strb` immediate range (31), so each needs its own
 * address register:
 *
 *   1. TWO INDEPENDENT BASES, NOT ONE CHAINED. Writing the three fields as
 *      plain struct stores makes gcc compute `&f55` and then DERIVE `&f22` from
 *      it with `sub r2, #51`; the ROM computes both from the actor. Naming a
 *      pointer for the f22/f23 pair is what splits them.
 *   2. THE PAIR IS A WALKING POINTER. `ac->f23 = 2` gives `strb r3, [r2, #1]`;
 *      the ROM has `add r2, #1 / strb r3, [r2]`, so the source advances the
 *      pointer between the two stores.
 *   3. THE ZERO IS SHARED WITH THE LATER STORE. The ROM keeps 0 in r7 across
 *      the whole body and uses it for both `f55 = 0` and the sprite host's
 *      `f26 = 0`. A named local does it; two literals do not (18 differing).
 *
 * THE RESIDUE is which register the f55 base gets -- r3 in the ROM, r1 here --
 * and the four instructions whose order follows from it. The ROM's `strb` of
 * the zero happens BEFORE `mov r3, #1`, so r3 is free to be reused for the
 * constants; ours schedules the constant first, so the base has to live
 * somewhere else. That is sched2 placing an independent `mov` early, the same
 * shape as the recorded Func_942e0 residue.
 *
 * MEASURED AND INERT, all 7: three declaration permutations, `&ac->f55` through
 * a named pointer against the plain field, and assigning the zero before the
 * pointer. MEASURED AND WORSE (back to 29 and two instructions short): moving
 * the f22 pointer's computation after the f55 store, in either arrangement.
 *
 * NEXT: this is the smallest rotation in the pool -- TWO values -- and unlike
 * the main-ROM specimens it is a rotation between two SCRATCH registers rather
 * than callee-saved ones, so it may be a different mechanism. Worth comparing
 * against Func_80165d8 (three values) before assuming they are the same class.
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
    u8 *p;
    u8 *q;
    int z;

    ac = __CreateActor(0x18, src->f8, src->fc, src->f10);
    if (ac != NULL) {
        h = ac->f50;
        __Actor_SetScript(ac, gScript_924__0200de08);
        p = &ac->f22;
        z = 0;
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
