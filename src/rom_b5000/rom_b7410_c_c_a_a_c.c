/* Func_80b8000 -- MATCHING, 0 of 45 encodings, 100 bytes, PIN-FREE.
 *
 * PINS: 0.  FLAG GROUP: none.  SPLIT: none -- asm/rom_b5000/rom_b7410_c_c_a_a_c.s
 * holds exactly ONE function (`grep -c thumb_func_start` = 1), so the .c simply
 * replaces it.  tools/datacheck.py on that .s is SILENT (exit 0): no data
 * requirement, and stage1.ld names the object ONCE (line 1589, `.text`).
 * tools/split_s.py is not needed and must not be run.
 *
 * Verify with (INSTALLED path):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_b5000/rom_b7410_c_c_a_a_c.c \
 *     asm/rom_b5000/rom_b7410_c_c_a_a_c.s --func Func_80b8000
 *   -> OK Func_80b8000 -- 100 bytes, 45 encodings and 4 relocations identical
 *
 * ==================================================================
 * THE PARK READ 19 of 45 AT ref 45 / ours 44, AND IT WAS TWO CAUSES.
 * ==================================================================
 *
 * (A) THE ANGLE ADDEND IS NARROWED TO HImode, worth 10 of the 19 and the whole
 *     1-instruction count deficit.  `a->f6 = atan2(...) + (0x80 << 8);` lets
 *     combine narrow the addition to the `short` store's mode, so the addend
 *     becomes a HImode -32768 and gcc POOLS it:
 *         ldr r3,.L4+4 / add r0,r0,r3     with   .word -32768
 *     against the ROM's three-instruction SImode build
 *         mov r3,#0x80 / lsl r3,#8 / add r0,r3
 *     Breaking the expression at a named `int` --
 *         ang = atan2(c->f10 / 8, c->fc);  ang += 0x80 << 8;  a->f6 = ang;
 *     -- keeps the add in SImode, where 0x8000 is mov+lsl constructible and
 *     never reaches the pool.  19 -> 9, count 44 -> 45.  This is the HImode
 *     pool mechanism from the opposite side: the usual symptom is a HImode fix
 *     SORTING ahead of the SImode ones; here the symptom is a pooled
 *     `.word 0xffff8000` where the ROM has no pool word at all.
 *
 * (B) THE +0x5a POINTER MUST GET r3, worth the other 9.  The park's own
 *     account of the register use was right and its conclusion -- "Nothing at
 *     the statement level asks gcc to run out of registers" -- was wrong.
 *
 *     The ROM's program IS the park's REJECTED order (the +0x58 pointer
 *     computed AFTER the +0x5a store), and the park rejected it for the right
 *     observation: it comes out as `add r2,#0x5a / strb / sub r2,#0x2 / strb`,
 *     reproduced here at 43 instructions and 35 differing.  That derivation is
 *     `reload_cse_move2add` (reload1.c:8840), and it FIRES ONLY WHEN BOTH
 *     ADDRESSES LAND IN ONE HARD REGISTER.  So the fix is not to avoid the
 *     ROM's order; it is to break the register coincidence.
 *
 *     Evidence, from `-da` dumps rather than inference.  In the park's order,
 *     .17.lreg says:
 *         Register 37 [the zero]  used 4 times across 8 insns; pref LO_REGS
 *         Register 34 [p5a]       used 2 times across 5 insns; pref STACK_REG
 *     The zero wins qty_compare (local-alloc.c:1496: floor_log2(4)*4 / span),
 *     so it takes r3 -- REG_ALLOC_ORDER (arm.h:989) begins { 3, 2, 1, 0, ... }.
 *     p5a's `pref STACK_REG` means local-alloc's find_free_reg for its
 *     min_class finds only r13 and fails, so p5a is left to global-alloc, whose
 *     .18.greg conflict line then reads `;; 34 conflicts: 32 33 34 37 3 13` --
 *     hard reg 3 BARRED, because the zero holds it -- and global gives p5a r2.
 *     p58 does not conflict with p5a, inherits r2, and move2add derives.
 *
 *     THE EDIT IS ONE LINE MOVED: `z = 0;` hoisted above the three constant
 *     stores.  That lengthens the zero's live range from 8 insns to 22 without
 *     emitting a single extra instruction, which DEMOTES it in qty_compare
 *     below the three short-lived store constants (0x20000, 0x80000, 0xab85,
 *     2 refs over ~4 insns each).  They keep r3; the zero takes r2.  .18.greg
 *     then reads `;; 34 conflicts: 32 33 34 37 2 13` -- hard reg TWO barred
 *     instead of three -- and global gives p5a r3, p58 keeps r2, and with the
 *     two addresses in DIFFERENT hard registers move2add cannot fire.
 *
 *     Measured dispositions, park order -> this file:
 *         zero  r3 -> r2      p5a  r2 -> r3      p58  r2 -> r2
 *
 *     So this is NOT a priority computation to be quoted, it is a RANKING
 *     flip, and the thing that flips it is the length of a live range you
 *     choose by where you put one assignment.
 *
 * CROSSED (tools/crossfire.py, depth 2, base = the ROM-order body at 35):
 *   "z early" is the whole of (B).  Crossed with each of "p5a late (narrow
 *   span)", "f5a as struct field", "f58 as struct field", "byte zero its own
 *   local" and "zero stores as literals" it still reads 0 -- all five are
 *   exactly inert BOTH alone (35, = base) and on top of the fix, so none of
 *   them is load-bearing in either direction.  The two pointers computed up
 *   front -- the park's own best body -- reads 9 at 45 instructions and is
 *   structurally unreachable: it needs THREE scratch registers where the ROM
 *   uses two, so `mov r1,r5` can never become `mov r3,r5`.
 */
struct A {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned char pad08[0x28 - 8];
    int f28;
    unsigned char pad2c[4];
    int f30;
    int f34;
    unsigned char pad38[0x44 - 0x38];
    int f44;
    unsigned char pad48_[0];
    int f48;
    unsigned char pad4c[0x58 - 0x4c];
    unsigned char f58;
    unsigned char pad59[1];
    unsigned char f5a;
};

struct C {
    struct A *f0;
    unsigned char pad04[8];
    int fc;
    int f10;
};

extern struct C *GetBattleActor(void);
extern void _Actor_Stop(struct A *a);
extern void _Actor_TravelTo(struct A *a, int x, int y, int z);
extern int atan2(int y, int x);

void Func_80b8000(void)
{
    struct C *c;
    struct A *a;
    unsigned char *p5a;
    unsigned char *p58;
    int ang;
    int z;

    c = GetBattleActor();
    a = c->f0;
    z = 0;
    a->f34 = 0x80 << 10;
    a->f30 = 0x80 << 12;
    a->f48 = 0xab85;
    p5a = &a->f5a;
    a->f28 = z;
    a->f44 = z;
    *p5a = z;
    p58 = &a->f58;
    *p58 = 1;
    _Actor_Stop(a);
    _Actor_TravelTo(a, c->fc, 0, c->f10);
    ang = atan2(c->f10 / 8, c->fc);
    ang += 0x80 << 8;
    a->f6 = ang;
}
