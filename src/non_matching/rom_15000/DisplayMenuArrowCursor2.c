/* DisplayMenuArrowCursor2 (DrawPartyPanel) -- 0x0801b248, PARK.
 *
 * STILL NON-MATCHING, 30 of 140 encodings -- RE-MEASURED batch 324B.  WAS 34.
 *   COUNTS EQUAL (ref 140, ours 140), SIZE EQUAL (292 bytes, objcmp prints no
 *   SIZE line), RELOCATIONS EQUAL.  So the figure IS a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/DisplayMenuArrowCursor2.c \
 *     asm/rom_15000/rom_1aeec_a_a_c_a_c_a_a.s --func DisplayMenuArrowCursor2
 *
 * SPLIT SHAPE: none -- the .s holds one .thumb_func_start.  PINS: 0.
 * The sibling DisplayMenuArrowCursor is ALSO a park (6 of 133), not a landing;
 * its struct layout reproduces here and is kept unchanged.
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
 * RUN 1 -- prologue addressing, 6 encodings, indices 10-19.
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
 * RUN 2 -- the residue of the src placement, 4 encodings.  Even at 30 the ROM
 *   has "ldr r5,=.L342f8" BEFORE the "cmp r3,#0 / beq" and "mov r11,r5" as the
 *   last instruction of the arm; ours emits both after the merge label.  The
 *   pair is created by reload, so only sched2 can separate it.
 *
 * RUN 3 -- a PURE GLOBAL-ALLOC RANKING SWAP, 5 encodings.  o is in r8 and the
 *   m + i*0x34 + 0x10 pointer is in r10; the ROM has them the other way.
 *   REG_ALLOC_ORDER (arm.h:989) runs 3,2,1,0,12,14,4,5,6,7,8,10,9,11 -- so
 *   among the hi registers r8 is tried before r10, and whichever allocno is
 *   allocated FIRST takes r8.  From base.c.17.lreg / base.c.18.greg on this
 *   body:
 *       ;; 10 regs to allocate: 32 37 51 117 33 35 34 119 121 36
 *       Register 34  used 7 times across 82 insns ... pref BASE_REGS; pointer
 *       Register 119 used 3 times across 18 insns ... pref LO_REGS;   pointer
 *   34 is o and 119 is that pointer.  allocno_compare (global.c:598) prices
 *   them as floor_log2(n_refs) * n_refs / live_length * 10000 * size:
 *       34 : floor_log2(7)=2 -> 2*7/82  * 10000 = 1707
 *       119: floor_log2(3)=1 -> 1*3/18  * 10000 = 1666
 *   a 41-point gap, 2.4 percent.  34 therefore sorts first and takes r8.  Any
 *   ONE of these flips it: o at 6 refs (1463), o live_length >= 85 (1647), 119
 *   live_length 17 (1764), or 119 at 4 refs (4444).  THAT is the next move --
 *   not another spelling of the addresses.
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
