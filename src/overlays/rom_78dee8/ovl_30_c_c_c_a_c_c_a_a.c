/* OvlFunc_895_2008a24 (SetupArea10) -- whole-file conversion of
 * asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a_a.s (1 function, no data, so it
 * lands WHOLE and needs no new export).  EXACT: 760 bytes, 284 encodings and
 * 85 relocations identical.  NO SHIM, NO PIN, NO FLAG.
 *
 * Verify with (the .equ copy stands in for area.sym, which the standalone
 * candidate does not link against -- the shipped file has no .equ and objcmp
 * on IT necessarily reads "1 encoding + relocations differ", because the ROM's
 * `ldr r2,=0x10` assembles to a bare `.word 0x10` while ours is a `.word 0`
 * plus R_ARM_ABS32 _AREA_10 that the link fills with 0x10):
 *
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a_a.c \
 *     asm/overlays/rom_78dee8/ovl_30_c_c_c_a_c_c_a_a.s --func OvlFunc_895_2008a24
 *
 * `_AREA_10 = 0x10;` is ALREADY in area.sym (line 55) -- no new .sym row.
 *
 * WHAT CLOSED IT, both the same lever, both single-dropped from this file:
 *
 * 1. `cx`/`cy`/`cz` ASSIGNED AT THE TOP OF THE FUNCTION for the three IDENTICAL
 *    `0x80 << 9` arguments to __Func_8012330, 1 -> 231.  This REPLACES the
 *    three r0/r1/r2 register pins the park carried and the two-barrier
 *    alternative beside them: both were exact, this is exact with ZERO shims.
 *    Block-scoped locals immediately before the call are 231, a plain triple
 *    literal is 231, a one-member-struct carrier is 231, three block locals
 *    plus one barrier is 231, `volatile` locals are 227.  Only the
 *    top-of-function form works, and it works because cse2 cannot common three
 *    values whose sets are that far from the use.
 * 2. `pe3`/`pe7` ASSIGNED AT THE TOP for the two `__MapActor_SetPos(0xa, ...)`
 *    x arguments, 3 -> 1 (the residual 1 is the _AREA_10 pool word above).
 *    This is the residue the park was parked on: the ROM has
 *    `mov r0,#0xa / lsl r1,#19` and the literal form emits `lsl r1 / mov r0`.
 *    THE IN-FUNCTION CONTROL was already in the file and had been read past:
 *    the two slot-9 SetPos calls use `pb`/`pf`, top-of-function locals, and
 *    they ALREADY matched -- only the slot-0xa pair used inline literals.
 *    Ordering among the top-of-function assignments is inert (cx/cy/cz before
 *    or after pz/pb/pf: both 1; pe3/pe7 before or after cx/cy/cz: both 1).
 *
 * The park named the blocker as precompute_register_parameters (calls.c:850)
 * choosing an argument order sched2 then cannot undo, and that is right; what
 * it missed is that a top-of-function local takes the argument OUT of the
 * rtx_cost > 2 class entirely, because by the call its value is already a REG
 * and the `copy_to_mode_reg` at calls.c:854 is skipped.  Nine spellings had
 * been measured; none of them was this one.
 *
 * Levers 1-5 from the park's own list still stand and are still load-bearing:
 * separate `oneA`/`oneE` locals for the two 1s (47 -> 34), two separate SetPos
 * calls instead of a shared `px` so jump.c's cross_jump merges the tails
 * (28 -> 16), and `m`/`n` locals at the last __Func_8010704 (16 -> 13).
 *
 * SHIMS: register class 0, .equ class 0 (the .equ lives only in the
 * measurement copy, OvlFunc_895_2008a24.equ.c).
 */
extern short gState[];
extern int _AREA_10;
extern unsigned char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __StartEarthquake(void);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);
extern void __Func_8012330(int x, int y, int z);
extern void __Func_8091ff0(int a);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __MapActor_SetPos(int slot, int x, int z);

extern void OvlFunc_895_2008d1c(void);
extern void OvlFunc_895_2008f8c(void);
extern void OvlFunc_895_200961c(void);
extern void OvlFunc_895_20097c0(int a);

void OvlFunc_895_2008a24(void)
{
    unsigned char *b;
    unsigned char *gs;
    int oneA;
    int oneE;
    int three;
    int h;
    int m;
    int n;
    int pz;
    int pb;
    int pf;
    int cx;
    int cy;
    int cz;
    int pe3;
    int pe7;

    cx = 0x80 << 9;
    cy = 0x80 << 9;
    cz = 0x80 << 9;
    pz = 0x88 << 16;
    pb = 0xbb << 19;
    pf = 0xbf << 19;
    pe3 = 0xe3 << 19;
    pe7 = 0xe7 << 19;
    b = iwram_3001ebc;
    *(int *)(b + 0x1c0) = 0x204;
    if (__GetFlag(0x814) != 0) {
        __Func_8091ff0(0x8d);
        __Func_8012330(cx, cy, cz);
        __StartEarthquake();
    }
    gs = (unsigned char *)gState;
    switch (gState[0xe1]) {
    case 1:
    case 2:
        if (__GetFlag(0x81a) == 0)
            break;
        oneA = 1;
        __CopyMapTiles(1, 0x6d, 4, 0x51, oneA, oneA);
        __CopyMapTiles(0, 0x46, 0x1e, 0x2a, oneA, oneA);
        three = 3;
        __CopyMapTiles(0, 0x1d, 3, 1, three, 2);
        __Func_8010704(0, 0x1d, 3, 2, three, oneA);
        __Func_800fe9c();
        break;
    case 3:
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        break;
    case 8:
        gState[0x120] = (int)&_AREA_10;
        gState[0x121] = 8;
        if (__GetFlag(0x802) == 0)
            OvlFunc_895_2008d1c();
        break;
    case 11:
    case 12:
    case 13:
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xf), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x10), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x11), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x12), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0x13), 0);
        if (__GetFlag(0x804) == 0)
            OvlFunc_895_2008f8c();
        if (__GetFlag(0x303) != 0)
            __MapActor_SetPos(9, pb, pz);
        else if (__GetFlag(0x302) != 0)
            __MapActor_SetPos(9, pf, pz);
        if (__GetFlag(0x301) != 0)
            __MapActor_SetPos(0xa, pe3, pz);
        else if (__GetFlag(0xc0 << 2) != 0)
            __MapActor_SetPos(0xa, pe7, pz);
        break;
    case 14:
    case 15:
    case 16:
        __Actor_SetSpriteFlags(__MapActor_GetActor(9), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xb), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xc), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xd), 0);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xe), 0);
        if (__GetFlag(0x825) == 0)
            OvlFunc_895_200961c();
        OvlFunc_895_20097c0(1);
        __SetFlag(0x8d << 2);
        if (__GetFlag(0x821) == 0)
            break;
        oneE = 1;
        __CopyMapTiles(0, 0x47, 0x64, 0x47, oneE, oneE);
        __CopyMapTiles(0x7a, 0x14, 0x78, 0x1e, oneE, 2);
        m = 0x78;
        n = 0x1e;
        __Func_8010704(0x7a, 0x14, 1, 2, m, n);
        __Func_800fe9c();
        break;
    }
}
