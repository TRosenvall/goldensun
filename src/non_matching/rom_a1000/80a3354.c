/* Func_80a3354 (0x080a3354) -- NON-MATCHING.
 *
 * NON-MATCHING, 25 of 58 encodings  (MEASURED batch 334, brief C, on the body
 *   BELOW -- see the batch-334 note further down for the arithmetic and for the
 *   body the superseded figure of TWENTY-SEVEN was measured on).
 *   COUNT DIFFERS (ref 58, ours 56) and the instruction count is ref 51 / ours
 *   50, so this positional figure still measures MISALIGNMENT and the honest
 *   statement is WE ARE ONE 16-BIT INSTRUCTION SHORT.  Read the count first.
 *   SIZE ref 128 bytes, ours 124.
 *   (The batch-319 recipe backfill measured TWENTY-SEVEN on a different body.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a3354.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c_a.s --func Func_80a3354
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: address-birth placement in the loop setup.
 *
 * 54 lines against the ROM's 56, 27 differing, with the first eleven exact and
 * the whole loop body, the CreateUIBox call and its stack argument correct.
 *
 * The divergence is where the loop's base address is computed:
 *
 *     rom    mov r0,#0xa5 / lsl r0,#1 / ldr r1,=0x1e / mov r2,#3 / add r3,r7,r0
 *     ours   mov r1,#0xa5 / lsl r1,#1 / add r3,r7,r1 / ldr r1,=0x1e / mov r2,#3
 *
 * The ROM builds the offset, sets up the loop's stored value and counter, and
 * only THEN forms the address. Ours forms it immediately, which also costs the
 * register the ROM keeps the offset in.
 *
 * MEASURED, all 54 lines and 27 differing:
 *   the pointer assigned after `i = 3;`
 *   the offset named in its own local and the pointer assigned after `i = 3;`
 *
 * So the address-local birth-statement lever does NOT reach this one either.
 * That is now two negatives against it (see src/non_matching/ovl_797990/
 * 2008c1c.c for the first); the lever's demonstrated scope remains a pointer
 * COMPUTED from a base in straight-line code, not one feeding a loop.
 *
 * WHAT IS RIGHT: the pooled 0x1e. The ROM loads it from a mid-function pool
 * (`ldr r1, .La33b0 @ 0x1e`) for a halfword store while using `mov r2, #0x1e`
 * for the same value as a call argument a few instructions later -- the
 * halfword exception running in the ROM-pools direction, so the plain literal
 * is correct at both sites and naming either would be the error. The
 * decrement-first loop, the shared `2` between the stack argument and the last
 * byte store, and the derived 0x111/0x113 offset pair are all exact.
 *
 * NEXT: the two-line deficit is not yet attributed; the mid-function pool
 * makes tryc's line count unreliable here, so this wants a build-gated check
 * rather than more screening.
 *
 * ============== BATCH 334, BRIEF C: RE-PRICED AND ONE OF TWO CLOSED ==============
 *
 * The figure claimed at the top of this file was measured on the body below.
 * Its size is unchanged from the superseded body because the instruction gained
 * costs two bytes and the pad it displaces gave two back.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a3354.c \
 *     asm/rom_a1000/rom_a1814_c_a_c_c_a_c_c_c_c_c_a.s --whole
 *
 * ==================== THE DEFICIT IS REGISTER-OFFSET ADDRESSING ====================
 *
 * The previous header said "the two-line deficit is not yet attributed".  It is
 * attributed now, and it is not the loop setup at all -- the loop setup is a
 * BYTE difference, not a LENGTH one.  Both missing instructions were at the two
 * stores through the derived offset at the end of the function:
 *
 *     rom    ldr r0,=0x111 ... add r3,r7,r0 / strb r5,[r3]
 *                           ... add r0,#2 / add r3,r7,r0 / strb r6,[r3]
 *     parked                    strb r5,[r7,r2]
 *                           ... add r2,r2,#2 / strb r6,[r7,r2]
 *
 * The ROM MATERIALISES THE ADDRESS and uses an immediate-offset store; the parked
 * body uses Thumb's register-offset form and saves an instruction at each site.
 * docs/agbcc-thumb-regoffset-bug.md's scope note is the relevant text: gcc296
 * emits reg+reg freely and "the offset as a named local" is a recorded lever that
 * EARNS that form.  This park needs the INVERSE of that lever, and the inverse is
 * to name the ADDRESS instead of the offset:
 *
 *     r = g + off;  *r = z;        ->  add r3,r7,r2 / strb r5,[r3]
 *
 * That closed the FIRST site and took the figure from twenty-seven to the figure
 * claimed above, with the instruction count moving by one.  It is the brief's
 * "copy survives when its DESTINATION is a named variable" lever, measured on an
 * address rather than on a value.
 *
 * ==================== WHAT IS STILL OPEN, NARROWED TO ONE SITE ====================
 *
 * The SECOND site -- the last statement of the function -- refuses the same edit.
 * Naming its address changes NOTHING; the store stays `strb r6,[r7,r2]`.  Measured
 * this batch, every one at ref 58 / ours 56 with the instruction count at 50
 * except where noted, so these are flat and not merely equal-figure:
 *
 *   `s = g + off; *s = two;` with a SECOND pointer local .............. 25
 *   the same with the two pointer locals declared in the other order .. 25
 *   `s[0] = two;` instead of `*s = two;` .............................. 25
 *   `ad = (int)g + off; *(unsigned char *)ad = two;` .................. 25
 *   ONE pointer local reused at both sites ............................ 25
 *   `t = off + 2; s = g + t;` (the increment in its own local) ........ 25
 *   `*s = (unsigned char)two;` (narrowing the stored value) ........... 25
 *   `r = off + g` / `s = off + g` (operand order at both sites) ....... 25
 *   all FOUR stores through one named pointer ......................... 27, worse
 *   site TWO named and site ONE left as a subscript ................... 27, inert
 *   the 0x112 store moved AFTER the 0x113 store ....... 25, better aligned, wrong order
 *   both offsets written as literal constants, no `off` local ......... LENGTH EXACT
 *     at ref 58 / ours 58 -- but it relates 0x111 to 0x110 by one and pools 0x113
 *     separately, losing the ROM's mid-function pool and its branch over it.
 *     That last row is the most interesting negative here: the LENGTH is reachable
 *     by dropping the offset local, and the CONSTANT RELATION is reachable by
 *     keeping it, and no spelling tried this batch has both.
 *
 * >> NAMED REMAINING QUESTION: the two sites are spelled identically and differ
 *    only in that the first leaves `off` LIVE (it is incremented afterwards) while
 *    the second is the function's last use of everything.  So the discriminator is
 *    not the spelling of the address -- it is whether the index register survives
 *    the store.  The next thing to read is which pass performs the fold: compare
 *    the two stores in `.13.combine` and in `.19.flow2` and name the one that
 *    rewrites `(set reg (plus base idx)) / (set (mem reg) v)` into
 *    `(set (mem (plus base idx)) v)` for the dead-index site only.
 *
 * ALSO CORRECTED: the previous header's loop-setup diagnosis ("The ROM builds the
 * offset ... and only THEN forms the address") describes a real ORDER difference
 * but it accounts for ZERO of the length gap.  There is a second byte-level
 * difference in that region it did not name: the ROM loads the stored halfword
 * with `ldr` from a word pool entry and we load it with `ldrh` from the same
 * word.  That is the HImode/SImode pool contrast running in the direction the
 * previous header credited to the ROM, but on the LOAD WIDTH rather than on
 * whether the constant pools at all -- our value is HImode at the store, the
 * ROM's is a word.  Both pool; only the load width differs.
 *
 * THE BODY THE FIGURE OF TWENTY-SEVEN WAS MEASURED ON had, in place of the
 * `r = g + off; *r = z;` pair below, the plain subscripts `g[off] = z;` and
 * `g[off] = two;` with no pointer local and no `unsigned char *r` declaration.
 */
extern unsigned char *iwram_3001f2c;
extern void *Func_80a1814(void *g);
extern void Func_80a1870(void *q, int a, int b, int c, int d);
extern void *_CreateUIBox(int a, int b, int c, int d, int e);

void Func_80a3354(void)
{
    unsigned char *g;
    void *q;
    short *p;
    int i;
    int z;
    int two;
    int off;
    unsigned char *r;

    g = iwram_3001f2c;
    q = Func_80a1814(g);
    Func_80a1870(q, 2, 2, 8, 0);
    p = (short *)(g + (0xa5 << 1));
    i = 3;
    do {
        i--;
        *p = 0x1e;
        p--;
    } while (i >= 0);
    z = 0;
    *(int *)(g + 0x28) = z;
    *(int *)(g + 0x24) = z;
    two = 2;
    *(void **)(g + 0x2c) = _CreateUIBox(0, 0x11, 0x1e, 3, two);
    off = 0x111;
    *(int *)(g + 0x20) = z;
    g[0x88 << 1] = z;
    r = g + off;
    *r = z;
    g[0x89 << 1] = 8;
    off += 2;
    r = g + off;
    *r = two;
}
