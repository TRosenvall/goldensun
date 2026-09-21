/* OvlFunc_883_200dd68 (0x0200dd68) -- NON-MATCHING, ONE EXTRA INSTRUCTION (ref 133 encodings /
 * 284 bytes; ours 135 / 288). Blocker class: TWO ROUTES THAT COST EACH OTHER, priced.
 *
 * asm/overlays/rom_780898/ovl_30_c_c_c_c_c_c_c_c_c_b.s (1 function, so landing needs NO split).
 * Progression 27 -> 17 -> 13 -> 1 real instruction, ~20 candidates.
 *
 * THE WHOLE REMAINING RESIDUE:
 *     rom   adds r3,r0,#0 / adds r3,#85 / movs r2,#0 / strb r2,[r3] / adds r3,#15 / strh r2,[r3]
 *     ours  adds r1,r0,#0 / adds r1,#85 / movs r2,#0 / adds r3,r0,#0 / strb r2,[r1] /
 *           adds r3,#100 / strh r2,[r3]
 * plus one `.short 0x0000` pool pad (a consequence of the odd count) and `negs r1,r1` one slot
 * off. Everything else and all 7 relocations match.
 *
 * ===== THE TWO ROUTES ARE MUTUALLY EXCLUSIVE, AND THE FORMULA PRICES IT =====
 *
 * The ROM CHAINS one address register; gcc builds two. It is NOT scheduling --
 * -fno-schedule-insns, -fno-schedule-insns2 and -fno-strength-reduce all still emit two base
 * copies. And the POINTER-WALK spelling DOES produce the ROM's exact chain -- but it costs the
 * allocation, measured in `.17.lreg`:
 *
 *     the walk gives the pooled zero one extra IN-LOOP use, so REG_N_REFS goes 6 -> 8,
 *     floor_log2 goes 2 -> 3, and priority goes 12/92 = 0.130 -> 24/92 = 0.261,
 *     OVERTAKING the loop index at 27/142 = 0.190.
 *
 * `i` and the pooled zero then swap r7/r8, which costs `mov r1, r8` in the loop -- EXACTLY THE
 * ONE INSTRUCTION THE WALK SAVED. Both routes assemble to 135. Eleven walk spellings (bare
 * `unsigned char *`, `unsigned short *`, `p[0]`, `((u16 *)p)[0]`, a `short` cast, two-pointer,
 * two statement-order permutations, a named zero) all held the zero at 8 refs. Naming the zero
 * restores 6 refs but LICM hoists it to the preheader where it takes a global register: 139.
 *
 * That is the clearest instance so far of the batch-277 loop-weighting rule biting the OTHER
 * way: an extra reference AT DEPTH is worth three, and here that is a cost rather than a lever.
 *
 * TWO LEVERS THAT DID LAND:
 *   * THE TAIL WANTS `arr[k]->...` DIRECTLY, NOT A REUSED LOCAL. Reassigning `a = arr[0]; ...
 *     a = arr[1];` makes `a` a global allocno gcc will not clobber, so `arr[1]->f23 = 2` costs
 *     `mov r2,r0 / adds r2,#35` where the ROM destroys r1 in place. 27 -> 17.
 *   * THE TWO TAIL BLOCKS WANT DIFFERENT STATEMENT ORDER. Block 0 is f6c then f8_b; block 1 is
 *     f8_b, then f6c, then f23. Both symmetric orderings measured 17 and 22; the ASYMMETRIC one
 *     is 13. Do not assume two similar blocks want the same order.
 *
 * NEXT: a spelling that gets the chain without the extra in-loop reference. On the arithmetic
 * that means the walk must not touch the pooled zero at depth.
 */
struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

struct Spr {
    unsigned char pad0[5];
    unsigned char f5_a : 5;
    unsigned char f5_b : 1;
    unsigned char f5_c : 2;
    unsigned char pad6;
    unsigned char f7_a : 6;
    unsigned char f7_b : 2;
    unsigned short f8_a : 10;
    unsigned short f8_b : 2;
    unsigned short f8_c : 4;
    unsigned char pad_a[0x12];
    unsigned char f1c;
    unsigned char f1d;
    unsigned char pad1e[8];
    unsigned char f26;
    unsigned char pad27;
    unsigned char *f28;
};

struct Actor {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0xb];
    unsigned char f23;
    unsigned char pad24[0x2c];
    struct Spr *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0xe];
    unsigned short f64;
    unsigned char pad66[2];
    struct Actor *f68;
    void *f6c;
};

extern struct SpriteSlot gSpriteSlots[];
extern unsigned char *iwram_3001f30;

extern struct Actor *__CreateActor(int kind, int x, int y, int z);
extern void __Sprite_SetAnim(struct Spr *s, int n);
extern void __Func_8003f3c(int n);
extern void OvlFunc_883_200dd14(void);
extern void OvlFunc_883_200dcc4(void);

void OvlFunc_883_200dd68(struct Actor *c)
{
    struct Actor *arr[2];
    struct Actor *a;
    struct Spr *s;
    unsigned char *w;
    int i;

    w = iwram_3001f30;
    for (i = 0; i <= 1; i++) {
        a = __CreateActor(0x1a, c->f8, c->fc, c->f10);
        arr[i] = a;
        if (a != 0) {
            a->f14 = c->f14;
            s = a->f50;
            a->f55 = 0;
            a->f64 = 0;
            a->f68 = c;
            if (s != 0) {
                __Sprite_SetAnim(s, 0);
                s->f26 = 0;
                __Func_8003f3c(s->f1c);
                s->f1c = *(unsigned short *)(w + 0x46);
                s->f1d |= 1;
                s->f8_a = gSpriteSlots[s->f1c].vramOffset >> 5;
                s->f5_b = 0;
                s->f5_c = 1;
                s->f7_b = 2;
                *(s->f28 + 0x16) = 0;
            }
        }
    }
    arr[0]->f6c = OvlFunc_883_200dd14;
    arr[0]->f50->f8_b = 1;
    arr[1]->f50->f8_b = 1;
    arr[1]->f6c = OvlFunc_883_200dcc4;
    arr[1]->f23 = 2;
}
