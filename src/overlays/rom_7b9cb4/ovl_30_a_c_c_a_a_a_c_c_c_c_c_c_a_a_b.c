/* OvlFunc_932_20086a0 (0x020086a0) -- MATCHING.
 *
 * Turns BG1 on when a scaled random draw clears the threshold halfword at
 * .L5238, by read-modify-writing REG_DISPCNT.  (The reading of the constants
 * is the batch-331 park's and it was right: 0x80 << 19 is 0x04000000, the
 * display-control register, and 0x80 << 2 is DISPCNT_BG1_ON.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_a_a_b.c \
 *     asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_a_a_b.s \
 *     --whole
 *   (before the split, against the unsplit reference:
 *      ... --func OvlFunc_932_20086a0 on
 *      asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_a_a.s
 *    -- OK, 60 bytes, 27 encodings and 2 relocations identical.)
 *
 * PINS: none.  shimcount.py reports nothing.  The `__asm__(".L5238")` name on
 * the extern is the established way to reach a local data label and is not a
 * shim; the label is already `.global` at
 * asm/overlays/rom_7b9cb4/ovl_30_c_c_c.s:161, so this split needs NO export.
 *
 * SPLIT: datacheck.py on the reference is CLEAN (exit 0, no output).
 * split_s.py --dry-run cuts the .s TWO ways:
 *     ..._a_a_b.s   1 function,  34 lines   (this one)
 *     ..._a_a_c.s   1 function, 119 lines   (OvlFunc_932_20086dc, parked at 42)
 *     overlays/rom_7b9cb4/overlay.ld rewritten
 * The mate stays parked at its own figure; a merge could never have changed it.
 * ITS PARK RECIPE MUST BE REPOINTED: src/non_matching/ovl_7b9cb4/20086dc.c:12
 * names the unsplit .s, which this split consumes.
 *
 * ===================== BATCH 337 BRIEF D: HOW IT CLOSED =====================
 *
 * THE PARK'S FIGURE WAS 10 of 27 WITH THE STREAMS UNEQUAL (instructions 24 ref
 * / 22 ours, size 60 / 56), so that 10 measured MISALIGNMENT and the real gap
 * was TWO instructions: the ROM narrows the value before the halfword store
 * and gcc does not --
 *
 *      rom   lsl r3, r5, #16 / lsr r3, #16 / strh r3, [r6]
 *      ours                                  strh r5, [r6]
 *
 * THE ANSWER WAS THE PRODUCT OF TWO OF THE PARK'S OWN NEGATIVES.  The park had
 * eleven measured spellings, every one byte-identical to its body, and two of
 * them were:
 *      "a volatile unsigned-halfword pointer, plain/cast store"   inert
 *      "`m = (unsigned short)s; *p = m;` through an int temp"     inert
 * Both reproduce as inert (10 of 27, re-measured).  CROSSED THEY ARE EXACT.
 *
 * WHY BOTH HALVES ARE LOAD-BEARING, read off the pass dumps:
 *   * Without the 32-bit temp, expand never creates a zero-extension insn --
 *     the conversion is the destination's HImode subreg of the value, so there
 *     is nothing for a later pass to keep.  The park's "the conversion never
 *     becomes an insn at all" paragraph is correct, and is about THIS half.
 *   * With the temp, `.00.rtl` holds the pair (`ashift:SI` 16, `lshiftrt:SI`
 *     16, carrying `REG_EQUAL (zero_extend:SI (subreg:HI ...))`) and a separate
 *     `(set (mem:HI ...) (subreg:HI ...))`.  The volatile and non-volatile
 *     bodies' `.00.rtl` are IDENTICAL except `mem:HI` vs `mem/v:HI`.
 *   * `.13.combine` is where they part.  Non-volatile: both shift insns become
 *     `NOTE_INSN_DELETED` and the store's source is substituted to
 *     `(subreg:HI (reg 35) 0)`.  Volatile: both survive combine untouched.
 *
 * A VOLATILE MEM OPERAND IS A DEVICE-FREE FENCE AGAINST COMBINE, AND THIS IS
 * GENERAL.  combine_instructions calls `init_recog_no_volatile ()` at
 * combine.c:545 and only restores volatile-MEM recognition at combine.c:783.
 * init_recog_no_volatile clears `volatile_ok` (recog.c:88-96), and
 * general_operand refuses a volatile MEM while that flag is clear
 * (recog.c:954).  So any pattern combine builds that needs a volatile MEM as a
 * general_operand fails recog_for_combine and the combination is abandoned --
 * while the very same insn was recognised at expand, where volatile_ok is 1.
 * Nothing between combine.c:545 and :783 tests MEM_VOLATILE_P itself; the
 * refusal lives entirely in recog.  Worth generalising: GBA code writes
 * hardware registers constantly, so EVERY volatile store in this tree is
 * already a combine fence -- all this park lacked was a redundant computation
 * for that fence to protect.
 *
 * THE ATTRIBUTION LADDER, measured (encodings differing, objcmp):
 *      park body (non-volatile ptr, cast at the store)                10
 *      int temp, non-volatile ptr                                     10
 *      `s & 0xffff` into an int temp, non-volatile ptr                10
 *      bare `__asm__ volatile ("")` between temp and store            10
 *      clobber barrier BEFORE the extension                           10
 *      `unsigned short` local assigned in both arms                   10
 *      `short` local assigned in both arms                            10
 *      `(unsigned short)(unsigned)s` chain                            10
 *      `(unsigned)s << 16; >>= 16` pair                               10
 *      volatile ptr, cast at the store, NO temp                       10
 *      volatile ptr + temp but no conversion (`z = s`)                10
 *      volatile ptr + `unsigned short` temp                            6
 *      volatile ptr + int temp + `s & 0xffff`                          4
 *      volatile ptr + int temp + `(s << 16 >> 16) & 0xffff`            4
 *      volatile ptr + `int` temp + `(unsigned short)` cast             0  <-- this
 *      volatile ptr + `unsigned int` temp + cast                       0
 *      the same through vu16 + DISPCNT_BG1_ON from gba/io.h            0  <-- shipped
 *      `::: "memory"` barrier between temp and store (non-volatile)    0  (instrument)
 *      `:: "i"(0)` barrier between temp and store (non-volatile)       0  (instrument)
 * The last two are the labelled instruments that confirm the actor is combine
 * and nothing later.  They are NOT shipped: the volatile destination reaches
 * the same place with no device, and it is what the hardware register is.
 *
 * A REACHABILITY CENSUS, which explains why eleven spellings failed.  Scanning
 * every `.s` in asm/ for `lsl rX,rY,#16 / lsr rX,#16 / strh`: ZERO hits in the
 * 4,527 compiler-generated files and ONE in the 704 hand-written ones -- and
 * that one hit is this function.  The window is unique in the ROM, so no amount
 * of corpus-led spelling search could have labelled it; it needed the mechanism.
 *
 * NOT A FLAG MATCH: it is byte-identical under the production -O2 flags.
 * The address stays spelled `(vu16 *)(0x80 << 19)` rather than REG_DISPCNT,
 * because the ROM builds 0x04000000 as `mov r6,#0x80 / lsl r6,#19` and the
 * macro's bare constant pools.
 */
#include "gba/types.h"
#include "gba/io.h"

extern unsigned char L5238[] __asm__(".L5238");
extern int __Random(void);

void OvlFunc_932_20086a0(void)
{
    vu16 *p;
    int v;
    int t;
    int s;
    int r;
    unsigned int x;
    int lim;
    int z;

    p = (vu16 *)(0x80 << 19);
    v = *p;
    t = ~DISPCNT_BG1_ON & 0xffff;
    t &= v;
    s = (short)(t << 16 >> 16);
    r = __Random();
    x = 0x64;
    x *= r;
    lim = *(unsigned short *)L5238;
    x >>= 16;
    if (x >= (unsigned int)lim) {
        int m = DISPCNT_BG1_ON;
        s |= m;
    }
    z = (unsigned short)s;
    *p = z;
}
