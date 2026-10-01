/* OvlFunc_969_200da28 -- 0x0200da28, first of the two functions in
 * asm/overlays/rom_7f6e64/ovl_314_c_c_c.s (147 instructions by the .s comment,
 * 157 encodings / 360 bytes as an object).
 *
 * NON-MATCHING: 48 encodings of 157 differ (objcmp).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/ovl_7f6e64/200da28.c \
 *       asm/overlays/rom_7f6e64/ovl_314_c_c_c.s --func OvlFunc_969_200da28
 *
 * 48 IS A TRUE DISTANCE: SIZE 360 == 360, ENCODINGS 157 == 157, and the
 * relocation list holds the same 19 symbols in the same order (the reported
 * "RELOCATIONS differ" is offsets only, shifted by the local scheduling swaps
 * below, plus the known one-symbol `_divsi3_RAM` / `__divsi3` overlay alias).
 *
 * THIS FILE CANNOT BE BUILT AS-IS -- IT NEEDS A TEXT/DATA SPLIT FIRST.
 * `python3 tools/datacheck.py asm/overlays/rom_7f6e64/ovl_314_c_c_c.s` prints,
 * verbatim:
 *
 *   asm/overlays/rom_7f6e64/ovl_314_c_c_c.s
 *       data sections : .bss, .data
 *       functions     : OvlFunc_969_200da28, OvlFunc_969_200db90
 *       EXPORTS       : gScript_969__0200dfc4, gScript_969__0200e004,
 *         gScript_969__0200e03c, gScript_969__0200e074, gScript_969__0200e088,
 *         gScript_969__0200e0ac, gScript_969__0200e0d0, gScript_969__0200e0f4,
 *         gScript_969__0200e130, gScript_969__0200e16c, gScript_969__0200e22c,
 *         gScript_969__0200e324, gScript_969__0200e360, gScript_969__0200e39c,
 *         gScript_969__0200e3c0, gOvl_0200e464, gOvl_0200e478, .L66e8,
 *         gOvl_0200e6ec, gOvl_0200e3d4, gScript_969__0200e1cc,
 *         gScript_969__0200e2d0, gScript_969__0200e734, .L6760, .L6764
 *       -> converting a function here needs a TEXT/DATA SPLIT; the data must
 *          keep its own object.
 *
 * Per batch 290's rule, the split must export every label crossing the new
 * object boundary IN EITHER DIRECTION, not only the 25 already `.global`:
 * this function reads `gScript_969__0200e734` (already exported) and the
 * file-mate OvlFunc_969_200db90 stays in assembly and reads its own labels.
 *
 * WHAT THE FOUR LOAD-BEARING CONSTRUCTS ARE (each measured, from 131 of 157):
 *
 * 1. `sp = (short *)(base + 0xe8); ... sp[1]` -- a NAMED short pointer at
 *    +0xe8 indexed [1].  Written `((short *)(base + 0xe8))[1]` gcc folds to
 *    `adds r5, #0xea` with a zero index; the ROM's `adds r5,#0xe8 /
 *    movs r0,#2 / ldrsh r3,[r5,r0]` needs the pointer to be a value.  This is
 *    the ldrsh register-offset rule with a NON-ZERO offset -- the batch 290
 *    caution is about supplying the ZERO; supplying the BASE is required.
 *
 * 2. `int r = __Random() & 0xffff000; t->f64 = r;` -- an `int` CARRIER FOR THE
 *    MASK.  Written straight into the `unsigned short` field, combine
 *    distributes the truncation and pools `0x0000f000`; the ROM pools
 *    `0x0ffff000`.  Same family as the `field |= 0xffff` note.
 *
 * 3. ONE VARIABLE FOR THE ANGLE, AND THE DIVIDE SPLIT FROM THE SHIFT:
 *        off = off / 0x60000;
 *        off <<= 16;
 *    A separate `ang` local costs 2 instructions and puts the value in r6;
 *    `off = (off / 0x60000) << 16;` in one statement is 2 instructions SHORT.
 *    The ROM's `mov r8,r0 / mov r1,r8 / lsl r1,#16 / mov r8,r1` is exactly a
 *    HIGH-register pseudo updated in place: Thumb `lsl` needs a LO_REGS
 *    destination, so the in-place `<<=` on an r8 pseudo has to round-trip.
 *    THAT ROUND TRIP IS THE EVIDENCE THE VALUE IS ONE VARIABLE IN A HIGH REG.
 *
 * 4. `struct HalfWord { unsigned short v; }; z.v = 0; sub->f26 = z.v;` --
 *    docs/elevation.md's "A POOLED ZERO REACHING A `strb` IS THE
 *    `struct HalfWord` CASE", applied as written; gcc emits `ldrh r4, .L12`
 *    and the mid-function pool the ROM has.  `z.v = 0;` must be assigned
 *    EARLY (here just before the `__Random` mask): at the store it is 50 of
 *    157, before `sub = t->f50` it is 87.
 *
 * Also load-bearing and unsurprising: the `f = iwram_3001e40 & 0xf;` variable
 * is REUSED as the zero for `t->f55` and `t->f66` (the ROM's own idiom -- r6
 * is the tested value, and gcc cannot know it is 0), `sub = t->f50;` is read
 * BEFORE `__Actor_SetScript` (the ROM's `ldr r5,[r7,#0x50]` sits above the
 * call, so the source cannot re-read it after), and both GetActor(0x17) calls
 * inside the two arms are written in BOTH arms.
 *
 * THE BLOCKER: RELOAD-REGISTER ROTATION FOR THE r10 BASE, PLUS THREE SCHED2
 * SLOT SWAPS.  `a` lives in r10 (sl) across the whole function, so every
 * `a->fN` read needs a LO copy.  Both ROM and candidate make exactly TWO such
 * copies and issue exactly three loads; they differ only in WHICH registers:
 *
 *     ROM   mov r1,sl / cmp r1,#0 ... mov r0,sl / ldr r2,[r0,#12]
 *                                    / ldr r1,[r1,#8] / ldr r3,[r0,#16]
 *     ours  mov r3,sl / cmp r3,#0 ... mov r4,sl / ldr r2,[r4,#12]
 *                                    / ldr r1,[r4,#8] / ldr r3,[r4,#16]
 *
 * i.e. the ROM's compare copy SURVIVES to the `->f8` load and a second copy
 * serves `->fc` and `->f10`, where gcc lets one copy serve all three and
 * spends the compare copy on r3.  This is the web-session handoff's
 * "reload registers rotate" class (allocate_reload_reg, reload1.c:5003,
 * `last_spill_reg`), with REG_ALLOC_ORDER {3,2,1,0,...}: the ROM used r0/r1,
 * which means r2 and r3 were live at those points and gcc's are not.  The same
 * one-register rotation accounts for `movs r0,#2` vs `movs r2,#2` at the
 * `ldrsh`, `mov r1,r8` vs `mov r2,r8` at the shift round-trip, and `mov r1,sl`
 * vs `mov r4,sl` at `t->f68`.
 *
 * The three remaining slot swaps are sched2, one instruction each and in both
 * directions, so no statement order reaches them:
 *   - `movs r5,#128` scheduled ABOVE `bl __MapActor_GetActor` (ROM: below)
 *   - `adds r7,r0,#0` / `lsls r1,r1,#11` exchanged after `__CreateActor`
 *   - the mid-function pool dump splits the last block six instructions
 *     earlier than the ROM's, so `strb r4,[r3]` lands after the pool instead
 *     of before the `b` over it.  The pool CONTENTS and ORDER are already the
 *     ROM's (0, 0, 0, 0x0ffff000, 0x000fffff, 0) and the HImode fixup that
 *     forces a mid-function dump is present -- only the split point differs.
 *
 * MEASURED LADDER (differing encodings of 157; shipped is 48)
 *
 *   spelling                                                       differing
 *   -------------------------------------------------------------  ---------
 *   SHIPPED (this file) ..........................................        48
 *   `z.v = 0;` at the store instead of before the mask ...........        50
 *   `z.v = 0;` right after `sub = t->f50;` .......................        87
 *   separate `ang` local, one-statement divide+shift .. (+2 insns)       124
 *   ... and `t->f64 = __Random() & 0xffff000;` direct .. (+2 insns)       131
 *   `((short *)(base + 0xe8))[1]` folded .............. (+2 insns)       131
 *
 * The three rows marked (+2 insns) are 364 bytes against 360, so those counts
 * are NOT distances; only 48, 50 and 87 are.
 *
 * NO .sym CANDIDATE. Every constant here is a literal or an already-exported
 * label; nothing is reached by register arithmetic off a neighbouring id.
 */
