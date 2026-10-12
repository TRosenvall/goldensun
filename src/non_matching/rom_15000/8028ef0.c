/* Func_8028ef0 -- 0x08028ef0  (asm/rom_15000/rom_23178_a_c_a.s)
 *
 * NON-MATCHING, 20 of 73 encodings.  RE-MEASURED batch 342, brief B: unmoved.
 *   THE FIGURE IS A DISTANCE -- 73 stream slots against 73, 168 bytes against
 *   168, relocations identical.  BUT 73 IS THE STREAM COUNT AND THE
 *   INSTRUCTIONS ARE 62/62; the line below used to read "73 real slots", which
 *   is the eighth park found quoting a stream count on the instruction line
 *   (reports/batch-341.md, "Open").
 *   PINS: 0.  Device-free.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/8028ef0.c \
 *     asm/rom_15000/rom_23178_a_c_a.s --func Func_8028ef0
 *
 * SPLIT SHAPE: none.  asm/rom_15000/rom_23178_a_c_a.s holds exactly one
 * .thumb_func_start (Func_8028ef0), so this would convert WHOLE.
 *
 * AUTOSWEPT AT FULL DEPTH, batch 342 (tools/autosweep.py --max 200).  FLAG
 * AXIS: -fno-gcse, -ffixed-r7, -fno-strict-aliasing, -fno-strength-reduce,
 * -fno-rerun-cse-after-loop and -fno-rerun-loop-opt all inert at 20; -O1 and
 * -fno-schedule-insns2 both 30.  ANCHOR AXIS: all 10 of 10 statement
 * boundaries inert.  BOTH CHEAP AXES ARE CLOSED.
 *
 * === BATCH 342: THE TWENTY IS ONE PHASE, NOT FIVE INDEPENDENT CHOICES ===
 *
 * This is the one claim in the record below that does not survive, and it is
 * the claim that made the park look unreachable.  Every scratch site disagrees
 * IN THE SAME DIRECTION: the ROM holds whichever of r2/r3 we do not, at every
 * site, in alternation.  Read off the full stream diff (not the keyhole
 * tools/tryc.py prints):
 *
 *   idx 10,11   `mov r3,#0 / ldrsh r1,[r2,r3]`      IDENTICAL on both sides
 *   idx 16,19   the 0x99b pool constant               rom r3 / ours r2
 *   idx 21,22   0xe materialised for `str [sp]`       rom r2 / ours r3
 *   idx 23..27  argument-setup order                  CONSEQUENCE of the above
 *   idx 29,31   the second ldrsh's base/offset pair   rom r2,r3 / ours r3,r2
 *   idx 32,33   r8 copied back to a low reg           rom r3 / ours r2
 *   idx 34,35   argument-setup order                  CONSEQUENCE
 *   idx 38,39   the .L37428 pool address into r8      rom r2 / ours r3
 *   idx 45,46   the 0xa07 pool constant               rom r3 / ours r2
 *
 * THAT IDENTICAL FIRST ROW IS THE NEW INFORMATION.  The two
 * `*thumb_extendhisi2_insn` insns are the function's only two-reload insns
 * (greg: "Spilling for insn 21 ... reg 3 for reload 1 / reg 2 for reload 0"
 * and the same pair for insn 54).  The ROM gives BOTH the same assignment --
 * base r2, offset r3 -- and so does our FIRST one; only our SECOND is
 * reversed.  So the divergence is not present at the first scratch site in the
 * function; it is INTRODUCED somewhere in the window idx 16..27, and every
 * later site simply inherits it.  That bounds the search to three reloads.
 *
 * WHY ONE WINDOW CAN FLIP ALL EIGHT SITES.  `allocate_reload_reg` does not
 * have a per-site preference at all; it takes the next register off a
 * ROUND-ROBIN CURSOR:
 *
 *     i = last_spill_reg;                      reload1.c:5003
 *     for (count = 0; count < n_spills; count++) { i++; if (i >= n_spills) i -= n_spills; ... }
 *     ...  last_spill_reg = i;                 reload1.c:4937
 *
 * `last_spill_reg` is reset to -1 exactly once per function (reload1.c:821),
 * and `spill_regs` is filled in ASCENDING HARD-REGISTER ORDER over
 * `used_spill_regs` (reload1.c:3527-3534).  Here `used_spill_regs` is {r2, r3},
 * so n_spills is 2 and the cursor has PERIOD TWO: one extra or one fewer
 * reload allocated anywhere earlier in the function inverts every later site
 * at once.  The park's decomposition into "FIVE INDEPENDENT RELOAD-SCRATCH
 * CHOICES" is therefore wrong in the direction that matters -- there is ONE
 * free variable, and the park's own conclusion that nine of the twenty close
 * for free is the understatement: ALL TWENTY do.
 *
 * SO THE NEXT QUESTION IS NARROW: what makes our build allocate one FEWER
 * reload than the ROM's between the two `ldrsh` insns?  (Not "which register
 * does site N want", which is what every spelling sweep including this batch's
 * has been asking.)  The cursor is advanced by `set_reload_reg`, so a reload
 * whose insn is later deleted or inherited still moves it -- the extra reload
 * need not be a visible instruction, which is consistent with the two sides
 * having identical 62-instruction streams.
 *
 * MEASURED IN BATCH 342 AND INERT AT 20 (byte-identical):
 *   .L37428 named in a local assigned just before its first use
 *   0xe named in a local assigned between the two Func_801e9a0 calls
 *   `w` copied into a local used by every call
 *   0x99b named in a local assigned before the first call
 * WORSE, and each is a DIFFERENT PROGRAM (dirty relocations):
 *   the +0x99b deferred to the final DrawSmallText argument            44
 *   `v + 0xa07` precomputed into a local before the calls      74, size +8
 *   `v + 0xa07` precomputed between the two Func_801e9a0 calls         24
 *   .L37428 named in a local assigned at the top                74, size +8
 *   `*p` read once into a local                                56, size -4
 *   `id` sign-extended in place, no `v`                        73, size -12
 *
 * --- THE BATCH-323 RECORD FOLLOWS; ITS SITE TABLE IS SOUND, ITS
 * --- "FIVE INDEPENDENT CHOICES" FRAMING IS SUPERSEDED ABOVE ---
 *
 * The `-da` dump b.c.26.mach confirms the pool order is already correct, so
 * there is NO pool-order component to this residue.
 *
 *   #  indices        what
 *   1  16, 19         scratch for the 0x99b pool constant feeding `add sl, rX`
 *                     rom r3 / ours r2
 *   2  21, 22         scratch materialising 0xe into r8   rom r2 / ours r3
 *   3  23, 24, 25,    sched2 order of the first Func_801e9a0 argument setup
 *      26, 27         -- A CONSEQUENCE of #2
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
 * The same inversion explains 34/35.
 *
 * THE DECIDING PASS IS RELOAD, read out of the dumps:
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
 * .18.greg also confirms `;; 0 regs to allocate:` and `;; Hard regs used:
 * 0 1 2 3 5 6 8 9 10 13 14 25 26` with NO pseudo assigned r2 or r3, so these
 * are not allocator quantities -- the batch-267 correction stands.
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
