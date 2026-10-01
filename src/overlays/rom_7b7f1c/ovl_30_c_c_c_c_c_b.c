/* OvlFunc_930_20091b0 -- *** EXACT AT THE TREE DEFAULT ***, 744 bytes, 302
 * encodings and 59 relocations identical, stable over three repeats.
 * 288 instructions.  NO PER-FILE COMPILER FLAG IS NEEDED ANY MORE.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/overlays/rom_7b7f1c/ovl_30_c_c_c_c_c_b.c \
 *     asm/overlays/rom_7b7f1c/ovl_30_c_c_c_c_c_b.s
 *   OK OvlFunc_930_20091b0 -- 744 bytes, 302 encodings and 59 relocations identical
 * (One function; no --func needed.  Before the split, measure against
 *  asm/overlays/rom_7b7f1c/ovl_30_c_c_c_c_c.s.)
 *
 * ============ REQUIREMENT 1 IS RETIRED.  IT WAS NEVER A FLAG. ============
 * The previous revision parked this file because landing it needed "THREE
 * SEPARATE DECISIONS, AND ONE OF THEM IS NOT MINE TO MAKE" -- a per-file
 * `ALIAS_CFLAGS` row, held open across several batches because a flag row is a
 * claim about how the ORIGINAL was compiled.  THERE IS NO CLAIM TO MAKE.
 * `-fno-strict-aliasing` was a GLOBAL WAY OF SPELLING A LOCAL FACT, and the
 * local spelling is ordinary C:
 *
 *     *(int *)(iw + off) = 0x80 << 1;
 *  -> ((union Pun *)(iw + off))->i = 0x80 << 1;        with  union Pun { int i; short h; };
 *
 * The park's "No source spelling moved it" was true only of the three spellings
 * it tried -- an `iw` local, dropping `gs`, an `__asm__ volatile("")` barrier --
 * none of which touches an ALIAS SET.
 *
 * ============== WHY, READ OFF THE DUMPS RATHER THAN GUESSED ==============
 * The whole 5-encoding residue was ONE instruction in the wrong slot:
 * `ldr r5,=gState` (insn 25) issued at position 4 instead of 8, rotating four
 * neighbours.  **`.19.flow2` PRINTS EVERY MEM'S ALIAS SET AS A BARE TRAILING
 * INTEGER**, so whether two accesses conflict is a READ, not an experiment:
 *
 *     insn 22  (set (mem:SI (plus (reg r1) (reg r2)) 7) (reg r3))         <- store, set 7 (int)
 *     insn 30  (set (reg r2) (sign_extend (mem:HI (plus (reg r5) (reg r2)) 10)))
 *                                                                        <- load,  set 10 (short)
 *
 * 7 != 10 and neither is 0, so they do not conflict and the only 22 -> 30 link
 * is an ANTI dependence on the r3 that insn 30 clobbers.  An anti dependence
 * COSTS 0 (`insn_cost`: `if (REG_NOTE_KIND (link) != 0) cost = 0`), so the whole
 * chain above the store comes out 2 cycles short of the ROM's.
 *
 * DIFFING THE sched2 TABLES AT DEFAULT AGAINST -fno-strict-aliasing ISOLATES THE
 * AXIS WITH NO AMBIGUITY -- **THE DEPENDENCE SETS ARE BYTE-IDENTICAL AND ONLY
 * THE PRIORITIES MOVE**:
 *
 *     insn        10   12/852/854   853/855   22    25    30/33
 *     default      7       5           4       3     5      3
 *     -fno-s-a     9       7           6       5     5      3
 *
 * Everything reaching the load THROUGH the store gains exactly 2; insn 25 does
 * not, because it reaches the load directly.  At default 25 ties 854 on priority
 * and wins the lower rungs, so it issues at t=5.  Make the pair conflict and 854
 * goes to 7, outranks 25 ON PRIORITY, and 25 falls back to its ROM slot.
 * ONE LINK'S COST, worth five encodings.
 *
 * NOTE WHICH RUNG THIS IS.  The store PRECEDES the load, so the manufactured
 * dependence is a TRUE read-after-write one and the lever acts on PRIORITY,
 * leaving every dependent count untouched.  The mirror case -- load before store
 * -- yields an ANTI dependence that adds a DEPENDENT and leaves priority
 * untouched.  So the store/load ORDER decides WHICH RUNG of
 * rank_for_schedule (priority -> class vs last_scheduled_insn -> dependent count
 * -> INSN_LUID) the lever reaches, not whether it works.
 *
 * ============ CONTROLS: IT IS THE UNION, NOT THE COMPONENT REF ============
 *     union member store    ((union Pun *)(iw+off))->i = ...     0  *** EXACT ***
 *     union member load     ((union Pun *)(gs+off))->h           0  *** EXACT ***
 *     both at once                                               0  *** EXACT ***
 *     STRUCT member store   ((struct IPun *)(iw+off))->i = ...   5  INERT
 *     STRUCT member load    ((struct SPun *)(gs+off))->h         5  INERT
 *     `unsigned int` store instead of `int`                      5  INERT
 *     `signed short` load instead of `short`                     5  INERT
 *     a `(void *)` cast before the short cast                    5  INERT
 *     CONTROL: the union/struct TYPES declared, no access changed 5  INERT
 * Not a component ref, not a qualifier, not an unsigned variant, not the type
 * declarations: specifically **a COMPONENT_REF WHOSE BASE IS A UNION**, which
 * `c_get_alias_set` answers with **0**, and alias set 0 conflicts with
 * everything.  Pass two taught us to REMOVE an accidental `alias set 0`; this is
 * the first recorded case of DELIBERATELY MANUFACTURING one at a single named
 * access.  THE STORE SIDE IS CHOSEN HERE because it is the function's ONLY
 * `int` store to iwram, whereas the halfword read has four siblings and punning
 * one of four would read arbitrary; the load side is equally exact if preferred.
 *
 * ================= WHAT IS STILL OPEN -- TWO ITEMS, NEITHER MINE =================
 * 2. `_AREA_58 = 0x58;` ADDED TO area.sym.  A documented HOLE: _AREA_57 at
 *    area.sym:162, _AREA_59 at :168, 0x58 absent.  The value is read from
 *    gState+0x1C0 -- exactly the halfword area.sym documents -- and compared
 *    directly rather than passed to __GetFile, so THE NAMESPACE IS CHECKED.
 *    IN-FUNCTION CONTROL: 0x58 and 0x4a are the only two `ldr rN, =<value <=
 *    0xff>` here, while the function emits `mov r0,#0xa9`, `mov r3,#0x15`,
 *    `mov r2,#0x49`, `mov r0,#0xb` and `mov r0,#0x80 / lsl #2` as plain
 *    literals.  SECOND CONTROL: _AREA_4a is already in area.sym and is
 *    referenced the same way by the file-neighbour ovl_30_c_c_c_c_b.c.
 *    MEASURED: with both symbol references, exact; with plain 0x58/0x4a
 *    literals, 732 bytes / 298 encodings / 288 differing.
 *    THE TWO `__asm__(".equ ...")` LINES BELOW ARE MEASUREMENT-ONLY -- they let
 *    objcmp assemble the candidate standalone and MUST NOT LAND.
 *
 * 3. A TEXT/DATA SPLIT.  `split_s.py --dry-run` is clean and names the paths:
 *      asm/overlays/rom_7b7f1c/ovl_30_c_c_c_c_c.s
 *        -> ..._b.s (the function, 311 lines) and ..._c.s (the data, 40 lines)
 *      then src/overlays/rom_7b7f1c/ovl_30_c_c_c_c_c_b.c
 *    CORRECTION TO THE PARK: it reported "1984 bytes across 14 .incbin blocks"
 *    and "All 12 labels"; split_s reads **13 blob(s) and 8 label(s)**, all of it
 *    AFTER the code.  And tools/datacheck.py adds the fact that actually settles
 *    the export gate -- `OvlFunc_930_20091b0 reads no data label -> split needs
 *    NO new export`.  The 12 globals the park counted are IRRELEVANT to the gate
 *    rather than satisfying it.
 *    overlay.ld names this .o TWICE -- line 42 in .text and line 48 in .data, as
 *    the SECOND of two .data entries -- so the .data line must be repointed IN
 *    THE SAME SLOT so addresses do not move.
 *
 * ===================== PINS: 22, AND ALL 22 MUST STAY =====================
 * Re-derived against THIS baseline, not inherited.  Per-site drops, and grouped
 * by the value they materialise per the interacting-pins law (sites C and G
 * materialise the IDENTICAL triple 9 / 0xa4<<16 / 0x8c<<17; E+F share `q2 = 0`;
 * B+E share `q0 = 8`; D+F share `q0 = 0xa`), each group removed AS A UNIT:
 *     singles      A 3 | B 3 | C 3 | D 3 | E 2 | F 2 | G 2
 *     groups       C+G 5 | E+F 4 | B+E 5 | D+F 5
 *     all seven    18          six (B..G)  15
 *     GF r0 pins   g1 192 | g2 170 | g3 86 | all three 190  (RELOCDIFF, +4..8 bytes)
 * So the park's "all seven cost 2-5" and its three-of-twelve GF finding BOTH
 * survive the structural change.  22 pins is minimal; it needs a fakematch row:
 *     OvlFunc_930_20091b0  src/overlays/rom_7b7f1c/ovl_30_c_c_c_c_c_b.c
 *
 * THE OTHER LEVERS are unchanged from the previous revision and were re-checked
 * against this baseline: seven PIN3 blocks with ascending whole-value fill;
 * three r0 pins on `__GetFlag(0x8b2)`; separate stack-argument locals per call
 * site; `int h` from `*(unsigned short *)p` so the subtraction happens in SImode;
 * and `gs` live for sites 2-3 with a re-materialised `&gState` for sites 4-5.
 */
