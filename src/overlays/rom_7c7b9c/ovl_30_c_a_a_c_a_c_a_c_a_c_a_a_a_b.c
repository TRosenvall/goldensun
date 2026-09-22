/* Cluster OvlFunc_943_2009444..OvlFunc_943_2009444 extracted from
 * goldensun/asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_a.s.
 *
 * Total .text for this TU = 576 bytes (= 0x240). Never attempted before batch 279.
 * ONE PIN2 SITE -- one fakematch row. No flags. Plain C was already 13 differing of 212.
 *
 * 1. THE STRUCTURE. The 16-entry jump table's DEFAULT (`.L15d0`) is the code AFTER the switch,
 *    with cases 0xa..0x13 ending in `return;`. Case 0x10 is
 *    `if (__GetFlag(0x109)) OvlFunc_943_2009920(); else OvlFunc_943_200a9d4();` and gcc
 *    CROSS-JUMPS its `bl 2009920 / b end` onto the ladder's 0x92b arm -- that is `.L1604`, and it
 *    is NOT a `goto` in the source. Another instance of the recorded rule that a cross-jump is
 *    downstream of allocation rather than something to write.
 *
 * 2. `0xc8 << 4` COMMONED INTO r5 across the two __StartTask calls in the 0x927 arm needs one
 *    PIN2 -- r0 AND r1 -- at the SECOND site. r1 alone is 2 differing; r0 alone, a named local,
 *    `0xc80` as a literal, and a `do{}while(0)` barrier are all 14-16.
 *
 * 3. THE MASK IS `~0xc`, NOT `~0xd`, AND THAT COLLAPSES THE WHOLE CASE-4 BODY TO ONE BITFIELD
 *    WRITE. The ROM's `mov r3, #0xd / neg r3, r3` is -13, and -13 == ~12, so the body is
 *    `p->b23 = 1;` on `struct { unsigned char pad00[9]; unsigned char lo:2; unsigned char b23:2; }`.
 *    The expression store `p[9] = (p[9] & ~0xd) | 4;` truncates to `mov r3, #0xf2` -- the
 *    batch-278 bitfield rule again, now with the arithmetic trap attached: READ `neg` AS
 *    TWO'S COMPLEMENT BEFORE CHOOSING THE MASK.
 *
 *    And adding a second `b0 = 0` write costs three instructions in EITHER order, because Thumb's
 *    `and` is register-only: two masks forced into registers can never be combined. So a
 *    one-bitfield reading is not just tidier, it is the only one that fits.
 *
 * GETTING THE BITFIELD RIGHT MADE FOUR OTHER PIECES OF SCAFFOLDING UNNECESSARY. An incremental
 * greedy drop from the five-item scaffolded version removed a second PIN2 at 200b4bc, a PIN2 at
 * the 0x928-arm __StartTask, an `__asm__ volatile` barrier on the gState base, AND both r2/r3
 * register pins on the gState access -- all inert once the mask was correct. **A pin set that
 * looks irreducible can collapse when an unrelated reading is fixed; re-run the greedy drop after
 * every structural change, not just at the end.**
 *
 * The gState access still needs its TWO NAMED LOCALS (`gb = gState; go = 0xe1; go <<= 1;
 * gb += go;`); collapsing to `*(short *)(gState + (0xe1 << 1))` folds the address into the pool
 * (`ldr r3, =gState+450`) and is 145 differing. That idiom came from the sibling
 * ovl_30_c_a_a_c_a_c_a_c_a_c_a_a_c_a_b.c, whose write-up OVER-PRESCRIBES for this function -- it
 * also calls for pins this one does not need.
 *
 * Also confirmed: the ROM's `str r5, [r3, #4]` in the 0x928 arm is the cse1 branch-proven-zero
 * substitution, reproduced by a plain `L5b50[1] = 0;`, and r5 appearing in the push list is the
 * tell.
 */
