/*
 * OvlFunc_951_2008dd0 -- asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_c.s
 *
 * BLOCKER: the +0x27 register-role swap. 56 lines against 57 -- ONE SHORT --
 * and the first difference is the now-familiar
 *
 *      rom   mov r3, r0 / add r3, #0x27 / ldrb r3, [r3]
 *      ours  mov r2, r0 / add r2, #0x27 / ldrb r3, [r2]
 *
 * THIS IS THE THIRD INSTANCE of the family that already holds Func_808e0b0
 * (3 of 52) and Func_8096b88 (7 of 49): same +0x25/+0x27/+0x28 actor-array
 * layout, same address-temp register choice. All three should be re-attacked
 * together.
 *
 * Four orderings of the three preheader assignments were measured here and the
 * one below is the best; every permutation is worse (23 differing against 25,
 * 25 and 26). Consistent with the family: the swap that helps Func_808e0b0
 * hurts Func_8096b88, and statement order does not transfer.
 *
 * SETTLED, and this is the finding worth keeping:
 *
 *   A POOLED ZERO IS A SYMBOL. The ROM emits `ldr r2, =0` and uses it for two
 *   BYTE stores. Zero is the cheapest possible immediate, so this is the
 *   strongest form of const.sym's symbol tell -- but it was worth checking
 *   whether byte stores have a QImode analogue of the halfword exception, the
 *   way `*(short *)p = 0` genuinely does pool. MEASURED: they do not. Compiling
 *
 *       p[0x55] = 0;  q[0x26] = 0;
 *
 *   emits `mov r2, #0` and reuses it for both. So the pool entry means a
 *   symbol, and substituting one moves the first difference from line 17 to
 *   line 29 -- past every store in the guarded block.
 *
 *   The reference below uses area.sym's _AREA_00 because it is the only
 *   zero-valued symbol in the tree and it proves the shape. It is almost
 *   certainly the WRONG NAME: the value is stored to a byte at +0x55 and to
 *   actor+0x26, not to the area halfword. No const.sym entry was added, because
 *   that file's bar wants a symbol that completes a function and this one still
 *   differs by the family blocker above. Resolve the family first, then name it.
  *
 * FAMILY UPDATE: Func_808e0b0 is now ELEVATED using struct types plus a guard
 * written as `i = 0; if (i < o->f27)`. That shape is worth trying here before
 * anything else -- this park is one instruction short on the same address-temp
 * residue. It did NOT transfer to Func_8096b88, so measure rather than assume.
 *
 * FAMILY LEVER APPLIED, and it worked as far as it can. Struct types plus the
 * guard written `i = 0; if (i < q->f27)` FIXES the address-temp residue that
 * this park was recorded on: the first difference moves from line 29 to line
 * 37 and the count from 23 differing to 20. The candidate below is updated to
 * that form.
 *
 * WHAT IS LEFT IS A DIFFERENT BLOCKER. At line 37 the ROM has `b .Le24` where
 * we fall straight through, and .Le24 sits immediately after a `.pool_aligned`
 * -- so that branch is the jump OVER a mid-function literal pool, not a loop
 * rotation. Our function is short enough that gcc puts its pool at the end and
 * emits no branch, which is why we are one instruction short.
 *
 * That is the branch-over-pool class. Batch 141 showed it is not a ceiling and
 * that pool ENTRY ORDER is source-reachable, so this is worth re-attacking with
 * those levers -- but it is not the address-temp problem any more, and the park
 * header above should be read with that in mind.
*/
extern int _AREA_00;

struct Ent {
    unsigned char pad0[5];
    unsigned char f5;
    unsigned char pad6[0x16 - 6];
    unsigned char f16;
};

struct Obj {
    unsigned char pad0[0x26];
    unsigned char f26;
    unsigned char f27;
    struct Ent *f28[1];
};

struct Actor {
    unsigned char pad0[6];
    short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x50 - 0x14];
    struct Obj *f50;
    unsigned char pad54;
    unsigned char f55;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetAnimSpeed(struct Actor *a, int n);

void OvlFunc_951_2008dd0(int slot, int *src, int h, int v, int sp)
{
    struct Actor *e;
    struct Obj *q;
    struct Ent *r;
    struct Ent **list;
    unsigned int i;
    unsigned int n;
    int m;
    int t;

    e = __MapActor_GetActor(slot);
    if (e != 0) {
        e->f8 = *src++;
        e->fc = *src++;
        e->f10 = *src;
        e->f6 = h;
        e->f55 = (int)&_AREA_00;
        e->f50->f26 = (int)&_AREA_00;
        __Actor_SetAnimSpeed(e, sp);
    }
    q = e->f50;
    i = 0;
    if (i < q->f27) {
        m = 0xff;
        list = q->f28;
        n = q->f27;
        do {
            r = *list++;
            if (r->f5 != v) {
                t = r->f16 | m;
                r->f5 = v;
                r->f16 = t;
            }
            n--;
        } while (n != 0);
    }
}