__asm__(".equ _AREA_4a, 0x4a");   /* already in area.sym; shim is for standalone
                                     objcmp only */

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_58;
extern int _AREA_4a;
extern unsigned char gScript_930__02009730[];

union Pun { int i; short h; };
struct SPun { short h; };
struct IPun { int i; };

struct Actor {
    unsigned char pad00[0x18];
    int f18;
    unsigned char pad1c[0x23 - 0x1c];
    unsigned char f23;
};

extern int __GetFlag(int id);

/* Every __GetFlag argument is pinned to r0: see header note. */
#define GF(v) (g0 = (v), __GetFlag(g0))
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8091ff0(int a);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_930_2008870(void);
extern void OvlFunc_930_20081ec(void);
extern void OvlFunc_930_20090b8(void);
extern void OvlFunc_930_2009144(void);
extern void OvlFunc_930_2008b2c(void);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

int OvlFunc_930_20091b0(void)
{
    unsigned char *iw;
    unsigned int base;
    unsigned int gs;
    unsigned int p;
    unsigned int off;
    int area;
    int sub;
    int h;
    int m;
    int n;
    int u;
    int w;
    register int g0 __asm__("r0");

    iw = iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    ((union Pun *)(iw + off))->i = 0x80 << 1;
    gs = (unsigned int)&gState;
    area = *(short *)((char *)gs + off);
    if (area == (int)(&_AREA_58)) {
        __Func_8091ff0(0xa9);
        __MapActor_SetAnim(0xb, 5);
        __MapActor_SetAnim(0xc, 5);
        __MapActor_SetAnim(0xe, 2);
        u = 0x15;
        w = 0x49;
        __Func_8010704(0x15, 9, 1, 1, u, w);
        OvlFunc_930_2008870();
        if (__GetFlag(0x8b2)) {
            { PIN3; q0 = 0xd; q1 = 0x88 << 16; q2 = 0x80 << 17;
              __MapActor_SetPos(q0, q1, q2); }
            __Func_8092adc(0xd, 0, 0);
        }
        off = 0xe1;
        off <<= 1;
        p = gs + off;
        off = 0;
        sub = *(short *)((char *)p + off);
        if (sub == 2) {
            __ClearFlag(0x12f);
        } else if (sub == 3) {
            if (!__GetFlag(0x109))
                OvlFunc_930_20081ec();
        }
    } else if (area == (int)(&_AREA_4a)) {
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
        __MapActor_GetActor(0xe)->f23 |= 2;
        if (__GetFlag(0x80 << 2)) {
            __MapActor_SetAnim(0xe, 5);
            OvlFunc_930_20090b8();
        }
        if (__GetFlag(0x201)) {
            __MapActor_SetAnim(0xf, 4);
            OvlFunc_930_2009144();
        }
        off = 0xe1;
        off <<= 1;
        p = gs + off;
        h = *(unsigned short *)p;
        if ((unsigned short)(h - 4) <= 1)
            __ClearFlag(0x12f);
        if (!__GetFlag(0x89a) && !__GetFlag(0x895) && !__GetFlag(0x8b2))
            __MapActor_SetPos(0xa, 0, 0);
        if (!GF(0x8b2) && __GetFlag(0x895)) {
            base = (unsigned int)&gState;
            off = 0xe1;
            off <<= 1;
            base += off;
            off = 0;
            if (*(short *)((char *)base + off) == 2) {
                __MapActor_SetPos(0xb, 0, 0);
                __SetFlag(0x8b2);
                __SetFlag(0x8b3);
                __MapActor_SetPos(0xa, 0, 0);
            }
        }
        if (GF(0x8b2)) {
            m = 1;
            __CopyMapTiles(0x36, 0x15, 0x35, 0x15, m, 2);
            n = 0x11;
            __Func_8010704(0x12, 0x14, 1, 3, n, 0x15);
            __CopyMapTiles(0x2c, 0x12, 0x2b, 0x11, m, m);
            __Func_8010704(8, 0x11, 1, 1, 7, n);
        }
        if (__GetFlag(0x895) && !__GetFlag(0x8b2)) {
            __MapActor_SetPos(0xc, 0, 0);
            __MapActor_SetPos(0xd, 0, 0);
            { PIN3; q0 = 8; q1 = 0xc0 << 16; q2 = 0x84 << 17;
              __MapActor_SetPos(q0, q1, q2); }
            { PIN3; q0 = 9; q1 = 0xa4 << 16; q2 = 0x8c << 17;
              __MapActor_SetPos(q0, q1, q2); }
            { PIN3; q0 = 0xa; q1 = 0xb8 << 16; q2 = 0x98 << 17;
              __MapActor_SetPos(q0, q1, q2); }
            { PIN3; q0 = 8; q1 = 0xa0 << 7; q2 = 0;
              __Func_8092adc(q0, q1, q2); }
            { PIN3; q0 = 0xa; q1 = 0xb0 << 8; q2 = 0;
              __Func_8092adc(q0, q1, q2); }
            __MapActor_SetBehavior(9, gScript_930__02009730);
            __MapActor_GetActor(9)->f18 = 0xffff0000;
        }
        if (!GF(0x8b2)) {
            { PIN3; q0 = 9; q1 = 0xa4 << 16; q2 = 0x8c << 17;
              __MapActor_SetPos(q0, q1, q2); }
            __MapActor_SetBehavior(9, gScript_930__02009730);
            __MapActor_GetActor(9)->f18 = 0xffff0000;
        }
        base = (unsigned int)&gState;
        off = 0xe1;
        off <<= 1;
        base += off;
        off = 0;
        if (*(short *)((char *)base + off) == 5 && !__GetFlag(0x8b1)
            && !__GetFlag(0x109) && !__GetFlag(0x8b2))
            OvlFunc_930_2008b2c();
    }
    return 0;
}
