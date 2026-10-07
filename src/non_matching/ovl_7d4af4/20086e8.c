/* OvlFunc_949_20086e8  --  NOT MATCHING
 *
 * NON-MATCHING, 2 of 31 encodings  (MEASURED, batch 333).
 *   The two sides' 16-bit INSTRUCTION COUNTS now AGREE at 29 each, and the
 *   stream lengths agree at 31 each, so this positional figure IS a distance
 *   for the first time.  SIZE ref 64 bytes, ours 64.
 *   The superseded figure was eighteen of thirty-one, and it was PHASE: the
 *   body below was one instruction SHORT, so every encoding after the gap
 *   counted as differing.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7d4af4/20086e8.c \
 *     asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_c_c_a_a.s --func OvlFunc_949_20086e8
 *
 * This piece holds NO other function (one .thumb_func_start), so it converts
 * ALONE -- nothing else has to land with it.
 *
 * Source asm: goldensun/asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_c_c_c_c_c.s
 *
 * ---------------------------------------------------------------------------
 * WHAT CLOSED THE LENGTH GAP (batch 333).  The missing instruction was the
 * mask copy at the FIRST of the two and-or-store sites:
 *
 *      rom   mov r3, r2 / and r3, r0     <- the mask is COPIED and the copy is
 *      ours  and r3, r2                     the destination
 *
 * The standing verdict below called that unreachable because "both operands are
 * registers by the time the AND happens".  That verdict is REFUTED.  The copy
 * appears as soon as the read-modify-write is ONE EXPRESSION with no named
 * destination:
 *
 *      t = n; t &= v; t |= m; d[9] = t;        ->  and r3, r2        (one insn)
 *      d[9] = (n & d[9]) | m;                  ->  mov r3,r2 / and r3,r0
 *
 * Naming the destination `t` is what let regmove retarget the AND onto the
 * loaded byte's register; with no name there is no pseudo to retarget to and
 * the two-address expansion has to copy the mask.  This is the batch-332 lever
 * "A COPY ONLY SURVIVES WHEN IT IS NOT A COPY IN THE SOURCE" in a new costume:
 * here the copy survives when the DESTINATION is not a named variable.
 *
 * THE TWO SITES WANT OPPOSITE THINGS, which is why the park's own table never
 * found this -- it varied the first site only.  The second site's AND already
 * matched the reference (mask as destination, `and r2, r3`) and must be left
 * alone.
 *
 * WHAT THE REMAINING TWO ARE.  The second site's final OR:
 *
 *      rom   orr r2, r1 / strb r2, [r0, #0x15]   <- the AND result is the dest
 *      ours  orr r1, r2 / strb r1, [r0, #0x15]   <- the mask is the dest
 *
 * and it is COUPLED to where the mask lives, in opposite directions.  The
 * site-2 OR's source operand order decides BOTH:
 *
 *      e[0x15] = m | (n & e[0x15]);   mask in r1 (right), OR dest wrong  -> this body
 *      e[0x15] = (n & e[0x15]) | m;   OR dest right, mask in r0 (wrong)
 *
 * The second spelling reads nine of thirty-one and EVERY INSTRUCTION SHAPE IS
 * IDENTICAL to the reference -- all nine differences are a pure r0 <-> r1
 * rename of (mask) against (byte, second pointer).  So the residue is a
 * local-alloc ordering decision, not a construct: `QTY_CMP_PRI`
 * (local-alloc.c:1496-1498) is floor_log2(n_refs) * n_refs * size /
 * (death - birth), and the non-suggested allocation pass at
 * local-alloc.c:1384-1394 hands out registers in that order.  The reference
 * allocates the byte and the second pointer BEFORE the mask; we allocate the
 * mask first.  In the first spelling regmove ties the OR's destination to the
 * mask's pseudo, which merges the mask into the result, lengthens its range and
 * flips that order -- that IS the coupling.
 *
 * MEASURED AND EXACTLY INERT on the nine (all 29/29 instructions, nine
 * differing, first difference at the mask's materialisation): single-expression
 * `m = u & 0xc` after the pointer load; `m = 0xc & u`; four declaration orders
 * of the integer locals; `m &= u` against `m = m & u`; the source byte as
 * `unsigned char`; the mask as `unsigned int`; dropping the zero local;
 * dropping the biased pointer local in favour of `a[0x23] = z`; dropping the
 * argument copy and guarding the parameter itself; one pointer variable reused
 * for both sites; the mask computed immediately after its source byte; naming
 * both sites' results.
 * MEASURED WORSE: `m = s[9] & 0xc` (fifteen); the mask written out at both
 * sites with no named mask at all (seventeen); the split `n &= d[9]` form
 * (thirty instructions).
 *
 * NEXT: the r0/r1 rename.  It is the same wall as
 * src/non_matching/rom_c9000/80e3908.c's residue -- read that park's note.
 * ---------------------------------------------------------------------------
 *
 * STANDING OBSERVATIONS, kept because they are sound (the VERDICT above them
 * was not):
 *
 * The mask -13 is used twice, so it has to stay live either way; the ROM keeps
 * it by copying into a scratch register and ANDing there.  `n = 0xd; n = -n;`
 * is what produces the reference's `mov r2, #13 / neg r2, r2` pair -- a signed
 * -13, NOT the unsigned `0xf3` that an unsigned byte field would collapse to.
 *
 * The ordering of the constant against the actor-pointer load is not a lever:
 * the ROM materialises the mask before loading the pointer and gcc emits the
 * load first whatever order the source assigns them.  Same non-effect as
 * src/non_matching/rom_b5000/80be02c.c.
 *
 * NOT the r4-without-push question.  This function uses r4 under a
 * `push {r5, lr}` prologue, which looks like a clobbered callee-saved register,
 * but r4 is call-used in this build and that lead was checked and dropped in
 * src/non_matching/ovl_77a7c8/200811c.c.
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
    n = -n;
    d[9] = (n & d[9]) | m;
    e = *(unsigned char **)(a + 0x50);
    e[0x15] = m | (n & e[0x15]);
}
