/* OvlFunc_930_20091b0 -- **EXACT UNDER `-fno-strict-aliasing`**, 744 bytes, 302
 * encodings and 59 relocations identical, stable over three repeats.  288 instructions.
 * At the tree default it is 5 of 302.
 *
 * THIS IS NOT PARKED BECAUSE IT DOES NOT MATCH.  IT IS PARKED BECAUSE LANDING IT NEEDS
 * THREE SEPARATE DECISIONS, AND ONE OF THEM IS NOT MINE TO MAKE.
 *
 * AT THE TREE DEFAULT AND WITH THE TWO SHIMS BELOW IN PLACE IT IS **5 encodings of 302**
 * -- that is the number tools/parkcheck.py will confirm.  Without the shims it reads 7,
 * the two extra being the _AREA_58 and _AREA_4a pool words, since objcmp assembles the
 * candidate standalone and cannot see a linker-script definition.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7b7f1c/20091b0.c \
 *     asm/overlays/rom_7b7f1c/ovl_30_c_c_c_c_c.s
 * ONE function.  Note objcmp uses the DEFAULT flags, so it reports 5 -- the number above
 * the line.  The exact result needs the flag (see 1).
 *
 * ================================================================
 * THE THREE REQUIREMENTS
 * ================================================================
 *
 * 1. A PER-FILE `ALIAS_CFLAGS` ROW for asm/overlays/rom_7b7f1c/ovl_30_c_c_c_c_c.o.  The
 *    group already exists (Makefile:235).  This is the decision that is not mine: a flag
 *    row is a claim about how the ORIGINAL was compiled, and this project has left
 *    -fcall-saved-r4 for OvlFunc_970_2008f80 and ALIAS_CFLAGS for OvlFunc_882_200c41c
 *    open for the same reason across several batches.
 *
 *    THE EVIDENCE IS NARROW AND CLEAN.  At the default the single defect is one misplaced
 *    instruction: `ldr r5, =gState` scheduled FOUR SLOTS EARLY, before the `str` to
 *    iwram, instead of immediately before its `ldrsh`.  No source spelling moved it --
 *    an `iw` local, dropping the `gs` variable, and an `__asm__ volatile("")` barrier all
 *    measured 9 of 302, worse.  CAUTION recorded by the file-neighbour
 *    src/overlays/rom_7b7f1c/ovl_30_c_c_a_c_c_c_c_a.c: this overlay has -O1 PATTERN rules
 *    nearby, so the row must be an EXACT-PATH row, not a pattern.
 *
 * 2. `_AREA_58 = 0x58;` ADDED TO area.sym.  It COMPLETES the file, and it is a HOLE:
 *    _AREA_57 is at area.sym:162 and _AREA_59 at :168, with 0x58 absent.  The value is
 *    read from gState+0x1C0 -- exactly the halfword area.sym documents -- and compared
 *    directly rather than passed to __GetFile, so the NAMESPACE IS CHECKED: this is the
 *    area space, not the file space.
 *
 *    IN-FUNCTION CONTROL: 0x58 and 0x4a are the only two `ldr rN, =<value <= 0xff>` in
 *    the function, while it emits `mov r0,#0xa9`, `mov r3,#0x15`, `mov r2,#0x49`,
 *    `mov r0,#0xb` and `mov r0,#0x80 / lsl #2` -- five constants under 0xff that gcc
 *    reproduces as plain literals.  SECOND CONTROL: _AREA_4a is already in area.sym and
 *    is referenced the same way by the file-neighbour ovl_30_c_c_c_c_b.c.
 *    MEASURED: with both symbol references, exact; with plain 0x58/0x4a literals, 732
 *    bytes / 298 encodings / 288 differing, because the two pool loads collapse to
 *    mov+lsl.
 *
 * 3. A TEXT/DATA REHOME.  datacheck.py reports .data; the blob is 1984 bytes across 14
 *    .incbin blocks, so rehoming is right rather than emitting from C.  All 12 labels are
 *    ALREADY .global, so split_s.py will not refuse on the export gate.  overlay.ld names
 *    this .o TWICE -- line 42 in .text and line 48 in .data, as the SECOND of two .data
 *    entries -- so the .data line must be repointed IN THE SAME SLOT so addresses do not
 *    move.  The only other section is /DISCARD/.
 *
 * ================================================================
 * THE LEVERS, minimal set re-derived after every structural change
 * ================================================================
 *
 * SEVEN PIN3 BLOCKS on the __MapActor_SetPos / __Func_8092adc sites with shifted
 * arguments, ascending whole-value fill.  A greedy per-site drop re-run on the minimised
 * file shows all seven cost 2-5 of 302.
 *
 * THREE r0 PINS ON `__GetFlag(0x8b2)` via `#define GF(v) (g0 = (v), __GetFlag(g0))` with
 * a function-scope `register int g0 __asm__("r0")`.  The ROM reloads 0x8b2 from the pool
 * at all six sites; gcc commons it into r5/r7.  THE GREEDY DROP IS THE POINT HERE: of
 * TWELVE GF sites only THREE are load-bearing -- all three on 0x8b2, and all three
 * FIRST-USE-PER-REGION exactly as the batch-281 donor records -- and the other nine,
 * including every singleton constant, were inert and stripped.  Removing all twelve: 190
 * of 302 and +8 bytes.
 *
 * SEPARATE STACK-ARGUMENT LOCALS PER CALL SITE.  __Func_8010704(0x15,9,1,1,u,w) needs
 * u/w distinct from the m/n pair used later by the __CopyMapTiles block: reusing m/n
 * stretched their live ranges across calls, moved them to r6/r7, and forced
 * `push {r5,r6,r7,lr}` against the ROM's `push {r5,r6,lr}`.
 *
 * `int h` FROM `*(unsigned short *)p`, THEN `(unsigned short)(h - 4) <= 1`.  With
 * `unsigned short h` the subtraction happens in HImode and gcc builds +0xfffc from the
 * pool and derives the bound as 0xfffc + 4; an `int` carrier gives the ROM's
 * `sub r3,#4 / lsl r3,#16 / cmp r3, 0x80<<9`.  (Same shape as the -6/0xfffa finding in
 * src/overlays/rom_787e04/ovl_30_c_a_c_a_c_c_c_c_c_c_c_c_c_c_c_a_a_a_b.c: the MODE of the
 * subtraction is the axis.)
 *
 * `gs` LIVE FOR SITES 2-3, RE-MATERIALISED `&gState` FOR SITES 4-5, AND THE READOUT IS
 * IN THE INSTRUCTION: the ROM's `add r3, r5, r2` PRESERVES the base while
 * `ldr r3,=gState; add r3, r2` CONSUMES it.  So a live pointer local for the first pair
 * and a fresh `base = (unsigned int)&gState; base += off;` for the second.
 */
__asm__(".equ _AREA_58, 0x58");   /* NOT YET IN area.sym -- see requirement 2 */
__asm__(".equ _AREA_4a, 0x4a");   /* already in area.sym; shim is for standalone
                                     objcmp only */

typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_58;
extern int _AREA_4a;
extern unsigned char gScript_930__02009730[];

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
    *(int *)(iw + off) = 0x80 << 1;
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
