/* OvlFunc_common1_1ecc -- NON-MATCHING, 3 encodings of 100 differ (97 of 100 identical).
 * SIZE IDENTICAL (100 = 100 encodings), same 12 relocations but one symbol (see LABEL).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_common/common1_1ecc.c \
 *     asm/overlays/common/common1_c_a_c_c_a_c_c.s --func OvlFunc_common1_1ecc
 *
 * Allocates the 0x7170-byte work block (__galloc_ewram id 0x3b), records the seven
 * arguments at +0xde..+0xec, mirrors actor b about actor a on x (unless flag 0x109),
 * decompresses .L5 into a 0x200 scratch buffer, claims a sprite slot, uploads, and
 * starts task OvlFunc_common1_1b08.
 *
 * BLOCKER: A sched2 TIE at the __UploadSpriteGFX argument setup, nothing else.
 *     rom   mov r1,#0x80 / lsl r0,#16 / mov r2,r11 / lsl r1,#2 / asr r0,#16
 *     ours  mov r1,#0x80 / lsl r0,#16 / mov r2,r11 / asr r0,#16 / lsl r1,#2
 * -da .23.sched2 shows the ready list {377 lsl r1, 264 asr r0} at t=83: equal
 * priority (both feed only the call), neither depends on the last-scheduled insn
 * (272, mov r2,fp), so rank_for_schedule falls through to depend_count and then
 * INSN_LUID, and the sign-extension (lower luid -- it is emitted before the size
 * constant, which is split into mov/lsl only after reload) wins. The ROM needs the
 * size constant's insns EARLIER in the stream than the (short) extension of the
 * slot, i.e. a different argument-expansion order.
 *
 * INERT (all stay at exactly these 3 encodings): passing w->fd8 vs a named
 * `int slot` with `(short)slot`; `short` vs `int` prototype for the slot parameter
 * and for __AllocSpriteSlot's return; `w->fd8 = slot = ...` chained; the whole
 * assignment as the argument; naming the size `int size = 0x200` in the entry
 * block, after the flag test, in both arms, or right before the call (the
 * documented dominating-block lever does nothing here -- the defect is a luid
 * tie, not a ready-time one). A `short slot` local is catastrophic (35 of 100).
 *
 * LABEL: `.L5` is a LOW-numbered gcc label, so `extern ... __asm__(".L5")` collides
 * with gcc's own `.L5` -- objcmp shows R_ARM_ABS32 `.text` where the ROM has `.L5`.
 * When this lands it needs the linker alias remedy (`_TBL_L5 = .L5;` in each
 * overlay.ld that lists the object, and `__asm__("_TBL_L5")` here), as
 * common1_a_a_a_a_c_c_a_a_a_b.c does for .L10.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

struct Work {
    unsigned char pad00[0xd8];
    short fd8;
    short fda;
    short fdc;
    short fde;
    short fe0;
    short fe2;
    short fe4;
    short fe6;
    int fe8;
    int fec;
};

extern unsigned char L5[] __asm__(".L5");
extern void OvlFunc_common1_1b08(void);
extern void *__galloc_ewram(int id, int size);
extern void *__Func_8004970(int size);
extern struct Actor *__MapActor_GetActor(int id);
extern int __GetFlag(int flag);
extern void __DecompressLZ(void *src, void *dst);
extern int __AllocSpriteSlot(void);
extern void __UploadSpriteGFX(int slot, int size, void *src);
extern void __StartTask(void (*f)(void), int n);
extern void __free(void *p);

void OvlFunc_common1_1ecc(int a, int b, int c, int d, int e, int f, int g)
{
    struct Work *w;
    void *buf;
    struct Actor *pa;
    struct Actor *pb;
    int slot;

    w = __galloc_ewram(0x3b, 0x7170);
    buf = __Func_8004970(0x200);
    w->fde = a;
    w->fe0 = b;
    w->fe2 = f;
    w->fe4 = g;
    w->fe6 = c;
    w->fe8 = d;
    w->fec = e;
    pa = __MapActor_GetActor(a);
    pb = __MapActor_GetActor(b);
    if (!__GetFlag(0x109)) {
        pb->f8 = d * 2 - pa->f8;
        pb->f10 = pa->f10;
    }
    w->fda = 0;
    w->fdc = 0;
    __DecompressLZ(L5, buf);
    slot = __AllocSpriteSlot();
    w->fd8 = slot;
    __UploadSpriteGFX((short)slot, 0x200, buf);
    __StartTask(OvlFunc_common1_1b08, 0xc76);
    __free(buf);
}
