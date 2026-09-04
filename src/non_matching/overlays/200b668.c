/* OvlFunc_932_200b668 -- 0x0200b668, and its twin
 * OvlFunc_932_200b5ac -- 0x0200b5ac,
 * both in asm/overlays/rom_7b9cb4/ovl_30_c_a.s
 *
 * Swings an actor through a quarter turn: snap its facing to a 0x4000 boundary,
 * step 0x180000 along it to find the orbit centre, snap that to a 0x100000
 * grid, then run sixteen frames rotating 0x400 per frame and writing the new
 * position and facing back each time. A sound at each end.
 *
 * 5 of 83, with the instruction count exact and every operation right.
 *
 * BLOCKER: A TWO-REGISTER ROTATION, r8 against r10. The ROM puts the saved z
 * coordinate in r10 and the loop counter in r8; we get the reverse, and the
 * three follow-on differences are just those two registers appearing in the
 * loop body and the decrement.
 *
 * REG_ALLOC_ORDER runs {..., 4, 5, 6, 7, 8, 10, 9, 11}, so r8 is handed out
 * before r10 and goes to whichever pseudo is allocated first. Allocation order
 * follows local-alloc's priority, not declaration order, which is why the
 * declaration sweep below is completely inert -- all three orders give the
 * identical 8. In the ROM's build the loop counter won that race; in ours the
 * saved coordinate does.
 *
 * TRIED -- SIX spellings, three of them tying at EXACTLY 5:
 *   a   as written, stores in the loop as v[2] then v[0]        8 differing
 *   b   loop counter declared first                             8
 *   c   sz declared before sx                                   8
 *   d   loop counter declared before everything                 8
 *   e   the two loop stores SWAPPED to v[0] then v[2]           5  <- best
 *   f   the two save computations swapped                       9
 *   j1  __vec3_translate declared with unsigned parameters      5
 *   j2  sx and sz declared unsigned                             5
 *   j3  the angle declared unsigned                             5
 * h  a do/while loop with the decrement written out            77
 *
 * The signedness levers (j1-j3) are entirely inert, which is worth recording:
 * the neighbour file declares __vec3_translate with unsigned parameters and it
 * makes no difference at all here, so copying a neighbour's declaration is not
 * automatically right and not automatically wrong -- it simply does not reach
 * this residue.
 *
 * WHAT WAS WON: swapping the two stores at the top of the loop body is worth
 * three instructions (8 -> 5). The ROM writes v[0] before v[2]; writing them in
 * the other order lets gcc pick its own and it picks wrong. Cheap, and easy to
 * miss because both orders look equally natural in source.
 *
 * THE TWIN LANDS IDENTICALLY, which is the strongest evidence here that this is
 * the allocator and not the source. OvlFunc_932_200b5ac is the same routine
 * rotating the other way -- `+ 0x4000` where this one has `- 0x4000`, anim 5
 * for 6, and `ang += 0x400` for `-=`. Written to the same shape it screens at
 * 86 of 86 instructions with FIVE differing and the SAME r8/r10 rotation at
 * four sites. Two independently written functions reaching the identical
 * residue by the identical mechanism is not a spelling accident.
 *
 * This is a good REG_ALLOC_ORDER probe, like OvlFunc_919_200805c: the residue
 * is two registers and nothing else, so if that hypothesis is ever tested this
 * should go to zero and nothing else can move -- and the twin gives a free
 * second reading of the same experiment.
 */

struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
};

extern void __vec3_translate(int dist, int angle, int *v);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);

void OvlFunc_932_200b668(struct Actor *a)
{
    int v[3];
    int ang;
    int sx;
    int sz;
    int i;

    ang = (a->f6 - 0x4000) & 0xc000;
    v[0] = a->f8;
    v[1] = a->fc;
    v[2] = a->f10;
    __vec3_translate(0xc0 << 13, ang, v);
    sx = (v[0] + (0x80 << 12)) & 0xfff00000;
    sz = (v[2] + (0x80 << 12)) & 0xfff00000;
    ang += 0x80 << 8;
    __Actor_SetAnim(a, 6);
    __PlaySound(0xb8);
    for (i = 15; i >= 0; i--) {
        ang -= 0x400;
        v[0] = sx;
        v[2] = sz;
        __vec3_translate(0xc0 << 13, ang, v);
        a->f8 = v[0];
        a->f10 = v[2];
        a->f6 = ang - 0x4000;
        __WaitFrames(1);
    }
    __PlaySound(0xe9);
}

void OvlFunc_932_200b5ac(struct Actor *a)
{
    int v[3];
    int ang;
    int sx;
    int sz;
    int i;

    ang = (a->f6 + (0x80 << 7)) & 0xc000;
    v[0] = a->f8;
    v[1] = a->fc;
    v[2] = a->f10;
    __vec3_translate(0xc0 << 13, ang, v);
    sx = (v[0] + (0x80 << 12)) & 0xfff00000;
    sz = (v[2] + (0x80 << 12)) & 0xfff00000;
    ang += 0x80 << 8;
    __Actor_SetAnim(a, 5);
    __PlaySound(0xb8);
    for (i = 15; i >= 0; i--) {
        ang += 0x80 << 3;
        v[0] = sx;
        v[2] = sz;
        __vec3_translate(0xc0 << 13, ang, v);
        a->f8 = v[0];
        a->f10 = v[2];
        a->f6 = ang + (0x80 << 7);
        __WaitFrames(1);
    }
    __PlaySound(0xe9);
}
