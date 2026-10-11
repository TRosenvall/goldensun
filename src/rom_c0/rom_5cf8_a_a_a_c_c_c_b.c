/* Func_8006240 (0x08006240)  --  LANDING, 3 -> 0 of 130.
 *
 * FIGURE I MEASURED (never inherited): the installed park reads
 * **3 differing stream encodings of 130**, ref 130 / ours 130 encodings,
 * **119/119 16-bit instructions**, first differing index 21.  Lengths are
 * equal, so that 3 was a TRUE DISTANCE and not a phase offset.  This body
 * reads **0** -- objcmp OK, SIZE / INSTRUCTION COUNT / RELOCATIONS all silent.
 * PINS 0 (`tools/shimcount.py` reports nothing).  DEVICE-FREE, FLAG-FREE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c0/rom_5cf8_a_a_a_c_c_c_b.c \
 *     asm/rom_c0/rom_5cf8_a_a_a_c_c_c_b.s --func Func_8006240
 * (Before the split, against the unsplit reference:
 *   ... python3 tools/objcmp.py scratch_elev/b337/B/p2_candidate.c \
 *       asm/rom_c0/rom_5cf8_a_a_a_c_c_c.s --func Func_8006240)
 *
 * SPLIT SHAPE.  `tools/piece.py` confirms the piece
 * `asm/rom_c0/rom_5cf8_a_a_a_c_c_c.s` holds exactly TWO functions, both parked:
 * Func_800615c (84 of 109) and Func_8006240 (this one).  Func_8006240 is the
 * LAST, so `tools/split_s.py --dry-run asm/rom_c0/rom_5cf8_a_a_a_c_c_c.s
 * Func_8006240` yields only two parts and no `_c`:
 *     _a.s  1 function(s), 116 lines   (Func_800615c, stays hand-written)
 *     _b.s  1 function(s), 148 lines   (this function)  -> src/.../_b.c
 * `tools/datacheck.py` on the piece prints NOTHING, so there is no data
 * section, no export and no text/data split.  Func_800615c's own 84 is
 * IRRELEVANT to this landing -- the brief's framing was right and the park's
 * batch-334/335 line "converting one without the other buys nothing" is the
 * "neither ships alone" error.
 *
 * ---------------------------------------------------------------------------
 * WHAT THE 3 WERE, AND WHAT CLOSED THEM
 *
 * The residue was the whole `f14 == -1` arm, indices 20-25:
 *
 *     ROM                         park body
 *  20 ldr  r3, =0xfefe            ldr  r3, =0xfefe
 *  21 ldr  r2, [r1, #0x2c]        strh r3, [r0, #2]     <-- store moved up
 *  22 strh r3, [r0, #2]           ldr  r0, [r1, #0x2c]  <-- and so f2c's value
 *  23 ldr  r3, [r1, #0x28]        ldr  r3, [r1, #0x28]      takes the control
 *  24 str  r2, [r1, #0x28]        str  r0, [r1, #0x28]      pointer's own r0
 *  25 str  r3, [r1, #0x2c]        str  r3, [r1, #0x2c]
 *
 * The park had this right as far as it went: the f2c read must happen BEFORE
 * the volatile halfword store, and the register difference is a consequence --
 * the ROM's control pointer is still live at the store, so the loaded f2c
 * value cannot have r0.  What the park could not do was move the read without
 * losing the prologue, and its "NEXT" named the right dump (.18.greg) but the
 * wrong unit of work.  It took TWO edits CROSSED, neither of which pays alone:
 *
 *   (1) **A FRESH, ARM-LOCAL VARIABLE FOR THE f2c VALUE.**  The park reused
 *       `q`, the loop's `unsigned short *`.  `q` is live in the loop as well,
 *       so it is a GLOBAL allocno (global.c), and once the f2c read is moved
 *       ahead of the store it CONFLICTS with the control pointer -- which is
 *       also global.  global-alloc then resolves the conflict by giving the
 *       control pointer **r4** and moving its pool load four insns later,
 *       which is the "9 / 10 / 12" family the park recorded and read as a
 *       prologue-coupling bound.  A variable scoped to the arm is a LOCAL
 *       quantity instead, local-alloc places it, and the control pointer keeps
 *       r0 at index 6.  Measured: the same statement order with `q` reads
 *       **10 of 130, first index 6**; with a fresh `u` it reads **6 of 130,
 *       first index 20** -- the prologue defect is gone and only the arm is
 *       left.  The park's own note "every spelling that separates the
 *       constant's definition from its use moves the control pointer's pool
 *       load later" is therefore true of `q` and FALSE in general: it is the
 *       allocno CLASS of the temp, not the separation, that moves the load.
 *
 *   (2) **THE 0xfefe CARRIER HOISTED ABOVE THE f2c READ.**  With (1) alone the
 *       arm is the ROM's six instructions with r2 and r3 SWAPPED, because
 *       whichever of the two loads is emitted first takes the head of
 *       REG_ALLOC_ORDER (arm.h:989, `3, 2, 1, 0, ...`).  The ROM emits the
 *       constant first.  SET_IO (include/gba/io.h:781-785) puts its
 *       `unsigned __value =` assignment IMMEDIATELY before the store, so with
 *       SET_IO there is nowhere to put the read.  Writing the carrier out --
 *       `unsigned v = 0xfefe;` then the read then `sc[1] = v;` -- orders the
 *       two loads the ROM's way and the swap disappears.
 *       The SImode carrier is still doing the park's lever (3): a HImode
 *       constant always matches `mn` at alternative 1 of `*thumb_movhi_insn`
 *       (arm.md:4318-4320) whose `pool_range` is 64 (arm.md:4353), which drags
 *       arm_reorg's dump back to the first barrier (arm.c:5518-5527).  So the
 *       carrier is NOT a device introduced for this landing -- it is SET_IO's
 *       own body, written out so a statement can sit inside it.
 *
 * The cross is the finding: `q` + carrier is **9**; fresh `u` + SET_IO is
 * **6**; fresh `u` + carrier is **0**.
 *
 * MEASURED THIS ROUND (ref 130 encodings, 119/119 instructions unless noted).
 * Twelve dependency-legal statement orders of the arm x three temp choices,
 * plus ten carrier placements -- 7 cells match, and the inert/worse list is:
 *   0   v=0xfefe; u=f2c; sc[1]=v; t=f28; f28=u; f2c=t   (SEVEN spellings:
 *         block-scoped initialisers; `v` at function scope; `int v` for
 *         `unsigned v`; `u` at function scope; `v` declared before `u`;
 *         both temps fresh; the f2c/f28 stores swapped)
 *   3   the park body, and the park body with the two stores swapped
 *   4   t=f28; sc[1]=..; u=f2c; f28=u; f2c=t  (fresh temp, store in the middle)
 *   6   fresh temp + SET_IO after the read (three orders)
 *   9   carrier + `q` (the park's own "nine", reproduced exactly)
 *  10-12 `q` + the read before the store, every order -- first index 6
 *  16   carrier with the f28 read also moved before the store; f28's value
 *         read into a block-scoped initialiser too
 * 114/118  moving SET_IO below the whole swap, or folding both temps into one
 *         -- 117/118 instructions against 119, COUNT+SIZE+RELOC, dead
 *
 * ONE CORRECTION TO THE PARK worth carrying: its struct comment says
 * "f28/f2c u16* (swapped as a pair)".  That is right, and the landing shows
 * the swap's TEMPS are not interchangeable with the loop's pointers even
 * though both are `unsigned short *` -- a pass-3 humanisation that merges
 * them back into one declaration will reopen this function.
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
        unsigned v = 0xfefe;
        unsigned short *u = ewram_2002240.f2c;

        sc[1] = v;
        t = ewram_2002240.f28;
        ewram_2002240.f28 = u;
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
