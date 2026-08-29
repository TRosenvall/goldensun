/* OvlFunc_949_20086e8  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_c.s
 * Best screen: 7 instructions in disagreeing regions, of 31 (rom 31, ours 30).
 *
 * BLOCKER CLASS: register-register AND canonicalisation, plus one reordering.
 *
 *      rom   mov r3, r2 / and r3, r0     <- the mask is COPIED and the copy is
 *      ours  and r3, r2                     the destination
 *
 * The mask -13 is used twice, so it has to stay live either way; the ROM keeps
 * it by copying into a scratch register and ANDing there, gcc keeps it by
 * ANDing into the loaded byte's register instead. Ours is one instruction
 * shorter.
 *
 * This is the same floor as src/non_matching/rom_c0/8006384.c: the
 * constant-as-destination lever needs a LITERAL on one side, and here both
 * operands are registers by the time the AND happens, because the mask came
 * from `neg`. gcc canonicalises register-register AND regardless of which side
 * the source names first, and `t = n; t &= v;` -- which is exactly the copy the
 * ROM shows -- is what is written below and does not produce it.
 *
 * The second difference is ordering: the ROM materialises `mov r1, #0xc` before
 * loading the actor pointer, and gcc emits the load first, even though the
 * source assigns the constant first. Same non-effect as
 * src/non_matching/rom_b5000/80be02c.c, where both statement orders are
 * byte-identical.
 *
 * NOT the r4-without-push question. This function uses r4 under a
 * `push {r5, lr}` prologue, which looks like a clobbered callee-saved register,
 * but that lead was checked and dropped in
 * src/non_matching/ovl_77a7c8/200811c.c -- 826 of 2779 unelevated functions do
 * the same, so it is ordinary codegen and not a signal.
 */
extern unsigned char *__MapActor_GetActor(int slot);

void OvlFunc_949_20086e8(unsigned char *actor)
{
    unsigned char *a;
    unsigned char *q;
    unsigned char *s;
    unsigned char *p;
    unsigned char *d;
    unsigned char *e;
    int u;
    int z;
    int m;
    int n;
    int t;
    int v;

    a = actor;
    if (a == 0)
        return;
    q = __MapActor_GetActor(0);
    s = *(unsigned char **)(q + 0x50);
    p = a;
    p += 0x23;
    u = s[9];
    z = 0;
    *p = z;
    m = 0xc;
    d = *(unsigned char **)(a + 0x50);
    m &= u;
    n = 0xd;
    v = d[9];
    n = -n;
    t = n;
    t &= v;
    t |= m;
    d[9] = t;
    e = *(unsigned char **)(a + 0x50);
    v = e[0x15];
    n &= v;
    n |= m;
    e[0x15] = n;
}
