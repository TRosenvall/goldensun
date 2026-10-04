/* Func_8095fcc (0x08095fcc) -- EXACT.  batch 321, brief A, target 4.
 *
 * ref: asm/rom_8a000/rom_944ec_a_c_a_c_c_a_a.s
 * install: src/rom_8a000/rom_944ec_a_c_a_c_c_a_a.c
 *
 * objcmp: OK -- 124 bytes, 56 encodings and 4 relocations identical, --func AND
 * --whole.  PINS 0.  NO shims: no register pins, no inline asm, no barriers, no
 * volatile, no .equ, default flags.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_8a000/rom_944ec_a_c_a_c_c_a_a.c \
 *     asm/rom_8a000/rom_944ec_a_c_a_c_c_a_a.s --func Func_8095fcc
 *
 * SPLIT SHAPE: NONE.  `grep -c func_start` = 1, datacheck.py prints nothing:
 * WHOLE-FILE conversion.  No exports, no linker change.
 *
 * WHAT WAS THE RESIDUE, AND WHAT CLOSED IT -------------------------------------
 *
 * Parked at 3 of 56 (src/non_matching/rom_8a000/8095fcc.c) with "NEXT: nothing
 * source-level outstanding", on the strength of four spellings all measuring 3.
 * The residue:
 *
 *     rom   ldrh r1,[r3] / sub r1,#1 / strh r1,[r3] / mov r6,r0
 *     ours  ldrh r1,[r3] / mov r6,r0 / sub r1,#1 / strh r1,[r3]
 *
 * `mov r6,r0` is the MapActor_GetActor result copy.  The park is right that this
 * is sched2 and that statement order cannot reach it.  IT IS REACHABLE ANYWAY,
 * and not through the copy -- through its RIVAL.
 *
 * sched2 ranks the ready list by PRIORITY, the longest path to the end of the
 * block.  The copy and the `sub r1,#1` chain are the two candidates and the copy
 * wins.  Making the halfword access ALIAS SET 0 (a union member access) gives the
 * `strh` three new successors -- the `ldr [r6,#8]`, `[r6,#0x10]` and `[r6,#0xc]`
 * loads it now conflicts with -- which RAISES the ldrh/sub/strh chain's priority
 * above the copy's.  The copy then loses and lands where the ROM has it.
 *
 * SO THE UNION IS NOT ONLY AN ANTI-DEPENDENCE DEVICE.  Field_Whirlwind's landed
 * lever 5 uses alias set 0 to stop a STORE sinking past a load, which needs a
 * dependence between two memory operands.  THE RESIDUE HERE IS A REGISTER COPY
 * and no alias set can depend on it.  It still closes, because the same edge that
 * would have been an anti-dependence also lengthens the rival's path.  That is a
 * second, distinct way for this lever to pay and it widens the class: it applies
 * wherever sched2 picked the wrong one of two READY insns, not only where it
 * crossed two memory accesses.
 *
 * THE CROSSED TABLE (tools/crossfire.py, 8 edits, depth 2).  TWO independent
 * single-edit zeroes, one at each end of the same conflict:
 *
 *     0   union on the e+0x64 halfword decrement         <= this file
 *     0   union on the `a + 8` word load
 *     0   both, and either one crossed with any inert row
 *     3   BASE
 *     3   union on the e+0x66 read          exactly inert
 *     3   union on the a+0x10 load          exactly inert
 *     3   union on the a+0xc  load          exactly inert
 *     3   union on the e+8 store            exactly inert
 *     3   `p = v` hoisted above the decrement   exactly inert
 *    18   `unsigned short *ph`   (57 insns, COUNT MEM -- misalignment)
 *    37   that crossed with the union        (RELOC MEM)
 *
 * READ THE INERT ROWS: the union on e+0x66 -- the OTHER halfword read of the
 * SAME object -- is exactly inert, while the one on e+0x64 is exact.  e+0x64 is
 * the access in the contested window; e+0x66 is read later and raising its
 * priority changes nothing.  The lever has to sit on the access whose CHAIN must
 * outrank the rival, which is what makes it aimable instead of lucky.
 *
 * The cast-at-the-dereference spelling, keeping `short *ph` and writing
 * `((union blob *)ph)->hh = ((union blob *)ph)->hh - 1;`, is ALSO exact
 * (scratch_elev/b321/A/p4_v_cast.c).  The union-pointer declaration below is
 * shipped because it reads as code rather than as a shim, and because it matches
 * Field_Whirlwind's idiom in the same subsystem.
 *
 * EVERYTHING THE PARK CREDITED IS KEPT, including the one lever that was worth
 * 53 differing: THE gState OFFSET IS BUILT, NOT FOLDED.  `off = 0xfa << 1` first,
 * then `gState + off`, gives the ROM's `mov r1,#0xfa / lsl r1,#1 / add r3,r1`;
 * folding it (`gState + (0xfa << 1)`) pools `gState+500` as one symbol and costs
 * three instructions.  SEE THE HEADER NOTE BELOW -- this is why a typed struct
 * member is the WRONG declaration for this access.
 *
 * ON THE BATCH BRIEF'S DECLARATION LEVER, MEASURED HERE AND CONTRAINDICATED FOR
 * ONE OF THE TWO POINTERS IN THIS FUNCTION:
 *   - `e` walks a real, already-declared struct: src/rom_8a000/rom_93304_a_c_a_c.c
 *     declares `struct Actor` with f08/f0c/f10 at 8/0xc/0x10 and f64/f66 as u16 at
 *     0x64/0x66 -- exactly this function's offsets.  Adopting it is available and
 *     is better code; it is NOT adopted here only because the union on e+0x64 is
 *     the load-bearing construct and a `u16` struct member cannot carry alias
 *     set 0.  A `union blob` member inside a declared `struct Actor` would be the
 *     principled version and is left for whoever types that struct properly.
 *   - `gState` must NOT become a typed struct member access.  The ROM BUILDS the
 *     offset 500 at run time; a struct member at offset 500 is a compile-time
 *     constant offset, which gcc folds into one pooled `gState+500` word -- the
 *     exact 51-line regression the park already measured.  So pattern 4 is wrong
 *     for this access, with evidence, and that is a bound worth recording.
 */
union blob { unsigned char *pp; short hh; int ii; };
extern unsigned char gState[];
extern unsigned char *MapActor_GetActor(int slot);
extern void vec3_translate(int a, int b, int *v);
extern void _DeleteActor(unsigned char *e);

void Func_8095fcc(unsigned char *e)
{
    unsigned char *a;
    union blob *ph;
    int v[3];
    int *p;
    int x;
    int off;

    off = 0xfa << 1;
    a = MapActor_GetActor(*(int *)(gState + off));
    ph = (union blob *)(e + 0x64);
    ph->hh = ph->hh - 1;
    p = v;
    p[0] = *(int *)(a + 8);
    p[2] = *(int *)(a + 0x10);
    vec3_translate(ph->hh * 0x6666, (ph->hh << 11) + *(short *)(e + 0x66), p);
    *(int *)(e + 8) = p[0];
    *(int *)(e + 0x10) = p[2];
    x = *(int *)(e + 0xc) + 0xffff0000;
    *(int *)(e + 0xc) = x;
    if (x < *(int *)(a + 0xc) + (0xa0 << 13))
        _DeleteActor(e);
}
