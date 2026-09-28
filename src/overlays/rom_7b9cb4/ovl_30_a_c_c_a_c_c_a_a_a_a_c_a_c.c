/* OvlFunc_932_200a0d0
 *   [asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_c.s -- the file's
 *   ONLY function, confirmed with `grep -ci func_start` = 1.  226 instructions.
 *
 *   NO SPLIT.  `python3 tools/datacheck.py` on the .s reports no data section,
 *   and it defines no `.L` label; the three data symbols it READS
 *   (gState, iwram_3001e70, gScript_932__0200bd34) are all named globals
 *   defined elsewhere -- gScript_932__0200bd34 is `.global` at
 *   asm/overlays/rom_7b9cb4/ovl_30_c_c_c.s:5.  Landing is one .ld line
 *   (asm/ -> src/) plus deleting the .s.
 *
 *   NO FLAG GROUP.  rom_7b9cb4 has no `%` pattern rule in the Makefile and the
 *   one CSE_CFLAGS rule in that directory names a different stem
 *   (ovl_30_a_c_c_a_c_c_a_a_a_c_a_c_a_b).  objcmp prints no
 *   `(built with: ...)` line, so the generic `asm/%.o: src/%.c` and the tree
 *   default -O2 -mthumb -mthumb-interwork -fcall-used-r4 apply.]
 *
 * EXACT, measured with --whole against the original asm/ path, reproduced on
 * three consecutive runs:
 *
 *   OK whole file -- 576 bytes, 237 encodings and 45 relocations identical
 *
 * struct Actor and every callee prototype are the FILE-MATE's --
 * src/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_a_c_a_a.c -- extended with
 * f06 (short), f23, f55 and fc.  Reading that file first is most of this
 * landing: it also supplies the `sc = (int)gScript_932__0200bd34;`-adjacent-to-
 * the-call rule used below.
 *
 * ---------------------------------------------------------------- LEVERS ----
 * 1. THE gState OFFSET MUST BE BUILT, NOT FOLDED, **AND THE LOCAL BASE MUST BE
 *    BORN INSIDE THE GUARD.**  `*(short *)(gState + (0xe1 << 1))` folds to
 *    `ldr r3, =gState+450`; the recorded cure is a local `unsigned char *`
 *    base.  What is NOT recorded is that WHERE the local is assigned decides
 *    its register: written above the `if`, `g` is live across
 *    `__GetFlag(0x109)` and takes callee-saved r5, so the add comes out
 *    three-operand -- `ldr r5, =gState / add r3, r5, r2` against the ROM's
 *    `ldr r3, =gState / add r3, r2`.  Assigning it inside `if (__GetFlag(...)
 *    == 0) { ... }` makes it a caller-saved temporary and the add destructive.
 *    Worth 1 instruction and the whole opening: 25 -> 13 aligned together with
 *    lever 2.
 *
 * 2. __Func_8010704's TWO STACK ARGUMENTS ARE A NAMED PAIR, AND THE ASSIGNMENT
 *    ORDER IS PER SITE.  Bare literals make gcc reuse ONE register --
 *    `mov r3,#0x12 / str r3,[sp] / mov r3,#0xd / str r3,[sp,#4]` -- where the
 *    ROM holds both at once: `mov r2,#0xd / mov r3,#0x12 / str r3,[sp] /
 *    str r2,[sp,#4]`.  Two locals fix that, and then the ORDER of the two
 *    assignments is the ROM's emission order, which DIFFERS BETWEEN SITES:
 *      sites 1 and 2 (0x12/0xd)   `s6 = ...; s5 = ...;`  6th slot first
 *      sites 3, 4, 5 (3/0xe, 9/0xd, 0x11/0xd)  `s5 = ...; s6 = ...;`
 *    Getting all five the same way is 13 aligned; per-site is 4.  This sharpens
 *    the file-mate's "named locals apply to sites whose two slots DIFFER
 *    between sites": the pair is needed here, and the order is a per-site fact,
 *    not a per-function one.
 *
 * 3. TWO PINS OF TWO PINNABLE SITES, AND THEY ARE THE LAST FOUR ENCODINGS.
 *    In the 0x904 arm the ROM interleaves the slot constant into the middle of
 *    two shifted-constant argument builds:
 *      mov r1,#0x82 / mov r2,#0xd7 / mov r0,#0xa / lsl r1,#17 / lsl r2,#16
 *      mov r1,#0x80 / ldr r2,=gScript / mov r0,#0xa / lsl r1,#9
 *    Unpinned, sched2 puts `mov r0,#0xa` LAST at both sites.  Measured, all on
 *    the finished base (objcmp --whole, 237 encodings):
 *      PIN2 ascending at SetPos + PIN1 at 8092a1c        EXACT   <- ships
 *      PIN3 ascending at both sites                      EXACT (one pin more)
 *      PIN3 at SetPos + PIN1 at 8092a1c                  EXACT (one pin more)
 *      PIN2 at SetPos, NO pin at 8092a1c                   2 differ
 *      PIN1 at SetPos + PIN1 at 8092a1c                    4 differ
 *      no pin at SetPos + PIN1 at 8092a1c                  3 differ
 *      a bare `register int p1 __asm__("r1")` at SetPos     3 differ
 *      PIN3 DESCENDING at 8092a1c                           2 differ
 *      named locals for the two shifted arguments            4 differ
 *      `sl = 0xa;` shared by both calls                       4 differ
 *    So the minimal set is THREE pinned registers over two sites, and both
 *    sites are load-bearing -- the recorded EVICTION PINS COMPETE AND MUST BE
 *    ADDED AS A SET, again at two sites.
 *
 * 4. NO LEVER WAS NEEDED FOR THE TWO REUSED ZEROS.  The ROM stores the
 *    `__GetFlag` results in r5 / r6 and then uses those registers as the
 *    literal 0 at `actor->f55 = 0` and `actor->f06 = 0`.  Writing plain `0`
 *    produces that for free: cse's record_jump_equiv makes the flag result
 *    equivalent to 0 on the fall-through of `cmp r5,#0 / bne`.  Do not spell it.
 *
 * SHIMS, counted on the code with this comment stripped:
 *   register-pin shims: 3  -- `q0`/`r0` and `q1`/`r1` at the __MapActor_SetPos
 *     site (PIN2), `q0`/`r0` at the __Func_8092a1c site (PIN1).  Both sites are
 *     measured above; neither is a measurement aid.
 *   `__asm__(".equ ...")` shims: 0.
 *   (The `__asm__` tokens in the two `#define PIN` lines ARE those three
 *   declarations; there is no fourth.)
 */
