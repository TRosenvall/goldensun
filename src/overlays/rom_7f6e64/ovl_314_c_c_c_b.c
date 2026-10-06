/* OvlFunc_969_200db90 -- 0x0200db90.  LANDING, 0 differing encodings of 43.
 *
 * WAS a park at 2 differing encodings of 43 (first differing index 31,
 * ref 4905 ours 8833).  Now EXACT: 92 bytes, 43 encodings and 2 relocations
 * identical.  PIN COUNT: 0 (tools/shimcount.py).  SHIM-FREE, FLAG-FREE,
 * DEVICE-FREE, BARRIER-FREE.  makefile_flags() is empty, so plain -O2.
 *
 * Verify with: python3 tools/objcmp.py src/overlays/rom_7f6e64/ovl_314_c_c_c_b.c asm/overlays/rom_7f6e64/ovl_314_c_c_c_b.s --func OvlFunc_969_200db90
 *
 * That is the POST-SPLIT recipe.  Pre-split and pre-install, the same line with
 * scratch_elev/b329/F/p4_candidate.c and asm/overlays/rom_7f6e64/ovl_314_c_c_c.s
 * reads OK.
 *
 * SPLIT: YES, A THREE-WAY SPLIT, and it does NOT need the sibling landed.
 *   tools/datacheck.py on asm/overlays/rom_7f6e64/ovl_314_c_c_c.s reports
 *   ".bss AND .data" and TWO functions (OvlFunc_969_200da28 and this one), and
 *   that neither function reads a data label, so the split needs NO new export.
 *   tools/split_s.py --dry-run on that file with this function says:
 *       ovl_314_c_c_c_a.s   1 function, 173 lines   (OvlFunc_969_200da28)
 *       ovl_314_c_c_c_b.s   1 function,  49 lines   (this function)
 *       ovl_314_c_c_c_c.s   1 function,  79 lines   (the data)
 *   so the sibling stays in asm and does not have to match first.  It is still
 *   parked at 48 differing encodings of 157 in
 *   src/non_matching/ovl_7f6e64/200da28.c.
 *
 * ===== WHY THE PARKED FIGURE WAS NOT A FLOOR =====
 *
 * The residue was a single transposed adjacent pair in the tail:
 *      rom    ldr r1,=0xfffffe00 | ldrh r3,[r6]
 *      ours   ldrh r3,[r6]       | ldr r1,=0xfffffe00
 * The park closed it at 2 on a reload-placement argument and recorded its twin
 * OvlFunc_969_200b600 as an independent confirmation of the same floor.  Both
 * were wrong, and both for the same reason: the parked bodies carried a
 * `do { } while (0)` barrier immediately before that tail.
 *
 * A `do { } while (0)` plants NOTE_INSN_LOOP_BEG and NOTE_INSN_LOOP_END, and
 * sched_analyze collects those as `loop_notes` and hands them to the FIRST insn
 * after them.  sched_analyze_insn then treats it as a scheduling barrier
 * (haifa-sched.c:3714 onward, CONFIRMED IN THE COMPILER SOURCE): at :3744 it
 * gets a REG_DEP_ANTI on every reg_last_uses entry, at :3748 and :3751 a
 * dependence of type 0 -- REG_DEP_TRUE, NOT anti -- on every reg_last_sets and
 * reg_last_clobbers entry, at :3756 flush_pending_lists also clears the memory
 * lists, and at :3754 reg_pending_sets_all is set.  Then :3780-3789 assigns
 * reg_last_sets[i] = this insn FOR EVERY REGISTER i, which is exactly why the
 * next insn to write any register takes a REG_DEP_OUTPUT on it.
 * (Only LOOP_BEG, LOOP_END, the two EH_REGION notes and SETJMP arm this, per
 * the scan at :3727-3731; a RANGE note deliberately does not.)  With the
 * barrier sitting before the tail, the
 * anchor was the ldrh and the reload-created constant was nailed behind it by a
 * hard dependence edge.  Measured on the twin: the barrier one statement
 * earlier reads 0, and removed entirely reads 8.
 *
 * That also settles the half this park reported as NOT established -- why the
 * const insn readied late.  It is a REG_DEP_OUTPUT edge from the ldrh, created
 * by the barrier.  No priority or dependent-count manipulation was ever
 * involved, and the park's correction to its own rank_for_schedule reasoning
 * was right to withdraw it.
 *
 * ===== THE BODY SHIPPED, AND WHERE IT CAME FROM =====
 *
 * OvlFunc_925_200b460 -- which this park has named for several batches as
 * "its twin, differing in ONE constant, so one solution elevates both" -- IS
 * NOT A PARK.  It LANDED, in src/overlays/rom_7b0400/ovl_314_c_c_c_c_a.c, and
 * asm/overlays/rom_7b0400/ovl_314_c_c_c_c_a.s carries gcc's own banner.  Its
 * body is twelve lines with no barrier and no locals beyond two, and the ROM's
 * two functions are instruction-for-instruction equal apart from
 * `mov r2,#0xa4` against `mov r2,#144`.  Porting that body with 0x90 -> 0xa4
 * lands this function exactly, on the first try.
 *
 * tools/dupfuncs.py cannot find this pairing: it compares only the 774
 * REMAINING functions, so a park whose duplicate has already LANDED is
 * structurally invisible to it.  The cheap guard is to grep asm/ for the
 * sibling's symbol and check whether its .s carries the gcc banner.
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
    unsigned short f66;
    struct Actor *f68;
};

extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_969_200db90(struct Actor *a)
{
    unsigned short ang;
    struct Actor *t;

    ang = a->f64;
    t = a->f68;
    a->f8 = t->f8 + __cos(ang) * (a->f30 + 0x1c);
    a->f10 = (__sin(ang) << 4) + (0xa4 << 16);
    a->f38 = a->f8;
    a->f40 = a->f10;
    a->f64 -= 0x200;
}
