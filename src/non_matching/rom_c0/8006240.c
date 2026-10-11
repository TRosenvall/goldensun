/* Func_8006240 (0x08006240) -- NON-MATCHING.
 * NON-MATCHING: 3 encodings of 130 differ (objcmp), ours 130 -- lengths,
 * SIZE and RELOCATIONS now all agree, so this IS a distance.
 * Blocker class: instruction ORDER inside the f14 == -1 arm, one hunk of three
 * encodings, with the register assignment following from it.
 *
 * Batch 335 took this from a first draft reading one hundred and thirty-four
 * (on streams of 130 against 135, so not a distance at all) down to three.
 * FOUR LEVERS, and the two the earlier draft named were both real; the two it
 * did not name are the ones that mattered most.
 *
 * (1) THE 8-BYTE LOCAL IS LOADED AS ONE 64-BIT MOVE FROM REG_SIOMLT_RECV.
 *     This is the whole prologue, and it is also what the earlier draft's
 *     "the frame address must be a pseudo from the first store" was really
 *     seeing.  The reference does
 *         ldr r4, [r3, #4] / ldr r3, [r3, #0]      (HIGH word first)
 *         mov lr, sp / mov r2, lr
 *         str r3, [r2, #0] / str r4, [r2, #4]
 *     which is gcc's DImode move split, not two word moves: the high word is
 *     loaded first precisely because the destination pair overlaps the base
 *     register, and the Thumb DImode store needs its address in a general
 *     register, which is where `mov lr, sp / mov r2, lr` comes from.  No
 *     spelling of two 32-bit stores reaches it -- a pointer local for the
 *     frame (u32 *, u16 *, u8 *, assigned early or late) is EXACTLY INERT,
 *     because gcse cprop (pass 07) collapses `bp = buf` and the constant
 *     subscripts then use sp+offset addressing.
 *     The address is the lever's other half: REG_ADDR_SIOMLT_RECV and
 *     REG_ADDR_SIODATA32 are the SAME address (include/gba/io.h:159 against
 *     :161, both 0x120), and the 8 bytes there are SIOMULTI0..3 -- so reading
 *     them as one 64-bit multiplayer receive buffer and then indexing the
 *     halfwords per player is what the function is actually doing.
 *     NOTE for pass 3: u64 in this tree is `unsigned long int`
 *     (include/gba/types.h:7), which is 32 bits on this ABI, so vu64 is NOT a
 *     64-bit type here and REG_SIOMLT_RECV cannot be used as spelled; the
 *     explicit `unsigned long long` cast below is doing the DImode work.  A
 *     union view would read better and should be screened in the humanisation
 *     pass.
 *
 * (2) ONE POINTER TO REG_SIOCNT, READ AT +0 AND STORED AT +2.
 *     The reference keeps 0x04000128 in one register and writes the halfword
 *     at `[r0, #2]`; the earlier draft spelled that destination as its own
 *     absolute address and so pooled 0x0400012a, costing ONE INSTRUCTION IN
 *     EACH of the two f14 arms plus two pool words.  A `vu16 *` to the control
 *     register, dereferenced wide for the 32-bit read and subscripted `[1]`
 *     for the halfword store, is both arms at once.  This also repairs the
 *     draft's first named defect from the other side: with two independent
 *     register pointers there is nothing for cse to derive, and the
 *     `adds r2, #8` that turned one pooled address into the other is gone.
 *
 * (3) THE 0xfefe STORE MUST GO THROUGH AN INT CARRIER -- SET_IO, NOT A BARE
 *     HALFWORD ASSIGNMENT.  This is the single largest lever and it is a MODE
 *     effect, not a spelling one.  `*thumb_movhi_insn` (arm.md:4318-4320)
 *     constrains operand 1 "l,mn,l,*h,*r,I": a HImode constant matches `mn` at
 *     alternative 1 long before the immediate `I` at 5, and recog takes the
 *     first match, so a HImode constant ALWAYS pools -- and alternative 1
 *     carries `pool_range 64` (arm.md:4353).  arm_reorg's barrier walk
 *     (arm.c:5518-5527) keeps the LAST barrier whose address is still below
 *     `minipool_vector_head->max_address`, so a single 64-byte fix drags the
 *     whole dump back to the FIRST barrier in the function -- which is how a
 *     bare `sc[1] = 0xfefe;` bought an entire extra pool dump early, four
 *     words, a third relocation against the struct, and sixteen bytes of size.
 *     SET_IO's `unsigned __value` carrier (include/gba/io.h:781) makes the
 *     constant SImode, where it never pools, and the dump returns to the
 *     reference's position: pool entries went 13 to 10 and SIZE 296 to 276.
 *     The same mechanism is written up on the piece-mate -- see
 *     src/non_matching/rom_c0/800615c.c, "SET_IO(REG_IME, 1), not
 *     REG_IME = 1".
 *
 * (4) TWO TEMPS FOR THE f28/f2c SWAP, IN THE REFERENCE'S ORDER, AND NO
 *     HALFWORD TEMP IN THE LOOP.  `q = f2c; t = f28; f28 = q; f2c = t;`
 *     beats `q = f2c; f2c = f28; f28 = q;` because the reference stores f28
 *     first; and writing the loop's halfword copy as one statement,
 *     `q[n] = buf[i];`, removes a `ldrsh` that a named `unsigned short w`
 *     carrier was producing.  Together: twelve differing down to eight, then
 *     the SET_IO position (before the swap, not between its halves) took it to
 *     three.
 *
 * WHAT REMAINS -- ONE HUNK, THREE ENCODINGS, AND IT IS A SCHEDULING ORDER.
 * The reference loads the pooled 0xfefe, THEN reads f2c, THEN stores the
 * halfword; we load the constant, store, then read f2c.  Because our f2c read
 * happens after the volatile store, the control-register pointer is dead by
 * then and gcc reuses its register for the loaded pointer -- the reference's
 * is still live, so the pointer gets its own.  The register difference is a
 * CONSEQUENCE of the order, not an independent defect.
 * MEASURED, all worse than the three above: an explicit `unsigned int` or
 * `int` carrier at function scope with the f2c read between it and the store
 * (nine) -- it gets the arm's order right and loses the control pointer's
 * materialisation position in the prologue instead; the same carrier
 * block-scoped with an initialiser, SET_IO's own shape (nine); reusing the
 * loop's `int n` as the carrier (twenty-one); the swap reading f28 before f2c
 * (five); reusing the loop's `r` as the swap temp (five); declaring the swap
 * temp before the other two pointers (three, a tie).
 *
 * NEXT: the two defects are coupled through register pressure in the prologue
 * -- every spelling that separates the constant's definition from its use
 * moves the control pointer's pool load later.  The quantitative test is
 * whether the arm's three-insn order can be had while the prologue keeps the
 * control pointer live from the 32-bit read onward; .18.greg's allocno list
 * for that pseudo against the reference's r0 is where to look.
 *
 * STRUCT LAYOUT, read off the offsets and consistent with the three landed
 * neighbours: f0 u8; f4[2] u8 flags; f9 u8; f14 int; f18[2] int; f28/f2c u16*
 * (swapped as a pair); f30[4] / f40[4] u16* (swapped element-wise). f28/f2c and
 * f30/f40 are the same pointers Func_8005d10 plants and Func_800615c swaps.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c0/8006240.c \
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_c.s --func Func_8006240
 */
