/* Func_8028ef0 -- 0x08028ef0  (asm/rom_15000/rom_23178_a_c_a.s)
 *
 * NON-MATCHING, 20 of 73 encodings.  MEASURED in batch 323, brief I.
 * NOT IMPROVED.  The body below is the existing park's body, unchanged; what
 * batch 323 adds is the DECOMPOSITION and the deciding pass.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8028ef0.c \
 *     asm/rom_15000/rom_23178_a_c_a.s --func Func_8028ef0
 *
 * SPLIT SHAPE: none.  asm/rom_15000/rom_23178_a_c_a.s holds exactly one
 * .thumb_func_start (Func_8028ef0), so this would convert WHOLE.
 * PINS: 0.
 *
 * THE FIGURE IS A DISTANCE, not a misalignment.  73 real slots against 73,
 * 168 bytes against 168, and all three pool words (0x99b, the L37428
 * relocation placeholder, 0xa07) are identical and in the ROM's order.  The
 * `-da` dump b.c.26.mach confirms the pool order is already correct, so there
 * is NO pool-order component to this residue.
 *
 * THE 20 IS FIVE INDEPENDENT RELOAD-SCRATCH CHOICES PLUS TWO SCHED2
 * CONSEQUENCES -- it is not one exchange, and it is not an allocator tie.
 *
 *   #  indices        what
 *   1  16, 19         scratch for the 0x99b pool constant feeding `add sl, rX`
 *                     rom r3 / ours r2
 *   2  21, 22         scratch materialising 0xe into r8   rom r2 / ours r3
 *   3  23, 24, 25,    sched2 order of the first Func_801e9a0 argument setup
 *      26, 27         -- A CONSEQUENCE of #2, see below
 *   4  29, 30, 31     the second `ldrsh rd, [rb, ro]`: base and offset-scratch
 *                     exchanged.  rom base r2 + scratch r3 / ours the reverse
 *   5  32, 33         scratch moving r8 back to a low reg for `str [sp]`
 *                     rom r3 / ours r2
 *   6  34, 35         sched2 order -- A CONSEQUENCE of #5
 *   7  38, 39         scratch for the L37428 pool address into r8
 *                     rom r2 / ours r3
 *   8  45, 46         scratch for the 0xa07 pool constant  rom r3 / ours r2
 *                                                            total  20
 *
 * WHY 3 AND 6 ARE CONSEQUENCES AND NOT A SCHEDULING CAUSE.  At idx 21-27 the
 * ROM holds 0xe in r2, so `movs r3, #0` (the fourth argument) has no dependence
 * on it and sched2 may place it early; we hold 0xe in r3, so `movs r3, #0` must
 * wait for `mov r8, r3`, and `adds r2, r6, #0` is hoisted into the gap instead.
 * The same inversion explains 34/35.  Nine of the twenty indices therefore
 * close for free the moment the scratch choices do, and there is nothing to
 * attack with a scheduling lever.
 *
 * THE DECIDING PASS IS RELOAD, and this is now read out of the dumps rather
 * than inferred.  The batch-267 correction already said these are not
 * allocator quantities; .18.greg confirms `;; 0 regs to allocate:` and
 * `;; Hard regs used: 0 1 2 3 5 6 8 9 10 13 14 25 26` with NO pseudo assigned
 * r2 or r3.  The sharper fact:
 *
 *   * The 0xe is pseudo 43, and .18.greg assigns it HARD REG 8.  Thumb cannot
 *     `mov r8, #14`, so reload inserts a LO_REGS scratch.  .19.flow2 insn 128
 *     -- a reload-generated insn, numbered above the original stream --
 *     is `(set (reg:SI 3 r3) (const_int 14))` feeding
 *     `(set (reg:SI 8 r8) (reg:SI 3 r3))` with `REG_EQUIV (const_int 14)`.
 *   * The 0x99b is .19.flow2 insn 125, also reload-generated:
 *     `(set (reg:SI 2 r2) (const_int 2459))`.
 *   * .18.greg's own log is the authority on the pairings:
 *         Spilling for insn 21.  Using reg 3 for reload 1 / reg 2 for reload 0
 *         Spilling for insn 54.  Using reg 3 for reload 1 / reg 2 for reload 0
 *         Spilling for insns 32, 39, 40, 57, 69, 83 -- reg 3 only
 *     Insns 21 and 54 are both `*thumb_extendhisi2_insn`, a parallel carrying
 *     `(clobber (scratch:SI))` because thumb `ldrsh` only has the
 *     register-offset form.  They are the ONLY two insns taking two reloads,
 *     and they are exactly cause #4's pair.
 *
 * So the lever is not a spelling and not an allocator priority: it is WHICH
 * RELOAD INDEX a given operand becomes within one insn, and
 * allocate_reload_reg then hands r2 to reload 0 and r3 to reload 1 off the
 * spill list.  The park's standing instruction -- "STOP SWEEPING SPELLINGS ON
 * THIS CLASS" -- is confirmed, and the next reader should start at
 * reload1.c's allocate_reload_reg / choose_reload_regs and at how
 * reload_order is built, with insns 21 and 54 of this function as the
 * two-reload specimen.
 *
 * The park's own MEASURED AND INERT / WORSE lists are retained below and were
 * not re-run; nothing in this brief contradicts them.
 *
 * MEASURED AND INERT, all 20:
 *   0xe as three bare literals, or named in a local assigned after the first
 *     call (the two spellings that keep the size exact)
 *   `short v` parameter, `short v` local, `(s16)id` cast into an int local
 *   `void *w` against `int w`
 *   `name = f(...); name += 0x99b;` and `name = 0x99b + f(...);`
 *   -fno-schedule-insns
 * MEASURED AND WORSE:
 *   0xe named and assigned BEFORE the first call    172 bytes, 75 instructions
 *   -fno-schedule-insns2                            30 differing
 *
 * -fno-schedule-insns2 being worse says the ROM was built with sched2 ON.
 */
#include "gba/types.h"

extern unsigned char L37428[] __asm__(".L37428");
extern int _GetLocationName(int a, int b);
extern void Func_8016478(void *w);
extern void Func_801e9a0(int a, int b, void *w, int c, int d);
extern void Func_801e858(void *s, void *w, int x, int y);
extern void DrawSmallText(int id, void *w, int x, int y);

void Func_8028ef0(void *w, int id, s16 *p)
{
    int v;
    int name;

    v = (s16)id;
    name = _GetLocationName(v, *p) + 0x99b;
    Func_8016478(w);
    Func_801e9a0(v, 3, w, 0, 0xe);
    Func_801e9a0(*p, 3, w, 0x52, 0xe);
    Func_801e858(L37428, w, 0x4a, 0);
    DrawSmallText(v + 0xa07, w, 0, 0);
    Func_801e858(L37428, w, 0x4a, 0xe);
    DrawSmallText(name, w, 0x52, 0);
}
