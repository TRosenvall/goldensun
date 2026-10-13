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
  * ===== BATCH 336: 2 OF 31 CONFIRMED; THE RESIDUE IS A CLOSED TWO-CELL SPACE. =====
 *
 * Re-measured, not inherited: 2 of 31 in STREAM terms (ref 31, ours 31),
 * instructions 29/29, SIZE 64/64, first differing index 25.  Pin-free.  Body
 * unchanged.  The two are indices 25 and 26:
 *
 *   rom    orrs r2, r1 / strb r2, [r0, #0x15]
 *   ours   orrs r1, r2 / strb r1, [r0, #0x15]
 *
 * (1) THE TWO-ADDRESS DIRECTION FOLLOWS THE FIRST OPERAND OF THE `|`, and that
 * is now a clean rule rather than a coupling to be guessed at:
 *   `e[0x15] = m | (n & e[0x15]);`   -> OR dest = the MASK   (this body, 2)
 *   `e[0x15] = (n & e[0x15]) | m;`   -> OR dest = the AND    (9)
 * The `&`'s OWN operand order is EXACTLY INERT, which the file did not record
 * before.  All four cells:
 *   `m | (n & e[0x15])`  2      `m | (e[0x15] & n)`  2
 *   `(n & e[0x15]) | m`  9      `(e[0x15] & n) | m`  9
 * Only the `|` matters, and it matters completely.
 *
 * (2) THE 9 REPRODUCES EXACTLY AS DESCRIBED: all nine are a pure r0 <-> r1
 * rename of (mask) against (the second byte load, the second pointer) at
 * indices 12, 14, 16, 19, 20, 22, 23, 25, 26, with every instruction SHAPE
 * identical.  Confirmed by side-by-side dump.
 *
 * (3) NAMING THE RESULT DOES NOT BUY THE DIRECTION -- it was worth checking
 * because the batch-333 lever that closed the length gap was exactly "name the
 * destination", and here it fails in the other direction.  Measured:
 *   `t = n & e[0x15]; e[0x15] = t | m;`                  14
 *   `t = n & e[0x15]; t |= m; e[0x15] = t;`              14
 *   `t = e[0x15]; t &= n; t |= m; e[0x15] = t;`          14
 *   `t = n; t &= e[0x15]; t |= m; e[0x15] = t;`          14
 *   `t = m | (n & e[0x15]); e[0x15] = t;`                19
 *   `t = n & e[0x15]; e[0x15] = m | t;`                  19
 * So the OR's destination is decided before regmove can be given a pseudo to
 * retarget, and the dest-naming lever is specific to the AND at site 1.
 *
 * (4) FURTHER MEASURED ON THE NINE, all still nine and all first-differing at
 * index 12 (the mask's materialisation) -- i.e. the allocation never budged:
 *   the zero store moved after the mask build; `n = 0xd; n = -n;` moved to just
 *   before site 1; one pointer variable serving both sites; `u` as
 *   `unsigned char`.
 * WORSE: `m = 0xc;` hoisted above the early `return` (to push the mask's
 *   pseudo out of local-alloc and into global-alloc, the batch-335
 *   "allocno membership" lever) 23 -- gcc emits `movs r1,#12` in the entry
 *   block, before the `cmp`, and the whole stream shifts; `n` built before the
 *   mask 16; naming the site-2 byte 14; naming the site-1 byte 18 and FOUR BYTES
 *   SHORT (SIZE 60 against 64); naming both bytes 24 and also four short.
 *
 * (5) WHERE THE WALL ACTUALLY IS.  The park's QTY_CMP_PRI reading is right in
 * substance -- local-alloc.c:1496-1498 is
 *   floor_log2(n_refs) * n_refs * size / (death - birth)
 * sorted by qty_compare_1 (:1397-1410) with "sort by qty number" as the ONLY
 * tie-break -- but the allocation that follows is NOT a plain first-fit.  Under
 * INSN_SCHEDULING, local-alloc tries find_free_reg with a FAKE LIFETIME first
 * (local-alloc.c:1398-1440): `fake_birth = MAX (0, birth - 2 + birth % 2)` and
 * `fake_death = MIN (..., death + 2 - death % 2)`, so a quantity born
 * immediately after this one dies, or dying immediately before it is born,
 * CONFLICTS.  Both bounds are parity-dependent on the insn index.  That makes
 * the r0/r1 assignment a function of WHERE IN THE INSN NUMBERING the mask's
 * birth and death fall, which is why every spelling-level change above is
 * exactly inert: none of them moves an insn boundary.
 *
 * NEXT, concretely: the thing to vary is the PARITY of the mask's birth or
 * death index -- the number of insns between the mask's materialisation and the
 * second byte load -- while keeping the instruction count at 29.  Nothing in
 * the source space tried so far does that, and the only two cells of the
 * `|`-operand space are the 2 and the 9, so this residue is a closed two-cell
 * space whose exit is an insn-numbering parity, not a construct.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 2 of 31, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: all 18 statement boundaries at every brace depth
 *   measured, all inert or worse.  No variant was capped and none failed to
 *   compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  Every brief so far has spent part of a round on
 *   exactly that.  The residue here is NOT a placement or flag question; it is a
 *   register choice, a tie, or a pass-level effect, and it needs a hypothesis.
 *
 *   Read as "inert against THIS body".  A recorded negative expires silently
 *   when the body changes -- six batches running, the decisive lever has turned
 *   up inside a park's own negative list.
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
