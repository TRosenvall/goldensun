/* DisplayMenuArrowCursor2 (DrawPartyPanel) -- 0x0801b248, PARK.  HOLDS at 30.
 *
 * STILL NON-MATCHING, **30 of 140 stream encodings** -- RE-DERIVED batch 343
 * brief C, figure CONFIRMED.
 *   ref 140 encodings / ours 140;  **134 INSTRUCTIONS AGAINST 134** (16-bit);
 *   SIZE EQUAL (292 bytes), RELOCATIONS EQUAL;  first differing index 10.
 *   So the figure IS a distance.
 *   PINS 0 (tools/shimcount.py prints NO ROWS).  No devices, no flags.
 *
 *   NOTE ON THE OLD COUNT LINE: "COUNTS EQUAL (ref 140, ours 140)" quotes the
 *   STREAM count on the count line.  The INSTRUCTION counts are 134/134.  Both
 *   are equal either way, so the old verdict stood; recorded because this tree
 *   has now found this slip in a dozen parks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/DisplayMenuArrowCursor2.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_a_a.s --func DisplayMenuArrowCursor2
 *
 * SPLIT SHAPE: none -- the .s holds one .thumb_func_start; tools/piece.py ->
 * rom_1aeec_a_a_c_a_c_a_a.s, 1 function, all 1 parked.  tools/datacheck.py
 * exits 0 silently (no data section).
 *
 * STALE CROSS-REFERENCE CORRECTED: the old header says "the sibling
 * DisplayMenuArrowCursor is ALSO a park (**6 of 133**)".  It is at **5 of 133**
 * since batch 337 brief A, which also swapped its batch-329 inline-asm barrier
 * for a `do { } while (0)` anchor at the same figure, so that park is now
 * SHIM-FREE.  Its struct layout still reproduces here and is kept unchanged.
 *
 * AUTOSWEEP, BOTH CHEAP AXES, **CLOSED** -- a recorded negative, do not
 * re-screen: six flag groups EXACTLY INERT at 30 (-O1 99,
 * -fno-schedule-insns2 47, -ffixed-r7 **141**, i.e. worse than the whole
 * residue), and ALL 26 anchor placements inert or worse.
 *
 * ###########################################################################
 * ## BATCH 343 BRIEF C: THE PARK'S OWN "THAT IS THE NEXT MOVE" IS NOW
 * ## **EXECUTED**, AND ALL FOUR OF ITS NAMED ROUTES ARE CLOSED.
 * ## ITS ARITHMETIC IS EXACTLY RIGHT.  THE LEVER IS NOT THERE.
 * ###########################################################################
 *
 * FIRST, THE ARITHMETIC REPRODUCES VERBATIM, every figure of it:
 *   .17.lreg  `Register 34 used 7 times across 82 insns ... pref BASE_REGS; pointer`
 *             `Register 119 used 3 times across 18 insns ... pref LO_REGS; pointer`
 *   .18.greg  `;; 10 regs to allocate: 32 37 51 117 33 35 34 119 121 36`
 *             dispositions `34 in 8`, `119 in 10`
 *   prices    34 -> 1707,  119 -> 1666,  gap 41 (2.4%)
 *
 * AND THE PRICING MODEL IS NOW CHECKED RATHER THAN ASSUMED: allocno_compare
 * (**global.c:600-618**), floor_log2(n_refs)*n_refs/live_length*10000*size,
 * predicts that ten-allocno priority line EXACTLY from the lreg refs and spans
 * -- and did the same on Func_80165d8 (ten allocnos) and Func_80c0f98 (eight)
 * this batch.  `global.c:1875-1881` prints `allocno_order` ascending, so that
 * line IS the priority order.  Three functions, three exact reproductions.
 *
 * ROUTE "119 at 4 refs (4444)" -- **CLOSED BY THE INSTRUCTION SET.**  The park
 * could not have known this without reading the RTL.  reg 119 is
 * `m + (i*0x34 + 0x10)`: insn 223 is `(set (reg:SI 119) (plus (reg/v:SI 32)
 * (reg:SI 118)))`, and its THREE references are exactly that set, a
 * sign-extended HImode read of `(119 + 2)` at insn 227, and a HImode write of
 * `(119 + 2)` at insn 325 -- i.e. `m->a[i].y`, which is `short`.  **Every other
 * field touched in that block sits at a NEGATIVE offset from 119** (`slot` -4,
 * `tile` -2, `f0` -8; `x` is at +0 but is never accessed with index `i`), and
 * Thumb `ldrh`/`strh` take a NON-NEGATIVE immediate only.  So no further access
 * can share base 119 and the fourth reference CANNOT BE BOUGHT.
 *
 * ROUTE "o at 6 refs (1463)" -- **CLOSED BY MEASUREMENT**, and the mechanism
 * looked right.  o's eight bitfield stores do not fall in OAM-halfword order:
 *     objMode mosaic bpp affineMode shape -> attr0
 *     matrixNum size                      -> attr1
 *     priority                            -> attr2
 * and the body writes them objMode, mosaic, bpp, affineMode, matrixNum, size,
 * **shape**, priority -- so `shape`, an attr0 field, sits AFTER the two attr1
 * writes and breaks the attr0 run, which should force an extra read-modify-write
 * and be o's seventh reference.  Swept 23 variants: `shape` at all 8 positions,
 * all 6 attribute-group rotations, and each of the 8 stores routed through
 * `a->oam.` instead of `o->`.  **Nothing improves.**  The park's own order
 * (`shape` at position 6) is 30; every order that changes anything reads
 * **110-111 with RELOCDIFF**.
 *
 * ROUTES "119 live_length 17 (1764)" and "o live_length >= 85 (1647)" --
 * **CLOSED BY MEASUREMENT.**  reg 119 is born at the `m->a[i].y == 0` TEST (it
 * is the base of the sign-extended read at insn 227) and dies at the
 * `m->a[i].y = m->f398` write (insn 325), so its span is exactly that distance
 * and MOVING THAT STORE is the whole lever; o is born at `o = &a->oam;` and
 * dies at the last bitfield store.  I crossed ALL 60 orderings of the y-block's
 * five units (`slot=`, `tile=`, `y=`, `f0=`, the bitfield block), separating the
 * 12 semantics-preserving ones from the 48 that move a store across a call
 * (generated as instruments and labelled `flag_*`, since a callee may read the
 * field).  **All 60 inert or worse**: the park's own order is 30, the best other
 * SAFE row 37, the best instrument 40 with RELOCDIFF.  The park's own earlier
 * row "the y store hoisted above the UploadSpriteGFX call (138, size 280)" is
 * one of this set and reproduces as worse.
 *
 *   >> SO ALL FOUR NAMED ROUTES ARE CLOSED -- three by measurement and one by
 *      the instruction set.  **"That is the next move" has been made and it is
 *      negative.**  The 41-point gap between 1707 and 1666 is real and the
 *      arithmetic for closing it is right; what does not exist is an edit that
 *      changes either allocno's refs or span WITHOUT changing the program.
 *      DO NOT RE-DERIVE THIS ARITHMETIC -- it is confirmed.  The open ground is
 *      run 1 and run 2 below, which this batch did not re-attack. <<
 *
 * ###########################################################################
 * ## EVERYTHING BELOW IS CARRIED FORWARD UNCHANGED AND STILL THE MAP.
 * ###########################################################################
 *
 * 34 -> 30:  src = L342f8 MOVED DOWN inside the i != 0 arm, to sit AFTER the
 * "if (m->f39c != 0) n -= m->f39c;" test rather than at the top of the arm.
 * The ROM loads .L342f8 into r5 (a LO register) mid-arm and only does
 * "mov r11, r5" near the arm end; reload splits the hi-register store into
 * that pair and sched2, which runs after reload, separates them.  Position
 * sweep, all with equal counts: top of arm 34 | after the f39c test 30 |
 * after the n > 5 block 42 | end of the arm 132 (size 276, eight insns lost).
 *
 * THE REMAINING 30 IS THREE RUNS.  Named, with the deciding pass identified:
 *
 * RUN 1 -- prologue addressing, 6 encodings, indices 10-19.  OPEN.
 *   The ROM builds TWO bases from the scaled index:
 *       add r2,r6,r3 / add r2,#0x28 / mov r10,r2   -- o = (m + i*0x34) + 0x28
 *       add r3,#0x8  / add r4,r6,r3                -- a = m + (i*0x34 + 8)
 *   so the index register survives the first add and dies in the second.  Ours
 *   computes m + i*0x34 ONCE and adds 8 and 0x28 to copies of it, which costs
 *   a "mov r4,r3" where the ROM has a second "add rX,r6,r3".  Same instruction
 *   count either way.
 *   This is expr.c:7301 both_summands: when op1 is a PLUS with a CONSTANT_P
 *   second operand, the "associate it to put the constant outside" branch
 *   REWRITES m + (idx + 8) into (m + idx) + 8, after which cse shares the
 *   m + idx.  Defeating it needs idx + 8 in its own pseudo, and that costs two
 *   instructions (measured below).
 *
 * RUN 2 -- the residue of the src placement, 4 encodings.  OPEN.  Even at 30
 *   the ROM has "ldr r5,=.L342f8" BEFORE the "cmp r3,#0 / beq" and
 *   "mov r11,r5" as the last instruction of the arm; ours emits both after the
 *   merge label.  The pair is created by reload, so only sched2 can separate it.
 *
 * RUN 3 -- a PURE GLOBAL-ALLOC RANKING SWAP, 5 encodings.  **THE ARITHMETIC IS
 *   CONFIRMED AND ALL FOUR OF ITS ROUTES ARE NOW CLOSED -- see above.**
 *   o is in r8 and the m + i*0x34 + 0x10 pointer is in r10; the ROM has them
 *   the other way.  REG_ALLOC_ORDER (arm.h:989) runs
 *   3,2,1,0,12,14,4,5,6,7,8,10,9,11 -- so among the hi registers r8 is tried
 *   before r10, and whichever allocno is allocated FIRST takes r8.
 *       ;; 10 regs to allocate: 32 37 51 117 33 35 34 119 121 36
 *       Register 34  used 7 times across 82 insns ... pref BASE_REGS; pointer
 *       Register 119 used 3 times across 18 insns ... pref LO_REGS;   pointer
 *   34 is o and 119 is that pointer.  allocno_compare (global.c:598) prices
 *   them as floor_log2(n_refs) * n_refs / live_length * 10000 * size:
 *       34 : floor_log2(7)=2 -> 2*7/82  * 10000 = 1707
 *       119: floor_log2(3)=1 -> 1*3/18  * 10000 = 1666
 *   a 41-point gap, 2.4 percent.  34 therefore sorts first and takes r8.
 *
 * MEASURED INERT OR WORSE.  The addressing dimension is a FLAT CROSS: thirteen
 * spellings at the 34 baseline and five more crossed against the 30 baseline,
 * every one of them EXACTLY 30 or much worse.
 *   inert at 30: a=&m->a[i] with o=&m->a[i].oam; a=(char *)m + (i*0x34 + 8)
 *     with o=&a->oam; a=&m->a[i] with o=(char *)m + i*0x34 + 0x28; a named
 *     "idx = i*0x34" feeding both.
 *   worse: o assigned BEFORE a, in any spelling (131, +2 insns, size 296);
 *     a named "off = idx + 8" (129, +2 insns); a = (char *)o - 0x20 (128);
 *     m->a[i].f0 = 0 before m->a[i].y = m->f398 (37); the y store hoisted
 *     above the UploadSpriteGFX call (138, size 280); a->f0 for m->a[i].f0
 *     (130, size 280); the whole y==0 block routed through a (139, size 248).
 *   ADDED BATCH 343: the 8-store bitfield order (23 variants, 110-111 or
 *     inert), the y-block's 60 statement orders (37 best safe, 40 best
 *     instrument), all 26 anchor placements, all 8 flag groups.
 *
 * REGISTER PINS ARE NOT A USABLE INSTRUMENT HERE, which is worth recording
 * because run 3 invites one.  "register struct OamSprite *o __asm__("r10")"
 * reads 125 at SIZE 308 and the same pin on src at r11 reads 125 at SIZE 288:
 * pinning a hi register changes the hi-register save/restore set in the
 * prologue and epilogue, so the figure is not a distance at all.  This is the
 * brief rule "a register pin is NOT a free diagnostic" with numbers attached.
 */