struct HalfWord { unsigned short v; };

struct Sub {
    unsigned char pad0[9];
    unsigned char f9_lo : 2;
    unsigned char f9_sel : 2;
    unsigned char f9_hi : 4;
    unsigned char pada[0x26 - 0xa];
    unsigned char f26;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x18 - 0x14];
    int f18;
    int f1c;
    unsigned char pad20[0x30 - 0x20];
    int f30;
    unsigned char pad34[0x50 - 0x34];
    struct Sub *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    unsigned short f64;
    unsigned short f66;
    struct Actor *f68;
    void *f6c;
};

extern unsigned char *iwram_3001e70;
extern int iwram_3001e40;
extern unsigned char gScript_969__0200e734[] __asm__("gScript_969__0200e734");

extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern unsigned int __Random(void);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetScript(struct Actor *a, void *script);
extern void __Func_80929d8(struct Actor *a, int n);
extern int __sin(int a);
extern void OvlFunc_969_200db90(void);

void OvlFunc_969_200da28(void)
{
    struct HalfWord z;
    struct Actor *a;
    struct Actor *b;
    struct Actor *t;
    struct Sub *sub;
    unsigned char *base;
    short *sp;
    unsigned int u;
    int off;
    int w;
    int f;
    int r;

    a = __MapActor_GetActor(0x17);
    base = iwram_3001e70;
    sp = (short *)(base + 0xe8);
    u = __Random() * 0x30;
    off = (u >> 16) << 16;
    if (sp[1] <= 0x81) {
        if (iwram_3001e40 & 1) {
            __MapActor_SetPos(0x17, 0x98 << 17, 0xa4 << 16);
            b = __MapActor_GetActor(0x17);
            w = 0x80 << 9;
        } else {
            __MapActor_SetPos(0x17, 0x98 << 17, 0xab << 16);
            b = __MapActor_GetActor(0x17);
            w = 0x14ccc;
        }
        b->f18 = w;
        __MapActor_GetActor(0x17)->f1c = w;
    } else {
        __MapActor_SetPos(0x17, 0, 0);
    }
    if (a == 0)
        return;
    f = iwram_3001e40 & 0xf;
    if (f != 0)
        return;
    t = __CreateActor(0x8e << 1, a->f8 + 0x80000, a->fc + off + 0x80000, a->f10);
    off = off / 0x60000;
    off <<= 16;
    if (t == 0)
        return;
    sub = t->f50;
    __Actor_SetScript(t, gScript_969__0200e734);
    __Func_80929d8(t, 5);
    t->f55 = f;
    z.v = 0;
    r = __Random() & 0xffff000;
    t->f64 = r;
    t->f66 = f;
    t->f68 = a;
    t->f6c = OvlFunc_969_200db90;
    t->f30 = (__sin((off & 0xfffff) >> 4) * 24) >> 16;
    sub->f26 = z.v;
    sub->f9_sel = a->f50->f9_sel;
}
