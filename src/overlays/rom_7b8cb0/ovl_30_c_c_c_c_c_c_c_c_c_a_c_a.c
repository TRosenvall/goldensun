/* OvlFunc_931_2008524  --  0x02008524, overlay rom_7b8cb0,
 * ovl_30_c_c_c_c_c_c_c_c_c_a_c_a.
 *
 *   OK OvlFunc_931_2008524 -- 384 bytes, 166 encodings and 26 relocations identical
 *   (objcmp, four passes)
 *
 * The map-exit cutscene: clear every actor's 0x55 byte, look the exit number up
 * in .L1e70, play the door animation for it, then slide the party out and fade.
 *
 * WRITTEN FROM ITS NEAR-TWIN AND FOUR EDITS.  src/overlays/rom_7a7298/
 * ovl_30_c_c_c_c_c_a_a_a_a_b.c (OvlFunc_921_20086c0) is the same scene with a
 * different table shape, and it supplies verbatim: `base = iwram_3001ebc;`
 * before __CutsceneStart, the `for (i = 8; i <= 0x41; i++)` sweep with an
 * `unsigned int` counter (the ROM's `bls`), `t = *(unsigned short *)(base +
 * 0x16c); n = (short)(t - K);`, and the named-constant idiom `k1 = 0x80 << 8;
 * k2 = 0x80 << 7;` for __MapActor_SetSpeed.  Transcribed straight across, the
 * first candidate was 166 encodings against 166 with only 49 differing, and the
 * whole remaining distance was four independent, already-recorded levers:
 *
 *   lever                                                      differing after
 *   ---------------------------------------------------------  ---------------
 *   (first candidate)                                                 49
 *   + `int e1 = 1, f1 = 2;` for __CopyMapTiles' two STACK args        --
 *   + `__MapActor_GetActor(0)[0x55] = 0;` NOT a named pointer         --
 *   + the n==1 test written `else if (n != 1)` so the ELSE arm
 *     is the fall-through                                             --
 *   (all three together)                                               9
 *   + k1/k2 named and hoisted above the if/else                        6
 *   + k3 = -4 named and hoisted with them                              0
 *
 * THE INTERLEAVED TABLE IS ONE STRUCT, NOT TWO ARRAYS.  The twin declares
 * `void *L3190[]` and `short L31a8[][2]` separately; .L1e70 packs them, and the
 * ROM's `ldr r0,[r5,r6]` at n*8, `ldrsh r1,[r5,r3]` at n*8+4 and `ldrsh r7,
 * [r3,#2]` at n*8+6 read as `struct T { void *f0; short f4; short f6; }`.
 * Thumb-1 `ldrsh` has no immediate-offset form at all, so the second short
 * comes out as base+n*8+4 with a register offset of 2 -- that is gcc's own
 * output for `L1e70[n].f6`, not a transcription artefact.
 *
 * ------------------------------------------------------------------- NEW ----
 * A NEGATED CONSTANT ARGUMENT IS AN ORDERING RESIDUE OF THE SAME FAMILY AS A
 * POOLED ONE, AND HOISTING ITS LOCAL FIXES IT WHERE A PIN WOULD ALSO WORK.
 *
 * The last 6 encodings were two `__Func_8092208(0, 2, -4)` / `__Func_809228c(0,
 * 0, -4)` sites where the ROM issues `mov r2,#4 / mov r0,#0 / mov r1,#2 /
 * neg r2,r2` -- the `neg` LAST, immediately before the `bl` -- and gcc issues it
 * second, right after the `mov`.  Two spellings are exact and byte-identical to
 * each other:
 *
 *   { PIN3; q0 = 0; q1 = 2; q2 = -4; __Func_8092208(q0, q1, q2); }   0
 *   `int k3 = -4;` hoisted beside k1/k2, `__Func_8092208(0, 2, k3);` 0  <- ships
 *
 * The named-and-hoisted local ships because it costs no fakematch.txt row, and
 * because it is the same device the twin already uses for k1/k2/k3/k4/k5 --
 * which is now readable as an ORDERING device, not merely a constant-CSE one.
 * The uniform-ascending pin fill is confirmed correct at these two sites too.
 *
 * MINIMISED.  Every hoisted local was dropped and retested:
 *   k1/k2 dropped (constants inline)                          3 differing
 *   e1/f1 dropped (literals 1, 2 at the n==3 CopyMapTiles)     3 differing
 *   k3 dropped                                                6 differing
 *   `s = 2` for the n==1 CopyMapTiles stack pair              INERT -> dropped
 *   `t` folded into the `n =` expression                      LONGER by 4 bytes
 * So the shipping set is k1, k2, k3, e1, f1 and `t`, and nothing else.
 *
 * `base` IS USED AT TWO OF THE THREE iwram_3001ebc SITES AND THE LITERAL AT THE
 * THIRD.  The ROM caches the pointer in r9 for the `+0x16c` read at the top and
 * for the `ldrsh` at the bottom, but RELOADS `ldr r3,=iwram_3001ebc / ldr r3,
 * [r3]` for the `*(int *)(... + 0x1c0) = 0x100` store in between.  Writing the
 * global directly at that one site is what reproduces it; `base` there would
 * reuse r9 and lose two instructions.
 *
 * NO PINS SHIP -> no fakematch.txt entry.
 *
 * ---------------------------------------------------------------- LANDING --
 *   tools/asmfacts.py -> WHOLE  convert directly.  ONE function, no data, no
 *   split, no hand split, no linker edit.
 *   tryc.makefile_flags('overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_a')
 *     = set() -- tree default; no Makefile rule mentions rom_7b8cb0 at all, so
 *     the generic `asm/%.o: src/%.c` fires and there is no wildcard hazard.
 *   The .ld line stays VERBATIM on the asm/ path:
 *       overlays/rom_7b8cb0/overlay.ld:35
 *           asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_a_c_a.o(.text)
 *     and it is the object's ONLY appearance in that .ld.
 *   `.L1e70` is referenced here and defined in another object; it is ALREADY
 *   `.global .L1e70` at
 *   asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c.s:80 (label at
 *   :113, an `.incbin` of overlays/rom_7b8cb0/orig.bin).  Grepped -- no export
 *   to add.  Every other callee is a main-ROM import resolved by the overlay's
 *   own thunk table (__CopyMapTiles at 0x02008e68, __Func_8091e9c at
 *   0x02008f90 in overlay.map).
 *
 * -- worked in scratch_elev/b256/small
 */