struct OamSprite {
    unsigned char pad[4];
    unsigned int y:8;
    unsigned int affineMode:2;
    unsigned int objMode:2;
    unsigned int mosaic:1;
    unsigned int bpp:1;
    unsigned int shape:2;
    unsigned int x:9;
    unsigned int matrixNum:5;
    unsigned int size:2;
    unsigned int tileNum:10;
    unsigned int priority:2;
    unsigned int paletteNum:4;
};

struct Arrow {
    unsigned short f0;
    unsigned short f2;
    unsigned short slot;
    unsigned short tile;
    unsigned short x;
    short y;
    unsigned char pad0c[0x14];
    struct OamSprite oam;
    unsigned char pad2c[8];
};

struct Menu {
    unsigned char pad00[8];
    struct Arrow a[2];
    unsigned char pad70[0x394 - 0x70];
    unsigned short f394;
    unsigned short f396;
    unsigned short f398;
    unsigned short pad39a;
    unsigned short f39c;
};

extern unsigned char L342f8[] __asm__(".L342f8");
extern unsigned char L33ef8[] __asm__(".L33ef8");
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int n, void *src);

void DisplayMenuArrowCursor2(struct Menu *m, int i)
{
    struct OamSprite *o;
    struct Arrow *a;
    void *src;
    unsigned int n;

    a = &m->a[i];
    o = &a->oam;
    src = 0;
    a->f2 = 0;
    if (i != 0) {
        n = m->f394;
        if (m->f39c != 0)
            n -= m->f39c;
        src = L342f8;
        if (n > 5) {
            a->f2 = 1;
            n = 5;
        }
        m->a[1].x = m->f396 + (n - 1) * 16 + 0x11;
    } else {
        src = L33ef8;
        m->a[0].x = m->f396 - 9;
        if (m->f39c != 0)
            m->a[0].f2 = 1;
    }
    if (m->a[i].y == 0) {
        m->a[i].slot = AllocSpriteSlot();
        m->a[i].tile = UploadSpriteGFX(m->a[i].slot, 0x80, src);
        m->a[i].y = m->f398;
        m->a[i].f0 = 0;
        o->objMode = 0;
        o->mosaic = 0;
        o->bpp = 1;
        o->affineMode = 0;
        o->matrixNum = 0;
        o->size = 0;
        o->shape = 2;
        o->priority = 0;
    }
}
