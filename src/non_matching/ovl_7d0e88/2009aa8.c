/* OvlFunc_947_2009aa8 -- NON-MATCHING, 2 encodings of 152.  SIZES EQUAL, INSTRUCTION
 * COUNT EXACT (152 = 152).  One of the closest parks in the corpus.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7d0e88/2009aa8.c \
 *     asm/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c.s --func OvlFunc_947_2009aa8
 *
 * Everything matches including all four `mov ip,..`/`mov ..,ip` round-trips through r12,
 * the cross-jumped shared `strb` tail, both `bls` unsigned loop bounds, the `signed char`
 * flag read (plain `char` loses the lsl/asr pair -- ARM `char` is UNSIGNED), and both f50
 * reloads.
 *
 * THE NEW LEVER IS THE VALUABLE PART, AND IT AMENDS A SIBLING PARK.  The `~0xc` constant
 * distinguishes a BITFIELD store from a HAND-MASKED BYTE store, and the two pull in
 * OPPOSITE directions -- the ROM has both:
 *
 *   p->sub->sel = q->sub->sel   (bitfield)  ~0xc as `mov r3,#0xd / neg r3,r3`  = ROM
 *                                           but CSE keeps the pointer, NO f50 reload
 *   b[9] = (b[9] & ~0xc) | ...  (byte)      ~0xc NARROWED to `mov r2,#0xf3`, 1 insn short
 *                                           but the f50 reload DOES appear
 *
 * THE RESOLUTION IS TO PUT THE UNION ON THE POINTER FIELD, NOT ON THE BITFIELD:
 *
 *     union { struct Sub *p; unsigned char raw[4]; } f50;
 *     ... a->f50.p->f9_sel = b->f50.p->f9_sel;
 *
 * The LOAD then has alias set 0 so the intervening byte store invalidates it and the
 * reload appears, while the STORE stays a bitfield insert and keeps 0xfffffff3.  Wrapping
 * the bitfields themselves does NOT work: gcc-2.96's union type-punning escape does not
 * reach a bitfield store, and one shape silently changed the union's alignment to 4 so
 * offset 9 became 0xc.  Confirmed by cross-check -- `-fno-strict-aliasing` produces the
 * same reload, which is how the cause was identified before hunting for a source route.
 *
 * THAT MAKES src/non_matching/ovl_7d0e88/2009938.c's CLOSING NOTE HALF-WRONG: it says "the
 * aliasing tell does not apply here ... a byte store is not it".  A byte store IS the tell
 * -- but the union has to go on the POINTER BEING RELOADED, not on the thing being stored.
 * Amend that park when it is next touched.
 *
 * BLOCKER: a TWO-INSTRUCTION sched2 SWAP inside ONE basic block, in the `== 1` arm only.
 * At index 21 the ROM issues `ldrb r1,[r0,#9]` BEFORE `and r2,r3`; this issues them the
 * other way.  Register allocation is already IDENTICAL -- purely ready-list order.  THE
 * SAME CONSTRUCT IN THE `else` ARM SCHEDULES CORRECTLY, so it is context, not spelling.
 *
 * SEVEN SPELLINGS INERT AT EXACTLY 2: a temp for the source bitfield (150/152, worse);
 * `a->f59 |= 1;` moved before the copy (134 differing); a named `struct Sub *sa`; a named
 * `struct Sub *sb` (150/152); both named; `a->f59 = a->f59 | 1` spelled out; a dummy
 * self-assignment of an adjacent bitfield (160/152); and both union member orderings.
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
        a->f59 |= 1;
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
