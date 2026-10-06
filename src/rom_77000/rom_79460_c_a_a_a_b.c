/* AddPartyMember (0x0807961c) -- MATCHING, batch 332 D.
 *
 * MATCHES: 72 bytes, 33 encodings and 3 relocations identical, whole file.
 * PINS: 0.  No shim, no device, no flag group, no fakematch row.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_77000/rom_79460_c_a_a_a_b.c \
 *     asm/rom_77000/rom_79460_c_a_a_a_b.s --whole
 *
 * INSTALL PATH: src/rom_77000/rom_79460_c_a_a_a_b.c
 * SPLIT REQUIRED.  tools/datacheck.py on the parent piece prints nothing, so
 * the split is TEXT-ONLY.  tools/split_s.py on the parent naming this function
 * with --dry-run writes _b.s (this function, 44 lines) and _c.s
 * (Func_8079664, 53 lines) and removes the parent.  Func_8079664 does NOT land
 * this batch -- it is re-derived at 14 differing encodings of 44 and stays
 * parked, which is why the piece is split rather than converted whole.
 *
 * ---------------------------------------------------------------------------
 * WHAT THE PARK HAD RIGHT, AND THE ONE THING IT HAD BACKWARDS
 *
 * The park's map of the residue was exact and is worth preserving: the loop
 * body was already byte-identical, and the whole difference was that the ROM
 * puts the loop guard FIRST and then rematerialises `ldr r0, =gState` on the
 * guard's not-taken path, which makes the store block a join reached from two
 * places and costs an extra unconditional branch.  Reference minus ours was
 * exactly those two instructions.
 *
 * Its verdict was that this is "gcc declining to rematerialise a pool load
 * that the original build did rematerialise", i.e. an allocator difference no
 * source form selects.  That is the part that was wrong.  The rematerialised
 * load is not a rematerialisation at all -- it is a SECOND READ ON A SECOND
 * PATH, and the construct that puts it there is an if/ELSE whose else-arm
 * assigns the base:
 *
 *     if (i < n) { g = gState; p = g + 0x1f8; do { ... } while (i < n); ... }
 *     else       { g = gState; }
 *     g[i + 0x1f8] = id;
 *
 * gcc then emits the else-arm's load as its own block on the guard's
 * not-taken edge, and the then-arm's fallthrough has to branch over that block
 * to reach the join -- which is the second missing instruction.  Both come from
 * one structural change.
 *
 * THE PARK HAD ALREADY MEASURED THE NEIGHBOURING SPELLING AND READ IT AS A
 * CLOSED DOOR.  Its row "a second named gState read for the store" was
 * recorded as commoned by CSE and therefore inert.  It is inert when the two
 * reads sit on the SAME path, which is what that spelling does -- the store
 * block directly follows the loop, so the second read is plainly redundant.
 * The else-arm puts the second read on a path where it is NOT redundant, and
 * then nothing commons it.  Same number of reads; different control flow.
 *
 * THE OTHER HALF IS BLOCK LAYOUT, and it has to be done at the same time.
 * gcc-2.96 lays basic blocks out in the order the RTL was emitted, so the
 * early-exit block has to be written INSIDE the then-arm, after the loop, for
 * the ROM's order (guard, preheader, loop, branch-to-join, found block,
 * else-arm, join, shared epilogue) to come out.  Writing the early exit after
 * the if/else instead puts the found block last and the join second, which is
 * our old shape with the branch in the other place.  The park's own
 * observation that `goto found` is what puts the `return n` block out of line
 * is the same fact seen from one step back.
 *
 * STILL TRUE AND STILL REQUIRED, carried over from the park:
 *   * the NAMED base.  `p = gState + 0x1f8;` as one expression folds to a
 *     pooled gState+504 and the runtime `mov #0xfc / lsl #1 / add` disappears;
 *     `g = gState; p = g + 0x1f8;` keeps it.  Both arms must name it.
 *   * the walking pointer `*p++` in the search loop.
 *   * the NAMED index `k = i + 0x1f8` for the register-plus-register store.
 *   * the explicit `i = 0;` before the guard.  gcc does NOT fold
 *     `i = 0; if (i < n)` to `if (n > 0)` -- it compares the variable, which is
 *     the reference's `mov r2, #0 / cmp r2, r5 / bge`.  The park measured this
 *     spelling as four lines worse and it is four lines CHEAPER once the
 *     else-arm is there; a lever measured without its prerequisite reads as a
 *     cost.
 */
extern unsigned char gState[];
extern int GetPartySize(void);
extern void SetFlag(int id);

int AddPartyMember(int id)
{
    unsigned char *g;
    unsigned char *p;
    int n;
    int i;
    int k;

    n = GetPartySize();
    SetFlag(id);
    i = 0;
    if (i < n) {
        g = gState;
        p = g + 0x1f8;
        do {
            if (*p++ == id)
                goto found;
            i++;
        } while (i < n);
        goto store;
    found:
        return n;
    } else {
        g = gState;
    }
store:
    k = i + 0x1f8;
    g[k] = id;
    return n + 1;
}
