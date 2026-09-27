/* OvlFunc_947_2009aa8  --  0x02009aa8, split out of
 * asm/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c.s; OvlFunc_947_2009938 and
 * OvlFunc_947_20099f0 (parked) stay in _a.s, 2009be8 and 2009d84 in _c.s.
 *
 * Parked at 2 of 152: a two-instruction sched2 swap (`ldrb r1,[r0,#9]` vs
 * `and r2,r3`) in the `== 1` arm. Read out of -fsched-verbose=5: the two tie
 * in rank_for_schedule on priority (8), on class against the last-scheduled
 * insn (3) and on dependent count (4), so INSN_LUID decides and `and` wins.
 *
 * AN ALIAS-SET UNION ON THE BASE POINTER DECIDES THE TIE. `a->f59 |= 1` goes
 * through store_bit_field, where the store takes the alias set of the BASE
 * object (expr.c:5008), so a union on the member does nothing (and moved the
 * field to 0x5c). Casting the base -- `((union ActorSub *)a)->a.f59 =
 * a->f59 | 1;` -- gives the store the union's alias set, a superset of struct
 * Sub's, so it conflicts with the ldrb of the Sub byte: one more anti-dep,
 * five dependents, and the ldrb wins. The RHS read must keep its own alias
 * set. Every char-pointer store fixed the swap but landed at 5-6 (the direct
 * store path swaps the ior operands).
 *
 * The union on the f50 POINTER FIELD (not on the bitfield) is what makes the
 * f50 reload appear while the store stays a bitfield insert keeping
 * 0xfffffff3 -- see batch 285.
 */
struct Sub {
    unsigned char pad0[9];
    unsigned char f9_lo : 2;
    unsigned char f9_sel : 2;
    unsigned char f9_hi : 4;
    unsigned char pada[11];
    unsigned char f15_lo : 2;
    unsigned char f15_sel : 2;
    unsigned char f15_hi : 4;
};

struct Actor {
    unsigned char pad00[0xc];
    int fc;
    unsigned char pad10[0x13];
    unsigned char f23;
    unsigned char pad24[0x2c];
    union { struct Sub *p; unsigned char raw[4]; } f50;
    unsigned char pad54[5];
    unsigned char f59;
};

union ActorSub { struct Actor a; struct Sub s; };

extern signed char *iwram_3001ebc;
extern struct Actor *__MapActor_GetActor(int slot);
extern int OvlFunc_947_2009938(struct Actor *a, struct Actor *b);
extern int OvlFunc_947_20099f0(struct Actor *a, struct Actor *b);

int OvlFunc_947_2009aa8(struct Actor *a)
{
    struct Actor *b;
    unsigned int i;
    int n;

    b = __MapActor_GetActor(0);
    if (*(iwram_3001ebc + 0xcc7) == 1) {
        a->f50.p->f9_sel = b->f50.p->f9_sel;
        ((union ActorSub *)a)->a.f59 = a->f59 | 1;
    } else {
        n = OvlFunc_947_2009938(a, b);
        for (i = 8; i <= 0xb; i++)
            n += OvlFunc_947_2009938(a, __MapActor_GetActor(i));
        if (n != 0) {
            for (i = 8; i <= 0xb; i++)
                OvlFunc_947_20099f0(a, __MapActor_GetActor(i));
        }
        if (a->fc < b->fc) {
            a->f23 |= 2;
            a->f59 &= 0xfe;
            if (a->f50.p->f9_sel < b->f50.p->f9_sel) {
                a->f23 &= 0xfe;
                a->f50.p->f9_sel = b->f50.p->f9_sel;
                a->f50.p->f15_sel = b->f50.p->f15_sel;
                n = 1;
            }
        } else {
            a->f23 &= 0xfd;
            a->f59 |= 1;
        }
        if (n == 0)
            a->f23 |= 1;
    }
    return 0;
}
