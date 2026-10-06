/* OvlFunc_969_200b600 -- 0x0200b600.  LANDING, 0 differing encodings of 44.
 *
 * WAS a park at 2 differing encodings of 44 (first differing index 32,
 * ref 4905 ours 8833).  Now EXACT: 96 bytes, 44 encodings and 3 relocations
 * identical, both as one function and as the whole translation unit.
 * PIN COUNT: 0 (tools/shimcount.py).  SHIM-FREE, FLAG-FREE, DEVICE-FREE,
 * BARRIER-FREE.  makefile_flags() is empty, so plain -O2.
 *
 * Verify with: python3 tools/objcmp.py src/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_a_a_a.c asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_c_c_c_c_a_a_a.s --func OvlFunc_969_200b600
 *
 * Pre-install the same line with scratch_elev/b329/F/p3_candidate.c in place of
 * the src path reads OK, and with --whole in place of the function selector it
 * reads "OK whole file".
 *
 * SPLIT: NONE NEEDED.  The reference .s holds ONE function and
 * tools/datacheck.py reports nothing, so no text/data cut and no new export.
 *
 * ===== HOW THE PARK'S "FLOOR" DISSOLVED =====
 *
 * The park's residue was a single transposed adjacent pair in the tail:
 *      rom    ldr r1,=0xfffff800 | ldrh r3,[r6]
 *      ours   ldrh r3,[r6]       | ldr r1,=0xfffff800
 * It had measured TWELVE tail spellings, ELEVEN exactly inert, and had closed
 * the question as a floor on a reload-placement argument.  All twelve varied
 * the SAME dimension -- how the subtraction is written -- against a base that
 * always carried a `do { } while (0)` barrier before the tail.
 *
 * THE BARRIER WAS THE CAUSE.  Read out of .23.sched2 on the parked body:
 *      (insn 110 (set (reg:SI 1 r1) (const_int -2048))
 *          (insn_list:REG_DEP_ANTI 42 (insn_list:REG_DEP_OUTPUT 79 (nil))))
 * 79 sets r3 and 110 sets r1, so that REG_DEP_OUTPUT is not a register
 * conflict.  It is the barrier: the two NOTE_INSN_LOOP_BEG / LOOP_END notes a
 * `do { } while (0)` plants are collected as `loop_notes` and attached to the
 * FIRST insn after them, and sched_analyze_insn then gives that insn an
 * anti-dependence on every prior insn -- insn 79's LOG_LINKS list is 23 entries
 * long, the whole block -- AND sets reg_pending_sets_all, which records it as
 * the last setter of EVERY register.  The next insn to write any register
 * therefore takes an output dependence on it.  That insn was the
 * reload-created constant.  So the barrier is a TOTAL order in BOTH
 * directions, and with it sitting before the tail the ldrh became the wall the
 * constant could never cross.
 *
 * This also answers the half the park reported as not established -- why insn
 * 110 became ready only at t=145.  It was never a readiness accident and never
 * a rank_for_schedule tie: there is a hard dependence edge 79 -> 110.
 *
 * MEASURED, barrier POSITION with everything else held at the parked body:
 *      after a->f40 (the parked body)                2 of 44  (idx 32,33)
 *      removed entirely                              8 of 44  (idx 27-34)
 *      after a reloaded `h = *p`                     6 of 44  (idx 27,28,30-33)
 *      BEFORE a->f40                                 0 of 44  -- EXACT
 *
 * ===== BUT THE BODY SHIPPED IS NOT THAT ONE, AND THIS IS THE REAL LESSON =====
 *
 * OvlFunc_925_200b460 -- named in BOTH this park and OvlFunc_969_200db90's as
 * "a sibling that one solution would also elevate" -- IS NOT A PARK.  It
 * LANDED, in src/overlays/rom_7b0400/ovl_314_c_c_c_c_a.c, and its .s carries
 * gcc's own banner.  Its body is twelve lines with no barrier, no named reload
 * and no pointer local, and porting that shape here lands this function with
 * NOTHING holding the schedule:
 *      a struct FIELD `a->f64` in place of the park's `unsigned short *p`
 *      formed from `(char *)a + 0x64`, the mirror stores written plain, and
 *      `a->f64 -= 0x800` as the tail.
 * So the park's `t`/`w` locals and its barrier were both artifacts of its own
 * raw-pointer spelling, and the shared "floor" was never a floor.
 *
 * tools/dupfuncs.py cannot see this: it scans only the 774 REMAINING
 * functions, so a park whose twin has already LANDED is invisible to it.  Both
 * parks cited 200b460 for two batches and neither noticed it had left the
 * parked set.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    unsigned char pad34[4];
    int f38;
    unsigned char pad3c[4];
    int f40;
    unsigned char pad44[0x64 - 0x44];
    unsigned short f64;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_969_200b600(struct Actor *a)
{
    unsigned short ang;
    struct Actor *t;

    t = __MapActor_GetActor(0x18);
    ang = a->f64;
    a->f8 = t->f8 + __cos(ang) * (a->f30 + 3);
    a->f10 = t->f10 + (__sin(ang) << 1);
    a->f38 = a->f8;
    a->f40 = a->f10;
    a->f64 -= 0x800;
}