struct Sub {
    unsigned char pad00[9];
    unsigned char lo : 2;
    unsigned char b23 : 2;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001e70;
extern int L5b50[] __asm__(".L5b50");
extern int L5b60[] __asm__(".L5b60");

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __StartTask(void *fn, int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8092950(int a, int b);

extern void OvlFunc_943_200b950(void);
extern void OvlFunc_943_200b9b8(void);
extern void OvlFunc_943_200b284(void);
extern void OvlFunc_943_200b4bc(void);
extern void OvlFunc_943_200b1a8(void);
extern void OvlFunc_943_200b710(void);
extern void OvlFunc_943_2009b58(void);
extern void OvlFunc_943_2009a98(void);
extern void OvlFunc_943_2009d0c(void);
extern void OvlFunc_943_2009db0(void);
extern void OvlFunc_943_2009f90(void);
extern void OvlFunc_943_200a2c0(void);
extern void OvlFunc_943_200a618(void);
extern void OvlFunc_943_200a9d4(void);
extern void OvlFunc_943_200ab7c(void);
extern void OvlFunc_943_200ac84(void);
extern void OvlFunc_943_200ba0c(void);
extern void OvlFunc_943_20099c0(void);
extern void OvlFunc_943_2009920(void);
extern void OvlFunc_943_200985c(void);
extern void OvlFunc_943_20097a0(void);
extern void OvlFunc_943_2009684(void);

#define PIN2 register void *q0 __asm__("r0"); \
             register int q1 __asm__("r1")

void OvlFunc_943_2009444(void)
{
    struct Sub *p;

    __SetFlag(0xa2 << 1);
    if (__GetFlag(0x109)) {
        __ClearFlag(0x271);
        __ClearFlag(0x272);
    }
    if (__GetFlag(0x93e)) {
        OvlFunc_943_200b950();
        OvlFunc_943_200b9b8();
        __Func_8092950(0x18, 2);
    } else if (__GetFlag(0x8a << 4)) {
        OvlFunc_943_200b950();
        OvlFunc_943_200b9b8();
    } else if (__GetFlag(0x927)) {
        OvlFunc_943_200b950();
        OvlFunc_943_200b284();
        __StartTask(OvlFunc_943_200b4bc, 0xc8 << 4);
        L5b50[1] = 0x80 << 14;
        { PIN2; q1 = 0xc8; L5b60[1] = 0x13333; q0 = (void *)OvlFunc_943_200b1a8; q1 <<= 4; __StartTask(q0, q1); }
    } else if (__GetFlag(0x928)) {
        OvlFunc_943_200b950();
        L5b50[1] = 0;
        L5b60[1] = 0;
        __StartTask(OvlFunc_943_200b1a8, 0xc8 << 4);
    }
    if (!__GetFlag(0x927))
        OvlFunc_943_200b710();
    {
    unsigned char *gb;
    int go;
    gb = gState;
    go = 0xe1;
    go <<= 1;
    gb += go;
    switch (*(short *)gb) {
    case 4:
        p = *(struct Sub **)(__MapActor_GetActor(0) + 0x50);
        p->b23 = 1;
        break;
    case 0xa:
        if (__GetFlag(0x928))
            OvlFunc_943_2009b58();
        else
            OvlFunc_943_2009a98();
        return;
    case 0xb:
        OvlFunc_943_2009d0c();
        return;
    case 0xc:
        OvlFunc_943_2009db0();
        return;
    case 0xd:
        OvlFunc_943_2009f90();
        return;
    case 0xe:
        OvlFunc_943_200a2c0();
        return;
    case 0xf:
        OvlFunc_943_200a618();
        return;
    case 0x10:
        if (__GetFlag(0x109))
            OvlFunc_943_2009920();
        else
            OvlFunc_943_200a9d4();
        return;
    case 0x11:
        OvlFunc_943_200ab7c();
        return;
    case 0x12:
        if (!__GetFlag(0x109))
            OvlFunc_943_200ac84();
        return;
    case 0x13:
        OvlFunc_943_200ba0c();
        return;
    }
    }
    if (__GetFlag(0x93e)) {
        *(int *)(iwram_3001e70 + 0xec) = 0x82 << 15;
    } else if (__GetFlag(0x8a << 4)) {
        OvlFunc_943_20099c0();
    } else if (__GetFlag(0x92b)) {
        OvlFunc_943_2009920();
    } else if (__GetFlag(0x928)) {
        OvlFunc_943_200985c();
    } else if (__GetFlag(0x925)) {
        OvlFunc_943_20097a0();
    } else if (__GetFlag(0x911)) {
        OvlFunc_943_2009684();
    }
}
