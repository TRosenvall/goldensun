/* Func_801f200 (0x0801f200) -- NON-MATCHING, 404 of 447 encodings differ.
 * Reference asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c_c_c.s.  Intended park path:
 * src/non_matching/rom_15000/801f200.c
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_15000/801f200.c \
 *       asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c_c_c.s --func Func_801f200
 *
 * NON-MATCHING, 404 of 447 encodings differ.
 * A TRUE DISTANCE: 980 == 980 bytes AND 447 == 447 encodings, so 404 ranks.
 * tools/aligncmp.py puts 194 of 447 aligned-equal, 333 in 79 hunks -- the high
 * figure is a register rotation, not 404 separate defects; see the blocker.
 * Shim count 0 (tools/shimcount.py): pin-free, no fakematch.txt row.
 * THE RELOCATION SET, SYMBOLS AND MULTIPLICITIES ALREADY MATCH EXACTLY -- 2
 * R_ARM_ABS32 (iwram_3001e90, gState) and 31 R_ARM_THM_CALL, same symbols and
 * same counts.  Only the SEQUENCE differs, in one place: the ROM emits its
 * iwram_3001e90 pool word AFTER the `bl _GetPartySize` and we emit it before.
 * That is one literal-pool dump position, which is downstream of branch
 * distances -- do not read it as a second finding.
 *
 * SPLIT: asm/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c_c_c.s holds TWO functions,
 * Func_801f088 (0x0801f088, also unattempted) and Func_801f200, so landing
 * needs a two-way split into `..._c_c_c_a.s` / `..._c_c_c_b.s`.
 * tools/datacheck.py reports NO data requirement and stage1.ld:502 names the
 * object in .text ONLY, so the split is a plain text split with no rodata to
 * place.  Func_801f088 is CALLED from inside Func_801f200, so whichever half
 * is converted first must keep the other's symbol visible.
 *
 * ================================================================
 * WHAT CLOSED, BY PASS (428 -> 432 -> 431 -> 404 at exact length)
 * ================================================================
 *
 * 1. THE MASK IS `~2`, NOT `~3`.  `mov r3, #3 / neg r3, r3 / and r0, r3` is
 *    `& -3` == `& ~2`.  Reading the `neg` as part of the literal (`~3`) makes
 *    gcc emit `mov r3, #4 / neg r3, r3` and is wrong at BOTH sites.  The rule:
 *    a `mov #N` + `neg` before an `and` is `& ~(N-1)`, and the off-by-one is
 *    invisible in the mnemonics.
 *
 * 2. THE 0xff SENTINEL TEST IS A HALFWORD COMPARE.  The ROM's
 *    `movs r1,#255 / lsls r1,#16 / lsls r3,#16 / cmp r3,r1` is the HImode
 *    compare documented in docs/elevation.md ("A HALFWORD COMPARE SHIFTS BOTH
 *    SIDES LEFT 16").  Holding the value in an `int` and casting at the
 *    comparison gives `cmp r3, #255`; an `unsigned short` local gives the
 *    ROM's pair.  Both loop exits are the same test.
 *
 * 3. THE PARTY-ID SOURCE IS A `unsigned short *`, INDEXED.  `ldrh`'s Thumb
 *    immediate offset stops at 62, so `p[0x2c]` (byte offset 0x58) forces the
 *    register-offset form `movs r3,#88 / ldrh r3,[r7,r3]`.  Spelling the same
 *    address as `*(unsigned short *)(charptr + 0x58)` gives
 *    `add / ldrh [r3,#0]` instead -- one instruction more and a different
 *    register.  Reach a far halfword through a TYPED pointer, not a byte offset.
 *
 * 4. THREE GLOBALS, ONE POOL WORD, ONE NAMED POINTER.  The ROM loads
 *    &iwram_3001e90 once and reaches iwram_3001e8c as -4 and iwram_3001e74 as
 *    -0x1c off it (0x3001e90-4 and -0x1c).  A named `unsigned char *g` local
 *    with `*(T **)(g - 4)` / `*(T **)(g - 0x1c)` gives the single ABS32
 *    relocation the ROM has; naming the globals separately gives three.
 *
 * 5. `i = 0;` MUST COME BEFORE `if (flags & 2) d = 5;`.  THIS IS THE PASS THAT
 *    MADE SIZE AND COUNT EXACT (445/976 -> 447/980).  With the two statements
 *    the other way round gcc folds the loop's zero into the branch and comes
 *    out two instructions short.  Same family as the `i = n;` merge-point-copy
 *    lever in src/rom_15000/rom_1de5c_c_c_c_c_a_a_a_c_c_b.c (the matched
 *    sibling Func_801ef68): where the loop index's live range STARTS decides
 *    the count, and the cheapest control over it is which side of the `if` the
 *    initialisation sits on.
 *
 * 6. THE TWO `col * 8` SPILL SLOTS ARE NOT TWO VARIABLES.  sp+0x0c and sp+0x14
 *    both receive the same register in the preheader.  Two named locals and the
 *    expression written inline at both uses (loop-invariance-by-construction)
 *    score IDENTICALLY at exact length, so the duplication is loop.c hoisting
 *    `col * 8` once per basic block and NOT a declaration to be recovered.
 *    The full spill set is reproduced either way: sp+4/8/0xc/0x10/0x14/0x18/
 *    0x1c/0x20 with the outgoing-arg word at sp+0 and frame 0x34, and the two
 *    declared aggregates on top (u16 ids[6] at 0x28, u8 buf[4] at 0x24).
 *
 * ================================================================
 * THE BLOCKER: A TWO-CYCLE REGISTER SWAP, (r5 r6) AND (r8 r10)
 * ================================================================
 *
 * At exact size, exact count and an exact relocation multiset, the ENTIRE
 * residue is that four allocnos sit in each other's registers:
 *
 *              ROM          ours
 *     d / u    r5           r6
 *     w        r6           r5
 *     box      r8           r10 (sl)
 *     i        r10 (sl)     r8
 *     p        r7           r7      (agree)
 *     v        r9           r9      (agree)
 *     flags    r11 (fp)     r11     (agree)
 *
 * Note `d` and the unit pointer `u` share ONE allocno on BOTH sides, so that
 * part of the reading is confirmed.  Because the count matches, this is an
 * ALLOCATION TIE and not a missing quantity (docs/elevation.md's
 * discriminator), which also means respelling the body cannot fix it: the
 * inputs to change are n_refs / live_length, i.e. WHERE a value's first and
 * last reference sit.
 *
 * Measured against it, all at exact length: three permutations of the entry
 * block's six statements (404, 405, 404 -- statement order does not touch it,
 * as docs/elevation.md's sched2 entry predicts), `col = d;` to share d's zero
 * with col (404), and two named `col * 8` locals (404).
 *
 * The priority formula from the matched sibling
 * (floor_log2(REG_N_REFS) * REG_N_REFS / REG_LIVE_LENGTH, both inputs printed
 * by `-da` in .17.lreg) is the instrument to point at this: the next step is
 * one `-da` dump and the `;; N regs to allocate:` line from .18.greg, to see
 * which of the two pairs is closest to tied and by how much.  This function is
 * a good candidate for that because its sibling in the same object is the
 * tree's worked example for the formula.
 */
