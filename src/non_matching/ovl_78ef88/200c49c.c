/* OvlFunc_896_200c49c -- NON-MATCHING, 337 of 360 encodings differ.
 *
 * ===== BATCH 319: tools/dupfuncs.py PAIRS THIS WITH A TWIN, AND THE TWIN IS 38 CLOSER =====
 *
 *   this park   OvlFunc_896_200c49c         337 of its reference
 *   the twin    OvlFunc_897_200b01c         299   (ovl_791794/200b01c.c)
 *
 * Batch 316 established that a duplicate group is A TRANSFER OPPORTUNITY WITH
 * WORK ATTACHED, not a shared fix: a twin measured 181 of 177 (saturated, figure
 * withdrawn) while the solved body ported across with three renames read 5 of
 * 179 -- the identical residue.  So the better body here is worth 38 encodings
 * to this function, FOR THE COST OF A PORT PLUS ONE MEASUREMENT.
 *
 * BUT THE PORT IS NOT THREE RENAMES THIS TIME, and that was checked, not assumed:
 * the two bodies reference DIFFERENT NUMBERS OF DATA LABELS, so there is no clean
 * symbol mapping and dupfuncs' "duplicate" verdict is about the NORMALISED
 * INSTRUCTION STREAM, not the data references.  Expect to map the label and pool
 * order by hand against both references.  Do the port, then MEASURE -- a twin's
 * figure describes a different body until you have.
 *   [asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c_c.s, 1st of 2 functions]
 *   Unattempted before batch 300e.  Draft = scratch_elev/b300e/fc49c/x1.c.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_78ef88/200c49c.c \
 *       asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c_c.s --func OvlFunc_896_200c49c
 *
 * READ THE SHAPE BEFORE THE NUMBER.  SIZE IS EXACT (752 bytes both), the
 * ENCODING COUNT IS EXACT (360 both), the FRAME IS EXACT (`sub sp, #0x44`), ALL
 * SEVENTEEN SPILL SLOTS LAND ON THE ROM'S OFFSETS, and the relocation list is
 * the ROM's in the same order and the same count -- the only symbol difference
 * is `__udivsi3` against `_udivsi3_RAM`, which overlays/rom_78ef88/overlay.ld:72
 * aliases (`__udivsi3 = _udivsi3_RAM;`), so it is NOT a residue.
 *      first at index 12: ref 9010 (str r0,[sp,#0x40])  ours 2100 (mov r1,#0)
 * objcmp's 337 is index-aligned and is SATURATED BY A TWO-INSTRUCTION INSERT AT
 * INDEX 12: everything after the preheader is shifted by one.  The honest
 * distance is the ALIGNED one -- 214 of 354 instructions aligned-equal (60.5%),
 * 140 differing/inserted/deleted in 43 hunks.  DO NOT read 337 as the distance
 * and DO NOT read a future 337 as "no progress".
 *
 * ############ WHAT THIS DRAFT ALREADY GETS RIGHT -- DO NOT REDERIVE ##########
 *
 * THE STRUCT.  __galloc_ewram(0x21, 0xca << 1) returns 0x194 bytes =
 * `struct { struct E e[10]; unsigned short n; }` with `struct E` EXACTLY 0x28:
 * actor pointer at 0, three ints at 4/8/0xc, five ints at 0x10..0x20, and two
 * unsigned chars at 0x24 (a 3-frame countdown) and 0x25 (a bounce phase).  The
 * sibling OvlFunc_896_200c78c in the SAME .s is the constructor and proves it:
 * it strides 0x28, writes .L5140[i] to +0x1c, -.L5168[i] to +0x20, 3 to +0x24,
 * and the element count to base+0x190.  Both tables are ten ints (0x5140..0x5190
 * of orig.bin); gOvl_0200d0e4, .L5102 and gScript_936__0200d120 are ten TRIPLES
 * of bytes (the last signed: values 1/0/-1).
 *
 * THE SECOND POINTER IS REAL.  The ROM walks the array with TWO cursors: `c` at
 * base+0x28*i (fields 0, 4, 8, 0xc) and r10 at base+8+0x28*i, dereferenced at
 * +0, +8, +0xc, +0x10, +0x14, +0x18 (= element +8, +0x10 .. +0x20).  Element +8
 * is LOADED through `c` and STORED through r10.  Modelled here as a second
 * struct `struct M` (seven ints) pointing at &e[i].f8; that reproduces every one
 * of those eleven accesses.
 *
 * THREE LEVERS WERE WORTH THE WHOLE DISTANCE AND ALL THREE ARE MEASURED:
 *
 * 1. THE DIVISION'S ELSE ARM NEEDS ITS OWN DESTINATION VARIABLE.  The ROM has
 *    `b .Lx / mov r6,#0` on each of the three `(n << 16) / 1000` guards.
 *    Written `if (u != 0) u = (u << 16) / 1000; else u = 0;` gcc DELETES the
 *    else arm -- cse's jump equivalence knows u == 0 there -- and the function
 *    comes out SIX INSTRUCTIONS SHORT.  Written into a fresh `du` the store
 *    survives and coalesces onto u's register, which is what the ROM has.
 *    This took 353 encodings to the ROM's 360.  (General: a redundant-looking
 *    `else x = 0;` that survives in the ROM names a SECOND variable.)
 *
 * 2. THE `+ 1` ON A DATA SYMBOL MUST BE NAMED OR IT GOES INTO THE POOL WORD.
 *    `pd = gOvl_0200d0e4 + 1;` pools `gOvl_0200d0e4+1` as one word; the ROM has
 *    `ldr r0, =gOvl_0200d0e4 / add r0, #1`.  `one = 1; pd = gOvl_0200d0e4 + one;`
 *    restores the add AND the zero-addend relocation.  Same rule as the recorded
 *    gState-offset idiom, with the offset equal to 1.
 *
 * 3. THE FIVE LOW SPILL SLOTS BELONG TO AN INNER SCOPE.  gcc assigns spill slots
 *    in PSEUDO-NUMBER order, highest frame offset first, and pseudo numbers come
 *    from the order in which each BLOCK's declarations are expanded.  The ROM's
 *    order is b, c, i, x, y, z, vx, vy, t, px, py, pz (0x40 down to 0x14) and
 *    then p24, p25, j, pd, ps (0x10 down to 0).  Declaring the five cursors in
 *    the `if (b->n != 0) { ... }` BLOCK, in that order, puts all seventeen slots
 *    on the ROM's offsets in one step.  Declaring them at the top of the
 *    function instead puts the cursors at 0x38..0x2c and moves every other slot.
 *    THIS IS A NEW LEVER: an inner-scope declaration is a SPILL-SLOT ORDERING
 *    lever, not just a scoping choice.
 *    (Measured inert: expressing the cursors as index expressions -- b->e[i].f24,
 *    gOvl_0200d0e4[i*3+1], .L5102[i*3+2] -- so that loop.c makes them GIVs.  It
 *    gets the same slot ORDER for the body locals but manufactures the WRONG giv
 *    set: gcc reaches e[i].f25 as p24[1] and builds three index givs 0/1/2 where
 *    the ROM has p24, p25, j, pd, ps.  748 bytes, 358 encodings.)
 *
 * ALSO ALREADY RIGHT: `__Random` declared UNSIGNED so the (random * byte) >> 16
 * comes out `lsr`; `t` and `g` as `unsigned char`, which is why `*p24 = t` needs
 * `add r0, sp, #0x20 / ldrb` (Thumb has no sp-relative `ldrb`); the countdown
 * body guarded by `if (--t == 0)`; the bounce arm's polarity
 * (`if (vx >= .L5140[i]) ... else if (vx <= 0x1999) ...`, from `blt`/`bgt`);
 * `a->f8 = 0` commoning onto g's register inside `if (g == 0)`; and the loop as
 * `if (b->n != 0) do { ... } while (i != b->n);`.
 *
 * ################## THE RESIDUE, AND IT IS ONE QUESTION ######################
 *
 * TWO REGISTER DECISIONS, AND EVERYTHING ELSE IS THEIR KNOCK-ON.
 *
 * (a) THE PREHEADER.  The ROM RELOADS the base pointer for the `b->n` test:
 *        ref    str r0,[sp,#0x40] / str r0,[sp,#0x3c] / ldr r1,[sp,#0x40] /
 *               mov r0,#0 / mov r2,#0xc8 / str r0,[sp,#0x38] / ... add r3,r1,r2
 *        ours   mov r1,#0 / mov r2,#0xc8 / str r0,[sp,#0x40] / str r0,[sp,#0x3c]
 *               / str r1,[sp,#0x38] / ... add r3,r0,r2
 *     The ROM's constant zero takes r0, which ANTI-DEPENDS on the two stores and
 *     pins it after them; ours takes r1 and sched2 hoists it above them.  r0 is
 *     free in the ROM because reload did NOT inherit the base in r0, so the zero
 *     got the 4th register in REG_ALLOC_ORDER {3,2,1,0,...} and the base needed
 *     r1.  Ours inherits the base in r0, so the zero gets r1 -- 3rd.  The ROM
 *     then REUSES that r0 zero for `j = 0` across the branch (`str r0,[sp,#8]`)
 *     and reuses the reloaded r1 for `m = base + 8` (`add r1,#8`).
 *     BLOCKER CLASS: RELOAD INHERITANCE / SPILL-REGISTER CHOICE, which no source
 *     spelling in this sweep reached.
 *
 * (b) r9 AND r11 ARE SWAPPED.  ref g->r11, dv->r9;  ours g->r9, dv->r11.
 *     Both builds use exactly r5,r6,r7,r8,r9,r10,r11 and agree on r5 (the actor),
 *     r6 (u then sin x), r7 (v then sin y), r8 (the third random), r10 (the
 *     second cursor).  global.c's `allocno_compare` priority is
 *     floor_log2(n_refs) * freq / live_length: g has 7 refs and dv 6, so both sit
 *     in the SAME log2 bucket (2) and the longer-lived g loses.  Raising g to 8
 *     refs or dropping dv to 3 would flip it; neither is honest source here.
 *
 * MEASURED AND EXACT TIES (each re-run against the draft, none is the answer):
 *   * all five declaration permutations of g / u,v,w / du,dv,dw -- the swap in
 *     (b) is a PRIORITY decision, not an allocno-number tie, so declaration
 *     order is completely inert on it;
 *   * `b->e[i].f25 = g` against `*p25 = g` (and the same for f24) -- the giv and
 *     the explicit cursor are byte-identical here;
 *   * `while (i != b->n) { ... }` in place of the rotated `if` + `do/while`;
 *   * `j = i;` in place of `j = 0;` (it does NOT common the zero into r0);
 *   * swapping the mul operands at the three __sin/__cos sites fixes two of the
 *     three `mov r0,rX / mul r0,rY` pairs and is an exact tie overall;
 *   * moving `i++` ahead of `p24 += 0x28` in the increment run.
 * MEASURED WORSE: `vy = .L5168[i]` before `vx = 0x1999` (+1, though it is what
 * forces the ROM's SECOND `ldr =0x1999` -- the compare's r3 is clobbered by the
 * table address); testing `v != 0` instead of `dv != 0` (744 bytes, 356); `t` as
 * `int` (744 bytes, 356, 347 differing).
 *
 * LANDING SHAPE WHEN CLOSED.  tools/datacheck.py exits 1 on this .s: it carries
 * `.section .data` with ten incbin symbols, so landing needs a TEXT/DATA SPLIT
 * -- the data keeps its own object, OvlFunc_896_200c78c stays as assembly, and
 * the split MUST ADD `.global .L5102`, `.global .L5140` and `.global .L5168`
 * (the file already exports `.L5088` the same way, so the shape is precedented).
 * The C reaches them with the asm-label extension, e.g.
 * `extern int L5140[] __asm__(".L5140");` -- all three numbers are high enough
 * not to collide with gcc's own `.LN`.  Function symbols need no export:
 * include/macros.inc's `.thumb_func_start` expands through
 * `.thumb_func_start_noalign`, which emits `.global` itself.
 * overlay.ld ALREADY SPLITS THIS STEM'S SECTIONS: rom_78ef88/overlay.ld names
 * `asm/overlays/rom_78ef88/ovl_314_c_c_c_c_c_c.o(.text)` at line 48 and the SAME
 * object's `(.data)` at line 58, so the asm object keeps BOTH rows and the new
 * `src/overlays/rom_78ef88/...o(.text)` row goes in front of line 48.  The stem
 * is named by 4 overlay.ld rows in 2 overlay.ld files (rom_78ef88 and
 * rom_793768, .text + .data each); 4 files in overlays/ mention it in total.
 * NO PINS, NO FAKEMATCH ROW: tools/shimcount.py reports zero shims.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x18 - 0x14];
    int f18;
    int f1c;
    unsigned char pad20[0x38 - 0x20];
    int f38;
    int f3c;
    int f40;
};

struct M {
    int g0;
    int g4;
    int g8;
    int gc;
    int g10;
    int g14;
    int g18;
};

struct E {
    struct Actor *actor;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    unsigned char f24;
    unsigned char f25;
    unsigned char pad26[2];
};

struct Blob {
    struct E e[10];
    unsigned short n;
};

extern void *__galloc_ewram(int tag, int size);
extern unsigned int __Random(void);
extern int __sin(int a);
extern int __cos(int a);
extern unsigned char gOvl_0200d0e4[];
extern signed char gScript_936__0200d120[];
extern unsigned char L5102[] __asm__(".L5102");
extern int L5140[] __asm__(".L5140");
extern int L5168[] __asm__(".L5168");

void OvlFunc_896_200c49c(void)
{
    struct Blob *b;
    struct E *c;
    int i;
    int x, y, z;
    int vx, vy;
    unsigned char t;
    int px, py, pz;
    struct M *m;
    struct Actor *a;
    unsigned char g;
    unsigned int u, v, w;
    unsigned int du, dv, dw;
    int s;
    int sx, sy, cz;
    int one;

    b = __galloc_ewram(0x21, 0xca << 1);
    c = b->e;
    i = 0;
    if (b->n != 0) {
        unsigned char *p24;
        unsigned char *p25;
        int j;
        unsigned char *pd;
        unsigned char *ps;

        m = (struct M *) &b->e[0].f8;
        p24 = &b->e[0].f24;
        p25 = &b->e[0].f25;
        one = 1;
        pd = gOvl_0200d0e4 + one;
        j = i;
        ps = L5102;
        do {
            x = m->g8;
            a = c->actor;
            y = m->gc;
            z = m->g10;
            vx = m->g14;
            vy = m->g18;
            g = *p25;
            px = c->f4;
            py = c->f8;
            pz = c->fc;
            t = *p24;
            t--;
            if (t == 0) {
                t = 3;
                if (g == 0) {
                    vx += vy;
                    if (vx >= L5140[i]) {
                        vy = -L5168[i];
                    } else if (vx <= 0x1999) {
                        vx = 0x1999;
                        vy = L5168[i];
                        px = a->f8;
                        py = a->fc;
                        pz = a->f10;
                        a->f8 = 0;
                        a->fc = 0;
                        a->f10 = 0;
                        g = 0x18;
                    }
                    a->f18 = vx;
                    a->f1c = vx;
                }
                u = (__Random() * gOvl_0200d0e4[j]) >> 16;
                v = (__Random() * pd[0]) >> 16;
                w = (__Random() * pd[1]) >> 16;
                if (u != 0)
                    du = (u << 16) / 1000;
                else
                    du = 0;
                if (v != 0)
                    dv = (v << 16) / 1000;
                else
                    dv = 0;
                if (w != 0)
                    dw = (w << 16) / 1000;
                else
                    dw = 0;
                s = gScript_936__0200d120[j];
                if (s == 1) {
                    x += du;
                } else {
                    x -= du;
                    if (s != -1)
                        x = 0;
                }
                s = gScript_936__0200d120[j + 1];
                if (s == 1) {
                    y += dv;
                } else {
                    y -= dv;
                    if (s != -1)
                        y = 0;
                }
                s = gScript_936__0200d120[j + 2];
                if (s == 1) {
                    z += dw;
                } else {
                    z -= dw;
                    if (s != -1)
                        z = 0;
                }
                sx = __sin(x * ps[0]) << 1;
                sy = __sin(y * ps[1]) << 1;
                cz = __cos(z * ps[2]) << 1;
                if (g != 0) {
                    px += sx;
                    py += sy;
                    pz += cz;
                    g--;
                    if (g == 0) {
                        a->f8 = px;
                        a->f38 = px;
                        if (dv != 0) {
                            a->fc = py;
                            a->f3c = py;
                        }
                        a->f10 = pz;
                        a->f40 = pz;
                    }
                } else {
                    a->f8 += sx;
                    a->f38 = a->f8;
                    if (dv != 0) {
                        a->fc += sy;
                        a->f3c = a->fc;
                    }
                    a->f10 += cz;
                    a->f40 = a->f10;
                }
            }
            m->g8 = x;
            m->gc = y;
            m->g10 = z;
            m->g14 = vx;
            m->g18 = vy;
            *p25 = g;
            c->f4 = px;
            m->g0 = py;
            c->fc = pz;
            *p24 = t;
            ps += 3;
            j += 3;
            pd += 3;
            i++;
            p24 += 0x28;
            p25 += 0x28;
            c++;
            m = (struct M *) ((unsigned char *) m + 0x28);
        } while (i != b->n);
    }
}