struct Actor {
    unsigned char pad0[6];
    short f06;
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x23 - 0x18];
    unsigned char f23;
    unsigned char pad24[0x28 - 0x24];
    int f28;
    unsigned char pad2c[0x55 - 0x2c];
    unsigned char f55;
    unsigned char pad56[0x6c - 0x56];
    void (*f6c)(struct Actor *);
};

extern unsigned char gState[];
extern unsigned short *iwram_3001e70;
extern unsigned char gScript_932__0200bd34[];

extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Func_8092950(int a, int b);
extern void __Func_8092a1c(int slot, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

extern void OvlFunc_932_200ad58(void);
extern void OvlFunc_932_200ba44(void);
extern void OvlFunc_932_200b460(int slot);
extern void OvlFunc_932_200abb0(int a, int b, int c, int d);
extern void OvlFunc_932_200ace0(struct Actor *a);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")

void OvlFunc_932_200a0d0(void)
{
    struct Actor *a;
    unsigned short *b;
    unsigned char *g;
    int sc;
    int s5, s6;

    if (__GetFlag(0x109) == 0) {
        g = gState;
        if (*(short *)(g + (0xe1 << 1)) == 0x63)
            OvlFunc_932_200ad58();
    }
    OvlFunc_932_200ba44();
    if (__GetFlag(0x8fd) == 0) {
        __MapActor_SetPos(8, 0, 0);
    } else {
        a = __MapActor_GetActor(8);
        if (a != 0) {
            a->f23 = 2;
            __Actor_SetSpriteFlags(a, 0);
        }
    }
    a = __MapActor_GetActor(9);
    if (a != 0) {
        a->f23 = 2;
        __Actor_SetSpriteFlags(a, 0);
    }
    if (__GetFlag(0x8fd) == 0) {
        __Func_8092950(0xa, 2);
        if (__GetFlag(0x905) != 0) {
            __MapActor_SetAnim(9, 0);
            __MapActor_GetActor(9)->f6c = OvlFunc_932_200ace0;
            __MapActor_GetActor(9)->f55 = 0;
            __MapActor_GetActor(9)->fc = 0x80 << 14;
            s6 = 0xd;
            s5 = 0x12;
            __Func_8010704(2, 0, 1, 1, s5, s6);
            __MapActor_SetPos(0xa, 0xf0 << 15, 0xd7 << 16);
            __MapActor_GetActor(0xa)->f06 = 0;
            __MapActor_SetAnim(0xa, 3);
            OvlFunc_932_200abb0(0x82 << 16, 0, 0xa8 << 16, 0);
        } else if (__GetFlag(0x904) != 0) {
            __MapActor_SetAnim(9, 0);
            __MapActor_GetActor(9)->f6c = OvlFunc_932_200ace0;
            __MapActor_GetActor(9)->f55 = 0;
            __MapActor_GetActor(9)->fc = 0x80 << 14;
            s6 = 0xd;
            s5 = 0x12;
            __Func_8010704(2, 0, 1, 1, s5, s6);
            { PIN2; q0 = 0xa; q1 = 0x82 << 17;
              __MapActor_SetPos(q0, q1, 0xd7 << 16); }
            sc = (int)gScript_932__0200bd34;
            { PIN1; q0 = 0xa;
              __Func_8092a1c(q0, 0x80 << 9, sc); }
        }
    } else {
        b = iwram_3001e70;
        __MapActor_SetPos(0xa, 0, 0);
        s5 = 3;
        s6 = 0xe;
        __Func_8010704(0, 0, 1, 2, s5, s6);
        b[0xa] &= 0xfdff;
        OvlFunc_932_200b460(8);
        if (__GetFlag(0x80 << 2) != 0) {
            __MapActor_SetAnim(8, 5);
            s5 = 9;
            s6 = 0xd;
            __Func_8010704(7, 0xd, 1, 1, s5, s6);
            a = __MapActor_GetActor(8);
            a->fc = 0;
            a->f23 |= 2;
        }
        OvlFunc_932_200b460(9);
        if (__GetFlag(0x201) != 0) {
            __MapActor_SetAnim(9, 5);
            s5 = 0x11;
            s6 = 0xd;
            __Func_8010704(0x1d, 1, 3, 1, s5, s6);
            a = __MapActor_GetActor(9);
            a->fc = 0x80 << 14;
            a->f23 |= 2;
        }
    }
}
