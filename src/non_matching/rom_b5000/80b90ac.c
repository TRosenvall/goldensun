/* Func_80b90ac (0x080b90ac) -- NON-MATCHING.
 *
 * NON-MATCHING, 26 of 34 stream encodings  (RE-MEASURED, batch 333).
 *   STREAM lengths differ, ref 34 against ours 32, and the 16-bit INSTRUCTION
 *   counts differ, ref 29 against ours 28 -- so this positional figure
 *   measures MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 76 bytes, ours 72.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80b90ac.c \
 *     asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_a.s --func Func_80b90ac
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: the ROM copies a buffer address into a second callee-saved
 * register inside a guard, and gcc coalesces the copy away.
 *
 * THIRD instance of this exact wall. The full measurement table lives in
 * src/non_matching/rom_b5000/80c1014.c -- that function has the same
 * `Func_80b6c08(3, buf)` opener, the same guarded countdown loop, and the same
 * residue -- and src/non_matching/ovl_77dd1c/2009498.c states it from the other
 * side. Not re-derived here.
 *
 * 33 lines against the ROM's 34:
 *
 *     rom    mov r5, sp ... (guard) ... mov r7, r5   then ldrh r0, [r6, r7]
 *     ours   mov r7, sp ... no copy ...              then ldrh r0, [r6, r7]
 *
 * Measured: a second local `p = base;` assigned inside the guard -- the shape
 * the ROM's `mov r7, r5` reads as -- is byte-identical to not writing it, 33
 * lines and 25 differing either way. That is the same result 80c1014 got from
 * three spellings including one where the call argument and the loop base share
 * no variable at all.
 *
 * WHAT IS NEW HERE, and worth having: the copy goes to r7, a LOW callee-saved
 * register, where 80c1014's went to r8. So this is not about the high-register
 * bank -- gcc will not emit a redundant address copy into any callee-saved
 * register, whatever the pressure.
 *
 * A SECOND, SEPARATE DIFFERENCE that the base copy hides: the ROM re-reads the
 * halfword BEFORE the byte store (`ldrh / strb`), ours after (`strb / ldrh`).
 * The read-twice spelling is right -- both streams have two `ldrh` -- only the
 * order differs, and it is downstream of the allocation.
 *
 * NEXT: nothing source-level. Three specimens now agree.
 *
 * ---------------------------------------------------------------------------
 * BATCH 333: THE PASS THAT EATS THE COPY IS NAMED, AND IT IS NOT REGISTER
 * ALLOCATION.
 *
 * Aligned against the reference, this function is ONE instruction from done and
 * nothing else is a shape difference.  Of the reference's thirty-four stream
 * encodings, the only differences are the base register's name in three
 * instructions, two branch displacements, the missing copy, and the
 * reference's trailing pad -- the loop body, both calls, the pool word and the
 * whole prologue and epilogue are identical.
 *
 * THIS HEADER AND 80c1014's BOTH BLAME THE REGISTER ALLOCATOR -- "gcc coalesces
 * the copy away", "gcc will not emit a redundant address copy into any
 * callee-saved register".  That is the wrong pass.  The copy is deleted by
 * GCSE's COPY PROPAGATION in pass 07, before local-alloc or global-alloc ever
 * see it.  With a second pointer local assigned in the guard -- the spelling
 * this header records as byte-identical to not writing it -- `cand.c.07.gcse`
 * prints:
 *
 *     SET hash table ... Index 0 (set (reg/v:SI 33) (reg/v:SI 32))
 *     COPY-PROP: Replacing reg 33 in insn 42 with reg 32
 *     COPY-PROP: Replacing reg 33 in insn 59 with reg 32
 *     CPROP of Func_80b90ac, pass 1: 0 const props, 2 copy props
 *
 * and the copy insn is present in `cand.c.03.cse`, absent from `cand.c.09.cse2`.
 * Two copy props, both uses of the second pointer, nothing else.  So the search
 * is a cprop blocker in gcse.c, not a spelling that "asks for" a copy -- which
 * is why three specimens' worth of pointer-splitting spellings all measured
 * inert.  Func_8006088 in this same batch loses its join copy to the same pass,
 * with the same dump line.
 *
 * WHY THE ALLOCATION FOLLOWS FOR FREE ONCE THE COPY SURVIVES.  The reference's
 * two values are a call-argument address in r5 and a loop base in r7.  r7 can
 * only come from global-alloc -- HARD_FRAME_POINTER_REGNUM is r7
 * (arm.h:898-899) and local-alloc.c:1978-1990 reserves it unconditionally -- and
 * the loop base crosses basic blocks, so it qualifies.  Nothing has to ask for
 * r7; the two allocnos just have to exist.
 *
 * MEASURED AND EXACTLY INERT, all twenty-eight instructions and twenty-six
 * differing: the call argument inline with the loop base named inside the guard;
 * the base held as an int with the call taking a cast of it; that plus a second
 * int for the loop; a third pointer name for the loop; the base copy written
 * INSIDE the loop body so loop.c would hoist it after gcse (it does not survive
 * -- gcse runs first and propagates it where it stands); the same as an int; the
 * loop indexing `(char *)base + i` instead of casting the sum.
 * MEASURED MUCH WORSE, and worth knowing: any spelling that names the array
 * itself more than once in the loop (the array inline at both reads, a union
 * member, a struct wrapper, the base folded into the index) collapses to
 * twenty-two or twenty-three instructions -- gcc drops the base register
 * entirely and the prologue loses its high-register save.  Those are not near
 * misses; they are a different program shape.
 *
 * PIECE: this .s holds ONE .thumb_func_start, so it converts ALONE.
 * ---------------------------------------------------------------------------
 */
extern int Func_80b6c08(int a, short *buf);
extern unsigned char *_GetUnit(int id);
extern void _CalcStats(int id);

void Func_80b90ac(void)
{
    short buf[14];
    short *base;
    int count;
    int n;
    int off;
    int z;

    base = buf;
    count = Func_80b6c08(3, base);
    if (count > 0) {
        off = 0;
        z = 0;
        n = count;
        do {
            _GetUnit(*(unsigned short *)(off + (int)base))[0x12b] = z;
            _CalcStats(*(unsigned short *)(off + (int)base));
            n--;
            off += 2;
        } while (n != 0);
    }
}