#include "gba/types.h"
#include "gba/io.h"

struct Snd {
    /* 0x00 */ unsigned char f0;
    /* 0x01 */ unsigned char f1[3];
    /* 0x04 */ unsigned char f4[5];
    /* 0x09 */ unsigned char f9;
    /* 0x0a */ unsigned char fa[10];
    /* 0x14 */ int f14;
    /* 0x18 */ int f18[2];
    /* 0x20 */ unsigned char f20[8];
    /* 0x28 */ unsigned short *f28;
    /* 0x2c */ unsigned short *f2c;
    /* 0x30 */ unsigned short *f30[4];
    /* 0x40 */ unsigned short *f40[4];
};

extern struct Snd ewram_2002240;

void Func_8006240(void)
{
    unsigned short buf[4];
    vu16 *sc;
    unsigned short *q;
    unsigned short *r;
    unsigned short *t;
    int n;
    int i;

    *(unsigned long long *)buf = *(volatile unsigned long long *)REG_ADDR_SIOMLT_RECV;
    sc = &REG_SIOCNT;
    ewram_2002240.f9 = (*(vu32 *)sc << 25) >> 31;
    if (ewram_2002240.f14 == -1) {
        SET_IO(sc[1], 0xfefe);
        q = ewram_2002240.f2c;
        t = ewram_2002240.f28;
        ewram_2002240.f28 = q;
        ewram_2002240.f2c = t;
    } else if (ewram_2002240.f14 >= 0) {
        sc[1] = ewram_2002240.f2c[ewram_2002240.f14];
    }
    if (ewram_2002240.f14 <= 14)
        ewram_2002240.f14++;
    for (i = 0; i < 2; i++) {
        if (buf[i] == 0xfefe && ewram_2002240.f18[i] > 13) {
            ewram_2002240.f18[i] = -1;
        } else {
            n = ewram_2002240.f18[i];
            q = ewram_2002240.f30[i];
            q[n] = buf[i];
            if (n == 13) {
                r = ewram_2002240.f40[i];
                ewram_2002240.f40[i] = q;
                ewram_2002240.f30[i] = r;
                ewram_2002240.f4[i] |= 1;
            }
        }
        if (ewram_2002240.f9 != 0)
            ewram_2002240.f4[i] |= 2;
        if (ewram_2002240.f18[i] <= 14)
            ewram_2002240.f18[i]++;
    }
    if (ewram_2002240.f0 == 8) {
        REG_TM3CNT_H = 0;
        REG_SIOCNT |= 0x80;
        REG_TM3CNT_H = 0xc0;
    }
}