struct T {
    void *f0;
    short f4;
    short f6;
};

extern unsigned char *iwram_3001ebc;
extern struct T L1e70[] __asm__(".L1e70");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010560(void *p, int a, int b);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092b08(int slot, int a);
extern void __Func_809228c(int slot, int a, int b);
extern void __Func_8092208(int slot, int a, int b);
extern void __Func_8091e9c(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_931_2008524(void)
{
    unsigned char *base;
    unsigned char *a;
    unsigned int i;
    int n, t, x, y, k1, k2, k3;

    base = iwram_3001ebc;
    __CutsceneStart();
    for (i = 8; i <= 0x41; i++) {
        a = __MapActor_GetActor(i);
        if (a != 0)
            a[0x55] = 0;
    }
    t = *(unsigned short *)(base + 0x16c);
    n = (short)(t - 2);
    k1 = 0x80 << 8;
    k2 = 0x80 << 7;
    k3 = -4;
    x = L1e70[n].f4;
    y = L1e70[n].f6;
    if (n == 1) {
        __PlaySound(0xbc);
        __CopyMapTiles(0x2a, 0x21, x, y, 2, 2);
        __CopyMapTiles(0x2a, 0x23, x + 2, y, 2, 2);
        __CutsceneWait(4);
        __CopyMapTiles(0x28, 0x21, x, y, 2, 2);
        __CopyMapTiles(0x28, 0x23, x + 2, y, 2, 2);
        __CutsceneWait(4);
    } else {
        __PlaySound(0x9e);
        if (n == 3) {
            int e1 = 1, f1 = 2;
            __CopyMapTiles(0x21, 0x2a, 8, 0x11, e1, f1);
        }
        __Func_8010560(L1e70[n].f0, x, y);
    }
    __MapActor_SetSpeed(0, k1, k2);
    *(int *)(iwram_3001ebc + 0x1c0) = 0x80 << 1;
    __MapActor_GetActor(0)[0x55] = 0;
    __MapActor_SetAnim(0, 2);
    if (n == 6) {
        __Func_8092208(0, 2, 0);
    } else if (n != 1) {
        __Func_8092208(0, 2, k3);
    } else {
        __Func_8092b08(0, 2);
        __Func_809228c(0, 0, k3);
    }
    __CutsceneWait(0xa);
    __Func_8091e9c(*(short *)(base + 0x16c));
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
