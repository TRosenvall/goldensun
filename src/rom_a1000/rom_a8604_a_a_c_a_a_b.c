/* Func_80a8b10 -- LANDS.  124 bytes, 60 encodings and 1 relocation IDENTICAL.
 *
 * CURRENT FIGURE, in stream terms: 0 of 60.  ref 58 16-bit instructions and
 * ours 58; SIZE 124 both; 2/2 32-bit entries; 1/1 relocations.  Pin-free and
 * device-free (tools/shimcount.py reports nothing).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_a1000/rom_a8604_a_a_c_a_a_b.c \
 *     asm/rom_a1000/rom_a8604_a_a_c_a_a_b.s --whole
 *   (BEFORE the split, against the undivided reference:
 *    python3 tools/objcmp.py scratch_elev/b344/C/p3_candidate.c \
 *      asm/rom_a1000/rom_a8604_a_a_c_a_a.s --func Func_80a8b10)
 *
 * SPLIT SHAPE.  tools/datacheck.py on asm/rom_a1000/rom_a8604_a_a_c_a_a.s is
 * SILENT (no interior data, so this is a plain text split, exports []).
 * tools/split_s.py --dry-run:
 *   would write asm/rom_a1000/rom_a8604_a_a_c_a_a_a.s  (1 function, 220 lines)
 *   would write asm/rom_a1000/rom_a8604_a_a_c_a_a_b.s  (1 function,  83 lines)
 *   would REMOVE asm/rom_a1000/rom_a8604_a_a_c_a_a.s, rewrite stage1.ld
 * The _a part keeps Func_80a8914, which stays PARKED at 185 of 219
 * (src/non_matching/rom_a1000/80a8914.c).  Its park's `Verify with:` recipe
 * names the pre-split .s and must be repointed to ..._a_a.s --
 * install_batch.py's repoint_orphaned_recipes does this.
 *
 * ========== WHAT THE PARK CLAIMED, AND WHAT SURVIVED =====================
 *
 * The park's residue reading is EXACTLY RIGHT and its verdict is wrong.
 * Reading, reproduced here and confirmed:
 *
 *     rom    add r3, r5, #4 / mov r12, r5 / L0: ... cmp r3, r12 / bge L0
 *     ours   add r3, r5, #4 /               L0: ... cmp r3, r5  / bge L0
 *
 * Verdict, refuted: "a copy of an UNCHANGING value is unreachable ... any local
 * initialised from `out` is provably the same value, so gcc coalesces it and
 * emits no `mov`.  A pure duplicate of a live value is an allocator artifact,
 * not a source construct."  That conclusion is also written into
 * docs/elevation.md under "The copy-then-modify tell is ONE-DIRECTIONAL, and it
 * has a boundary", citing this very function.  **IT NEEDS CORRECTING.**
 *
 * THE COPY IS NOT A SECOND SOURCE NAME.  IT IS loop.c's BIV ELIMINATION.
 * The ROM's loop is written on an INDEX, not on a pointer:
 *
 *     for (i = 4; i >= 0; i--) out[i] = 0;
 *
 * `strength_reduce` replaces the biv `i` with the address giv, so the exit test
 * `i >= 0` becomes a comparison of the cursor against the giv's value at i == 0
 * -- and that value is materialised by an insn emitted in the loop PREHEADER,
 * a pseudo distinct from `out` because loop.c creates it after cse has run.
 * local-alloc then gives it r12: REG_ALLOC_ORDER (arm.h:989) is
 * {3,2,1,0,12,14,4,...}, so a value that does not cross a call takes r12 as
 * soon as r0-r3 are busy -- which is exactly the readout docs/elevation.md
 * already records for `mov r12, rN`.  Nothing needs to be "named" at all; the
 * park was looking for a source construct where the answer was a loop form.
 *
 * Writing the clear loop on a POINTER (the park's body) gives gcc a cursor it
 * can compare against `out` itself, which is the one instruction cheaper form
 * and is what left us short.
 *
 * SAME CAUSE as batch 334's Func_801965c, whose missing instruction was also a
 * `mov r12,r5` and whose two "independent" causes turned out to be one.
 *
 * FIVE SPELLINGS LAND, so this is not a knife-edge:
 *   for (i = 4; i >= 0; i--) out[i] = 0;                          0  <-- shipped
 *   i = 4; do { out[i] = 0;    i--; } while (i >= 0);             0
 *   i = 4; do { out[i] = zero; i--; } while (i >= 0);             0  (`zero` local)
 *   same with `zero` declared last                                0
 *   i = 4; do { *(out + i) = zero; i--; } while (i >= 0);         0
 * The park's `zero` local, which it credited with fixing the head ordering, is
 * NOT needed once the loop is an index loop -- the plain literal lands.
 *
 * MEASURED AND STILL ONE SHORT (the pointer-loop family):
 *   the park's body, unchanged                     51 of 60, 58/57 instructions
 *   the same with `lim` deleted and `out` compared 51 of 60, 58/57 instructions
 *   (bit-identical, confirming the park's own note that `lim` is coalesced)
 *
 * GAP ARITHMETIC, per region (the loop is the clear loop):
 *   park body   REF pre-loop 8  in-loop 4  post-loop 46  = 58
 *               OURS pre-loop 7 in-loop 4  post-loop 46  = 57   -1, ALL PRE-LOOP
 *   this body   OURS pre-loop 8 in-loop 4  post-loop 46  = 58   0
 * The missing instruction was in the PREHEADER, not in the loop -- which is
 * itself the tell for a loop.c-manufactured value.
 */
extern char *_GetUnit(int id);

int Func_80a8b10(char *out, int flag, int unit)
{
    char *u;
    int count;
    int v;
    int i;

    u = _GetUnit(unit);
    for (i = 4; i >= 0; i--)
        out[i] = 0;
    count = 0;
    if (*(short *)(u + 0x38) == 0 && flag == 1) {
        out[0] = flag;
        count = 1;
    }
    v = *(signed char *)(u + 0x131);
    if (v != 0) {
        if (v == 1)
            out[1] = v;
        else
            out[2] = 1;
        count++;
    }
    v = *(signed char *)(u + 0x98 * 2);
    if (v != 0) {
        out[3] = 1;
        count++;
    }
    if (*(unsigned char *)(u + 0xa0 * 2) != 0) {
        out[4] = 1;
        count++;
    }
    return count;
}