extern unsigned char iwram_3001e90[];
extern unsigned char gState[];

extern int _Func_80b6a60(int n);
extern int _GetPartySize(void);
extern int _Func_80be0b4(int a, void *buf);
extern void ClearUIRegion(int x, int y, int w, int h);
extern void Func_8016498(void *box);
extern void Func_801ef68(unsigned short *box, int flags);
extern void Func_801eea0(int a);
extern void Func_80170f8(int x, int y, int w, int h);
extern void SetTextColor(int c);
extern void Func_801ea3c(int v, unsigned short *box, int x, int y, int mode);
extern void Func_801e8b0(void *u, unsigned short *box, int x, int y);
extern void Func_801f088(unsigned short *box, int a, int b, int c);
extern void Func_8019000(unsigned short *box, int id, int a, int b, int e);
extern void Func_8018efc(unsigned short *box, int ch, int a, int b);
extern short *_GetUnit(int id);

void Func_801f200(int flags)
{
    unsigned short ids[6];
    unsigned char buf[4];
    char *base;
    int count;
    int col;
    int x1;
    unsigned short *idp;
    int x2;
    int y2;
    int off;
    unsigned short *w;
    unsigned short *box;
    short *u;
    unsigned short *p;
    unsigned char *src;
    unsigned char *g;
    unsigned short t;
    int i, d, hp, mhp, v, c2;

    g = iwram_3001e90;
    w = *(unsigned short **)g;
    base = *(char **)(g - 4);
    p = *(unsigned short **)(g - 0x1c);
    d = 0;
    box = *(unsigned short **)w;
    col = d;
    if (base[0xea5] != 0) {
        count = _Func_80b6a60(0);
        col = -1;
        i = 0;
        if ((unsigned)i < (unsigned)count) {
            idp = ids;
            t = p[0x2c];
            *idp = t;
            if (t != 0xff) {
                unsigned short *q = p + 0x2c;
                off = 0;
                do {
                    i++;
                    off += 2;
                    if ((unsigned)i >= (unsigned)count)
                        break;
                    q++;
                    t = *q;
                    *(unsigned short *)((char *)idp + off) = t;
                } while (t != 0xff);
            }
        }
    } else {
        count = _GetPartySize();
        i = 0;
        idp = ids;
        if ((unsigned)i < (unsigned)count) {
            unsigned short *dst = idp;
            src = &gState[0xfc * 2];
            do {
                *dst = *src;
                i++;
                src++;
                dst++;
            } while ((unsigned)i < (unsigned)count);
        }
        idp[i] = 0xff;
    }
    count = i;
    if (flags == -1)
        flags = w[6];
    if ((flags & 1) == 0)
        flags &= ~2;
    if (base[0xea5] == 0 || _Func_80be0b4(0, 0) == 0)
        flags &= ~2;
    if (flags == 9) {
        ClearUIRegion(w[2], w[3], w[4], w[5]);
        return;
    }
    base[0xea6] = 1;
    if (w[6] == flags) {
        Func_8016498(box);
        Func_801ef68(box, flags);
    } else {
        ClearUIRegion(w[2], w[3], w[4], w[5]);
        Func_801eea0(flags);
        box[4] = w[4];
        box[5] = w[5];
        box[6] = w[2];
        Func_80170f8(w[2], w[3], w[4], w[5]);
        Func_801ef68(box, flags);
    }
    i = 0;
    if (flags & 2)
        d = 5;
    if (count != 0) {
        x1 = col * 8;
        x2 = col * 8;
        y2 = d + 1;
        off = 0;
        idp = ids;
        v = d * 8;
        do {
            u = _GetUnit(*(unsigned short *)((char *)idp + off));
            hp = u[0x38 / 2];
            mhp = u[0x34 / 2];
            if (hp == 0) {
                SetTextColor(2);
            } else if (hp <= mhp / 4) {
                SetTextColor(4);
            } else {
                SetTextColor(0xf);
            }
            base[0xea7] = 0xe;
            if (base[0xea5] != 0)
                base[0xea7] = 5;
            Func_801ea3c(hp, box, v, x1 + 8, 0);
            base[0xea7] = 0xf;
            Func_801e8b0(u, box, v, x1);
            SetTextColor(0xf);
            if (u[0x34 / 2] != 0) {
                c2 = u[0x38 / 2] * 40 / u[0x34 / 2];
                if (c2 == 0 && u[0x38 / 2] != 0)
                    c2 = 1;
                Func_801f088(box, y2, col + 2, c2);
            }
            if (flags & 1) {
                base[0xea7] = 0xe;
                if (base[0xea5] != 0)
                    base[0xea7] = 5;
                Func_801ea3c(u[0x3a / 2], box, v, x2 + 0x10, 1);
                if (u[0x36 / 2] != 0) {
                    c2 = u[0x3a / 2] * 40 / u[0x36 / 2];
                    if (c2 == 0 && u[0x3a / 2] != 0)
                        c2 = 1;
                    Func_801f088(box, y2, col + 3, c2);
                }
            }
            y2 += 6;
            off += 2;
            i++;
            v += 0x30;
        } while (i != count);
    }
    base[0xea7] = 0xf;
    if (base[0xea5] != 0 && (flags & 2)) {
        int r = col;
        int r2;
        if (flags & 1)
            r++;
        _Func_80be0b4(0, buf);
        Func_8019000(box, 0x5001, 0, r, 0);
        Func_8019000(box, 0x5002, 2, r, 0);
        r2 = r + 1;
        Func_8019000(box, 0x5003, 0, r2, 0);
        Func_8019000(box, 0x5004, 2, r2, 0);
        Func_8018efc(box, buf[0] + 0x30, 1, r);
        Func_8018efc(box, buf[1] + 0x30, 3, r);
        Func_8018efc(box, buf[2] + 0x30, 1, r2);
        Func_8018efc(box, buf[3] + 0x30, 3, r2);
    }
    base[0xea6] = 0;
}
